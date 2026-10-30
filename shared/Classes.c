/* Classes — C port of FreePascal Classes unit subset */
#include "Classes.h"
#include "SysUtils.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

struct StringList {
    char **strings;
    int count;
    int capacity;
};

StringList *StringList_Create(void) {
    StringList *sl = calloc(1, sizeof(StringList));
    if (!sl) return NULL;
    sl->capacity = 16;
    sl->strings = calloc(sl->capacity, sizeof(char *));
    if (!sl->strings) { free(sl); return NULL; }
    return sl;
}

void StringList_Free(StringList *sl) {
    if (!sl) return;
    for (int i = 0; i < sl->count; i++) {
        free(sl->strings[i]);
    }
    free(sl->strings);
    free(sl);
}

int StringList_Count(const StringList *sl) {
    return sl ? sl->count : 0;
}

const char *StringList_Get(const StringList *sl, int index) {
    if (!sl || index < 0 || index >= sl->count) return NULL;
    return sl->strings[index];
}

void StringList_Set(StringList *sl, int index, const char *s) {
    if (!sl || index < 0 || index >= sl->count) return;
    free(sl->strings[index]);
    sl->strings[index] = s ? strdup(s) : NULL;
}

int StringList_Add(StringList *sl, const char *s) {
    if (!sl) return -1;
    if (sl->count >= sl->capacity) {
        int new_cap = sl->capacity * 2;
        char **new_strings = realloc(sl->strings, new_cap * sizeof(char *));
        if (!new_strings) return -1;
        sl->strings = new_strings;
        sl->capacity = new_cap;
    }
    sl->strings[sl->count] = s ? strdup(s) : NULL;
    return sl->count++;
}

int StringList_AddIfNotPresent(StringList *sl, const char *s) {
    if (!sl) return -1;
    int idx = StringList_IndexOf(sl, s);
    if (idx >= 0) return idx;
    return StringList_Add(sl, s);
}

void StringList_Insert(StringList *sl, int index, const char *s) {
    if (!sl || index < 0 || index > sl->count) return;
    if (sl->count >= sl->capacity) {
        int new_cap = sl->capacity * 2;
        char **new_strings = realloc(sl->strings, new_cap * sizeof(char *));
        if (!new_strings) return;
        sl->strings = new_strings;
        sl->capacity = new_cap;
    }
    memmove(&sl->strings[index + 1], &sl->strings[index],
            (sl->count - index) * sizeof(char *));
    sl->strings[index] = s ? strdup(s) : NULL;
    sl->count++;
}

void StringList_Delete(StringList *sl, int index) {
    if (!sl || index < 0 || index >= sl->count) return;
    free(sl->strings[index]);
    memmove(&sl->strings[index], &sl->strings[index + 1],
            (sl->count - index - 1) * sizeof(char *));
    sl->count--;
    sl->strings[sl->count] = NULL;
}

void StringList_Clear(StringList *sl) {
    if (!sl) return;
    for (int i = 0; i < sl->count; i++) {
        free(sl->strings[i]);
    }
    sl->count = 0;
}

int StringList_IndexOf(const StringList *sl, const char *s) {
    if (!sl || !s) return -1;
    for (int i = 0; i < sl->count; i++) {
        if (sl->strings[i] && strcmp(sl->strings[i], s) == 0) return i;
    }
    return -1;
}

int StringList_IndexOfName(const StringList *sl, const char *name) {
    if (!sl || !name) return -1;
    size_t len = strlen(name);
    for (int i = 0; i < sl->count; i++) {
        if (sl->strings[i] && strncmp(sl->strings[i], name, len) == 0 &&
            sl->strings[i][len] == '=') {
            return i;
        }
    }
    return -1;
}

const char *StringList_GetValue(const StringList *sl, const char *name) {
    int idx = StringList_IndexOfName(sl, name);
    if (idx < 0) return NULL;
    const char *eq = strchr(sl->strings[idx], '=');
    return eq ? eq + 1 : "";
}

