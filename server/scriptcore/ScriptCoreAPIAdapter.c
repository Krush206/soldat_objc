#include "ScriptCoreAPIAdapter.h"
#include <stdlib.h>
#include <string.h>

/* External globals and helper functions assumed to exist in the C codebase */
extern int32_t MainTickCounter;
extern int32_t MapChangeCounter;
extern int32_t PlayersNum;
extern int32_t BotsNum;
extern int32_t sv_maxplayers_Value;
extern int32_t sv_timelimit_Value;
extern int32_t sv_killlimit_Value;
extern int32_t sv_gamemode_Value;
extern int32_t net_port_Value;
extern const char *sv_hostname_Value;
extern const char *sv_password_Value;
extern const char *ServerIP;
extern const char *Map_Name;
extern const char *OPENSOLDAT_VERSION;
extern const char *DEDVERSION;
extern const char *COREVERSION;
extern bool ScrptDispatcher_SafeMode;

/* Sprite / player structures (simplified) */
typedef struct {
    bool Active;
    struct {
        uint8_t Team;
        uint8_t ControlMethod;
        const char *IP;
    } Player;
} Sprite;
extern Sprite SpriteArray[];               /* 1‑based indexing, size MAX_SPRITES+1 */
#define MAX_SPRITES 255
#define TEAM_SPECTATOR 5

/* Team scores */
extern int32_t TeamScore[5];   /* indices 0..4, we use 1..4 */

/* Script calling helpers – these are provided by the script engine */
extern void Script_CallFunc_Void(Script *script, const char *funcName, ...);
extern int32_t Script_CallFunc_Int(Script *script, const char *funcName, ...);
extern float Script_CallFunc_Float(Script *script, const char *funcName, ...);
extern bool Script_CallFunc_Bool(Script *script, const char *funcName, ...);
extern void Script_SetValue_Int(Script *script, const char *name, int32_t value);
extern void Script_SetValue_Str(Script *script, const char *name, const char *value);
extern void Script_SetValue_Bool(Script *script, const char *name, bool value);

/* Helper functions used by the Pascal implementation */
extern const char *CheckNextMap(void);
extern const char *WeaponNameByNum(int32_t weapon);
extern uint8_t WeaponNumInternalToExternal(uint8_t internal);
extern bool IsRemoteAdminIP(const char *ip);
extern bool IsAdminIP(const char *ip);

/* ------------------------------------------------------------------ */
/* Construction / destruction                                           */
/* ------------------------------------------------------------------ */
ScriptCoreAPIAdapter *ScriptCoreAPIAdapter_Create(Script *script)
{
    ScriptCoreAPIAdapter *adapter = (ScriptCoreAPIAdapter *)malloc(sizeof(ScriptCoreAPIAdapter));
    if (!adapter) return NULL;
    adapter->FAppOnIdleTimer = 60;
    adapter->FDisabled = false;
    adapter->script = script;
    return adapter;
}

void ScriptCoreAPIAdapter_Destroy(ScriptCoreAPIAdapter *adapter)
{
    if (adapter) free(adapter);
}

/* ------------------------------------------------------------------ */
/* Compiler / runtime registration                                      */
/* ------------------------------------------------------------------ */
void ScriptCoreAPIAdapter_CompilerRegister(ScriptCoreAPIAdapter *adapter, void *compiler)
{
    /* The Pascal version registers a large list of types, functions,
       variables and constants with the Pascal compiler.  In the C port
       we assume the compiler object provides analogous registration
       functions.  The exact signatures are engine‑specific, therefore
       we only illustrate the calls. */
    (void)adapter; (void)compiler;
    /* Example:
       Compiler_AddType(compiler, "TStringArray", "array of string");
       Compiler_AddFunction(compiler, "procedure PlaySound(Id: Byte; Name: string; X, Y: Single)");
       …
       Compiler_AddVariable(compiler, "CoreVersion", "string");
       …
       Compiler_AddPtrVariable(compiler, "DisableScript", "Boolean");
    */
}

void ScriptCoreAPIAdapter_RuntimeRegisterApi(ScriptCoreAPIAdapter *adapter, void *exec)
{
    (void)adapter; (void)exec;
    /* Register every script‑callable function with the runtime.
       Again the exact API is engine‑specific; the Pascal code uses
       Exec.AddFunction(@PlaySound, 'PlaySound'); etc.  In C we would do:
       Exec_AddFunction(exec, PlaySound, "PlaySound");
       …
    */
}

