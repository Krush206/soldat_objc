#include "ServerCommands.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

/* Project headers – assumed to exist in the source tree */
#include "Command.h"
#include "Server.h"
#include "BanSystem.h"
#include "Net.h"
#include "Demo.h"
#include "Sprites.h"
#include "Weapons.h"
#include "Constants.h"
#include "Util.h"
#include "Version.h"
#include "NetworkServerMessages.h"
#include "NetworkServerGame.h"
#include "NetworkServerConnection.h"
#include "NetworkServerFunctions.h"
#include "PolyMap.h"
#include "Game.h"
#include "Things.h"
#include "ServerHelper.h"

#ifdef SCRIPT
#include "ScriptDispatcher.h"
#endif

/* ----------------------------------------------------------------------
 * Helper macros / small utilities used by the command implementations
 * ---------------------------------------------------------------------- */
#define MIN(a,b) ((a) < (b) ? (a) : (b))
#define MAX(a,b) ((a) > (b) ? (a) : (b))

static inline int StrToIntDef(const char *s, int def)
{
    if (!s || !*s) return def;
    char *end;
    long v = strtol(s, &end, 10);
    return (end != s) ? (int)v : def;
}

/* ----------------------------------------------------------------------
 * External globals (declared in other modules)
 * ---------------------------------------------------------------------- */
extern int PlayersNum;
extern int MAX_PLAYERS;
extern int MAX_SPRITES;
extern int MAX_BULLETS;
extern int MAX_THINGS;

extern struct MapInfo Map;
extern int MapChangeCounter;
extern char *UserDirectory;
extern char *ServerIP;
extern int ServerPort;
extern uint32_t LoadedWMChecksum;
extern uint32_t DefaultWMChecksum;
extern char *WMName;
extern char *WMVersion;
extern char *LastWepMod;
extern int LastPlayer;
extern char *LastBan;
extern char *LastBanHW;
extern char **MuteList;
extern char **mutename;
extern int *VoteCooldown;
extern bool VoteActive;
extern char *VoteTarget;
extern bool *VoteHasVoted;
extern int VoteType;
extern struct Sprite Sprite[];
extern struct SortedPlayer SortedPlayers[];
extern struct ServerVar sv_gamemode;
extern struct ServerVar sv_timelimit;
extern struct ServerVar sv_hostname;
extern struct ServerVar sv_greeting;
extern struct ServerVar sv_greeting2;
extern struct ServerVar sv_greeting3;
extern struct ServerVar sv_realisticmode;
extern struct ServerVar sv_lockedmode;
extern struct ServerVar sv_adminpassword;
extern struct ServerVar sc_enable;
extern struct UDPServer UDP;

/* Forward declarations of functions defined elsewhere */
extern void AddBotPlayer(const char *name, int team);
extern void NextMap(void);
extern bool GetMapInfo(const char *mapName, const char *userDir, struct MapInfo *info);
extern void PrepareMapChange(const char *mapName);
extern void ServerSyncMsg(void);
extern void KickPlayer(int playerNum, bool ban, int reason, int minutes, const char *msg);
extern struct CommandTargetList CommandTarget(const char *name, uint8_t sender);
extern bool IsRemoteAdminIP(const char *ip);
extern void RemoteIPs_Add(const char *ip);
extern void RemoteIPs_Delete(const char *ip);
extern int RemoteIPs_IndexOf(const char *ip);
extern void SaveTxtLists(void);
extern void SaveMapList(void);
extern void MainConsole_Console(const char *msg, int color, uint8_t sender);
extern void ServerSendStringMessage(const wchar_t *msg, int to, uint8_t from, int msgType);
extern void ServerVars(int playerNum);
extern void LoadWeapons(const char *name);
extern void LoadConfig(const char *filename);
extern void ServerDisconnect(void);
extern void StartServer(void);
extern void SetWeaponActive(int playerNum, int weaponIdx, bool enable);
extern void AddBannedIP(const char *ip, const char *reason, int minutes);
extern void AddBannedHW(const char *hw, const char *reason, int minutes);
extern bool DelBannedIP(const char *ip);
extern bool DelBannedHW(const char *hw);
extern const char *IDToName(int id);
extern void ServerSendVoteOn(int voteType, uint8_t starter, const char *target, const char *param);
extern void StartVote(uint8_t starter, int type, const char *target, const char *param);
extern void CountVote(uint8_t voter);
extern const char *CheckNextMap(void);
extern struct DemoRecorder DemoRecorder;
extern void ScrptDispatcher_Prepare(void);
extern void ScrptDispatcher_Launch(const char *name);
extern bool ScrptDispatcher_OnVoteMapStart(uint8_t starter, const char *mapName);
extern void ScrptDispatcher_OnVoteMap(uint8_t voter, const char *mapName);
extern struct StringList *ScrptDispatcher_ScriptList(void);

/* ----------------------------------------------------------------------
 * Command implementations
 * ---------------------------------------------------------------------- */

void CommandAddbot(int argc, const char * const *argv, uint8_t sender)
{
    if (argc < 2) return;
    const char *name = argv[1];
    if (!name || !*name) return;
    if (PlayersNum >= MAX_PLAYERS) return;

    int teamSet = 0;
    const char *cmd = argv[0];
    if (cmd && strlen(cmd) >= 7)
        teamSet = StrToIntDef(&cmd[6], 0);   // 7th character (0‑based index 6)

    AddBotPlayer(name, teamSet);
}

void CommandNextmap(int argc, const char * const *argv, uint8_t sender)
{
    (void)argc; (void)argv; (void)sender;
    NextMap();
}

