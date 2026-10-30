/* Things — C port of shared/mechanics/Things.pas */
#include "Things.h"
#include "Parts.h"
#include "Anims.h"
#include "MapFile.h"
#include "PolyMap.h"
#include "Net.h"
#include "Weapons.h"
#include "Constants.h"
#include "Vector.h"
#include "Sprites.h"
#include "Game.h"
#include <stdlib.h>
#include <string.h>

extern TThing Thing[MAX_THINGS + 1];
extern int MySprite;

#define MAX_THINGS 90

TThingCollision ThingCollision(unsigned char thingNum, int cooldownEnd) {
    TThingCollision c = {thingNum, cooldownEnd};
    return c;
}

void TThing_Update(TThing *self) {
    (void)self;
}

void TThing_Render(TThing *self, double timeElapsed) {
    (void)self; (void)timeElapsed;
}

void TThing_PolygonsRender(TThing *self) {
    (void)self;
}

bool TThing_CheckMapCollision(TThing *self, int i, float x, float y) {
    (void)self; (void)i; (void)x; (void)y;
    return false;
}

void TThing_Kill(TThing *self) {
    if (!self) return;
    self->Active = false;
}

void TThing_CheckOutOfBounds(TThing *self) {
    (void)self;
}

void TThing_Respawn(TThing *self) {
    (void)self;
}

void TThing_MoveSkeleton(TThing *self, float x1, float y1, bool fromZero) {
    (void)self; (void)x1; (void)y1; (void)fromZero;
}

int TThing_CheckStationaryGunCollision(TThing *self, bool clientCheck) {
    (void)self; (void)clientCheck;
    return 0;
}

int CreateThing(TVector2 sPos, unsigned char owner, unsigned char sStyle, unsigned char n) {
    (void)sPos; (void)owner; (void)sStyle; (void)n;
    return 0;
}

bool SpawnBoxes(TVector2 *start, unsigned char team, unsigned char num) {
    (void)start; (void)team; (void)num;
    return false;
}

bool RandomizeStart(TVector2 *start, unsigned char team) {
    (void)start; (void)team;
    return false;
}