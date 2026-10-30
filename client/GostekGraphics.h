/* GostekGraphics — C port of client/GostekGraphics.pas */

#ifndef GOSTEKGRAPHICS_H
#define GOSTEKGRAPHICS_H

#include "Classes.h"
#include "Sprites.h"

void GostekGraphics_RenderGostek(TSprite *soldier);
void GostekGraphics_LoadGostekData(void *data);
void GostekGraphics_ApplyGostekConstraints(void);

#endif /* GOSTEKGRAPHICS_H */