#include "ScriptDispatcher.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <dirent.h>
#include <sys/stat.h>
#include <pthread.h>

/* ---------- List implementation ---------- */
static TList* List_Create(void) {
    TList* list = (TList*)malloc(sizeof(TList));
    if (!list) return NULL;
    list->items = NULL;
    list->count = 0;
    list->capacity = 0;
    return list;
}

static void List_Free(TList* list) {
    if (!list) return;
    free(list->items);
    free(list);
}

static bool List_EnsureCapacity(TList* list, size_t needed) {
    if (list->capacity >= needed) return true;
    size_t newCap = list->capacity ? list->capacity * 2 : 4;
    while (newCap < needed) newCap *= 2;
    void** newItems = (void**)realloc(list->items, newCap * sizeof(void*));
    if (!newItems) return false;
    list->items = newItems;
    list->capacity = newCap;
    return true;
}

static bool List_Add(TList* list, void* item) {
    if (!List_EnsureCapacity(list, list->count + 1)) return false;
    list->items[list->count++] = item;
    return true;
}

/* Remove first occurrence, returns true if removed */
static bool List_Remove(TList* list, void* item) {
    for (size_t i = 0; i < list->count; ++i) {
        if (list->items[i] == item) {
            memmove(&list->items[i], &list->items[i + 1],
                    (list->count - i - 1) * sizeof(void*));
            list->count--;
            return true;
        }
    }
    return false;
}

static void List_Clear(TList* list) {
    list->count = 0;
}

/* ---------- StringList implementation ---------- */
static TStringList* StringList_Create(void) {
    TStringList* sl = (TStringList*)malloc(sizeof(TStringList));
    if (!sl) return NULL;
    sl->strings = NULL;
    sl->count = 0;
    sl->capacity = 0;
    return sl;
}

static void StringList_Free(TStringList* sl) {
    if (!sl) return;
    for (size_t i = 0; i < sl->count; ++i) free(sl->strings[i]);
    free(sl->strings);
    free(sl);
}

static bool StringList_Add(TStringList* sl, const char* str) {
    if (!sl) return false;
    size_t len = strlen(str) + 1;
    char* copy = (char*)malloc(len);
    if (!copy) return false;
    memcpy(copy, str, len);
    if (sl->count == sl->capacity) {
        size_t newCap = sl->capacity ? sl->capacity * 2 : 4;
        char** newArr = (char**)realloc(sl->strings, newCap * sizeof(char*));
        if (!newArr) { free(copy); return false; }
        sl->strings = newArr;
        sl->capacity = newCap;
    }
    sl->strings[sl->count++] = copy;
    return true;
}

/* ---------- Global singleton ---------- */
static TScriptDispatcher* gDispatcher = NULL;
TScriptDispatcher* ScriptDispatcher = NULL;  /* exported symbol */

/* ---------- Helper functions ---------- */
static void Dispatcher_DoLock(TScriptDispatcher* self) {
    pthread_t selfId = pthread_self();
    /* In this simple port we lock always; real code checks main thread */
    pthread_mutex_lock(&self->base.Lock);
}
static void Dispatcher_DoUnlock(TScriptDispatcher* self) {
    pthread_mutex_unlock(&self->base.Lock);
}

/* Process pending unregistrations */
static void Dispatcher_ProcessUnregisterQueue(TScriptDispatcher* self) {
    for (size_t i = 0; i < self->FScriptsToUnregister->count; ++i) {
        TScript* scr = (TScript*)self->FScriptsToUnregister->items[i];
        if (List_Remove(self->FScripts, scr)) {
            char buf[256];
            snprintf(buf, sizeof(buf), "Disabling %s", scr->Name ? scr->Name : "unknown");
            ScriptDispatcher_WriteInfo(buf);
        }
        if (scr && scr->Free) scr->Free(scr);
    }
    List_Clear(self->FScriptsToUnregister);
}

