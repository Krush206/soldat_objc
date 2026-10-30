/* SysUtils — C implementation of Pascal SysUtils/StrUtils/Math functionality
 * for OpenSoldat. Plain C functions, no Foundation. */

#include "SysUtils.h"
#include <stdarg.h>
#include <math.h>
#include <time.h>
#include <errno.h>

/* ------------------------------------------------------------------ */
/* String helpers                                                      */
/* ------------------------------------------------------------------ */

static char *gc_strdup(const char *s) {
    if (!s) return NULL;
    size_t len = strlen(s);
    char *p = malloc(len + 1);
    if (p) memcpy(p, s, len + 1);
    return p;
}

/* Copy(Source, Start, Length) — Pascal-style 1-based start. */
char *Copy(const char *source, int start, int length) {
    if (!source) return gc_strdup("");
    int len = (int)strlen(source);
    if (start < 1) start = 1;
    if (start > len + 1) return gc_strdup("");
    int idx = start - 1;
    if (length < 0) length = 0;
    if (idx + length > len) length = len - idx;
    char *result = malloc(length + 1);
    if (!result) return NULL;
    memcpy(result, source + idx, length);
    result[length] = '\0';
    return result;
}

/* Pos(SubStr, S) — returns 1-based position, 0 when not found. */
int Pos(const char *substr, const char *s) {
    if (!substr || !s) return 0;
    char *found = strstr(s, substr);
    return found ? (int)(found - s) + 1 : 0;
}

int PosEx(const char *substr, const char *s, int offset) {
    if (!substr || !s || offset < 1) return 0;
    int len = (int)strlen(s);
    if (offset > len) return 0;
    char *found = strstr(s + offset - 1, substr);
    return found ? (int)(found - s) + 1 : 0;
}

/* Delete(S, Index, Count) — in-place Pascal-style delete, 1-based. */
void Delete(char *s, int index, int count) {
    if (!s) return;
    int len = (int)strlen(s);
    if (index < 1) index = 1;
    if (index > len) return;
    int idx = index - 1;
    if (count < 0) count = 0;
    if (idx + count > len) count = len - idx;
    memmove(s + idx, s + idx + count, len - idx - count + 1);
}

/* Insert(Source, S, Index) — in-place Pascal-style insert, 1-based. */
void Insert(const char *source, char *s, int index) {
    if (!source || !s) return;
    int src_len = (int)strlen(source);
    int dst_len = (int)strlen(s);
    if (index < 1) index = 1;
    if (index > dst_len + 1) index = dst_len + 1;
    int idx = index - 1;
    memmove(s + idx + src_len, s + idx, dst_len - idx + 1);
    memcpy(s + idx, source, src_len);
}

char *UpperCase(const char *s) {
    if (!s) return gc_strdup("");
    int len = (int)strlen(s);
    char *result = malloc(len + 1);
    if (!result) return NULL;
    for (int i = 0; i < len; i++) {
        result[i] = (char)toupper((unsigned char)s[i]);
    }
    result[len] = '\0';
    return result;
}

char *LowerCase(const char *s) {
    if (!s) return gc_strdup("");
    int len = (int)strlen(s);
    char *result = malloc(len + 1);
    if (!result) return NULL;
    for (int i = 0; i < len; i++) {
        result[i] = (char)tolower((unsigned char)s[i]);
    }
    result[len] = '\0';
    return result;
}

char *Trim(const char *s) {
    if (!s) return gc_strdup("");
    const char *start = s;
    while (*start && isspace((unsigned char)*start)) start++;
    const char *end = s + strlen(s) - 1;
    while (end >= start && isspace((unsigned char)*end)) end--;
    int len = (int)(end - start + 1);
    if (len < 0) len = 0;
    char *result = malloc(len + 1);
    if (!result) return NULL;
    memcpy(result, start, len);
    result[len] = '\0';
    return result;
}

char *TrimLeft(const char *s) {
    if (!s) return gc_strdup("");
    const char *start = s;
    while (*start && isspace((unsigned char)*start)) start++;
    int len = (int)strlen(start);
    char *result = malloc(len + 1);
    if (!result) return NULL;
    memcpy(result, start, len + 1);
    return result;
}

char *TrimRight(const char *s) {
    if (!s) return gc_strdup("");
    int len = (int)strlen(s);
    while (len > 0 && isspace((unsigned char)s[len - 1])) len--;
    char *result = malloc(len + 1);
    if (!result) return NULL;
    memcpy(result, s, len);
    result[len] = '\0';
    return result;
}

