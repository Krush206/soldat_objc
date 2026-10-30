/* Game — C port of shared/Game.pas */

#include "Game.h"

#include "Constants.h"
#include "Vector.h"
#include "Weapons.h"
#include "PolyMap.h"
#include "Anims.h"
#include "Sound.h"
#include "GameMenus.h"
#include "GameStrings.h"
#include "InterfaceGraphics.h"
#include "GameRendering.h"
#include "Console.h"
#include "Util.h"
#include "Cvar.h"
#include "PhysFS.h"
#include "TraceLog.h"
#include "Demo.h"
#include "Net.h"
#include "NetworkClientGame.h"
#include "Client.h"
#include "Bullets.h"
#include "Sparks.h"
#include "Things.h"
#include "Sprites.h"
#include "Parts.h"
#include "Waypoints.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <time.h>

/* --- Global array definitions (declared extern in headers) --- */
TSprite Sprite[MAX_SPRITES + 1] = {{0}};
TSpark Spark[MAX_SPARKS + 1] = {{0}};
TBullet Bullet[MAX_BULLETS + 1] = {{0}};
TThing Thing[MAX_THINGS + 1] = {{0}};
ParticleSystem SpriteParts = {0};
ParticleSystem BulletParts = {0};
ParticleSystem SparkParts = {0};
struct TDemoRecorder *DemoRecorder = NULL;

/* --- Global game state definitions --- */

/* Tick/frame counters */
int Ticks = 0;
int TicksPerSecond = 0;
int Frames = 0;
int FramesPerSecond = 0;
int TickTime = 0;
int TickTimeLast = 0;
int GOALTICKS = DEFAULT_GOALTICKS;

/* Time/timing */
unsigned long long TimeInMil = 0;
unsigned long long TimeInMilLast = 0;
unsigned int TimePassed = 0;
int Seconds = 0;
int SecondsLast = 0;

/* Bullet time */
int BulletTimeTimer = 0;

/* Gravity */
extern float Grav;

/* Game dimensions */
int GameWidth = DEFAULT_WIDTH;
int GameHeight = DEFAULT_HEIGHT;
float GameWidthHalf = DEFAULT_WIDTH / 2.0f;
float GameHeightHalf = DEFAULT_HEIGHT / 2.0f;

/* Sprite/Player counts */
extern int PlayersNum;
extern int MySprite;
unsigned char AliveNum = 0;
unsigned char TeamAliveNum[5] = {0};
unsigned char TeamPlayersNum[5] = {0};

/* Game mode */
int sv_gamemode = GAMESTYLE_DEATHMATCH;

/* Client-side chat/IP */
extern char *JoinIP;
extern char *JoinPort;
extern char *UserDirectory;

/* Scores */
int TeamScore[6] = {0};
int TeamFlag[5] = {0};

/* Survival */
bool SurvivalEndRound = false;
bool WeaponsCleaned = false;
int CeaseFireTime = DEFAULT_CEASEFIRE_TIME;
int MapChangeTime = DEFAULT_MAPCHANGE_TIME;
int MapChangeCounter = 0;
int TimeLimitCounter = 3600;
int StartHealth = 150;
int TimeLeftSec = 0;
int TimeLeftMin = 0;

/* Weapon selection */
unsigned char WeaponSel[MAX_SPRITES + 1][MAIN_WEAPONS + 1] = {{0}};

/* Map info */
TPolyMap Map = {0};
TMapInfo MapChange = {NULL, NULL, 0, NULL};
uint64_t MapChangeItemID = 0;
TSHA1Digest MapChangeChecksum = {0};
char *MapChangeName = NULL;
int MapIndex = 0;

/* Waypoints */
TWaypoints BotPath = {0};

