/* ClientGame — C port of client/ClientGame.pas */
#include "ClientGame.h"
#include "Client.h"
#include "Game.h"
#include "Vector.h"
#include "Console.h"
#include "Constants.h"
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

TVector2 MousePrev = {0, 0};
float mx = 0.0f, my = 0.0f;
bool MapChanged = false;
bool ChatChanged = false;
bool ShouldRenderFrames = true;

unsigned char ActionSnap = 0;
bool ActionSnapTaken = false;
int CapScreen = 0;
bool ShowScreen = false;
unsigned char ScreenCounter = 0;

bool IsFullscreen = false;
int ScreenWidth = 800;
int ScreenHeight = 600;
int RenderWidth = 800;
int RenderHeight = 600;
int WindowWidth = 800;
int WindowHeight = 600;

char *ChatText = NULL;
char *LastChatText = NULL;
char *FireChatText = NULL;
unsigned char ChatType = 0;
unsigned char LastChatType = 0;
unsigned char FireChatType = 0;
char *CompletionBase = NULL;
int CompletionBaseSeparator = 0;
unsigned char CurrentTabCompletePlayer = 0;
unsigned char CursorPosition = 0;
bool TabCompletePressed = false;
int ChatTimeCounter = 0;

int ClientStopMovingCounter = 0;
bool ForceClientSpriteSnapshotMov = false;
long LastForceClientSpriteSnapshotMovTick = 0;
int MenuTimer = 0;

void ClientGame_ResetFrameTiming(void) {
}

void ClientGame_GameLoop(void) {
}

int ClientGame_GetGameFps(void) {
    return 60;
}

double ClientGame_GetCurrentTime(void) {
    return 0.0;
}

void ClientGame_TabComplete(void) {
}

void ClientGame_ResetWeaponStats(void) {
}

void ClientGame_BigMessage(const char *text, int delay, unsigned int col) {
    (void)text; (void)delay; (void)col;
}

unsigned char ClientGame_GetCameraTarget(void) {
    return 0;
}

unsigned char ClientGame_GetCameraTargetBackwards(bool backwards) {
    (void)backwards;
    return 0;
}