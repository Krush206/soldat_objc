#ifndef SCRIPT_TEAM_H
#define SCRIPT_TEAM_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

/* Forward declarations */
struct ScriptActivePlayer;

/* Callback types */
typedef int8_t (*ScriptTeam_OnBeforeJoin)(struct ScriptActivePlayer *player,
                                         struct ScriptTeam *team,
                                         struct ScriptTeam *oldTeam);
typedef void (*ScriptTeam_OnJoin)(struct ScriptActivePlayer *player,
                                 struct ScriptTeam *team);
typedef void (*ScriptTeam_OnLeave)(struct ScriptActivePlayer *player,
                                  struct ScriptTeam *team,
                                  bool kicked);

/* Team structure */
typedef struct ScriptTeam {
    uint8_t id;
    struct ScriptActivePlayer **players;   /* dynamic array */
    size_t count;
    size_t capacity;

    /* Event callbacks */
    ScriptTeam_OnBeforeJoin onBeforeJoin;
    ScriptTeam_OnJoin onJoin;
    ScriptTeam_OnLeave onLeave;
} ScriptTeam;

/* External global score array (defined elsewhere) */
extern uint8_t TeamScore[];

/* Constructor / Destructor */
ScriptTeam *ScriptTeam_Create(uint8_t id);
void ScriptTeam_Destroy(ScriptTeam *team);

/* Player management */
void ScriptTeam_AddPlayer(ScriptTeam *team, struct ScriptActivePlayer *player);
void ScriptTeam_RemovePlayer(ScriptTeam *team, struct ScriptActivePlayer *player);

/* Properties */
uint8_t ScriptTeam_GetScore(const ScriptTeam *team);
void ScriptTeam_SetScore(ScriptTeam *team, uint8_t score);

struct ScriptActivePlayer *ScriptTeam_GetPlayer(const ScriptTeam *team, uint8_t index);
uint8_t ScriptTeam_GetCount(const ScriptTeam *team);
uint8_t ScriptTeam_GetID(const ScriptTeam *team);

/* Callback accessors */
ScriptTeam_OnBeforeJoin ScriptTeam_GetOnBeforeJoin(const ScriptTeam *team);
void ScriptTeam_SetOnBeforeJoin(ScriptTeam *team, ScriptTeam_OnBeforeJoin cb);

ScriptTeam_OnJoin ScriptTeam_GetOnJoin(const ScriptTeam *team);
void ScriptTeam_SetOnJoin(ScriptTeam *team, ScriptTeam_OnJoin cb);

ScriptTeam_OnLeave ScriptTeam_GetOnLeave(const ScriptTeam *team);
void ScriptTeam_SetOnLeave(ScriptTeam *team, ScriptTeam_OnLeave cb);

/* API registration (mirrors Pascal TScriptTeamAPI) */
void ScriptTeamAPI_CompilerRegister(void *compiler);
void ScriptTeamAPI_RuntimeRegisterApi(void *exec);

#endif /* SCRIPT_TEAM_H */