void CommandMap(int argc, const char * const *argv, uint8_t sender)
{
    if (argc < 2) return;
    const char *mapName = argv[1];
    if (!mapName || !*mapName) return;

    struct MapInfo status;
    if (GetMapInfo(mapName, UserDirectory, &status))
    {
        PrepareMapChange(mapName);
    }
    else
    {
#ifdef STEAM
        if (status.WorkshopID > 0)
        {
            /* MapChangeItemID is a global in the original code */
            extern uint64_t MapChangeItemID;
            MapChangeItemID = status.WorkshopID;
            if (SteamAPI_UGC_DownloadItem(MapChangeItemID, true))
                MainConsole_Console("[Steam] Workshop map not found in cache, downloading.", WARNING_MESSAGE_COLOR, sender);
            else
                MainConsole_Console("[Steam] Workshop map is invalid", WARNING_MESSAGE_COLOR, sender);
        }
        else
#endif
        {
            char buf[256];
            snprintf(buf, sizeof(buf), "Map not found (%s)", mapName);
            MainConsole_Console(buf, WARNING_MESSAGE_COLOR, sender);
        }
    }
}

void CommandPause(int argc, const char * const *argv, uint8_t sender)
{
    (void)argc; (void)argv; (void)sender;
    MapChangeCounter = 999999999;
    ServerSyncMsg();
}

void CommandUnpause(int argc, const char * const *argv, uint8_t sender)
{
    (void)argc; (void)argv; (void)sender;
    MapChangeCounter = -60;
    ServerSyncMsg();
}

void CommandRestart(int argc, const char * const *argv, uint8_t sender)
{
    (void)argc; (void)argv; (void)sender;
    PrepareMapChange(Map.Name);
}

void CommandKick(int argc, const char * const *argv, uint8_t sender)
{
    if (argc < 2) return;
    const char *name = argv[1];
    if (!name || !*name) return;

    struct CommandTargetList targets = CommandTarget(name, sender);
    for (int i = 0; i < targets.count; ++i)
        KickPlayer(targets.ids[i], false, KICK_CONSOLE, 0, NULL);
}

void CommandKicklast(int argc, const char * const *argv, uint8_t sender)
{
    (void)argc; (void)argv; (void)sender;
    if (LastPlayer > 0 && LastPlayer <= MAX_SPRITES && Sprite[LastPlayer].Active)
        KickPlayer(LastPlayer, false, KICK_CONSOLE, 0, NULL);
}

void CommandBan(int argc, const char * const *argv, uint8_t sender)
{
    if (argc < 2) return;
    const char *name = argv[1];
    if (!name || !*name) return;

    const char *adminName = (sender == 255) ? "an admin" : Sprite[sender].Player.Name;

    struct CommandTargetList targets = CommandTarget(name, sender);
    for (int i = 0; i < targets.count; ++i)
    {
        char reason[256];
        snprintf(reason, sizeof(reason), "Banned by %s", adminName);
        KickPlayer(targets.ids[i], true, KICK_CONSOLE, DAY * 30, reason);
    }
}

void CommandBanIPHW(int argc, const char * const *argv, uint8_t sender)
{
    if (argc < 2) return;
    const char *value = argv[1];
    if (!value || !*value) return;

    const char *adminName = (sender == 0 || sender == 255) ? "an admin" : Sprite[sender].Player.Name;
    const char *cmd = argv[0];

    if (strcmp(cmd, "banhw") == 0)
    {
        char reason[256];
        snprintf(reason, sizeof(reason), "Banned by %s", adminName);
        AddBannedHW(value, reason, DAY * 30);
        char msg[256];
        snprintf(msg, sizeof(msg), "HWID %s banned", value);
        MainConsole_Console(msg, CLIENT_MESSAGE_COLOR, sender);
    }
    else
    {
        char reason[256];
        snprintf(reason, sizeof(reason), "Banned by %s", adminName);
        AddBannedIP(value, reason, DAY * 30);
        char msg[256];
        snprintf(msg, sizeof(msg), "IP number %s banned", value);
        MainConsole_Console(msg, CLIENT_MESSAGE_COLOR, sender);
    }
    SaveTxtLists();
}

void CommandUnban(int argc, const char * const *argv, uint8_t sender)
{
    if (argc < 2) return;
    const char *value = argv[1];
    if (!value || !*value) return;

    if (DelBannedIP(value))
    {
        char msg[256];
        snprintf(msg, sizeof(msg), "IP number %s unbanned", value);
        MainConsole_Console(msg, CLIENT_MESSAGE_COLOR, sender);
    }
    if (DelBannedHW(value))
    {
        char msg[256];
        snprintf(msg, sizeof(msg), "HWID %s unbanned", value);
        MainConsole_Console(msg, CLIENT_MESSAGE_COLOR, sender);
    }
    SaveTxtLists();
}

void CommandUnbanlast(int argc, const char * const *argv, uint8_t sender)
{
    (void)argc; (void)argv;
    if (DelBannedIP(LastBan))
    {
        char msg[256];
        snprintf(msg, sizeof(msg), "IP number %s unbanned", LastBan);
        MainConsole_Console(msg, CLIENT_MESSAGE_COLOR, sender);
    }
    if (DelBannedHW(LastBanHW))
    {
        char msg[256];
        snprintf(msg, sizeof(msg), "HWID %s unbanned", LastBanHW);
        MainConsole_Console(msg, CLIENT_MESSAGE_COLOR, sender);
    }
    SaveTxtLists();
}