/* StringReplace with flags: rfReplaceAll = 1, rfIgnoreCase = 2 */
char *StringReplace(const char *s, const char *oldstr, const char *newstr, int flags) {
    if (!s || !oldstr || !newstr || strlen(oldstr) == 0) return gc_strdup(s ? s : "");
    int ignore_case = flags & RF_IGNORECASE;
    int replace_all = flags & RF_REPLACEALL;

    size_t old_len = strlen(oldstr);
    size_t new_len = strlen(newstr);
    const char *p = s;
    size_t result_size = strlen(s) + 1;
    char *result = malloc(result_size);
    if (!result) return NULL;
    result[0] = '\0';

    while (*p) {
        const char *match = ignore_case ? strcasestr(p, oldstr) : strstr(p, oldstr);
        if (match) {
            size_t prefix_len = match - p;
            size_t needed = strlen(result) + prefix_len + new_len + 1;
            if (needed >= result_size) {
                result_size = needed * 2;
                char *new_result = realloc(result, result_size);
                if (!new_result) {
                    free(result);
                    return NULL;
                }
                result = new_result;
            }
            strncat(result, p, prefix_len);
            strcat(result, newstr);
            p = match + old_len;
            if (!replace_all) {
                strcat(result, p);
                break;
            }
        } else {
            size_t needed = strlen(result) + strlen(p) + 1;
            if (needed >= result_size) {
                result_size = needed * 2;
                char *new_result = realloc(result, result_size);
                if (!new_result) {
                    free(result);
                    return NULL;
                }
                result = new_result;
            }
            strcat(result, p);
            break;
        }
    }
    return result;
}

char *LeftStr(const char *s, int n) {
    if (!s) return gc_strdup("");
    if (n <= 0) return gc_strdup("");
    int len = (int)strlen(s);
    if (n > len) n = len;
    return Copy(s, 1, n);
}

char *RightStr(const char *s, int n) {
    if (!s) return gc_strdup("");
    if (n <= 0) return gc_strdup("");
    int len = (int)strlen(s);
    if (n > len) n = len;
    return Copy(s, len - n + 1, n);
}

char *MidStr(const char *s, int start, int count) {
    return Copy(s, start, count);
}

int LastDelimiter(const char *delimiters, const char *s) {
    if (!delimiters || !s) return 0;
    int len = (int)strlen(s);
    for (int i = len - 1; i >= 0; i--) {
        if (strchr(delimiters, s[i])) return i + 1; /* 1-based */
    }
    return 0;
}

char *strconcat(const char *a, const char *b) {
    if (!a) a = "";
    if (!b) b = "";
    size_t len_a = strlen(a);
    size_t len_b = strlen(b);
    char *result = malloc(len_a + len_b + 1);
    if (!result) return NULL;
    memcpy(result, a, len_a);
    memcpy(result + len_a, b, len_b + 1);
    return result;
}

char *strconcat3(const char *a, const char *b, const char *c) {
    if (!a) a = "";
    if (!b) b = "";
    if (!c) c = "";
    size_t len_a = strlen(a);
    size_t len_b = strlen(b);
    size_t len_c = strlen(c);
    char *result = malloc(len_a + len_b + len_c + 1);
    if (!result) return NULL;
    char *p = result;
    memcpy(p, a, len_a); p += len_a;
    memcpy(p, b, len_b); p += len_b;
    memcpy(p, c, len_c + 1);
    return result;
}

char *strdup2(const char *s) {
    return gc_strdup(s);
}

char *Format(const char *fmt, ...) {
    if (!fmt) return gc_strdup("");
    va_list args;
    va_start(args, fmt);
    va_list args_copy;
    va_copy(args_copy, args);
    int needed = vsnprintf(NULL, 0, fmt, args_copy);
    va_end(args_copy);
    if (needed < 0) {
        va_end(args);
        return gc_strdup("");
    }
    char *result = malloc(needed + 1);
    if (!result) {
        va_end(args);
        return NULL;
    }
    vsnprintf(result, needed + 1, fmt, args);
    va_end(args);
    return result;
}

char *IntToStr(int value) {
    char buf[32];
    snprintf(buf, sizeof(buf), "%d", value);
    return gc_strdup(buf);
}

char *IntToStr64(int64_t value) {
    char buf[32];
    snprintf(buf, sizeof(buf), "%lld", (long long)value);
    return gc_strdup(buf);
}

