#ifndef SCRIPTCORE3API_H
#define SCRIPTCORE3API_H

/* Forward declarations of types used by the API */
typedef struct Script Script;
typedef struct PascalCompiler PascalCompiler;
typedef struct PascalExec PascalExec;
typedef struct Variant Variant;

/* Opaque handle for the ScriptCore3 API implementation */
typedef struct ScriptCore3API ScriptCore3API;

/* Constructor – creates a new API object bound to the given script */
ScriptCore3API* ScriptCore3API_Create(Script* script);

/* ICompilerAPI methods */
void ScriptCore3API_CompilerRegister(ScriptCore3API* api, PascalCompiler* compiler);

/* IRuntimeAPI methods */
void ScriptCore3API_RuntimeRegisterApi(ScriptCore3API* api, PascalExec* exec);
void ScriptCore3API_RuntimeRegisterVariables(ScriptCore3API* api, PascalExec* exec);
void ScriptCore3API_BeforeExecute(ScriptCore3API* api, PascalExec* exec);
void ScriptCore3API_AfterExecute(ScriptCore3API* api, PascalExec* exec);

/* Event calling helper – forwards to the underlying script implementation */
Variant ScriptCore3API_CallEvent(ScriptCore3API* api,
                                 const void* event,
                                 const Variant* params,
                                 size_t paramCount);

#endif /* SCRIPTCORE3API_H */