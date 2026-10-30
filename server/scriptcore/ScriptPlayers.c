#include "ScriptPlayers.h"
#include <stdlib.h>
#include <string.h>
#include <assert.h>

/* ------------------------------------------------------------------ */
/* External references (provided by other parts of the server)          */
/* ------------------------------------------------------------------ */
extern void RandomizeStart(void* outPos, int team);
extern int CreateSprite(const void* pos, const void* vel, int team, int playerNum,
                        void* clonedSprite, int isBot);
extern void* Sprite[];                 /* array of sprite objects, 1‑based */
extern void MainConsole_console(const char* msg, uint32_t color);
extern void ServerSendNewPlayerInfo(int playerNum, int joinType);
extern void ScrptDispatcher_OnJoinTeam(int playerNum, int oldTeam, int newTeam, int silent);
extern void ScrptDispatcher_OnWeaponChange(int playerNum, int primary, int secondary,
                                           int primaryAmmo, int secondaryAmmo);
extern void ServerHelper_WriteConsole(int playerNum, const char* text, int32_t color);
extern void NetworkServerMessages_ServerSendSpecialMessage(const char* text, int type,
                                                            uint8_t layer, int delay,
                                                            float scale, uint32_t color,
                                                            int x, int y, int reserved);
extern void NetworkServerMessages_ServerSendSpecialMessageF(const char* text, int type,
                                                            uint8_t layer, int delay,
                                                            float scale, uint32_t color,
                                                            float x, float y, int reserved);
extern void ServerSendStringMessage(const wchar_t* text, int target, uint8_t team,
                                    int msgType);

/* Helper to convert UTF‑8 to wide string (very small wrapper) */
static wchar_t* utf8_to_wide(const char* src) {
    if (!src) return NULL;
    size_t len = mbstowcs(NULL, src, 0);
    wchar_t* dst = (wchar_t*)malloc((len + 1) * sizeof(wchar_t));
    if (!dst) return NULL;
    mbstowcs(dst, src, len + 1);
    return dst;
}

/* ------------------------------------------------------------------ */
/* Internal structure definitions                                        */
/* ------------------------------------------------------------------ */
struct TScriptPlayers {
    TScriptActivePlayer* FPlayers[MAX_SPRITES + 1]; /* 1‑based */
    TFPGList* FActivePlayers;
};

struct TScriptPlayersAPI {
    TScriptPlayers* FPlayers;
};

/* ------------------------------------------------------------------ */
/* TScriptPlayers implementation                                         */
/* ------------------------------------------------------------------ */
TScriptPlayers* ScriptPlayers_Create(void) {
    TScriptPlayers* self = (TScriptPlayers*)calloc(1, sizeof(TScriptPlayers));
    if (!self) return NULL;

    for (int i = 1; i <= MAX_SPRITES; ++i) {
        /* TScriptActivePlayer constructor expects (Sprite[i], i) */
        self->FPlayers[i] = (TScriptActivePlayer*)calloc(1, sizeof(TScriptActivePlayer));
        /* In the real code the constructor would initialise the object with the sprite.
         * Here we only allocate the memory; the real initialisation is done elsewhere. */
    }

    /* TFPGList creation – we assume a simple dynamic array implementation */
    self->FActivePlayers = (TFPGList*)calloc(1, sizeof(TFPGList));
    return self;
}

void ScriptPlayers_Destroy(TScriptPlayers* self) {
    if (!self) return;
    for (int i = 1; i <= MAX_SPRITES; ++i) {
        free(self->FPlayers[i]);
        self->FPlayers[i] = NULL;
    }
    free(self->FActivePlayers);
    self->FActivePlayers = NULL;
    free(self);
}

/* The Pascal code uses a property getter that raises on bad IDs.
 * In C we simply return NULL for invalid IDs. */
TScriptActivePlayer* ScriptPlayers_GetPlayer(TScriptPlayers* self, uint8_t id) {
    if (!self || id == 0 || id > MAX_SPRITES) return NULL;
    return self->FPlayers[id];
}