void StringList_SetValue(StringList *sl, const char *value, const char *name) {
    if (!sl || !name) return;
    int idx = StringList_IndexOfName(sl, name);
    if (idx >= 0) {
        free(sl->strings[idx]);
        if (value) {
            size_t len = strlen(name) + strlen(value) + 2;
            sl->strings[idx] = malloc(len);
            if (sl->strings[idx]) snprintf(sl->strings[idx], len, "%s=%s", name, value);
        } else {
            sl->strings[idx] = NULL;
        }
    } else if (value) {
        StringList_Add(sl, "");
        idx = sl->count - 1;
        size_t len = strlen(name) + strlen(value) + 2;
        sl->strings[idx] = malloc(len);
        if (sl->strings[idx]) snprintf(sl->strings[idx], len, "%s=%s", name, value);
    }
}

const char *StringList_GetName(const StringList *sl, int index) {
    if (!sl || index < 0 || index >= sl->count) return NULL;
    const char *eq = strchr(sl->strings[index], '=');
    if (!eq) return NULL;
    size_t len = eq - sl->strings[index];
    char *name = malloc(len + 1);
    if (!name) return NULL;
    memcpy(name, sl->strings[index], len);
    name[len] = '\0';
    return name;
}

const char *StringList_GetText(const StringList *sl) {
    if (!sl) return strdup("");
    size_t total = 1;
    for (int i = 0; i < sl->count; i++) {
        if (sl->strings[i]) total += strlen(sl->strings[i]) + 1;
    }
    char *text = malloc(total);
    if (!text) return strdup("");
    char *p = text;
    for (int i = 0; i < sl->count; i++) {
        if (sl->strings[i]) {
            size_t len = strlen(sl->strings[i]);
            memcpy(p, sl->strings[i], len);
            p += len;
        }
        *p++ = '\n';
    }
    if (p > text && *(p - 1) == '\n') p--;
    *p = '\0';
    return text;
}

void StringList_SetText(StringList *sl, const char *text) {
    if (!sl) return;
    StringList_Clear(sl);
    if (!text) return;
    
    char *copy = strdup(text);
    if (!copy) return;
    
    char *line = strtok(copy, "\n");
    while (line) {
        StringList_Add(sl, line);
        line = strtok(NULL, "\n");
    }
    free(copy);
}

const char *StringList_GetDelimitedText(const StringList *sl) {
    if (!sl || sl->count == 0) return strdup("");
    size_t total = 1;
    for (int i = 0; i < sl->count; i++) {
        if (sl->strings[i]) total += strlen(sl->strings[i]) + 1;
    }
    char *text = malloc(total);
    if (!text) return strdup("");
    char *p = text;
    for (int i = 0; i < sl->count; i++) {
        if (sl->strings[i]) {
            size_t len = strlen(sl->strings[i]);
            memcpy(p, sl->strings[i], len);
            p += len;
        }
        if (i < sl->count - 1) *p++ = ',';
    }
    *p = '\0';
    return text;
}

void StringList_SetDelimitedText(StringList *sl, const char *text) {
    if (!sl) return;
    StringList_Clear(sl);
    if (!text) return;
    
    char *copy = strdup(text);
    if (!copy) return;
    
    char *token = strtok(copy, ",");
    while (token) {
        StringList_Add(sl, token);
        token = strtok(NULL, ",");
    }
    free(copy);
}

bool StringList_LoadFromFile(StringList *sl, const char *filename) {
    if (!sl || !filename) return false;
    FILE *f = fopen(filename, "r");
    if (!f) return false;
    StringList_Clear(sl);
    char buf[4096];
    while (fgets(buf, sizeof(buf), f)) {
        size_t len = strlen(buf);
        if (len > 0 && buf[len - 1] == '\n') buf[--len] = '\0';
        if (len > 0 && buf[len - 1] == '\r') buf[--len] = '\0';
        StringList_Add(sl, buf);
    }
    fclose(f);
    return true;
}

bool StringList_SaveToFile(const StringList *sl, const char *filename) {
    if (!sl || !filename) return false;
    FILE *f = fopen(filename, "w");
    if (!f) return false;
    for (int i = 0; i < sl->count; i++) {
        if (sl->strings[i]) {
            fputs(sl->strings[i], f);
        }
        fputc('\n', f);
    }
    fclose(f);
    return true;
}