void CommandAdm(int argc, const char * const *argv, uint8_t sender)
{
    if (argc < 2) return;
    const char *name = argv[1];
    if (!name || !*name) return;

    struct CommandTargetList targets = CommandTarget(name, sender);
    for (int i = 0; i < targets.count; ++i)
    {
        const char *ip = Sprite[targets.ids[i]].Player.IP;
        if (!IsRemoteAdminIP(ip))
        {
            RemoteIPs_Add(ip);
            char msg[256];
            snprintf(msg, sizeof(msg), "IP number %s added to Remote Admins", ip);
            MainConsole_Console(msg, CLIENT_MESSAGE_COLOR, sender);
            SaveTxtLists();
        }
    }
}

void CommandAdmip(int argc, const char * const *argv, uint8_t sender)
{
    if (argc < 2) return;
    const char *ip = argv[1];
    if (!ip || !*ip) return;

    if (!IsRemoteAdminIP(ip))
    {
        RemoteIPs_Add(ip);
        char msg[256];
        snprintf(msg, sizeof(msg), "IP number %s added to Remote Admins", ip);
        MainConsole_Console(msg, CLIENT_MESSAGE_COLOR, sender);
        SaveTxtLists();
    }
}

void CommandUnadm(int argc, const char * const *argv, uint8_t sender)
{
    if (argc < 2) return;
    const char *ip = argv[1];
    if (!ip || !*ip) return;

    if (IsRemoteAdminIP(ip))
    {
        int idx = RemoteIPs_IndexOf(ip);
        if (idx >= 0)
        {
            RemoteIPs_Delete(ip);
            char msg[256];
            snprintf(msg, sizeof(msg), "IP number %s removed from Remote Admins", ip);
            MainConsole_Console(msg, CLIENT_MESSAGE_COLOR, sender);
            SaveTxtLists();
        }
    }
}

void CommandSetteam(int argc, const char * const *argv, uint8_t sender)
{
    if (argc < 2) return;
    const char *name = argv[1];
    if (!name || !*name) return;

    const char *cmd = argv[0];
    int teamSet = 1;
    if (cmd && strlen(cmd) >= 8)
        teamSet = StrToIntDef(&cmd[7], 1);   // 8th character (index 7)

    struct CommandTargetList targets = CommandTarget(name, sender);
    for (int i = 0; i < targets.count; ++i)
        Sprite[targets.ids[i]].ChangeTeam(teamSet, true);
}

void CommandSay(int argc, const char * const *argv, uint8_t sender)
{
    if (argc < 2) return;

    size_t total = 0;
    for (int i = 1; i < argc; ++i) total += strlen(argv[i]) + 1;
    char *msg = malloc(total + 1);
    if (!msg) return;
    msg[0] = '\0';
    for (int i = 1; i < argc; ++i)
    {
        strcat(msg, argv[i]);
        strcat(msg, " ");
    }

    if (strlen(msg) == 0) { free(msg); return; }

    /* Convert to wide string for the network layer */
    size_t wlen = mbstowcs(NULL, msg, 0);
    wchar_t *wmsg = malloc((wlen + 1) * sizeof(wchar_t));
    if (wmsg) {
        mbstowcs(wmsg, msg, wlen + 1);
        ServerSendStringMessage(wmsg, ALL_PLAYERS, 255, MSGTYPE_PUB);
        free(wmsg);
    }
    free(msg);
}

void CommandKill(int argc, const char * const *argv, uint8_t sender)
{
    if (argc < 2) return;
    const char *name = argv[1];
    if (!name || !*name) return;

    struct Vector2 a = {0,0};
    struct CommandTargetList targets = CommandTarget(name, sender);
    for (int i = 0; i < targets.count; ++i)
    {
        int id = targets.ids[i];
        Sprite[id].Vest = 0;
        Sprite[id].HealthHit(3430, id, 1, -1, a);
        char msg[256];
        snprintf(msg, sizeof(msg), "%s killed by admin", Sprite[id].Player.Name);
        MainConsole_Console(msg, CLIENT_MESSAGE_COLOR, sender);
    }
}

void CommandLoadwep(int argc, const char * const *argv, uint8_t sender)
{
    const char *name;
    if (argc < 2)
    {
        name = sv_realisticmode.Value ? "weapons_realistic" : "weapons";
    }
    else
        name = argv[1];

    if (LastWepMod) free(LastWepMod);
    LastWepMod = strdup(name);
    LoadWeapons(name);

    for (int i = 1; i <= MAX_PLAYERS; ++i)
        if (Sprite[i].Active && Sprite[i].Player.ControlMethod == HUMAN)
            ServerVars(i);
}

void CommandLoadcon(int argc, const char * const *argv, uint8_t sender)
{
    const char *configFile = (argc < 2) ? "server.cfg" : argv[1];

    if (sv_lockedmode.Value)
    {
        MainConsole_Console("Locked Mode is enabled. Settings can't be changed mid-game.", SERVER_MESSAGE_COLOR, sender);
        return;
    }

    MapChangeCounter = -60;
    ServerDisconnect();

    for (int i = 1; i <= MAX_BULLETS; ++i) Bullet[i].Kill();
    for (int i = 1; i <= MAX_THINGS; ++i) Thing[i].Kill();

    for (int i = 1; i <= MAX_PLAYERS; ++i)
    {
        if (Sprite[i].Active)
        {
            Sprite[i].Player.Team = FixTeam(Sprite[i].Player.Team);
            Sprite[i].Respawn();
            Sprite[i].Player.Kills = 0;
            Sprite[i].Player.Deaths = 0;
            Sprite[i].Player.Flags = 0;
            Sprite[i].Player.TKWarnings = 0;
            Sprite[i].Player.ChatWarnings = 0;
            Sprite[i].Player.KnifeWarnings = 0;
            Sprite[i].Player.ScoresPerSecond = 0;
            Sprite[i].Player.GrabsPerSecond = 0;
        }
    }

    LoadConfig(configFile);
    char msg[256];
    snprintf(msg, sizeof(msg), "Config reloaded %s", configFile);
    MainConsole_Console(msg, CLIENT_MESSAGE_COLOR, sender);
    StartServer();
}

