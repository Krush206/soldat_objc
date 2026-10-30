/* LobbyClient — C port of server/LobbyClient.pas (stub implementation) */

#include "LobbyClient.h"
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

/* ---------- Internal configuration storage ---------- */
static struct {
    char *lobby_url;
    char *hostname;
    int max_players;
    int port;
    char *password;
    char *map_name;
    int game_mode;
    int advance_mode;
    int bonus_freq;
    char *info;
    int realistic_mode;
    int respawn_time;
    int survival_mode;
    char *version;
    bool modded;
    bool wm_modified;
    int bots_num;
    int players_num;
    char **player_names;
    int player_count;
} g_cfg = {0};

/* ---------- Helper: OS detection ---------- */
static int GetOS(void) {
#if defined(_WIN32) || defined(_WIN64)
    return 0;               /* Windows */
#elif defined(__linux__)
    return 1;               /* Linux */
#elif defined(__APPLE__) && defined(__MACH__)
    return 2;               /* macOS */
#else
    return -1;              /* Unknown */
#endif
}

/* ---------- Thread routine (stub) ---------- */
static void *LobbyThreadFunc(void *arg) {
    (void)arg;
    /* Stub: In a real implementation, this would use cURL and cJSON to register with the lobby server.
     * For now, we just log and exit. */
    return NULL;
}

/* ---------- Public API ---------- */
void LobbyClient_Init(const char *lobby_url,
                      const char *hostname,
                      int max_players,
                      int port,
                      const char *password,
                      const char *map_name,
                      int game_mode,
                      int advance_mode,
                      int bonus_freq,
                      const char *info,
                      int realistic_mode,
                      int respawn_time,
                      int survival_mode,
                      const char *version,
                      bool modded,
                      bool wm_modified,
                      int bots_num,
                      int players_num,
                      const char **player_names,
                      int player_count) {
    /* Free any previous config */
    LobbyClient_Cleanup();

    g_cfg.lobby_url = lobby_url ? strdup(lobby_url) : NULL;
    g_cfg.hostname = hostname ? strdup(hostname) : NULL;
    g_cfg.max_players = max_players;
    g_cfg.port = port;
    g_cfg.password = password ? strdup(password) : NULL;
    g_cfg.map_name = map_name ? strdup(map_name) : NULL;
    g_cfg.game_mode = game_mode;
    g_cfg.advance_mode = advance_mode;
    g_cfg.bonus_freq = bonus_freq;
    g_cfg.info = info ? strdup(info) : NULL;
    g_cfg.realistic_mode = realistic_mode;
    g_cfg.respawn_time = respawn_time;
    g_cfg.survival_mode = survival_mode;
    g_cfg.version = version ? strdup(version) : NULL;
    g_cfg.modded = modded;
    g_cfg.wm_modified = wm_modified;
    g_cfg.bots_num = bots_num;
    g_cfg.players_num = players_num;
    g_cfg.player_count = player_count > 0 ? player_count : 0;

    if (g_cfg.player_count > 0 && player_names) {
        g_cfg.player_names = malloc(g_cfg.player_count * sizeof(char *));
        for (int i = 0; i < g_cfg.player_count; ++i) {
            g_cfg.player_names[i] = player_names[i] ? strdup(player_names[i]) : NULL;
        }
    } else {
        g_cfg.player_names = NULL;
        g_cfg.player_count = 0;
    }
}

void LobbyClient_RegisterAsync(void) {
    pthread_t thread;
    if (pthread_create(&thread, NULL, LobbyThreadFunc, NULL) != 0) {
        /* Failed to create thread */
        return;
    }
    pthread_detach(thread);
}

void LobbyClient_Cleanup(void) {
    free(g_cfg.lobby_url);
    free(g_cfg.hostname);
    free(g_cfg.password);
    free(g_cfg.map_name);
    free(g_cfg.info);
    free(g_cfg.version);

    if (g_cfg.player_names) {
        for (int i = 0; i < g_cfg.player_count; ++i) {
            free(g_cfg.player_names[i]);
        }
        free(g_cfg.player_names);
    }

    memset(&g_cfg, 0, sizeof(g_cfg));
}