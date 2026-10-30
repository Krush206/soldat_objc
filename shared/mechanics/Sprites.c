/* Sprites — C port of shared/mechanics/Sprites.pas */
#include "Sprites.h"
#include "Parts.h"
#include "Anims.h"
#include "MapFile.h"
#include "PolyMap.h"
#include "../network/Net.h"
#include "Weapons.h"
#include "Constants.h"
#include "Vector.h"
#include "SysUtils.h"
#include "Console.h"
#include "Game.h"
#include "Sparks.h"
#include "Sound.h"
#include "Demo.h"
#include "Bullets.h"
#include "PhysFS.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>

extern TSprite Sprite[MAX_SPRITES + 1];
extern int MySprite;
extern TPlayer **Players;
extern TMapFile Map;
extern int SpriteMapColCount;
extern bool wasReloading;

void TBackgroundState_BackgroundTest(TBackgroundState *self, unsigned short poly) {
    if (!self) return;
    self->BackgroundPoly = poly;
    self->BackgroundTestResult = true;
}

void TBackgroundState_BackgroundTestBigPolyCenter(TBackgroundState *self, TVector2 pos) {
    (void)self; (void)pos;
}

short TBackgroundState_BackgroundFindCurrentPoly(TBackgroundState *self, TVector2 pos) {
    (void)self; (void)pos;
    return 0;
}

void TBackgroundState_BackgroundTestPrepare(TBackgroundState *self) {
    (void)self;
}

void TBackgroundState_BackgroundTestReset(TBackgroundState *self) {
    if (!self) return;
    self->BackgroundStatus = 0;
    self->BackgroundPoly = 0;
    self->BackgroundTestResult = false;
}

void TSprite_Update(TSprite *self) {
    (void)self;
}

void TSprite_Kill(TSprite *self) {
    if (!self) return;
    self->Active = false;
    self->DeadMeat = true;
}

void TSprite_Die(TSprite *self, int how, int who, int where, int what, TVector2 impact) {
    (void)self; (void)how; (void)who; (void)where; (void)what; (void)impact;
}

int TSprite_DropWeapon(TSprite *self) {
    (void)self;
    return 0;
}

void TSprite_LegsApplyAnimation(TSprite *self, TAnimation anim, int curr) {
    (void)self; (void)anim; (void)curr;
}

void TSprite_BodyApplyAnimation(TSprite *self, TAnimation anim, int curr) {
    (void)self; (void)anim; (void)curr;
}

void TSprite_MoveSkeleton(TSprite *self, float x1, float y1, bool fromZero) {
    (void)self; (void)x1; (void)y1; (void)fromZero;
}

bool TSprite_CheckMapCollision(TSprite *self, float x, float y, int area) {
    (void)self; (void)x; (void)y; (void)area;
    return false;
}

bool TSprite_CheckRadiusMapCollision(TSprite *self, float x, float y, bool hasCollided) {
    (void)self; (void)x; (void)y; (void)hasCollided;
    return false;
}

bool TSprite_CheckMapVerticesCollision(TSprite *self, float x, float y, float r, bool hasCollided) {
    (void)self; (void)x; (void)y; (void)r; (void)hasCollided;
    return false;
}

bool TSprite_CheckSkeletonMapCollision(TSprite *self, int i, float x, float y) {
    (void)self; (void)i; (void)x; (void)y;
    return false;
}

void TSprite_HandleSpecialPolyTypes(TSprite *self, int polyType, TVector2 pos) {
    (void)self; (void)polyType; (void)pos;
}

void TSprite_ApplyWeaponByNum(TSprite *self, unsigned char wnum, unsigned char gun, int ammo, bool restorePrimaryState) {
    (void)self; (void)wnum; (void)gun; (void)ammo; (void)restorePrimaryState;
}

void TSprite_ApplyWeaponByNumDefault(TSprite *self, unsigned char wnum, unsigned char gun) {
    (void)self; (void)wnum; (void)gun;
}

void TSprite_HealthHit(TSprite *self, float amount, int who, int where, int what, TVector2 impact) {
    (void)self; (void)amount; (void)who; (void)where; (void)what; (void)impact;
}

void TSprite_FreeControls(TSprite *self) {
    if (!self) return;
    memset(&self->Control, 0, sizeof(TControl));
}

void TSprite_CheckOutOfBounds(TSprite *self) {
    (void)self;
}

void TSprite_CheckSkeletonOutOfBounds(TSprite *self) {
    (void)self;
}

void TSprite_Respawn(TSprite *self) {
    if (!self) return;
    self->Active = true;
    self->DeadMeat = false;
    self->Health = 150.0f;
    self->Vest = 0.0f;
    self->Position = POS_STAND;
}

void TSprite_Parachute(TSprite *self, TVector2 a) {
    (void)self; (void)a;
}

void TSprite_ChangeTeam(TSprite *self, int team) {
    if (!self || !self->Player) return;
    self->Player->Team = team;
}

void TSprite_Fire(TSprite *self) {
    (void)self;
}

void TSprite_ThrowFlag(TSprite *self) {
    (void)self;
}

void TSprite_ThrowGrenade(TSprite *self) {
    (void)self;
}

float TSprite_GetMoveacc(TSprite *self) {
    (void)self;
    return 0.0f;
}

TVector2 TSprite_GetCursorAimDirection(TSprite *self) {
    TVector2 v = {0, 0};
    (void)self;
    return v;
}

TVector2 TSprite_GetHandsAimDirection(TSprite *self) {
    TVector2 v = {0, 0};
    (void)self;
    return v;
}

bool TSprite_IsSolo(TSprite *self) {
    (void)self;
    return false;
}

bool TSprite_IsNotSolo(TSprite *self) {
    (void)self;
    return true;
}

bool TSprite_IsInTeam(TSprite *self) {
    (void)self;
    return false;
}

bool TSprite_IsSpectator(TSprite *self) {
    (void)self;
    return false;
}

bool TSprite_IsNotSpectator(TSprite *self) {
    (void)self;
    return true;
}

bool TSprite_IsInSameTeam(TSprite *self, TSprite *otherPlayer) {
    (void)self; (void)otherPlayer;
    return false;
}

bool TSprite_IsNotInSameTeam(TSprite *self, TSprite *otherPlayer) {
    (void)self; (void)otherPlayer;
    return true;
}

bool TSprite_CanRespawn(TSprite *self, bool deadMeatBeforeRespawn) {
    (void)self; (void)deadMeatBeforeRespawn;
    return false;
}

int CreateSprite(TVector2 sPos, TVector2 sVelocity, unsigned char sStyle, unsigned char n, TPlayer *player, bool transferOwnership) {
    (void)sPos; (void)sVelocity; (void)sStyle; (void)n; (void)player; (void)transferOwnership;
    return 0;
}

bool TeamCollides(int poly, int team, bool bullet) {
    (void)poly; (void)team; (void)bullet;
    return false;
}