/* Add a new bot player ------------------------------------------------ */
TScriptActivePlayer* ScriptPlayers_Add(TScriptPlayers* self,
                                       const TScriptNewPlayer* player,
                                       TJoinType joinType) {
    if (!self || !player) return NULL;

    /* Limit of 32 real players (as in Pascal) */
    extern int PlayersNum;   /* defined elsewhere */
    if (PlayersNum >= MAX_PLAYERS) return NULL;

    /* Random start position */
    void* a = malloc(16); /* placeholder for TVector2 */
    void* b = malloc(16);
    RandomizeStart(a, 0);

    /* Create sprite – returns new sprite index */
    int newIdx = CreateSprite(a, b, 1, 255, player->Sprite.Player, 1);
    free(a);
    free(b);
    if (newIdx <= 0 || newIdx > MAX_SPRITES) return NULL;

    TScriptActivePlayer* result = self->FPlayers[newIdx];

    /* Initialise the newly created sprite (mirroring Pascal) */
    /* The following assumes the sprite structure has the same fields. */
    extern void Sprite_ApplyShirtColorFromTeam(int idx);
    extern void Sprite_SetBrain(int idx, void* brain);
    extern void Sprite_SetDeadMeat(int idx, int alive);
    extern void Sprite_SetHealth(int idx, int health);
    extern void Sprite_SetWeapon(int idx, int primary);
    extern void Sprite_SetSecondaryWeapon(int idx, int secondary);
    extern void Sprite_SetDummy(int idx, int dummy);
    extern void Sprite_SetControlMethod(int idx, int method);
    extern void Sprite_SetJetColor(int idx, uint32_t color);
    extern void Sprite_FreeControls(int idx);
    extern void Sprite_Respawn(int idx);
    extern void Sprite_SetChatWarnings(int idx, int v);
    extern void Sprite_SetGrabsPerSecond(int idx, int v);

    Sprite_ApplyShirtColorFromTeam(newIdx);
    Sprite_SetBrain(newIdx, player->Sprite.Brain);
    Sprite_SetDeadMeat(newIdx, player->Alive);
    Sprite_SetHealth(newIdx, player->Health);
    Sprite_SetWeapon(newIdx, player->Primary.Gun);
    Sprite_SetSecondaryWeapon(newIdx, player->Secondary.Gun);
    Sprite_SetDummy(newIdx, player->Dummy);
    Sprite_SetControlMethod(newIdx, 0); /* BOT */
    Sprite_SetJetColor(newIdx,
        (/* existing jet color */0x00FFFFFF) + COLOR_TRANSPARENCY_BOT);
    Sprite_FreeControls(newIdx);
    Sprite_Respawn(newIdx);
    Sprite_SetControlMethod(newIdx, 0); /* BOT again */
    Sprite_SetChatWarnings(newIdx, 0);
    Sprite_SetGrabsPerSecond(newIdx, 0);

    /* Script dispatcher notifications (only when SCRIPT defined) */
#ifdef SCRIPT
    extern int Sprite_GetTeam(int idx);
    extern int Sprite_GetWeaponNum(int idx);
    extern int Sprite_GetSecondaryWeaponNum(int idx);
    extern int Sprite_GetWeaponAmmo(int idx);
    extern int Sprite_GetSecondaryWeaponAmmo(int idx);
    ScrptDispatcher_OnJoinTeam(newIdx,
                               Sprite_GetTeam(newIdx),
                               Sprite_GetTeam(newIdx),
                               1);
    ScrptDispatcher_OnWeaponChange(newIdx,
                                   Sprite_GetWeaponNum(newIdx),
                                   Sprite_GetSecondaryWeaponNum(newIdx),
                                   Sprite_GetWeaponAmmo(newIdx),
                                   Sprite_GetSecondaryWeaponAmmo(newIdx));
#endif

    /* Join message handling */
    const char* joinMsg = NULL;
    switch (joinType) {
        case TJoinNormal: {
            static const char* teamNames[] = {
                "the game", "alpha team", "bravo team",
                "charlie team", "delta team", "as spectator"
            };
            int team = player->Team;               /* assume 0..5 */
            if (team < 0) team = 0;
            if (team > 5) team = 5;
            joinMsg = teamNames[team];
            /* Broadcast join message */
            extern const char* Sprite_GetName(int idx);
            char buf[256];
            snprintf(buf, sizeof(buf), "%s has joined %s.", Sprite_GetName(newIdx), joinMsg);
            MainConsole_console(buf, ENTER_MESSAGE_COLOR);
            ServerSendNewPlayerInfo(newIdx, 0); /* JOIN_NORMAL */
            break;
        }
        case TJoinSilent:
            ServerSendNewPlayerInfo(newIdx, 1); /* JOIN_SILENT */
            break;
        default:
            /* Unknown join type – ignore */
            break;
    }

    /* Add to active list */
    extern void FPGList_Add(TFPGList* list, TScriptActivePlayer* item);
    FPGList_Add(self->FActivePlayers, result);

    return result;
}

/* Console output ------------------------------------------------------ */
void ScriptPlayers_WriteConsole(TScriptPlayers* self,
                                const char* text,
                                int32_t color) {
    (void)self;
    ServerHelper_WriteConsole(0, text, color);
}

/* Big text (type 1) --------------------------------------------------- */
void ScriptPlayers_BigText(TScriptPlayers* self,
                           uint8_t layer,
                           const char* text,
                           int delay,
                           int32_t color,
                           float scale,
                           int x,
                           int y) {
    (void)self;
    NetworkServerMessages_ServerSendSpecialMessage(text, 1, layer, delay,
                                                   scale, (uint32_t)color,
                                                   x, y, 0);
}

