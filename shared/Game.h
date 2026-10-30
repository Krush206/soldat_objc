/* Game — C port of shared/Game.pas */

#ifndef GAME_H
#define GAME_H

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include "Constants.h"
#include "Vector.h"
#include "Weapons.h"
#include "PolyMap.h"
#include "Anims.h"
#include "Sound.h"
#include "Console.h"
#include "GameMenus.h"
#include "Demo.h"
#include "Util.h"
#include "Bullets.h"
#include "Sparks.h"
#include "Things.h"
#include "Sprites.h"

/* --- Core types --- */

typedef struct {
    int Kills;
    int Deaths;
    int Flags;
    int PlayerNum;
    unsigned int Color;
} TKillSort;

/* --- Global game state --- */

/* Tick/frame counters */
extern int Ticks;
extern int TicksPerSecond;
extern int Frames;
extern int FramesPerSecond;
extern int TickTime;
extern int TickTimeLast;
extern int GOALTICKS;

/* Time/timing */
extern unsigned long long TimeInMil;
extern unsigned long long TimeInMilLast;
extern unsigned int TimePassed;
extern int Seconds;
extern int SecondsLast;

/* Bullet time */
extern int BulletTimeTimer;

/* Gravity */
extern float Grav;

/* Game dimensions */
extern int GameWidth;
extern int GameHeight;
extern float GameWidthHalf;
extern float GameHeightHalf;

/* Sprite/Player counts */
extern int PlayersNum;
extern unsigned char AliveNum;
extern unsigned char TeamAliveNum[5];
extern unsigned char TeamPlayersNum[5];

/* Game mode */
extern int sv_gamemode;

/* Client-side chat/IP */
extern char *JoinIP;
extern char *JoinPort;
extern char *UserDirectory;

/* Scores */
extern int TeamScore[6];
extern int TeamFlag[5];

/* Survival */
extern bool SurvivalEndRound;
extern bool WeaponsCleaned;
extern int CeaseFireTime;
extern int MapChangeTime;
extern int MapChangeCounter;
extern int TimeLimitCounter;
extern int StartHealth;
extern int TimeLeftSec;
extern int TimeLeftMin;

/* Weapon selection */
extern unsigned char WeaponSel[MAX_SPRITES + 1][MAIN_WEAPONS + 1];

/* Map info */
extern TMapInfo MapChange;
extern uint64_t MapChangeItemID;
extern TSHA1Digest MapChangeChecksum;
extern char *MapChangeName;
extern int MapIndex;

/* Waypoints */
extern TWaypoints BotPath;

/* Voting */
extern bool VoteActive;
extern unsigned char VoteType;
extern char *VoteTarget;
extern char *VoteStarter;
extern char *VoteReason;
extern int VoteTimeRemaining;
extern unsigned char VoteNumVotes;
extern unsigned char VoteMaxVotes;
extern bool VoteHasVoted[MAX_SPRITES + 1];
extern int VoteCooldown[MAX_SPRITES + 1];
extern bool VoteKickReasonType;

/* Sorted players */
extern TKillSort SortedPlayers[MAX_SPRITES + 1];

/* Sine wave counter */
extern float SinusCounter;

/* Map checksums */
extern TSHA1Digest GameModChecksum;
extern TSHA1Digest CustomModChecksum;
extern TSHA1Digest MapCheckSum;

/* Player weapons */
extern unsigned char WeaponSelArray[MAX_SPRITES + 1][MAIN_WEAPONS + 1];

/* Sprite parts / Animations */
extern TAnimation Run, Stand, RunBack;
extern TAnimation Jump, JumpSide, Roll, RollBack;
extern TAnimation Fall;
extern TAnimation Crouch, CrouchRun, CrouchRunBack;
extern TAnimation Reload, Throw, Recoil, Shotgun, Barret, SmallRecoil, AimRecoil, HandsUpRecoil;
extern TAnimation ClipIn, ClipOut, SlideBack;
extern TAnimation Change;
extern TAnimation ThrowWeapon, WeaponNone;
extern TAnimation Punch, ReloadBow, Melee;
extern TAnimation Cigar, Match, Smoke, Wipe, Groin, TakeOff, Victory, Piss, Mercy, Mercy2, Own;
extern TAnimation Prone, GetUp, ProneMove;
extern TAnimation Aim, HandsUpAim;

/* Old sprite positions for ping compensation */
extern TVector2 OldSpritePos[MAX_SPRITES + 1][MAX_OLDPOS + 1];

/* Camera (client-side) */
extern float CameraX, CameraY;

/* Map changed flag */
extern bool MapChanged;

/* Client menu/game state externs */
extern PGameMenu limbomenu;
extern bool FragsMenuShow, StatsMenuShow;
extern TConsole *MainConsole;

/* --- Function declarations --- */

void game_init(void);
void game_update(void);
void SortPlayers(void);

/* Timing */
void Number27Timing(void);

/* Bullet time */
void ToggleBulletTime(bool TurnOn, int Duration);

/* Visibility */
bool PointVisible(float X, float Y, int i);
bool PointVisible2(float X, float Y, int I);
bool IsPointOnScreen(TVector2 Point);

/* Game stats */
void UpdateGameStats(void);

/* Voting */
void StartVote(unsigned char StarterVote, unsigned char TypeVote, const char *TargetVote, const char *ReasonVote);
void StopVote(void);
void TimerVote(void);
#ifdef SERVER
void CountVote(unsigned char Voter);
#endif

/* Map change */
void ShowMapChangeScoreboard(void);
void ShowMapChangeScoreboardMap(const char *NextMap);
void ChangeMap(void);

/* Game mode */
bool IsTeamGame(void);

/* Client functions */
void ExitToMenu(void);
void JoinServer(void);
void NextMap(void);
void RenderGameInfo(const char *text);

#endif /* GAME_H */