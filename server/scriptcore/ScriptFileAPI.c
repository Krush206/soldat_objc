#include "ScriptFileAPI.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>
#include <limits.h>
#include <errno.h>
#include <dirent.h>
#include <libgen.h>
#include <regex.h>

/* ------------------------------------------------------------------ */
/* Global configuration (normally supplied by the server)             */
/* ------------------------------------------------------------------ */
static const char *g_user_directory = "/opt/soldat/";   /* base folder of the server */
static const char *g_server_ini_name = "server.ini";
static const char *g_soldat_ini_name = "soldat.ini";

/* ------------------------------------------------------------------ */
/* Simple dynamic string helper                                        */
/* ------------------------------------------------------------------ */
typedef struct {
    char *data;
    size_t len;
    size_t cap;
} DynStr;

static void ds_init(DynStr *s) { s->data = NULL; s->len = 0; s->cap = 0; }
static void ds_free(DynStr *s) { free(s->data); s->data = NULL; s->len = s->cap = 0; }
static void ds_ensure(DynStr *s, size_t need) {
    if (s->len + need + 1 > s->cap) {
        size_t ncap = s->cap ? s->cap * 2 : 64;
        while (s->len + need + 1 > ncap) ncap *= 2;
        s->data = realloc(s->data, ncap);
        s->cap = ncap;
    }
}
static void ds_append(DynStr *s, const char *str, size_t n) {
    ds_ensure(s, n);
    memcpy(s->data + s->len, str, n);
    s->len += n;
    s->data[s->len] = '\0';
}
static void ds_append_cstr(DynStr *s, const char *str) {
    ds_append(s, str, strlen(str));
}
static void ds_set(DynStr *s, const char *str) {
    s->len = 0;
    ds_append_cstr(s, str);
}

/* ------------------------------------------------------------------ */
/* Opaque structures                                                    */
/* ------------------------------------------------------------------ */
struct Script {
    const char *Dir;   /* relative script directory, e.g. "scripts/myscript/" */
    int SandboxLevel;  /* 0 = none, 1 = server folder, 2 = script data folder */
    bool AllowIniEdit;
};

struct ScriptFileAPI {
    Script *script;
    struct ScriptFile *file;
};

struct ScriptFile {
    ScriptFileAPI *api;
};

struct IniFile {
    char *path;
    ScriptFileAPI *api;
    bool case_sensitive;
    bool strip_quotes;
    bool cache_updates;
};

struct StringList {
    ScriptFileAPI *api;
    char **items;
    size_t count;
    size_t capacity;
    bool sorted;
    int duplicates; /* 0=ignore,1=accept,2=error */
    void (*on_change)(void *sender);
    void (*on_changing)(void *sender);
};

struct MemoryStream {
    ScriptFileAPI *api;
    unsigned char *buffer;
    size_t size;
    size_t capacity;
    size_t position;
};

struct StringStream {
    char *data;
    size_t len;
    size_t capacity;
    size_t position;
};

/* ------------------------------------------------------------------ */
/* Utility functions                                                    */
/* ------------------------------------------------------------------ */
static char *normalize_path(const char *base, const char *rel) {
    DynStr ds; ds_init(&ds);
    ds_append_cstr(&ds, base);
    ds_append_cstr(&ds, rel);
    char *res = realpath(ds.data, NULL);
    ds_free(&ds);
    return res; /* caller must free */
}

static bool path_starts_with(const char *path, const char *prefix) {
    size_t pl = strlen(prefix);
    return strncmp(path, prefix, pl) == 0;
}

static char *make_relative(const char *base, const char *path) {
    /* Very small implementation: if path starts with base, return substring */
    if (path_starts_with(path, base)) {
        const char *rel = path + strlen(base);
        if (*rel == '/') ++rel;
        return strdup(rel);
    }
    return strdup(path);
}

