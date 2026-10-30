#include "Server.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <sys/stat.h>
#include <physfs.h>
#include "shared/Util.h"
#include "shared/Vector.h"
#include "shared/Constants.h"
#include "shared/LogFile.h"
#include "shared/Version.h"
#include "shared/Anims.h"
#include "shared/BanSystem.h"
#include "shared/Command.h"
#include "shared/Console.h"
#include "shared/Cvar.h"
#include "shared/FileServer.h"
#include "shared/Game.h"
#include "shared/LobbyClient.h"
#include "shared/Net.h"
#include "shared/NetworkServerConnection.h"
#include "shared/NetworkServerGame.h"
#include "shared/NetworkServerSprite.h"
#include "shared/NetworkUtils.h"
#include "shared/PolyMap.h"
#ifdef RCON
#include "shared/Rcon.h"
#endif
#ifdef SCRIPT
#include "shared/ScriptDispatcher.h"
#endif
#include "shared/ServerCommands.h"
#include "shared/ServerHelper.h"
#include "shared/ServerLauncherIPC.h"
#include "shared/SharedConfig.h"
#include "shared/Sprites.h"
#include "shared/Things.h"
#include "shared/Weapons.h"
#include "shared/TraceLog.h"

/* ------------------------------------------------------------------ */
/* Global variable definitions                                         */
/* ------------------------------------------------------------------ */
bool ProgReady = false;
char BaseDirectory[PATH_MAX] = {0};
char UserDirectory[PATH_MAX] = {0};
uint64_t MainThreadID = 0;

char ServerIP[16] = "127.0.0.1";
int ServerPort = 23073;
int BonusFreq = 3600;
uint8_t WeaponActive[17] = {0};          /* index 0 corresponds to -1 */
TStringList *MapsList = NULL;
uint8_t LastPlayer = 0;
TMsg_BulletSnapshot OldBulletSnapshotMsg[MAX_SPRITES + 1];
char MuteList[MAX_PLAYERS + 1][MAX_NAME_LENGTH];
char MuteName[MAX_PLAYERS + 1][MAX_NAME_LENGTH];
char TKList[MAX_PLAYERS + 1][MAX_IP_LENGTH];
uint8_t TKListKills[MAX_PLAYERS + 1];
int64_t TCPBytesSent = 0;
int64_t TCPBytesReceived = 0;
TConsole MainConsole;
TStringList *RemoteIPs = NULL;
TStringList *AdminIPs = NULL;
char FloodIP[MAX_FLOODIPS + 1][MAX_IP_LENGTH];
int FloodNum[MAX_FLOODIPS + 1];
char LastReqIP[4][MAX_IP_LENGTH];
uint8_t LastReqID = 0;
char DropIP[MAX_IP_LENGTH] = {0};
int WaveRespawnTime = 0;
int WaveRespawnCounter = 0;
int WeaponsInGame = 0;
uint8_t BulletWarningCount[MAX_SPRITES + 1];
uint8_t CheatTag[MAX_SPRITES + 1];
#ifdef RCON
TAdminServer *AdminServer = NULL;
#endif
float ShotDistance = 0.0f;
float ShotLife = 0.0f;
int ShotRicochet = 0;
int HTFTime = HTF_SEC_POINT;
char WMName[MAX_NAME_LENGTH] = {0};
char WMVersion[MAX_NAME_LENGTH] = {0};
char LastWepMod[MAX_NAME_LENGTH] = {0};
char ModDir[MAX_PATH_LENGTH] = {0};
TServerNetwork *UDP = NULL;
TServerLauncherIPC *LauncherIPC = NULL;
TLobbyThread *LobbyThread = NULL;
#ifdef STEAM
TSteamGS *SteamAPI = NULL;
#endif

/* ------------------------------------------------------------------ */
/* Helper functions                                                    */
/* ------------------------------------------------------------------ */
static void WriteLnCenter(const char *s)
{
    int len = (int)strlen(s);
    int pad = (80 - len) / 2;
    if (pad < 0) pad = 0;
    printf("%*s%s\n", pad, "", s);
}