/* Unregister a single script immediately */
static void Dispatcher_DoUnregisterScript(TScriptDispatcher* self, TScript* script) {
    if (List_Remove(self->FScripts, script)) {
        char buf[256];
        snprintf(buf, sizeof(buf), "Disabling %s", script->Name ? script->Name : "unknown");
        ScriptDispatcher_WriteInfo(buf);
    }
    if (script && script->Free) script->Free(script);
}

/* ---------- Console output ---------- */
void ScriptDispatcher_WriteInfo(const char* Text) {
    /* Placeholder for MainConsole.Console */
    fprintf(stdout, " [*] %s\n", Text);
}

/* ---------- Singleton creation ---------- */
void ScriptDispatcher_Create(void) {
    if (gDispatcher) return;
    gDispatcher = (TScriptDispatcher*)calloc(1, sizeof(TScriptDispatcher));
    if (!gDispatcher) return;

    /* Initialise base script mutex */
    pthread_mutex_init(&gDispatcher->base.Lock, NULL);

    gDispatcher->FScripts = List_Create();
    gDispatcher->FScriptsToUnregister = List_Create();
    gDispatcher->FCheckFunctions = List_Create();

    gDispatcher->SafeMode = true;
    gDispatcher->Dir = strdup("scripts");

    ScriptDispatcher = gDispatcher;   /* expose globally */
}

/* ---------- Destruction ---------- */
void ScriptDispatcher_Destroy(void) {
    if (!gDispatcher) return;
    Dispatcher_DoLock(gDispatcher);
    for (size_t i = 0; i < gDispatcher->FScripts->count; ++i) {
        TScript* scr = (TScript*)gDispatcher->FScripts->items[i];
        if (scr && scr->Free) scr->Free(scr);
    }
    List_Free(gDispatcher->FScripts);
    List_Free(gDispatcher->FScriptsToUnregister);
    List_Free(gDispatcher->FCheckFunctions);
    free(gDispatcher->Dir);
    pthread_mutex_destroy(&gDispatcher->base.Lock);
    free(gDispatcher);
    gDispatcher = NULL;
    ScriptDispatcher = NULL;
}

/* ---------- GetScriptNames ---------- */
TStringList* ScriptDispatcher_GetScriptNames(void) {
    if (!gDispatcher) return NULL;
    Dispatcher_DoLock(gDispatcher);
    TStringList* result = StringList_Create();
    for (size_t i = 0; i < gDispatcher->FScripts->count; ++i) {
        TScript* scr = (TScript*)gDispatcher->FScripts->items[i];
        if (scr && scr->Name) StringList_Add(result, scr->Name);
    }
    Dispatcher_DoUnlock(gDispatcher);
    return result;
}

