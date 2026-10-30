#ifndef SCRIPT_UNIT_H
#define SCRIPT_UNIT_H

#ifdef __cplusplus
extern "C" {
#endif

/* Forward declarations for related types */
typedef struct TScript TScript;
typedef struct TScriptUnit TScriptUnit;
typedef struct TScriptUnitAPI TScriptUnitAPI;

/* Callback types matching Pascal's TOnException and TOnUnhandledException */
typedef void (*TOnException)(int errorCode,
                             const char *message,
                             const char *unitName,
                             const char *functionName,
                             unsigned int row,
                             unsigned int col);

typedef int (*TOnUnhandledException)(int errorCode,
                                     const char *message,
                                     const char *unitName,
                                     const char *functionName,
                                     unsigned int row,
                                     unsigned int col);

/* TScriptUnit public API */
TScriptUnit *ScriptUnit_Create(TScript *script);
void ScriptUnit_Destroy(TScriptUnit *self);

void ScriptUnit_Recompile(TScriptUnit *self, int force);
void ScriptUnit_Unload(TScriptUnit *self);

/* Read‑only properties */
const char *ScriptUnit_GetName(TScriptUnit *self);
const char *ScriptUnit_GetVersion(TScriptUnit *self);
const char *ScriptUnit_GetDir(TScriptUnit *self);
int ScriptUnit_GetDebugMode(TScriptUnit *self);

/* Event properties */
void ScriptUnit_SetOnException(TScriptUnit *self, TOnException callback);
TOnException ScriptUnit_GetOnException(TScriptUnit *self);

void ScriptUnit_SetOnUnhandledException(TScriptUnit *self, TOnUnhandledException callback);
TOnUnhandledException ScriptUnit_GetOnUnhandledException(TScriptUnit *self);

/* TScriptUnitAPI public API */
TScriptUnitAPI *ScriptUnitAPI_Create(TScript *script);
void ScriptUnitAPI_Destroy(TScriptUnitAPI *self);

void ScriptUnitAPI_CompilerRegister(TScriptUnitAPI *self, void *compiler);
void ScriptUnitAPI_RuntimeRegisterApi(TScriptUnitAPI *self, void *exec);
void ScriptUnitAPI_RuntimeRegisterVariables(TScriptUnitAPI *self, void *exec);

TScriptUnit *ScriptUnitAPI_GetScriptUnit(TScriptUnitAPI *self);

#ifdef __cplusplus
}
#endif

#endif /* SCRIPT_UNIT_H */