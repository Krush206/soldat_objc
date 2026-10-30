#ifndef SCRIPTEXCEPTIONS_H
#define SCRIPTEXCEPTIONS_H

#include <stdbool.h>
#include <stddef.h>

/* Base exception type */
typedef struct ScriptException {
    char *message;
    const char *class_name;

    /* Virtual methods */
    char *(*get_message)(struct ScriptException *);
    void (*set_message)(struct ScriptException *, const char *);
    void (*destroy)(struct ScriptException *);
} ScriptException;

/* Constructors / destructor */
ScriptException *ScriptException_create(const char *msg);
void ScriptException_destroy(ScriptException *e);
char *ScriptException_get_message(ScriptException *e);
void ScriptException_set_message(ScriptException *e, const char *msg);

/* EScriptUnload – no extra data */
typedef struct EScriptUnload {
    ScriptException base;
} EScriptUnload;

EScriptUnload *EScriptUnload_create(const char *msg);
void EScriptUnload_destroy(EScriptUnload *e);

/* EScriptRecompile – carries a Force flag */
typedef struct EScriptRecompile {
    ScriptException base;
    bool force;
} EScriptRecompile;

EScriptRecompile *EScriptRecompile_create(const char *msg, bool force);
void EScriptRecompile_destroy(EScriptRecompile *e);
bool EScriptRecompile_get_force(const EScriptRecompile *e);

/* EAccessDenied – inherits ScriptException */
typedef struct EAccessDenied {
    ScriptException base;
} EAccessDenied;

EAccessDenied *EAccessDenied_create(const char *msg);
void EAccessDenied_destroy(EAccessDenied *e);

/* ENotImplemented – inherits ScriptException */
typedef struct ENotImplemented {
    ScriptException base;
} ENotImplemented;

ENotImplemented *ENotImplemented_create(const char *msg);
void ENotImplemented_destroy(ENotImplemented *e);

#endif /* SCRIPTEXCEPTIONS_H */