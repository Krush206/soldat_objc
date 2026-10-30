#include "ScriptMap.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/* External constants (defined elsewhere) */
extern int MAX_THINGS;
extern int MAX_BULLETS;
extern int MAX_SPAWNPOINTS;

/* External game globals (defined elsewhere) */
extern struct TThing *Thing;               /* array[1..MAX_THINGS] */
extern struct TBullet *Bullet;             /* array[1..MAX_BULLETS] */
extern struct TMap Map;                    /* current map */
extern struct TGun *Guns;                  /* weapon definitions */
extern struct TServer Server;              /* server instance */
extern struct TMainConsole MainConsole;    /* console */

/* Forward declarations of helper constructors (implemented in their own units) */
TScriptActiveObject *TScriptActiveObject_CreateActive(int id, struct TThing *thing);
TScriptActiveBullet *TScriptActiveBullet_CreateActive(int id, struct TBullet *bullet);
TScriptActiveSpawnPoint *TScriptActiveSpawnPoint_CreateActive(int id, struct TSpawnPoint *spawn);
TScriptActiveFlag *TScriptActiveFlag_CreateActive(int id, struct TThing *thing);

/* External functions used */
extern bool Map_RayCast(const TVector2 *a, const TVector2 *b,
                        float *distance, float maxDist,
                        bool player, bool flag, bool bullet,
                        bool checkCollider, uint8_t team);
extern int ServerCreateBullet(const TVector2 *pos, const TVector2 *vel,
                              int weaponNum, int ownerID, int team,
                              float hitM, bool fromScript);
extern int CreateThing(const TVector2 *pos, uint8_t owner, uint8_t style, uint8_t team);
extern bool PrepareMapChange(const char *newMap);
extern void Server_NextMap(void);
extern void MainConsole_Console(const char *msg, int color);

/* Definitions for new object structs (mirrored from Pascal) */
struct TScriptNewObject {
    float X, Y;
    uint8_t Style;
};

struct TScriptNewSpawnPoint {
    float X, Y;
    uint8_t Style;   /* team */
};

/* ------------------------------------------------------------------ */
/* Constructor / Destructor                                            */
/* ------------------------------------------------------------------ */
TScriptMap *ScriptMap_Create(void)
{
    TScriptMap *self = (TScriptMap *)calloc(1, sizeof(TScriptMap));
    if (!self) return NULL;

    /* allocate arrays (1‑based indexing, index 0 unused) */
    self->objects = (TScriptActiveObject *)calloc(MAX_THINGS + 1, sizeof(TScriptActiveObject));
    self->bullets = (TScriptActiveBullet *)calloc(MAX_BULLETS + 1, sizeof(TScriptActiveBullet));
    self->spawns  = (TScriptActiveSpawnPoint *)calloc(MAX_SPAWNPOINTS + 1, sizeof(TScriptActiveSpawnPoint));

    if (!self->objects || !self->bullets || !self->spawns) {
        ScriptMap_Destroy(self);
        return NULL;
    }

    for (int i = 1; i <= MAX_THINGS; ++i)
        self->objects[i] = *TScriptActiveObject_CreateActive(i, &Thing[i]);

    for (int i = 1; i <= MAX_BULLETS; ++i)
        self->bullets[i] = *TScriptActiveBullet_CreateActive(i, &Bullet[i]);

    for (int i = 1; i <= MAX_SPAWNPOINTS; ++i)
        self->spawns[i] = *TScriptActiveSpawnPoint_CreateActive(i, &Map.SpawnPoints[i]);

    /* last flag objects start as NULL */
    self->lastFlagObjs[1] = NULL;
    self->lastFlagObjs[2] = NULL;
    self->lastFlagObjs[3] = NULL;

    self->onBeforeMapChange = NULL;
    self->onAfterMapChange  = NULL;

    return self;
}

void ScriptMap_Destroy(TScriptMap *self)
{
    if (!self) return;

    for (int i = 1; i <= MAX_THINGS; ++i) {
        /* Assuming a Free method exists */
        // TScriptActiveObject_Free(&self->objects[i]);
    }
    for (int i = 1; i <= MAX_BULLETS; ++i) {
        // TScriptActiveBullet_Free(&self->bullets[i]);
    }
    for (int i = 1; i <= MAX_SPAWNPOINTS; ++i) {
        // TScriptActiveSpawnPoint_Free(&self->spawns[i]);
    }
    for (int i = 1; i <= 3; ++i) {
        if (self->lastFlagObjs[i]) {
            // TScriptActiveFlag_Free(self->lastFlagObjs[i]);
            self->lastFlagObjs[i] = NULL;
        }
    }

    free(self->objects);
    free(self->bullets);
    free(self->spawns);
    free(self);
}

