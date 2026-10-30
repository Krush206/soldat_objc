#ifndef SCRIPT_DISPATCHER_H
#define SCRIPT_DISPATCHER_H

#include <stddef.h>
#include <stdbool.h>
#include <pthread.h>

/* Forward declarations */
struct TScript;
struct TStringList;

/* Function pointer types */
typedef struct TScript* (*TCheckFunction)(const char* dir);

/* Dynamic array (list) */
typedef struct {
    void** items;
    size_t count;
    size_t capacity;
} TList;

/* String list (simple) */
typedef struct {
    char** strings;
    size_t count;
    size_t capacity;
} TStringList;

/* Base script object */
typedef struct TScript {
    /* Identification */
    char* Name;
    char* Dir;

    /* Mutex for thread safety */
    pthread_mutex_t Lock;

    /* Virtual methods */
    bool (*Prepare)(struct TScript* self);
    void (*Launch)(struct TScript* self);
    void (*Free)(struct TScript* self);
    void* (*CallFunc)(struct TScript* self, void* const* Params, size_t paramCount,
                      const char* FuncName, void* DefaultReturn);

    /* Event handlers */
    void (*OnActivateServer)(struct TScript* self);
    void (*OnClockTick)(struct TScript* self);
    void (*OnIdle)(struct TScript* self);
    int (*OnRequestGame)(struct TScript* self, const char* Ip, const char* Hw,
                         unsigned short Port, unsigned char State,
                         bool Forwarded, const char* Password);
    signed char (*OnBeforeJoinTeam)(struct TScript* self,
                                    unsigned char Id, unsigned char Team,
                                    unsigned char OldTeam);
    void (*OnJoinTeam)(struct TScript* self, unsigned char Id,
                       unsigned char Team, unsigned char OldTeam, bool JoinGame);
    void (*OnLeaveGame)(struct TScript* self, unsigned char Id, bool Kicked);
    void (*OnBeforeMapChange)(struct TScript* self, const char* Map);
    void (*OnAfterMapChange)(struct TScript* self, const char* Map);
    void (*OnAdminConnect)(struct TScript* self, const char* Ip, unsigned short Port);
    void (*OnAdminDisconnect)(struct TScript* self, const char* Ip, unsigned short Port);
    void (*OnAdminMessage)(struct TScript* self, const char* Ip, unsigned short Port,
                           const char* Msg);
    void (*OnFlagGrab)(struct TScript* self, unsigned char Id, unsigned char TeamFlag,
                       bool GrabbedInBase);
    void (*OnFlagScore)(struct TScript* self, unsigned char Id, unsigned char TeamFlag);
    void (*OnFlagReturn)(struct TScript* self, unsigned char Id, unsigned char TeamFlag);
    void (*OnFlagDrop)(struct TScript* self, unsigned char Id, unsigned char TeamFlag,
                       bool Thrown);
    void (*OnKitPickup)(struct TScript* self, unsigned char Id, unsigned char KitId);
    /* Vector2 is represented as two floats */
    void (*OnBeforePlayerRespawn)(struct TScript* self, unsigned char Id,
                                  float* outX, float* outY);
    void (*OnAfterPlayerRespawn)(struct TScript* self, unsigned char Id);
    float (*OnPlayerDamage)(struct TScript* self, unsigned char Victim,
                            unsigned char Shooter, float Damage, unsigned char Weapon);
    void (*OnPlayerKill)(struct TScript* self, unsigned char Killer,
                         unsigned char Victim, unsigned char BulletID);
    void (*OnWeaponChange)(struct TScript* self, unsigned char Id,
                           unsigned char Primary, unsigned char Secondary,
                           unsigned char PrimaryAmmo, unsigned char SecondaryAmmo);
    bool (*OnVoteMapStart)(struct TScript* self, unsigned char Id, const char* Map);
    bool (*OnVoteKickStart)(struct TScript* self, unsigned char Id,
                            unsigned char Victim, const char* Reason);
    void (*OnVoteMap)(struct TScript* self, unsigned char Id, const char* Map);
    void (*OnVoteKick)(struct TScript* self, unsigned char Id, unsigned char Victim);
    void (*OnPlayerSpeak)(struct TScript* self, unsigned char Id, const char* Text);
    bool (*OnPlayerCommand)(struct TScript* self, unsigned char Id, const char* Command);
    bool (*OnConsoleCommand)(struct TScript* self, const char* Ip,
                             unsigned short Port, const char* Command);
} TScript;

