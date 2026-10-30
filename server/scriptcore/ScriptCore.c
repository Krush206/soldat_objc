#include "ScriptCore.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/* Forward declarations of external functions used by the original Pascal code.
   In a real build these would be provided by the corresponding C modules. */
extern void PascalScript_Init(void* script, const char* dir);
extern void PascalScript_Free(void* script);
extern bool PascalScript_Compile(void* script);
extern int PascalScript_CompilerMessageCount(void* script);
extern const char* PascalScript_CompilerErrorToStr(void* script, int index);
extern int PascalScript_ExecErrorCode(void* script);
extern const char* PascalScript_ExecErrorToString(void* script);
extern void PascalScript_OnCompile(void* script, void (*callback)(void*));
extern void PascalScript_OnExecute(void* script, void (*callback)(void*));
extern void PascalScript_SetPointerToData(void* script, const char* name, void* ptr, int type);
extern int PascalScript_FindBaseType(void* script, int baseType);
extern bool PascalScript_ExecuteFunction(void* script, const void* params, const char* funcName, int* result);
extern int PascalScript_GetProc(void* script, const char* funcName);
extern void RegisterFunctions(ScriptCore* core, void* script);
extern void SetVariables(ScriptCore* core, void* script);
extern void ScrptDispatcher_WriteInfo(const char* msg);
extern void ScrptDispatcher_UnregisterScript(ScriptCore* core);
extern bool sc_onscriptcrash_ValueEquals(const char* value);
extern void ParseInput(const char* cmd, int param);
extern int MainTickCounter;
extern int SECOND;
extern void* Sprite[];   /* placeholder for sprite array */
extern bool IsRemoteAdminIP(const char* ip);
extern bool IsAdminIP(const char* ip);
extern int WeaponNumInternalToExternal(int internal);
extern const char* WeaponNameByNum(int num);
extern int Bullet[];     /* placeholder for bullet array */

/* Internal lock implementation (simple mutex using pthreads) */
#include <pthread.h>

struct ScriptCore {
    char* FDir;
    char* FName;
    int FAppOnIdleTimer;
    uint8_t FErrorCount;
    uint16_t FErrorTime;
    bool FDisabled;
    void* PascalScript;
    pthread_mutex_t Lock;
};

/* Helper to duplicate a string */
static char* strdup_safe(const char* s) {
    if (!s) return NULL;
    size_t len = strlen(s) + 1;
    char* copy = malloc(len);
    if (copy) memcpy(copy, s, len);
    return copy;
}

/* Constructor */
ScriptCore* ScriptCore_Create(const char* dir) {
    ScriptCore* self = calloc(1, sizeof(ScriptCore));
    if (!self) return NULL;

    self->FDir = strdup_safe(dir);
    /* FName := Copy(Dir, 9, Length(Dir));  // skip first 8 chars (e.g. "scripts/") */
    if (dir && strlen(dir) > 8) {
        self->FName = strdup_safe(dir + 8);
    } else {
        self->FName = strdup_safe("");
    }

    self->FErrorCount = 0;
    self->FErrorTime = 0;
    self->FDisabled = false;
    self->FAppOnIdleTimer = 60;

    pthread_mutex_init(&self->Lock, NULL);

    /* Initialise PascalScript engine */
    PascalScript_Init(self->PascalScript, dir);

    /* Attach DLL plugin if not in safe mode (placeholder) */
    // if (!ScrptDispatcher_SafeMode) { ... }

    /* Register callbacks */
    PascalScript_OnCompile(self->PascalScript, (void(*)(void*))/*OnCompile*/);
    PascalScript_OnExecute(self->PascalScript, (void(*)(void*))/*OnExecute*/);

    return self;
}

/* Destructor */
void ScriptCore_Destroy(ScriptCore* self) {
    if (!self) return;
    ScriptCore_CallFunc(self, NULL, "DeactivateServer", 0);
    pthread_mutex_destroy(&self->Lock);
    PascalScript_Free(self->PascalScript);
    free(self->FDir);
    free(self->FName);
    free(self);
}

/* CheckFunction */
ScriptCore* ScriptCore_CheckFunction(const char* dir) {
    if (!dir) return NULL;
    char path[1024];
    snprintf(path, sizeof(path), "%s/Includes.txt", dir);
    FILE* f = fopen(path, "r");
    if (!f) return NULL;
    fclose(f);
    return ScriptCore_Create(dir);
}