/* Voting */
bool VoteActive = false;
unsigned char VoteType = 0;
char *VoteTarget = NULL;
char *VoteStarter = NULL;
char *VoteReason = NULL;
int VoteTimeRemaining = -1;
unsigned char VoteNumVotes = 0;
unsigned char VoteMaxVotes = 0;
bool VoteHasVoted[MAX_SPRITES + 1] = {false};
int VoteCooldown[MAX_SPRITES + 1] = {0};
bool VoteKickReasonType = false;

/* Sorted players */
TKillSort SortedPlayers[MAX_SPRITES + 1] = {{0}};

/* Sine wave counter */
float SinusCounter = 0.0f;

/* Map checksums */
TSHA1Digest GameModChecksum = {0};
TSHA1Digest CustomModChecksum = {0};
TSHA1Digest MapCheckSum = {0};

/* Player weapons */
unsigned char WeaponSelArray[MAX_SPRITES + 1][MAIN_WEAPONS + 1] = {{0}};

/* Sprite parts / Animations */
TAnimation Run = {0}, Stand = {0}, RunBack = {0};
TAnimation Jump = {0}, JumpSide = {0}, Roll = {0}, RollBack = {0};
TAnimation Fall = {0};
TAnimation Crouch = {0}, CrouchRun = {0}, CrouchRunBack = {0};
TAnimation Reload = {0}, Throw = {0}, Recoil = {0}, Shotgun = {0}, Barret = {0}, SmallRecoil = {0}, AimRecoil = {0}, HandsUpRecoil = {0};
TAnimation ClipIn = {0}, ClipOut = {0}, SlideBack = {0};
TAnimation Change = {0};
TAnimation ThrowWeapon = {0}, WeaponNone = {0};
TAnimation Punch = {0}, ReloadBow = {0}, Melee = {0};
TAnimation Cigar = {0}, Match = {0}, Smoke = {0}, Wipe = {0}, Groin = {0}, TakeOff = {0}, Victory = {0}, Piss = {0}, Mercy = {0}, Mercy2 = {0}, Own = {0};
TAnimation Prone = {0}, GetUp = {0}, ProneMove = {0};
TAnimation Aim = {0}, HandsUpAim = {0};

/* Old sprite positions for ping compensation */
TVector2 OldSpritePos[MAX_SPRITES + 1][MAX_OLDPOS + 1] = {{{0, 0}}};

/* Camera (client-side) */
extern float CameraX, CameraY;

/* Map changed flag */
extern bool MapChanged;

/* --- Implementation --- */

/* --- Timing --- */

void Number27Timing(void)
{
    TimeInMilLast = TimeInMil;
    TimeInMil = (unsigned long long)SDL_GetTicks();

    if (TimeInMil - TimeInMilLast > 2000) {
        TimeInMilLast = TimeInMil;  /* safety precaution */
    }

    TimePassed += (unsigned int)(TimeInMil - TimeInMilLast);
    SecondsLast = Seconds;
    Seconds = (int)(TimePassed / 1000);

    if (Seconds != SecondsLast) {
        /* new Second */
        TicksPerSecond = Ticks;
        Ticks = 0;

        FramesPerSecond = Frames;
        Frames = 0;
    }

    Frames++;

    TickTimeLast = TickTime;
    TickTime = (int)(TimePassed / (1000 / GOALTICKS));
}

/* --- Bullet time --- */

void ToggleBulletTime(bool TurnOn, int Duration)
{
    if (Duration <= 0) Duration = 30;

    if (TurnOn) {
        BulletTimeTimer = Duration;
        GOALTICKS = DEFAULT_GOALTICKS / 3;
    } else {
        GOALTICKS = DEFAULT_GOALTICKS;
    }

    Number27Timing();
}

/* --- Visibility --- */