/* Script dispatcher singleton */
typedef struct TScriptDispatcher {
    /* Inherit base script fields (composition) */
    TScript base;

    /* Lists */
    TList* FScripts;
    TList* FScriptsToUnregister;
    TList* FCheckFunctions;

    /* Public fields */
    bool SafeMode;
    char* Dir;
} TScriptDispatcher;

/* Global singleton accessor */
extern TScriptDispatcher* ScriptDispatcher;

/* Public API */
void ScriptDispatcher_Create(void);
void ScriptDispatcher_Destroy(void);
void ScriptDispatcher_WriteInfo(const char* Text);
void ScriptDispatcher_FindScripts(const char* Name); /* Name may be NULL or empty */
bool ScriptDispatcher_Prepare(void);
void ScriptDispatcher_Launch(void);
void ScriptDispatcher_LaunchName(const char* Name);
void ScriptDispatcher_RegisterCheckFunction(TCheckFunction Func);
void ScriptDispatcher_UnregisterScript(TScript* Script);

/* Event wrappers forwarded to all registered scripts */
void ScriptDispatcher_OnActivateServer(void);
void ScriptDispatcher_OnClockTick(void);
void ScriptDispatcher_OnIdle(void);
int ScriptDispatcher_OnRequestGame(const char* Ip, const char* Hw,
                                   unsigned short Port, unsigned char State,
                                   bool Forwarded, const char* Password);
signed char ScriptDispatcher_OnBeforeJoinTeam(unsigned char Id,
                                              unsigned char Team,
                                              unsigned char OldTeam);
void ScriptDispatcher_OnJoinTeam(unsigned char Id, unsigned char Team,
                                 unsigned char OldTeam, bool JoinGame);
void ScriptDispatcher_OnLeaveGame(unsigned char Id, bool Kicked);
void ScriptDispatcher_OnBeforeMapChange(const char* Map);
void ScriptDispatcher_OnAfterMapChange(const char* Map);
void ScriptDispatcher_OnAdminConnect(const char* Ip, unsigned short Port);
void ScriptDispatcher_OnAdminDisconnect(const char* Ip, unsigned short Port);
void ScriptDispatcher_OnAdminMessage(const char* Ip, unsigned short Port,
                                     const char* Msg);
void ScriptDispatcher_OnFlagGrab(unsigned char Id, unsigned char TeamFlag,
                                 bool GrabbedInBase);
void ScriptDispatcher_OnFlagScore(unsigned char Id, unsigned char TeamFlag);
void ScriptDispatcher_OnFlagReturn(unsigned char Id, unsigned char TeamFlag);
void ScriptDispatcher_OnFlagDrop(unsigned char Id, unsigned char TeamFlag,
                                 bool Thrown);
void ScriptDispatcher_OnKitPickup(unsigned char Id, unsigned char KitId);
void ScriptDispatcher_OnBeforePlayerRespawn(unsigned char Id,
                                            float* outX, float* outY);
void ScriptDispatcher_OnAfterPlayerRespawn(unsigned char Id);
float ScriptDispatcher_OnPlayerDamage(unsigned char Victim,
                                      unsigned char Shooter,
                                      float Damage, unsigned char Weapon);
void ScriptDispatcher_OnPlayerKill(unsigned char Killer,
                                   unsigned char Victim,
                                   unsigned char BulletID);
void ScriptDispatcher_OnWeaponChange(unsigned char Id,
                                     unsigned char Primary,
                                     unsigned char Secondary,
                                     unsigned char PrimaryAmmo,
                                     unsigned char SecondaryAmmo);
bool ScriptDispatcher_OnVoteMapStart(unsigned char Id, const char* Map);
bool ScriptDispatcher_OnVoteKickStart(unsigned char Id, unsigned char Victim,
                                      const char* Reason);
void ScriptDispatcher_OnVoteMap(unsigned char Id, const char* Map);
void ScriptDispatcher_OnVoteKick(unsigned char Id, unsigned char Victim);
void ScriptDispatcher_OnPlayerSpeak(unsigned char Id, const char* Text);
bool ScriptDispatcher_OnPlayerCommand(unsigned char Id, const char* Command);
bool ScriptDispatcher_OnConsoleCommand(const char* Ip, unsigned short Port,
                                       const char* Command);

/* Helper to obtain list of script names (caller must free) */
TStringList* ScriptDispatcher_GetScriptNames(void);

#endif /* SCRIPT_DISPATCHER_H */