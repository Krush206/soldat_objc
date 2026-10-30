#include "Script.h"
#include <stdlib.h>
#include <string.h>

/* Helper to duplicate a string or return NULL */
static char *strdup_or_null(const char *s) {
    return s ? strdup(s) : NULL;
}

/* Constructor */
Script *Script_Create(void) {
    Script *self = (Script *)calloc(1, sizeof(Script));
    if (!self) return NULL;

    self->name = NULL;
    self->dir = NULL;
    pthread_mutex_init(&self->lock, NULL);

    /* Assign default virtual methods */
    self->prepare = Script_DefaultPrepare;
    self->launch = Script_DefaultLaunch;
    self->callFunc = Script_DefaultCallFunc;

    /* Assign default event handlers */
    self->onActivateServer = Script_DefaultOnActivateServer;
    self->onClockTick = Script_DefaultOnClockTick;
    self->onIdle = Script_DefaultOnIdle;

    self->onRequestGame = Script_DefaultOnRequestGame;
    self->onBeforeJoinTeam = Script_DefaultOnBeforeJoinTeam;
    self->onJoinTeam = Script_DefaultOnJoinTeam;
    self->onLeaveGame = Script_DefaultOnLeaveGame;

    self->onBeforeMapChange = Script_DefaultOnBeforeMapChange;
    self->onAfterMapChange = Script_DefaultOnAfterMapChange;

    self->onAdminConnect = Script_DefaultOnAdminConnect;
    self->onAdminDisconnect = Script_DefaultOnAdminDisconnect;
    self->onAdminMessage = Script_DefaultOnAdminMessage;

    self->onFlagGrab = Script_DefaultOnFlagGrab;
    self->onFlagScore = Script_DefaultOnFlagScore;
    self->onFlagReturn = Script_DefaultOnFlagReturn;
    self->onFlagDrop = Script_DefaultOnFlagDrop;

    self->onKitPickup = Script_DefaultOnKitPickup;

    self->onBeforePlayerRespawn = Script_DefaultOnBeforePlayerRespawn;
    self->onAfterPlayerRespawn = Script_DefaultOnAfterPlayerRespawn;

    self->onPlayerDamage = Script_DefaultOnPlayerDamage;
    self->onPlayerKill = Script_DefaultOnPlayerKill;
    self->onWeaponChange = Script_DefaultOnWeaponChange;

    self->onVoteMapStart = Script_DefaultOnVoteMapStart;
    self->onVoteKickStart = Script_DefaultOnVoteKickStart;
    self->onVoteMap = Script_DefaultOnVoteMap;
    self->onVoteKick = Script_DefaultOnVoteKick;

    self->onPlayerSpeak = Script_DefaultOnPlayerSpeak;
    self->onPlayerCommand = Script_DefaultOnPlayerCommand;
    self->onConsoleCommand = Script_DefaultOnConsoleCommand;

    return self;
}

/* Destructor */
void Script_Destroy(Script *self) {
    if (!self) return;

    free(self->name);
    free(self->dir);
    pthread_mutex_destroy(&self->lock);
    free(self);
}

/* Default virtual method implementations */
bool Script_DefaultPrepare(Script *self) {
    (void)self;
    return false; /* Abstract in Pascal – default false */
}

void Script_DefaultLaunch(Script *self) {
    (void)self;
    /* Abstract – do nothing */
}

void *Script_DefaultCallFunc(Script *self, void *params, const char *funcName, void *defaultReturn) {
    (void)self; (void)params; (void)funcName;
    return defaultReturn;
}

/* Default event implementations */
void Script_DefaultOnActivateServer(Script *self) { (void)self; }
void Script_DefaultOnClockTick(Script *self) { (void)self; }
void Script_DefaultOnIdle(Script *self) { (void)self; }

int Script_DefaultOnRequestGame(Script *self, const char *ip, const char *hw, uint16_t port, uint8_t state, bool forwarded, const char *password) {
    (void)self; (void)ip; (void)hw; (void)port; (void)forwarded; (void)password;
    return (int)state;
}

int8_t Script_DefaultOnBeforeJoinTeam(Script *self, uint8_t id, uint8_t team, uint8_t oldTeam) {
    (void)self; (void)id; (void)oldTeam;
    return (int8_t)team;
}