void CommandLoadlist(int argc, const char * const *argv, uint8_t sender)
{
    (void)sender;
    if (argc > 1)
        sv_maplist.SetValue(argv[1]);
    else
        sv_maplist.SetValue(sv_maplist.Value);
}

void CommandPm(int argc, const char * const *argv, uint8_t sender)
{
    if (argc < 3) return;
    const char *targetName = argv[1];
    const char *pmMsg = argv[2];

    struct CommandTargetList targets = CommandTarget(targetName, sender);
    for (int i = 0; i < targets.count; ++i)
    {
        int id = targets.ids[i];
        const char *toName = IDToName(id);
        char consoleMsg[512];
        snprintf(consoleMsg, sizeof(consoleMsg), "Private Message sent to %s", toName);
        MainConsole_Console(consoleMsg, SERVER_MESSAGE_COLOR, sender);

        snprintf(consoleMsg, sizeof(consoleMsg), "(PM) To: %s From: %s Message: %s", toName, IDToName(sender), pmMsg);
        MainConsole_Console(consoleMsg, SERVER_MESSAGE_COLOR, 0);

        size_t wlen = mbstowcs(NULL, pmMsg, 0);
        wchar_t *wmsg = malloc((wlen + 4) * sizeof(wchar_t)); // "(PM) "
        if (wmsg) {
            wcscpy(wmsg, L"(PM) ");
            mbstowcs(wmsg + 4, pmMsg, wlen + 1);
            ServerSendStringMessage(wmsg, id, 255, MSGTYPE_PUB);
            free(wmsg);
        }
    }
}

void CommandGmute(int argc, const char * const *argv, uint8_t sender)
{
    if (argc < 2) return;
    const char *name = argv[1];
    if (!name || !*name) return;

    struct CommandTargetList targets = CommandTarget(name, sender);
    for (int i = 0; i < targets.count; ++i)
    {
        int id = targets[i];
        Sprite[id].Player.Muted = 1;
        for (int j = 1; j <= MAX_PLAYERS; ++j)
        {
            if (MuteList[j] && !*MuteList[j])
            {
                MuteList[j] = strdup(Sprite[id].Player.IP);
                mutename[j] = strdup(Sprite[id].Player.Name);
                break;
            }
        }
        char msg[256];
        snprintf(msg, sizeof(msg), "%s has been muted.", Sprite[id].Player.Name);
        MainConsole_Console(msg, CLIENT_MESSAGE_COLOR, sender);
    }
}

void CommandUngmute(int argc, const char * const *argv, uint8_t sender)
{
    if (argc < 2) return;
    const char *name = argv[1];
    if (!name || !*name) return;

    struct CommandTargetList targets = CommandTarget(name, sender);
    for (int i = 0; i < targets.count; ++i)
    {
        int id = targets[i];
        Sprite[id].Player.Muted = 0;
        for (int j = 1; j <= MAX_PLAYERS; ++j)
        {
            if (MuteList[j] && strcmp(MuteList[j], Sprite[id].Player.IP) == 0)
            {
                free(MuteList[j]); MuteList[j] = NULL;
                free(mutename[j]); mutename[j] = NULL;
                break;
            }
        }
        char msg[256];
        snprintf(msg, sizeof(msg), "%s has been unmuted.", Sprite[id].Player.Name);
        MainConsole_Console(msg, CLIENT_MESSAGE_COLOR, sender);
    }
}

void CommandAddmap(int argc, const char * const *argv, uint8_t sender)
{
    if (argc < 2) return;
    const char *name = argv[1];
    if (!name || !*name) return;

    // In the original code map existence check was commented out.
    MapsList_Add(name);
    char msg[256];
    snprintf(msg, sizeof(msg), "%s has been added to the map list.", name);
    MainConsole_Console(msg, SERVER_MESSAGE_COLOR, sender);
    SaveMapList();
}

void CommandDelmap(int argc, const char * const *argv, uint8_t sender)
{
    if (argc < 2) return;
    const char *name = argv[1];
    if (!name || !*name) return;

    for (int i = 0; i < MapsList_Count(); ++i)
    {
        const char *existing = MapsList_Get(i);
        if (strcasecmp(existing, name) == 0)
        {
            char msg[256];
            snprintf(msg, sizeof(msg), "%s has been removed from the map list.", name);
            MainConsole_Console(msg, SERVER_MESSAGE_COLOR, sender);
            MapsList_Delete(i);
            break;
        }
    }
    SaveMapList();
}

void CommandTempban(int argc, const char * const *argv, uint8_t sender)
{
    if (argc < 3) return;
    const char *minutesStr = argv[1];
    const char *ip = argv[2];
    if (!minutesStr || !*minutesStr || !ip || !*ip) return;

    int minutes = StrToIntDef(minutesStr, 1);
    char reason[256];
    snprintf(reason, sizeof(reason), "Temporary Ban by an Admin");
    AddBannedIP(ip, reason, minutes * MINUTE);
    char msg[256];
    snprintf(msg, sizeof(msg), "IP number %s banned for %d minutes.", ip, minutes);
    MainConsole_Console(msg, CLIENT_MESSAGE_COLOR, sender);
    SaveTxtLists();
}