/* ---------- FindScripts ---------- */
void ScriptDispatcher_FindScripts(const char* Name) {
    if (!gDispatcher) return;
    Dispatcher_DoLock(gDispatcher);

    /* If full reload (Name NULL/empty) clear existing scripts */
    if (!Name || Name[0] == '\0') {
        for (size_t i = 0; i < gDispatcher->FScripts->count; ++i) {
            TScript* scr = (TScript*)gDispatcher->FScripts->items[i];
            if (scr && scr->Free) scr->Free(scr);
        }
        List_Free(gDispatcher->FScripts);
        gDispatcher->FScripts = List_Create();
    } else {
        /* Remove specific script by name */
        for (size_t i = 0; i < gDispatcher->FScripts->count; ++i) {
            TScript* scr = (TScript*)gDispatcher->FScripts->items[i];
            if (scr && scr->Name && strcmp(scr->Name, Name) == 0) {
                if (scr->Free) scr->Free(scr);
                List_Remove(gDispatcher->FScripts, scr);
                break;
            }
        }
    }

    /* Scan directory */
    DIR* dir = opendir(gDispatcher->Dir);
    if (!dir) {
        Dispatcher_DoUnlock(gDispatcher);
        return;
    }

    /* Collect folder names */
    TStringList* folders = StringList_Create();
    struct dirent* ent;
    while ((ent = readdir(dir)) != NULL) {
        if (strcmp(ent->d_name, ".") == 0 || strcmp(ent->d_name, "..") == 0) continue;
        char path[1024];
        snprintf(path, sizeof(path), "%s/%s", gDispatcher->Dir, ent->d_name);
        struct stat st;
        if (stat(path, &st) == 0 && S_ISDIR(st.st_mode)) {
            StringList_Add(folders, ent->d_name);
        }
    }
    closedir(dir);

    /* Simple sort (bubble) – acceptable for small counts */
    for (size_t i = 0; i + 1 < folders->count; ++i) {
        for (size_t j = i + 1; j < folders->count; ++j) {
            if (strcmp(folders->strings[i], folders->strings[j]) > 0) {
                char* tmp = folders->strings[i];
                folders->strings[i] = folders->strings[j];
                folders->strings[j] = tmp;
            }
        }
    }

    /* Iterate folders reverse order as Pascal did */
    for (int fi = (int)folders->count - 1; fi >= 0; --fi) {
        const char* folder = folders->strings[fi];
        for (size_t cf = 0; cf < gDispatcher->FCheckFunctions->count; ++cf) {
            TCheckFunction check = (TCheckFunction)gDispatcher->FCheckFunctions->items[cf];
            char fullPath[1024];
            snprintf(fullPath, sizeof(fullPath), "%s/%s", gDispatcher->Dir, folder);
            TScript* script = check(fullPath);
            if (script && ((!Name || Name[0] == '\0') || (script->Name && strcmp(script->Name, Name) == 0))) {
                List_Add(gDispatcher->FScripts, script);
                break; /* first matching check function wins */
            }
        }
    }

    StringList_Free(folders);
    Dispatcher_DoUnlock(gDispatcher);
}

/* ---------- Prepare ---------- */
bool ScriptDispatcher_Prepare(void) {
    if (!gDispatcher) return false;
    ScriptDispatcher_WriteInfo("Preparing scripts to be launched");
    ScriptDispatcher_FindScripts(NULL);
    Dispatcher_DoLock(gDispatcher);
    bool overall = true;
    for (int i = (int)gDispatcher->FScripts->count - 1; i >= 0; --i) {
        TScript* scr = (TScript*)gDispatcher->FScripts->items[i];
        if (scr && scr->Prepare && !scr->Prepare(scr)) {
            Dispatcher_DoUnregisterScript(gDispatcher, scr);
            overall = false;
        }
    }
    Dispatcher_ProcessUnregisterQueue(gDispatcher);
    Dispatcher_DoUnlock(gDispatcher);
    ScriptDispatcher_WriteInfo("Done");
    return overall;
}

/* ---------- Launch (all) ---------- */
void ScriptDispatcher_Launch(void) {
    if (!gDispatcher) return;
    Dispatcher_DoLock(gDispatcher);
    for (int i = (int)gDispatcher->FScripts->count - 1; i >= 0; --i) {
        TScript* scr = (TScript*)gDispatcher->FScripts->items[i];
        if (scr && scr->Launch) scr->Launch(scr);
    }
    Dispatcher_ProcessUnregisterQueue(gDispatcher);
    Dispatcher_DoUnlock(gDispatcher);
}

/* ---------- Launch (single by name) ---------- */
void ScriptDispatcher_LaunchName(const char* Name) {
    if (!gDispatcher || !Name) return;
    ScriptDispatcher_FindScripts(Name);
    Dispatcher_DoLock(gDispatcher);
    for (int i = (int)gDispatcher->FScripts->count - 1; i >= 0; --i) {
        TScript* scr = (TScript*)gDispatcher->FScripts->items[i];
        if (scr && scr->Name && strcmp(scr->Name, Name) == 0) {
            if (scr->Prepare && scr->Prepare(scr)) {
                if (scr->Launch) scr->Launch(scr);
            } else {
                Dispatcher_DoUnregisterScript(gDispatcher, scr);
            }
            break;
        }
    }
    Dispatcher_ProcessUnregisterQueue(gDispatcher);
    Dispatcher_DoUnlock(gDispatcher);
}

