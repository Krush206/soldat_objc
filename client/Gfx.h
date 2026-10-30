/* Gfx — C port of client/Gfx.pas (minimal C-compatible header) */

#ifndef GFX_H
#define GFX_H

#include <SDL2/SDL.h>
#include "Vector.h"
#include <stdbool.h>

#define GFX_MONOCHROME 1

typedef enum {
    GFX_CLAMP = 0,
    GFX_REPEAT
} TGfxTextureWrap;

typedef enum {
    GFX_TOP = 0,
    GFX_BASELINE,
    GFX_BOTTOM
} TGfxVerticalAlign;

typedef enum {
    GFX_LINEAR = 0,
    GFX_NEAREST,
    GFX_MIPMAP_LINEAR,
    GFX_MIPMAP_NEAREST
} TGfxTextureFilter;

typedef struct TGfxMat3 {
    float m[9];
} TGfxMat3;

typedef struct TGfxColor {
    union {
        struct { unsigned char r, g, b, a; };
        unsigned int rgba;
    };
} TGfxColor;
typedef TGfxColor *PGfxColor;

typedef struct TGfxVertex {
    float x, y;
    float u, v;
    TGfxColor Color;
} TGfxVertex;
typedef TGfxVertex *PGfxVertex;

typedef struct TGfxRect {
    float Left, Right;
    float Top, Bottom;
} TGfxRect;
typedef TGfxRect *PGfxRect;

typedef void *TGfxFont;

typedef struct TGfxSprite {
    int x, y;
    int Width, Height;
    float Scale;
    int Delay;
    TGfxRect TexCoords;
    void *Texture;
    struct TGfxSprite *Next;
} TGfxSprite;
typedef TGfxSprite *PGfxSprite;

typedef struct {
    PGfxSprite *items;
    int count;
} TGfxSpriteArray;

typedef struct TGfxDrawCommand {
    void *Texture;
    int Offset;
    int Count;
} TGfxDrawCommand;
typedef TGfxDrawCommand *PGfxDrawCommand;

extern void (*GfxLog)(const char *s);

bool GfxFramebufferSupported(void);
bool GfxInitContext(void *wnd, bool dithering, bool fixed_pipeline);
void GfxDestroyContext(void);
void GfxPresent(bool finish);
void GfxTarget(void *render_target);
void GfxBlit(void *src, void *dst, SDL_Rect srcrect, SDL_Rect dstrect, int filter);
void GfxClear_rgba(unsigned char r, unsigned char g, unsigned char b, unsigned char a);
void GfxClear_color(TGfxColor c);
void GfxDraw_buffer_offset_count(void *buffer, int offset, int count);
void GfxDraw_buffer_index_offset_count(void *buffer, void *index_buffer, int offset, int count);
void GfxDraw_buffer_cmds(void *buffer, PGfxDrawCommand cmds, int cmd_count);
void GfxDraw_buffer_index_cmds(void *buffer, void *index_buffer, PGfxDrawCommand cmds, int cmd_count);
void GfxViewport(int x, int y, int w, int h);
void GfxTransform(const TGfxMat3 *t);
void GfxSpriteVertices(PGfxSprite s, float x, float y, float w, float h, float sx, float sy, float cx, float cy, float r, TGfxColor color, PGfxVertex v);
void GfxSaveScreen(const char *filename, int x, int y, int w, int h, bool async);
void GfxSetMipmapBias(float bias);

TGfxColor ARGB(unsigned int argb);
TGfxColor RGBA_rgb(unsigned char r, unsigned char g, unsigned char b);
TGfxColor RGBA_rgba(unsigned char r, unsigned char g, unsigned char b, unsigned char a);
TGfxColor RGBA_int(unsigned int rgba);
TGfxColor RGBA_rgb_ext(unsigned char r, unsigned char g, unsigned char b, float a);
TGfxColor RGBA_int_ext(unsigned int rgb, float a);
TGfxVertex GfxVertex_make(float x, float y, float u, float v, TGfxColor c);

void *GfxCreateTexture(int w, int h, int c, void *data);
void *GfxCreateRenderTarget(int w, int h, int c, bool msaa);
void GfxBindTexture(void *texture);
void GfxTextureWrap(void *texture, int s, int t);
void GfxTextureFilter(void *texture, int min, int mag);
void GfxUpdateTexture(void *texture, int x, int y, int w, int h, void *data);
void GfxGenerateMipmap(void *texture);
void GfxDeleteTexture(void **texture);

void *GfxCreateBuffer(int capacity, bool static_, PGfxVertex data);
void GfxUpdateBuffer(void *b, int i, int n, PGfxVertex data);
void GfxDeleteBuffer(void **b);