void CommandWeaponon(int argc, const char * const *argv, uint8_t sender)
{
    if (argc < 2) return;
    const char *weaponStr = argv[1];
    if (!weaponStr || !*weaponStr) return;
    int w = StrToIntDef(weaponStr, -1);
    if (w < 0 || w >= 15) return;

    for (int i = 1; i <= MAX_PLAYERS; ++i)
        if (Sprite[i].Active && Sprite[i].Player.ControlMethod == HUMAN)
            SetWeaponActive(i, w, true);
}

void CommandWeaponoff(int argc, const char * const *argv, uint8_t sender)
{
    if (argc < 2) return;
    const char *weaponStr = argv[1];
    if (!weaponStr || !*weaponStr) return;
    int w = StrToIntDef(weaponStr, -1);
    if (w < 0 || w >= 15) return;

    for (int i = 1; i <= MAX_PLAYERS; ++i)
        if (Sprite[i].Active && Sprite[i].Player.ControlMethod == HUMAN)
            SetWeaponActive(i, w, false);
}

void CommandBanlist(int argc, const char * const *argv, uint8_t sender)
{
    (void)argc; (void)argv;
    MainConsole_Console("HWID                | Duration | Reason", SERVER_MESSAGE_COLOR, sender);
    for (int i = 1; i < (int)ARRAY_SIZE(BannedHWList); ++i)
    {
        if (!BannedHWList[i].HW) continue;
        char dur[32];
        if (BannedHWList[i].Time == PERMANENT)
            strcpy(dur, "PERMANENT");
        else
        {
            time_t now = time(NULL);
            time_t exp = now + (BannedHWList[i].Time / 60);
            struct tm *tm = localtime(&exp);
            strftime(dur, sizeof(dur), "%dd%Hh%Mm", tm);
        }
        char line[256];
        snprintf(line, sizeof(line), "%-15s | %-9s | %s", BannedHWList[i].HW, dur, BannedHWList[i].Reason);
        MainConsole_Console(line, SERVER_MESSAGE_COLOR, sender);
    }

    MainConsole_Console("IP                  | Duration | Reason", SERVER_MESSAGE_COLOR, sender);
    for (int i = 1; i < (int)ARRAY_SIZE(BannedIPList); ++i)
    {
        if (!BannedIPList[i].IP) continue;
        char dur[32];
        if (BannedIPList[i].Time == PERMANENT)
            strcpy(dur, "PERMANENT");
        else
        {
            time_t now = time(NULL);
            time_t exp = now + (BannedIPList[i].Time / 60);
            struct tm *tm = localtime(&exp);
            strftime(dur, sizeof(dur), "%dd%Hh%Mm", tm);
        }
        char line[256];
        snprintf(line, sizeof(line), "%-15s | %-9s | %s", BannedIPList[i].IP, dur, BannedIPList[i].Reason);
        MainConsole_Console(line, SERVER_MESSAGE_COLOR, sender);
    }
}

void CommandNetStats(int argc, const char * const *argv, uint8_t sender)
{
    (void)argc; (void)argv; (void)sender;
    char stats[2048];
    if (UDP.NetworkingSocket.GetDetailedConnectionStatus(1, stats, sizeof(stats)) == 0)
        printf("%s\n", stats);

    for (int i = 1; i <= MAX_PLAYERS; ++i)
    {
        if (!Sprite[i].Active) continue;
        if (UDP.NetworkingSocket.GetDetailedConnectionStatus(Sprite[i].Player.Peer, stats, sizeof(stats)) == 0)
        {
            printf("----------------\n");
            printf("%s\n", Sprite[i].Player.Name);
            printf("%d\n", Sprite[i].Player.Peer);
            printf("%s\n", stats);
            printf("----------------\n");
        }
    }
}

/* Player‑only commands (tabac, smoke, …) */
void CommandPlayerCommand(int argc, const char * const *argv, uint8_t sender)
{
    (void)argc;
    struct Vector2 a = {0,0};
    const char *cmd = argv[0];

    if (strcmp(cmd, "tabac") == 0)
    {
        Sprite[sender].IdleRandom = 0;
        Sprite[sender].IdleTime = 1;
    }
    else if (strcmp(cmd, "smoke") == 0)
    {
        Sprite[sender].IdleRandom = 1;
        Sprite[sender].IdleTime = 1;
    }
    else if (strcmp(cmd, "takeoff") == 0)
    {
        Sprite[sender].IdleRandom = 4;
        Sprite[sender].IdleTime = 1;
    }
    else if (strcmp(cmd, "victory") == 0)
    {
        Sprite[sender].IdleRandom = 5;
        Sprite[sender].IdleTime = 1;
    }
    else if (strcmp(cmd, "piss") == 0)
    {
        Sprite[sender].IdleRandom = 6;
        Sprite[sender].IdleTime = 1;
    }
    else if (strcmp(cmd, "mercy") == 0)
    {
        Sprite[sender].IdleRandom = 7;
        Sprite[sender].IdleTime = 1;
        if (Sprite[sender].Player.Kills > 0)
            Sprite[sender].Player.Kills--;
    }
    else if (strcmp(cmd, "pwn") == 0)
    {
        Sprite[sender].IdleRandom = 8;
        Sprite[sender].IdleTime = 1;
    }
    else if (strcmp(cmd, "kill") == 0)
    {
        Sprite[sender].Vest = 0;
        Sprite[sender].HealthHit(150, sender, 1, -1, a);
        if (Sprite[sender].Player.Kills > 0)
            Sprite[sender].Player.Kills--;
    }
    else if (strcmp(cmd, "brutalkill") == 0)
    {
        Sprite[sender].Vest = 0;
        Sprite[sender].HealthHit(3423, sender, 1, -1, a);
        if (Sprite[sender].Player.Kills > 0)
            Sprite[sender].Player.Kills--;
    }
}