bool PointVisible(float X, float Y, int i)
{
    int GAME_WIDTH, GAME_HEIGHT;
    float SX, SY;

#ifdef SERVER
    GAME_WIDTH = MAX_GAME_WIDTH;
    GAME_HEIGHT = 480;
#else
    GAME_WIDTH = GameWidth;
    GAME_HEIGHT = GameHeight;
#endif

    if (i > MAX_PLAYERS || i < 1) {
        return false;
    }

    SX = SpriteParts.Pos[i].X - ((SpriteParts.Pos[i].X - Sprite[i].Control.MouseAimX) / 2.0f);
    SY = SpriteParts.Pos[i].Y - ((SpriteParts.Pos[i].Y - Sprite[i].Control.MouseAimY) / 2.0f);

    if (X > (SX - GAME_WIDTH) && X < (SX + GAME_WIDTH) &&
        Y > (SY - GAME_HEIGHT) && Y < (SY + GAME_HEIGHT)) {
        return true;
    }
    return false;
}

bool PointVisible2(float X, float Y, int I)
{
    int GAME_WIDTH, GAME_HEIGHT;
    float SX, SY;

#ifdef SERVER
    GAME_WIDTH = MAX_GAME_WIDTH;
    GAME_HEIGHT = 480;
#else
    GAME_WIDTH = 600;
    GAME_HEIGHT = 440;
#endif

    SX = SpriteParts.Pos[I].X;
    SY = SpriteParts.Pos[I].Y;

    if (X > (SX - GAME_WIDTH) && X < (SX + GAME_WIDTH) &&
        Y > (SY - GAME_HEIGHT) && Y < (SY + GAME_HEIGHT)) {
        return true;
    }
    return false;
}

#ifndef SERVER
bool IsPointOnScreen(TVector2 Point)
{
    float P1, P2;
    bool Result = true;

    P1 = GameWidthHalf - (CameraX - Point.X);
    P2 = GameHeightHalf - (CameraY - Point.Y);

    if (P1 < 0 || P1 > GameWidth) Result = false;
    if (P2 < 0 || P2 > GameHeight) Result = false;

    return Result;
}
#endif

/* --- Game stats --- */

void UpdateGameStats(void)
{
    char *filename = NULL;
    FILE *f = NULL;

    if (!CVar_GetInteger("log_enable")) return;

    filename = (char*)malloc(strlen(UserDirectory) + strlen("logs/gamestat.txt") + 2);
    if (!filename) return;
    sprintf(filename, "%slogs/gamestat.txt", UserDirectory);

    f = fopen(filename, "w");
    if (!f) {
        free(filename);
        return;
    }

    fprintf(f, "In-Game Statistics\n");
    fprintf(f, "Players: %d\n", PlayersNum);
    fprintf(f, "Map: %s\n", Map.Name);

    switch (sv_gamemode) {
        case GAMESTYLE_DEATHMATCH: fprintf(f, "Gamemode: Deathmatch\n"); break;
        case GAMESTYLE_POINTMATCH: fprintf(f, "Gamemode: Pointmatch\n"); break;
        case GAMESTYLE_TEAMMATCH: fprintf(f, "Gamemode: Teammatch\n"); break;
        case GAMESTYLE_CTF: fprintf(f, "Gamemode: Capture the Flag\n"); break;
        case GAMESTYLE_RAMBO: fprintf(f, "Gamemode: Rambomatch\n"); break;
        case GAMESTYLE_INF: fprintf(f, "Gamemode: Infiltration\n"); break;
        case GAMESTYLE_HTF: fprintf(f, "Gamemode: Hold the Flag\n"); break;
    }
    fprintf(f, "Timeleft: %d:%02d\n", TimeLeftMin, TimeLeftSec);

    if (IsTeamGame()) {
        fprintf(f, "Team 1: %d\n", TeamScore[1]);
        fprintf(f, "Team 2: %d\n", TeamScore[2]);
        fprintf(f, "Team 3: %d\n", TeamScore[3]);
        fprintf(f, "Team 4: %d\n", TeamScore[4]);
    }

    fprintf(f, "Players list: (name/kills/deaths/team/ping)\n");
    if (PlayersNum > 0) {
        for (int i = 1; i <= PlayersNum; i++) {
            int idx = SortedPlayers[i].PlayerNum;
            if (idx >= 1 && idx <= MAX_SPRITES && Sprite[idx].Active && Sprite[idx].Player) {
                fprintf(f, "%s\n", Sprite[idx].Player->Name);
                fprintf(f, "%d\n", Sprite[idx].Player->Kills);
                fprintf(f, "%d\n", Sprite[idx].Player->Deaths);
                fprintf(f, "%d\n", Sprite[idx].Player->Team);
                fprintf(f, "%d\n", Sprite[idx].Player->RealPing);
            }
        }
    }

#ifndef SERVER
    fprintf(f, "\nServer:\n");
    fprintf(f, "%s:%s\n", JoinIP, JoinPort);
#endif

    fclose(f);
    free(filename);
}

