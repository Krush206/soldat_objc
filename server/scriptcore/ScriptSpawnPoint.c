#include "ScriptSpawnPoint.h"
#include <stdlib.h>
#include <string.h>

/* ---------- ScriptSpawnPoint property accessors ---------- */

bool ScriptSpawnPoint_GetActive(const ScriptSpawnPoint *self)
{
    return self->spawn->Active;
}

void ScriptSpawnPoint_SetActive(ScriptSpawnPoint *self, bool active)
{
    self->spawn->Active = active;
}

int32_t ScriptSpawnPoint_GetX(const ScriptSpawnPoint *self)
{
    return self->spawn->X;
}

void ScriptSpawnPoint_SetX(ScriptSpawnPoint *self, int32_t x)
{
    self->spawn->X = x;
}

int32_t ScriptSpawnPoint_GetY(const ScriptSpawnPoint *self)
{
    return self->spawn->Y;
}

void ScriptSpawnPoint_SetY(ScriptSpawnPoint *self, int32_t y)
{
    self->spawn->Y = y;
}

uint8_t ScriptSpawnPoint_GetStyle(const ScriptSpawnPoint *self)
{
    return self->spawn->Team;
}

void ScriptSpawnPoint_SetStyle(ScriptSpawnPoint *self, uint8_t style)
{
    self->spawn->Team = style;
}

/* ---------- ScriptNewSpawnPoint ---------- */

ScriptNewSpawnPoint *ScriptNewSpawnPoint_Create(void)
{
    ScriptNewSpawnPoint *obj = (ScriptNewSpawnPoint *)malloc(sizeof(ScriptNewSpawnPoint));
    if (!obj) return NULL;

    obj->base.spawn = (MapSpawnPoint *)malloc(sizeof(MapSpawnPoint));
    if (!obj->base.spawn) {
        free(obj);
        return NULL;
    }

    memset(obj->base.spawn, 0, sizeof(MapSpawnPoint));
    return obj;
}

void ScriptNewSpawnPoint_Destroy(ScriptNewSpawnPoint *self)
{
    if (self) {
        free(self->base.spawn);
        free(self);
    }
}

/* ---------- ScriptActiveSpawnPoint ---------- */

ScriptActiveSpawnPoint *ScriptActiveSpawnPoint_CreateActive(uint8_t id, MapSpawnPoint *spawn)
{
    ScriptActiveSpawnPoint *obj = (ScriptActiveSpawnPoint *)malloc(sizeof(ScriptActiveSpawnPoint));
    if (!obj) return NULL;

    obj->base.spawn = spawn;
    obj->ID = id;
    return obj;
}

/* ---------- Helper functions used by the runtime registration ---------- */

static bool ActiveReadHelper(const ScriptSpawnPoint *self)
{
    return ScriptSpawnPoint_GetActive(self);
}

static void ActiveWriteHelper(ScriptSpawnPoint *self, bool value)
{
    ScriptSpawnPoint_SetActive(self, value);
}

static int32_t XReadHelper(const ScriptSpawnPoint *self)
{
    return ScriptSpawnPoint_GetX(self);
}

static void XWriteHelper(ScriptSpawnPoint *self, int32_t value)
{
    ScriptSpawnPoint_SetX(self, value);
}

static int32_t YReadHelper(const ScriptSpawnPoint *self)
{
    return ScriptSpawnPoint_GetY(self);
}

static void YWriteHelper(ScriptSpawnPoint *self, int32_t value)
{
    ScriptSpawnPoint_SetY(self, value);
}

static uint8_t StyleReadHelper(const ScriptSpawnPoint *self)
{
    return ScriptSpawnPoint_GetStyle(self);
}

static void StyleWriteHelper(ScriptSpawnPoint *self, uint8_t value)
{
    ScriptSpawnPoint_SetStyle(self, value);
}

static uint8_t IDReadHelper(const ScriptActiveSpawnPoint *self)
{
    return self->ID;
}

/* ---------- API registration stubs (to be hooked into the Pascal compiler/executor) ---------- */

/* The real implementation would call into the Pascal compiler / executor APIs.
   Here we provide empty bodies that can be filled when the host integrates the C code. */
void ScriptSpawnPointAPI_CompilerRegister(PascalCompilerHandle compiler)
{
    (void)compiler;
    /* Example registration pseudocode:
       Class *spawnClass = PascalCompiler_AddClass(compiler, NULL, "TSpawnPoint");
       PascalClass_RegisterProperty(spawnClass, "Active", "Boolean", RW);
       PascalClass_RegisterProperty(spawnClass, "X", "LongInt", RW);
       PascalClass_RegisterProperty(spawnClass, "Y", "LongInt", RW);
       PascalClass_RegisterProperty(spawnClass, "Style", "Byte", RW);

       Class *newClass = PascalCompiler_AddClass(compiler, spawnClass, "TNewSpawnPoint");
       PascalClass_RegisterMethod(newClass, "constructor Create");
       PascalClass_RegisterMethod(newClass, "procedure Free");

       Class *activeClass = PascalCompiler_AddClass(compiler, spawnClass, "TActiveSpawnPoint");
       PascalClass_RegisterProperty(activeClass, "ID", "Byte", ReadOnly);
    */
}

void ScriptSpawnPointAPI_RuntimeRegisterApi(PascalExecHandle exec)
{
    (void)exec;
    /* Example registration pseudocode:
       RuntimeClass *rc = PascalExec_AddClass(exec, "TSpawnPoint");
       PascalRuntime_RegisterPropertyHelper(rc, "Active", ActiveReadHelper, ActiveWriteHelper);
       PascalRuntime_RegisterPropertyHelper(rc, "X", XReadHelper, XWriteHelper);
       PascalRuntime_RegisterPropertyHelper(rc, "Y", YReadHelper, YWriteHelper);
       PascalRuntime_RegisterPropertyHelper(rc, "Style", StyleReadHelper, StyleWriteHelper);

       rc = PascalExec_AddClass(exec, "TNewSpawnPoint");
       PascalRuntime_RegisterConstructor(rc, (void *)ScriptNewSpawnPoint_Create, "Create");
       PascalRuntime_RegisterMethod(rc, (void *)ScriptNewSpawnPoint_Destroy, "Free");

       rc = PascalExec_AddClass(exec, "TActiveSpawnPoint");
       PascalRuntime_RegisterPropertyHelper(rc, "ID", IDReadHelper, NULL);
    */
}