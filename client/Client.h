/* Client — C port of client/Client.pas (interface contract) */

#ifndef CLIENT_H
#define CLIENT_H

#include "Cvar.h"
#include "Weapons.h"
#include "Console.h"
#include "Vector.h"
#include "Classes.h"

struct TClientNetwork;
struct TDownloadThread;

typedef struct TWeaponStat {
    char *Name;
    unsigned int TextureID;
    unsigned int Shots, Hits, Kills, Headshots;
    unsigned char Accuracy;
} TWeaponStat;

extern bool GameLoopRun;
extern bool ProgReady;

extern char *JoinPassword;
extern char *JoinPort;
extern char *JoinIP;

extern bool WindowReady;
extern unsigned char Initing;
extern bool GraphicsInitialized;

extern char *BaseDirectory;
extern char *UserDirectory;

extern char *ModDir;
extern bool UsesServerMod;

extern char *ServerIP;
extern int ServerPort;

extern float Grav;
extern unsigned char Connection;

extern unsigned char WeaponActive[MAIN_WEAPONS + 1];
extern int WeaponsInGame;

extern unsigned char Trails;
extern unsigned char Spectator;

extern unsigned char PacketAdjusting;

extern bool LimboLock;
extern unsigned char SelTeam;

extern int MySprite;

extern struct TClientNetwork *UDP;

extern TConsole *MainConsole;
extern TConsole *BigConsole;
extern TConsole *KillConsole;

extern TWeaponStat WepStats[20 + 1];
extern unsigned char WepStatsNum;

extern char *GunDisplayName[16 + 1];

extern unsigned char GameThingTarget;
extern int GrenadeEffectTimer;

extern unsigned char BadMapIDCount;

extern char *ExeName;
extern bool AbnormalTerminate;

extern char *HWID;

extern unsigned short HitSprayCounter;
extern bool ScreenTaken;

extern int ShotDistanceShow;
extern float ShotDistance;
extern float ShotLife;
extern int ShotRicochet;

extern bool TargetMode;

extern bool MuteAll;

extern bool RedirectToServer;
extern char RedirectIP[256];
extern int RedirectPort;
extern char *RedirectMsg;

extern StringList *RadioMenu;
extern char RMenuState[2 + 1];
extern bool ShowRadioMenu;
extern unsigned char RadioCooldown;

extern TVector2 CameraPrev;
extern float CameraX, CameraY;
extern unsigned char CameraFollowSprite;

extern struct TDownloadThread *DownloadThread;

void Client_JoinServer(void);
void Client_StartGame(void);
void Client_ShutDown(void);
void Client_ExitToMenu(void);
void Client_RestartGraph(void);
void Client_ShowMessage(const char *message_text);
TConsole *Client_MainConsole(void);

#endif /* CLIENT_H */