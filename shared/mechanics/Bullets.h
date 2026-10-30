/* Bullets — C port of shared/mechanics/Bullets.pas */

#ifndef BULLETS_H
#define BULLETS_H

#include "Parts.h"
#include "MapFile.h"
#include "PolyMap.h"
#include "Net.h"
#include "Weapons.h"
#include "Constants.h"
#include "Vector.h"
#include "Sprites.h"
#include "Things.h" /* for TThingCollision */

/* Pascal `Set of 1..32` — consistent bitmask, bit (i-1). */
#define SPRITE_COLLISION_ADD(set, i) ((set) |= (1u << ((i) - 1)))
#define SPRITE_COLLISION_HAS(set, i) (((set) & (1u << ((i) - 1))) != 0)
#define SPRITE_COLLISION_CLEAR(set) ((set) = 0)

typedef struct TBullet {
    bool Active;
    bool HasHit;
    unsigned char Style;
    short Num;
    unsigned char Owner;
    unsigned char OwnerWeapon;
    float TimeOutReal;
    short TimeOut, TimeOutPrev;
    float HitMultiply, HitMultiplyPrev;
    TVector2 VelocityPrev;
    bool Whizzed;
    unsigned char OwnerPingTick;
    unsigned char HitBody;
    TVector2 HitSpot;
    unsigned char Tracking;
    unsigned char ImageStyle;
    TVector2 Initial;
    int StartUpTime, RicochetCount, DegradeCount;
    unsigned short Seed;
    TThingCollision *ThingCollisions;
    int ThingCollisionsCount;
    unsigned int SpriteCollisions;
    /* client branch */
    short PingAdd, PingAddStart;
} TBullet;

void TBullet_Update(TBullet *self);
void TBullet_Render(TBullet *self, double timeElapsed);
void TBullet_Kill(TBullet *self);
TVector2 TBullet_CheckMapCollision(TBullet *self, float x, float y);
TVector2 TBullet_CheckSpriteCollision(TBullet *self, float lasthitdist);
TVector2 TBullet_CheckThingCollision(TBullet *self, float lasthitdist);
TVector2 TBullet_CheckColliderCollision(TBullet *self, float lasthitdist);
void TBullet_Hit(TBullet *self, int t, int spriteHit, int where);
void TBullet_HitDefault(TBullet *self, int t);
void TBullet_ExplosionHit(TBullet *self, int typ, int spriteHit, int where);
void TBullet_CheckOutOfBounds(TBullet *self);
int TBullet_FilterSpritesByDistance(TBullet *self, TSpriteIndexes *spriteIndexes);
bool TBullet_TargetableSprite(TBullet *self, int i);
float TBullet_GetComparableSpriteDistance(TBullet *self, int i);
TVector2 TBullet_GetSpriteCollisionPoint(TBullet *self, int i);
unsigned char TBullet_GetWeaponIndex(TBullet *self);

#define HIT_TYPE_WALL 1
#define HIT_TYPE_BLOOD 2
#define HIT_TYPE_EXPLODE 3
#define HIT_TYPE_FRAGNADE 4
#define HIT_TYPE_THING 5
#define HIT_TYPE_CLUSTERNADE 6
#define HIT_TYPE_CLUSTER 7
#define HIT_TYPE_FLAK 8
#define HIT_TYPE_BODYHIT 9
#define HIT_TYPE_RICOCHET 10

int CreateBullet(TVector2 sPos, TVector2 sVelocity, unsigned char sNum,
    int sOwner, unsigned char n, float hitM, bool net, bool mustCreate,
    int seed);
int CreateBulletDefaultSeed(TVector2 sPos, TVector2 sVelocity,
    unsigned char sNum, int sOwner, unsigned char n, float hitM, bool net,
    bool mustCreate);
bool CanHitSpray(int victim, int attacker);
void CalculateRecoil(float px, float py, float *cx, float *cy, float da);
void HitSpray(void);
bool BulletCanSend(float x, float y, int i, float vX);

extern TBullet Bullet[MAX_BULLETS + 1];

#endif /* BULLETS_H */