/* Gfx — C port of client/Gfx.pas (minimal stub implementation) */
#include "Gfx.h"
#include "Vector.h"
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>

void (*GfxLog)(const char *s) = NULL;

bool GfxFramebufferSupported(void) { return false; }
bool GfxInitContext(void *wnd, bool dithering, bool fixed_pipeline) { (void)wnd; (void)dithering; (void)fixed_pipeline; return false; }
void GfxDestroyContext(void) {}
void GfxPresent(bool finish) { (void)finish; }
void GfxTarget(void *render_target) { (void)render_target; }
void GfxBlit(void *src, void *dst, SDL_Rect srcrect, SDL_Rect dstrect, int filter) { (void)src; (void)dst; (void)srcrect; (void)dstrect; (void)filter; }
void GfxClear_rgba(unsigned char r, unsigned char g, unsigned char b, unsigned char a) { (void)r; (void)g; (void)b; (void)a; }
void GfxClear_color(TGfxColor c) { (void)c; }
void GfxDraw_buffer_offset_count(void *buffer, int offset, int count) { (void)buffer; (void)offset; (void)count; }
void GfxDraw_buffer_index_offset_count(void *buffer, void *index_buffer, int offset, int count) { (void)buffer; (void)index_buffer; (void)offset; (void)count; }
void GfxDraw_buffer_cmds(void *buffer, PGfxDrawCommand cmds, int cmd_count) { (void)buffer; (void)cmds; (void)cmd_count; }
void GfxDraw_buffer_index_cmds(void *buffer, void *index_buffer, PGfxDrawCommand cmds, int cmd_count) { (void)buffer; (void)index_buffer; (void)cmds; (void)cmd_count; }
void GfxViewport(int x, int y, int w, int h) { (void)x; (void)y; (void)w; (void)h; }
void GfxTransform(const TGfxMat3 *t) { (void)t; }
void GfxSpriteVertices(PGfxSprite s, float x, float y, float w, float h, float sx, float sy, float cx, float cy, float r, TGfxColor color, PGfxVertex v) { (void)s; (void)x; (void)y; (void)w; (void)h; (void)sx; (void)sy; (void)cx; (void)cy; (void)r; (void)color; (void)v; }
void GfxSaveScreen(const char *filename, int x, int y, int w, int h, bool async) { (void)filename; (void)x; (void)y; (void)w; (void)h; (void)async; }
void GfxSetMipmapBias(float bias) { (void)bias; }
void *GfxCreateTexture(int w, int h, int c, void *data) { (void)w; (void)h; (void)c; (void)data; return NULL; }
void *GfxCreateRenderTarget(int w, int h, int c, bool msaa) { (void)w; (void)h; (void)c; (void)msaa; return NULL; }
void GfxBindTexture(void *texture) { (void)texture; }
void GfxTextureWrap(void *texture, int s, int t) { (void)texture; (void)s; (void)t; }
void GfxTextureFilter(void *texture, int min, int mag) { (void)texture; (void)min; (void)mag; }
void GfxUpdateTexture(void *texture, int x, int y, int w, int h, void *data) { (void)texture; (void)x; (void)y; (void)w; (void)h; (void)data; }
void GfxGenerateMipmap(void *texture) { (void)texture; }
void GfxDeleteTexture(void **texture) { (void)texture; }
void *GfxCreateBuffer(int capacity, bool static_, PGfxVertex data) { (void)capacity; (void)static_; (void)data; return NULL; }
void GfxUpdateBuffer(void *b, int i, int n, PGfxVertex data) { (void)b; (void)i; (void)n; (void)data; }
void GfxDeleteBuffer(void **b) { (void)b; }
void *GfxCreateIndexBuffer(int capacity, bool static_, unsigned short *data) { (void)capacity; (void)static_; (void)data; return NULL; }
void GfxUpdateIndexBuffer(void *b, int i, int n, unsigned short *data) { (void)b; (void)i; (void)n; (void)data; }
void GfxDeleteIndexBuffer(void **b) { (void)b; }
void *GfxCreateFont(const char *filename, int w, int h) { (void)filename; (void)w; (void)h; return NULL; }
void GfxDeleteFont(void **font) { (void)font; }
int GfxSetFont(void *font, float font_size, unsigned int flags, float stretch) { (void)font; (void)font_size; (void)flags; (void)stretch; return 0; }
void GfxSetFontTable(void *font, int table_index) { (void)font; (void)table_index; }
void GfxTextPixelRatio(void *pixel_ratio) { (void)pixel_ratio; }
void GfxTextScale(float s) { (void)s; }
void GfxTextColor(TGfxColor color) { (void)color; }
void GfxTextShadow(float dx, float dy, TGfxColor color) { (void)dx; (void)dy; (void)color; }
void GfxTextVerticalAlign(int align) { (void)align; }
void GfxTextMetrics(void) {}
void GfxTextMetrics_text(const char *text) { (void)text; }
void GfxDrawText_xy(float x, float y) { (void)x; (void)y; }
void GfxDrawText_text_xy(const char *text, float x, float y) { (void)text; (void)x; (void)y; }
void GfxDrawText_ansi_xy(const char *text, float x, float y) { (void)text; (void)x; (void)y; }
void GfxBegin(void) {}
void GfxEnd(void) {}
void GfxDrawQuad_texture_vertices(void *texture, PGfxVertex vertices) { (void)texture; (void)vertices; }
void GfxDrawQuad_texture_abcd(void *texture, PGfxVertex a, PGfxVertex b, PGfxVertex c, PGfxVertex d) { (void)texture; (void)a; (void)b; (void)c; (void)d; }
void GfxDrawSprite_sprite_xy(PGfxSprite s, float x, float y) { (void)s; (void)x; (void)y; }
void GfxDrawSprite_sprite_xy_scale(PGfxSprite s, float x, float y, float scale) { (void)s; (void)x; (void)y; (void)scale; }
void GfxDrawSprite_sprite_xy_sx_sy(PGfxSprite s, float x, float y, float sx, float sy) { (void)s; (void)x; (void)y; (void)sx; (void)sy; }
void GfxDrawSprite_sprite_xy_rx_ry_r(PGfxSprite s, float x, float y, float rx, float ry, float r) { (void)s; (void)x; (void)y; (void)rx; (void)ry; (void)r; }
void GfxDrawSprite_sprite_xy_sx_sy_rx_ry_r(PGfxSprite s, float x, float y, float sx, float sy, float rx, float ry, float r) { (void)s; (void)x; (void)y; (void)sx; (void)sy; (void)rx; (void)ry; (void)r; }
void GfxDrawSprite_sprite_xy_rc(PGfxSprite s, float x, float y, TGfxRect rc) { (void)s; (void)x; (void)y; (void)rc; }
void GfxDrawSprite_sprite_xy_scale_rc(PGfxSprite s, float x, float y, float scale, TGfxRect rc) { (void)s; (void)x; (void)y; (void)scale; (void)rc; }
void GfxDrawSprite_sprite_xy_sx_sy_rc(PGfxSprite s, float x, float y, float sx, float sy, TGfxRect rc) { (void)s; (void)x; (void)y; (void)sx; (void)sy; (void)rc; }
void GfxDrawSprite_sprite_xy_rx_ry_r_rc(PGfxSprite s, float x, float y, float rx, float ry, float r, TGfxRect rc) { (void)s; (void)x; (void)y; (void)rx; (void)ry; (void)r; (void)rc; }
void GfxDrawSprite_sprite_xy_sx_sy_rx_ry_r_rc(PGfxSprite s, float x, float y, float sx, float sy, float rx, float ry, float r, TGfxRect rc) { (void)s; (void)x; (void)y; (void)sx; (void)sy; (void)rx; (void)ry; (void)r; (void)rc; }
void GfxDrawSprite_sprite_xy_color(PGfxSprite s, float x, float y, TGfxColor color) { (void)s; (void)x; (void)y; (void)color; }
void GfxDrawSprite_sprite_xy_scale_color(PGfxSprite s, float x, float y, float scale, TGfxColor color) { (void)s; (void)x; (void)y; (void)scale; (void)color; }
void GfxDrawSprite_sprite_xy_sx_sy_color(PGfxSprite s, float x, float y, float sx, float sy, TGfxColor color) { (void)s; (void)x; (void)y; (void)sx; (void)sy; (void)color; }
void GfxDrawSprite_sprite_xy_rx_ry_r_color(PGfxSprite s, float x, float y, float rx, float ry, float r, TGfxColor color) { (void)s; (void)x; (void)y; (void)rx; (void)ry; (void)r; (void)color; }
void GfxDrawSprite_sprite_xy_sx_sy_rx_ry_r_color(PGfxSprite s, float x, float y, float sx, float sy, float rx, float ry, float r, TGfxColor color) { (void)s; (void)x; (void)y; (void)sx; (void)sy; (void)rx; (void)ry; (void)r; (void)color; }
void GfxDrawSprite_sprite_xy_color_rc(PGfxSprite s, float x, float y, TGfxColor color, TGfxRect rc) { (void)s; (void)x; (void)y; (void)color; (void)rc; }
void GfxDrawSprite_sprite_xy_scale_color_rc(PGfxSprite s, float x, float y, float scale, TGfxColor color, TGfxRect rc) { (void)s; (void)x; (void)y; (void)scale; (void)color; (void)rc; }
void GfxDrawSprite_sprite_xy_sx_sy_color_rc(PGfxSprite s, float x, float y, float sx, float sy, TGfxColor color, TGfxRect rc) { (void)s; (void)x; (void)y; (void)sx; (void)sy; (void)color; (void)rc; }
void GfxDrawSprite_sprite_xy_rx_ry_r_color_rc(PGfxSprite s, float x, float y, float rx, float ry, float r, TGfxColor color, TGfxRect rc) { (void)s; (void)x; (void)y; (void)rx; (void)ry; (void)r; (void)color; (void)rc; }
void GfxDrawSprite_sprite_xy_sx_sy_rx_ry_r_color_rc(PGfxSprite s, float x, float y, float sx, float sy, float rx, float ry, float r, TGfxColor color, TGfxRect rc) { (void)s; (void)x; (void)y; (void)sx; (void)sy; (void)rx; (void)ry; (void)r; (void)color; (void)rc; }

void GfxMat3Rot_out(TGfxMat3 *out, float r) { (void)out; (void)r; }
void GfxMat3Ortho_out(TGfxMat3 *out, float l, float r, float t, float b) { (void)out; (void)l; (void)r; (void)t; (void)b; }
void GfxMat3Transform_out(TGfxMat3 *out, float tx, float ty, float sx, float sy, float cx, float cy, float r) { (void)out; (void)tx; (void)ty; (void)sx; (void)sy; (void)cx; (void)cy; (void)r; }
TVector2 GfxMat3Mul(const TGfxMat3 *m, float x, float y) { (void)m; (void)x; (void)y; return (TVector2){0, 0}; }

int Npot(int x) { return 1; }
float RectWidth(const void *rect) { (void)rect; return 0; }
float RectHeight(const void *rect) { (void)rect; return 0; }