char *IntToHex(unsigned int value, int digits) {
    char buf[32];
    if (digits <= 0) digits = 1;
    if (digits > 8) digits = 8;
    snprintf(buf, sizeof(buf), "%0*X", digits, value);
    return gc_strdup(buf);
}

char *IntToHex64(uint64_t value, int digits) {
    char buf[32];
    if (digits <= 0) digits = 1;
    if (digits > 16) digits = 16;
    snprintf(buf, sizeof(buf), "%0*llX", digits, (unsigned long long)value);
    return gc_strdup(buf);
}

char *BoolToStr(int value, const char *trueS, const char *falseS) {
    return gc_strdup(value ? (trueS ? trueS : "True") : (falseS ? falseS : "False"));
}

int StrToInt(const char *s) {
    if (!s) return 0;
    return (int)strtol(s, NULL, 10);
}

int StrToIntDef(const char *s, int def) {
    if (!s) return def;
    char *endptr;
    long val = strtol(s, &endptr, 10);
    if (endptr == s || *endptr != '\0') return def;
    return (int)val;
}

double StrToFloat(const char *s) {
    if (!s) return 0.0;
    return strtod(s, NULL);
}

int64_t StrToInt64Def(const char *s, int64_t def) {
    if (!s) return def;
    char *endptr;
    int64_t val = strtoll(s, &endptr, 10);
    if (endptr == s || *endptr != '\0') return def;
    return val;
}

unsigned int StrToHexDef(const char *s, unsigned int def) {
    if (!s) return def;
    char *endptr;
    unsigned long val = strtoul(s, &endptr, 16);
    if (endptr == s || *endptr != '\0') return def;
    return (unsigned int)val;
}

double StrToFloatDef(const char *s, double def) {
    if (!s) return def;
    char *endptr;
    double val = strtod(s, &endptr);
    if (endptr == s || *endptr != '\0') return def;
    return val;
}

float StrToFloatDefF(const char *s, float def) {
    if (!s) return def;
    char *endptr;
    float val = strtof(s, &endptr);
    if (endptr == s || *endptr != '\0') return def;
    return val;
}

int StrToBoolDef(const char *s, int def) {
    if (!s) return def;
    char *lower = LowerCase(s);
    int result = def;
    if (strcmp(lower, "true") == 0 || strcmp(lower, "yes") == 0 ||
        strcmp(lower, "y") == 0 || strcmp(lower, "1") == 0) {
        result = 1;
    } else if (strcmp(lower, "false") == 0 || strcmp(lower, "no") == 0 ||
               strcmp(lower, "n") == 0 || strcmp(lower, "0") == 0) {
        result = 0;
    }
    free(lower);
    return result;
}

char *FloatToStr(double value) {
    char buf[64];
    snprintf(buf, sizeof(buf), "%.15g", value);
    return gc_strdup(buf);
}

char *FloatToStrF(double value, const char *format) {
    if (!format) format = "0.##";
    char buf[128];
    /* Simplified FormatFloat subset: handle '0.##', '#,##0', '0.######' */
    if (strcmp(format, "0.##") == 0) {
        snprintf(buf, sizeof(buf), "%.2f", value);
        /* Trim trailing zeros after decimal */
        char *dot = strchr(buf, '.');
        if (dot) {
            char *end = buf + strlen(buf) - 1;
            while (end > dot && *end == '0') *end-- = '\0';
            if (end == dot) *end = '\0';
        }
    } else if (strcmp(format, "#,##0") == 0) {
        /* Thousands separator not fully implemented; just integer */
        snprintf(buf, sizeof(buf), "%.0f", value);
    } else if (strcmp(format, "0.######") == 0) {
        snprintf(buf, sizeof(buf), "%.6f", value);
        char *dot = strchr(buf, '.');
        if (dot) {
            char *end = buf + strlen(buf) - 1;
            while (end > dot && *end == '0') *end-- = '\0';
            if (end == dot) *end = '\0';
        }
    } else {
        /* Fallback */
        snprintf(buf, sizeof(buf), "%g", value);
    }
    return gc_strdup(buf);
}

int StartsWith(const char *s, const char *prefix) {
    if (!s || !prefix) return 0;
    size_t prefix_len = strlen(prefix);
    return strncmp(s, prefix, prefix_len) == 0;
}