/* ------------------------------------------------------------------ */
/* ActivateServer                                                       */
/* ------------------------------------------------------------------ */
void ActivateServer(void)
{
    int i, j;
    char s[PATH_MAX];

    MainThreadID = GetThreadID();

    printf("\n");
    WriteLnCenter("-= OpenSoldat Dedicated Server " OPENSOLDAT_VERSION " - " DEDVERSION
                  " (build " OPENSOLDAT_VERSION_LONG ") =-");
#ifdef FPC
    WriteLnCenter("Compiled with FreePascal " FPC_VERSION);
#endif
    printf("\n");
    WriteLnCenter("OpenSoldat Dedicated Server initializing...");
    printf("\n");
    printf("   Need help running your server?\n");
    printf("    Discord: https://discord.gg/6TqqtVpRdV\n");
    printf("    Forum:   https://forums.soldat.pl/\n");
    printf("\n");
    printf("   Additional parameters:\n");
    printf("    -net_port PORT\n");
    printf("    -sv_maxplayers MAXPLAYERS\n");
    printf("    -sv_password PASSWORD\n");
    printf("\n");
    printf("   Example:\n");
    printf("    ./opensoldatserver -net_port 23073 -sv_maxplayers 16 -sv_password \"mypass\"\n");
    printf("\n");

#ifndef CPUARM
    /* Disable FPU exceptions – not needed on most platforms */
#endif

    srand((unsigned)time(NULL));

    /* Decimal separator for parsing config files */
    setlocale(LC_NUMERIC, "C");

    ServerTickCounter = 0;
    MainTickCounter = 0;

    /* Initialise dummy player used for empty sprite slots */
    DummyPlayer = Player_Create();
    for (i = 1; i <= MAX_SPRITES; ++i)
        Sprite[i].Player = DummyPlayer;

    /* Console defaults */
    MainConsole.CountMax = 7;
    MainConsole.ScrollTickMax = 150;
    MainConsole.NewMessageWait = 150;
    MainConsole.AlphaCount = 255;

    if (getenv("COLORTERM") != NULL)
        MainConsole.TerminalColors = true;

    Cvar_Init();
    InitServerCommands();
    ParseCommandLine();

    /* UserDirectory */
    snprintf(UserDirectory, PATH_MAX, "%s", ExtractFilePath(GetProgramPath()));
    if (fs_userpath.Value && fs_userpath.Value[0]) {
        char expanded[PATH_MAX];
        ExpandFileName(fs_userpath.Value, expanded);
        if (DirectoryExists(expanded)) {
            snprintf(UserDirectory, PATH_MAX, "%s%s", expanded,
                     (expanded[strlen(expanded)-1] == '/') ? "" : "/");
        } else {
            Debug("[FS] Warning: Specified fs_userpath directory '%s' does not exist.",
                  fs_userpath.Value);
        }
    }

    /* BaseDirectory */
    snprintf(BaseDirectory, PATH_MAX, "%s", ExtractFilePath(GetProgramPath()));
    if (fs_basepath.Value && fs_basepath.Value[0]) {
        char expanded[PATH_MAX];
        ExpandFileName(fs_basepath.Value, expanded);
        if (DirectoryExists(expanded)) {
            snprintf(BaseDirectory, PATH_MAX, "%s%s", expanded,
                     (expanded[strlen(expanded)-1] == '/') ? "" : "/");
        } else {
            Debug("[FS] Warning: Specified fs_basepath directory '%s' does not exist.",
                  fs_basepath.Value);
        }
    }

    Debug("[FS] UserDirectory: %s", UserDirectory);
    Debug("[FS] BaseDirectory: %s", BaseDirectory);

    chdir(UserDirectory);

    if (!PHYSFS_init(GetProgramPath())) {
        printf("Could not initialize PhysFS.\n");
        ProgReady = false;
        Cvar_SetValueBool(&sc_enable, false);
        return;
    }
    char baseArchive[PATH_MAX];
    snprintf(baseArchive, PATH_MAX, "%s/soldat.smod", BaseDirectory);
    if (!PHYSFS_mount(baseArchive, "/", false)) {
        printf("Could not load base game archive (soldat.smod).\n");
        ProgReady = false;
        Cvar_SetValueBool(&sc_enable, false);
        return;
    }

    GameModChecksum = Sha1File(baseArchive, 4096);

    /* Create basic directories */
    CreateDirIfMissing(UserDirectory + "/configs");
    CreateDirIfMissing(UserDirectory + "/demos");
    CreateDirIfMissing(UserDirectory + "/logs");
    CreateDirIfMissing(UserDirectory + "/logs/kills");
    CreateDirIfMissing(UserDirectory + "/maps");
    CreateDirIfMissing(UserDirectory + "/mods");
#ifdef SCRIPT
    CreateDirIfMissing(UserDirectory + "/scripts");
#endif

    /* Copy default configs from archive */
    PHYSFS_CopyFileFromArchive("configs/server.cfg", UserDirectory + "/configs/server.cfg");
    PHYSFS_CopyFileFromArchive("configs/weapons.ini", UserDirectory + "/configs/weapons.ini");
    PHYSFS_CopyFileFromArchive("configs/weapons_realistic.ini",
                               UserDirectory + "/configs/weapons_realistic.ini");
    PHYSFS_CopyFileFromArchive("configs/mapslist.txt", UserDirectory + "/configs/mapslist.txt");
    PHYSFS_CopyFileFromArchive("configs/remote.txt", UserDirectory + "/configs/remote.txt");
#ifdef SCRIPT
    PHYSFS_CopyFileFromArchive("scripts/README.txt", UserDirectory + "/scripts/README.txt");
#endif

    /* Bots directory */
    if (!DirectoryExists(UserDirectory + "/configs/bots")) {
        if (!CreateDir(UserDirectory + "/configs/bots"))
            printf("Could not create bots directory.\n");
        else if (!PHYSFS_CopyFilesFromArchiveDirectory("configs/bots",
                               UserDirectory + "/configs/bots"))
            printf("Could not copy bots from mod archive.\n");
    }

    LoadConfig("server.cfg");
    CvarsInitialized = true;

    if (launcher_ipc_enable.Value) {
        LauncherIPC = ServerLauncherIPC_Create();
        ServerLauncherIPC_Connect(LauncherIPC, launcher_ipc_port.Value);
    }

    ModDir[0] = '\0';
    if (fs_mod.Value && fs_mod.Value[0]) {
        char modArchive[PATH_MAX];
        snprintf(modArchive, PATH_MAX, "%smods/%s.smod", UserDirectory, fs_mod.Value);
        char mountPoint[PATH_MAX];
        snprintf(mountPoint, PATH_MAX, "mods/%s/", fs_mod.Value);
        if (!PHYSFS_mount(modArchive, mountPoint, false)) {
            printf("Could not load mod archive (%s).\n", fs_mod.Value);
            ProgReady = false;
            Cvar_SetValueBool(&sc_enable, false);
            return;
        }
        snprintf(ModDir, MAX_PATH_LENGTH, "%s", mountPoint);
        CustomModChecksum = Sha1File(modArchive, 4096);
    }

    NewLogFiles();

    Debug("ActivateServer");

#ifdef SCRIPT
    ScriptDispatcher_SetSafeMode(sc_safemode.Value);
#endif

#ifdef STEAM
    SteamAPI = SteamGS_Init(0,
                            net_port.Value,
                            net_port.Value + 20,
                            eServerModeAuthenticationAndSecure,
                            OPENSOLDAT_VERSION);
    SteamAPI_ManualDispatch_Init();

    if (!SteamAPI) {
        printf("[Steam] Failed to initialize Steam instance.\n");
        ShutDown();
    } else {
        SteamAPI_Utils_SetWarningMessageHook(SteamWarning);
        SteamAPI_GameServer_SetModDir(SteamAPI, "Soldat");
        SteamAPI_GameServer_SetProduct(SteamAPI, "Soldat");
        SteamAPI_GameServer_SetGameDescription(SteamAPI, "Soldat");

        if (sv_setsteamaccount.Value && sv_setsteamaccount.Value[0])
            SteamAPI_GameServer_LogOn(SteamAPI, sv_setsteamaccount.Value);
        else
            SteamAPI_GameServer_LogOnAnonymous(SteamAPI);

        if (SteamAPI_UGC_BInitWorkshopForGameServer(SteamAPI, 638490,
                (UserDirectory + "/workshop")))
            printf("[Steam] Initialized Workshop.\n");
        else
            printf("[Steam] Failed to initialize Workshop.\n");
    }
#endif

    ProgReady = true;

#ifndef SCRIPT
    Cvar_ParseAndSetValue(&sc_enable, "0");
#endif

    /* Default all weapons active */
    for (i = 1; i <= MAIN_WEAPONS; ++i)
        WeaponActive[i] = 1;

    LoadAnimObjects("");
    if (ModDir[0])
        LoadAnimObjects(ModDir);

    printf(" Hit CTRL+C to Exit\n");
    printf(" Please command the server using the Soldat Admin program\n");

    MapChangeCounter = -60;
    SinusCounter = 0;

    AddLineToLogFile(GameLog, "Loading Maps List", ConsoleLogFileName);
    MapsList = StringList_Create();
    LoadMapsList("");

    for (i = 1; i <= MAX_SPRITES; ++i)
        for (j = 1; j <= MAX_SPRITES; ++j)
            OldHelmetMsg[i][j].WearHelmet = 1;

    if (!CreateFileIfMissing(UserDirectory + "/configs/banned.txt"))
        FatalError("Failed to create configs/banned.txt");
    if (!CreateFileIfMissing(UserDirectory + "/configs/bannedhw.txt"))
        FatalError("Failed to create configs/bannedhw.txt");

    LoadBannedList(UserDirectory + "/configs/banned.txt");
    LoadBannedListHW(UserDirectory + "/configs/bannedhw.txt");

    RemoteIPs = StringList_Create();
    if (FileExists(UserDirectory + "/configs/remote.txt"))
        StringList_LoadFromFile(RemoteIPs, UserDirectory + "/configs/remote.txt");

    AdminIPs = StringList_Create();
    StringList_Assign(AdminIPs, RemoteIPs);
    StringList_Add(AdminIPs, "127.0.0.1");

    for (i = 0; i <= MAX_FLOODIPS; ++i) {
        FloodIP[i][0] = ' ';
        FloodIP[i][1] = '\0';
        FloodNum[i] = 0;
    }

    WeaponsInGame = 0;
    for (j = 1; j <= MAIN_WEAPONS; ++j)
        if (WeaponActive[j] == 1)
            ++WeaponsInGame;

#ifdef RCON
    if (sv_adminpassword.Value && sv_adminpassword.Value[0])
        AdminServer = AdminServer_Create(sv_adminpassword.Value);
    else {
        printf("\n The server must be started with an Admin Password parameter"
               " to run Admin\n   edit server.cfg and set sv_adminpassword variable\n\n");
    }
#endif

    printf(" Server name: %s\n", sv_hostname.Value);
    UpdateGameStats();
    WriteLogFile(KillLog, KillLogFileName);
    WriteLogFile(GameLog, ConsoleLogFileName);

    RunDeferredCommands();
}

