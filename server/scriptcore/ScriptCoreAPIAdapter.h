#ifndef SCRIPTCORE_API_ADAPTER_H
#define SCRIPTCORE_API_ADAPTER_H

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Forward declaration of the script context used by the adapter */
typedef struct Script Script;

/* Adapter structure mirroring the Pascal class */
typedef struct ScriptCoreAPIAdapter {
    int32_t FAppOnIdleTimer;   /* Longint in Pascal */
    bool    FDisabled;         /* Boolean in Pascal */
    Script *script;            /* Pointer to the owning TScript instance */
} ScriptCoreAPIAdapter;

/* Construction / destruction */
ScriptCoreAPIAdapter *ScriptCoreAPIAdapter_Create(Script *script);
void ScriptCoreAPIAdapter_Destroy(ScriptCoreAPIAdapter *adapter);

/* Compiler / runtime registration */
void ScriptCoreAPIAdapter_CompilerRegister(ScriptCoreAPIAdapter *adapter, void *compiler);
void ScriptCoreAPIAdapter_RuntimeRegisterApi(ScriptCoreAPIAdapter *adapter, void *exec);
void ScriptCoreAPIAdapter_RuntimeRegisterVariables(ScriptCoreAPIAdapter *adapter, void *exec);
void ScriptCoreAPIAdapter_BeforeExecute(ScriptCoreAPIAdapter *adapter, void *exec);

/* Event callbacks */
void ScriptCoreAPIAdapter_OnClockTick(ScriptCoreAPIAdapter *adapter);
void ScriptCoreAPIAdapter_OnIdle(ScriptCoreAPIAdapter *adapter);
int32_t ScriptCoreAPIAdapter_OnRequestGame(ScriptCoreAPIAdapter *adapter,
                                          const char *ip,
                                          uint16_t port,
                                          uint8_t state,
                                          bool forwarded,
                                          const char *password);
void ScriptCoreAPIAdapter_OnJoinTeam(ScriptCoreAPIAdapter *adapter,
                                    uint8_t id,
                                    uint8_t team,
                                    uint8_t oldTeam,
                                    bool joinGame);
void ScriptCoreAPIAdapter_OnLeaveGame(ScriptCoreAPIAdapter *adapter,
                                     uint8_t id,
                                     bool kicked);
void ScriptCoreAPIAdapter_OnBeforeMapChange(ScriptCoreAPIAdapter *adapter,
                                           const char *map);
void ScriptCoreAPIAdapter_OnAfterMapChange(ScriptCoreAPIAdapter *adapter,
                                          const char *map);
void ScriptCoreAPIAdapter_OnAdminConnect(ScriptCoreAPIAdapter *adapter,
                                        const char *ip,
                                        uint16_t port);
void ScriptCoreAPIAdapter_OnAdminDisconnect(ScriptCoreAPIAdapter *adapter,
                                           const char *ip,
                                           uint16_t port);
void ScriptCoreAPIAdapter_OnAdminMessage(ScriptCoreAPIAdapter *adapter,
                                        const char *ip,
                                        uint16_t port,
                                        const char *message);
void ScriptCoreAPIAdapter_OnFlagGrab(ScriptCoreAPIAdapter *adapter,
                                    uint8_t id,
                                    uint8_t teamFlag,
                                    bool grabbedInBase);
void ScriptCoreAPIAdapter_OnFlagScore(ScriptCoreAPIAdapter *adapter,
                                     uint8_t id,
                                     uint8_t teamFlag);
void ScriptCoreAPIAdapter_OnFlagReturn(ScriptCoreAPIAdapter *adapter,
                                      uint8_t id,
                                      uint8_t teamFlag);
void ScriptCoreAPIAdapter_OnAfterPlayerRespawn(ScriptCoreAPIAdapter *adapter,
                                              uint8_t id);
float ScriptCoreAPIAdapter_OnPlayerDamage(ScriptCoreAPIAdapter *adapter,
                                         uint8_t victim,
                                         uint8_t shooter,
                                         float damage,
                                         uint8_t bulletID);
void ScriptCoreAPIAdapter_OnPlayerKill(ScriptCoreAPIAdapter *adapter,
                                      uint8_t killer,
                                      uint8_t victim,
                                      uint8_t weapon);
void ScriptCoreAPIAdapter_OnWeaponChange(ScriptCoreAPIAdapter *adapter,
                                        uint8_t id,
                                        uint8_t primary,
                                        uint8_t secondary);
bool ScriptCoreAPIAdapter_OnVoteMapStart(ScriptCoreAPIAdapter *adapter,
                                        uint8_t id,
                                        const char *map);
bool ScriptCoreAPIAdapter_OnVoteKickStart(ScriptCoreAPIAdapter *adapter,
                                         uint8_t id,
                                         uint8_t victim,
                                         const char *reason);
void ScriptCoreAPIAdapter_OnVoteMap(ScriptCoreAPIAdapter *adapter,
                                   uint8_t id,
                                   const char *map);
void ScriptCoreAPIAdapter_OnVoteKick(ScriptCoreAPIAdapter *adapter,
                                    uint8_t id,
                                    uint8_t victim);
void ScriptCoreAPIAdapter_OnPlayerSpeak(ScriptCoreAPIAdapter *adapter,
                                       uint8_t id,
                                       const char *text);
bool ScriptCoreAPIAdapter_OnPlayerCommand(ScriptCoreAPIAdapter *adapter,
                                         uint8_t id,
                                         const char *command);
bool ScriptCoreAPIAdapter_OnConsoleCommand(ScriptCoreAPIAdapter *adapter,
                                          const char *ip,
                                          uint16_t port,
                                          const char *command);

/* Property accessors */
static inline int32_t ScriptCoreAPIAdapter_GetAppOnIdleTimer(const ScriptCoreAPIAdapter *adapter) {
    return adapter->FAppOnIdleTimer;
}
static inline bool ScriptCoreAPIAdapter_GetDisabled(const ScriptCoreAPIAdapter *adapter) {
    return adapter->FDisabled;
}
static inline void ScriptCoreAPIAdapter_SetDisabled(ScriptCoreAPIAdapter *adapter, bool value) {
    adapter->FDisabled = value;
}

#ifdef __cplusplus
}
#endif

#endif /* SCRIPTCORE_API_ADAPTER_H */