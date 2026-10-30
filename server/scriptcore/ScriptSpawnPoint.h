#ifndef SCRIPT_SPAWN_POINT_H
#define SCRIPT_SPAWN_POINT_H

#include <stdbool.h>
#include <stdint.h>

/* Forward declaration of map spawn point structure used by the engine */
typedef struct MapSpawnPoint {
    bool Active;
    int32_t X;
    int32_t Y;
    uint8_t Team;
} MapSpawnPoint;

/* Base spawn point script object */
typedef struct ScriptSpawnPoint {
    MapSpawnPoint *spawn;
} ScriptSpawnPoint;

/* New spawn point allocated by script */
typedef struct ScriptNewSpawnPoint {
    ScriptSpawnPoint base;
} ScriptNewSpawnPoint;

/* Active spawn point referencing existing map data */
typedef struct ScriptActiveSpawnPoint {
    ScriptSpawnPoint base;
    uint8_t ID;
} ScriptActiveSpawnPoint;

/* Property accessors for ScriptSpawnPoint */
bool ScriptSpawnPoint_GetActive(const ScriptSpawnPoint *self);
void ScriptSpawnPoint_SetActive(ScriptSpawnPoint *self, bool active);

int32_t ScriptSpawnPoint_GetX(const ScriptSpawnPoint *self);
void ScriptSpawnPoint_SetX(ScriptSpawnPoint *self, int32_t x);

int32_t ScriptSpawnPoint_GetY(const ScriptSpawnPoint *self);
void ScriptSpawnPoint_SetY(ScriptSpawnPoint *self, int32_t y);

uint8_t ScriptSpawnPoint_GetStyle(const ScriptSpawnPoint *self);
void ScriptSpawnPoint_SetStyle(ScriptSpawnPoint *self, uint8_t style);

/* Constructors / destructors */
ScriptNewSpawnPoint *ScriptNewSpawnPoint_Create(void);
void ScriptNewSpawnPoint_Destroy(ScriptNewSpawnPoint *self);

ScriptActiveSpawnPoint *ScriptActiveSpawnPoint_CreateActive(uint8_t id, MapSpawnPoint *spawn);

/* Script API registration (opaque compiler / executor handles) */
typedef void *PascalCompilerHandle;
typedef void *PascalExecHandle;

void ScriptSpawnPointAPI_CompilerRegister(PascalCompilerHandle compiler);
void ScriptSpawnPointAPI_RuntimeRegisterApi(PascalExecHandle exec);

#endif /* SCRIPT_SPAWN_POINT_H */