/* ------------------------------------------------------------------ */
/* ShutDown                                                            */
/* ------------------------------------------------------------------ */
void ShutDown(void)
{
    Debug("ShutDown");
    ProgReady = false;

    Console_Print(&MainConsole, "Shutting down server...", GAME_MESSAGE_COLOR);
    remove((UserDirectory + "/logs/" + sv_pidfilename.Value).c_str());

    if (UDP) {
        ServerDisconnect();
        Console_Print(&MainConsole, "Shutting down game networking.", GAME_MESSAGE_COLOR);
        ServerNetwork_Destroy(UDP);
        UDP = NULL;
    }

#ifdef RCON
    if (sv_adminpassword.Value && sv_adminpassword.Value[0]) {
        Console_Print(&MainConsole, "Shutting down admin server...", GAME_MESSAGE_COLOR);
        AdminServer_Destroy(AdminServer);
        AdminServer = NULL;
    }
#endif

    StopFileServer();

    if (launcher_ipc_enable.Value) {
        ServerLauncherIPC_Destroy(LauncherIPC);
        LauncherIPC = NULL;
    }

    StringList_Destroy(MapsList);
    MapsList = NULL;
    StringList_Destroy(RemoteIPs);
    RemoteIPs = NULL;
    StringList_Destroy(AdminIPs);
    AdminIPs = NULL;

#ifdef SCRIPT
    ScriptDispatcher_Destroy();
#endif

#ifdef STEAM
    Debug("[Steam] Shutdown");
    SteamGameServer_Shutdown();
#endif

#ifndef STEAM
    GameNetworkingSockets_Kill();
#endif

    Player_Destroy(DummyPlayer);
    DummyPlayer = NULL;

    AddLineToLogFile(GameLog, "PhysFS closing.", ConsoleLogFileName);
    PHYSFS_deinit();

    AddLineToLogFile(GameLog, "   End of Log.", ConsoleLogFileName);
    Debug("Updating gamestats");
    UpdateGameStats();
    Debug("Saving killlog");
    WriteLogFile(KillLog, KillLogFileName);
    Debug("Saving gamelog");
    WriteLogFile(GameLog, ConsoleLogFileName);
    Debug("Freeing gamelog");
    LogFile_Destroy(GameLog);
    GameLog = NULL;
    Debug("Freeing killlog");
    LogFile_Destroy(KillLog);
    KillLog = NULL;

    Command_Cleanup();
    Cvar_Cleanup();
}