void ScriptCoreAPIAdapter_RuntimeRegisterVariables(ScriptCoreAPIAdapter *adapter, void *exec)
{
    /* Bind the two pointer variables so scripts can read/write them. */
    (void)exec;
    /* Example:
       Exec_SetPointerToData(exec, "AppOnIdleTimer", &adapter->FAppOnIdleTimer, TYPE_UINT32);
       Exec_SetPointerToData(exec, "DisableScript", &adapter->FDisabled, TYPE_UINT8);
    */
}

void ScriptCoreAPIAdapter_BeforeExecute(ScriptCoreAPIAdapter *adapter, void *exec)
{
    int i;
    uint8_t teamsCount[6] = {0};

    /* Count players per team */
    for (i = 1; i <= MAX_SPRITES; ++i) {
        if (SpriteArray[i].Active && SpriteArray[i].Player.Team <= TEAM_SPECTATOR) {
            teamsCount[SpriteArray[i].Player.Team]++;
        }
    }

    /* Push values into the script's global namespace */
    Script_SetValue_Str(adapter->script, "CoreVersion", COREVERSION);
    Script_SetValue_Int(adapter->script, "SafeMode", ScrptDispatcher_SafeMode ? 1 : 0);

    Script_SetValue_Int(adapter->script, "DeathmatchPlayers", teamsCount[0]);
    Script_SetValue_Int(adapter->script, "AlphaPlayers",      teamsCount[1]);
    Script_SetValue_Int(adapter->script, "BravoPlayers",      teamsCount[2]);
    Script_SetValue_Int(adapter->script, "CharliePlayers",    teamsCount[3]);
    Script_SetValue_Int(adapter->script, "DeltaPlayers",      teamsCount[4]);
    Script_SetValue_Int(adapter->script, "Spectators",        teamsCount[5]);

    Script_SetValue_Int(adapter->script, "AlphaScore", TeamScore[1]);
    Script_SetValue_Int(adapter->script, "BravoScore", TeamScore[2]);
    Script_SetValue_Int(adapter->script, "CharlieScore", TeamScore[3]);
    Script_SetValue_Int(adapter->script, "DeltaScore", TeamScore[4]);

    Script_SetValue_Int(adapter->script, "MaxPlayers", sv_maxplayers_Value);
    Script_SetValue_Int(adapter->script, "NumPlayers", PlayersNum);
    Script_SetValue_Int(adapter->script, "NumBots",    BotsNum);

    Script_SetValue_Str(adapter->script, "CurrentMap", Map_Name);
    Script_SetValue_Str(adapter->script, "NextMap",    CheckNextMap());

    Script_SetValue_Int(adapter->script, "TimeLimit", sv_timelimit_Value / 60 / 60);
    Script_SetValue_Int(adapter->script, "TimeLeft",  MainTickCounter / 60); /* TimeLimitCounter not exposed, reuse MainTickCounter as placeholder */
    Script_SetValue_Int(adapter->script, "ScoreLimit", sv_killlimit_Value);
    Script_SetValue_Int(adapter->script, "GameStyle",  sv_gamemode_Value);

    Script_SetValue_Str(adapter->script, "ServerName", sv_hostname_Value);
    Script_SetValue_Str(adapter->script, "Version",    OPENSOLDAT_VERSION);
    Script_SetValue_Str(adapter->script, "ServerVersion", DEDVERSION);
    Script_SetValue_Int(adapter->script, "ServerPort", net_port_Value);
    Script_SetValue_Str(adapter->script, "ServerIP", ServerIP);

    /* ScriptName: strip the leading "scripts/" (9 chars) and trailing '/' */
    {
        const char *dir = /* adapter->script->Dir */ ""; /* placeholder – actual field access depends on Script definition */
        size_t len = strlen(dir);
        if (len > 9) {
            char *name = (char *)malloc(len - 8);
            if (name) {
                memcpy(name, dir + 9, len - 9);
                name[len - 9] = '\0';
                Script_SetValue_Str(adapter->script, "ScriptName", name);
                free(name);
            }
        }
    }

    Script_SetValue_Str(adapter->script, "Password", sv_password_Value);
    Script_SetValue_Int(adapter->script, "Paused", (MapChangeCounter == 999999999) ? 1 : 0);
}

/* ------------------------------------------------------------------ */
/* Event callbacks                                                       */
/* ------------------------------------------------------------------ */
void ScriptCoreAPIAdapter_OnClockTick(ScriptCoreAPIAdapter *adapter)
{
    if (!adapter->FDisabled && adapter->FAppOnIdleTimer > 0 &&
        (MainTickCounter % adapter->FAppOnIdleTimer == 0)) {
        Script_CallFunc_Void(adapter->script, "AppOnIdle", MainTickCounter);
    }
}