int EndsWith(const char *s, const char *suffix) {
    if (!s || !suffix) return 0;
    size_t s_len = strlen(s);
    size_t suffix_len = strlen(suffix);
    if (suffix_len > s_len) return 0;
    return strcmp(s + s_len - suffix_len, suffix) == 0;
}

char **SplitString(const char *s, const char *delim, int *count) {
    if (!s || !delim || !count) {
        if (count) *count = 0;
        return NULL;
    }
    if (strlen(delim) == 0) {
        *count = 0;
        return NULL;
    }

    size_t delim_len = strlen(delim);
    char *copy = gc_strdup(s);
    if (!copy) { *count = 0; return NULL; }

    /* First pass: count */
    int c = 0;
    char *p = copy;
    while ((p = strstr(p, delim))) {
        c++;
        p += delim_len;
    }
    c++; /* last piece */

    char **result = malloc(c * sizeof(char *));
    if (!result) { free(copy); *count = 0; return NULL; }

    /* Second pass: split */
    int i = 0;
    p = copy;
    char *start = copy;
    while ((p = strstr(start, delim))) {
        size_t len = p - start;
        result[i] = malloc(len + 1);
        if (!result[i]) {
            for (int j = 0; j < i; j++) free(result[j]);
            free(result);
            free(copy);
            *count = 0;
            return NULL;
        }
        memcpy(result[i], start, len);
        result[i][len] = '\0';
        i++;
        start = p + delim_len;
    }
    /* Last piece */
    result[i] = gc_strdup(start);
    if (!result[i]) {
        for (int j = 0; j < i; j++) free(result[j]);
        free(result);
        free(copy);
        *count = 0;
        return NULL;
    }
    free(copy);
    *count = c;
    return result;
}

char *SplitFirst(const char *s, const char *delim, char **right) {
    if (!s || !delim || !right) {
        if (right) *right = NULL;
        return gc_strdup(s ? s : "");
    }
    char *found = strstr(s, delim);
    if (!found) {
        *right = gc_strdup("");
        return gc_strdup(s);
    }
    size_t left_len = found - s;
    char *left = malloc(left_len + 1);
    if (!left) { *right = NULL; return NULL; }
    memcpy(left, s, left_len);
    left[left_len] = '\0';
    *right = gc_strdup(found + strlen(delim));
    return left;
}

char *QuotedStr(const char *s) {
    if (!s) return gc_strdup("''");
    size_t len = strlen(s);
    /* Count single quotes to double them */
    size_t quote_count = 0;
    for (size_t i = 0; i < len; i++) if (s[i] == '\'') quote_count++;
    size_t result_len = 2 + len + quote_count; /* wrapping quotes + doubled quotes */
    char *result = malloc(result_len + 1);
    if (!result) return NULL;
    char *p = result;
    *p++ = '\'';
    for (size_t i = 0; i < len; i++) {
        *p++ = s[i];
        if (s[i] == '\'') *p++ = '\'';
    }
    *p++ = '\'';
    *p = '\0';
    return result;
}

/* ------------------------------------------------------------------ */
/* Math helpers (Pascal Math/System unit)                              */
/* ------------------------------------------------------------------ */

int Random(int range) {
    if (range <= 0) return 0;
    return rand() % range;
}

void Randomize(void) {
    srand((unsigned int)time(NULL) ^ (unsigned int)getpid());
}

void Sleep(int milliseconds) {
    if (milliseconds <= 0) return;
    struct timespec ts;
    ts.tv_sec = milliseconds / 1000;
    ts.tv_nsec = (milliseconds % 1000) * 1000000L;
    nanosleep(&ts, NULL);
}

/* ------------------------------------------------------------------ */
/* File system helpers                                                 */
/* ------------------------------------------------------------------ */

int FileExists(const char *filename) {
    if (!filename) return 0;
    struct stat st;
    return stat(filename, &st) == 0 && S_ISREG(st.st_mode);
}

int DirectoryExists(const char *dirname) {
    if (!dirname) return 0;
    struct stat st;
    return stat(dirname, &st) == 0 && S_ISDIR(st.st_mode);
}

int CreateDir(const char *dirname) {
    if (!dirname) return 0;
    return mkdir(dirname, 0755) == 0;
}

int DeleteFile(const char *filename) {
    if (!filename) return 0;
    return unlink(filename) == 0;
}

long FileSizeOf(const char *filename) {
    if (!filename) return -1;
    struct stat st;
    if (stat(filename, &st) != 0 || !S_ISREG(st.st_mode)) return -1;
    return (long)st.st_size;
}