/* Copy file implementation used by script_file_copy */
bool copy_file(const char *source, const char *destination, bool fail_if_exists) {
    FILE *src = fopen(source, "rb");
    if (!src) return false;
    if (!fail_if_exists) {
        FILE *dst = fopen(destination, "wb");
        if (!dst) { fclose(src); return false; }
        char buf[65536];
        size_t n;
        while ((n = fread(buf, 1, sizeof(buf), src)) > 0) {
            if (fwrite(buf, 1, n, dst) != n) { fclose(src); fclose(dst); return false; }
        }
        fclose(src);
        fclose(dst);
        return true;
    } else {
        if (access(destination, F_OK) == 0) { fclose(src); return false; }
        FILE *dst = fopen(destination, "wb");
        if (!dst) { fclose(src); return false; }
        char buf[65536];
        size_t n;
        while ((n = fread(buf, 1, sizeof(buf), src)) > 0) {
            if (fwrite(buf, 1, n, dst) != n) { fclose(src); fclose(dst); return false; }
        }
        fclose(src);
        fclose(dst);
        return true;
    }
}

/* ------------------------------------------------------------------ */
/* ScriptFileAPI implementation                                         */
/* ------------------------------------------------------------------ */
ScriptFileAPI *script_file_api_create(Script *script) {
    ScriptFileAPI *api = calloc(1, sizeof(*api));
    if (!api) return NULL;
    api->script = script;
    api->file = calloc(1, sizeof(*api->file));
    if (!api->file) { free(api); return NULL; }
    api->file->api = api;
    return api;
}

void script_file_api_destroy(ScriptFileAPI *api) {
    if (!api) return;
    free(api->file);
    free(api);
}

uint8_t script_file_api_get_sandbox_level(const ScriptFileAPI *api) {
    return (uint8_t)api->script->SandboxLevel;
}

bool script_file_api_get_allow_ini_edit(const ScriptFileAPI *api) {
    return api->script->AllowIniEdit;
}

const char *script_file_api_get_data_folder(const ScriptFileAPI *api) {
    static char buffer[PATH_MAX];
    snprintf(buffer, sizeof(buffer), "%s%sdata/", g_user_directory, api->script->Dir);
    return buffer;
}

/* Core access check – mirrors Pascal CheckAccess */
bool script_file_api_check_access(ScriptFileAPI *api, char **path) {
    if (!path || !*path) return false;
    char *p = *path;

    /* Resolve absolute path */
    char *expanded = NULL;
    if (p[0] == '/') {
        expanded = realpath(p, NULL);
    } else if (p[0] == '~') {
        const char *data = script_file_api_get_data_folder(api);
        DynStr ds; ds_init(&ds);
        ds_append_cstr(&ds, data);
        ds_append_cstr(&ds, p + 2); /* skip ~/ */
        expanded = realpath(ds.data, NULL);
        ds_free(&ds);
    } else {
        #ifdef _WIN32
        /* Windows drive letter detection omitted for brevity */
        #endif
        DynStr ds; ds_init(&ds);
        ds_append_cstr(&ds, g_user_directory);
        ds_append_cstr(&ds, p);
        expanded = realpath(ds.data, NULL);
        ds_free(&ds);
    }

    if (!expanded) return false;

    bool allowed = false;
    int level = api->script->SandboxLevel;
    const char *data_folder = script_file_api_get_data_folder(api);
    char *data_folder_abs = realpath(data_folder, NULL);

    switch (level) {
        case 0:
            allowed = true;
            break;
        case 1: {
            allowed = path_starts_with(expanded, g_user_directory);
            if (allowed && !api->script->AllowIniEdit) {
                char *rel = make_relative(g_user_directory, expanded);
                if (strcmp(rel, g_server_ini_name) == 0 || strcmp(rel, g_soldat_ini_name) == 0) {
                    allowed = false;
                }
                free(rel);
            }
            if (allowed) {
                /* forbid scripts/*/config.ini */
                regex_t regex;
                if (regcomp(&regex, "scripts[/\\\\][^/\\\\]+[/\\\\]config\\.ini", REG_EXTENDED|REG_NOSUB) == 0) {
                    char *rel = make_relative(g_user_directory, expanded);
                    if (regexec(&regex, rel, 0, NULL, 0) == 0) allowed = false;
                    free(rel);
                    regfree(&regex);
                }
            }
            break;
        }
        default: /* level 2 or higher */
            if (data_folder_abs) {
                allowed = path_starts_with(expanded, data_folder_abs);
            }
            break;
    }

    if (allowed) {
        /* Return relative path to hide server root */
        char *rel = make_relative(g_user_directory, expanded);
        free(*path);
        *path = rel;
    }

    free(expanded);
    if (data_folder_abs) free(data_folder_abs);
    return allowed;
}