/* World text (type 2) ------------------------------------------------- */
void ScriptPlayers_WorldText(TScriptPlayers* self,
                             uint8_t layer,
                             const char* text,
                             int delay,
                             int32_t color,
                             float scale,
                             float x,
                             float y) {
    (void)self;
    NetworkServerMessages_ServerSendSpecialMessageF(text, 2, layer, delay,
                                                    scale, (uint32_t)color,
                                                    x, y, 0);
}

/* Find by name -------------------------------------------------------- */
TScriptActivePlayer* ScriptPlayers_GetByName(TScriptPlayers* self,
                                             const char* name) {
    if (!self || !name) return NULL;
    for (int i = 1; i <= MAX_PLAYERS; ++i) {
        TScriptActivePlayer* p = self->FPlayers[i];
        extern int Sprite_Active(int idx);
        extern const char* Sprite_GetName(int idx);
        if (!Sprite_Active(i)) continue;
        if (strcmp(Sprite_GetName(i), name) == 0) return p;
    }
    return NULL;
}

/* Find by IP ---------------------------------------------------------- */
TScriptActivePlayer* ScriptPlayers_GetByIP(TScriptPlayers* self,
                                           const char* ip) {
    if (!self || !ip) return NULL;
    for (int i = 1; i <= MAX_PLAYERS; ++i) {
        TScriptActivePlayer* p = self->FPlayers[i];
        extern int Sprite_Active(int idx);
        extern const char* Sprite_GetIP(int idx);
        if (!Sprite_Active(i)) continue;
        if (strcmp(Sprite_GetIP(i), ip) == 0) return p;
    }
    return NULL;
}

/* Broadcast public message -------------------------------------------- */
void ScriptPlayers_Tell(TScriptPlayers* self, const char* text) {
    (void)self;
    wchar_t* w = utf8_to_wide(text);
    if (w) {
        ServerSendStringMessage(w, ALL_PLAYERS, 255, MSGTYPE_PUB);
        free(w);
    }
}

/* Active list accessor ------------------------------------------------ */
TFPGList* ScriptPlayers_GetActive(TScriptPlayers* self) {
    return self ? self->FActivePlayers : NULL;
}

/* ------------------------------------------------------------------ */
/* TScriptPlayersAPI implementation                                      */
/* ------------------------------------------------------------------ */
TScriptPlayersAPI* ScriptPlayersAPI_Create(void) {
    TScriptPlayersAPI* self = (TScriptPlayersAPI*)calloc(1, sizeof(TScriptPlayersAPI));
    if (!self) return NULL;
    self->FPlayers = ScriptPlayers_Create();
    return self;
}

void ScriptPlayersAPI_Destroy(TScriptPlayersAPI* self) {
    if (!self) return;
    ScriptPlayers_Destroy(self->FPlayers);
    self->FPlayers = NULL;
    free(self);
}

/* The following three functions only forward to the Pascal‑script
 * integration layer; the real work is done by the script engine.
 * Here we provide empty bodies – the engine will call the helpers
 * registered at runtime. */
void ScriptPlayersAPI_CompilerRegister(TScriptPlayersAPI* self,
                                       void* compiler) {
    (void)self; (void)compiler;
    /* In the original Pascal code this registers the TPlayers and
     * TActivePlayerList classes with the compiler. The C side does not
     * have a Pascal compiler, so nothing is required here. */
}

void ScriptPlayersAPI_RuntimeRegisterApi(TScriptPlayersAPI* self,
                                         void* exec) {
    (void)self; (void)exec;
    /* Registration of methods / properties with the Pascal executor.
     * Not needed in pure C. */
}

void ScriptPlayersAPI_RuntimeRegisterVariables(TScriptPlayersAPI* self,
                                               void* exec) {
    (void)self; (void)exec;
    /* The Pascal version creates the global "Players" variable and
     * fills the active list. In C the global instance is already
     * accessible via ScriptPlayersAPI_GetPlayers(). */
    if (self && self->FPlayers) {
        /* Ensure active list is up‑to‑date */
        extern int Sprite_Active(int idx);
        extern void FPGList_Clear(TFPGList* list);
        extern void FPGList_Add(TFPGList* list, TScriptActivePlayer* item);
        FPGList_Clear(self->FPlayers->FActivePlayers);
        for (int i = 1; i <= MAX_PLAYERS; ++i) {
            if (Sprite_Active(i))
                FPGList_Add(self->FPlayers->FActivePlayers, self->FPlayers->FPlayers[i]);
        }
    }
}

TScriptPlayers* ScriptPlayersAPI_GetPlayers(TScriptPlayersAPI* self) {
    return self ? self->FPlayers : NULL;
}