/* Prepare */
bool ScriptCore_Prepare(ScriptCore* self) {
    if (!self) return false;
    pthread_mutex_lock(&self->Lock);
    bool result = false;

    /* Load Includes.txt */
    char includesPath[1024];
    snprintf(includesPath, sizeof(includesPath), "%sIncludes.txt", self->FDir);
    FILE* incFile = fopen(includesPath, "r");
    if (!incFile) {
        pthread_mutex_unlock(&self->Lock);
        return false;
    }

    char line[1024];
    char* buffer = NULL;
    size_t bufferLen = 0;

    while (fgets(line, sizeof(line), incFile)) {
        /* Remove trailing newline */
        line[strcspn(line, "\r\n")] = 0;
        if (line[0] == '/' && line[1] == '/') continue; // skip comments
        if (line[0] == 0) continue; // empty line

        /* Strip inline comments */
        char* commentPos = strstr(line, "//");
        if (commentPos) *commentPos = 0;

        char filePath[1024];
        snprintf(filePath, sizeof(filePath), "%s%s", self->FDir, line);
        ScrptDispatcher_WriteInfo("[Info] Compiling ");
        ScrptDispatcher_WriteInfo(self->FName);
        ScrptDispatcher_WriteInfo(" -> ");
        ScrptDispatcher_WriteInfo(line);
        ScrptDispatcher_WriteInfo("...");

        FILE* srcFile = fopen(filePath, "r");
        if (!srcFile) {
            ScrptDispatcher_WriteInfo("[Error] Included File not found: ");
            ScrptDispatcher_WriteInfo(line);
            fclose(incFile);
            free(buffer);
            pthread_mutex_unlock(&self->Lock);
            return false;
        }
        fseek(srcFile, 0, SEEK_END);
        long sz = ftell(srcFile);
        fseek(srcFile, 0, SEEK_SET);
        char* src = malloc(sz + 1);
        fread(src, 1, sz, srcFile);
        src[sz] = 0;
        fclose(srcFile);

        /* Append to buffer */
        size_t newLen = bufferLen + sz + 2; // +2 for CR LF
        char* newBuf = realloc(buffer, newLen + 1);
        if (!newBuf) { free(src); free(buffer); fclose(incFile); pthread_mutex_unlock(&self->Lock); return false; }
        buffer = newBuf;
        memcpy(buffer + bufferLen, src, sz);
        bufferLen += sz;
        buffer[bufferLen++] = '\r';
        buffer[bufferLen++] = '\n';
        buffer[bufferLen] = 0;
        free(src);
    }
    fclose(incFile);

    /* Append program wrapper */
    const char* wrapper = "begin end.";
    size_t wlen = strlen(wrapper);
    char* newBuf = realloc(buffer, bufferLen + wlen + 1);
    if (!newBuf) { free(buffer); pthread_mutex_unlock(&self->Lock); return false; }
    buffer = newBuf;
    memcpy(buffer + bufferLen, wrapper, wlen + 1);
    bufferLen += wlen;

    /* Assign script text to PascalScript (placeholder) */
    // PascalScript_SetScript(self->PascalScript, buffer);
    free(buffer);

    bool compiled = PascalScript_Compile(self->PascalScript);
    if (PascalScript_CompilerMessageCount(self->PascalScript) > 0) {
        for (int i = 0; i < PascalScript_CompilerMessageCount(self->PascalScript); ++i) {
            ScrptDispatcher_WriteInfo(self->FName);
            ScrptDispatcher_WriteInfo(" -> ");
            ScrptDispatcher_WriteInfo(PascalScript_CompilerErrorToStr(self->PascalScript, i));
        }
    }
    if (!compiled) {
        if (PascalScript_ExecErrorCode(self->PascalScript) != 0) {
            ScrptDispatcher_WriteInfo(self->FName);
            ScrptDispatcher_WriteInfo(" -> ");
            ScrptDispatcher_WriteInfo(PascalScript_ExecErrorToString(self->PascalScript));
        }
        pthread_mutex_unlock(&self->Lock);
        return false;
    }

    /* Successful compile */
    ScrptDispatcher_WriteInfo(self->FName);
    ScrptDispatcher_WriteInfo(" compiled");
    ScriptCore_OnAfterCompile(self);
    result = true;

    pthread_mutex_unlock(&self->Lock);
    return result;
}

