#include "PascalCore.h"
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <stdio.h>

/* External PascalScript library functions (to be linked) */
extern TPSScript* TPSScript_Create(void* owner);
extern void TPSScript_Free(TPSScript* script);
extern void TPSScript_ExecuteFunction(TPSScript* script, void** params, size_t count, const char* funcName);

/* ------------------------------------------------------------------ */
/* TPascalCore implementation */
struct TPascalCore {
    TPSScript* FPascalScript;
    char* FDir;
    /* optionally a name for logging */
    char* Name;
};

TPascalCore* TPascalCore_Create(const char* Dir)
{
    if (!Dir) return NULL;

    TPascalCore* self = (TPascalCore*)malloc(sizeof(TPascalCore));
    if (!self) return NULL;

    self->FPascalScript = TPSScript_Create(NULL);
    if (!self->FPascalScript) {
        free(self);
        return NULL;
    }

    /* Ensure trailing slash */
    size_t len = strlen(Dir);
    self->FDir = (char*)malloc(len + 2);
    if (!self->FDir) {
        TPSScript_Free(self->FPascalScript);
        free(self);
        return NULL;
    }
    strcpy(self->FDir, Dir);
    if (self->FDir[len - 1] != '/') {
        self->FDir[len] = '/';
        self->FDir[len + 1] = '\0';
    }

    self->Name = NULL; /* could be set by subclass */
    return self;
}

void TPascalCore_Destroy(TPascalCore* self)
{
    if (!self) return;
    if (self->FPascalScript) TPSScript_Free(self->FPascalScript);
    free(self->FDir);
    free(self->Name);
    free(self);
}

TPSScript* TPascalCore_GetPascalScript(const TPascalCore* self)
{
    return self ? self->FPascalScript : NULL;
}

void TPascalCore_SetPascalScript(TPascalCore* self, TPSScript* script)
{
    if (!self) return;
    if (self->FPascalScript) TPSScript_Free(self->FPascalScript);
    self->FPascalScript = script;
}

/* ------------------------------------------------------------------ */
/* TThreadScript implementation */
struct TThreadScript {
    TPascalCore* FScript;
    char* FFuncName;
    void** FParams;
    size_t FParamCount;
    pthread_t thread;
};

static void* thread_entry(void* arg)
{
    TThreadScript* self = (TThreadScript*)arg;
    if (self) {
        TThreadScript_Execute(self);
        /* Free resources after execution (FreeOnTerminate) */
        TThreadScript_Destroy(self);
    }
    return NULL;
}

TThreadScript* TThreadScript_Create(TPascalCore* Script, void** Parameters, size_t paramCount, const char* FuncName)
{
    if (!Script || !FuncName) return NULL;

    TThreadScript* self = (TThreadScript*)malloc(sizeof(TThreadScript));
    if (!self) return NULL;

    self->FScript = Script;
    self->FFuncName = strdup(FuncName);
    if (!self->FFuncName) {
        free(self);
        return NULL;
    }

    self->FParamCount = paramCount;
    if (paramCount > 0) {
        self->FParams = (void**)malloc(paramCount * sizeof(void*));
        if (!self->FParams) {
            free(self->FFuncName);
            free(self);
            return NULL;
        }
        memcpy(self->FParams, Parameters, paramCount * sizeof(void*));
    } else {
        self->FParams = NULL;
    }

    /* Create the thread (FreeOnTerminate = true) */
    int rc = pthread_create(&self->thread, NULL, thread_entry, self);
    if (rc != 0) {
        /* thread creation failed – clean up */
        free(self->FParams);
        free(self->FFuncName);
        free(self);
        return NULL;
    }

    /* Detach so that resources are reclaimed when thread finishes */
    pthread_detach(self->thread);
    return self;
}

void TThreadScript_Execute(TThreadScript* self)
{
    if (!self || !self->FScript || !self->FScript->FPascalScript) return;

    /* Call the PascalScript function */
    TPSScript_ExecuteFunction(self->FScript->FPascalScript,
                               self->FParams,
                               self->FParamCount,
                               self->FFuncName);
}

void TThreadScript_Destroy(TThreadScript* self)
{
    if (!self) return;
    free(self->FFuncName);
    free(self->FParams);
    free(self);
}

/* ------------------------------------------------------------------ */
/* TPascalCore::ThreadFunc */
void TPascalCore_ThreadFunc(TPascalCore* self, void** Parameters, size_t paramCount, const char* FuncName)
{
    if (!self) return;
    /* Creates a TThreadScript which starts its own thread */
    TThreadScript_Create(self, Parameters, paramCount, FuncName);
    /* No need to keep the returned pointer – thread frees itself */
}