/* --- Voting --- */

void StartVote(unsigned char StarterVote, unsigned char TypeVote, const char *TargetVote, const char *ReasonVote)
{
    unsigned char I;

    VoteActive = true;
    if (StarterVote < 1 || StarterVote > MAX_PLAYERS) {
        VoteStarter = strdup("Server");
    } else {
        VoteStarter = strdup(Sprite[StarterVote].Player ? Sprite[StarterVote].Player->Name : "Unknown");
        VoteCooldown[StarterVote] = DEFAULT_VOTE_TIME;
#ifndef SERVER
        if (StarterVote == MySprite) {
            if (TypeVote == VOTE_KICK) {
                if (MainConsole) console_write((PConsole)MainConsole, "You have voted to kick ", VOTE_MESSAGE_COLOR);
                VoteActive = false;
                ClientVoteKick(atoi(TargetVote), true, "");
            }
        }
#endif
    }
    VoteType = TypeVote;
    VoteTarget = TargetVote ? strdup(TargetVote) : NULL;
    VoteReason = ReasonVote ? strdup(ReasonVote) : NULL;
    VoteTimeRemaining = DEFAULT_VOTING_TIME;
    VoteNumVotes = 0;
    VoteMaxVotes = 0;
    for (I = 1; I <= MAX_PLAYERS; I++) {
        if (Sprite[I].Active && Sprite[I].Player && Sprite[I].Player->ControlMethod == HUMAN) {
            VoteMaxVotes++;
        }
    }
}

void StopVote(void)
{
    unsigned char I;

    VoteActive = false;
    VoteNumVotes = 0;
    VoteMaxVotes = 0;
    VoteType = 0;
    free(VoteTarget); VoteTarget = NULL;
    free(VoteStarter); VoteStarter = NULL;
    free(VoteReason); VoteReason = NULL;
    VoteTimeRemaining = -1;
    for (I = 1; I <= MAX_PLAYERS; I++) {
        VoteHasVoted[I] = false;
    }
}

void TimerVote(void)
{
#ifdef SERVER
    if (VoteActive) {
#endif
        if (VoteTimeRemaining > -1) {
            VoteTimeRemaining--;
        }

        if (VoteTimeRemaining == 0) {
            if (VoteType == VOTE_MAP) {
#ifndef SERVER
                if (MainConsole) console_write((PConsole)MainConsole, "No map has been voted", VOTE_MESSAGE_COLOR);
#endif
            }
            StopVote();
        }
#ifdef SERVER
    }
#endif
}

#ifdef SERVER
void CountVote(unsigned char Voter)
{
    int I;
    float Edge;

    if (VoteActive && !VoteHasVoted[Voter]) {
        VoteNumVotes++;
        VoteHasVoted[Voter] = true;
        Edge = (float)VoteNumVotes / (float)VoteMaxVotes;
        if (Edge >= ((float)CVar_GetInteger("sv_votepercent") / 100.0f)) {
            if (VoteType == VOTE_KICK) {
                I = atoi(VoteTarget);
                if (CheatTag[I] == 0) {
                    KickPlayer(I, true, KICK_VOTED, HOUR, "Vote Kicked");
                } else {
                    KickPlayer(I, true, KICK_VOTED, DAY, "Vote Kicked by Server");
                }
                DoBalanceBots(1, Sprite[I].Player->Team);
            } else if (VoteType == VOTE_MAP) {
                if (!PrepareMapChange(VoteTarget)) {
                    if (MainConsole) console_write((PConsole)MainConsole, "Map not found (", WARNING_MESSAGE_COLOR);
                    if (MainConsole) console_write((PConsole)MainConsole, VoteTarget, WARNING_MESSAGE_COLOR);
                    if (MainConsole) console_write((PConsole)MainConsole, ")", WARNING_MESSAGE_COLOR);
                    if (MainConsole) console_write((PConsole)MainConsole, "No map has been voted", VOTE_MESSAGE_COLOR);
                }
            }
            StopVote();
            ServerSendVoteOff();
        }
    }
}
#endif