/* OnAfterCompile – expose variables to script */
void ScriptCore_OnAfterCompile(ScriptCore* self) {
    if (!self) return;
    PascalScript_SetPointerToData(self->PascalScript, "AppOnIdleTimer", &self->FAppOnIdleTimer, PascalScript_FindBaseType(self->PascalScript, /*btU32*/ 0));
    PascalScript_SetPointerToData(self->PascalScript, "DisableScript", &self->FDisabled, PascalScript_FindBaseType(self->PascalScript, /*btU8*/ 1));
}

/* Launch */
void ScriptCore_Launch(ScriptCore* self) {
    if (!self) return;
    ScriptCore_CallFunc(self, NULL, "ActivateServer", 0);
}

/* CallFunc – generic caller */
int ScriptCore_CallFunc(ScriptCore* self, const void* params, const char* funcName, int defaultReturn) {
    if (!self || !funcName) return defaultReturn;
    if (PascalScript_GetProc(self->PascalScript, funcName) == 0) return defaultReturn;

    pthread_mutex_lock(&self->Lock);
    int result = defaultReturn;
    // Execute function; PascalScript_ExecuteFunction returns bool success, fills result pointer
    if (!PascalScript_ExecuteFunction(self->PascalScript, params, funcName, &result)) {
        if (PascalScript_ExecErrorCode(self->PascalScript) != 0) {
            char msg[512];
            snprintf(msg, sizeof(msg), "[Error] %s -> %s(%d:%d) %s",
                     self->FName, funcName,
                     0, 0, // row/col not available in this stub
                     PascalScript_ExecErrorToString(self->PascalScript));
            ScrptDispatcher_WriteInfo(msg);
        } else {
            char msg[512];
            snprintf(msg, sizeof(msg), "[Error] %s -> (%s): execution failed",
                     self->FName, funcName);
            ScrptDispatcher_WriteInfo(msg);
        }
        self->FErrorCount++;
        if (self->FErrorCount > 10) {
            if (sc_onscriptcrash_ValueEquals("shutdown")) {
                // ProgReady = false; // global
                ScrptDispatcher_WriteInfo(self->FName);
                ScrptDispatcher_WriteInfo(" crashed, shutting down");
                ParseInput("say Script crashed, server is shutting down", 255);
            } else if (sc_onscriptcrash_ValueEquals("recompile")) {
                ScrptDispatcher_WriteInfo(self->FName);
                ScrptDispatcher_WriteInfo(" crashed, recompiling...");
                ParseInput("say Script crashed, recompiling...", 255);
                pthread_mutex_unlock(&self->Lock);
                ScriptCore_Prepare(self);
                ScriptCore_Launch(self);
                pthread_mutex_lock(&self->Lock);
            } else if (sc_onscriptcrash_ValueEquals("disable")) {
                ScrptDispatcher_WriteInfo(self->FName);
                ScrptDispatcher_WriteInfo(" crashed, disabling");
                ParseInput("say Script crashed, disabling it", 255);
                ScrptDispatcher_UnregisterScript(self);
            }
        }
    }
    pthread_mutex_unlock(&self->Lock);
    return result;
}

/* OnClockTick */
void ScriptCore_OnClockTick(ScriptCore* self) {
    if (!self || self->FDisabled) return;
    if (self->FAppOnIdleTimer > 0 && (MainTickCounter % self->FAppOnIdleTimer) == 0) {
        ScriptCore_CallFunc(self, &MainTickCounter, "AppOnIdle", 0);
    }
    if ((MainTickCounter % SECOND) == 0 && self->FErrorCount > 0) {
        self->FErrorCount--;
    }
}

/* OnIdle */
void ScriptCore_OnIdle(ScriptCore* self) {
    if (!self || self->FDisabled) return;
    ScriptCore_CallFunc(self, NULL, "OnIdle", 0);
}

