#ifndef SCRIPTCORE_H
#define SCRIPTCORE_H

#include <stdbool.h>
#include <stdint.h>

/* Opaque forward declaration */
typedef struct ScriptCore ScriptCore;

/* Public API */
ScriptCore* ScriptCore_CheckFunction(const char* dir);
ScriptCore* ScriptCore_Create(const char* dir);
void ScriptCore_Destroy(ScriptCore* self);
bool ScriptCore_Prepare(ScriptCore* self);
void ScriptCore_Launch(ScriptCore* self);
int ScriptCore_CallFunc(ScriptCore* self, const void* params, const char* funcName, int defaultReturn);
void ScriptCore_OnClockTick(ScriptCore* self);
void ScriptCore_OnIdle(ScriptCore* self);
int ScriptCore_OnRequestGame(ScriptCore* self, const char* ip, const char* hw, uint16_t port, uint8_t state, bool forwarded, const char* password);
void ScriptCore_OnJoinTeam(ScriptCore* self, uint8_t id, uint8_t team, uint8_t oldTeam, bool joinGame);
void ScriptCore_OnLeaveGame(ScriptCore* self, uint8_t id, bool kicked);
void ScriptCore_OnBeforeMapChange(ScriptCore* self, const char* map);
void ScriptCore_OnAfterMapChange(ScriptCore* self, const char* map);
void ScriptCore_OnAdminConnect(ScriptCore* self, const char* ip, uint16_t port);
void ScriptCore_OnAdminDisconnect(ScriptCore* self, const char* ip, uint16_t port);
void ScriptCore_OnAdminMessage(ScriptCore* self, const char* ip, uint16_t port, const char* msg);
void ScriptCore_OnFlagGrab(ScriptCore* self, uint8_t id, uint8_t teamFlag, bool grabbedInBase);
void ScriptCore_OnFlagScore(ScriptCore* self, uint8_t id, uint8_t teamFlag);
void ScriptCore_OnFlagReturn(ScriptCore* self, uint8_t id, uint8_t teamFlag);
void ScriptCore_OnAfterPlayerRespawn(ScriptCore* self, uint8_t id);
float ScriptCore_OnPlayerDamage(ScriptCore* self, uint8_t victim, uint8_t shooter, float damage, uint8_t bulletID);
void ScriptCore_OnPlayerKill(ScriptCore* self, uint8_t killer, uint8_t victim, uint8_t bulletID);
void ScriptCore_OnWeaponChange(ScriptCore* self, uint8_t id, uint8_t primary, uint8_t secondary, uint8_t primaryAmmo, uint8_t secondaryAmmo);
bool ScriptCore_OnVoteMapStart(ScriptCore* self, uint8_t id, const char* map);
bool ScriptCore_OnVoteKickStart(ScriptCore* self, uint8_t id, uint8_t victim, const char* reason);
void ScriptCore_OnVoteMap(ScriptCore* self, uint8_t id, const char* map);
void ScriptCore_OnVoteKick(ScriptCore* self, uint8_t id, uint8_t victim);
void ScriptCore_OnPlayerSpeak(ScriptCore* self, uint8_t id, const char* text);
bool ScriptCore_OnPlayerCommand(ScriptCore* self, uint8_t id, const char* command);
bool ScriptCore_OnConsoleCommand(ScriptCore* self, const char* ip, uint16_t port, const char* command);

#endif /* SCRIPTCORE_H */