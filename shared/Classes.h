/* Classes — C port of FreePascal Classes unit subset */

#ifndef CLASSES_H
#define CLASSES_H

#include <pthread.h>
#include <stdbool.h>

typedef struct StringList StringList;

StringList *StringList_Create(void);
void StringList_Free(StringList *sl);
int StringList_Count(const StringList *sl);
const char *StringList_Get(const StringList *sl, int index);
void StringList_Set(StringList *sl, int index, const char *s);
int StringList_Add(StringList *sl, const char *s);
int StringList_AddIfNotPresent(StringList *sl, const char *s);
void StringList_Insert(StringList *sl, int index, const char *s);
void StringList_Delete(StringList *sl, int index);
void StringList_Clear(StringList *sl);
int StringList_IndexOf(const StringList *sl, const char *s);
int StringList_IndexOfName(const StringList *sl, const char *name);
const char *StringList_GetValue(const StringList *sl, const char *name);
void StringList_SetValue(StringList *sl, const char *value, const char *name);
const char *StringList_GetName(const StringList *sl, int index);
const char *StringList_GetText(const StringList *sl);
void StringList_SetText(StringList *sl, const char *text);
const char *StringList_GetDelimitedText(const StringList *sl);
void StringList_SetDelimitedText(StringList *sl, const char *text);
bool StringList_LoadFromFile(StringList *sl, const char *filename);
bool StringList_SaveToFile(const StringList *sl, const char *filename);
StringList *StringList_Copy(const StringList *sl);

typedef struct Thread Thread;
typedef void (*ThreadExecuteFunc)(Thread *self);
typedef void (*ThreadSyncFunc)(void *data);

Thread *Thread_Create(void);
void Thread_Free(Thread *thread);
bool Thread_Start(Thread *thread, ThreadExecuteFunc execute);
void Thread_Terminate(Thread *thread);
bool Thread_WaitFor(Thread *thread);
bool Thread_Running(const Thread *thread);
bool Thread_Finished(const Thread *thread);
void Thread_Synchronize(Thread *thread, ThreadSyncFunc func, void *data);
void Thread_Queue(Thread *thread, ThreadSyncFunc func, void *data);
void Thread_CheckSynchronize(void);

#endif /* CLASSES_H */