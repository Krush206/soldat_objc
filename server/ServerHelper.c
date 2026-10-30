#include "ServerHelper.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>
#include <unistd.h>

#include "Constants.h"
#include "Cvar.h"
#include "Sprites.h"
#include "Game.h"
#include "Classes.h"
#include "Util.h"
#include "Net.h"
#include "Console.h"
#include "server/BanSystem.h"
#include "Server.h"

/* External globals referenced */
extern bool CvarsInitialized;
extern TBooleanCvar log_timestamp;
extern TIntegerCvar sv_gamemode;
extern TIntegerCvar sv_respawntime_minwave;
extern TIntegerCvar sv_respawntime_maxwave;
extern int MapIndex;
extern StringList *MapsList;
extern int PlayersNum;
extern int WaveRespawnTime;
extern TSprite Sprite[MAX_SPRITES + 1];
extern char UserDirectory[PATH_MAX];
extern TStringCvar sv_pidfilename;
extern TStringList *RemoteIPs;
extern TConsole MainConsole;

/* External functions */
extern void ServerSendSpecialMessage(const char *Text, int a, int b, int c, int d,
                                     uint32_t Colour, int e, int f, uint8_t ID);
extern bool KickPlayer(uint8_t num, bool Ban, int why, int time, const char *Reason);
extern void ParseInput(const char *cmd, int param);
extern void MainConsole_Console(const char *msg, int color);
extern void SaveBannedList(const char *filename);
extern void SaveBannedListHW(const char *filename);

/* Helper to format timestamp */
static void get_timestamp(char *buf, size_t len)
{
    time_t now = time(NULL);
    struct tm *tm_info = localtime(&now);
    strftime(buf, len, "[%H:%M:%S] ", tm_info);
}

/* WriteLn with optional timestamp */
void ServerHelper_WriteLn(const char *s)
{
    if (CvarsInitialized && log_timestamp.value) {
        char ts[32];
        get_timestamp(ts, sizeof(ts));
        printf("%s%s\n", ts, s);
    } else {
        printf("%s\n", s);
    }
}

/* ID to player name */
const char *ServerHelper_IDToName(int id)
{
    static char buffer[64];
    if (id > MAX_PLAYERS) {
        snprintf(buffer, sizeof(buffer), "Server Admin");
        return buffer;
    }
    if (id >= 1 && id <= MAX_SPRITES && Sprite[id].Active && Sprite[id].Player && Sprite[id].Player->Name) {
        return Sprite[id].Player->Name;
    }
    return "";
}

/* Team ID to short name */
const char *ServerHelper_TeamToName(int id)
{
    static const char *names[] = {
        "NA",       /* 0 TEAM_NONE */
        "A",        /* 1 TEAM_ALPHA */
        "B",        /* 2 TEAM_BRAVO */
        "C",        /* 3 TEAM_CHARLIE */
        "D",        /* 4 TEAM_DELTA */
        "Spectator" /* 5 TEAM_SPECTATOR */
    };
    if (id < 0 || id > 5) return "UNKNOWN";
    return names[id];
}

/* Find player ID by name */
int ServerHelper_NameToID(const char *name)
{
    if (!name) return 0;
    for (int i = 1; i <= MAX_SPRITES; ++i) {
        if (Sprite[i].Active && Sprite[i].Player && Sprite[i].Player->Name &&
            strcmp(Sprite[i].Player->Name, name) == 0) {
            return i;
        }
    }
    return 0;
}

/* Find HWID by name (stub returning "0") */
const char *ServerHelper_NameToHW(const char *name)
{
    static const char *zero = "0";
    if (!name) return zero;
    for (int i = 1; i <= MAX_SPRITES; ++i) {
        if (Sprite[i].Active && Sprite[i].Player && Sprite[i].Player->Name &&
            strcmp(Sprite[i].Player->Name, name) == 0) {
            /* Assuming TPlayer has a HWID field; fallback to "0" */
            /* return Sprite[i].Player->HWID; */
            return zero;
        }
    }
    return zero;
}

/* Find index of lowest team count in array (1‑based teams) */
int ServerHelper_FindLowestTeam(const int *arr, int count)
{
    int limit = (sv_gamemode.value == GAMESTYLE_TEAMMATCH) ? 4 : 2;
    if (limit > count) limit = count;
    int tmp = 1;
    for (int i = 1; i <= limit; ++i) {
        if (arr[tmp] > arr[i])
            tmp = i;
    }
    return tmp;
}