void CommandInfo(int argc, const char * const *argv, uint8_t sender)
{
    (void)argc; (void)argv;
    const wchar_t *greeting;
    if (sv_greeting.Value && *sv_greeting.Value) {
        wchar_t wbuf[256];
        mbstowcs(wbuf, sv_greeting.Value, 255);
        ServerSendStringMessage(wbuf, sender, 255, MSGTYPE_PUB);
    }
    if (sv_greeting2.Value && *sv_greeting2.Value) {
        wchar_t wbuf[256];
        mbstowcs(wbuf, sv_greeting2.Value, 255);
        ServerSendStringMessage(wbuf, sender, 255, MSGTYPE_PUB);
    }
    if (sv_greeting3.Value && *sv_greeting3.Value) {
        wchar_t wbuf[256];
        mbstowcs(wbuf, sv_greeting3.Value, 255);
        ServerSendStringMessage(wbuf, sender, 255, MSGTYPE_PUB);
    }

    wchar_t wbuf[512];
    swprintf(wbuf, 511, L"Server: %hs", sv_hostname.Value);
    ServerSendStringMessage(wbuf, sender, 255, MSGTYPE_PUB);
    swprintf(wbuf, 511, L"Address: %hs:%d", ServerIP, ServerPort);
    ServerSendStringMessage(wbuf, sender, 255, MSGTYPE_PUB);
    swprintf(wbuf, 511, L"Version: %hs", DEDVERSION);
    ServerSendStringMessage(wbuf, sender, 255, MSGTYPE_PUB);

    const char *gametype = "";
    switch (sv_gamemode.Value)
    {
        case GAMESTYLE_DEATHMATCH: gametype = "Deathmatch"; break;
        case GAMESTYLE_POINTMATCH: gametype = "Pointmatch"; break;
        case GAMESTYLE_TEAMMATCH:  gametype = "Teammatch"; break;
        case GAMESTYLE_CTF:        gametype = "Capture the Flag"; break;
        case GAMESTYLE_RAMBO:      gametype = "Rambomatch"; break;
        case GAMESTYLE_INF:        gametype = "Infiltration"; break;
        case GAMESTYLE_HTF:        gametype = "Hold the Flag"; break;
    }
    swprintf(wbuf, 511, L"Gamemode: %hs", gametype);
    ServerSendStringMessage(wbuf, sender, 255, MSGTYPE_PUB);
    swprintf(wbuf, 511, L"Timelimit: %d", sv_timelimit.Value / 3600);
    ServerSendStringMessage(wbuf, sender, 255, MSGTYPE_PUB);
    const char *next = CheckNextMap();
    swprintf(wbuf, 511, L"Nextmap: %hs", next);
    ServerSendStringMessage(wbuf, sender, 255, MSGTYPE_PUB);

#ifdef SCRIPT
    swprintf(wbuf, 511, L"Scripting: %hs", sc_enable.Value ? "Enabled" : "Disabled");
    ServerSendStringMessage(wbuf, sender, 255, MSGTYPE_PUB);
    if (sc_enable.Value)
    {
        struct StringList *list = ScrptDispatcher_ScriptList();
        char scripts[1024] = "";
        for (int i = 0; i < list->Count; ++i)
        {
            if (i) strcat(scripts, ", ");
            strcat(scripts, list->Strings[i]);
        }
        swprintf(wbuf, 511, L"Scripts: %hs", scripts);
        ServerSendStringMessage(wbuf, sender, 255, MSGTYPE_PUB);
        StringList_Free(list);
    }
#endif

    if (LoadedWMChecksum != DefaultWMChecksum)
    {
        swprintf(wbuf, 511, L"Server uses weapon mod \"%hs v%hs\" (checksum %u)", WMName, WMVersion, LoadedWMChecksum);
        ServerSendStringMessage(wbuf, sender, 255, MSGTYPE_PUB);
    }
}

void CommandAdminlog(int argc, const char * const *argv, uint8_t sender)
{
    if (argc < 2) return;
    const char *pwd = argv[1];
    if (sv_adminpassword.Value && *sv_adminpassword.Value && strcmp(pwd, sv_adminpassword.Value) == 0)
    {
        if (!IsAdminIP(Sprite[sender].Player.IP))
            AdminIPs_Add(Sprite[sender].Player.IP);
        char msg[256];
        snprintf(msg, sizeof(msg), "%s added to Game Admins", Sprite[sender].Player.Name);
        MainConsole_Console(msg, SERVER_MESSAGE_COLOR, sender);
    }
    else
    {
        char msg[256];
        snprintf(msg, sizeof(msg), "%s tried to login as Game Admin with bad password", Sprite[sender].Player.Name);
        MainConsole_Console(msg, SERVER_MESSAGE_COLOR, sender);
    }
}