/* ------------------------------------------------------------------ */
/* Property accessors                                                  */
/* ------------------------------------------------------------------ */
TScriptActiveObject *ScriptMap_GetObject(TScriptMap *self, uint8_t id)
{
    if (id == 0 || id > MAX_THINGS) {
        fprintf(stderr, "ScriptMap_GetObject: ID must be 1..%d\n", MAX_THINGS);
        return NULL;
    }
    return &self->objects[id];
}

TScriptActiveBullet *ScriptMap_GetBullet(TScriptMap *self, uint8_t id)
{
    if (id == 0 || id > MAX_BULLETS) {
        fprintf(stderr, "ScriptMap_GetBullet: ID must be 1..%d\n", MAX_BULLETS);
        return NULL;
    }
    return &self->bullets[id];
}

TScriptActiveSpawnPoint *ScriptMap_GetSpawn(TScriptMap *self, uint8_t id)
{
    if (id == 0 || id > MAX_SPAWNPOINTS) {
        fprintf(stderr, "ScriptMap_GetSpawn: ID must be 1..%d\n", MAX_SPAWNPOINTS);
        return NULL;
    }
    return &self->spawns[id];
}

void ScriptMap_SetSpawn(TScriptMap *self, uint8_t id, const TScriptActiveSpawnPoint *spawn)
{
    if (id == 0 || id > MAX_SPAWNPOINTS) return;
    self->spawns[id].Active = spawn->Active;
    self->spawns[id].X = spawn->X;
    self->spawns[id].Y = spawn->Y;
    self->spawns[id].Style = spawn->Style;
}

const char *ScriptMap_GetName(TScriptMap *self)
{
    (void)self;
    return Map.Name;
}

TScriptActiveFlag *ScriptMap_GetFlag(TScriptMap *self, int id)
{
    if (id < 1 || id > 3) return NULL;

    TScriptActiveFlag *result = self->lastFlagObjs[id];
    if (result == NULL || result->Style != id) {
        if (result) {
            // TScriptActiveFlag_Free(result);
            self->lastFlagObjs[id] = NULL;
        }
        for (int i = 1; i <= MAX_THINGS; ++i) {
            if (Thing[i].Style == id) {
                self->lastFlagObjs[id] = TScriptActiveFlag_CreateActive(i, &Thing[i]);
                result = self->lastFlagObjs[id];
                break;
            }
        }
    }
    return result;
}

/* ------------------------------------------------------------------ */
/* RayCast                                                            */
/* ------------------------------------------------------------------ */
bool ScriptMap_RayCast(TScriptMap *self,
                       float x1, float y1, float x2, float y2,
                       bool player, bool flag, bool bullet,
                       bool checkCollider, uint8_t team)
{
    (void)self;
    TVector2 a = {x1, y1};
    TVector2 b = {x2, y2};
    float distance = 0.0f;
    return Map_RayCast(&a, &b, &distance, 99999.0f,
                       player, flag, bullet, checkCollider, team);
}

bool ScriptMap_RayCastVector(TScriptMap *self,
                             const TVector2 *a, const TVector2 *b,
                             bool player, bool flag, bool bullet,
                             bool checkCollider, uint8_t team)
{
    (void)self;
    float distance = 0.0f;
    return Map_RayCast(a, b, &distance, 99999.0f,
                       player, flag, bullet, checkCollider, team);
}

/* ------------------------------------------------------------------ */
/* Bullet creation                                                    */
/* ------------------------------------------------------------------ */
int ScriptMap_CreateBulletVector(TScriptMap *self,
                                 const TVector2 *a, const TVector2 *b,
                                 float hitM, uint8_t style,
                                 TScriptActivePlayer *owner)
{
    (void)self;
    int foundWeapon = -1;
    for (int i = 0; Guns[i].BulletStyle != 0; ++i) { /* assuming terminated */
        if (Guns[i].BulletStyle == style) {
            foundWeapon = i;
            break;
        }
    }
    if (foundWeapon >= 0) {
        return ServerCreateBullet(a, b,
                                  Guns[foundWeapon].Num,
                                  owner ? owner->ID : 255,
                                  255, hitM, true);
    }
    return -1;
}

int ScriptMap_CreateBullet(TScriptMap *self,
                           float x, float y, float velX, float velY,
                           float hitM, uint8_t style,
                           TScriptActivePlayer *owner)
{
    TVector2 pos = {x, y};
    TVector2 vel = {velX, velY};
    return ScriptMap_CreateBulletVector(self, &pos, &vel, hitM, style, owner);
}

