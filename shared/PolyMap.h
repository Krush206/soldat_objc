/* PolyMap — C port of shared/PolyMap.pas */

#ifndef POLYMAP_H
#define POLYMAP_H

#include "MapFile.h"
#include "Waypoints.h"
#include "Util.h"
#include <stdbool.h>

#define MAX_POLYS 5000
#define MIN_SECTOR (-25)
#define MAX_SECTOR 25
#define MIN_SECTORZ (-35)
#define MAX_SECTORZ 35
#define TILESECTOR 3
#define MIN_TILE (MIN_SECTOR * TILESECTOR)
#define MAX_TILE (MAX_SECTOR * TILESECTOR)
#define MAX_PROPS 500
#define MAX_SPAWNPOINTS 255
#define MAX_COLLIDERS 128

/* Polygon constants */
#define POLY_TYPE_NORMAL 0
#define POLY_TYPE_ONLY_BULLETS 1
#define POLY_TYPE_ONLY_PLAYER 2
#define POLY_TYPE_DOESNT 3
#define POLY_TYPE_ICE 4
#define POLY_TYPE_DEADLY 5
#define POLY_TYPE_BLOODY_DEADLY 6
#define POLY_TYPE_HURTS 7
#define POLY_TYPE_REGENERATES 8
#define POLY_TYPE_LAVA 9
#define POLY_TYPE_RED_BULLETS 10
#define POLY_TYPE_RED_PLAYER 11
#define POLY_TYPE_BLUE_BULLETS 12
#define POLY_TYPE_BLUE_PLAYER 13
#define POLY_TYPE_YELLOW_BULLETS 14
#define POLY_TYPE_YELLOW_PLAYER 15
#define POLY_TYPE_GREEN_BULLETS 16
#define POLY_TYPE_GREEN_PLAYER 17
#define POLY_TYPE_BOUNCY 18
#define POLY_TYPE_EXPLODES 19
#define POLY_TYPE_HURTS 7
#define POLY_TYPE_RED_BULLETS 10
#define POLY_TYPE_RED_PLAYER 11
#define POLY_TYPE_BLUE_BULLETS 12
#define POLY_TYPE_BLUE_PLAYER 13
#define POLY_TYPE_YELLOW_BULLETS 14
#define POLY_TYPE_YELLOW_PLAYER 15
#define POLY_TYPE_GREEN_BULLETS 16
#define POLY_TYPE_GREEN_PLAYER 17
#define POLY_TYPE_BOUNCY 18
#define POLY_TYPE_EXPLODES 19
#define POLY_TYPE_HURTS_FLAGGERS 20
#define POLY_TYPE_ONLY_FLAGGERS 21
#define POLY_TYPE_NOT_FLAGGERS 22
#define POLY_TYPE_NON_FLAGGER_COLLIDES 23
#define POLY_TYPE_BACKGROUND 24
#define POLY_TYPE_BACKGROUND_TRANSITION 25

/* The poly will interact as an "only players collide" poly */
#define BACKGROUND_NORMAL 0
/* The poly will interact as a "doesn't collide" poly */
#define BACKGROUND_TRANSITION 1
/* Whether a background poly is being interacted with is currently unknown
 * and must be found out */
#define BACKGROUND_POLY_UNKNOWN (-2)
/* No background poly is currently being interacted with */
#define BACKGROUND_POLY_NONE (-1)

/* Map Pascal sector index (-35..35) onto the C array. */
#define SECTOR_IDX(i) ((i) - MIN_SECTORZ)

typedef void (*TLoadMapGraphics)(struct TMapFile *mapFile, bool bgForce,
    TMapColor bgColorTop, TMapColor bgColorBtm);

typedef struct TPolyMap {
    unsigned int MapID;
    struct TMapInfo MapInfo;
    char Name[65];        /* Pascal shortstring string[64] */
    char *Filename;
    int SectorsDivision;
    int SectorsNum;
    int StartJet;
    unsigned char Grenades;
    unsigned char Medikits;
    unsigned char Weather;
    unsigned char Steps;
    int PolyCount;
    int BackPolyCount;
    int ColliderCount;
    TMapPolygon Polys[MAX_POLYS + 1];            /* 1..MAX_POLYS */
    struct PMapPolygon *BackPolys[MAX_POLYS + 1];         /* 1..BackPolyCount */
    unsigned char PolyType[MAX_POLYS + 1];
    TVector2 Perp[MAX_POLYS + 1][4];              /* [1..3] */
    float Bounciness[MAX_POLYS + 1];
    TMapSector Sectors[MAX_SECTORZ - MIN_SECTORZ + 1][MAX_SECTORZ - MIN_SECTORZ + 1];
    TMapSpawnpoint Spawnpoints[MAX_SPAWNPOINTS + 1];
    TMapCollider Collider[MAX_COLLIDERS + 1];
    int FlagSpawn[3];                             /* 1..2 */
    void (*LoadGraphics)(struct TMapFile *mapFile, bool bgForce,
        TMapColor bgColorTop, TMapColor bgColorBtm);
} TPolyMap;

void TPolyMap_Initialize(TPolyMap *self);
void TPolyMap_LoadData(TPolyMap *self, struct TMapFile *mapFile);
bool TPolyMap_LoadMap(TPolyMap *self, struct TMapInfo map);
bool TPolyMap_LoadMapBg(TPolyMap *self, struct TMapInfo map, bool bgForce,
    int bgColorTop, int bgColorBtm);
bool TPolyMap_LineInPoly(TPolyMap *self, TVector2 a, TVector2 b, int poly,
    TVector2 *v);
bool TPolyMap_PointInPolyEdges(TPolyMap *self, float x, float y, int i);
bool TPolyMap_PointInPoly(TPolyMap *self, TVector2 p, TMapPolygon *poly);
TVector2 TPolyMap_ClosestPerpendicular(TPolyMap *self, int j, TVector2 pos,
    float *d, int *n);
bool TPolyMap_CollisionTest(TPolyMap *self, TVector2 pos, TVector2 *perpVec,
    bool isFlag);
bool TPolyMap_CollisionTestExcept(TPolyMap *self, TVector2 pos,
    TVector2 *perpVec, int c);
bool TPolyMap_RayCast(TPolyMap *self, TVector2 a, TVector2 b, float *distance,
    float maxDist, bool player, bool flag, bool bullet, bool checkCollider,
    unsigned char team);

void CheckOutOfBoundsF(float *x, float *y);
void CheckOutOfBoundsS(short *x, short *y);

#endif /* POLYMAP_H */