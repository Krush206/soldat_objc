#ifndef SERVER_HELPER_H
#define SERVER_HELPER_H

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include "Cvar.h"
#include "Classes.h"
#include "Sprites.h"

/* Forward declarations of external globals */
extern bool CvarsInitialized;
extern TBooleanCvar log_timestamp;
extern TIntegerCvar sv_gamemode;
extern TIntegerCvar sv_respawntime_minwave;
extern TIntegerCvar sv_respawntime_maxwave;
extern int MapIndex;
extern StringList *MapsList;
extern int PlayersNum;
extern int WaveRespawnTime;
extern TSprite Sprite[];          /* defined in shared/mechanics/Sprites.h */

/* Function prototypes */

void ServerHelper_WriteLn(const char *s);
const char *ServerHelper_IDToName(int id);
const char *ServerHelper_TeamToName(int id);
int ServerHelper_NameToID(const char *name);
const char *ServerHelper_NameToHW(const char *name);
int ServerHelper_FindLowestTeam(const int *arr, int count);
void ServerHelper_SaveTxtLists(void);
void ServerHelper_SaveMapList(void);
uint32_t ServerHelper_RGB(uint8_t r, uint8_t g, uint8_t b);
uint8_t ServerHelper_FixTeam(uint8_t team);
const char *ServerHelper_WeaponNameByNum(int num);
int ServerHelper_CheckFileSize(const char *filename);
const char *ServerHelper_CheckNextMap(void);
void ServerHelper_WritePID(void);
int ServerHelper_GetPID(void);
void ServerHelper_WriteConsole(uint8_t id, const char *text, uint32_t colour);
void ServerHelper_UpdateWaveRespawnTime(void);
const char *ServerHelper_RandomBot(void);
void ServerHelper_DoBalanceBots(uint8_t leftGame, uint8_t newTeam);

#endif /* SERVER_HELPER_H */