/* --- Map change --- */

void ShowMapChangeScoreboard(void)
{
    ShowMapChangeScoreboardMap("EXIT*!*");
}

void ShowMapChangeScoreboardMap(const char *NextMap)
{
    MapChangeName = strdup(NextMap);
    MapChangeCounter = MapChangeTime;
#ifndef SERVER
    if (limbomenu) game_menu_show(limbomenu, false);
    FragsMenuShow = true;
    StatsMenuShow = false;
    for (int i = 1; i <= MAX_PLAYERS; i++) {
        if (Sprite[i].Active) {
            stop_sound(Sprite[i].ReloadSoundChannel);
            stop_sound(Sprite[i].JetsSoundChannel);
            stop_sound(Sprite[i].GattlingSoundChannel);
            stop_sound(Sprite[i].GattlingSoundChannel2);
        }
    }
#endif
}

/* --- Game mode --- */

bool IsTeamGame(void)
{
    switch (sv_gamemode) {
        case GAMESTYLE_TEAMMATCH:
        case GAMESTYLE_CTF:
        case GAMESTYLE_INF:
        case GAMESTYLE_HTF:
            return true;
        default:
            return false;
    }
}

/* --- Map change --- */

void ChangeMap(void)
{
    int i;

#ifdef SERVER
    TVector2 a = {0, 0};

    for (i = 1; i <= MAX_WAYPOINTS; i++) {
        BotPath.Waypoint[i].Active = false;
        BotPath.Waypoint[i].Id = 0;
        BotPath.Waypoint[i].PathNum = 0;
    }

    if (!TPolyMap_LoadMap(&Map, MapChange)) {
        if (MainConsole) console_write((PConsole)MainConsole, "Error: Could not load map (", DEBUG_MESSAGE_COLOR);
        if (MainConsole) console_write((PConsole)MainConsole, MapChange.Name, DEBUG_MESSAGE_COLOR);
        if (MainConsole) console_write((PConsole)MainConsole, ")", DEBUG_MESSAGE_COLOR);
        NextMap();
        ChangeMap();
        return;
    }
#else
    TMapInfo MapChangeStatus = {NULL, NULL, 0, NULL};
    MapChanged = true;
    TDemoRecorder_StopRecord(DemoRecorder);

    if (get_map_info(MapChangeName, ".", &MapChangeStatus) &&
        verify_map_checksum(MapChangeStatus, MapChangeChecksum)) {
        if (!TPolyMap_LoadMapBg(&Map, MapChangeStatus, false, 0, 0)) {
            char msg[256];
            snprintf(msg, sizeof(msg), "Could not load map: %s", MapChangeName);
            GameRendering_RenderGameInfo(msg);
            Client_ExitToMenu();
            return;
        }
    } else {
        Client_ExitToMenu();
        Client_JoinServer();
        return;
    }
#endif

    /* Common map change logic */
    for (i = 1; i <= MAX_BULLETS; i++) {
        TBullet_Kill(&Bullet[i]);
    }
    for (i = 1; i <= MAX_THINGS; i++) {
        TThing_Kill(&Thing[i]);
    }
#ifndef SERVER
    for (i = 1; i <= MAX_SPARKS; i++) {
        TSpark_Kill(&Spark[i]);
    }
#endif

    for (i = 1; i <= MAX_SPRITES; i++) {
        if (Sprite[i].Active && Sprite[i].Player && !Sprite[i].Player->DemoPlayer) {
            /* Reset sprite state for new map */
            Sprite[i].Player->Kills = 0;
            Sprite[i].Player->Deaths = 0;
            Sprite[i].Player->Flags = 0;
        }
    }

    for (i = 1; i <= 4; i++) {
        TeamScore[i] = 0;
    }
    for (i = 1; i <= 2; i++) {
        TeamFlag[i] = 0;
    }

#ifndef SERVER
    FragsMenuShow = false;
    StatsMenuShow = false;

    if (MySprite > 0) {
        game_menu_show(limbomenu, true);
    }
#endif

    SortPlayers();

    MapChangeCounter = -60;

    TimeLimitCounter = CVar_GetInteger("sv_timelimit");
}