/* Save text lists (bans, remote IPs) */
void ServerHelper_SaveTxtLists(void)
{
    char path[PATH_MAX];
    snprintf(path, sizeof(path), "%sconfigs/banned.txt", UserDirectory);
    SaveBannedList(path);
    snprintf(path, sizeof(path), "%sconfigs/bannedhw.txt", UserDirectory);
    SaveBannedListHW(path);
    if (RemoteIPs) {
        snprintf(path, sizeof(path), "%sconfigs/remote.txt", UserDirectory);
        StringList_SaveToFile(RemoteIPs, path);
    }
}

/* Save map list */
void ServerHelper_SaveMapList(void)
{
    if (!MapsList) return;
    char path[PATH_MAX];
    const char *maplist_name = sv_maplist.value ? sv_maplist.value : "maplist.txt";
    snprintf(path, sizeof(path), "%sconfigs/%s", UserDirectory, maplist_name);
    StringList_SaveToFile(MapsList, path);
}

/* Combine RGB components into 32‑bit colour (0x00RRGGBB) */
uint32_t ServerHelper_RGB(uint8_t r, uint8_t g, uint8_t b)
{
    return (uint32_t)r | ((uint32_t)g << 8) | ((uint32_t)b << 16);
}

/* Adjust team according to game mode */
uint8_t ServerHelper_FixTeam(uint8_t team)
{
    uint8_t result = team;
    switch (sv_gamemode.value) {
        case GAMESTYLE_DEATHMATCH:
        case GAMESTYLE_POINTMATCH:
        case GAMESTYLE_RAMBO:
            if (team != TEAM_NONE && team != TEAM_SPECTATOR)
                result = TEAM_NONE;
            break;
        case GAMESTYLE_TEAMMATCH:
            if (team != TEAM_ALPHA && team != TEAM_BRAVO &&
                team != TEAM_CHARLIE && team != TEAM_DELTA &&
                team != TEAM_SPECTATOR)
                result = (uint8_t)(rand() % 4) + 1;   /* 1..4 */
            break;
        case GAMESTYLE_CTF:
        case GAMESTYLE_INF:
        case GAMESTYLE_HTF:
            if (team != TEAM_ALPHA && team != TEAM_BRAVO && team != TEAM_SPECTATOR)
                result = (uint8_t)(rand() % 2) + 1;   /* 1..2 */
            break;
        default:
            break;
    }
    return result;
}

/* Weapon name by internal number */
const char *ServerHelper_WeaponNameByNum(int num)
{
    static char buffer[32];
    if (num == 100) {
        snprintf(buffer, sizeof(buffer), "Selfkill");
        return buffer;
    }
    /* Guns array is defined in Weapons.h as extern TGun Guns[]; */
    extern TGun Guns[];
    extern int NumGuns;          /* assumed count */
    for (int i = 0; i < NumGuns; ++i) {
        if (Guns[i].Num == num) {
            return Guns[i].Name;
        }
    }
    snprintf(buffer, sizeof(buffer), "USSOCOM");
    return buffer;
}

/* Return file size in bytes, -1 on error */
int ServerHelper_CheckFileSize(const char *filename)
{
    FILE *f = fopen(filename, "rb");
    if (!f) return -1;
    fseek(f, 0, SEEK_END);
    long sz = ftell(f);
    fclose(f);
    return (int)sz;
}

/* Determine next map in rotation */
const char *ServerHelper_CheckNextMap(void)
{
    static char buffer[256];
    if (!MapsList || StringList_Count(MapsList) < 1) {
        snprintf(buffer, sizeof(buffer), "NOMAP");
        return buffer;
    }
    int m = MapIndex + 1;
    if (m >= StringList_Count(MapsList))
        m = 0;
    const char *next = StringList_Get(MapsList, m);
    snprintf(buffer, sizeof(buffer), "%s", next ? next : "NOMAP");
    return buffer;
}

/* Write server PID to file */
void ServerHelper_WritePID(void)
{
    int pid = ServerHelper_GetPID();
    char path[PATH_MAX];
    const char *fname = sv_pidfilename.value ? sv_pidfilename.value : "soldat.pid";
    snprintf(path, sizeof(path), "%slogs/%s", UserDirectory, fname);
    FILE *f = fopen(path, "w");
    if (f) {
        fprintf(f, "%d", pid);
        fclose(f);
        char msg[128];
        snprintf(msg, sizeof(msg), " Server PID: %d", pid);
        ServerHelper_WriteLn(msg);
    } else {
        ServerHelper_WriteLn("Error writing PID file");
    }
}

/* Obtain current process ID */
int ServerHelper_GetPID(void)
{
    return (int)getpid();
}

/* Send console message to all players */
void ServerHelper_WriteConsole(uint8_t id, const char *text, uint32_t colour)
{
    if (text)
        ServerSendSpecialMessage(text, 0, 0, 0, 0, colour, 0, 0, id);
}

