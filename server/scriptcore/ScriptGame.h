#ifndef SCRIPTGAME_H
#define SCRIPTGAME_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Opaque handle for the game instance */
typedef struct ScriptGame ScriptGame;

/* Callback function pointer types */
typedef void (*ScriptGame_OnClockTick)(int ticks);
typedef void (*ScriptGame_OnIdle)(void);
typedef void (*ScriptGame_OnJoin)(void *player, void *team);
typedef void (*ScriptGame_OnLeave)(void *player, bool kicked);
typedef int (*ScriptGame_OnRequest)(const char *ip, const char *hw, uint16_t port,
                                    uint8_t state, bool forwarded, const char *password);
typedef bool (*ScriptGame_OnAdminCommand)(void *player, const char *command);
typedef void (*ScriptGame_OnTCPMessage)(const char *ip, uint16_t port, const char *message);
typedef bool (*ScriptGame_OnTCPCommand)(const char *ip, uint16_t port, const char *command);
typedef void (*ScriptGame_OnAdminConnect)(const char *ip, uint16_t port);
typedef void (*ScriptGame_OnAdminDisconnect)(const char *ip, uint16_t port);

/* Construction / destruction */
ScriptGame *ScriptGame_Create(void);
void ScriptGame_Destroy(ScriptGame *game);

/* Core methods */
void ScriptGame_Shutdown(ScriptGame *game);
void ScriptGame_StartVoteKick(ScriptGame *game, uint8_t id, const char *reason);
void ScriptGame_StartVoteMap(ScriptGame *game, const char *name);
void ScriptGame_StopRecord(ScriptGame *game);
void ScriptGame_Restart(ScriptGame *game);
bool ScriptGame_LoadWeap(ScriptGame *game, const char *weaponMod);
bool ScriptGame_LoadCon(ScriptGame *game, const char *configFile);
bool ScriptGame_LoadList(ScriptGame *game, const char *mapsList);
bool ScriptGame_StartRecord(ScriptGame *game, const char *demoName);

/* Property accessors */
uint8_t ScriptGame_GetGameStyle(ScriptGame *game);
void ScriptGame_SetGameStyle(ScriptGame *game, uint8_t style);

uint8_t ScriptGame_GetMaxPlayers(ScriptGame *game);
void ScriptGame_SetMaxPlayers(ScriptGame *game, uint8_t max);

const char *ScriptGame_GetNextMap(ScriptGame *game);
const char *ScriptGame_GetCurrentMap(ScriptGame *game);

uint8_t ScriptGame_GetNumBots(ScriptGame *game);
uint8_t ScriptGame_GetNumPlayers(ScriptGame *game);
uint8_t ScriptGame_GetSpectators(ScriptGame *game);

uint16_t ScriptGame_GetScoreLimit(ScriptGame *game);
void ScriptGame_SetScoreLimit(ScriptGame *game, uint16_t limit);

const char *ScriptGame_GetServerIP(ScriptGame *game);
const char *ScriptGame_GetServerName(ScriptGame *game);
uint16_t ScriptGame_GetServerPort(ScriptGame *game);
const char *ScriptGame_GetServerVersion(ScriptGame *game);
const char *ScriptGame_GetServerInfo(ScriptGame *game);

float ScriptGame_GetGravity(ScriptGame *game);
void ScriptGame_SetGravity(ScriptGame *game, float grav);

bool ScriptGame_GetPaused(ScriptGame *game);
void ScriptGame_SetPaused(ScriptGame *game, bool paused);

int32_t ScriptGame_GetRespawnTime(ScriptGame *game);
void ScriptGame_SetRespawnTime(ScriptGame *game, int32_t time);

int32_t ScriptGame_GetMinRespawnTime(ScriptGame *game);
void ScriptGame_SetMinRespawnTime(ScriptGame *game, int32_t time);

int32_t ScriptGame_GetMaxRespawnTime(ScriptGame *game);
void ScriptGame_SetMaxRespawnTime(ScriptGame *game, int32_t time);

uint8_t ScriptGame_GetMaxGrenades(ScriptGame *game);
void ScriptGame_SetMaxGrenades(ScriptGame *game, uint8_t num);

uint8_t ScriptGame_GetBonus(ScriptGame *game);
void ScriptGame_SetBonus(ScriptGame *game, uint8_t num);

