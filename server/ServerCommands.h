#ifndef SERVER_COMMANDS_H
#define SERVER_COMMANDS_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Forward declarations of types used by the command system */
struct Sprite;
struct Player;
struct Vector2;

/* Command function signature used by the command dispatcher */
typedef void (*CommandFunc)(int argc, const char * const *argv, uint8_t sender);

/* Initialise all server‑side console commands */
void InitServerCommands(void);

/* Individual command implementations – exported for possible scripting / testing */
void CommandAddbot   (int argc, const char * const *argv, uint8_t sender);
void CommandNextmap  (int argc, const char * const *argv, uint8_t sender);
void CommandMap      (int argc, const char * const *argv, uint8_t sender);
void CommandPause    (int argc, const char * const *argv, uint8_t sender);
void CommandUnpause  (int argc, const char * const *argv, uint8_t sender);
void CommandRestart  (int argc, const char * const *argv, uint8_t sender);
void CommandKick     (int argc, const char * const *argv, uint8_t sender);
void CommandKicklast (int argc, const char * const *argv, uint8_t sender);
void CommandBan      (int argc, const char * const *argv, uint8_t sender);
void CommandBanIPHW  (int argc, const char * const *argv, uint8_t sender);
void CommandUnban    (int argc, const char * const *argv, uint8_t sender);
void CommandUnbanlast(int argc, const char * const *argv, uint8_t sender);
void CommandAdm      (int argc, const char * const *argv, uint8_t sender);
void CommandAdmip    (int argc, const char * const *argv, uint8_t sender);
void CommandUnadm    (int argc, const char * const *argv, uint8_t sender);
void CommandSetteam  (int argc, const char * const *argv, uint8_t sender);
void CommandSay      (int argc, const char * const *argv, uint8_t sender);
void CommandKill     (int argc, const char * const *argv, uint8_t sender);
void CommandLoadwep  (int argc, const char * const *argv, uint8_t sender);
void CommandLoadcon  (int argc, const char * const *argv, uint8_t sender);
void CommandLoadlist (int argc, const char * const *argv, uint8_t sender);
void CommandPm       (int argc, const char * const *argv, uint8_t sender);
void CommandGmute    (int argc, const char * const *argv, uint8_t sender);
void CommandUngmute  (int argc, const char * const *argv, uint8_t sender);
void CommandAddmap   (int argc, const char * const *argv, uint8_t sender);
void CommandDelmap   (int argc, const char * const *argv, uint8_t sender);
void CommandTempban  (int argc, const char * const *argv, uint8_t sender);
void CommandWeaponon (int argc, const char * const *argv, uint8_t sender);
void CommandWeaponoff(int argc, const char * const *argv, uint8_t sender);
void CommandBanlist  (int argc, const char * const *argv, uint8_t sender);
void CommandNetStats (int argc, const char * const *argv, uint8_t sender);
void CommandPlayerCommand(int argc, const char * const *argv, uint8_t sender);
void CommandInfo     (int argc, const char * const *argv, uint8_t sender);
void CommandAdminlog (int argc, const char * const *argv, uint8_t sender);
void CommandVotemap  (int argc, const char * const *argv, uint8_t sender);
void CommandRecompile(int argc, const char * const *argv, uint8_t sender);
void CommandRecord   (int argc, const char * const *argv, uint8_t sender);
void CommandStop     (int argc, const char * const *argv, uint8_t sender);
void CommandListPlayers(int argc, const char * const *argv, uint8_t sender);

#ifdef __cplusplus
}
#endif

#endif /* SERVER_COMMANDS_H */