/* ------------------------------------------------------------------ */
/* LoadMapsList                                                        */
/* ------------------------------------------------------------------ */
bool LoadMapsList(const char *Filename)
{
    int i;
    char MapsListPath[PATH_MAX];

    if (!Filename || Filename[0] == '\0')
        Filename = sv_maplist.Value;
    if (strstr(Filename, ".txt") == NULL) {
        snprintf(MapsListPath, PATH_MAX, "%s.txt", Filename);
        Filename = MapsListPath;
    }

    snprintf(MapsListPath, PATH_MAX, "%s/configs/%s", UserDirectory, Filename);

    if (FileExists(MapsListPath)) {
        StringList_LoadFromFile(MapsList, MapsListPath);

        i = 0;
        while (i < StringList_Count(MapsList)) {
            if (StringList_Get(MapsList, i)[0] == '\0') {
                StringList_Delete(MapsList, i);
                continue;
            }
            ++i;
        }

        Console_Print(&MainConsole, "Loaded maps list: ", CLIENT_MESSAGE_COLOR);
        Console_Print(&MainConsole, Filename, CLIENT_MESSAGE_COLOR);
        return true;
    } else {
        if (strcmp(Filename, sv_maplist.DefaultValue) != 0) {
            return Cvar_SetValueString(&sv_maplist, sv_maplist.DefaultValue);
        } else {
            Console_Print(&MainConsole,
                          "Maps list file not found: configs/", WARNING_MESSAGE_COLOR);
            Console_Print(&MainConsole, Filename, WARNING_MESSAGE_COLOR);
            return false;
        }
    }

    if (StringList_Count(MapsList) == 0) {
        Console_Print(&MainConsole,
                      "Current maps list is empty, adding default map", WARNING_MESSAGE_COLOR);
        if (!IsTeamGame())
            StringList_Add(MapsList, "Arena");
        else
            StringList_Add(MapsList, "ctf_Ash");
    }
    return true;
}

/* ------------------------------------------------------------------ */
/* LoadWeapons                                                         */
/* ------------------------------------------------------------------ */
void LoadWeapons(const char *filename)
{
    bool IsRealistic = sv_realisticmode.Value;
    Debug("LoadWeapons");

    CreateWeapons(IsRealistic);
    DefaultWMChecksum = CreateWMChecksum();

    char path[PATH_MAX];
    snprintf(path, PATH_MAX, "%s/configs/%s.ini", UserDirectory, filename);
    if (!LoadWeaponsConfig(path)) {
        printf("Using default weapons mod\n");
        CreateWeapons(IsRealistic);
    }

    LoadedWMChecksum = CreateWMChecksum();

    if (LoadedWMChecksum != DefaultWMChecksum) {
        Console_Print(&MainConsole,
                      "Loaded weapons mod \"", SERVER_MESSAGE_COLOR);
        Console_Print(&MainConsole, WMName, SERVER_MESSAGE_COLOR);
        Console_Print(&MainConsole, " v", SERVER_MESSAGE_COLOR);
        Console_Print(&MainConsole, WMVersion, SERVER_MESSAGE_COLOR);
        Console_Print(&MainConsole, "\"", SERVER_MESSAGE_COLOR);
    }

    for (int i = 1; i <= MAX_PLAYERS; ++i) {
        if (Sprite[i].Active)
            Sprite_ApplyWeaponByNum(&Sprite[i], Sprite[i].Weapon.Num,
                                    1, Sprite[i].Weapon.AmmoCount);
    }
}