/* ------------------------------------------------------------------ */
/* AddObject                                                          */
/* ------------------------------------------------------------------ */
TScriptActiveObject *ScriptMap_AddObject(TScriptMap *self,
                                         const struct TScriptNewObject *obj)
{
    (void)self;
    if (obj->Style > OBJECT_STATIONARY_GUN || obj->Style < OBJECT_ALPHA_FLAG)
        return NULL;

    TVector2 pos = {obj->X, obj->Y};
    int idx = CreateThing(&pos, 255, obj->Style, 255);
    if (idx <= 0 || idx > MAX_THINGS) return NULL;

    TScriptActiveObject *result = &self->objects[idx];

    /* Weapon‑type objects need ammo initialisation */
    if ( (obj->Style > OBJECT_POINTMATCH_FLAG && obj->Style < OBJECT_MEDICAL_KIT) ||
         (obj->Style > OBJECT_PARACHUTE && obj->Style <= OBJECT_STATIONARY_GUN) ) {
        uint8_t realWeapon = 0;
        switch (obj->Style) {
            case OBJECT_USSOCOM:        realWeapon = COLT; break;
            case OBJECT_DESERT_EAGLE:   realWeapon = EAGLE; break;
            case OBJECT_HK_MP5:         realWeapon = MP5; break;
            case OBJECT_AK74:           realWeapon = AK74; break;
            case OBJECT_STEYR_AUG:      realWeapon = STEYRAUG; break;
            case OBJECT_SPAS12:         realWeapon = SPAS12; break;
            case OBJECT_RUGER77:        realWeapon = RUGER77; break;
            case OBJECT_M79:            realWeapon = M79; break;
            case OBJECT_BARRET_M82A1:   realWeapon = BARRETT; break;
            case OBJECT_MINIMI:         realWeapon = M249; break;
            case OBJECT_MINIGUN:        realWeapon = MINIGUN; break;
            case OBJECT_RAMBO_BOW:      realWeapon = BOW; break;
            case OBJECT_COMBAT_KNIFE:   realWeapon = CHAINSAW; break;
            case OBJECT_CHAINSAW:       realWeapon = KNIFE; break;
            case OBJECT_LAW:            realWeapon = LAW; break;
            case OBJECT_STATIONARY_GUN: realWeapon = M2; break;
            default: return result;
        }
        Thing[idx].AmmoCount = (int)(Guns[realWeapon].Ammo + 0.5f);
    }
    return result;
}

/* ------------------------------------------------------------------ */
/* AddSpawnPoint                                                      */
/* ------------------------------------------------------------------ */
TScriptActiveSpawnPoint *ScriptMap_AddSpawnPoint(TScriptMap *self,
                                                 const struct TScriptNewSpawnPoint *spawn)
{
    (void)self;
    for (int i = 1; i <= MAX_SPAWNPOINTS; ++i) {
        if (!Map.SpawnPoints[i].Active) {
            Map.SpawnPoints[i].X = spawn->X;
            Map.SpawnPoints[i].Y = spawn->Y;
            Map.SpawnPoints[i].Team = spawn->Style;
            Map.SpawnPoints[i].Active = true;
            /* update wrapper */
            self->spawns[i].Active = true;
            self->spawns[i].X = spawn->X;
            self->spawns[i].Y = spawn->Y;
            self->spawns[i].Style = spawn->Style;
            return &self->spawns[i];
        }
    }
    return NULL;
}

/* ------------------------------------------------------------------ */
/* Map changing                                                        */
/* ------------------------------------------------------------------ */
void ScriptMap_NextMap(TScriptMap *self)
{
    (void)self;
    Server_NextMap();
}

void ScriptMap_SetMap(TScriptMap *self, const char *newMap)
{
    (void)self;
    if (!PrepareMapChange(newMap)) {
        MainConsole_Console("Map not found", WARNING_MESSAGE_COLOR);
    }
}

/* ------------------------------------------------------------------ */
/* Callback accessors                                                  */
/* ------------------------------------------------------------------ */
TOnBeforeMapChange ScriptMap_GetOnBeforeMapChange(TScriptMap *self)
{
    return self->onBeforeMapChange;
}
void ScriptMap_SetOnBeforeMapChange(TScriptMap *self, TOnBeforeMapChange cb)
{
    self->onBeforeMapChange = cb;
}
TOnAfterMapChange ScriptMap_GetOnAfterMapChange(TScriptMap *self)
{
    return self->onAfterMapChange;
}
void ScriptMap_SetOnAfterMapChange(TScriptMap *self, TOnAfterMapChange cb)
{
    self->onAfterMapChange = cb;
}