/* GameRendering — C port of client/GameRendering.pas */

#ifndef GAMERENDERING_H
#define GAMERENDERING_H

#include <SDL2/SDL.h>
#include <stdbool.h>
#include "Gfx.h"

#define FONT_BIG 0
#define FONT_SMALL 1
#define FONT_SMALL_BOLD 2
#define FONT_SMALLEST 3
#define FONT_MENU 4
#define FONT_WEAPONS_MENU 5
#define FONT_WORLD 6
#define FONT_LAST FONT_WORLD

typedef struct TGameRenderingParams {
    char *InterfaceName;
} TGameRenderingParams;

extern TGameRenderingParams GameRenderingParams;
extern TGfxSpriteArray Textures;

bool GameRendering_InitGameGraphics(void);
void GameRendering_ReloadGraphics(void);
void GameRendering_DestroyGameGraphics(void);
void GameRendering_RenderFrame(double time_elapsed, double frame_percent, bool paused);
void GameRendering_RenderGameInfo(const char *text_string);
bool GameRendering_DoTextureLoading(void);
bool GameRendering_DoTextureLoadingFinish(bool finish_loading);
void GameRendering_SetFontStyle(int style);
void GameRendering_SetFontStyleScale(int style, float scale);
float GameRendering_FontStyleSize(int style);
void GameRendering_TakeScreenshot(const char *filename);
void GameRendering_TakeScreenshotAsync(const char *filename, bool async);
void GameRendering_FillCaseInsensitiveImageMap(void);
const char *GameRendering_FindImagePath(const char *filename);

#endif /* GAMERENDERING_H */