#include "ScriptGame.h"
#include <stdlib.h>
#include <string.h>

/* Forward declarations of engine globals / functions used by the original Pascal code.
   These are expected to be provided by the rest of the server code base. */
extern int sv_gamemode;
extern int sv_maxplayers;
extern bool sv_lockedmode;
extern int MapIndex;
extern int MapsList_Count;
extern const char **MapsList;          /* array of map names */
extern const char *Map_Name;           /* current map name */
extern int BotsNum;
extern int PlayersNum;
extern int SpectatorsNum;
extern int sv_killlimit;
extern const char *Server_ServerIP;
extern const char *sv_hostname;
extern int Server_ServerPort;
extern const char *DEDVERSION;
extern const char *sv_info;
extern float Grav;
extern int MapChangeCounter;
extern int sv_respawntime;
extern int sv_respawntime_minwave;
extern int sv_respawntime_maxwave;
extern int sv_maxgrenades;
extern int sv_bonus_frequency;
extern int sv_timelimit;
extern int Game_TimeLeftMin;
extern int Game_TimeLeftSec;
extern bool sv_friendlyfire;
extern const char *sv_password;
extern int sv_votepercent;
extern bool sv_realisticmode;
extern bool sv_survivalmode;
extern bool sv_advancemode;
extern bool sv_botbalance;
extern long MainTickCounter;
extern void ParseInput(const char *cmd, int len);
extern void DemoRecorder_StopRecord(void);
extern bool DemoRecorder_StartRecord(const char *filename);
extern void Server_ShutDown(void);
extern void StartVote(int a, int b, const char *c, const char *d);
extern void ServerSendVoteOn(int a, int b, const char *c, const char *d);

/* Internal structure definition */
struct ScriptGame {
    void *teams[6];                     /* TScriptTeam* for IDs 0..5 */
    void *mapsList;                     /* TScriptMapsList* */
    void *banLists;                     /* TScriptBanLists* */

    ScriptGame_OnClockTick onClockTick;
    ScriptGame_OnIdle onIdle;
    ScriptGame_OnJoin onJoin;
    ScriptGame_OnLeave onLeave;
    ScriptGame_OnRequest onRequest;
    ScriptGame_OnAdminCommand onAdminCommand;
    ScriptGame_OnTCPMessage onTCPMessage;
    ScriptGame_OnTCPCommand onTCPCommand;
    ScriptGame_OnAdminConnect onAdminConnect;
    ScriptGame_OnAdminDisconnect onAdminDisconnect;

    int32_t tickThreshold;
};

/* Helper to create the six team objects (opaque, here just placeholder) */
static void *CreateTeam(int id) {
    /* In the real code this would allocate a TScriptTeam instance.
       For this translation we return a dummy pointer. */
    return (void *)(intptr_t)id;
}
static void FreeTeam(void *team) {
    /* Release team resources if needed */
    (void)team;
}

/* ------------------------------------------------------------------ */
/* Construction / destruction                                           */
/* ------------------------------------------------------------------ */
ScriptGame *ScriptGame_Create(void)
{
    ScriptGame *g = (ScriptGame *)calloc(1, sizeof(ScriptGame));
    if (!g) return NULL;

    g->tickThreshold = 60;
    for (int i = 0; i < 6; ++i)
        g->teams[i] = CreateTeam(i);

    g->mapsList = NULL;   /* TScriptMapsList_Create() would be called here */
    g->banLists = NULL;   /* TScriptBanLists_Create() would be called here */

    return g;
}

void ScriptGame_Destroy(ScriptGame *game)
{
    if (!game) return;
    for (int i = 0; i < 6; ++i)
        FreeTeam(game->teams[i]);

    /* TScriptMapsList_Free(game->mapsList); */
    /* TScriptBanLists_Free(game->banLists); */

    free(game);
}

/* ------------------------------------------------------------------ */
/* Core methods                                                        */
/* ------------------------------------------------------------------ */
void ScriptGame_Shutdown(ScriptGame *game)
{
    (void)game;
    Server_ShutDown();
}

void ScriptGame_StartVoteKick(ScriptGame *game, uint8_t id, const char *reason)
{
    (void)game;
    char idStr[4];
    snprintf(idStr, sizeof(idStr), "%u", id);
    StartVote(255, 1, idStr, reason);
    ServerSendVoteOn(1, 255, idStr, reason);
}