ScriptFile *script_file_api_get_file(const ScriptFileAPI *api) {
    return api->file;
}

/* ------------------------------------------------------------------ */
/* ScriptFile methods                                                   */
/* ------------------------------------------------------------------ */
bool script_file_check_access(ScriptFile *file, const char *path) {
    char *mutable = strdup(path);
    bool ok = script_file_api_check_access(file->api, &mutable);
    free(mutable);
    return ok;
}

MemoryStream *script_file_create_file_stream(ScriptFile *file) {
    return memory_stream_create(file->api);
}

MemoryStream *script_file_create_file_stream_from_file(ScriptFile *file, const char *path) {
    char *mutable = strdup(path);
    if (!script_file_api_check_access(file->api, &mutable)) { free(mutable); return NULL; }
    MemoryStream *ms = memory_stream_create(file->api);
    if (ms) memory_stream_load_from_file(ms, mutable);
    free(mutable);
    return ms;
}

StringList *script_file_create_string_list(ScriptFile *file) {
    return string_list_create(file->api);
}

StringList *script_file_create_string_list_from_file(ScriptFile *file, const char *path) {
    char *mutable = strdup(path);
    if (!script_file_api_check_access(file->api, &mutable)) { free(mutable); return NULL; }
    StringList *sl = string_list_create(file->api);
    if (sl) string_list_load_from_file(sl, mutable);
    free(mutable);
    return sl;
}

IniFile *script_file_create_ini(ScriptFile *file, const char *path) {
    char *mutable = strdup(path);
    if (!script_file_api_check_access(file->api, &mutable)) { free(mutable); return NULL; }
    IniFile *ini = ini_file_create(mutable, file->api);
    free(mutable);
    return ini;
}

bool script_file_exists(ScriptFile *file, const char *path) {
    char *mutable = strdup(path);
    bool ok = script_file_api_check_access(file->api, &mutable);
    bool exists = false;
    if (ok) {
        struct stat st;
        exists = (stat(mutable, &st) == 0);
    }
    free(mutable);
    return exists;
}

bool script_file_copy(ScriptFile *file, const char *source, const char *destination) {
    char *src = strdup(source);
    char *dst = strdup(destination);
    bool ok = script_file_api_check_access(file->api, &src) &&
              script_file_api_check_access(file->api, &dst);
    bool result = false;
    if (ok) result = copy_file(src, dst, false);
    free(src);
    free(dst);
    return result;
}

bool script_file_move(ScriptFile *file, const char *source, const char *destination) {
    char *src = strdup(source);
    char *dst = strdup(destination);
    bool ok = script_file_api_check_access(file->api, &src) &&
              script_file_api_check_access(file->api, &dst);
    bool result = false;
    if (ok) result = rename(src, dst) == 0;
    free(src);
    free(dst);
    return result;
}

bool script_file_delete(ScriptFile *file, const char *path) {
    char *mutable = strdup(path);
    bool ok = script_file_api_check_access(file->api, &mutable);
    bool result = false;
    if (ok) result = unlink(mutable) == 0;
    free(mutable);
    return result;
}

/* ------------------------------------------------------------------ */
/* IniFile implementation                                               */
/* ------------------------------------------------------------------ */
IniFile *ini_file_create(const char *path, ScriptFileAPI *api) {
    IniFile *ini = calloc(1, sizeof(*ini));
    if (!ini) return NULL;
    ini->path = strdup(path);
    ini->api = api;
    ini->case_sensitive = false;
    ini->strip_quotes = false;
    ini->cache_updates = false;
    return ini;
}

void ini_file_destroy(IniFile *ini) {
    if (!ini) return;
    free(ini->path);
    free(ini);
}

void ini_file_rename(IniFile *ini, const char *new_path, bool reload) {
    (void)reload;
    char *mutable = strdup(new_path);
    if (script_file_api_check_access(ini->api, &mutable)) {
        free(ini->path);
        ini->path = mutable;
    } else {
        free(mutable);
    }
}