/* ---------- RegisterCheckFunction ---------- */
void ScriptDispatcher_RegisterCheckFunction(TCheckFunction Func) {
    if (!gDispatcher) return;
    List_Add(gDispatcher->FCheckFunctions, (void*)Func);
}

/* ---------- UnregisterScript ---------- */
void ScriptDispatcher_UnregisterScript(TScript* Script) {
    if (!gDispatcher || !Script) return;
    List_Add(gDispatcher->FScriptsToUnregister, Script);
}

/* ---------- Event forwarding helpers ---------- */
#define FOR_EACH_SCRIPT(eventCall) \
    Dispatcher_DoLock(gDispatcher); \
    for (int i = (int)gDispatcher->FScripts->count - 1; i >= 0; --i) { \
        TScript* scr = (TScript*)gDispatcher->FScripts->items[i]; \
        if (scr && scr->eventCall) scr->eventCall(scr); \
    } \
    Dispatcher_ProcessUnregisterQueue(gDispatcher); \
    Dispatcher_DoUnlock(gDispatcher)

/* Simple macros for events with parameters */
#define FOR_EACH_SCRIPT_1(eventCall, p1) \
    Dispatcher_DoLock(gDispatcher); \
    for (int i = (int)gDispatcher->FScripts->count - 1; i >= 0; --i) { \
        TScript* scr = (TScript*)gDispatcher->FScripts->items[i]; \
        if (scr && scr->eventCall) scr->eventCall(scr, p1); \
    } \
    Dispatcher_ProcessUnregisterQueue(gDispatcher); \
    Dispatcher_DoUnlock(gDispatcher)

#define FOR_EACH_SCRIPT_2(eventCall, p1, p2) \
    Dispatcher_DoLock(gDispatcher); \
    for (int i = (int)gDispatcher->FScripts->count - 1; i >= 0; --i) { \
        TScript* scr = (TScript*)gDispatcher->FScripts->items[i]; \
        if (scr && scr->eventCall) scr->eventCall(scr, p1, p2); \
    } \
    Dispatcher_ProcessUnregisterQueue(gDispatcher); \
    Dispatcher_DoUnlock(gDispatcher)

#define FOR_EACH_SCRIPT_3(eventCall, p1, p2, p3) \
    Dispatcher_DoLock(gDispatcher); \
    for (int i = (int)gDispatcher->FScripts->count - 1; i >= 0; --i) { \
        TScript* scr = (TScript*)gDispatcher->FScripts->items[i]; \
        if (scr && scr->eventCall) scr->eventCall(scr, p1, p2, p3); \
    } \
    Dispatcher_ProcessUnregisterQueue(gDispatcher); \
    Dispatcher_DoUnlock(gDispatcher)

#define FOR_EACH_SCRIPT_4(eventCall, p1, p2, p3, p4) \
    Dispatcher_DoLock(gDispatcher); \
    for (int i = (int)gDispatcher->FScripts->count - 1; i >= 0; --i) { \
        TScript* scr = (TScript*)gDispatcher->FScripts->items[i]; \
        if (scr && scr->eventCall) scr->eventCall(scr, p1, p2, p3, p4); \
    } \
    Dispatcher_ProcessUnregisterQueue(gDispatcher); \
    Dispatcher_DoUnlock(gDispatcher)