void ScriptGame_StartVoteMap(ScriptGame *game, const char *name)
{
    (void)game;
    StartVote(255, 0, name, "");
    ServerSendVoteOn(0, 255, name, "");
}

void ScriptGame_StopRecord(ScriptGame *game)
{
    (void)game;
    DemoRecorder_StopRecord();
}

void ScriptGame_Restart(ScriptGame *game)
{
    (void)game;
    ParseInput("restart", 255);
}

bool ScriptGame_LoadWeap(ScriptGame *game, const char *weaponMod)
{
    (void)game;
    char cmd[256];
    snprintf(cmd, sizeof(cmd), "loadwep %s", weaponMod);
    ParseInput(cmd, 255);
    return true;    /* ParseInput returns non‑zero on success in original code */
}

bool ScriptGame_LoadCon(ScriptGame *game, const char *configFile)
{
    (void)game;
    char cmd[256];
    snprintf(cmd, sizeof(cmd), "loadcon %s", configFile);
    ParseInput(cmd, 255);
    return true;
}

bool ScriptGame_LoadList(ScriptGame *game, const char *mapsList)
{
    (void)game;
    char cmd[256];
    snprintf(cmd, sizeof(cmd), "loadlist %s", mapsList);
    ParseInput(cmd, 255);
    return true;
}

bool ScriptGame_StartRecord(ScriptGame *game, const char *demoName)
{
    (void)game;
    char filename[260];
    snprintf(filename, sizeof(filename), "%s.sdm", demoName);
    return DemoRecorder_StartRecord(filename);
}

/* ------------------------------------------------------------------ */
/* Property accessors                                                   */
/* ------------------------------------------------------------------ */
uint8_t ScriptGame_GetGameStyle(ScriptGame *game) { (void)game; return (uint8_t)sv_gamemode; }
void ScriptGame_SetGameStyle(ScriptGame *game, uint8_t style)
{
    (void)game;
    char cmd[32];
    snprintf(cmd, sizeof(cmd), "gamemode %u", style);
    ParseInput(cmd, 255);
}

uint8_t ScriptGame_GetMaxPlayers(ScriptGame *game) { (void)game; return (uint8_t)sv_maxplayers; }
void ScriptGame_SetMaxPlayers(ScriptGame *game, uint8_t max)
{
    (void)game;
    if (!sv_lockedmode) {
        sv_maxplayers = max;          /* direct assignment mimics SetValue */
    } else {
        /* locked mode – original code raised an exception */
    }
}

const char *ScriptGame_GetNextMap(ScriptGame *game)
{
    (void)game;
    int m = MapIndex + 1;
    if (m >= MapsList_Count) m = 0;
    return MapsList[m];
}

const char *ScriptGame_GetCurrentMap(ScriptGame *game) { (void)game; return Map_Name; }

uint8_t ScriptGame_GetNumBots(ScriptGame *game) { (void)game; return (uint8_t)BotsNum; }
uint8_t ScriptGame_GetNumPlayers(ScriptGame *game) { (void)game; return (uint8_t)PlayersNum; }
uint8_t ScriptGame_GetSpectators(ScriptGame *game) { (void)game; return (uint8_t)SpectatorsNum; }

uint16_t ScriptGame_GetScoreLimit(ScriptGame *game) { (void)game; return (uint16_t)sv_killlimit; }
void ScriptGame_SetScoreLimit(ScriptGame *game, uint16_t limit)
{
    (void)game;
    char cmd[32];
    snprintf(cmd, sizeof(cmd), "limit %u", limit);
    ParseInput(cmd, 255);
}

const char *ScriptGame_GetServerIP(ScriptGame *game) { (void)game; return Server_ServerIP; }
const char *ScriptGame_GetServerName(ScriptGame *game) { (void)game; return sv_hostname; }
uint16_t ScriptGame_GetServerPort(ScriptGame *game) { (void)game; return (uint16_t)Server_ServerPort; }
const char *ScriptGame_GetServerVersion(ScriptGame *game) { (void)game; return DEDVERSION; }
const char *ScriptGame_GetServerInfo(ScriptGame *game) { (void)game; return sv_info; }