/* Event wrappers – each forwards to CallFunc if not disabled */
int ScriptCore_OnRequestGame(ScriptCore* self, const char* ip, const char* hw, uint16_t port, uint8_t state, bool forwarded, const char* password) {
    if (!self || self->FDisabled) return state;
    // Set global variable ReqPort (placeholder)
    // PascalScript_SetVariable(self->PascalScript, "ReqPort", port);
    state = ScriptCore_CallFunc(self, &(struct{const char* ip; uint8_t state;}){ip, state}, "OnRequestGame", state);
    state = ScriptCore_CallFunc(self, &(struct{const char* ip; uint16_t port; uint8_t state; bool forwarded; const char* password;}){ip, port, state, forwarded, password}, "OnRequestGameEx", state);
    return state;
}

void ScriptCore_OnJoinTeam(ScriptCore* self, uint8_t id, uint8_t team, uint8_t oldTeam, bool joinGame) {
    if (!self || self->FDisabled) return;
    if (joinGame && ((/*Sprite[id].Player.ControlMethod*/0) == 0 /*HUMAN*/)) {
        ScriptCore_CallFunc(self, &(struct{uint8_t id; uint8_t team;}){id, team}, "OnJoinGame", 0);
    }
    ScriptCore_CallFunc(self, &(struct{uint8_t id; uint8_t team;}){id, team}, "OnJoinTeam", 0);
}

void ScriptCore_OnLeaveGame(ScriptCore* self, uint8_t id, bool kicked) {
    if (!self || self->FDisabled) return;
    // uint8_t team = Sprite[id].Player.Team;
    uint8_t team = 0;
    ScriptCore_CallFunc(self, &(struct{uint8_t id; uint8_t team; bool kicked;}){id, team, kicked}, "OnLeaveGame", 0);
}

void ScriptCore_OnBeforeMapChange(ScriptCore* self, const char* map) {
    if (!self || self->FDisabled) return;
    ScriptCore_CallFunc(self, NULL, "OnGameEnd", 0);
}

void ScriptCore_OnAfterMapChange(ScriptCore* self, const char* map) {
    if (!self || self->FDisabled) return;
    ScriptCore_CallFunc(self, &(struct{const char* map;}){map}, "OnMapChange", 0);
}

void ScriptCore_OnAdminConnect(ScriptCore* self, const char* ip, uint16_t port) {
    if (!self || self->FDisabled) return;
    ScriptCore_CallFunc(self, &(struct{const char* ip;}){ip}, "OnAdminConnect", 0);
}

void ScriptCore_OnAdminDisconnect(ScriptCore* self, const char* ip, uint16_t port) {
    if (!self || self->FDisabled) return;
    ScriptCore_CallFunc(self, &(struct{const char* ip;}){ip}, "OnAdminDisconnect", 0);
}

void ScriptCore_OnAdminMessage(ScriptCore* self, const char* ip, uint16_t port, const char* msg) {
    if (!self || self->FDisabled) return;
    ScriptCore_CallFunc(self, &(struct{const char* ip; const char* msg;}){ip, msg}, "OnAdminMessage", 0);
}

void ScriptCore_OnFlagGrab(ScriptCore* self, uint8_t id, uint8_t teamFlag, bool grabbedInBase) {
    if (!self || self->FDisabled) return;
    ScriptCore_CallFunc(self, &(struct{uint8_t id; uint8_t teamFlag; bool grabbedInBase;}){id, teamFlag, grabbedInBase}, "OnFlagGrab", 0);
}

void ScriptCore_OnFlagScore(ScriptCore* self, uint8_t id, uint8_t teamFlag) {
    if (!self || self->FDisabled) return;
    ScriptCore_CallFunc(self, &(struct{uint8_t id; uint8_t teamFlag;}){id, teamFlag}, "OnFlagScore", 0);
}

void ScriptCore_OnFlagReturn(ScriptCore* self, uint8_t id, uint8_t teamFlag) {
    if (!self || self->FDisabled) return;
    ScriptCore_CallFunc(self, &(struct{uint8_t id; uint8_t teamFlag;}){id, teamFlag}, "OnFlagReturn", 0);
}

void ScriptCore_OnAfterPlayerRespawn(ScriptCore* self, uint8_t id) {
    if (!self || self->FDisabled) return;
    ScriptCore_CallFunc(self, &(struct{uint8_t id;}){id}, "OnPlayerRespawn", 0);
}