const char *ini_file_get_file_name(const IniFile *ini) { return ini->path; }
bool ini_file_get_case_sensitive(const IniFile *ini) { return ini->case_sensitive; }
void ini_file_set_case_sensitive(IniFile *ini, bool v) { ini->case_sensitive = v; }
bool ini_file_get_strip_quotes(const IniFile *ini) { return ini->strip_quotes; }
void ini_file_set_strip_quotes(IniFile *ini, bool v) { ini->strip_quotes = v; }
bool ini_file_get_cache_updates(const IniFile *ini) { return ini->cache_updates; }
void ini_file_set_cache_updates(IniFile *ini, bool v) { ini->cache_updates = v; }

/* ------------------------------------------------------------------ */
/* StringList implementation                                            */
/* ------------------------------------------------------------------ */
StringList *string_list_create(ScriptFileAPI *api) {
    StringList *sl = calloc(1, sizeof(*sl));
    if (!sl) return NULL;
    sl->api = api;
    sl->capacity = 8;
    sl->items = calloc(sl->capacity, sizeof(char*));
    return sl;
}

void string_list_destroy(StringList *sl) {
    if (!sl) return;
    for (size_t i = 0; i < sl->count; ++i) free(sl->items[i]);
    free(sl->items);
    free(sl);
}

static void sl_ensure(StringList *sl) {
    if (sl->count >= sl->capacity) {
        sl->capacity *= 2;
        sl->items = realloc(sl->items, sl->capacity * sizeof(char*));
    }
}

