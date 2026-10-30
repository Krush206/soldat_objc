/* GameStrings — C port of client/GameStrings.pas */

#ifndef GAMESTRINGS_H
#define GAMESTRINGS_H

#include "PhysFS.h"
#include <stdbool.h>

typedef void *TMOFile;

extern TMOFile TranslationFile;

bool GameStrings_InitTranslation(const char *filename);
const char *GameStrings_Get(const char *s);
void GameStrings_DeInitTranslation(void);

#endif /* GAMESTRINGS_H */