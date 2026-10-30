/* GfxData — C port of shared/gfx.inc */
#include "GfxData.h"
#include "PhysFS.h"
#include "Constants.h"
#include <stdlib.h>

TGfxSpriteArray Textures = {0};
int GFX_MAX_ID = 0;

void GfxData_Init(void) {
    /* Initialize graphics data */
}

int GfxData_GetMaxID(void) {
    return GFX_MAX_ID;
}