#include "ScriptObject.h"
#include <stdlib.h>
#include <string.h>

/* Assume these are defined elsewhere */
extern TThing *Thing;               /* array of things indexed by Byte (0-255) */
extern void Thing_Kill(TThing *thing);
extern void ServerThingTaken(unsigned char id, unsigned char param);
extern const unsigned char OBJECT_ALPHA_FLAG;
extern const unsigned char OBJECT_BRAVO_FLAG;
/* ... other OBJECT_* constants ... */
extern int NUM_PARTICLES;

/* ------------------------------------------------------------------ */
/* ScriptObject virtual methods                                         */
/* ------------------------------------------------------------------ */
static float ScriptObject_GetX(ScriptObject *self)
{
    /* Pascal uses 1‑based index; convert to 0‑based */
    return self->FObj->Skeleton.Pos[0].X;
}

static float ScriptObject_GetY(ScriptObject *self)
{
    return self->FObj->Skeleton.Pos[0].Y;
}

/* ------------------------------------------------------------------ */
/* ScriptNewObject methods                                              */
/* ------------------------------------------------------------------ */
static unsigned char ScriptNewObject_GetStyle(ScriptObject *self)
{
    ScriptNewObject *obj = (ScriptNewObject *)self;
    return obj->base.FObj->Style;
}

static void ScriptNewObject_SetStyle(ScriptNewObject *self, unsigned char style)
{
    self->base.FObj->Style = style;
}

static void ScriptNewObject_SetX(ScriptNewObject *self, float x)
{
    for (int i = 0; i < NUM_PARTICLES; ++i)
        self->base.FObj->Skeleton.Pos[i].X = x;
}

static void ScriptNewObject_SetY(ScriptNewObject *self, float y)
{
    for (int i = 0; i < NUM_PARTICLES; ++i)
        self->base.FObj->Skeleton.Pos[i].Y = y;
}

/* Constructor / Destructor */
ScriptNewObject *ScriptNewObject_Create(void)
{
    ScriptNewObject *obj = (ScriptNewObject *)malloc(sizeof(ScriptNewObject));
    if (!obj) return NULL;

    obj->base.FObj = (TThing *)malloc(sizeof(TThing));
    if (!obj->base.FObj) {
        free(obj);
        return NULL;
    }
    memset(obj->base.FObj, 0, sizeof(TThing));

    /* Initialise virtual table */
    obj->base.GetStyle = ScriptNewObject_GetStyle;
    obj->base.GetX = ScriptObject_GetX;
    obj->base.GetY = ScriptObject_GetY;

    obj->SetStyle = ScriptNewObject_SetStyle;
    obj->SetX = ScriptNewObject_SetX;
    obj->SetY = ScriptNewObject_SetY;

    return obj;
}

void ScriptNewObject_Destroy(ScriptNewObject *obj)
{
    if (obj) {
        free(obj->base.FObj);
        free(obj);
    }
}

/* ------------------------------------------------------------------ */
/* ScriptActiveObject methods                                           */
/* ------------------------------------------------------------------ */
static int ScriptActiveObject_GetActive(ScriptActiveObject *self)
{
    return self->base.FObj->Active;
}

static unsigned char ScriptActiveObject_GetID(ScriptActiveObject *self)
{
    return self->FID;
}

static unsigned char ScriptActiveObject_GetStyle(ScriptObject *self)
{
    ScriptActiveObject *obj = (ScriptActiveObject *)self;
    return obj->base.FObj->Style;
}

static void ScriptActiveObject_Kill(ScriptActiveObject *self)
{
    if (!Thing[self->FID].Active)
        return;
    Thing_Kill(&Thing[self->FID]);
    ServerThingTaken(self->FID, 255);
}

ScriptActiveObject *ScriptActiveObject_CreateActive(unsigned char id, TThing *obj)
{
    ScriptActiveObject *aobj = (ScriptActiveObject *)malloc(sizeof(ScriptActiveObject));
    if (!aobj) return NULL;

    aobj->FID = id;
    aobj->base.FObj = obj;

    aobj->base.GetStyle = ScriptActiveObject_GetStyle;
    aobj->base.GetX = ScriptObject_GetX;
    aobj->base.GetY = ScriptObject_GetY;

    aobj->GetActive = ScriptActiveObject_GetActive;
    aobj->GetID = ScriptActiveObject_GetID;
    aobj->Kill = ScriptActiveObject_Kill;

    return aobj;
}

/* ------------------------------------------------------------------ */
/* ScriptActiveFlag methods                                             */
/* ------------------------------------------------------------------ */
static int ScriptActiveFlag_GetInBase(ScriptActiveFlag *self)
{
    return self->base.base.FObj->InBase;
}

/* ------------------------------------------------------------------ */
/* API registration (stubs – real implementation depends on compiler/exec) */
/* ------------------------------------------------------------------ */
void ScriptObjectAPI_CompilerRegister(void *compiler)
{
    /* The Pascal version registers many constants and classes.
       Here we only provide a placeholder. Real code would call into the
       Pascal compiler API (e.g. Compiler_AddConstant, Compiler_AddClass,
       RegisterProperty, RegisterMethod, etc.). */
    (void)compiler;
}

void ScriptObjectAPI_RuntimeRegisterApi(void *exec)
{
    /* Placeholder for runtime registration similar to the Pascal version. */
    (void)exec;
}