int FindFirst(const char *pattern, int attr, TSearchRec *rec) {
    if (!pattern || !rec) return -1;
    memset(rec, 0, sizeof(TSearchRec));
    strncpy(rec->_pattern, pattern, sizeof(rec->_pattern) - 1);

    /* Extract directory from pattern */
    char *last_slash = strrchr(pattern, '/');
    if (last_slash) {
        size_t dir_len = last_slash - pattern;
        if (dir_len >= sizeof(rec->_dirname)) dir_len = sizeof(rec->_dirname) - 1;
        memcpy(rec->_dirname, pattern, dir_len);
        rec->_dirname[dir_len] = '\0';
    } else {
        rec->_dirname[0] = '.';
        rec->_dirname[1] = '\0';
    }

    DIR *dir = opendir(rec->_dirname);
    if (!dir) return -1;
    rec->_dir = dir;
    return FindNext(rec);
}

int FindNext(TSearchRec *rec) {
    if (!rec || !rec->_dir) return -1;
    DIR *dir = (DIR *)rec->_dir;
    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) continue;

        /* Simple pattern matching for * and ? */
        const char *pattern = rec->_pattern;
        const char *name = entry->d_name;
        const char *p = pattern;
        const char *n = name;
        int match = 1;
        while (*p && *n) {
            if (*p == '*') {
                p++;
                if (!*p) { match = 1; break; }
                while (*n) {
                    if (FindNext == 0) {} /* placeholder */
                    if (strcasecmp(p, n) == 0) { match = 1; break; }
                    n++;
                }
                if (!*n) { match = 0; break; }
            } else if (*p == '?') {
                p++; n++;
            } else if (tolower((unsigned char)*p) == tolower((unsigned char)*n)) {
                p++; n++;
            } else {
                match = 0; break;
            }
        }
        if (*p == '*' && !*n) match = 1;
        else if (*p || *n) match = 0;

        if (match) {
            strncpy(rec->Name, entry->d_name, sizeof(rec->Name) - 1);
            rec->Name[sizeof(rec->Name) - 1] = '\0';

            char fullpath[2048];
            snprintf(fullpath, sizeof(fullpath), "%s/%s", rec->_dirname, entry->d_name);
            struct stat st;
            if (stat(fullpath, &st) == 0) {
                rec->Size = st.st_size;
                rec->Mode = (unsigned int)st.st_mode;
                rec->Time = (long)st.st_mtime;
                rec->Attr = 0;
                if (S_ISDIR(st.st_mode)) rec->Attr |= faDirectory;
                if (st.st_mode & S_IRUSR) rec->Attr |= faReadOnly; /* approximate */
            }
            return 0;
        }
    }
    return -1;
}

void FindClose(TSearchRec *rec) {
    if (rec && rec->_dir) {
        closedir((DIR *)rec->_dir);
        rec->_dir = NULL;
    }
}

char *ExtractFileName(const char *filename) {
    if (!filename) return gc_strdup("");
    const char *slash = strrchr(filename, '/');
    return gc_strdup(slash ? slash + 1 : filename);
}

char *ExtractFilePath(const char *filename) {
    if (!filename) return gc_strdup("");
    const char *slash = strrchr(filename, '/');
    if (!slash) return gc_strdup("");
    int len = (int)(slash - filename + 1);
    char *result = malloc(len + 1);
    if (!result) return NULL;
    memcpy(result, filename, len);
    result[len] = '\0';
    return result;
}

char *ExtractFileExt(const char *filename) {
    if (!filename) return gc_strdup("");
    const char *slash = strrchr(filename, '/');
    const char *dot = strrchr(filename, '.');
    if (dot && (!slash || dot > slash)) {
        return gc_strdup(dot);
    }
    return gc_strdup("");
}

char *ChangeFileExt(const char *filename, const char *ext) {
    if (!filename) return gc_strdup("");
    char *name = gc_strdup(filename);
    if (!name) return NULL;
    char *slash = strrchr(name, '/');
    char *dot = strrchr(name, '.');
    if (dot && (!slash || dot > slash)) {
        *dot = '\0';
    }
    if (ext && ext[0]) {
        char *newname = strconcat3(name, ext[0] == '.' ? "" : ".", ext);
        free(name);
        return newname;
    }
    return name;
}