float ScriptCore_OnPlayerDamage(ScriptCore* self, uint8_t victim, uint8_t shooter, float damage, uint8_t bulletID) {
    if (!self || self->FDisabled) return damage;
    int dmgInt = (int) (damage + 0.5f);
    damage = (float) ScriptCore_CallFunc(self, &(struct{uint8_t victim; uint8_t shooter; int damage;}){victim, shooter, dmgInt}, "OnPlayerDamage", (int)damage);
    int weaponExt = WeaponNumInternalToExternal(Bullet[bulletID]);
    damage = (float) ScriptCore_CallFunc(self, &(struct{uint8_t victim; uint8_t shooter; int damage; int weapon;}){victim, shooter, (int)(damage+0.5f), weaponExt}, "OnPlayerDamageEx", (int)damage);
    return damage;
}

void ScriptCore_OnPlayerKill(ScriptCore* self, uint8_t killer, uint8_t victim, uint8_t bulletID) {
    if (!self || self->FDisabled) return;
    int weapon = (killer == victim) ? 100 : Bullet[bulletID];
    const char* weaponName = WeaponNameByNum(weapon);
    ScriptCore_CallFunc(self, &(struct{uint8_t killer; uint8_t victim; const char* weapon;}){killer, victim, weaponName}, "OnPlayerKill", 0);
    int weaponExt = WeaponNumInternalToExternal(weapon);
    ScriptCore_CallFunc(self, &(struct{uint8_t killer; uint8_t victim; int weapon;}){killer, victim, weaponExt}, "OnPlayerKillEx", 0);
}

void ScriptCore_OnWeaponChange(ScriptCore* self, uint8_t id, uint8_t primary, uint8_t secondary, uint8_t primaryAmmo, uint8_t secondaryAmmo) {
    if (!self || self->FDisabled) return;
    int primExt = WeaponNumInternalToExternal(primary);
    int secExt = WeaponNumInternalToExternal(secondary);
    ScriptCore_CallFunc(self, &(struct{uint8_t id; int primary; int secondary;}){id, primExt, secExt}, "OnWeaponChange", 0);
}

bool ScriptCore_OnVoteMapStart(ScriptCore* self, uint8_t id, const char* map) {
    if (!self || self->FDisabled) return false;
    return ScriptCore_CallFunc(self, &(struct{uint8_t id; const char* map;}){id, map}, "OnVoteMapStart", 0) != 0;
}

bool ScriptCore_OnVoteKickStart(ScriptCore* self, uint8_t id, uint8_t victim, const char* reason) {
    if (!self || self->FDisabled) return false;
    return ScriptCore_CallFunc(self, &(struct{uint8_t id; uint8_t victim; const char* reason;}){id, victim, reason}, "OnVoteKickStart", 0) != 0;
}

void ScriptCore_OnVoteMap(ScriptCore* self, uint8_t id, const char* map) {
    if (!self || self->FDisabled) return;
    ScriptCore_CallFunc(self, &(struct{uint8_t id; const char* map;}){id, map}, "OnVoteMap", 0);
}

void ScriptCore_OnVoteKick(ScriptCore* self, uint8_t id, uint8_t victim) {
    if (!self || self->FDisabled) return;
    ScriptCore_CallFunc(self, &(struct{uint8_t id; uint8_t victim;}){id, victim}, "OnVoteKick", 0);
}

void ScriptCore_OnPlayerSpeak(ScriptCore* self, uint8_t id, const char* text) {
    if (!self || self->FDisabled) return;
    ScriptCore_CallFunc(self, &(struct{uint8_t id; const char* text;}){id, text}, "OnPlayerSpeak", 0);
}

bool ScriptCore_OnPlayerCommand(ScriptCore* self, uint8_t id, const char* command) {
    if (!self || self->FDisabled) return false;
    bool result = false;
    // if (IsRemoteAdminIP(Sprite[id].Player.IP) || IsAdminIP(Sprite[id].Player.IP))
    //     result = ScriptCore_CallFunc(self, &(struct{uint8_t id; const char* cmd;}){id, command}, "OnCommand", 0) != 0;
    result = result || (ScriptCore_CallFunc(self, &(struct{uint8_t id; const char* cmd;}){id, command}, "OnPlayerCommand", 0) != 0);
    return result;
}

bool ScriptCore_OnConsoleCommand(ScriptCore* self, const char* ip, uint16_t port, const char* command) {
    if (!self || self->FDisabled) return false;
    return ScriptCore_CallFunc(self, &(struct{uint8_t id; const char* cmd;}){255, command}, "OnCommand", 0) != 0;
}