/* Things — C port of shared/mechanics/Things.pas */

#ifndef THINGS_H
#define THINGS_H

#include "Parts.h"
#include "Anims.h"
#include "MapFile.h"
#include "PolyMap.h"
#include "Net.h"
#include "Weapons.h"
#include "Constants.h"
#include "Vector.h"
#include "Sprites.h"

typedef struct TThingCollision {
    unsigned char ThingNum;
    int CooldownEnd;
} TThingCollision;

typedef struct TThing {
    bool Active;
    unsigned char Style, Num, Owner;
    unsigned char HoldingSprite;
    unsigned char AmmoCount;
    float Radius;
    int TimeOut;
    bool StaticType;
    int Interest;
    bool CollideWithBullets;
    bool InBase;
    unsigned char LastSpawn, Team;
    ParticleSystem Skeleton;
    unsigned char CollideCount[4 + 1];      /* Pascal 1..4 */
    TMapPolygon Polys[2 + 1];                /* Pascal 1..2 */
    TBackgroundState BGState;
    /* client branch */
    int Tex1, Tex2;
    long Texture;
    unsigned int Color;
} TThing;

void TThing_Update(TThing *self);
void TThing_Render(TThing *self, double timeElapsed);
void TThing_PolygonsRender(TThing *self);
bool TThing_CheckMapCollision(TThing *self, int i, float x, float y);
void TThing_Kill(TThing *self);
void TThing_CheckOutOfBounds(TThing *self);
void TThing_Respawn(TThing *self);
void TThing_MoveSkeleton(TThing *self, float x1, float y1, bool fromZero);
int TThing_CheckStationaryGunCollision(TThing *self, bool clientCheck);

int CreateThing(TVector2 sPos, unsigned char owner, unsigned char sStyle,
    unsigned char n);
TThingCollision ThingCollision(unsigned char thingNum, int cooldownEnd);
bool SpawnBoxes(TVector2 *start, unsigned char team, unsigned char num);
bool RandomizeStart(TVector2 *start, unsigned char team);

extern TThing Thing[MAX_THINGS + 1];

#endif /* THINGS_H */