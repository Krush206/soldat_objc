/* GameRendering — C port of client/GameRendering.pas */
#include "GameRendering.h"
#include "Constants.h"
#include <stdlib.h>
#include <stdbool.h>

TGameRenderingParams GameRenderingParams = {0};
extern TGfxSpriteArray Textures;

bool GameRendering_InitGameGraphics(void) {
    return false;
}

void GameRendering_ReloadGraphics(void) {
}

void GameRendering_DestroyGameGraphics(void) {
}

void GameRendering_RenderFrame(double time_elapsed, double frame_percent, bool paused) {
    (void)time_elapsed; (void)frame_percent; (void)paused;
}

void GameRendering_RenderGameInfo(const char *text_string) {
    (void)text_string;
}

bool GameRendering_DoTextureLoading(void) {
    return false;
}

bool GameRendering_DoTextureLoadingFinish(bool finish_loading) {
    (void)finish_loading;
    return false;
}

void GameRendering_SetFontStyle(int style) {
    (void)style;
}

void GameRendering_SetFontStyleScale(int style, float scale) {
    (void)style; (void)scale;
}

float GameRendering_FontStyleSize(int style) {
    (void)style;
    return 0.0f;
}

void GameRendering_TakeScreenshot(const char *filename) {
    (void)filename;
}

void GameRendering_TakeScreenshotAsync(const char *filename, bool async) {
    (void)filename; (void)async;
}

void GameRendering_FillCaseInsensitiveImageMap(void) {
}

const char *GameRendering_FindImagePath(const char *filename) {
    (void)filename;
    return NULL;
}

/* End of file */