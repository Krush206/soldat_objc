#ifndef SCRIPTPLAYERS_H
#define SCRIPTPLAYERS_H

#include <stddef.h>
#include <stdint.h>

/* Forward declarations of related types */
typedef struct TScriptActivePlayer TScriptActivePlayer;
typedef struct TScriptNewPlayer TScriptNewPlayer;
typedef struct TFPGList TFPGList;

/* Opaque handle for the players manager */
typedef struct TScriptPlayers TScriptPlayers;

/* Opaque handle for the API object */
typedef struct TScriptPlayersAPI TScriptPlayersAPI;

/* Constants (values taken from the original Pascal code) */
#define MAX_SPRITES 255
#define MAX_PLAYERS 32
#define ENTER_MESSAGE_COLOR 0xFFFFFFFF
#define COLOR_TRANSPARENCY_BOT 0xFF000000
#define ALL_PLAYERS 255
#define MSGTYPE_PUB 0

/* Join type enumeration */
typedef enum {
    TJoinNormal = 0,
    TJoinSilent = 1
} TJoinType;

/* ------------------------------------------------------------------ */
/* TScriptPlayers public interface                                     */
/* ------------------------------------------------------------------ */
TScriptPlayers* ScriptPlayers_Create(void);
void ScriptPlayers_Destroy(TScriptPlayers* self);

/* Adds a new player (bot) to the game. Returns the created active player
 * or NULL if the maximum number of players has been reached. */
TScriptActivePlayer* ScriptPlayers_Add(TScriptPlayers* self,
                                       const TScriptNewPlayer* player,
                                       TJoinType joinType);

/* Sends a console message to all players. */
void ScriptPlayers_WriteConsole(TScriptPlayers* self,
                                const char* text,
                                int32_t color);

/* Shows a big text message on clients. */
void ScriptPlayers_BigText(TScriptPlayers* self,
                           uint8_t layer,
                           const char* text,
                           int delay,
                           int32_t color,
                           float scale,
                           int x,
                           int y);

/* Shows a world text message on clients. */
void ScriptPlayers_WorldText(TScriptPlayers* self,
                             uint8_t layer,
                             const char* text,
                             int delay,
                             int32_t color,
                             float scale,
                             float x,
                             float y);

/* Finds an active player by name (case‑sensitive). Returns NULL if not found. */
TScriptActivePlayer* ScriptPlayers_GetByName(TScriptPlayers* self,
                                             const char* name);

/* Finds an active player by IP address. Returns NULL if not found. */
TScriptActivePlayer* ScriptPlayers_GetByIP(TScriptPlayers* self,
                                           const char* ip);

/* Broadcasts a public chat message to everybody. */
void ScriptPlayers_Tell(TScriptPlayers* self, const char* text);

/* Returns the player structure for the given numeric ID (1 … MAX_SPRITES).
 * Returns NULL if the ID is out of range. */
TScriptActivePlayer* ScriptPlayers_GetPlayer(TScriptPlayers* self,
                                             uint8_t id);

/* Returns the list of currently active players. */
TFPGList* ScriptPlayers_GetActive(TScriptPlayers* self);

/* ------------------------------------------------------------------ */
/* TScriptPlayersAPI public interface                                   */
/* ------------------------------------------------------------------ */
TScriptPlayersAPI* ScriptPlayersAPI_Create(void);
void ScriptPlayersAPI_Destroy(TScriptPlayersAPI* self);

/* Registers the script classes with the Pascal compiler. */
void ScriptPlayersAPI_CompilerRegister(TScriptPlayersAPI* self,
                                       void* compiler);

/* Registers the runtime API (methods, properties) with the Pascal executor. */
void ScriptPlayersAPI_RuntimeRegisterApi(TScriptPlayersAPI* self,
                                         void* exec);

/* Registers the global "Players" variable for scripts. */
void ScriptPlayersAPI_RuntimeRegisterVariables(TScriptPlayersAPI* self,
                                               void* exec);

/* Accessor for the internal TScriptPlayers instance. */
TScriptPlayers* ScriptPlayersAPI_GetPlayers(TScriptPlayersAPI* self);

#endif /* SCRIPTPLAYERS_H */