#ifndef SERVER_H
#define SERVER_H

#include <stdbool.h>
#include <stdint.h>
#include "Constants.h"
#include "Cvar.h"
#include "Net.h"
#include "Sprites.h"
#include "Things.h"
#include "MapFile.h"
#include "BanSystem.h"
#include "LogFile.h"
#include "Console.h"
#include "ScriptDispatcher.h"
#include "ServerLauncherIPC.h"
#include "LobbyClient.h"
#include "Steam.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Global state */
extern bool ProgReady;
extern char BaseDirectory[PATH_MAX];
extern char UserDirectory[PATH_MAX];
extern uint64_t MainThreadID;

/* CVars (declared in Cvar.c) */
extern TBooleanCvar log_enable;
extern TIntegerCvar log_level;
extern TIntegerCvar log_filesupdate;
extern TBooleanCvar log_timestamp;

extern TStringCvar fs_mod;
extern TBooleanCvar fs_portable;
extern TStringCvar fs_basepath;
extern TStringCvar fs_userpath;

extern TBooleanCvar demo_autorecord;

extern TIntegerCvar sv_respawntime;
extern TIntegerCvar sv_respawntime_minwave;
extern TIntegerCvar sv_respawntime_maxwave;

extern TIntegerCvar sv_dm_limit;
extern TIntegerCvar sv_pm_limit;
extern TIntegerCvar sv_tm_limit;
extern TIntegerCvar sv_rm_limit;

extern TIntegerCvar sv_inf_redaward;
extern TIntegerCvar sv_inf_limit;
extern TIntegerCvar sv_inf_bluelimit;

extern TIntegerCvar sv_htf_limit;
extern TIntegerCvar sv_htf_pointstime;

extern TIntegerCvar sv_ctf_limit;

extern TIntegerCvar sv_bonus_frequency;
extern TBooleanCvar sv_bonus_flamer;
extern TBooleanCvar sv_bonus_predator;
extern TBooleanCvar sv_bonus_berserker;
extern TBooleanCvar sv_bonus_vest;
extern TBooleanCvar sv_bonus_cluster;

extern TBooleanCvar sv_stationaryguns;

extern TStringCvar sv_password;
extern TStringCvar sv_adminpassword;
extern TIntegerCvar sv_maxplayers;
extern TIntegerCvar sv_maxspectators;
extern TBooleanCvar sv_spectatorchat;
extern TStringCvar sv_greeting;
extern TStringCvar sv_greeting2;
extern TStringCvar sv_greeting3;
extern TStringCvar sv_info;
extern TIntegerCvar sv_minping;
extern TIntegerCvar sv_maxping;
extern TIntegerCvar sv_votepercent;
extern TBooleanCvar sv_lockedmode;
extern TStringCvar sv_pidfilename;
extern TStringCvar sv_maplist;
extern TBooleanCvar sv_lobby;
extern TStringCvar sv_lobbyurl;

extern TBooleanCvar sv_steamonly;

#ifdef STEAM
extern TBooleanCvar sv_voicechat;
extern TBooleanCvar sv_voicechat_alltalk;
extern TStringCvar sv_setsteamaccount;
#endif

extern TIntegerCvar sv_warnings_flood;
extern TIntegerCvar sv_warnings_ping;
extern TIntegerCvar sv_warnings_votecheat;
extern TIntegerCvar sv_warnings_knifecheat;
extern TIntegerCvar sv_warnings_tk;

extern TBooleanCvar sv_anticheatkick;
extern TBooleanCvar sv_punishtk;
extern TBooleanCvar sv_botbalance;
extern TBooleanCvar sv_echokills;
extern TBooleanCvar sv_antimassflag;
extern TIntegerCvar sv_healthcooldown;
extern TBooleanCvar sv_teamcolors;
extern TBooleanCvar sv_pauseonidle;

extern TIntegerCvar net_port;
extern TStringCvar net_ip;
extern TStringCvar net_adminip;
extern TIntegerCvar net_lan;
extern TBooleanCvar net_allowdownload;
extern TIntegerCvar net_maxadminconnections;
extern TIntegerCvar net_rcon_limit;
extern TIntegerCvar net_rcon_burst;

extern TIntegerCvar net_floodingpacketslan;
extern TIntegerCvar net_floodingpacketsinternet;

extern TIntegerCvar net_t1_snapshot;
extern TIntegerCvar net_t1_majorsnapshot;
extern TIntegerCvar net_t1_deadsnapshot;
extern TIntegerCvar net_t1_heartbeat;
extern TIntegerCvar net_t1_delta;
extern TIntegerCvar net_t1_ping;
extern TIntegerCvar net_t1_thingsnapshot;

extern TIntegerCvar bots_random_noteam;
extern TIntegerCvar bots_random_alpha;
extern TIntegerCvar bots_random_bravo;
extern TIntegerCvar bots_random_charlie;
extern TIntegerCvar bots_random_delta;
extern TIntegerCvar bots_difficulty;
extern TBooleanCvar bots_chat;