/* ------------------------------------------------------------------ */
/* AddBotPlayer                                                        */
/* ------------------------------------------------------------------ */
uint8_t AddBotPlayer(const char *Name, int team)
{
    TVector2 a = {0}, b = {0};
    int p;
    TPlayer *NewPlayer;

    Debug("AddBotPlayer");

    if (PlayersNum == MAX_PLAYERS) {
        Console_Print(&MainConsole,
                      "Bot cannot be added because server is full",
                      WARNING_MESSAGE_COLOR);
        return 0;
    }

    NewPlayer = Player_Create();
    NewPlayer->Team = team;
    Player_ApplyShirtColorFromTeam(NewPlayer);

    RandomizeStart(&a, team);
    p = CreateSprite(a, b, 1, 255, NewPlayer, true);

    if (!LoadBotConfig(UserDirectory + "/configs/bots/" + Name + ".bot",
                       &Sprite[p])) {
        Console_Print(&MainConsole, "Bot file ", WARNING_MESSAGE_COLOR);
        Console_Print(&MainConsole, Name, WARNING_MESSAGE_COLOR);
        Console_Print(&MainConsole, " not found", WARNING_MESSAGE_COLOR);
        Sprite_Kill(&Sprite[p]);
        return 0;
    }

    Sprite_Respawn(&Sprite[p]);
    Sprite[p].Player.ControlMethod = BOT;
    Sprite[p].Player.ChatWarnings = 0;
    Sprite[p].Player.GrabsPerSecond = 0;

    ServerSendNewPlayerInfo(p, JOIN_NORMAL);

    const char *TempStr;
    switch (team) {
        case 0: TempStr = "the game"; break;
        case 1: TempStr = "alpha team"; break;
        case 2: TempStr = "bravo team"; break;
        case 3: TempStr = "charlie team"; break;
        case 4: TempStr = "delta team"; break;
        case 5: TempStr = "as spectator"; break;
        default: TempStr = "the game";
    }
    Console_Print(&MainConsole,
                  Sprite[p].Player.Name, ENTER_MESSAGE_COLOR);
    Console_Print(&MainConsole, " has joined ", ENTER_MESSAGE_COLOR);
    Console_Print(&MainConsole, TempStr, ENTER_MESSAGE_COLOR);
    Console_Print(&MainConsole, ".", ENTER_MESSAGE_COLOR);

#ifdef SCRIPT
    ScriptDispatcher_OnJoinTeam(p, Sprite[p].Player.Team,
                                Sprite[p].Player.Team, true);
    ScriptDispatcher_OnWeaponChange(p,
                                    Sprite[p].Weapon.Num,
                                    Sprite[p].SecondaryWeapon.Num,
                                    Sprite[p].Weapon.AmmoCount,
                                    Sprite[p].SecondaryWeapon.AmmoCount);
#endif

    SortPlayers();
    return (uint8_t)p;
}

