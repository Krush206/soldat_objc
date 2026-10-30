#ifndef SCRIPTMAP_H
#define SCRIPTMAP_H

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>

/* Forward declarations of related script types */
typedef struct TScriptActiveObject TScriptActiveObject;
typedef struct TScriptActiveBullet TScriptActiveBullet;
typedef struct TScriptActiveSpawnPoint TScriptActiveSpawnPoint;
typedef struct TScriptActiveFlag TScriptActiveFlag;
typedef struct TScriptActivePlayer TScriptActivePlayer;
typedef struct TVector2 TVector2;

/* Callback types */
typedef void (*TOnBeforeMapChange)(const char *nextMap);
typedef void (*TOnAfterMapChange)(const char *nextMap);

/* ScriptMap structure */
typedef struct {
    TScriptActiveObject *objects;          /* array[1..MAX_THINGS] */
    TScriptActiveBullet *bullets;          /* array[1..MAX_BULLETS] */
    TScriptActiveSpawnPoint *spawns;       /* array[1..MAX_SPAWNPOINTS] */
    TScriptActiveFlag *lastFlagObjs[4];    /* indices 1..3 used */
    TOnBeforeMapChange onBeforeMapChange;
    TOnAfterMapChange onAfterMapChange;
} TScriptMap;

/* Constructor / destructor */
TScriptMap *ScriptMap_Create(void);
void ScriptMap_Destroy(TScriptMap *self);

/* Property accessors */
TScriptActiveObject *ScriptMap_GetObject(TScriptMap *self, uint8_t id);
TScriptActiveBullet *ScriptMap_GetBullet(TScriptMap *self, uint8_t id);
TScriptActiveSpawnPoint *ScriptMap_GetSpawn(TScriptMap *self, uint8_t id);
void ScriptMap_SetSpawn(TScriptMap *self, uint8_t id, const TScriptActiveSpawnPoint *spawn);
const char *ScriptMap_GetName(TScriptMap *self);
TScriptActiveFlag *ScriptMap_GetFlag(TScriptMap *self, int id); /* id 1..3 */

/* Methods */
bool ScriptMap_RayCast(TScriptMap *self,
                       float x1, float y1, float x2, float y2,
                       bool player, bool flag, bool bullet,
                       bool checkCollider, uint8_t team);
bool ScriptMap_RayCastVector(TScriptMap *self,
                             const TVector2 *a, const TVector2 *b,
                             bool player, bool flag, bool bullet,
                             bool checkCollider, uint8_t team);
int ScriptMap_CreateBulletVector(TScriptMap *self,
                                 const TVector2 *a, const TVector2 *b,
                                 float hitM, uint8_t style,
                                 TScriptActivePlayer *owner);
int ScriptMap_CreateBullet(TScriptMap *self,
                           float x, float y, float velX, float velY,
                           float hitM, uint8_t style,
                           TScriptActivePlayer *owner);
TScriptActiveObject *ScriptMap_AddObject(TScriptMap *self,
                                         const struct TScriptNewObject *obj);
TScriptActiveSpawnPoint *ScriptMap_AddSpawnPoint(TScriptMap *self,
                                                 const struct TScriptNewSpawnPoint *spawn);
void ScriptMap_NextMap(TScriptMap *self);
void ScriptMap_SetMap(TScriptMap *self, const char *newMap);

/* Callback getters / setters */
TOnBeforeMapChange ScriptMap_GetOnBeforeMapChange(TScriptMap *self);
void ScriptMap_SetOnBeforeMapChange(TScriptMap *self, TOnBeforeMapChange cb);
TOnAfterMapChange ScriptMap_GetOnAfterMapChange(TScriptMap *self);
void ScriptMap_SetOnAfterMapChange(TScriptMap *self, TOnAfterMapChange cb);

#endif /* SCRIPTMAP_H */