/* MapGraphics — C port of client/MapGraphics.pas */

#ifndef MAPGRAPHICS_H
#define MAPGRAPHICS_H

#include "Gfx.h"
#include "MapFile.h"
#include "Vector.h"
#include "Util.h"

typedef struct TMapGraphics {
    char *Filename;
    struct TMapInfo MapInfo;
    bool BgForce;
    TMapColor BgForcedColor[2];
    void *VertexBuffer;
    void *IndexBuffer;
    void **Textures;
    int TexturesCount;
    void **EdgeTextures;
    int EdgeTexturesCount;
    void *Spritesheet;
    void **Animations;
    int AnimationsCount;
    void **AnimationsCmd;
    int AnimationsCmdCount;
    void *AnimationsBuffer;
    int AnimationsBufferCount;
    int *AnimDuration;
    int AnimDurationCount;
    void **Props[3];
    int PropsCount[3];
    void *Minimap;
    float MinimapScale;
    TVector2 MinimapOffset;
    void *BgColorTop, *BgColorBtm;
    int Background;
    int BackgroundCount;
    void **Edges[2];
    int EdgesCount[2];
    void **Polys[2];
    int PolysCount[2];
} TMapGraphics;

extern TMapGraphics MapGfx;

void MapGraphics_LoadMapGraphics(void *map_file, bool bg_force, void *bg_color_top, void *bg_color_btm);
void MapGraphics_DestroyMapGraphics(void);
void MapGraphics_UpdateProps(double t);
void MapGraphics_RenderProps(int level);
void MapGraphics_RenderMinimap(float x, float y, unsigned char alpha);
void MapGraphics_WorldToMinimap(float x, float y, float *ox, float *oy);
void MapGraphics_SetTextureFilter(void *texture, bool allow_mipmaps);

#endif /* MAPGRAPHICS_H */