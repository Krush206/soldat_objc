/* Sparks — C port of shared/mechanics/Sparks.pas */
#include "Sparks.h"
#include "Parts.h"
#include "MapFile.h"
#include "PolyMap.h"
#include "Net.h"
#include "Constants.h"
#include "Vector.h"
#include "Sprites.h"
#include "Game.h"
#include "Sound.h"
#include <stdlib.h>
#include <string.h>

int SparksCount = 0;
extern int MySprite;
extern int CameraFollowSprite;
extern float CameraX, CameraY;
extern ParticleSystem SparkParts;

#define MAX_SPARKS 558

void TSpark_Update(TSpark *self) {
    if (!self || !self->Active) return;
    
    /* NONEULER_STYLE check would go here */
    /* SparkParts_DoEulerTimeStepFor(self->Num); */
    
    TSpark_CheckOutOfBounds(self);
    
    /* COLLIDABLE_STYLE check would go here */
    /* TSpark_CheckMapCollision(self, ...); */
    
    self->LifePrev = self->Life;
    if (self->Life > 0) self->Life--;
    if (self->Life == 0) {
        TSpark_Kill(self);
    }
}

void TSpark_Render(TSpark *self) {
    (void)self;
}

bool TSpark_CheckMapCollision(TSpark *self, float x, float y) {
    (void)self; (void)x; (void)y;
    return false;
}

void TSpark_Kill(TSpark *self) {
    if (!self) return;
    self->Active = false;
    self->Style = 0;
    self->Life = 0;
    self->LifePrev = 0;
    self->CollideCount = 0;
    if (SparksCount > 0) SparksCount--;
}

void TSpark_CheckOutOfBounds(TSpark *self) {
    (void)self;
}

int CreateSpark(TVector2 sPos, TVector2 sVelocity, unsigned char sStyle,
    unsigned char sOwner, int life) {
    (void)sPos; (void)sVelocity; (void)sStyle; (void)sOwner; (void)life;
    return 0;
}