int string_list_add(StringList *sl, const char *s) {
    sl_ensure(sl);
    sl->items[sl->count] = strdup(s);
    return (int)sl->count++;
}
void string_list_append(StringList *sl, const char *s) { string_list_add(sl, s); }
void string_list_add_strings(StringList *sl, StringList *other) {
    for (size_t i = 0; i < other->count; ++i) string_list_add(sl, other->items[i]);
}
void string_list_clear(StringList *sl) {
    for (size_t i = 0; i < sl->count; ++i) free(sl->items[i]);
    sl->count = 0;
}
void string_list_delete(StringList *sl, int index) {
    if (index < 0 || (size_t)index >= sl->count) return;
    free(sl->items[index]);
    memmove(&sl->items[index], &sl->items[index+1], (sl->count - index - 1) * sizeof(char*));
    sl->count--;
}
int string_list_index_of(StringList *sl, const char *s) {
    for (size_t i = 0; i < sl->count; ++i)
        if (strcmp(sl->items[i], s) == 0) return (int)i;
    return -1;
}
void string_list_insert(StringList *sl, int index, const char *s) {
    if (index < 0) index = 0;
    if ((size_t)index > sl->count) index = sl->count;
    sl_ensure(sl);
    memmove(&sl->items[index+1], &sl->items[index], (sl->count - index) * sizeof(char*));
    sl->items[index] = strdup(s);
    sl->count++;
}
void string_list_load_from_file(StringList *sl, const char *file_name) {
    FILE *f = fopen(file_name, "r");
    if (!f) return;
    char line[4096];
    while (fgets(line, sizeof(line), f)) {
        line[strcspn(line, "\r\n")] = 0;
        string_list_add(sl, line);
    }
    fclose(f);
}
void string_list_save_to_file(StringList *sl, const char *file_name) {
    FILE *f = fopen(file_name, "w");
    if (!f) return;
    for (size_t i = 0; i < sl->count; ++i) {
        fprintf(f, "%s\n", sl->items[i]);
    }
    fclose(f);
}
void string_list_begin_update(StringList *sl) { (void)sl; }
void string_list_end_update(StringList *sl) { (void)sl; }
bool string_list_equals(StringList *sl, StringList *other) {
    if (sl->count != other->count) return false;
    for (size_t i = 0; i < sl->count; ++i)
        if (strcmp(sl->items[i], other->items[i]) != 0) return false;
    return true;
}
void string_list_exchange(StringList *sl, int i1, int i2) {
    if (i1<0||i2<0||(size_t)i1>=sl->count||(size_t)i2>=sl->count) return;
    char *tmp = sl->items[i1];
    sl->items[i1] = sl->items[i2];
    sl->items[i2] = tmp;
}
int string_list_index_of_name(StringList *sl, const char *name) {
    for (size_t i = 0; i < sl->count; ++i) {
        char *eq = strchr(sl->items[i], '=');
        if (eq && strncmp(sl->items[i], name, eq - sl->items[i]) == 0) return (int)i;
    }
    return -1;
}
void string_list_load_from_stream(StringList *sl, MemoryStream *stream) {
    /* simple: treat stream as text lines */
    size_t pos = stream->position;
    char *buf = malloc(stream->size - pos + 1);
    memcpy(buf, stream->buffer + pos, stream->size - pos);
    buf[stream->size - pos] = 0;
    char *line = strtok(buf, "\n");
    while (line) {
        line[strcspn(line, "\r")] = 0;
        string_list_add(sl, line);
        line = strtok(NULL, "\n");
    }
    free(buf);
}
void string_list_move(StringList *sl, int cur, int newi) {
    if (cur<0||newi<0||(size_t)cur>=sl->count||(size_t)newi>=sl->count) return;
    char *tmp = sl->items[cur];
    if (cur < newi) memmove(&sl->items[cur], &sl->items[cur+1], (newi-cur)*sizeof(char*));
    else memmove(&sl->items[newi+1], &sl->items[newi], (cur-newi)*sizeof(char*));
    sl->items[newi] = tmp;
}
void string_list_save_to_stream(StringList *sl, MemoryStream *stream) {
    for (size_t i = 0; i < sl->count; ++i) {
        memory_stream_write(stream, sl->items[i], strlen(sl->items[i]));
        memory_stream_write(stream, "\n", 1);
    }
}
void string_list_set_text(StringList *sl, const char *text) {
    string_list_clear(sl);
    char *copy = strdup(text);
    char *line = strtok(copy, "\n");
    while (line) {
        line[strcspn(line, "\r")] = 0;
        string_list_add(sl, line);
        line = strtok(NULL, "\n");
    }
    free(copy);
}
int string_list_add_object(StringList *sl, const char *s, void *obj) { (void)obj; return string_list_add(sl, s); }
const char *string_list_get_text(StringList *sl) {
    size_t total = 0;
    for (size_t i = 0; i < sl->count; ++i) total += strlen(sl->items[i]) + 1;
    char *buf = malloc(total + 1);
    char *p = buf;
    for (size_t i = 0; i < sl->count; ++i) {
        strcpy(p, sl->items[i]); p += strlen(sl->items[i]);
        *p++ = '\n';
    }
    *p = 0;
    return buf; /* caller must free */
}
int string_list_index_of_object(StringList *sl, void *obj) { (void)obj; return -1; }
void string_list_insert_object(StringList *sl, int index, const char *s, void *obj) { (void)obj; string_list_insert(sl, index, s); }
bool string_list_find(StringList *sl, const char *s, int *index) {
    int idx = string_list_index_of(sl, s);
    if (idx >= 0) { *index = idx; return true; }
    return false;
}
void string_list_sort(StringList *sl) {
    qsort(sl->items, sl->count, sizeof(char*), (int(*)(const void*,const void*))strcmp);
}
int string_list_get_count(const StringList *sl) { return (int)sl->count; }
const char *string_list_get_text_property(const StringList *sl) { return string_list_get_text((StringList*)sl); }
void string_list_set_text_property(StringList *sl, const char *text) { string_list_set_text(sl, text); }
const char *string_list_get_comma_text(const StringList *sl) {
    size_t total = 0;
    for (size_t i = 0; i < sl->count; ++i) total += strlen(sl->items[i]) + 1;
    char *buf = malloc(total + 1);
    char *p = buf;
    for (size_t i = 0; i < sl->count; ++i) {
        strcpy(p, sl->items[i]); p += strlen(sl->items[i]);
        if (i+1 < sl->count) *p++ = ',';
    }
    *p = 0;
    return buf;
}
void string_list_set_comma_text(StringList *sl, const char *comma) {
    string_list_clear(sl);
    char *copy = strdup(comma);
    char *tok = strtok(copy, ",");
    while (tok) { string_list_add(sl, tok); tok = strtok(NULL, ","); }
    free(copy);
}
const char *string_list_get_string(const StringList *sl, int index) {
    if (index < 0 || (size_t)index >= sl->count) return NULL;
    return sl->items[index];
}
void string_list_set_string(StringList *sl, int index, const char *value) {
    if (index < 0 || (size_t)index >= sl->count) return;
    free(sl->items[index]);
    sl->items[index] = strdup(value);
}
void *string_list_get_object(const StringList *sl, int index) { (void)sl; (void)index; return NULL; }
void string_list_set_object(StringList *sl, int index, void *obj) { (void)sl; (void)index; (void)obj; }
const char *string_list_get_name(const StringList *sl, int index) {
    const char *s = string_list_get_string(sl, index);
    if (!s) return NULL;
    static char name[256];
    const char *eq = strchr(s, '=');
    if (eq) {
        size_t n = eq - s;
        if (n >= sizeof(name)) n = sizeof(name)-1;
        memcpy(name, s, n);
        name[n] = 0;
        return name;
    }
    return s;
}
const char *string_list_get_value(const StringList *sl, const char *key) {
    for (size_t i = 0; i < sl->count; ++i) {
        const char *s = sl->items[i];
        const char *eq = strchr(s, '=');
        if (eq && strncmp(s, key, eq - s) == 0 && eq[1]) return eq+1;
    }
    return "";
}
void string_list_set_value(StringList *sl, const char *key, const char *value) {
    int idx = string_list_index_of_name(sl, key);
    char buf[1024];
    snprintf(buf, sizeof(buf), "%s=%s", key, value);
    if (idx >= 0) string_list_set_string(sl, idx, buf);
    else string_list_add(sl, buf);
}
int string_list_get_duplicates(const StringList *sl) { return sl->duplicates; }
void string_list_set_duplicates(StringList *sl, int dup) { sl->duplicates = dup; }
bool string_list_get_sorted(const StringList *sl) { return sl->sorted; }
void string_list_set_sorted(StringList *sl, bool sorted) { sl->sorted = sorted; if (sorted) string_list_sort((StringList*)sl); }

