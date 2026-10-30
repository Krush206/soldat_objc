/* MapGraphics — C port of client/MapGraphics.pas */
#include "MapGraphics.h"
#include "Gfx.h"
#include "MapFile.h"
#include "Vector.h"
#include "Util.h"
#include <stdlib.h>

TMapGraphics MapGfx = {0};

void MapGraphics_LoadMapGraphics(void *map_file, bool bg_force, void *bg_color_top, void *bg_color_btm) {
    (void)map_file; (void)bg_force; (void)bg_color_top; (void)bg_color_btm;
}

void MapGraphics_DestroyMapGraphics(void) {
}

void MapGraphics_UpdateProps(double t) {
    (void)t;
}

void MapGraphics_RenderProps(int level) {
    (void)level;
}

void MapGraphics_RenderMinimap(float x, float y, unsigned char alpha) {
    (void)x; (void)y; (void)alpha;
}

void MapGraphics_WorldToMinimap(float x, float y, float *ox, float *oy) {
    (void)x; (void)y; (void)ox; (void)oy;
}

void MapGraphics_SetTextureFilter(void *texture, bool allow_mipmaps) {
    (void)texture; (void)allow_mipmaps;
}