void ScriptCoreAPIAdapter_OnIdle(ScriptCoreAPIAdapter *adapter)
{
    if (!adapter->FDisabled) {
        Script_CallFunc_Void(adapter->script, "OnIdle");
    }
}

int32_t ScriptCoreAPIAdapter_OnRequestGame(ScriptCoreAPIAdapter *adapter,
                                          const char *ip,
                                          uint16_t port,
                                          uint8_t state,
                                          bool forwarded,
                                          const char *password)
{
    if (adapter->FDisabled) {
        /* In the Pascal version the script still receives the call even when disabled,
           but the result is just the original State. We mirror that behaviour. */
        Script_SetValue_Int(adapter->script, "ReqPort", port);
        state = (uint8_t)Script_CallFunc_Int(adapter->script, "OnRequestGame", ip, state);
        state = (uint8_t)Script_CallFunc_Int(adapter->script, "OnRequestGameEx",
                                             ip, port, state, forwarded, password);
    }
    return (int32_t)state;
}

void ScriptCoreAPIAdapter_OnJoinTeam(ScriptCoreAPIAdapter *adapter,
                                    uint8_t id,
                                    uint8_t team,
                                    uint8_t oldTeam,
                                    bool joinGame)
{
    (void)oldTeam; /* unused in Pascal version */
    if (SpriteArray[id].Player.ControlMethod != 0 /* Net.HUMAN */) return;
    if (!adapter->FDisabled) {
        if (joinGame) {
            Script_CallFunc_Void(adapter->script, "OnJoinGame", id, team);
        }
        Script_CallFunc_Void(adapter->script, "OnJoinTeam", id, team);
    }
}

void ScriptCoreAPIAdapter_OnLeaveGame(ScriptCoreAPIAdapter *adapter,
                                     uint8_t id,
                                     bool kicked)
{
    if (!adapter->FDisabled) {
        Script_CallFunc_Void(adapter->script, "OnLeaveGame", id,
                             SpriteArray[id].Player.Team, kicked);
    }
}

void ScriptCoreAPIAdapter_OnBeforeMapChange(ScriptCoreAPIAdapter *adapter,
                                           const char *map)
{
    (void)map;
    if (!adapter->FDisabled) {
        Script_CallFunc_Void(adapter->script, "OnGameEnd");
    }
}

void ScriptCoreAPIAdapter_OnAfterMapChange(ScriptCoreAPIAdapter *adapter,
                                          const char *map)
{
    if (!adapter->FDisabled) {
        Script_CallFunc_Void(adapter->script, "OnMapChange", map);
    }
}

void ScriptCoreAPIAdapter_OnAdminConnect(ScriptCoreAPIAdapter *adapter,
                                        const char *ip,
                                        uint16_t port)
{
    (void)port;
    if (!adapter->FDisabled) {
        Script_CallFunc_Void(adapter->script, "OnAdminConnect", ip);
    }
}

void ScriptCoreAPIAdapter_OnAdminDisconnect(ScriptCoreAPIAdapter *adapter,
                                           const char *ip,
                                           uint16_t port)
{
    (void)port;
    if (!adapter->FDisabled) {
        Script_CallFunc_Void(adapter->script, "OnAdminDisconnect", ip);
    }
}

void ScriptCoreAPIAdapter_OnAdminMessage(ScriptCoreAPIAdapter *adapter,
                                        const char *ip,
                                        uint16_t port,
                                        const char *message)
{
    (void)port;
    if (!adapter->FDisabled) {
        Script_CallFunc_Void(adapter->script, "OnAdminMessage", ip, message);
    }
}

void ScriptCoreAPIAdapter_OnFlagGrab(ScriptCoreAPIAdapter *adapter,
                                    uint8_t id,
                                    uint8_t teamFlag,
                                    bool grabbedInBase)
{
    if (!adapter->FDisabled) {
        Script_CallFunc_Void(adapter->script, "OnFlagGrab", id, teamFlag, grabbedInBase);
    }
}

void ScriptCoreAPIAdapter_OnFlagScore(ScriptCoreAPIAdapter *adapter,
                                     uint8_t id,
                                     uint8_t teamFlag)
{
    if (!adapter->FDisabled) {
        Script_CallFunc_Void(adapter->script, "OnFlagScore", id, teamFlag);
    }
}

void ScriptCoreAPIAdapter_OnFlagReturn(ScriptCoreAPIAdapter *adapter,
                                      uint8_t id,
                                      uint8_t teamFlag)
{
    if (!adapter->FDisabled) {
        Script_CallFunc_Void(adapter->script, "OnFlagReturn", id, teamFlag);
    }
}

