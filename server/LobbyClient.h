#ifndef LOBBY_CLIENT_H
#define LOBBY_CLIENT_H

#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Initialize lobby client with server configuration.
 * All string arguments are copied internally.
 */
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
                      int player_count);

/* Start asynchronous registration of the server with the lobby.
 * This spawns a background thread that performs the HTTP POST.
 */
void LobbyClient_RegisterAsync(void);

/* Clean up any resources allocated by LobbyClient_Init. */
void LobbyClient_Cleanup(void);

#ifdef __cplusplus
}
#endif

#endif /* LOBBY_CLIENT_H */