/* --- Initialization --- */

void game_init(void)
{
    /* Initialize global game state */
    Ticks = 0;
    TicksPerSecond = 0;
    Frames = 0;
    FramesPerSecond = 0;
    TickTime = 0;
    TickTimeLast = 0;
    GOALTICKS = DEFAULT_GOALTICKS;

    TimeInMil = 0;
    TimeInMilLast = 0;
    TimePassed = 0;
    Seconds = 0;
    SecondsLast = 0;

    BulletTimeTimer = 0;
    GOALTICKS = DEFAULT_GOALTICKS;

    Grav = 0.06f;

    GameWidth = DEFAULT_WIDTH;
    GameHeight = DEFAULT_HEIGHT;
    GameWidthHalf = DEFAULT_WIDTH / 2.0f;
    GameHeightHalf = DEFAULT_HEIGHT / 2.0f;

    PlayersNum = 0;
    MySprite = 0;
    AliveNum = 0;
    memset(TeamAliveNum, 0, sizeof(TeamAliveNum));
    memset(TeamPlayersNum, 0, sizeof(TeamPlayersNum));

    memset(TeamScore, 0, sizeof(TeamScore));
    memset(TeamFlag, 0, sizeof(TeamFlag));

    SurvivalEndRound = false;
    WeaponsCleaned = false;
    CeaseFireTime = DEFAULT_CEASEFIRE_TIME;
    MapChangeTime = DEFAULT_MAPCHANGE_TIME;
    MapChangeCounter = 0;
    TimeLimitCounter = 3600;
    StartHealth = 150;
    TimeLeftSec = 0;
    TimeLeftMin = 0;

    memset(WeaponSel, 0, sizeof(WeaponSel));
    memset(WeaponSelArray, 0, sizeof(WeaponSelArray));

    /* Initialize Map */
    TPolyMap_Initialize(&Map);

    /* Initialize waypoints */
    for (int i = 1; i <= MAX_WAYPOINTS; i++) {
        BotPath.Waypoint[i].Active = false;
        BotPath.Waypoint[i].Id = 0;
        BotPath.Waypoint[i].PathNum = 0;
    }

    /* Voting */
    VoteActive = false;
    VoteType = 0;
    free(VoteTarget); VoteTarget = NULL;
    free(VoteStarter); VoteStarter = NULL;
    free(VoteReason); VoteReason = NULL;
    VoteTimeRemaining = -1;
    VoteNumVotes = 0;
    VoteMaxVotes = 0;
    memset(VoteHasVoted, 0, sizeof(VoteHasVoted));
    memset(VoteCooldown, 0, sizeof(VoteCooldown));
    VoteKickReasonType = false;

    memset(SortedPlayers, 0, sizeof(SortedPlayers));
    PlayersNum = 0;

    memset(TeamScore, 0, sizeof(TeamScore));
    memset(TeamFlag, 0, sizeof(TeamFlag));

    SinusCounter = 0.0f;

    memset(&GameModChecksum, 0, sizeof(GameModChecksum));
    memset(&CustomModChecksum, 0, sizeof(CustomModChecksum));
    memset(&MapCheckSum, 0, sizeof(MapCheckSum));

    memset(WeaponSel, 0, sizeof(WeaponSel));
    memset(WeaponSelArray, 0, sizeof(WeaponSelArray));

    /* Animations */
    memset(&Run, 0, sizeof(Run));
    memset(&Stand, 0, sizeof(Stand));
    memset(&RunBack, 0, sizeof(RunBack));
    memset(&Jump, 0, sizeof(Jump));
    memset(&JumpSide, 0, sizeof(JumpSide));
    memset(&Roll, 0, sizeof(Roll));
    memset(&RollBack, 0, sizeof(RollBack));
    memset(&Fall, 0, sizeof(Fall));
    memset(&Crouch, 0, sizeof(Crouch));
    memset(&CrouchRun, 0, sizeof(CrouchRun));
    memset(&CrouchRunBack, 0, sizeof(CrouchRunBack));
    memset(&Reload, 0, sizeof(Reload));
    memset(&Throw, 0, sizeof(Throw));
    memset(&Recoil, 0, sizeof(Recoil));
    memset(&Shotgun, 0, sizeof(Shotgun));
    memset(&Barret, 0, sizeof(Barret));
    memset(&SmallRecoil, 0, sizeof(SmallRecoil));
    memset(&AimRecoil, 0, sizeof(AimRecoil));
    memset(&HandsUpRecoil, 0, sizeof(HandsUpRecoil));
    memset(&ClipIn, 0, sizeof(ClipIn));
    memset(&ClipOut, 0, sizeof(ClipOut));
    memset(&SlideBack, 0, sizeof(SlideBack));
    memset(&Change, 0, sizeof(Change));
    memset(&ThrowWeapon, 0, sizeof(ThrowWeapon));
    memset(&WeaponNone, 0, sizeof(WeaponNone));
    memset(&Punch, 0, sizeof(Punch));
    memset(&ReloadBow, 0, sizeof(ReloadBow));
    memset(&Melee, 0, sizeof(Melee));
    memset(&Cigar, 0, sizeof(Cigar));
    memset(&Match, 0, sizeof(Match));
    memset(&Smoke, 0, sizeof(Smoke));
    memset(&Wipe, 0, sizeof(Wipe));
    memset(&Groin, 0, sizeof(Groin));
    memset(&TakeOff, 0, sizeof(TakeOff));
    memset(&Victory, 0, sizeof(Victory));
    memset(&Piss, 0, sizeof(Piss));
    memset(&Mercy, 0, sizeof(Mercy));
    memset(&Mercy2, 0, sizeof(Mercy2));
    memset(&Own, 0, sizeof(Own));
    memset(&Prone, 0, sizeof(Prone));
    memset(&GetUp, 0, sizeof(GetUp));
    memset(&ProneMove, 0, sizeof(ProneMove));
    memset(&Aim, 0, sizeof(Aim));
    memset(&HandsUpAim, 0, sizeof(HandsUpAim));

    memset(OldSpritePos, 0, sizeof(OldSpritePos));

    /* Camera */
    CameraX = 0.0f;
    CameraY = 0.0f;

    /* Map changed flag */
    MapChanged = false;
}

