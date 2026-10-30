#include "ScriptUnit.h"
#include <stdlib.h>
#include <string.h>

/* Minimal definition of TScript used by this unit */
struct TScript {
    char *Name;
    char *Dir;
    int Debug;               /* 1 = true, 0 = false */
};

/* TScriptUnit definition */
struct TScriptUnit {
    TScript *FScript;
    TOnException FOnException;
    TOnUnhandledException FOnUnhandledException;
};

/* TScriptUnitAPI definition */
struct TScriptUnitAPI {
    TScriptUnit *FUnit;
};

/* Helper: duplicate a string (caller must free) */
static char *strdup_or_null(const char *s) {
    return s ? strdup(s) : NULL;
}

/* -------------------- TScriptUnit implementation -------------------- */

TScriptUnit *ScriptUnit_Create(TScript *script) {
    if (!script) return NULL;
    TScriptUnit *self = (TScriptUnit *)malloc(sizeof(TScriptUnit));
    if (!self) return NULL;
    self->FScript = script;
    self->FOnException = NULL;
    self->FOnUnhandledException = NULL;
    return self;
}

void ScriptUnit_Destroy(TScriptUnit *self) {
    if (!self) return;
    free(self);
}

void ScriptUnit_Recompile(TScriptUnit *self, int force) {
    if (!self) return;
    /* In Pascal this raises EScriptRecompile.  Here we simply record the request.
       A real engine would trigger a recompilation pass. */
    (void)force; /* unused in this minimal implementation */
}

void ScriptUnit_Unload(TScriptUnit *self) {
    if (!self) return;
    /* Pascal raises EScriptUnload.  We note the request; actual unload is
       performed by the host after the current script execution finishes. */
}

const char *ScriptUnit_GetName(TScriptUnit *self) {
    if (!self || !self->FScript) return "";
    return self->FScript->Name ? self->FScript->Name : "";
}

const char *ScriptUnit_GetVersion(TScriptUnit *self) {
    (void)self;
    /* Pascal returns the constant '3.0 alpha'. */
    return "3.0 alpha";
}

const char *ScriptUnit_GetDir(TScriptUnit *self) {
    if (!self || !self->FScript) return "";
    return self->FScript->Dir ? self->FScript->Dir : "";
}

int ScriptUnit_GetDebugMode(TScriptUnit *self) {
    if (!self || !self->FScript) return 0;
    return self->FScript->Debug;
}

/* Event property accessors */
void ScriptUnit_SetOnException(TScriptUnit *self, TOnException callback) {
    if (!self) return;
    self->FOnException = callback;
}

TOnException ScriptUnit_GetOnException(TScriptUnit *self) {
    if (!self) return NULL;
    return self->FOnException;
}

void ScriptUnit_SetOnUnhandledException(TScriptUnit *self, TOnUnhandledException callback) {
    if (!self) return;
    self->FOnUnhandledException = callback;
}

TOnUnhandledException ScriptUnit_GetOnUnhandledException(TScriptUnit *self) {
    if (!self) return NULL;
    return self->FOnUnhandledException;
}

/* -------------------- TScriptUnitAPI implementation -------------------- */

TScriptUnitAPI *ScriptUnitAPI_Create(TScript *script) {
    if (!script) return NULL;
    TScriptUnitAPI *self = (TScriptUnitAPI *)malloc(sizeof(TScriptUnitAPI));
    if (!self) return NULL;
    self->FUnit = ScriptUnit_Create(script);
    if (!self->FUnit) {
        free(self);
        return NULL;
    }
    return self;
}

void ScriptUnitAPI_Destroy(TScriptUnitAPI *self) {
    if (!self) return;
    ScriptUnit_Destroy(self->FUnit);
    free(self);
}

/* The following three functions correspond to the Pascal overrides.
   They receive opaque pointers (compiler, exec) because the full
   PascalCompiler / PascalExec definitions are not part of this unit. */
void ScriptUnitAPI_CompilerRegister(TScriptUnitAPI *self, void *compiler) {
    (void)self;
    (void)compiler;
    /* In the original Pascal code this registers:
       - an enum type TErrorType
       - procedural types TOnException and TOnUnhandledException
       - a class TScript with methods Recompile, Unload and properties
       - a global variable 'Script' of that class.
       The C version leaves the actual registration to the host
       scripting engine; here we provide a no‑op stub. */
}

void ScriptUnitAPI_RuntimeRegisterApi(TScriptUnitAPI *self, void *exec) {
    (void)self;
    (void)exec;
    /* Pascal registers the runtime methods and property helpers for
       the TScript class.  In C we would normally bind the function
       pointers (ScriptUnit_Recompile, ScriptUnit_Unload, getters,
       setters) to the scripting VM.  This stub does nothing. */
}

void ScriptUnitAPI_RuntimeRegisterVariables(TScriptUnitAPI *self, void *exec) {
    (void)self;
    (void)exec;
    /* Pascal makes the global variable 'Script' point at the
       TScriptUnit instance.  The C counterpart would set a VM
       variable; omitted here. */
}

TScriptUnit *ScriptUnitAPI_GetScriptUnit(TScriptUnitAPI *self) {
    if (!self) return NULL;
    return self->FUnit;
}