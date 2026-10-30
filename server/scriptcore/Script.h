#ifndef SCRIPT_H
#define SCRIPT_H

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>
#include <pthread.h>

/* Forward declaration of vector type used by events */
typedef struct {
    float x;
    float y;
} Vector2;

/* Opaque script structure */
typedef struct Script Script;

/* Function pointer types for virtual methods */
typedef bool (*ScriptPrepareFunc)(Script *self);
typedef void (*ScriptLaunchFunc)(Script *self);
/* CallFunc is complex (variadic Variant), provide a simple placeholder */
typedef void* (*ScriptCallFuncFunc)(Script *self, void *params, const char *funcName, void *defaultReturn);

/* Event callbacks */
typedef void (*ScriptOnActivateServerFunc)(Script *self);
typedef void (*ScriptOnClockTickFunc)(Script *self);
typedef void (*ScriptOnIdleFunc)(Script *self);

typedef int (*ScriptOnRequestGameFunc)(Script *self, const char *ip, const char *hw, uint16_t port, uint8_t state, bool forwarded, const char *password);
typedef int8_t (*ScriptOnBeforeJoinTeamFunc)(Script *self, uint8_t id, uint8_t team, uint8_t oldTeam);
typedef void (*ScriptOnJoinTeamFunc)(Script *self, uint8_t id, uint8_t team, uint8_t oldTeam, bool joinGame);
typedef void (*ScriptOnLeaveGameFunc)(Script *self, uint8_t id, bool kicked);

typedef void (*ScriptOnBeforeMapChangeFunc)(Script *self, const char *map);
typedef void (*ScriptOnAfterMapChangeFunc)(Script *self, const char *map);

typedef void (*ScriptOnAdminConnectFunc)(Script *self, const char *ip, uint16_t port);
typedef void (*ScriptOnAdminDisconnectFunc)(Script *self, const char *ip, uint16_t port);
typedef void (*ScriptOnAdminMessageFunc)(Script *self, const char *ip, uint16_t port, const char *message);

typedef void (*ScriptOnFlagGrabFunc)(Script *self, uint8_t id, uint8_t teamFlag, bool grabbedInBase);
typedef void (*ScriptOnFlagScoreFunc)(Script *self, uint8_t id, uint8_t teamFlag);
typedef void (*ScriptOnFlagReturnFunc)(Script *self, uint8_t id, uint8_t teamFlag);
typedef void (*ScriptOnFlagDropFunc)(Script *self, uint8_t id, uint8_t teamFlag, bool thrown);

typedef void (*ScriptOnKitPickupFunc)(Script *self, uint8_t id, uint8_t kitId);

typedef Vector2 (*ScriptOnBeforePlayerRespawnFunc)(Script *self, uint8_t id);
typedef void (*ScriptOnAfterPlayerRespawnFunc)(Script *self, uint8_t id);

typedef float (*ScriptOnPlayerDamageFunc)(Script *self, uint8_t victim, uint8_t shooter, float damage, uint8_t bulletId);
typedef void (*ScriptOnPlayerKillFunc)(Script *self, uint8_t killer, uint8_t victim, uint8_t bulletId);
typedef void (*ScriptOnWeaponChangeFunc)(Script *self, uint8_t id, uint8_t primary, uint8_t secondary, uint8_t primaryAmmo, uint8_t secondaryAmmo);

typedef bool (*ScriptOnVoteMapStartFunc)(Script *self, uint8_t id, const char *map);
typedef bool (*ScriptOnVoteKickStartFunc)(Script *self, uint8_t id, uint8_t victim, const char *reason);
typedef void (*ScriptOnVoteMapFunc)(Script *self, uint8_t id, const char *map);
typedef void (*ScriptOnVoteKickFunc)(Script *self, uint8_t id, uint8_t victim);

typedef void (*ScriptOnPlayerSpeakFunc)(Script *self, uint8_t id, const char *text);
typedef bool (*ScriptOnPlayerCommandFunc)(Script *self, uint8_t id, const char *command);
typedef bool (*ScriptOnConsoleCommandFunc)(Script *self, const char *ip, uint16_t port, const char *command);

/* Script structure definition */
struct Script {
    /* Properties */
    char *name;
    char *dir;
    pthread_mutex_t lock;

    /* Virtual method table */
    ScriptPrepareFunc prepare;
    ScriptLaunchFunc launch;
    ScriptCallFuncFunc callFunc;

    /* Event callbacks (default implementations provided) */
    ScriptOnActivateServerFunc onActivateServer;
    ScriptOnClockTickFunc onClockTick;
    ScriptOnIdleFunc onIdle;

    ScriptOnRequestGameFunc onRequestGame;
    ScriptOnBeforeJoinTeamFunc onBeforeJoinTeam;
    ScriptOnJoinTeamFunc onJoinTeam;
    ScriptOnLeaveGameFunc onLeaveGame;