extern TBooleanCvar sc_enable;
extern TStringCvar sc_onscriptcrash;
extern TBooleanCvar sc_safemode;
extern TBooleanCvar sc_allowdlls;
extern TIntegerCvar sc_sandboxed;
extern TStringCvar sc_defines;
extern TStringCvar sc_searchpaths;

extern TBooleanCvar fileserver_enable;
extern TIntegerCvar fileserver_port;
extern TStringCvar fileserver_ip;
extern TIntegerCvar fileserver_maxconnections;

extern TBooleanCvar launcher_ipc_enable;
extern TIntegerCvar launcher_ipc_port;
extern TIntegerCvar launcher_ipc_reconnect_rate;

/* syncable cvars */
extern TIntegerCvar sv_gamemode;
extern TBooleanCvar sv_friendlyfire;
extern TIntegerCvar sv_timelimit;
extern TIntegerCvar sv_maxgrenades;
extern TBooleanCvar sv_bullettime;
extern TBooleanCvar sv_sniperline;
extern TBooleanCvar sv_balanceteams;
extern TBooleanCvar sv_survivalmode;
extern TBooleanCvar sv_survivalmode_antispy;
extern TBooleanCvar sv_survivalmode_clearweapons;
extern TBooleanCvar sv_realisticmode;
extern TBooleanCvar sv_advancemode;
extern TIntegerCvar sv_advancemode_amount;
extern TBooleanCvar sv_guns_collide;
extern TBooleanCvar sv_kits_collide;
extern TBooleanCvar sv_minimap_locations;
extern TBooleanCvar sv_advancedspectator;
extern TBooleanCvar sv_radio;
extern TSingleCvar sv_gravity;
extern TStringCvar sv_hostname;
extern TIntegerCvar sv_killlimit;
extern TStringCvar sv_downloadurl;
extern TBooleanCvar sv_pure;
extern TStringCvar sv_website;

#ifdef ENABLE_FAE
extern TBooleanCvar ac_enable;
#endif

/* Misc globals */
extern char ServerIP[16];
extern int ServerPort;
extern int BonusFreq;
extern uint8_t WeaponActive[17];          /* index -1..15, we store 0..16 */
extern TStringList *MapsList;
extern uint8_t LastPlayer;
extern TMsg_BulletSnapshot OldBulletSnapshotMsg[MAX_SPRITES + 1];
extern char MuteList[MAX_PLAYERS + 1][MAX_NAME_LENGTH];
extern char MuteName[MAX_PLAYERS + 1][MAX_NAME_LENGTH];
extern char TKList[MAX_PLAYERS + 1][MAX_IP_LENGTH];
extern uint8_t TKListKills[MAX_PLAYERS + 1];
extern int64_t TCPBytesSent;
extern int64_t TCPBytesReceived;
extern TConsole MainConsole;
extern TStringList *RemoteIPs;
extern TStringList *AdminIPs;
extern char FloodIP[MAX_FLOODIPS + 1][MAX_IP_LENGTH];
extern int FloodNum[MAX_FLOODIPS + 1];
extern char LastReqIP[4][MAX_IP_LENGTH];
extern uint8_t LastReqID;
extern char DropIP[MAX_IP_LENGTH];
extern int WaveRespawnTime;
extern int WaveRespawnCounter;
extern int WeaponsInGame;
extern uint8_t BulletWarningCount[MAX_SPRITES + 1];
extern uint8_t CheatTag[MAX_SPRITES + 1];
#ifdef RCON
extern TAdminServer *AdminServer;
#endif
extern float ShotDistance;
extern float ShotLife;
extern int ShotRicochet;
extern int HTFTime;
extern char WMName[MAX_NAME_LENGTH];
extern char WMVersion[MAX_NAME_LENGTH];
extern char LastWepMod[MAX_NAME_LENGTH];
extern char ModDir[MAX_PATH_LENGTH];
extern TServerNetwork *UDP;
extern TServerLauncherIPC *LauncherIPC;
extern TLobbyThread *LobbyThread;
#ifdef STEAM
extern TSteamGS *SteamAPI;
#endif

/* Function prototypes */
void ActivateServer(void);
uint8_t AddBotPlayer(const char *Name, int team);
void StartServer(void);
bool LoadMapsList(const char *Filename);
void LoadWeapons(const char *filename);
void ShutDown(void);
void NextMap(void);
void SpawnThings(uint8_t style, uint8_t amount);
bool KickPlayer(uint8_t num, bool Ban, int why, int time, const char *Reason);
bool PrepareMapChange(const char *Name);

#ifdef STEAM
#ifdef STEAMSTATS
void RequestUserStats(CSteamID Player);
void GSStatsStored(PGSStatsStored_t Callback);
void StoreStats(CSteamID Player);
#endif
void RunManualCallbacks(void);
#endif

#ifdef __cplusplus
}
#endif

#endif /* SERVER_H */