/* ------------------------------------------------------------------ */
/* StartServer                                                         */
/* ------------------------------------------------------------------ */
void StartServer(void)
{
    TVector2 a = {0};
    int i, k, j;
    TMapInfo StartMap;

    Debug("StartServer");

    AddLineToLogFile(GameLog, "Starting Game Server.", ConsoleLogFileName);

    if (!UDP)
        UDP = ServerNetwork_Create(net_ip.Value, net_port.Value);

    if (UDP && UDP->Active) {
        char addrStr[64];
        ServerNetwork_GetStringAddress(UDP, &UDP->Address, true, addrStr, sizeof(addrStr));
        printf("[NET] Game networking initialized.\n");
        printf("[NET] Server is listening on %s\n", addrStr);
    } else {
        printf("[NET] Failed to bind to %s:%d\n", net_ip.Value, net_port.Value);
        ProgReady = false;
        return;
    }

    ServerPort = UDP->Port;

    if (fileserver_enable.Value)
        StartFileServer();

    for (i = 1; i <= MAX_SPRITES; ++i) {
        NoClientUpdatetime[i] = 0;
        Time_SpriteSnapshot[i] = 0;
        Time_SpriteSnapshot_Mov[i] = 0;
    }

    for (i = 1; i <= 4; ++i)
        TeamScore[i] = 0;

    AddLineToLogFile(GameLog, "Loading Map.", ConsoleLogFileName);

    if (net_lan.Value == LAN) {
        Cvar_SetValueBool(&sv_guns_collide, true);
        Cvar_SetValueBool(&sv_kits_collide, true);
    }
    if (net_lan.Value == INTERNET) {
        Cvar_SetValueBool(&sv_guns_collide, false);
        Cvar_SetValueBool(&sv_kits_collide, false);
    }

    MapIndex = 0;

    if (GetMapInfo(StringList_Get(MapsList, MapIndex), UserDirectory, &StartMap)) {
        if (!Map_LoadMap(&StartMap)) {
            Console_Print(&MainConsole, "Error: Could not load map ",
                          DEBUG_MESSAGE_COLOR);
            Console_Print(&MainConsole, StartMap.Name, DEBUG_MESSAGE_COLOR);
            ProgReady = false;
            return;
        }
    } else {
        ProgReady = false;
        return;
    }

    MapCheckSum = GetMapChecksum(&StartMap);

    AddLineToLogFile(GameLog, "Creating Weapons.", ConsoleLogFileName);

    if (sv_realisticmode.Value) {
        Console_Print(&MainConsole, "Realistic Mode ON", MODE_MESSAGE_COLOR);
        StartHealth = REALISTIC_HEALTH;
        LoadWeapons("weapons_realistic");
        snprintf(LastWepMod, MAX_NAME_LENGTH, "weapons_realistic");
    } else {
        StartHealth = DEFAULT_HEALTH;
        LoadWeapons("weapons");
        snprintf(LastWepMod, MAX_NAME_LENGTH, "weapons");
    }

    WeaponsInGame = 0;
    for (i = 1; i <= 14; ++i)
        if (WeaponActive[i] == 1)
            ++WeaponsInGame;

    for (j = 1; j <= MAX_SPRITES; ++j)
        for (i = 1; i <= 10; ++i)
            WeaponSel[j][i] = WeaponActive[i];

    if (sv_advancemode.Value) {
        for (j = 1; j <= MAX_SPRITES; ++j)
            for (i = 1; i <= 10; ++i)
                WeaponSel[j][i] = 1;
        Console_Print(&MainConsole, "Advance Mode ON", MODE_MESSAGE_COLOR);
    }

    SortPlayers();

    MapChangeCounter = -60;
    LastPlayer = 0;
    TimeLimitCounter = sv_timelimit.Value;
    UpdateWaveRespawnTime();
    WaveRespawnCounter = WaveRespawnTime;

#ifdef ENABLE_FAE
    if (ac_enable.Value)
        printf("[AC] Anti-Cheat enabled\n");
#endif

    if (sv_lobby.Value && !LobbyThread)
        LobbyThread = LobbyThread_Create();

    switch (sv_gamemode.Value) {
        case GAMESTYLE_DEATHMATCH:   Cvar_SetValueInt(&sv_killlimit, sv_dm_limit.Value); break;
        case GAMESTYLE_POINTMATCH:   Cvar_SetValueInt(&sv_killlimit, sv_pm_limit.Value); break;
        case GAMESTYLE_RAMBO:        Cvar_SetValueInt(&sv_killlimit, sv_rm_limit.Value); break;
        case GAMESTYLE_TEAMMATCH:    Cvar_SetValueInt(&sv_killlimit, sv_tm_limit.Value); break;
        case GAMESTYLE_CTF:          Cvar_SetValueInt(&sv_killlimit, sv_ctf_limit.Value); break;
        case GAMESTYLE_INF:          Cvar_SetValueInt(&sv_killlimit, sv_inf_limit.Value); break;
        case GAMESTYLE_HTF:          Cvar_SetValueInt(&sv_killlimit, sv_htf_limit.Value); break;
    }

#ifdef SCRIPT
    if (sc_enable.Value)
        ScriptDispatcher_Launch();
#endif

#ifdef SCRIPT
    ScriptDispatcher_OnAfterMapChange(Map.Name);
#endif

    if (sv_gamemode.Value == GAMESTYLE_POINTMATCH) {
        RandomizeStart(&a, 14);
        TeamFlag[1] = CreateThing(a, 255, OBJECT_POINTMATCH_FLAG, 255);
    }

    if (sv_gamemode.Value == GAMESTYLE_HTF) {
        RandomizeStart(&a, 14);
        TeamFlag[1] = CreateThing(a, 255, OBJECT_POINTMATCH_FLAG, 255);
    }

    if (sv_gamemode.Value == GAMESTYLE_CTF) {
        if (RandomizeStart(&a, 5))
            TeamFlag[1] = CreateThing(a, 255, OBJECT_ALPHA_FLAG, 255);
        if (RandomizeStart(&a, 6))
            TeamFlag[2] = CreateThing(a, 255, OBJECT_BRAVO_FLAG, 255);
    }

    if (sv_gamemode.Value == GAMESTYLE_RAMBO) {
        RandomizeStart(&a, 15);
        CreateThing(a, 255, OBJECT_RAMBO_BOW, 255);
    }

    if (sv_gamemode.Value == GAMESTYLE_INF) {
        if (RandomizeStart(&a, 5))
            TeamFlag[1] = CreateThing(a, 255, OBJECT_ALPHA_FLAG, 255);
        if (RandomizeStart(&a, 6))
            TeamFlag[2] = CreateThing(a, 255, OBJECT_BRAVO_FLAG, 255);
    }

    if (!sv_survivalmode.Value) {
        SpawnThings(OBJECT_MEDICAL_KIT, Map.Medikits);
        if (sv_maxgrenades.Value > 0)
            SpawnThings(OBJECT_GRENADE_KIT, Map.Grenades);
    } else {
        Console_Print(&MainConsole, "Survival Mode ON", MODE_MESSAGE_COLOR);
    }

    if (sv_stationaryguns.Value) {
        for (i = 1; i <= MAX_SPAWNPOINTS; ++i) {
            if (Map.SpawnPoints[i].Active && Map.SpawnPoints[i].Team == 16) {
                a.x = Map.SpawnPoints[i].X;
                a.y = Map.SpawnPoints[i].Y;
                CreateThing(a, 255, OBJECT_STATIONARY_GUN, 255);
            }
        }
    }

    for (k = 1; k <= bots_random_noteam.Value; ++k)
        AddBotPlayer(RandomBot, 0);
    for (k = 1; k <= bots_random_alpha.Value; ++k)
        AddBotPlayer(RandomBot, 1);
    for (k = 1; k <= bots_random_bravo.Value; ++k)
        AddBotPlayer(RandomBot, 2);
    for (k = 1; k <= bots_random_charlie.Value; ++k)
        AddBotPlayer(RandomBot, 3);
    for (k = 1; k <= bots_random_delta.Value; ++k)
        AddBotPlayer(RandomBot, 4);

    UpdateGameStats();
}