void CommandVotemap(int argc, const char * const *argv, uint8_t sender)
{
    if (argc < 2) return;
    const char *mapName = argv[1];

    if (VoteActive)
    {
        if (strcmp(VoteTarget, mapName) != 0) return;
        if (VoteHasVoted[sender]) return;

#ifdef SCRIPT
        ScrptDispatcher_OnVoteMap(sender, mapName);
#endif
        CountVote(sender);
    }
    else
    {
        if (MapsList_IndexOf(mapName) != -1)
        {
            if (VoteCooldown[sender] < 0)
            {
#ifdef SCRIPT
                if (ScrptDispatcher_OnVoteMapStart(sender, mapName)) return;
#endif
                StartVote(sender, VOTE_MAP, mapName, "---");
                ServerSendVoteOn(VoteType, sender, mapName, "---");
            }
            else
            {
                ServerSendStringMessage(L"Can't vote for 2:00 minutes after joining game or last vote", sender, 255, MSGTYPE_PUB);
            }
        }
        else
        {
            wchar_t wbuf[256];
            swprintf(wbuf, 255, L"Map not found (%hs)", mapName);
            ServerSendStringMessage(wbuf, sender, 255, MSGTYPE_PUB);
        }
    }
}

#ifdef SCRIPT
void CommandRecompile(int argc, const char * const *argv, uint8_t sender)
{
    if (!sc_enable.Value)
    {
        MainConsole_Console("Scripting is currently disabled.", CLIENT_MESSAGE_COLOR, sender);
        return;
    }
    if (argc < 2)
    {
        ScrptDispatcher_Prepare();
        ScrptDispatcher_Launch(NULL);
    }
    else
    {
        ScrptDispatcher_Launch(argv[1]);
    }
}
#endif

void CommandRecord(int argc, const char * const *argv, uint8_t sender)
{
    (void)sender;
    const char *name;
    if (argc == 2)
        name = argv[1];
    else
    {
        time_t now = time(NULL);
        struct tm *tm = localtime(&now);
        static char buf[128];
        strftime(buf, sizeof(buf), "%Y-%m-%d_%H-%M-%S_", tm);
        char *full = malloc(strlen(buf) + strlen(Map.Name) + 5);
        sprintf(full, "%s%s.sdm", buf, Map.Name);
        name = full;
    }
    DemoRecorder_StopRecord();
    char path[512];
    snprintf(path, sizeof(path), "%s/demos/%s", UserDirectory, name);
    DemoRecorder_StartRecord(path);
    if (argc != 2) free((void*)name);
}

void CommandStop(int argc, const char * const *argv, uint8_t sender)
{
    (void)argc; (void)argv; (void)sender;
    DemoRecorder_StopRecord();
}

void CommandListPlayers(int argc, const char * const *argv, uint8_t sender)
{
    (void)argc; (void)argv;
    MainConsole_Console("[ TYPE] Player name              (K/D) [Team]", SERVER_MESSAGE_COLOR, sender);
    MainConsole_Console("---------------------------------------------", SERVER_MESSAGE_COLOR, sender);
    for (int i = 1; i <= MAX_PLAYERS; ++i)
    {
        int pnum = SortedPlayers[i].PlayerNum;
        if (pnum <= 0) continue;
        const char *type;
        if (Sprite[pnum].Player.ControlMethod == HUMAN)
            type = (Sprite[pnum].Player.Team == 5) ? "SPECT" : "HUMAN";
        else
            type = " BOT ";

        char line[256];
        snprintf(line, sizeof(line), "[%-5s] %-24s (%d/%d) [%d]",
                 type,
                 Sprite[pnum].Player.Name,
                 Sprite[pnum].Player.Kills,
                 Sprite[pnum].Player.Deaths,
                 Sprite[pnum].Player.Team);
        MainConsole_Console(line, SERVER_MESSAGE_COLOR, sender);
    }
}

/* ----------------------------------------------------------------------
 * Initialisation – registers all commands with the command system
 * ---------------------------------------------------------------------- */
