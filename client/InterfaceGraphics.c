/* InterfaceGraphics — C port of client/InterfaceGraphics.pas */
#include "InterfaceGraphics.h"
#include "Vector.h"
#include "Sprites.h"
#include "Constants.h"
#include "Game.h"
#include <stdlib.h>
#include <string.h>

char *ChatMessage[MAX_SPRITES + 1];
bool ChatTeam[MAX_SPRITES + 1];
int ChatDelay[MAX_SPRITES + 1];

char *BigText[MAX_BIG_MESSAGES + 1];
int BigDelay[MAX_BIG_MESSAGES + 1];
int BigX[MAX_BIG_MESSAGES + 1];
float BigScale[MAX_BIG_MESSAGES + 1];
unsigned int BigColor[MAX_BIG_MESSAGES + 1];
float BigPosX[MAX_BIG_MESSAGES + 1], BigPosY[MAX_BIG_MESSAGES + 1];

char *WorldText[MAX_BIG_MESSAGES + 1];
int WorldDelay[MAX_BIG_MESSAGES + 1];
int WorldX[MAX_BIG_MESSAGES + 1];
float WorldScale[MAX_BIG_MESSAGES + 1];
long WorldColor[MAX_BIG_MESSAGES + 1];
float WorldPosX[MAX_BIG_MESSAGES + 1], WorldPosY[MAX_BIG_MESSAGES + 1];

char *CursorText = NULL;
int CursorTextLength = 0;
bool CursorFriendly = false;

bool FragsMenuShow = false, StatsMenuShow = false, ConInfoShow = false;
bool PlayerNamesShow = false, MiniMapShow = false, NoobShow = false;

unsigned char FragsScrollMax = 0;
unsigned char FragsScrollLev = 0;
TVector2 _RScala = {0, 0};
TVector2 _IScala = {0, 0};
int fragx = 0, fragy = 0;

TInterface Int = {0};
TInterfaceRelInfo relinfo = {0};
TVector2 PixelSize = {0, 0};
double ChatInputTime = 0.0;

TInterfaceAlign IntAlign = {0};

void InterfaceGraphics_LoadInterfaceArchives(const char *path, bool first_only) {
    (void)path; (void)first_only;
}

void InterfaceGraphics_LoadInterfaceArchivesPath(const char *path) {
    (void)path;
}

bool InterfaceGraphics_LoadInterfaceData(const char *interface_name) {
    (void)interface_name;
    return false;
}

void InterfaceGraphics_RenderInterface(float time_elapsed, float width, float height) {
    (void)time_elapsed; (void)width; (void)height;
}

void InterfaceGraphics_RenderActionSnapText(double t) {
    (void)t;
}

bool InterfaceGraphics_IsDefaultInterface(const char *interface_name) {
    (void)interface_name;
    return false;
}