float ScriptGame_GetGravity(ScriptGame *game) { (void)game; return Grav; }
void ScriptGame_SetGravity(ScriptGame *game, float grav)
{
    (void)game;
    /* sv_gravity.SetValue(grav) – assumed to be a global variable */
    extern float sv_gravity;
    sv_gravity = grav;
}

bool ScriptGame_GetPaused(ScriptGame *game) { (void)game; return MapChangeCounter == 999999999; }
void ScriptGame_SetPaused(ScriptGame *game, bool paused)
{
    (void)game;
    ParseInput(paused ? "pause" : "unpause", 255);
}

int32_t ScriptGame_GetRespawnTime(ScriptGame *game) { (void)game; return sv_respawntime; }
void ScriptGame_SetRespawnTime(ScriptGame *game, int32_t time) { (void)game; sv_respawntime = time; }

int32_t ScriptGame_GetMinRespawnTime(ScriptGame *game) { (void)game; return sv_respawntime_minwave; }
void ScriptGame_SetMinRespawnTime(ScriptGame *game, int32_t time) { (void)game; sv_respawntime_minwave = time; }

int32_t ScriptGame_GetMaxRespawnTime(ScriptGame *game) { (void)game; return sv_respawntime_maxwave; }
void ScriptGame_SetMaxRespawnTime(ScriptGame *game, int32_t time) { (void)game; sv_respawntime_maxwave = time; }

uint8_t ScriptGame_GetMaxGrenades(ScriptGame *game) { (void)game; return (uint8_t)sv_maxgrenades; }
void ScriptGame_SetMaxGrenades(ScriptGame *game, uint8_t num) { (void)game; sv_maxgrenades = num; }

uint8_t ScriptGame_GetBonus(ScriptGame *game) { (void)game; return (uint8_t)sv_bonus_frequency; }
void ScriptGame_SetBonus(ScriptGame *game, uint8_t num) { (void)game; sv_bonus_frequency = num; }

int32_t ScriptGame_GetTimeLimit(ScriptGame *game) { (void)game; return sv_timelimit; }
void ScriptGame_SetTimeLimit(ScriptGame *game, int32_t num) { (void)game; sv_timelimit = num; }

int32_t ScriptGame_GetTimeLeft(ScriptGame *game)
{
    (void)game;
    return 60 * Game_TimeLeftMin + Game_TimeLeftSec;
}

bool ScriptGame_GetFriendlyFire(ScriptGame *game) { (void)game; return sv_friendlyfire; }
void ScriptGame_SetFriendlyFire(ScriptGame *game, bool enabled) { (void)game; sv_friendlyfire = enabled; }

const char *ScriptGame_GetPassword(ScriptGame *game) { (void)game; return sv_password; }
void ScriptGame_SetPassword(ScriptGame *game, const char *pass) { (void)game; sv_password = pass; }

#ifdef ENABLE_RCON
const char *ScriptGame_GetAdminPassword(ScriptGame *game)
{
    (void)game;
    extern const char *AdminServer_Password;
    extern bool AdminServer_Assigned;
    return AdminServer_Assigned ? AdminServer_Password : "";
}
void ScriptGame_SetAdminPassword(ScriptGame *game, const char *pass)
{
    (void)game;
    extern bool AdminServer_Assigned;
    extern char *AdminServer_Password;
    if (AdminServer_Assigned) {
        free(AdminServer_Password);
        AdminServer_Password = strdup(pass);
    }
}
#endif

uint8_t ScriptGame_GetVotePercent(ScriptGame *game) { (void)game; return (uint8_t)sv_votepercent; }
void ScriptGame_SetVotePercent(ScriptGame *game, uint8_t percent) { (void)game; sv_votepercent = percent; }

bool ScriptGame_GetRealistic(ScriptGame *game) { (void)game; return sv_realisticmode; }
void ScriptGame_SetRealistic(ScriptGame *game, bool enabled) { (void)game; sv_realisticmode = enabled; }

bool ScriptGame_GetSurvival(ScriptGame *game) { (void)game; return sv_survivalmode; }
void ScriptGame_SetSurvival(ScriptGame *game, bool enabled) { (void)game; sv_survivalmode = enabled; }

bool ScriptGame_GetAdvance(ScriptGame *game) { (void)game; return sv_advancemode; }
void ScriptGame_SetAdvance(ScriptGame *game, bool enabled) { (void)game; sv_advancemode = enabled; }

