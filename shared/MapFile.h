/* MapFile — C port of shared/MapFile.pas */

#ifndef MAPFILE_H
#define MAPFILE_H

#include "Vector.h"
#include "Waypoints.h"
#include "Util.h"

typedef unsigned char TMapColor[4]; /* [r,g,b,a] */

typedef struct TMapVertex {
    float x, y, z, rhw;
    TMapColor Color;
    float u, v;
} TMapVertex;
typedef TMapVertex *PMapVertex;

typedef struct TMapPolygon {
    TMapVertex Vertices[4];   /* Pascal 1..3 */
    TVector3 Normals[4];     /* Pascal 1..3 */
    unsigned char PolyType;
    unsigned char TextureIndex;
} TMapPolygon;
typedef TMapPolygon *PMapPolygon;

typedef struct TMapSector {
    unsigned short *Polys;
    int PolysCount;
} TMapSector;
typedef TMapSector *PMapSector;

typedef struct TMapProp {
    bool Active;
    unsigned short Style;
    int Width, Height;
    float x, y;
    float Rotation;
    float ScaleX, ScaleY;
    unsigned char Alpha;
    TMapColor Color;
    unsigned char Level;
} TMapProp;
typedef TMapProp *PMapProp;

typedef struct TMapScenery {
    char *Filename;
    int Date;
} TMapScenery;
typedef TMapScenery *PMapScenery;

typedef struct TMapCollider {
    bool Active;
    float x, y;
    float Radius;
} TMapCollider;
typedef TMapCollider *PMapCollider;

typedef struct TMapSpawnpoint {
    bool Active;
    int x, y, Team;
} TMapSpawnpoint;
typedef TMapSpawnpoint *PMapSpawnpoint;

typedef struct TMapFile {
    char *Filename;
    struct TMapInfo MapInfo;
    unsigned int Hash;
    int Version;
    char *MapName;
    char **Textures;
    int TexturesCount;
    TMapColor BgColorTop;
    TMapColor BgColorBtm;
    int StartJet;
    unsigned char GrenadePacks;
    unsigned char Medikits;
    unsigned char Weather;
    unsigned char Steps;
    int RandomID;
    TMapPolygon *Polygons;
    int PolygonsCount;
    int SectorsDivision;
    int SectorsNum;
    TMapSector *Sectors;
    int SectorsCount;
    TMapProp *Props;
    int PropsCount;
    TMapScenery *Scenery;
    int SceneryCount;
    TMapCollider *Colliders;
    int CollidersCount;
    TMapSpawnpoint *Spawnpoints;
    int SpawnpointsCount;
    struct TWaypoint *Waypoints;
    int WaypointsCount;
} TMapFile;
typedef TMapFile *PMapFile;

void MapColorOf(TMapColor dest, int color);
bool IsPropActive(TMapFile *map, int index);
bool LoadMapFile(struct TMapInfo mapinfo, TMapFile *map);

#endif /* MAPFILE_H */