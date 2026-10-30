/* ClientGame — C port of client/ClientGame.pas */

#ifndef CLIENTGAME_H
#define CLIENTGAME_H

#include <SDL2/SDL.h>
#include "Constants.h"
#include "Vector.h"
#include <stdbool.h>

/* Unit-level globals (Pascal `var` section). */
extern TVector2 MousePrev;
extern float mx, my;
extern bool MapChanged;
extern bool ChatChanged;
extern bool ShouldRenderFrames;

extern unsigned char ActionSnap;
extern bool ActionSnapTaken;
extern int CapScreen;
extern bool ShowScreen;
extern unsigned char ScreenCounter;

extern bool IsFullscreen;
extern int ScreenWidth;
extern int ScreenHeight;
extern int RenderWidth;
extern int RenderHeight;
extern int WindowWidth;
extern int WindowHeight;

extern char *ChatText;
extern char *LastChatText;
extern char *FireChatText;
extern unsigned char ChatType;
extern unsigned char LastChatType;
extern unsigned char FireChatType;
extern char *CompletionBase;
extern int CompletionBaseSeparator;
extern unsigned char CurrentTabCompletePlayer;
extern unsigned char CursorPosition;
extern bool TabCompletePressed;
extern int ChatTimeCounter;

extern int ClientStopMovingCounter;
extern bool ForceClientSpriteSnapshotMov;
extern long LastForceClientSpriteSnapshotMovTick;
extern int MenuTimer;

void ClientGame_ResetFrameTiming(void);
void ClientGame_GameLoop(void);
int ClientGame_GetGameFps(void);
double ClientGame_GetCurrentTime(void);
void ClientGame_TabComplete(void);
void ClientGame_ResetWeaponStats(void);
void ClientGame_BigMessage(const char *text, int delay, unsigned int col);
unsigned char ClientGame_GetCameraTarget(void);
unsigned char ClientGame_GetCameraTargetBackwards(bool backwards);

#endif /* CLIENTGAME_H */