StringList *StringList_Copy(const StringList *sl) {
    if (!sl) return NULL;
    StringList *copy = StringList_Create();
    if (!copy) return NULL;
    for (int i = 0; i < sl->count; i++) {
        if (sl->strings[i]) StringList_Add(copy, sl->strings[i]);
    }
    return copy;
}

struct Thread {
    pthread_t _tid;
    bool _running;
    bool _finished;
    bool _terminated;
    ThreadExecuteFunc _execute;
    pthread_mutex_t _sync_mutex;
    pthread_cond_t _sync_cond;
    ThreadSyncFunc _sync_func;
    void *_sync_data;
    bool _sync_pending;
    pthread_mutex_t _queue_mutex;
    ThreadSyncFunc *_queue_funcs;
    void **_queue_data;
    int _queue_count;
    int _queue_capacity;
};

Thread *Thread_Create(void) {
    Thread *t = calloc(1, sizeof(Thread));
    if (!t) return NULL;
    pthread_mutex_init(&t->_sync_mutex, NULL);
    pthread_cond_init(&t->_sync_cond, NULL);
    pthread_mutex_init(&t->_queue_mutex, NULL);
    t->_queue_capacity = 16;
    t->_queue_funcs = calloc(t->_queue_capacity, sizeof(ThreadSyncFunc));
    t->_queue_data = calloc(t->_queue_capacity, sizeof(void *));
    return t;
}

void Thread_Free(Thread *t) {
    if (!t) return;
    if (t->_running) Thread_Terminate(t);
    if (t->_tid) pthread_join(t->_tid, NULL);
    pthread_mutex_destroy(&t->_sync_mutex);
    pthread_cond_destroy(&t->_sync_cond);
    pthread_mutex_destroy(&t->_queue_mutex);
    free(t->_queue_funcs);
    free(t->_queue_data);
    free(t);
}

static void *thread_main(void *arg) {
    Thread *t = (Thread *)arg;
    t->_running = true;
    if (t->_execute) t->_execute(t);
    t->_running = false;
    t->_finished = true;
    return NULL;
}

bool Thread_Start(Thread *t, ThreadExecuteFunc execute) {
    if (!t || t->_running) return false;
    t->_execute = execute;
    t->_terminated = false;
    t->_finished = false;
    int ret = pthread_create(&t->_tid, NULL, thread_main, t);
    return ret == 0;
}

void Thread_Terminate(Thread *t) {
    if (!t) return;
    t->_terminated = true;
}

bool Thread_WaitFor(Thread *t) {
    if (!t || !t->_tid) return false;
    void *retval;
    int ret = pthread_join(t->_tid, &retval);
    t->_tid = 0;
    return ret == 0;
}

bool Thread_Running(const Thread *t) {
    return t && t->_running;
}

bool Thread_Finished(const Thread *t) {
    return t && t->_finished;
}

void Thread_Synchronize(Thread *t, ThreadSyncFunc func, void *data) {
    if (!t || !func) return;
    pthread_mutex_lock(&t->_sync_mutex);
    t->_sync_func = func;
    t->_sync_data = data;
    t->_sync_pending = true;
    pthread_cond_signal(&t->_sync_cond);
    pthread_mutex_unlock(&t->_sync_mutex);
}

void Thread_Queue(Thread *t, ThreadSyncFunc func, void *data) {
    if (!t || !func) return;
    pthread_mutex_lock(&t->_queue_mutex);
    if (t->_queue_count >= t->_queue_capacity) {
        t->_queue_capacity *= 2;
        t->_queue_funcs = realloc(t->_queue_funcs, t->_queue_capacity * sizeof(ThreadSyncFunc));
        t->_queue_data = realloc(t->_queue_data, t->_queue_capacity * sizeof(void *));
    }
    t->_queue_funcs[t->_queue_count] = func;
    t->_queue_data[t->_queue_count] = data;
    t->_queue_count++;
    pthread_mutex_unlock(&t->_queue_mutex);
}

void Thread_CheckSynchronize(void) {
    /* This would need a global thread registry or be called from specific thread context */
    /* For now, it's a no-op - the main thread should call this to process sync requests */
}