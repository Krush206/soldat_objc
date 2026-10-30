/* Sparks — C port of shared/mechanics/Sparks.pas */

#ifndef SPARKS_H
#define SPARKS_H

#include "Parts.h"
#include "MapFile.h"
#include "PolyMap.h"
#include "Net.h"
#include "Constants.h"
#include "Vector.h"
#include "Sprites.h"

typedef struct TSpark {
    bool Active;
    short Num;
    float LifeReal;
    unsigned char Life, LifePrev;
    unsigned char Style, Owner;
    unsigned char CollideCount;
} TSpark;

void TSpark_Update(TSpark *self);
void TSpark_Render(TSpark *self);
bool TSpark_CheckMapCollision(TSpark *self, float x, float y);
void TSpark_Kill(TSpark *self);
void TSpark_CheckOutOfBounds(TSpark *self);

int CreateSpark(TVector2 sPos, TVector2 sVelocity, unsigned char sStyle,
    unsigned char sOwner, int life);

extern int SparksCount;
extern TSpark Spark[MAX_SPARKS + 1];

#endif /* SPARKS_H */