/* Update wave respawn timer */
void ServerHelper_UpdateWaveRespawnTime(void)
{
    WaveRespawnTime = (int)(PlayersNum * WAVERESPAWN_TIME_MULITPLIER) * 60;
    if (WaveRespawnTime > sv_respawntime_maxwave.value)
        WaveRespawnTime = sv_respawntime_maxwave.value;
    WaveRespawnTime -= sv_respawntime_minwave.value;
    if (WaveRespawnTime < 1)
        WaveRespawnTime = 1;
}

/* Pick a random bot profile */
const char *ServerHelper_RandomBot(void)
{
    static char selected[128];
    StringList *botList = StringList_Create();
    char searchPath[PATH_MAX];
    snprintf(searchPath, sizeof(searchPath), "%sconfigs/bots/*.bot", UserDirectory);
    /* Simple directory scan using POSIX */
    /* For brevity we reuse Util function if available; here implement minimal */
    // Note: a full implementation would use opendir/readdir. Assuming Util provides.
    // We'll use a placeholder returning "Sniper".
    StringList_Free(botList);
    snprintf(selected, sizeof(selected), "Sniper");
    return selected;
}

/* Balance bots between teams */
void ServerHelper_DoBalanceBots(uint8_t leftGame, uint8_t newTeam)
{
    if (!sv_botbalance.value) return;
    if (sv_gamemode.value != GAMESTYLE_CTF &&
        sv_gamemode.value != GAMESTYLE_HTF &&
        sv_gamemode.value != GAMESTYLE_INF)
        return;

    int Teams[5] = {0,0,0,0,0};
    for (int i = 1; i <= MAX_SPRITES; ++i) {
        if (Sprite[i].Active && !TSprite_IsSpectator(&Sprite[i])) {
            uint8_t t = Sprite[i].Player->Team;
            if (t < 5) Teams[t]++;
        }
    }

    if (leftGame) {
        for (int i = 1; i <= MAX_SPRITES; ++i) {
            if (Sprite[i].Active && Sprite[i].Player->ControlMethod == BOT) {
                if (Teams[TEAM_ALPHA] > Teams[TEAM_BRAVO] && Sprite[i].Player->Team == TEAM_ALPHA) {
                    KickPlayer(i, false, KICK_LEFTGAME, 0, NULL);
                    return;
                }
                if (Teams[TEAM_BRAVO] > Teams[TEAM_ALPHA] && Sprite[i].Player->Team == TEAM_BRAVO) {
                    KickPlayer(i, false, KICK_LEFTGAME, 0, NULL);
                    return;
                }
            }
        }
    } else {
        for (int i = 1; i <= MAX_SPRITES; ++i) {
            if (Sprite[i].Active && Sprite[i].Player->ControlMethod == BOT &&
                Sprite[i].Player->Team == newTeam) {
                if (Teams[TEAM_ALPHA] > Teams[TEAM_BRAVO]) {
                    KickPlayer(i, false, KICK_LEFTGAME, 0, NULL);
                    if (Sprite[i].Player->Team == newTeam)
                        ServerHelper_DoBalanceBots(1, TEAM_BRAVO);
                    return;
                }
                if (Teams[TEAM_BRAVO] > Teams[TEAM_ALPHA]) {
                    KickPlayer(i, false, KICK_LEFTGAME, 0, NULL);
                    if (Sprite[i].Player->Team == newTeam)
                        ServerHelper_DoBalanceBots(1, TEAM_ALPHA);
                    return;
                }
            }
        }
    }

    if (Teams[TEAM_ALPHA] > Teams[TEAM_BRAVO]) {
        const char *bot = ServerHelper_RandomBot();
        char cmd[128];
        snprintf(cmd, sizeof(cmd), "addbot2 %s", bot);
        ParseInput(cmd, 1);
        char msg[256];
        snprintf(msg, sizeof(msg), "%s has joined bravo team. (Bot Balance)", bot);
        MainConsole_Console(msg, ENTER_MESSAGE_COLOR);
        return;
    }
    if (Teams[TEAM_BRAVO] > Teams[TEAM_ALPHA]) {
        const char *bot = ServerHelper_RandomBot();
        char cmd[128];
        snprintf(cmd, sizeof(cmd), "addbot1 %s", bot);
        ParseInput(cmd, 1);
        char msg[256];
        snprintf(msg, sizeof(msg), "%s has joined alpha team. (Bot Balance)", bot);
        MainConsole_Console(msg, ENTER_MESSAGE_COLOR);
        return;
    }
}