void InitServerCommands(void)
{
    CommandAdd("addbot",     CommandAddbot,     "Add specific bot to game",            CMD_ADMINONLY | CMD_DEFERRED);
    CommandAdd("addbot1",    CommandAddbot,     "Add specific bot to alpha team",      CMD_ADMINONLY | CMD_DEFERRED);
    CommandAdd("addbot2",    CommandAddbot,     "Add specific bot to blue team",       CMD_ADMINONLY | CMD_DEFERRED);
    CommandAdd("addbot3",    CommandAddbot,     "Add specific bot to charlie team",    CMD_ADMINONLY | CMD_DEFERRED);
    CommandAdd("addbot4",    CommandAddbot,     "Add specific bot to delta team",      CMD_ADMINONLY | CMD_DEFERRED);
    CommandAdd("addbot5",    CommandAddbot,     "Add specific bot to spectators",      CMD_ADMINONLY | CMD_DEFERRED);
    CommandAdd("nextmap",    CommandNextmap,    "Change map to next in maplist",       CMD_ADMINONLY | CMD_DEFERRED | CMD_INGAMEONLY);
    CommandAdd("map",        CommandMap,        "Change map to specified mapname",     CMD_ADMINONLY | CMD_DEFERRED | CMD_INGAMEONLY);
    CommandAdd("pause",      CommandPause,      "Pause game",                          CMD_ADMINONLY);
    CommandAdd("unpause",    CommandUnpause,    "Unpause game",                        CMD_ADMINONLY | CMD_INGAMEONLY);
    CommandAdd("restart",    CommandRestart,    "Restart current map",                 CMD_ADMINONLY | CMD_INGAMEONLY);
    CommandAdd("kick",       CommandKick,       "Kick player with specified nick/id", CMD_ADMINONLY | CMD_INGAMEONLY);
    CommandAdd("kicklast",   CommandKicklast,   "Kick last connected player",          CMD_ADMINONLY | CMD_INGAMEONLY);
    CommandAdd("ban",        CommandBan,        "Ban player with specified nick/id",   CMD_ADMINONLY | CMD_INGAMEONLY);
    CommandAdd("banip",      CommandBanIPHW,    "Ban specified IP address",            CMD_ADMINONLY | CMD_INGAMEONLY);
    CommandAdd("banhw",      CommandBanIPHW,    "Ban specified hwid",                  CMD_ADMINONLY | CMD_INGAMEONLY);
    CommandAdd("unban",      CommandUnban,      "Unban specified ip or hwid",          CMD_ADMINONLY | CMD_INGAMEONLY);
    CommandAdd("unbanlast",  CommandUnbanlast,  "Unban last player",                   CMD_ADMINONLY | CMD_INGAMEONLY);
    CommandAdd("adm",        CommandAdm,        "Give admin to specified nick/id",     CMD_ADMINONLY | CMD_INGAMEONLY);
    CommandAdd("admip",      CommandAdmip,      "Add IP to Remote Admins list",        CMD_ADMINONLY | CMD_INGAMEONLY);
    CommandAdd("unadm",      CommandUnadm,      "Remove IP from Remote Admins list",   CMD_ADMINONLY | CMD_INGAMEONLY);
    CommandAdd("setteam1",   CommandSetteam,    "Move specified id/nick to alpha team",   CMD_ADMINONLY);
    CommandAdd("setteam2",   CommandSetteam,    "Move specified id/nick to bravo team",   CMD_ADMINONLY);
    CommandAdd("setteam3",   CommandSetteam,    "Move specified id/nick to charlie team", CMD_ADMINONLY);
    CommandAdd("setteam4",   CommandSetteam,    "Move specified id/nick to delta team",   CMD_ADMINONLY);
    CommandAdd("setteam5",   CommandSetteam,    "Move specified id/nick to spectators",   CMD_ADMINONLY);
    CommandAdd("say",        CommandSay,        "Send chat message",                   CMD_ADMINONLY | CMD_INGAMEONLY);
    CommandAdd("pkill",      CommandKill,       "Kill specified id/nick",              CMD_ADMINONLY | CMD_INGAMEONLY);
    CommandAdd("loadwep",    CommandLoadwep,    "Load weapons config",                 CMD_ADMINONLY | CMD_INGAMEONLY);
    CommandAdd("loadcon",    CommandLoadcon,    "Load server config",                  CMD_ADMINONLY | CMD_INGAMEONLY);
    CommandAdd("loadlist",   CommandLoadlist,   "Load maplist",                        CMD_ADMINONLY);
    CommandAdd("pm",         CommandPm,         "Send private message to other player",CMD_ADMINONLY | CMD_INGAMEONLY);
    CommandAdd("gmute",      CommandGmute,      "Mute player on server",               CMD_ADMINONLY);
    CommandAdd("ungmute",    CommandUngmute,    "Unmute player on server",             CMD_ADMINONLY);
    CommandAdd("addmap",     CommandAddmap,     "Add map to the maplist",              CMD_ADMINONLY);
    CommandAdd("delmap",     CommandDelmap,     "Remove map from the maplist",         CMD_ADMINONLY);
    CommandAdd("weaponon",   CommandWeaponon,   "Enable specific weapon on the server",CMD_ADMINONLY);
    CommandAdd("weaponoff",  CommandWeaponoff,  "Disable specific weapon on the server",CMD_ADMINONLY);
    CommandAdd("banlist",    CommandBanlist,    "Show banlist",                        CMD_ADMINONLY);
    CommandAdd("net_stats",  CommandNetStats,   "Show network stats",                  CMD_ADMINONLY);

    CommandAdd("tabac",       CommandPlayerCommand, "tabac",       CMD_PLAYERONLY);
    CommandAdd("smoke",       CommandPlayerCommand, "smoke",       CMD_PLAYERONLY);
    CommandAdd("takeoff",     CommandPlayerCommand, "takeoff",     CMD_PLAYERONLY);
    CommandAdd("victory",     CommandPlayerCommand, "victory",     CMD_PLAYERONLY);
    CommandAdd("piss",        CommandPlayerCommand, "piss",        CMD_PLAYERONLY);
    CommandAdd("mercy",       CommandPlayerCommand, "mercy",       CMD_PLAYERONLY);
    CommandAdd("pwn",         CommandPlayerCommand, "pwn",         CMD_PLAYERONLY);
    CommandAdd("kill",        CommandPlayerCommand, "kill",        CMD_PLAYERONLY);
    CommandAdd("brutalkill",  CommandPlayerCommand, "brutalkill",  CMD_PLAYERONLY);
    CommandAdd("info",        CommandInfo,          "info",        CMD_PLAYERONLY);
    CommandAdd("adminlog",    CommandAdminlog,      "adminlog",    CMD_PLAYERONLY);
    CommandAdd("votemap",     CommandVotemap,       "votemap",     CMD_PLAYERONLY);
    CommandAdd("record",      CommandRecord,        "record demo", CMD_ADMINONLY);
    CommandAdd("stop",        CommandStop,          "stop recording demo", CMD_ADMINONLY);
    CommandAdd("listplayers", CommandListPlayers,   "list all current players", CMD_ADMINONLY);

#ifdef SCRIPT
    CommandAdd("recompile",   CommandRecompile,     "Recompile all or specific script", CMD_ADMINONLY);
#endif
}