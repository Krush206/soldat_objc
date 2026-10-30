#include "ScriptTeam.h"
#include <stdlib.h>
#include <string.h>

/* Ensure the players array has at least 'min_capacity' slots */
static bool ensure_capacity(ScriptTeam *team, size_t min_capacity) {
    if (team->capacity >= min_capacity)
        return true;

    size_t new_cap = team->capacity ? team->capacity * 2 : 4;
    if (new_cap < min_capacity)
        new_cap = min_capacity;

    struct ScriptActivePlayer **new_players =
        realloc(team->players, new_cap * sizeof(*team->players));
    if (!new_players)
        return false;

    team->players = new_players;
    team->capacity = new_cap;
    return true;
}

/* Constructor */
ScriptTeam *ScriptTeam_Create(uint8_t id) {
    ScriptTeam *team = calloc(1, sizeof(*team));
    if (!team)
        return NULL;

    team->id = id;
    team->players = NULL;
    team->count = 0;
    team->capacity = 0;
    team->onBeforeJoin = NULL;
    team->onJoin = NULL;
    team->onLeave = NULL;
    return team;
}

/* Destructor */
void ScriptTeam_Destroy(ScriptTeam *team) {
    if (!team)
        return;
    free(team->players);
    free(team);
}

/* Add a player to the team */
void ScriptTeam_AddPlayer(ScriptTeam *team, struct ScriptActivePlayer *player) {
    if (!team || !player)
        return;
    if (!ensure_capacity(team, team->count + 1))
        return; /* allocation failure – silently ignore */
    team->players[team->count++] = player;
}

/* Remove a player from the team (first matching pointer) */
void ScriptTeam_RemovePlayer(ScriptTeam *team, struct ScriptActivePlayer *player) {
    if (!team || !player)
        return;
    for (size_t i = 0; i < team->count; ++i) {
        if (team->players[i] == player) {
            /* shift remaining elements left */
            memmove(&team->players[i], &team->players[i + 1],
                    (team->count - i - 1) * sizeof(*team->players));
            --team->count;
            break;
        }
    }
}

/* Score property */
uint8_t ScriptTeam_GetScore(const ScriptTeam *team) {
    if (!team)
        return 0;
    return TeamScore[team->id];
}

void ScriptTeam_SetScore(ScriptTeam *team, uint8_t score) {
    if (!team)
        return;
    TeamScore[team->id] = score;
    /* In original Pascal: SortPlayers; – omitted because sorting
       depends on global game state not represented here. */
}

/* Player accessor */
struct ScriptActivePlayer *ScriptTeam_GetPlayer(const ScriptTeam *team, uint8_t index) {
    if (!team || index >= team->count)
        return NULL;
    return team->players[index];
}

/* Count property */
uint8_t ScriptTeam_GetCount(const ScriptTeam *team) {
    if (!team)
        return 0;
    return (uint8_t)team->count;   /* Count fits in Byte per Pascal definition */
}

/* ID property */
uint8_t ScriptTeam_GetID(const ScriptTeam *team) {
    return team ? team->id : 0;
}

/* Callback getters / setters */
ScriptTeam_OnBeforeJoin ScriptTeam_GetOnBeforeJoin(const ScriptTeam *team) {
    return team ? team->onBeforeJoin : NULL;
}
void ScriptTeam_SetOnBeforeJoin(ScriptTeam *team, ScriptTeam_OnBeforeJoin cb) {
    if (team)
        team->onBeforeJoin = cb;
}

ScriptTeam_OnJoin ScriptTeam_GetOnJoin(const ScriptTeam *team) {
    return team ? team->onJoin : NULL;
}
void ScriptTeam_SetOnJoin(ScriptTeam *team, ScriptTeam_OnJoin cb) {
    if (team)
        team->onJoin = cb;
}

ScriptTeam_OnLeave ScriptTeam_GetOnLeave(const ScriptTeam *team) {
    return team ? team->onLeave : NULL;
}
void ScriptTeam_SetOnLeave(ScriptTeam *team, ScriptTeam_OnLeave cb) {
    if (team)
        team->onLeave = cb;
}

/* ------------------------------------------------------------------ */
/* API registration – mirrors TScriptTeamAPI from Pascal.              */
/* The real implementation would interact with the Pascal compiler /   */
/* runtime structures (TPascalCompiler, TPascalExec). Here we provide  */
/* empty bodies with explanatory comments.                            */
/* ------------------------------------------------------------------ */

void ScriptTeamAPI_CompilerRegister(void *compiler) {
    /* Expected to register the TTeam class and its properties/events
       with the Pascal compiler (TPascalCompiler). Since the C side has
       no knowledge of the compiler internals, this function is a stub
       for the host application to implement. */
    (void)compiler;
}

void ScriptTeamAPI_RuntimeRegisterApi(void *exec) {
    /* Expected to expose the ScriptTeam methods and properties to the
       Pascal runtime (TPascalExec) via property/event helpers.
       Implementation depends on the script core internals; left empty. */
    (void)exec;
}