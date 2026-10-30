#ifndef SCRIPTGLOBAL_H
#define SCRIPTGLOBAL_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    char dateSeparator;
    char* shortDateFormat;
} ScriptGlobal;

typedef struct {
    ScriptGlobal* global;
} ScriptGlobalAPI;

/* ScriptGlobal functions */
ScriptGlobal* ScriptGlobal_Create(void);
void ScriptGlobal_Destroy(ScriptGlobal* self);
char ScriptGlobal_GetDateSeparator(ScriptGlobal* self);
void ScriptGlobal_SetDateSeparator(ScriptGlobal* self, char separator);
const char* ScriptGlobal_GetShortDateFormat(ScriptGlobal* self);
void ScriptGlobal_SetShortDateFormat(ScriptGlobal* self, const char* format);

/* ScriptGlobalAPI functions */
ScriptGlobalAPI* ScriptGlobalAPI_Create(void* script);
void ScriptGlobalAPI_Destroy(ScriptGlobalAPI* api);
void ScriptGlobalAPI_CompilerRegister(ScriptGlobalAPI* api, void* compiler);
void ScriptGlobalAPI_RuntimeRegisterApi(ScriptGlobalAPI* api, void* exec);
void ScriptGlobalAPI_RuntimeRegisterVariables(ScriptGlobalAPI* api, void* exec);

#ifdef __cplusplus
}
#endif

#endif /* SCRIPTGLOBAL_H */