    ScriptOnBeforeMapChangeFunc onBeforeMapChange;
    ScriptOnAfterMapChangeFunc onAfterMapChange;

    ScriptOnAdminConnectFunc onAdminConnect;
    ScriptOnAdminDisconnectFunc onAdminDisconnect;
    ScriptOnAdminMessageFunc onAdminMessage;

    ScriptOnFlagGrabFunc onFlagGrab;
    ScriptOnFlagScoreFunc onFlagScore;
    ScriptOnFlagReturnFunc onFlagReturn;
    ScriptOnFlagDropFunc onFlagDrop;

    ScriptOnKitPickupFunc onKitPickup;

    ScriptOnBeforePlayerRespawnFunc onBeforePlayerRespawn;
    ScriptOnAfterPlayerRespawnFunc onAfterPlayerRespawn;

    ScriptOnPlayerDamageFunc onPlayerDamage;
    ScriptOnPlayerKillFunc onPlayerKill;
    ScriptOnWeaponChangeFunc onWeaponChange;

    ScriptOnVoteMapStartFunc onVoteMapStart;
    ScriptOnVoteKickStartFunc onVoteKickStart;
    ScriptOnVoteMapFunc onVoteMap;
    ScriptOnVoteKickFunc onVoteKick;

    ScriptOnPlayerSpeakFunc onPlayerSpeak;
    ScriptOnPlayerCommandFunc onPlayerCommand;
    ScriptOnConsoleCommandFunc onConsoleCommand;
};

/* Constructor / Destructor */
Script *Script_Create(void);
void Script_Destroy(Script *script);

/* Default virtual method implementations (can be overridden by derived scripts) */
bool Script_DefaultPrepare(Script *self);
void Script_DefaultLaunch(Script *self);
void *Script_DefaultCallFunc(Script *self, void *params, const char *funcName, void *defaultReturn);

/* Default event implementations */
void Script_DefaultOnActivateServer(Script *self);
void Script_DefaultOnClockTick(Script *self);
void Script_DefaultOnIdle(Script *self);

int Script_DefaultOnRequestGame(Script *self, const char *ip, const char *hw, uint16_t port, uint8_t state, bool forwarded, const char *password);
int8_t Script_DefaultOnBeforeJoinTeam(Script *self, uint8_t id, uint8_t team, uint8_t oldTeam);
void Script_DefaultOnJoinTeam(Script *self, uint8_t id, uint8_t team, uint8_t oldTeam, bool joinGame);
void Script_DefaultOnLeaveGame(Script *self, uint8_t id, bool kicked);

void Script_DefaultOnBeforeMapChange(Script *self, const char *map);
void Script_DefaultOnAfterMapChange(Script *self, const char *map);

void Script_DefaultOnAdminConnect(Script *self, const char *ip, uint16_t port);
void Script_DefaultOnAdminDisconnect(Script *self, const char *ip, uint16_t port);
void Script_DefaultOnAdminMessage(Script *self, const char *ip, uint16_t port, const char *message);

void Script_DefaultOnFlagGrab(Script *self, uint8_t id, uint8_t teamFlag, bool grabbedInBase);
void Script_DefaultOnFlagScore(Script *self, uint8_t id, uint8_t teamFlag);
void Script_DefaultOnFlagReturn(Script *self, uint8_t id, uint8_t teamFlag);
void Script_DefaultOnFlagDrop(Script *self, uint8_t id, uint8_t teamFlag, bool thrown);

void Script_DefaultOnKitPickup(Script *self, uint8_t id, uint8_t kitId);

Vector2 Script_DefaultOnBeforePlayerRespawn(Script *self, uint8_t id);
void Script_DefaultOnAfterPlayerRespawn(Script *self, uint8_t id);

float Script_DefaultOnPlayerDamage(Script *self, uint8_t victim, uint8_t shooter, float damage, uint8_t bulletId);
void Script_DefaultOnPlayerKill(Script *self, uint8_t killer, uint8_t victim, uint8_t bulletId);
void Script_DefaultOnWeaponChange(Script *self, uint8_t id, uint8_t primary, uint8_t secondary, uint8_t primaryAmmo, uint8_t secondaryAmmo);

bool Script_DefaultOnVoteMapStart(Script *self, uint8_t id, const char *map);
bool Script_DefaultOnVoteKickStart(Script *self, uint8_t id, uint8_t victim, const char *reason);
void Script_DefaultOnVoteMap(Script *self, uint8_t id, const char *map);
void Script_DefaultOnVoteKick(Script *self, uint8_t id, uint8_t victim);

void Script_DefaultOnPlayerSpeak(Script *self, uint8_t id, const char *text);
bool Script_DefaultOnPlayerCommand(Script *self, uint8_t id, const char *command);
bool Script_DefaultOnConsoleCommand(Script *self, const char *ip, uint16_t port, const char *command);

#endif /* SCRIPT_H */