#ifndef SCRIPTOBJECT_H
#define SCRIPTOBJECT_H

#ifdef __cplusplus
extern "C" {
#endif

/* Forward declarations of internal structures */
typedef struct TThing TThing;
typedef struct TSkeleton TSkeleton;
typedef struct TPos TPos;

/* Base script object */
typedef struct ScriptObject {
    TThing *FObj;
    /* Virtual method table */
    unsigned char (*GetStyle)(struct ScriptObject *self);
    float (*GetX)(struct ScriptObject *self);
    float (*GetY)(struct ScriptObject *self);
} ScriptObject;

/* New object with writable properties */
typedef struct ScriptNewObject {
    ScriptObject base;
    void (*SetStyle)(struct ScriptNewObject *self, unsigned char style);
    void (*SetX)(struct ScriptNewObject *self, float x);
    void (*SetY)(struct ScriptNewObject *self, float y);
} ScriptNewObject;

/* Active object referencing existing thing */
typedef struct ScriptActiveObject {
    ScriptObject base;
    unsigned char FID;
    int (*GetActive)(struct ScriptActiveObject *self);
    unsigned char (*GetID)(struct ScriptActiveObject *self);
    void (*Kill)(struct ScriptActiveObject *self);
} ScriptActiveObject;

/* Active flag object */
typedef struct ScriptActiveFlag {
    ScriptActiveObject base;
    int (*GetInBase)(struct ScriptActiveFlag *self);
} ScriptActiveFlag;

/* Constructor / Destructor */
ScriptNewObject *ScriptNewObject_Create(void);
void ScriptNewObject_Destroy(ScriptNewObject *obj);

/* Active object constructor */
ScriptActiveObject *ScriptActiveObject_CreateActive(unsigned char id, TThing *obj);

/* API registration functions */
void ScriptObjectAPI_CompilerRegister(void *compiler);
void ScriptObjectAPI_RuntimeRegisterApi(void *exec);

#ifdef __cplusplus
}
#endif

#endif /* SCRIPTOBJECT_H */