bool ScriptGame_GetBalance(ScriptGame *game) { (void)game; return sv_botbalance; }
void ScriptGame_SetBalance(ScriptGame *game, bool enabled) { (void)game; sv_botbalance = enabled; }

int32_t ScriptGame_GetTickCount(ScriptGame *game) { (void)game; return MainTickCounter; }

int32_t ScriptGame_GetTickThreshold(ScriptGame *game) { return game ? game->tickThreshold : 0; }
void ScriptGame_SetTickThreshold(ScriptGame *game, int32_t threshold) { if (game) game->tickThreshold = threshold; }

/* ------------------------------------------------------------------ */
/* Team / Map / Ban list accessors                                      */
/* ------------------------------------------------------------------ */
void *ScriptGame_GetTeam(ScriptGame *game, uint8_t id)
{
    if (!game || id > 5) return NULL;
    return game->teams[id];
}
void *ScriptGame_GetMapsList(ScriptGame *game) { return game ? game->mapsList : NULL; }
void *ScriptGame_GetBanLists(ScriptGame *game) { return game ? game->banLists : NULL; }

/* ------------------------------------------------------------------ */
/* Callback registration                                                */
/* ------------------------------------------------------------------ */
void ScriptGame_SetOnClockTick(ScriptGame *game, ScriptGame_OnClockTick cb) { if (game) game->onClockTick = cb; }
ScriptGame_OnClockTick ScriptGame_GetOnClockTick(ScriptGame *game) { return game ? game->onClockTick : NULL; }

void ScriptGame_SetOnIdle(ScriptGame *game, ScriptGame_OnIdle cb) { if (game) game->onIdle = cb; }
ScriptGame_OnIdle ScriptGame_GetOnIdle(ScriptGame *game) { return game ? game->onIdle : NULL; }

void ScriptGame_SetOnJoin(ScriptGame *game, ScriptGame_OnJoin cb) { if (game) game->onJoin = cb; }
ScriptGame_OnJoin ScriptGame_GetOnJoin(ScriptGame *game) { return game ? game->onJoin : NULL; }

void ScriptGame_SetOnLeave(ScriptGame *game, ScriptGame_OnLeave cb) { if (game) game->onLeave = cb; }
ScriptGame_OnLeave ScriptGame_GetOnLeave(ScriptGame *game) { return game ? game->onLeave : NULL; }

void ScriptGame_SetOnRequest(ScriptGame *game, ScriptGame_OnRequest cb) { if (game) game->onRequest = cb; }
ScriptGame_OnRequest ScriptGame_GetOnRequest(ScriptGame *game) { return game ? game->onRequest : NULL; }

void ScriptGame_SetOnAdminCommand(ScriptGame *game, ScriptGame_OnAdminCommand cb) { if (game) game->onAdminCommand = cb; }
ScriptGame_OnAdminCommand ScriptGame_GetOnAdminCommand(ScriptGame *game) { return game ? game->onAdminCommand : NULL; }

void ScriptGame_SetOnTCPMessage(ScriptGame *game, ScriptGame_OnTCPMessage cb) { if (game) game->onTCPMessage = cb; }
ScriptGame_OnTCPMessage ScriptGame_GetOnTCPMessage(ScriptGame *game) { return game ? game->onTCPMessage : NULL; }

void ScriptGame_SetOnTCPCommand(ScriptGame *game, ScriptGame_OnTCPCommand cb) { if (game) game->onTCPCommand = cb; }
ScriptGame_OnTCPCommand ScriptGame_GetOnTCPCommand(ScriptGame *game) { return game ? game->onTCPCommand : NULL; }

void ScriptGame_SetOnAdminConnect(ScriptGame *game, ScriptGame_OnAdminConnect cb) { if (game) game->onAdminConnect = cb; }
ScriptGame_OnAdminConnect ScriptGame_GetOnAdminConnect(ScriptGame *game) { return game ? game->onAdminConnect : NULL; }

void ScriptGame_SetOnAdminDisconnect(ScriptGame *game, ScriptGame_OnAdminDisconnect cb) { if (game) game->onAdminDisconnect = cb; }
ScriptGame_OnAdminDisconnect ScriptGame_GetOnAdminDisconnect(ScriptGame *game) { return game ? game->onAdminDisconnect : NULL; }