/* ------------------------------------------------------------------ */
/* PrepareMapChange                                                    */
/* ------------------------------------------------------------------ */
bool PrepareMapChange(const char *Name)
{
    TMapInfo Status;
    if (GetMapInfo(Name, UserDirectory, &Status)) {
        MapChange = Status;
        TimeLimitCounter = 0;
        MapChangeCounter = MapChangeTime;
        ServerMapChange(ALL_PLAYERS);
        Console_Print(&MainConsole, "Next map: ", GAME_MESSAGE_COLOR);
        Console_Print(&MainConsole, Status.Name, GAME_MESSAGE_COLOR);
#ifdef SCRIPT
        ScriptDispatcher_OnBeforeMapChange(Status.Name);
#endif
        return true;
    }
    return false;
}

/* ------------------------------------------------------------------ */
/* NextMap                                                             */
/* ------------------------------------------------------------------ */
void NextMap(void)
{
    Debug("NextMap");
    if (StringList_Count(MapsList) < 1) {
        Console_Print(&MainConsole,
                      "Can't load maps from mapslist", GAME_MESSAGE_COLOR);
    } else {
        MapIndex = (MapIndex + 1) % StringList_Count(MapsList);
        PrepareMapChange(StringList_Get(MapsList, MapIndex));
    }
}

/* ------------------------------------------------------------------ */
/* SpawnThings                                                         */
/* ------------------------------------------------------------------ */
void SpawnThings(uint8_t style, uint8_t amount)
{
    int i, k, l, team;
    TVector2 a = {0};

    Trace("SpawnThings");

    k = 0;
    switch (style) {
        case OBJECT_MEDICAL_KIT:   k = 8; break;
        case OBJECT_GRENADE_KIT:   k = 7; break;
        case OBJECT_FLAMER_KIT:    k = 11; break;
        case OBJECT_PREDATOR_KIT:  k = 13; break;
        case OBJECT_VEST_KIT:      k = 10; break;
        case OBJECT_BERSERK_KIT:   k = 12; break;
        case OBJECT_CLUSTER_KIT:   k = 9; break;
    }

    for (i = 1; i <= amount; ++i) {
        team = 0;
        if (sv_gamemode.Value == GAMESTYLE_CTF &&
            (style == OBJECT_MEDICAL_KIT || style == OBJECT_GRENADE_KIT)) {
            team = (i % 2 == 0) ? 1 : 2;
        }

        Thing[MAX_THINGS - 1].Team = team;

        bool ok;
        if (team == 0)
            ok = RandomizeStart(&a, k);
        else
            ok = SpawnBoxes(&a, k, MAX_THINGS - 1) || RandomizeStart(&a, k);
        if (!ok) return;

        a.X -= SPAWNRANDOMVELOCITY +
               (rand() % (int)(2 * 100 * SPAWNRANDOMVELOCITY)) / 100.0;
        a.Y -= SPAWNRANDOMVELOCITY +
               (rand() % (int)(2 * 100 * SPAWNRANDOMVELOCITY)) / 100.0;

        l = CreateThing(a, 255, style, 255);
        if (l > 0 && l <= MAX_THINGS)
            Thing[l].Team = team;
    }
}

