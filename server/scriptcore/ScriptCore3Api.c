#include "ScriptCore3Api.h"
#include <stdlib.h>

/* Forward declaration of the low‑level ScriptCore3 implementation.
 * The actual definition resides in the ScriptCore3 module. */
typedef struct ScriptCore3 ScriptCore3;

/* Prototype of the internal CallEvent implementation provided by ScriptCore3. */
Variant ScriptCore3_CallEvent(ScriptCore3* core,
                              const void* event,
                              const Variant* params,
                              size_t paramCount);

/* Internal representation of the API object. */
struct ScriptCore3API
{
    Script* script;   /* Back‑reference to the owning script instance */
};

/* ------------------------------------------------------------------ */
/* Construction / destruction                                           */
/* ------------------------------------------------------------------ */
ScriptCore3API* ScriptCore3API_Create(Script* script)
{
    ScriptCore3API* api = (ScriptCore3API*)malloc(sizeof(ScriptCore3API));
    if (api)
    {
        api->script = script;
    }
    return api;
}

/* ------------------------------------------------------------------ */
/* ICompilerAPI implementation                                           */
/* ------------------------------------------------------------------ */
void ScriptCore3API_CompilerRegister(ScriptCore3API* api, PascalCompiler* compiler)
{
    (void)api;
    (void)compiler;
    /* No operation in the base implementation */
}

/* ------------------------------------------------------------------ */
/* IRuntimeAPI implementation                                            */
/* ------------------------------------------------------------------ */
void ScriptCore3API_RuntimeRegisterApi(ScriptCore3API* api, PascalExec* exec)
{
    (void)api;
    (void)exec;
    /* No operation in the base implementation */
}

void ScriptCore3API_RuntimeRegisterVariables(ScriptCore3API* api, PascalExec* exec)
{
    (void)api;
    (void)exec;
    /* No operation in the base implementation */
}

void ScriptCore3API_BeforeExecute(ScriptCore3API* api, PascalExec* exec)
{
    (void)api;
    (void)exec;
    /* No operation in the base implementation */
}

void ScriptCore3API_AfterExecute(ScriptCore3API* api, PascalExec* exec)
{
    (void)api;
    (void)exec;
    /* No operation in the base implementation */
}

/* ------------------------------------------------------------------ */
/* Event dispatch                                                        */
/* ------------------------------------------------------------------ */
Variant ScriptCore3API_CallEvent(ScriptCore3API* api,
                                 const void* event,
                                 const Variant* params,
                                 size_t paramCount)
{
    if (!api || !api->script)
    {
        /* Return an empty / invalid variant when the API object is not ready */
        Variant empty = {0};
        return empty;
    }

    /* The script instance is expected to be a ScriptCore3 object.
     * Cast through void* to avoid strict‑aliasing issues. */
    return ScriptCore3_CallEvent((ScriptCore3*)api->script, event, params, paramCount);
}