void *GfxCreateIndexBuffer(int capacity, bool static_, unsigned short *data);
void GfxUpdateIndexBuffer(void *b, int i, int n, unsigned short *data);
void GfxDeleteIndexBuffer(void **b);

void *GfxCreateFont(const char *filename, int w, int h);
void GfxDeleteFont(void **font);
int GfxSetFont(void *font, float font_size, unsigned int flags, float stretch);
void GfxSetFontTable(void *font, int table_index);
void GfxTextPixelRatio(void *pixel_ratio);
void GfxTextScale(float s);
void GfxTextColor(TGfxColor color);
void GfxTextShadow(float dx, float dy, TGfxColor color);
void GfxTextVerticalAlign(int align);
void GfxTextMetrics(void);
void GfxTextMetrics_text(const char *text);
void GfxDrawText_xy(float x, float y);
void GfxDrawText_text_xy(const char *text, float x, float y);
void GfxDrawText_ansi_xy(const char *text, float x, float y);

void GfxBegin(void);
void GfxEnd(void);
void GfxDrawQuad_texture_vertices(void *texture, PGfxVertex vertices);
void GfxDrawQuad_texture_abcd(void *texture, PGfxVertex a, PGfxVertex b, PGfxVertex c, PGfxVertex d);

void GfxDrawSprite_sprite_xy(PGfxSprite s, float x, float y);
void GfxDrawSprite_sprite_xy_scale(PGfxSprite s, float x, float y, float scale);
void GfxDrawSprite_sprite_xy_sx_sy(PGfxSprite s, float x, float y, float sx, float sy);
void GfxDrawSprite_sprite_xy_rx_ry_r(PGfxSprite s, float x, float y, float rx, float ry, float r);
void GfxDrawSprite_sprite_xy_sx_sy_rx_ry_r(PGfxSprite s, float x, float y, float sx, float sy, float rx, float ry, float r);
void GfxDrawSprite_sprite_xy_rc(PGfxSprite s, float x, float y, TGfxRect rc);
void GfxDrawSprite_sprite_xy_scale_rc(PGfxSprite s, float x, float y, float scale, TGfxRect rc);
void GfxDrawSprite_sprite_xy_sx_sy_rc(PGfxSprite s, float x, float y, float sx, float sy, TGfxRect rc);
void GfxDrawSprite_sprite_xy_rx_ry_r_rc(PGfxSprite s, float x, float y, float rx, float ry, float r, TGfxRect rc);
void GfxDrawSprite_sprite_xy_sx_sy_rx_ry_r_rc(PGfxSprite s, float x, float y, float sx, float sy, float rx, float ry, float r, TGfxRect rc);
void GfxDrawSprite_sprite_xy_color(PGfxSprite s, float x, float y, TGfxColor color);
void GfxDrawSprite_sprite_xy_scale_color(PGfxSprite s, float x, float y, float scale, TGfxColor color);
void GfxDrawSprite_sprite_xy_sx_sy_color(PGfxSprite s, float x, float y, float sx, float sy, TGfxColor color);
void GfxDrawSprite_sprite_xy_rx_ry_r_color(PGfxSprite s, float x, float y, float rx, float ry, float r, TGfxColor color);
void GfxDrawSprite_sprite_xy_sx_sy_rx_ry_r_color(PGfxSprite s, float x, float y, float sx, float sy, float rx, float ry, float r, TGfxColor color);
void GfxDrawSprite_sprite_xy_color_rc(PGfxSprite s, float x, float y, TGfxColor color, TGfxRect rc);
void GfxDrawSprite_sprite_xy_scale_color_rc(PGfxSprite s, float x, float y, float scale, TGfxColor color, TGfxRect rc);
void GfxDrawSprite_sprite_xy_sx_sy_color_rc(PGfxSprite s, float x, float y, float sx, float sy, TGfxColor color, TGfxRect rc);
void GfxDrawSprite_sprite_xy_rx_ry_r_color_rc(PGfxSprite s, float x, float y, float rx, float ry, float r, TGfxColor color, TGfxRect rc);
void GfxDrawSprite_sprite_xy_sx_sy_rx_ry_r_color_rc(PGfxSprite s, float x, float y, float sx, float sy, float rx, float ry, float r, TGfxColor color, TGfxRect rc);

void GfxMat3Rot_out(TGfxMat3 *out, float r);
void GfxMat3Ortho_out(TGfxMat3 *out, float l, float r, float t, float b);
void GfxMat3Transform_out(TGfxMat3 *out, float tx, float ty, float sx, float sy, float cx, float cy, float r);
TVector2 GfxMat3Mul(const TGfxMat3 *m, float x, float y);

int Npot(int x);
float RectWidth(const void *rect);
float RectHeight(const void *rect);

#endif /* GFX_H */