/* ------------------------------------------------------------------ */
/* KickPlayer                                                          */
/* ------------------------------------------------------------------ */
bool KickPlayer(uint8_t num, bool Ban, int why, int time, const char *Reason)
{
    int i;
    char TimeStr[64] = {0};

    Result = false;
    Debug("KickPlayer");

    if (num > MAX_PLAYERS || num < 1)
        return false;

    i = num;
    if (!Sprite[i].Active)
        return false;

    if (why == KICK_CHEAT && !sv_anticheatkick.Value)
        return false;

    if ((why == KICK_PING || why == KICK_FLOODING || why == KICK_VOTED) &&
        strlen(Sprite[i].Player.IP) > 5) {
        if (IsRemoteAdminIP(Sprite[i].Player.IP) ||
            IsAdminIP(Sprite[i].Player.IP)) {
            Console_Print(&MainConsole,
                          Sprite[i].Player.Name, CLIENT_MESSAGE_COLOR);
            Console_Print(&MainConsole,
                          " is admin and cannot be kicked.", CLIENT_MESSAGE_COLOR);
            return false;
        }
    }

    if (why == KICK_LEFTGAME) {
        const char *msg;
        switch (Sprite[i].Player.Team) {
            case 0: msg = " has left the game."; break;
            case 1: msg = " has left alpha team."; break;
            case 2: msg = " has left bravo team."; break;
            case 3: msg = " has left charlie team."; break;
            case 4: msg = " has left delta team."; break;
            case 5: msg = " has left spectators"; break;
            default: msg = " has left the game.";
        }
        Console_Print(&MainConsole, Sprite[i].Player.Name, ENTER_MESSAGE_COLOR);
        Console_Print(&MainConsole, msg, ENTER_MESSAGE_COLOR);
    }

    if (!Ban && why != KICK_LEFTGAME && why != KICK_SILENT) {
        const char *ipPart = (Sprite[i].Player.ControlMethod == BOT) ? "" : Sprite[i].Player.IP;
        Console_Print(&MainConsole, Sprite[i].Player.Name, CLIENT_MESSAGE_COLOR);
        Console_Print(&MainConsole, " has been kicked.", CLIENT_MESSAGE_COLOR);
        if (ipPart[0]) {
            Console_Print(&MainConsole, "(", CLIENT_MESSAGE_COLOR);
            Console_Print(&MainConsole, ipPart, CLIENT_MESSAGE_COLOR);
            Console_Print(&MainConsole, ")", CLIENT_MESSAGE_COLOR);
        }
    }

    if (Ban) {
        AddBannedIP(Sprite[i].Player.IP, Reason, time);
#ifdef STEAM
        char steamIDStr[32];
        snprintf(steamIDStr, sizeof(steamIDStr), "%llu",
                 (unsigned long long)Sprite[i].Player.SteamID.m_unAccountID);
        AddBannedHW(steamIDStr, Reason, time);
#else
        AddBannedHW(Sprite[i].Player.HWid, Reason, time);
#endif
    }

    if (Ban) {
        if (time > 0) {
            if ((time + 1) / 3600 > 1439)
                snprintf(TimeStr, sizeof(TimeStr), "%d days",
                         (time + 1) / 5184000);
            else
                snprintf(TimeStr, sizeof(TimeStr), "%d minutes",
                         (time + 1) / 3600);
            Console_Print(&MainConsole,
                          Sprite[i].Player.Name, CLIENT_MESSAGE_COLOR);
            Console_Print(&MainConsole,
                          " has been kicked and banned for ", CLIENT_MESSAGE_COLOR);
            Console_Print(&MainConsole, TimeStr, CLIENT_MESSAGE_COLOR);
            Console_Print(&MainConsole, " (", CLIENT_MESSAGE_COLOR);
            Console_Print(&MainConsole, Reason, CLIENT_MESSAGE_COLOR);
            Console_Print(&MainConsole, ")", CLIENT_MESSAGE_COLOR);
        } else {
            Console_Print(&MainConsole,
                          Sprite[i].Player.Name, CLIENT_MESSAGE_COLOR);
            Console_Print(&MainConsole,
                          " has been kicked and permanently banned (",
                          CLIENT_MESSAGE_COLOR);
            Console_Print(&MainConsole, Reason, CLIENT_MESSAGE_COLOR);
            Console_Print(&MainConsole, ")", CLIENT_MESSAGE_COLOR);
        }
    }

    SaveTxtLists();

    if (!Sprite[i].Active)
        return false;

    ServerPlayerDisconnect(&Sprite[i].Player, why);
    return true;
}

/* ------------------------------------------------------------------ */
#ifdef STEAM
#ifdef STEAMSTATS
void RequestUserStats(CSteamID Player)
{
    SteamAPI_GameServerStats_RequestUserStats(SteamAPI, Player);
}

void GSStatsStored(PGSStatsStored_t Callback)
{
    Debug("[Steam] GSStatsReceived result %d result steamid %s",
          Callback->m_eResult, SteamID3(Callback->m_steamIDUser));
}

void StoreStats(CSteamID Player)
{
    // implementation omitted – uses Steam CallResult dispatcher
}
#endif

void RunManualCallbacks(void)
{
    CallbackMsg_t callback;
    SteamAPICallCompleted_t *pCallCompleted;
    bool bFailed;
    void *Data;

    SteamAPI_ManualDispatch_RunFrame(SteamAPI->SteamPipeHandle);
    while (SteamAPI_ManualDispatch_GetNextCallback(SteamAPI->SteamPipeHandle,
                                                   &callback)) {
        if (callback.m_iCallback == CBID_SteamAPICallCompleted_t) {
            pCallCompleted = (SteamAPICallCompleted_t *)callback.m_pubParam;
            Data = malloc(pCallCompleted->m_cubParam);
            SteamAPI_ManualDispatch_GetAPICallResult(
                    SteamAPI->SteamPipeHandle,
                    pCallCompleted->m_hAsyncCall,
                    Data,
                    pCallCompleted->m_cubParam,
                    pCallCompleted->m_iCallback,
                    &bFailed);
            free(Data);
        } else if (callback.m_iCallback == CBID_SteamServersConnected_t) {
            // OnSteamServersConnected()
        } else if (callback.m_iCallback == CBID_SteamServerConnectFailure_t) {
            // OnSteamServerConnectFailure(...)
        } else if (callback.m_iCallback == CBID_SteamServersDisconnected_t) {
            // OnSteamServersDisconnected(...)
        } else if (callback.m_iCallback == CBID_SteamNetConnectionStatusChangedCallback_t) {
            if (UDP)
                ServerNetwork_ProcessEvents(UDP,
                    (PSteamNetConnectionStatusChangedCallback_t)callback.m_pubParam);
        }
        SteamAPI_ManualDispatch_FreeLastCallback(SteamAPI->SteamPipeHandle);
    }
}
#endif

/* ------------------------------------------------------------------ */