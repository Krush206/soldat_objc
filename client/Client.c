/* Client — C port of client/Client.pas */
#include "Client.h"
#include "Cvar.h"
#include "Weapons.h"
#include "Console.h"
#include "Vector.h"
#include "Classes.h"
#include "Game.h"
#include "Constants.h"
#include <stdlib.h>
#include <string.h>

bool GameLoopRun = false;
bool ProgReady = false;

char *JoinPassword = NULL;
char *JoinPort = NULL;
char *JoinIP = NULL;

bool WindowReady = false;
unsigned char Initing = 0;
bool GraphicsInitialized = false;

char *BaseDirectory = NULL;
char *UserDirectory = NULL;

char *ModDir = NULL;
bool UsesServerMod = false;

char *ServerIP = NULL;
int ServerPort = 0;

float Grav = 0.0f;
unsigned char Connection = 0;

unsigned char WeaponActive[MAIN_WEAPONS + 1] = {0};
int WeaponsInGame = 0;

unsigned char Trails = 0;
unsigned char Spectator = 0;

unsigned char PacketAdjusting = 0;

bool LimboLock = false;
unsigned char SelTeam = 0;

int MySprite = 0;

struct TClientNetwork *UDP = NULL;

TConsole *MainConsole = NULL;
TConsole *BigConsole = NULL;
TConsole *KillConsole = NULL;

TWeaponStat WepStats[20 + 1] = {0};
unsigned char WepStatsNum = 0;

char *GunDisplayName[16 + 1] = {0};

unsigned char GameThingTarget = 0;
int GrenadeEffectTimer = 0;

unsigned char BadMapIDCount = 0;

char *ExeName = NULL;
bool AbnormalTerminate = false;

char *HWID = NULL;

unsigned short HitSprayCounter = 0;
bool ScreenTaken = false;

int ShotDistanceShow = 0;
float ShotDistance = 0.0f;
float ShotLife = 0.0f;
int ShotRicochet = 0;

bool TargetMode = false;

bool MuteAll = false;

bool RedirectToServer = false;
char RedirectIP[256] = {0};
int RedirectPort = 0;
char *RedirectMsg = NULL;

StringList *RadioMenu = NULL;
char RMenuState[2 + 1] = {0};
bool ShowRadioMenu = false;
unsigned char RadioCooldown = 0;

TVector2 CameraPrev = {0, 0};
float CameraX = 0.0f, CameraY = 0.0f;
unsigned char CameraFollowSprite = 0;

struct TDownloadThread *DownloadThread = NULL;

void Client_JoinServer(void) {
}

void Client_StartGame(void) {
}

void Client_ShutDown(void) {
}

void Client_ExitToMenu(void) {
}

void Client_RestartGraph(void) {
}

void Client_ShowMessage(const char *message_text) {
    (void)message_text;
}

TConsole *Client_MainConsole(void) {
    return MainConsole;
}