/* ------------------------------------------------------------------ */
/* MemoryStream implementation                                          */
/* ------------------------------------------------------------------ */
MemoryStream *memory_stream_create(ScriptFileAPI *api) {
    MemoryStream *ms = calloc(1, sizeof(*ms));
    if (!ms) return NULL;
    ms->api = api;
    ms->capacity = 4096;
    ms->buffer = malloc(ms->capacity);
    return ms;
}
void memory_stream_destroy(MemoryStream *ms) {
    if (!ms) return;
    free(ms->buffer);
    free(ms);
}
void memory_stream_clear(MemoryStream *ms) { ms->size = 0; ms->position = 0; }
void memory_stream_load_from_stream(MemoryStream *dest, MemoryStream *src) {
    memory_stream_write(dest, src->buffer, src->size);
}
void memory_stream_load_from_file(MemoryStream *ms, const char *file_name) {
    FILE *f = fopen(file_name, "rb");
    if (!f) return;
    fseek(f, 0, SEEK_END);
    long fsize = ftell(f);
    fseek(f, 0, SEEK_SET);
    if (fsize > 0) {
        if ((size_t)fsize > ms->capacity) {
            ms->capacity = fsize + 1;
            ms->buffer = realloc(ms->buffer, ms->capacity);
        }
        fread(ms->buffer, 1, fsize, f);
        ms->size = fsize;
        ms->position = 0;
    }
    fclose(f);
}
void memory_stream_save_to_stream(MemoryStream *src, MemoryStream *dest) {
    memory_stream_write(dest, src->buffer, src->size);
}
void memory_stream_save_to_file(MemoryStream *ms, const char *file_name) {
    FILE *f = fopen(file_name, "wb");
    if (!f) return;
    fwrite(ms->buffer, 1, ms->size, f);
    fclose(f);
}
void memory_stream_set_size(MemoryStream *ms, long new_size) {
    if (new_size < 0) return;
    if ((size_t)new_size > ms->capacity) {
        ms->capacity = new_size + 1;
        ms->buffer = realloc(ms->buffer, ms->capacity);
    }
    ms->size = new_size;
    if (ms->position > ms->size) ms->position = ms->size;
}
long memory_stream_read(MemoryStream *ms, void *buffer, long count) {
    size_t avail = ms->size - ms->position;
    size_t to_read = count < (long)avail ? count : avail;
    memcpy(buffer, ms->buffer + ms->position, to_read);
    ms->position += to_read;
    return (long)to_read;
}
long memory_stream_write(MemoryStream *ms, const void *buffer, long count) {
    if (ms->position + count > ms->capacity) {
        size_t ncap = ms->capacity * 2;
        while (ms->position + count > ncap) ncap *= 2;
        ms->buffer = realloc(ms->buffer, ncap);
        ms->capacity = ncap;
    }
    memcpy(ms->buffer + ms->position, buffer, count);
    ms->position += count;
    if (ms->position > ms->size) ms->size = ms->position;
    return count;
}
void memory_stream_read_buffer(MemoryStream *ms, void *buffer, long count) {
    memory_stream_read(ms, buffer, count);
}
void memory_stream_write_buffer(MemoryStream *ms, const void *buffer, long count) {
    memory_stream_write(ms, buffer, count);
}
int64_t memory_stream_get_position(const MemoryStream *ms) { return (int64_t)ms->position; }
void memory_stream_set_position(MemoryStream *ms, int64_t pos) { if (pos >= 0 && (size_t)pos <= ms->size) ms->position = (size_t)pos; }
int64_t memory_stream_get_size(const MemoryStream *ms) { return (int64_t)ms->size; }
void memory_stream_set_size_int64(MemoryStream *ms, int64_t sz) { memory_stream_set_size(ms, (long)sz); }

