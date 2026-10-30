/* PolyMap — C port of shared/PolyMap.pas */

#include "PolyMap.h"
#include "Constants.h"
#include "Vector.h"
#include "MapFile.h"
#include "Util.h"
#include <string.h>
#include <stdio.h>

void TPolyMap_Initialize(TPolyMap *self)
{
    memset(self, 0, sizeof(TPolyMap));
    self->MapID = 0;
    self->SectorsDivision = 0;
    self->SectorsNum = 0;
    self->StartJet = 0;
    self->Grenades = 0;
    self->Medikits = 0;
    self->Weather = 0;
    self->Steps = 0;
    self->PolyCount = 0;
    self->BackPolyCount = 0;
    self->ColliderCount = 0;
    self->FlagSpawn[0] = 0;
    self->FlagSpawn[1] = 0;
    self->FlagSpawn[2] = 0;
    self->LoadGraphics = NULL;
}

bool TPolyMap_LoadMap(TPolyMap *self, struct TMapInfo map)
{
    (void)self;
    (void)map;
    return false;  /* TODO: implement */
}

bool TPolyMap_LoadMapBg(TPolyMap *self, struct TMapInfo map, bool bgForce,
    int bgColorTop, int bgColorBtm)
{
    (void)self;
    (void)map;
    (void)bgForce;
    (void)bgColorTop;
    (void)bgColorBtm;
    return false;  /* TODO: implement */
}

bool TPolyMap_LineInPoly(TPolyMap *self, TVector2 a, TVector2 b, int poly,
    TVector2 *v)
{
    (void)self;
    (void)a;
    (void)b;
    (void)poly;
    (void)v;
    return false;  /* TODO: implement */
}

bool TPolyMap_PointInPolyEdges(TPolyMap *self, float x, float y, int i)
{
    (void)self;
    (void)x;
    (void)y;
    (void)i;
    return false;  /* TODO: implement */
}

bool TPolyMap_PointInPoly(TPolyMap *self, TVector2 p, TMapPolygon *poly)
{
    (void)self;
    (void)p;
    (void)poly;
    return false;  /* TODO: implement */
}

TVector2 TPolyMap_ClosestPerpendicular(TPolyMap *self, int j, TVector2 pos,
    float *d, int *n)
{
    (void)self;
    (void)j;
    (void)pos;
    (void)d;
    (void)n;
    return (TVector2){0, 0};  /* TODO: implement */
}

/* TODO: implement remaining PolyMap functions */