void Script_DefaultOnJoinTeam(Script *self, uint8_t id, uint8_t team, uint8_t oldTeam, bool joinGame) {
    (void)self; (void)id; (void)team; (void)oldTeam; (void)joinGame;
}
void Script_DefaultOnLeaveGame(Script *self, uint8_t id, bool kicked) {
    (void)self; (void)id; (void)kicked;
}

void Script_DefaultOnBeforeMapChange(Script *self, const char *map) { (void)self; (void)map; }
void Script_DefaultOnAfterMapChange(Script *self, const char *map) { (void)self; (void)map; }

void Script_DefaultOnAdminConnect(Script *self, const char *ip, uint16_t port) { (void)self; (void)ip; (void)port; }
void Script_DefaultOnAdminDisconnect(Script *self, const char *ip, uint16_t port) { (void)self; (void)ip; (void)port; }
void Script_DefaultOnAdminMessage(Script *self, const char *ip, uint16_t port, const char *message) { (void)self; (void)ip; (void)port; (void)message; }

void Script_DefaultOnFlagGrab(Script *self, uint8_t id, uint8_t teamFlag, bool grabbedInBase) { (void)self; (void)id; (void)teamFlag; (void)grabbedInBase; }
void Script_DefaultOnFlagScore(Script *self, uint8_t id, uint8_t teamFlag) { (void)self; (void)id; (void)teamFlag; }
void Script_DefaultOnFlagReturn(Script *self, uint8_t id, uint8_t teamFlag) { (void)self; (void)id; (void)teamFlag; }
void Script_DefaultOnFlagDrop(Script *self, uint8_t id, uint8_t teamFlag, bool thrown) { (void)self; (void)id; (void)teamFlag; (void)thrown; }

void Script_DefaultOnKitPickup(Script *self, uint8_t id, uint8_t kitId) { (void)self; (void)id; (void)kitId; }

Vector2 Script_DefaultOnBeforePlayerRespawn(Script *self, uint8_t id) {
    (void)self; (void)id;
    /* In Pascal it returns SpriteParts.Pos[Id]; we have no such global, return zero vector */
    Vector2 v = {0.0f, 0.0f};
    return v;
}
void Script_DefaultOnAfterPlayerRespawn(Script *self, uint8_t id) { (void)self; (void)id; }

float Script_DefaultOnPlayerDamage(Script *self, uint8_t victim, uint8_t shooter, float damage, uint8_t bulletId) {
    (void)self; (void)victim; (void)shooter; (void)bulletId;
    return damage;
}
void Script_DefaultOnPlayerKill(Script *self, uint8_t killer, uint8_t victim, uint8_t bulletId) { (void)self; (void)killer; (void)victim; (void)bulletId; }
void Script_DefaultOnWeaponChange(Script *self, uint8_t id, uint8_t primary, uint8_t secondary, uint8_t primaryAmmo, uint8_t secondaryAmmo) {
    (void)self; (void)id; (void)primary; (void)secondary; (void)primaryAmmo; (void)secondaryAmmo;
}

bool Script_DefaultOnVoteMapStart(Script *self, uint8_t id, const char *map) { (void)self; (void)id; (void)map; return false; }
bool Script_DefaultOnVoteKickStart(Script *self, uint8_t id, uint8_t victim, const char *reason) { (void)self; (void)id; (void)victim; (void)reason; return false; }
void Script_DefaultOnVoteMap(Script *self, uint8_t id, const char *map) { (void)self; (void)id; (void)map; }
void Script_DefaultOnVoteKick(Script *self, uint8_t id, uint8_t victim) { (void)self; (void)id; (void)victim; }

void Script_DefaultOnPlayerSpeak(Script *self, uint8_t id, const char *text) { (void)self; (void)id; (void)text; }
bool Script_DefaultOnPlayerCommand(Script *self, uint8_t id, const char *command) { (void)self; (void)id; (void)command; return false; }
bool Script_DefaultOnConsoleCommand(Script *self, const char *ip, uint16_t port, const char *command) { (void)self; (void)ip; (void)port; (void)command; return false; }