/* ------------------------------------------------------------------ */
/* StringStream implementation                                          */
/* ------------------------------------------------------------------ */
StringStream *string_stream_create(void) {
    StringStream *ss = calloc(1, sizeof(*ss));
    ss->capacity = 256;
    ss->data = malloc(ss->capacity);
    return ss;
}
void string_stream_destroy(StringStream *ss) { free(ss->data); free(ss); }
long string_stream_read(StringStream *ss, void *buffer, long count) {
    size_t avail = ss->len - ss->position;
    size_t to_read = count < (long)avail ? count : avail;
    memcpy(buffer, ss->data + ss->position, to_read);
    ss->position += to_read;
    return (long)to_read;
}
const char *string_stream_read_string(StringStream *ss, long count) {
    size_t avail = ss->len - ss->position;
    size_t to_read = count < (long)avail ? count : avail;
    char *buf = malloc(to_read + 1);
    memcpy(buf, ss->data + ss->position, to_read);
    buf[to_read] = 0;
    ss->position += to_read;
    return buf; /* caller frees */
}
int64_t string_stream_seek(StringStream *ss, int64_t offset, int origin) {
    int64_t newpos = 0;
    switch (origin) {
        case 0: newpos = offset; break;           // soBeginning
        case 1: newpos = ss->position + offset; break; // soCurrent
        case 2: newpos = ss->len + offset; break; // soEnd
    }
    if (newpos < 0) newpos = 0;
    if (newpos > (int64_t)ss->len) newpos = ss->len;
    ss->position = (size_t)newpos;
    return (int64_t)ss->position;
}
long string_stream_write(StringStream *ss, const void *buffer, long count) {
    if (ss->position + count > ss->capacity) {
        size_t ncap = ss->capacity * 2;
        while (ss->position + count > ncap) ncap *= 2;
        ss->data = realloc(ss->data, ncap);
        ss->capacity = ncap;
    }
    memcpy(ss->data + ss->position, buffer, count);
    ss->position += count;
    if (ss->position > ss->len) ss->len = ss->position;
    return count;
}
void string_stream_write_string(StringStream *ss, const char *str) {
    string_stream_write(ss, str, strlen(str));
}
const char *string_stream_get_data_string(const StringStream *ss) { return ss->data; }

/* ------------------------------------------------------------------ */
/* End of file                                                          */
/* ------------------------------------------------------------------ */