#define FOR_EACH_SCRIPT_5(eventCall, p1, p2, p3, p4, p5) \
    Dispatcher_DoLock(gDispatcher); \
    for (int i = (int)gDispatcher->FScripts->count - 1; i >= 0; --i) { \
        TScript* scr = (TScript*)gDispatcher->FScripts->items[i]; \
        if (scr && scr->eventCall) scr->eventCall(scr, p1, p2, p3, p4, p5); \
    } \
    Dispatcher_ProcessUnregisterQueue(gDispatcher); \
    Dispatcher_DoUnlock(gDispatcher)

#define FOR_EACH_SCRIPT_6(eventCall, p1, p2, p3, p4, p5, p6) \
    Dispatcher_DoLock(gDispatcher); \
    for (int i = (int)gDispatcher->FScripts->count - 1; i >= 0; --i) { \
        TScript* scr = (TScript*)gDispatcher->FScripts->items[i]; \
        if (scr && scr->eventCall) scr->eventCall(scr, p1, p2, p3, p4, p5, p6); \
    } \
    Dispatcher_ProcessUnregisterQueue(gDispatcher); \
    Dispatcher_DoUnlock(gDispatcher)

/* ---------- Event implementations ---------- */
void ScriptDispatcher_OnActivateServer(void) {
    FOR_EACH_SCRIPT(OnActivateServer);
}
void ScriptDispatcher_OnClockTick(void) {
    FOR_EACH_SCRIPT(OnClockTick);
}
void ScriptDispatcher_OnIdle(void) {
    FOR_EACH_SCRIPT(OnIdle);
}
int ScriptDispatcher_OnRequestGame(const char* Ip, const char* Hw,
                                   unsigned short Port, unsigned char State,
                                   bool Forwarded, const char* Password) {
    int result = State;
    Dispatcher_DoLock(gDispatcher);
    for (int i = (int)gDispatcher->FScripts->count - 1; i >= 0; --i) {
        TScript* scr = (TScript*)gDispatcher->FScripts->items[i];
        if (scr && scr->OnRequestGame)
            result = scr->OnRequestGame(scr, Ip, Hw, Port, (unsigned char)result, Forwarded, Password);
    }
    Dispatcher_ProcessUnregisterQueue(gDispatcher);
    Dispatcher_DoUnlock(gDispatcher);
    return result;
}
signed char ScriptDispatcher_OnBeforeJoinTeam(unsigned char Id,
                                              unsigned char Team,
                                              unsigned char OldTeam) {
    signed char result = Team;
    Dispatcher_DoLock(gDispatcher);
    for (int i = (int)gDispatcher->FScripts->count - 1; i >= 0; --i) {
        TScript* scr = (TScript*)gDispatcher->FScripts->items[i];
        if (scr && scr->OnBeforeJoinTeam) {
            signed char r = scr->OnBeforeJoinTeam(scr, Id, Team, OldTeam);
            if (r != -1) {
                result = r;
                Team = r;
            }
        }
    }
    Dispatcher_ProcessUnregisterQueue(gDispatcher);
    Dispatcher_DoUnlock(gDispatcher);
    return result;
}
void ScriptDispatcher_OnJoinTeam(unsigned char Id, unsigned char Team,
                                 unsigned char OldTeam, bool JoinGame) {
    FOR_EACH_SCRIPT_4(OnJoinTeam, Id, Team, OldTeam, JoinGame);
}
void ScriptDispatcher_OnLeaveGame(unsigned char Id, bool Kicked) {
    FOR_EACH_SCRIPT_2(OnLeaveGame, Id, Kicked);
}
void ScriptDispatcher_OnBeforeMapChange(const char* Map) {
    FOR_EACH_SCRIPT_1(OnBeforeMapChange, Map);
}
void ScriptDispatcher_OnAfterMapChange(const char* Map) {
    FOR_EACH_SCRIPT_1(OnAfterMapChange, Map);
}
void ScriptDispatcher_OnAdminConnect(const char* Ip, unsigned short Port) {
    FOR_EACH_SCRIPT_2(OnAdminConnect, Ip, Port);
}
void ScriptDispatcher_OnAdminDisconnect(const char* Ip, unsigned short Port) {
    FOR_EACH_SCRIPT_2(OnAdminDisconnect, Ip, Port);
}
void ScriptDispatcher_OnAdminMessage(const char* Ip, unsigned short Port,
                                     const char* Msg) {
    FOR_EACH_SCRIPT_3(OnAdminMessage, Ip, Port, Msg);
}
void ScriptDispatcher_OnFlagGrab(unsigned char Id, unsigned char TeamFlag,
                                 bool GrabbedInBase) {
    FOR_EACH_SCRIPT_3(OnFlagGrab, Id, TeamFlag, GrabbedInBase);
}
void ScriptDispatcher_OnFlagScore(unsigned char Id, unsigned char TeamFlag) {
    FOR_EACH_SCRIPT_2(OnFlagScore, Id, TeamFlag);
}
void ScriptDispatcher_OnFlagReturn(unsigned char Id, unsigned char TeamFlag) {
    FOR_EACH_SCRIPT_2(OnFlagReturn, Id, TeamFlag);
}
void ScriptDispatcher_OnFlagDrop(unsigned char Id, unsigned char TeamFlag,
                                 bool Thrown) {
    FOR_EACH_SCRIPT_3(OnFlagDrop, Id, TeamFlag, Thrown);
}
void ScriptDispatcher_OnKitPickup(unsigned char Id, unsigned char KitId) {
    FOR_EACH_SCRIPT_2(OnKitPickup, Id, KitId);
}
void ScriptDispatcher_OnBeforePlayerRespawn(unsigned char Id,
                                            float* outX, float* outY) {
    /* Default position – in original Pascal uses SpriteParts.Pos[Id];
       here we just leave outX/outY unchanged if no script handles it. */
    Dispatcher_DoLock(gDispatcher);
    for (int i = (int)gDispatcher->FScripts->count - 1; i >= 0; --i) {
        TScript* scr = (TScript*)gDispatcher->FScripts->items[i];
        if (scr && scr->OnBeforePlayerRespawn)
            scr->OnBeforePlayerRespawn(scr, Id, outX, outY);
    }
    Dispatcher_ProcessUnregisterQueue(gDispatcher);
    Dispatcher_DoUnlock(gDispatcher);
}
void ScriptDispatcher_OnAfterPlayerRespawn(unsigned char Id) {
    FOR_EACH_SCRIPT_1(OnAfterPlayerRespawn, Id);
}
float ScriptDispatcher_OnPlayerDamage(unsigned char Victim,
                                      unsigned char Shooter,
                                      float Damage, unsigned char Weapon) {
    float dmg = Damage;
    Dispatcher_DoLock(gDispatcher);
    for (int i = (int)gDispatcher->FScripts->count - 1; i >= 0; --i) {
        TScript* scr = (TScript*)gDispatcher->FScripts->items[i];
        if (scr && scr->OnPlayerDamage)
            dmg = scr->OnPlayerDamage(scr, Victim, Shooter, dmg, Weapon);
    }
    Dispatcher_ProcessUnregisterQueue(gDispatcher);
    Dispatcher_DoUnlock(gDispatcher);
    return dmg;
}
void ScriptDispatcher_OnPlayerKill(unsigned char Killer,
                                   unsigned char Victim,
                                   unsigned char BulletID) {
    FOR_EACH_SCRIPT_3(OnPlayerKill, Killer, Victim, BulletID);
}
void ScriptDispatcher_OnWeaponChange(unsigned char Id,
                                     unsigned char Primary,
                                     unsigned char Secondary,
                                     unsigned char PrimaryAmmo,
                                     unsigned char SecondaryAmmo) {
    FOR_EACH_SCRIPT_5(OnWeaponChange, Id, Primary, Secondary,
                      PrimaryAmmo, SecondaryAmmo);
}
bool ScriptDispatcher_OnVoteMapStart(unsigned char Id, const char* Map) {
    bool result = false;
    Dispatcher_DoLock(gDispatcher);
    for (int i = (int)gDispatcher->FScripts->count - 1; i >= 0; --i) {
        TScript* scr = (TScript*)gDispatcher->FScripts->items[i];
        if (scr && scr->OnVoteMapStart)
            result = result || scr->OnVoteMapStart(scr, Id, Map);
    }
    Dispatcher_ProcessUnregisterQueue(gDispatcher);
    Dispatcher_DoUnlock(gDispatcher);
    return result;
}
bool ScriptDispatcher_OnVoteKickStart(unsigned char Id, unsigned char Victim,
                                      const char* Reason) {
    bool result = false;
    Dispatcher_DoLock(gDispatcher);
    for (int i = (int)gDispatcher->FScripts->count - 1; i >= 0; --i) {
        TScript* scr = (TScript*)gDispatcher->FScripts->items[i];
        if (scr && scr->OnVoteKickStart)
            result = result || scr->OnVoteKickStart(scr, Id, Victim, Reason);
    }
    Dispatcher_ProcessUnregisterQueue(gDispatcher);
    Dispatcher_DoUnlock(gDispatcher);
    return result;
}
void ScriptDispatcher_OnVoteMap(unsigned char Id, const char* Map) {
    FOR_EACH_SCRIPT_2(OnVoteMap, Id, Map);
}
void ScriptDispatcher_OnVoteKick(unsigned char Id, unsigned char Victim) {
    FOR_EACH_SCRIPT_2(OnVoteKick, Id, Victim);
}
void ScriptDispatcher_OnPlayerSpeak(unsigned char Id, const char* Text) {
    FOR_EACH_SCRIPT_2(OnPlayerSpeak, Id, Text);
}
bool ScriptDispatcher_OnPlayerCommand(unsigned char Id, const char* Command) {
    bool result = false;
    Dispatcher_DoLock(gDispatcher);
    for (int i = (int)gDispatcher->FScripts->count - 1; i >= 0; --i) {
        TScript* scr = (TScript*)gDispatcher->FScripts->items[i];
        if (scr && scr->OnPlayerCommand)
            result = result || scr->OnPlayerCommand(scr, Id, Command);
    }
    Dispatcher_ProcessUnregisterQueue(gDispatcher);
    Dispatcher_DoUnlock(gDispatcher);
    return result;
}
bool ScriptDispatcher_OnConsoleCommand(const char* Ip, unsigned short Port,
                                       const char* Command) {
    bool result = false;
    Dispatcher_DoLock(gDispatcher);
    for (int i = (int)gDispatcher->FScripts->count - 1; i >= 0; --i) {
        TScript* scr = (TScript*)gDispatcher->FScripts->items[i];
        if (scr && scr->OnConsoleCommand)
            result = result || scr->OnConsoleCommand(scr, Ip, Port, Command);
    }
    Dispatcher_ProcessUnregisterQueue(gDispatcher);
    Dispatcher_DoUnlock(gDispatcher);
    return result;
}

/* ---------- Initialization (called once at program start) ---------- */
__attribute__((constructor))
static void ScriptDispatcher_Init(void) {
    ScriptDispatcher_Create();
    /* The Pascal initialization registers two check functions:
       ScriptCore.CheckFunction and ScriptCore3.CheckFunction.
       Here we assume those symbols exist and have proper signature. */
    extern TScript* ScriptCore_CheckFunction(const char* dir);
    extern TScript* ScriptCore3_CheckFunction(const char* dir);
    ScriptDispatcher_RegisterCheckFunction(ScriptCore_CheckFunction);
    ScriptDispatcher_RegisterCheckFunction(ScriptCore3_CheckFunction);
}

/* ---------- Cleanup (optional) ---------- */
__attribute__((destructor))
static void ScriptDispatcher_Fini(void) {
    ScriptDispatcher_Destroy();
}