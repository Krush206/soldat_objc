/* GameStrings — C port of client/GameStrings.pas */
#include "GameStrings.h"
#include "PhysFS.h"
#include <stdlib.h>

TMOFile TranslationFile = NULL;

bool GameStrings_InitTranslation(const char *filename) {
    (void)filename;
    return false;
}

const char *GameStrings_Get(const char *s) {
    (void)s;
    return s;
}

void GameStrings_DeInitTranslation(void) {
}