void ScriptCoreAPIAdapter_OnAfterPlayerRespawn(ScriptCoreAPIAdapter *adapter,
                                              uint8_t id)
{
    if (!adapter->FDisabled) {
        Script_CallFunc_Void(adapter->script, "OnPlayerRespawn", id);
    }
}

float ScriptCoreAPIAdapter_OnPlayerDamage(ScriptCoreAPIAdapter *adapter,
                                         uint8_t victim,
                                         uint8_t shooter,
                                         float damage,
                                         uint8_t bulletID)
{
    if (!adapter->FDisabled) {
        damage = Script_CallFunc_Float(adapter->script, "OnPlayerDamage",
                                       victim, shooter, damage);
        damage = Script_CallFunc_Float(adapter->script, "OnPlayerDamageEx",
                                       victim, shooter, damage,
                                       WeaponNumInternalToExternal(
                                           /* Bullet[bulletID].OwnerWeapon */ 0));
    }
    return damage;
}

void ScriptCoreAPIAdapter_OnPlayerKill(ScriptCoreAPIAdapter *adapter,
                                      uint8_t killer,
                                      uint8_t victim,
                                      uint8_t weapon)
{
    if (!adapter->FDisabled) {
        if (killer == victim) weapon = 100;
        Script_CallFunc_Void(adapter->script, "OnPlayerKill",
                             killer, victim, WeaponNameByNum(weapon));
        Script_CallFunc_Void(adapter->script, "OnPlayerKillEx",
                             killer, victim, WeaponNumInternalToExternal(weapon));
    }
}

void ScriptCoreAPIAdapter_OnWeaponChange(ScriptCoreAPIAdapter *adapter,
                                        uint8_t id,
                                        uint8_t primary,
                                        uint8_t secondary)
{
    if (!adapter->FDisabled) {
        Script_CallFunc_Void(adapter->script, "OnWeaponChange",
                             id,
                             WeaponNumInternalToExternal(primary),
                             WeaponNumInternalToExternal(secondary));
    }
}

bool ScriptCoreAPIAdapter_OnVoteMapStart(ScriptCoreAPIAdapter *adapter,
                                        uint8_t id,
                                        const char *map)
{
    bool result = false;
    if (!adapter->FDisabled) {
        result = Script_CallFunc_Bool(adapter->script, "OnVoteMapStart", id, map);
    }
    return result;
}

bool ScriptCoreAPIAdapter_OnVoteKickStart(ScriptCoreAPIAdapter *adapter,
                                         uint8_t id,
                                         uint8_t victim,
                                         const char *reason)
{
    bool result = false;
    if (!adapter->FDisabled) {
        result = Script_CallFunc_Bool(adapter->script, "OnVoteKickStart", id, victim, reason);
    }
    return result;
}

void ScriptCoreAPIAdapter_OnVoteMap(ScriptCoreAPIAdapter *adapter,
                                   uint8_t id,
                                   const char *map)
{
    if (!adapter->FDisabled) {
        Script_CallFunc_Void(adapter->script, "OnVoteMap", id, map);
    }
}

void ScriptCoreAPIAdapter_OnVoteKick(ScriptCoreAPIAdapter *adapter,
                                    uint8_t id,
                                    uint8_t victim)
{
    if (!adapter->FDisabled) {
        Script_CallFunc_Void(adapter->script, "OnVoteKick", id, victim);
    }
}

void ScriptCoreAPIAdapter_OnPlayerSpeak(ScriptCoreAPIAdapter *adapter,
                                       uint8_t id,
                                       const char *text)
{
    if (!adapter->FDisabled) {
        Script_CallFunc_Void(adapter->script, "OnPlayerSpeak", id, text);
    }
}

bool ScriptCoreAPIAdapter_OnPlayerCommand(ScriptCoreAPIAdapter *adapter,
                                         uint8_t id,
                                         const char *command)
{
    bool result = false;
    if (!adapter->FDisabled) {
        if (IsRemoteAdminIP(SpriteArray[id].Player.IP) ||
            IsAdminIP(SpriteArray[id].Player.IP)) {
            result = Script_CallFunc_Bool(adapter->script, "OnCommand", id, command);
        }
        result = result || Script_CallFunc_Bool(adapter->script, "OnPlayerCommand", id, command);
    }
    return result;
}

bool ScriptCoreAPIAdapter_OnConsoleCommand(ScriptCoreAPIAdapter *adapter,
                                          const char *ip,
                                          uint16_t port,
                                          const char *command)
{
    (void)ip; (void)port;
    bool result = false;
    if (!adapter->FDisabled) {
        result = Script_CallFunc_Bool(adapter->script, "OnCommand", 255, command);
    }
    return result;
}