/* Bullets — C port of shared/mechanics/Bullets.pas */
#include "Bullets.h"
#include "Parts.h"
#include "MapFile.h"
#include "PolyMap.h"
#include "Net.h"
#include "Weapons.h"
#include "Constants.h"
#include "Vector.h"
#include "Sprites.h"
#include "Things.h"
#include "Game.h"
#include "Sparks.h"
#include "Sound.h"
#include "PhysFS.h"
#include "Calc.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>

extern TBullet Bullet[MAX_BULLETS + 1];
extern int MySprite;

#define MAX_BULLETS 254

void TBullet_Update(TBullet *self) {
    (void)self;
}

void TBullet_Render(TBullet *self, double timeElapsed) {
    (void)self; (void)timeElapsed;
}

void TBullet_Kill(TBullet *self) {
    if (!self) return;
    self->Active = false;
    if (self->ThingCollisions) {
        free(self->ThingCollisions);
        self->ThingCollisions = NULL;
        self->ThingCollisionsCount = 0;
    }
}

TVector2 TBullet_CheckMapCollision(TBullet *self, float x, float y) {
    TVector2 v = {0, 0};
    (void)self; (void)x; (void)y;
    return v;
}

TVector2 TBullet_CheckSpriteCollision(TBullet *self, float lasthitdist) {
    TVector2 v = {0, 0};
    (void)self; (void)lasthitdist;
    return v;
}

TVector2 TBullet_CheckThingCollision(TBullet *self, float lasthitdist) {
    TVector2 v = {0, 0};
    (void)self; (void)lasthitdist;
    return v;
}

TVector2 TBullet_CheckColliderCollision(TBullet *self, float lasthitdist) {
    TVector2 v = {0, 0};
    (void)self; (void)lasthitdist;
    return v;
}

void TBullet_Hit(TBullet *self, int t, int spriteHit, int where) {
    (void)self; (void)t; (void)spriteHit; (void)where;
}

void TBullet_HitDefault(TBullet *self, int t) {
    (void)self; (void)t;
}

void TBullet_ExplosionHit(TBullet *self, int typ, int spriteHit, int where) {
    (void)self; (void)typ; (void)spriteHit; (void)where;
}

void TBullet_CheckOutOfBounds(TBullet *self) {
    (void)self;
}

int TBullet_FilterSpritesByDistance(TBullet *self, TSpriteIndexes *spriteIndexes) {
    (void)self; (void)spriteIndexes;
    return 0;
}

bool TBullet_TargetableSprite(TBullet *self, int i) {
    (void)self; (void)i;
    return false;
}

float TBullet_GetComparableSpriteDistance(TBullet *self, int i) {
    (void)self; (void)i;
    return 0.0f;
}

TVector2 TBullet_GetSpriteCollisionPoint(TBullet *self, int i) {
    TVector2 v = {0, 0};
    (void)self; (void)i;
    return v;
}

unsigned char TBullet_GetWeaponIndex(TBullet *self) {
    (void)self;
    return 0;
}

int CreateBullet(TVector2 sPos, TVector2 sVelocity, unsigned char sNum,
    int sOwner, unsigned char n, float hitM, bool net, bool mustCreate,
    int seed) {
    (void)sPos; (void)sVelocity; (void)sNum; (void)sOwner; (void)n;
    (void)hitM; (void)net; (void)mustCreate; (void)seed;
    return 0;
}

int CreateBulletDefaultSeed(TVector2 sPos, TVector2 sVelocity,
    unsigned char sNum, int sOwner, unsigned char n, float hitM, bool net,
    bool mustCreate) {
    return CreateBullet(sPos, sVelocity, sNum, sOwner, n, hitM, net, mustCreate, -1);
}

bool CanHitSpray(int victim, int attacker) {
    (void)victim; (void)attacker;
    return false;
}

void CalculateRecoil(float px, float py, float *cx, float *cy, float da) {
    (void)px; (void)py; (void)cx; (void)cy; (void)da;
}

void HitSpray(void) {
}

bool BulletCanSend(float x, float y, int i, float vX) {
    (void)x; (void)y; (void)i; (void)vX;
    return false;
}