/* --- Game update --- */

void game_update(void)
{
    Number27Timing();
    Ticks++;

    /* Update game time */
    if (TimeLimitCounter > 0) {
        TimeLimitCounter--;
        TimeLeftSec = TimeLimitCounter % 60;
        TimeLeftMin = TimeLimitCounter / 60;
    }

    /* Update bullet time */
    if (BulletTimeTimer > 0) {
        BulletTimeTimer--;
        if (BulletTimeTimer == 0) {
            GOALTICKS = DEFAULT_GOALTICKS;
        }
    }

    /* Update map change counter */
    if (MapChangeCounter > 0) {
        MapChangeCounter--;
    }

    /* Update voting timer */
    TimerVote();

    /* Sort players periodically */
    if (Ticks % 60 == 0) {
        SortPlayers();
    }

    /* Update game stats periodically */
    if (Seconds % 30 == 0 && Seconds != SecondsLast) {
        UpdateGameStats();
    }
}

/* --- Sort players --- */

void SortPlayers(void)
{
    int i, j;
    TKillSort temp;

    /* Reset counts */
    PlayersNum = 0;
    memset(TeamAliveNum, 0, sizeof(TeamAliveNum));
    memset(TeamPlayersNum, 0, sizeof(TeamPlayersNum));

    /* Initialize SortedPlayers entries */
    for (i = 1; i <= MAX_SPRITES; i++) {
        SortedPlayers[i].Kills = 0;
        SortedPlayers[i].Deaths = 0;
        SortedPlayers[i].Flags = 0;
        SortedPlayers[i].PlayerNum = 0;
        SortedPlayers[i].Color = 0;
    }

    /* Build sorted player list */
    for (i = 1; i <= MAX_SPRITES; i++) {
        if (Sprite[i].Active && Sprite[i].Player && !Sprite[i].Player->DemoPlayer) {
            PlayersNum++;
            if (Sprite[i].Player->ControlMethod == BOT) {
                /* Bots handled */
            }
            if (Sprite[i].Player->Team >= TEAM_ALPHA && Sprite[i].Player->Team <= TEAM_DELTA) {
                TeamAliveNum[Sprite[i].Player->Team]++;
            }
            if (!Sprite[i].Player->DemoPlayer) {  /* Not spectator */
                SortedPlayers[PlayersNum].Kills = Sprite[i].Player->Kills;
                SortedPlayers[PlayersNum].Deaths = Sprite[i].Player->Deaths;
                SortedPlayers[PlayersNum].Flags = Sprite[i].Player->Flags;
                SortedPlayers[PlayersNum].PlayerNum = i;
                SortedPlayers[PlayersNum].Color = Sprite[i].Player->JetColor;
            } else {
                SortedPlayers[PlayersNum].Kills = 0;
                SortedPlayers[PlayersNum].Deaths = 0x7FFFFFFF; /* High(Integer) */
                SortedPlayers[PlayersNum].Flags = 0;
                SortedPlayers[PlayersNum].PlayerNum = i;
                SortedPlayers[PlayersNum].Color = Sprite[i].Player->JetColor;
            }
        }
    }

    /* Sort by flags descending */
    for (i = 1; i <= PlayersNum; i++) {
        for (j = i + 1; j <= PlayersNum; j++) {
            if (SortedPlayers[j].Flags > SortedPlayers[i].Flags) {
                temp = SortedPlayers[i];
                SortedPlayers[i] = SortedPlayers[j];
                SortedPlayers[j] = temp;
            }
        }
    }

    /* Sort by kills descending (tiebreaker: same flags) */
    for (i = 1; i <= PlayersNum; i++) {
        for (j = i + 1; j <= PlayersNum; j++) {
            if (SortedPlayers[j].Flags == SortedPlayers[i].Flags &&
                SortedPlayers[j].Kills > SortedPlayers[i].Kills) {
                temp = SortedPlayers[i];
                SortedPlayers[i] = SortedPlayers[j];
                SortedPlayers[j] = temp;
            }
        }
    }

    /* Final sort by deaths ascending (tiebreaker: same flags and kills) */
    for (i = 1; i <= PlayersNum; i++) {
        for (j = i + 1; j <= PlayersNum; j++) {
            if (SortedPlayers[j].Flags == SortedPlayers[i].Flags &&
                SortedPlayers[j].Kills == SortedPlayers[i].Kills &&
                SortedPlayers[j].Deaths < SortedPlayers[i].Deaths) {
                temp = SortedPlayers[i];
                SortedPlayers[i] = SortedPlayers[j];
                SortedPlayers[j] = temp;
            }
        }
    }
}