int32_t ScriptGame_GetTimeLimit(ScriptGame *game);
void ScriptGame_SetTimeLimit(ScriptGame *game, int32_t num);

int32_t ScriptGame_GetTimeLeft(ScriptGame *game);

bool ScriptGame_GetFriendlyFire(ScriptGame *game);
void ScriptGame_SetFriendlyFire(ScriptGame *game, bool enabled);

const char *ScriptGame_GetPassword(ScriptGame *game);
void ScriptGame_SetPassword(ScriptGame *game, const char *pass);

#ifdef ENABLE_RCON
const char *ScriptGame_GetAdminPassword(ScriptGame *game);
void ScriptGame_SetAdminPassword(ScriptGame *game, const char *pass);
#endif

uint8_t ScriptGame_GetVotePercent(ScriptGame *game);
void ScriptGame_SetVotePercent(ScriptGame *game, uint8_t percent);

bool ScriptGame_GetRealistic(ScriptGame *game);
void ScriptGame_SetRealistic(ScriptGame *game, bool enabled);

bool ScriptGame_GetSurvival(ScriptGame *game);
void ScriptGame_SetSurvival(ScriptGame *game, bool enabled);

bool ScriptGame_GetAdvance(ScriptGame *game);
void ScriptGame_SetAdvance(ScriptGame *game, bool enabled);

bool ScriptGame_GetBalance(ScriptGame *game);
void ScriptGame_SetBalance(ScriptGame *game, bool enabled);

int32_t ScriptGame_GetTickCount(ScriptGame *game);
int32_t ScriptGame_GetTickThreshold(ScriptGame *game);
void ScriptGame_SetTickThreshold(ScriptGame *game, int32_t threshold);

/* Team / Map / Ban list accessors (opaque) */
void *ScriptGame_GetTeam(ScriptGame *game, uint8_t id);          /* returns TScriptTeam* */
void *ScriptGame_GetMapsList(ScriptGame *game);                  /* returns TScriptMapsList* */
void *ScriptGame_GetBanLists(ScriptGame *game);                  /* returns TScriptBanLists* */

/* Callback registration */
void ScriptGame_SetOnClockTick(ScriptGame *game, ScriptGame_OnClockTick cb);
ScriptGame_OnClockTick ScriptGame_GetOnClockTick(ScriptGame *game);

void ScriptGame_SetOnIdle(ScriptGame *game, ScriptGame_OnIdle cb);
ScriptGame_OnIdle ScriptGame_GetOnIdle(ScriptGame *game);

void ScriptGame_SetOnJoin(ScriptGame *game, ScriptGame_OnJoin cb);
ScriptGame_OnJoin ScriptGame_GetOnJoin(ScriptGame *game);

void ScriptGame_SetOnLeave(ScriptGame *game, ScriptGame_OnLeave cb);
ScriptGame_OnLeave ScriptGame_GetOnLeave(ScriptGame *game);

void ScriptGame_SetOnRequest(ScriptGame *game, ScriptGame_OnRequest cb);
ScriptGame_OnRequest ScriptGame_GetOnRequest(ScriptGame *game);

void ScriptGame_SetOnAdminCommand(ScriptGame *game, ScriptGame_OnAdminCommand cb);
ScriptGame_OnAdminCommand ScriptGame_GetOnAdminCommand(ScriptGame *game);

void ScriptGame_SetOnTCPMessage(ScriptGame *game, ScriptGame_OnTCPMessage cb);
ScriptGame_OnTCPMessage ScriptGame_GetOnTCPMessage(ScriptGame *game);

void ScriptGame_SetOnTCPCommand(ScriptGame *game, ScriptGame_OnTCPCommand cb);
ScriptGame_OnTCPCommand ScriptGame_GetOnTCPCommand(ScriptGame *game);

void ScriptGame_SetOnAdminConnect(ScriptGame *game, ScriptGame_OnAdminConnect cb);
ScriptGame_OnAdminConnect ScriptGame_GetOnAdminConnect(ScriptGame *game);

void ScriptGame_SetOnAdminDisconnect(ScriptGame *game, ScriptGame_OnAdminDisconnect cb);
ScriptGame_OnAdminDisconnect ScriptGame_GetOnAdminDisconnect(ScriptGame *game);

#ifdef __cplusplus
}
#endif

#endif /* SCRIPTGAME_H */