char *IncludeTrailingPathDelimiter(const char *path) {
    if (!path) return gc_strdup("/");
    size_t len = strlen(path);
    if (len > 0 && path[len - 1] == '/') return gc_strdup(path);
    char *result = malloc(len + 2);
    if (!result) return NULL;
    strcpy(result, path);
    result[len] = '/';
    result[len + 1] = '\0';
    return result;
}

char *ExcludeTrailingPathDelimiter(const char *path) {
    if (!path) return gc_strdup("");
    size_t len = strlen(path);
    while (len > 0 && path[len - 1] == '/') len--;
    if (len == 0) return gc_strdup("/");
    char *result = malloc(len + 1);
    if (!result) return NULL;
    memcpy(result, path, len);
    result[len] = '\0';
    return result;
}

int64_t FileAge(const char *filename) {
    if (!filename) return -1;
    struct stat st;
    if (stat(filename, &st) != 0) return -1;
    return (int64_t)st.st_mtime;
}

/* ------------------------------------------------------------------ */
/* Date/time helpers (Pascal SysUtils DateUtils subset)               */
/* ------------------------------------------------------------------ */

static TDateTime UnixTimeToDateTime(time_t t) {
    /* Delphi/Pascal TDateTime: days since 1899-12-30, fractional day */
    const double unix_epoch_offset = 25569.0; /* days from 1899-12-30 to 1970-01-01 */
    return unix_epoch_offset + (double)t / 86400.0;
}

TDateTime Now(void) {
    return UnixTimeToDateTime(time(NULL));
}

TDateTime Date(void) {
    TDateTime now = Now();
    return floor(now);
}

TDateTime Time(void) {
    TDateTime now = Now();
    return now - floor(now);
}

char *FormatDateTime(const char *format, TDateTime datetime) {
    if (!format) return gc_strdup("");
    time_t t = (time_t)((datetime - 25569.0) * 86400.0);
    struct tm *tm = localtime(&t);
    if (!tm) return gc_strdup("");

    char buf[256];
    char *out = buf;
    size_t out_size = sizeof(buf);

    for (const char *f = format; *f; f++) {
        if (*f == 'y' || *f == 'Y') {
            int year_count = 1;
            while (f[year_count] == 'y' || f[year_count] == 'Y') year_count++;
            int year = tm->tm_year + 1900;
            if (year_count >= 4) {
                snprintf(out, out_size, "%04d", year);
            } else {
                snprintf(out, out_size, "%02d", year % 100);
            }
            f += year_count - 1;
        } else if (*f == 'm' || *f == 'M') {
            int month_count = 1;
            while (f[month_count] == 'm' || f[month_count] == 'M') month_count++;
            int month = tm->tm_mon + 1;
            snprintf(out, out_size, "%0*d", month_count, month);
            f += month_count - 1;
        } else if (*f == 'd' || *f == 'D') {
            int day_count = 1;
            while (f[day_count] == 'd' || f[day_count] == 'D') day_count++;
            snprintf(out, out_size, "%0*d", day_count, tm->tm_mday);
            f += day_count - 1;
        } else if (*f == 'h' || *f == 'H') {
            int hour_count = 1;
            while (f[hour_count] == 'h' || f[hour_count] == 'H') hour_count++;
            snprintf(out, out_size, "%0*d", hour_count, tm->tm_hour);
            f += hour_count - 1;
        } else if (*f == 'n' || *f == 'N') {
            int min_count = 1;
            while (f[min_count] == 'n' || f[min_count] == 'N') min_count++;
            snprintf(out, out_size, "%0*d", min_count, tm->tm_min);
            f += min_count - 1;
        } else if (*f == 's' || *f == 'S') {
            int sec_count = 1;
            while (f[sec_count] == 's' || f[sec_count] == 'S') sec_count++;
            snprintf(out, out_size, "%0*d", sec_count, tm->tm_sec);
            f += sec_count - 1;
        } else {
            char c[2] = { *f, '\0' };
            strncat(out, c, out_size - strlen(out) - 1);
        }
        out = buf + strlen(buf);
        out_size = sizeof(buf) - strlen(buf);
    }
    return gc_strdup(buf);
}

int DayOfWeek(TDateTime datetime) {
    /* 1 = Sunday .. 7 = Saturday */
    time_t t = (time_t)((datetime - 25569.0) * 86400.0);
    struct tm *tm = localtime(&t);
    if (!tm) return 1;
    return tm->tm_wday + 1;
}