#include "ServerLoop.h"

#include "Game.h"
#include "Net.h"
#include "Sprites.h"
#include "Constants.h"
#include "Cvar.h"
#include "LogFile.h"
#include "TraceLog.h"
#include "Vector.h"
#include "Weapons.h"
#include "Things.h"
#include "Parts.h"

#include <SDL.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>

/* External functions referenced from other modules */
extern void UDP_ProcessLoop(void);
extern void AdminServer_ProcessCommands(void);
extern void CheckSynchronize(void);
extern void UpdateIPBanList(void);
extern void UpdateHWBanList(void);
extern void *LobbyThread_Create(void);
extern void ServerSyncCvars(int a, int b, bool c);
extern void ServerSpriteSnapshot(int netw);
extern void ServerSpriteSnapshotMajor(int netw);
extern void ServerSkeletonSnapshot(int netw);
extern void ServerHeartBeat(void);
extern void ServerSpriteDeltas(int j);
extern void ServerPing(int j);
extern void ServerThingSnapshot(int j);
extern void ServerSyncMsg(void);
extern void ServerSendFaeChallenge(void *peer, bool flag);
extern void KickPlayer(int idx, bool a, int reason, int duration, const char *msg);
extern void SpawnThings(int objectType, int count);
extern bool RandomizeStart(TVector2 *out, int style);
extern int CreateThing(TVector2 pos, int owner, int style, int team);
extern void Thing_Kill(TThing *thing);
extern void MainConsole_Console(const char *text, int color);
extern void Trace(const char *msg);
extern void ToggleBulletTime(bool on, int duration);
extern void NextMap(void);
extern void SortPlayers(void);
extern void UpdateGameStats(void);
extern void WriteLogFile(const char *log, const char *filename);
extern long CheckFileSize(const char *filename);
extern void NewLogFiles(void);

/* Global from Pascal unit */
static unsigned long long LastMinuteTick = 0ULL;

/* Forward declarations of globals used */
extern int MainTickCounter;
extern int TickTime;
extern int TickTimeLast;
extern int Ticks;
extern int sv_gamemode;
extern int MapChangeCounter;
extern int BulletTimeTimer;
extern int TimeLimitCounter;
extern int TimeLeftMin;
extern int TimeLeftSec;
extern int PlayersNum;
extern int BotsNum;
extern int SpectatorsNum;
extern int PlayersTeamNum[5];
extern int TeamScore[6];
extern int TeamFlag[5];
extern int WaveRespawnCounter;
extern int WaveRespawnTime;
extern int VoteCooldown[MAX_SPRITES + 1];
extern int FloodNum[MAX_PLAYERS + 1];
extern int PingWarnings[MAX_PLAYERS + 1];
extern int FloodWarnings[MAX_PLAYERS + 1];
extern int MessagesASecNum[MAX_PLAYERS + 1];
extern int NoClientupdateTime[MAX_PLAYERS + 1];
extern int sv_lobby;
extern int sv_pauseonidle;
extern int sv_maxping;
extern int sv_minping;
extern int sv_warnings_ping;
extern int sv_warnings_flood;
extern int net_lan;
extern int net_floodingpacketslan;
extern int net_floodingpacketsinternet;
extern int net_t1_snapshot;
extern int net_t1_majorsnapshot;
extern int net_t1_deadsnapshot;
extern int net_t1_heartbeat;
extern int net_t1_delta;
extern int net_t1_ping;
extern int net_t1_thingsnapshot;
extern int launcher_ipc_enable;
extern int launcher_ipc_port;
extern int launcher_ipc_reconnect_rate;
extern bool sv_survivalmode;
extern bool sv_realisticmode;
extern int sv_bonus_frequency;
extern bool sv_bonus_berserker;
extern bool sv_bonus_flamer;
extern bool sv_bonus_predator;
extern bool sv_bonus_vest;
extern bool sv_bonus_cluster;
extern int sv_gamemode;
extern int sv_antimassflag;
extern int sv_healthcooldown;
extern int log_enable;
extern int log_filesupdate;
extern int demo_autorecord;
extern char *UserDirectory;
extern char *Map_Name; /* Map.Name */
extern struct TDemoRecorder *DemoRecorder;
extern char *LastReqIP[4];
extern char *DropIP;
extern int CheatTag[MAX_SPRITES + 1];
extern int HTFTime;
extern int HTF_SEC_POINT;

/* Helper macros for cvars */
#define CVar_GetInteger(name) CVar_GetInteger(name)

void AppOnIdle(void)
{
    int MainControl;
    int j;
    int HeavySendersNum;
    float Adjust;

    Trace("AppOnIdle");
    Number27Timing();  /* timing calculations */

    /* NET RECEIVE */
    UDP_ProcessLoop();

#if defined(RCON)
    if (/* AdminServer != nil */ false) {
        AdminServer_ProcessCommands();
    }
#endif

    /* Run methods synchronized from threads */
    CheckSynchronize();

    /* Run every 60 seconds */
    unsigned long long now = (unsigned long long)SDL_GetTicks64();
    if ((now - LastMinuteTick) >= 60000ULL) {
        LastMinuteTick = now;
        if (CVar_GetInteger("sv_lobby")) {
            LobbyThread_Create();
        }
        UpdateIPBanList();
        UpdateHWBanList();
    }

    if (CVar_GetInteger("sv_pauseonidle") && ((PlayersNum - BotsNum) <= 0)) {
#if defined(STEAM)
        /* RunManualCallbacks(); */
#endif
#if defined(SCRIPT)
        /* ScrptDispatcher.OnIdle(); */
#endif
        SDL_Delay(100);
        return;
    }

    for (MainControl = 1; MainControl <= (TickTime - TickTimeLast); ++MainControl) {
        /* frame‑rate independent code */
        Ticks++;
        MainTickCounter++;
        if (MainTickCounter == 2147483640) MainTickCounter = 0;

#if defined(SCRIPT)
        /* ScrptDispatcher.OnClockTick(); */
#endif

#if defined(STEAM)
        /* RunManualCallbacks(); */
#endif

        /* Flood Nums Cancel */
        if (MainTickCounter % 1000 == 0) {
            for (j = 0; j <= MAX_PLAYERS; ++j) FloodNum[j] = 0;
        }

        /* Warnings Cancel */
        if (MainTickCounter % (MINUTE * 5) == 0) {
            for (j = 1; j <= MAX_PLAYERS; ++j) {
                if (PingWarnings[j] > 0) PingWarnings[j]--;
                if (FloodWarnings[j] > 0) FloodWarnings[j]--;
            }
        }

        if (MainTickCounter % 1000 == 0) {
            for (j = 1; j <= MAX_SPRITES; ++j) {
                if (Sprite[j].Active)
                    Sprite[j].Player->KnifeWarnings = 0;
            }
        }

        /* sync changed cvars */
        if (/* CvarsNeedSyncing */ false) {
            ServerSyncCvars(0, 0, false);
        }

        /* General game updating */
        UpdateFrame();

        if (MapChangeCounter < 0) {
            if (DemoRecorder && DemoRecorder->Active) {
                /* DemoRecorder.SaveNextFrame(); */
            }
        }

        Trace("AppOnIdle 2");

        /* Network updating */
        if (MainTickCounter % SECOND == 0) {
            if (MapChangeCounter < 0) {
                if (MainTickCounter % (SECOND * 6) == 0) {
                    for (j = 1; j <= MAX_PLAYERS; ++j) {
                        if (Sprite[j].Active &&
                            Sprite[j].Player->ControlMethod == HUMAN &&
                            ((Sprite[j].Player->RealPing > CVar_GetInteger("sv_maxping")) ||
                             ((Sprite[j].Player->RealPing < CVar_GetInteger("sv_minping")) &&
                              (Sprite[j].Player->PingTime > 0)))) {
                            char buf[256];
                            snprintf(buf, sizeof(buf), "%s gets a ping warning",
                                     Sprite[j].Player->Name);
                            MainConsole_Console(buf, WARNING_MESSAGE_COLOR);
                            PingWarnings[j]++;
                            if (PingWarnings[j] > CVar_GetInteger("sv_warnings_ping")) {
                                KickPlayer(j, true, KICK_PING, SIXTY_MINUTES / 4, "Ping Kick");
                            }
                        }
                    }
                }
            }

            /* Player Packet Flooding */
            for (j = 1; j <= MAX_PLAYERS; ++j) {
                if (Sprite[j].Active) {
                    bool cond = false;
                    if (net_lan == LAN) {
                        cond = MessagesASecNum[j] > CVar_GetInteger("net_floodingpacketslan");
                    } else if (net_lan == INTERNET) {
                        cond = MessagesASecNum[j] > CVar_GetInteger("net_floodingpacketsinternet");
                    }
                    if (cond) {
                        char buf[256];
                        snprintf(buf, sizeof(buf), "%s is flooding the server",
                                 Sprite[j].Player->Name);
                        MainConsole_Console(buf, WARNING_MESSAGE_COLOR);
                        FloodWarnings[j]++;
                        if (FloodWarnings[j] > CVar_GetInteger("sv_warnings_flood")) {
                            KickPlayer(j, true, KICK_FLOODING, SIXTY_MINUTES / 4, "Flood Kicked");
                        }
                    }
                }
            }

            for (j = 1; j <= MAX_PLAYERS; ++j) MessagesASecNum[j] = 0;
        }

        if (MainTickCounter % (SECOND * 10) == 0) {
            for (j = 1; j <= MAX_PLAYERS; ++j)
                if (Sprite[j].Active)
                    /* UDP.UpdateNetworkStats(j); */ ;
        }

        /* Packet rate send adjusting */
        HeavySendersNum = PlayersNum - SpectatorsNum;
        if (HeavySendersNum < 5) Adjust = 0.66f;
        else if (HeavySendersNum < 9) Adjust = 0.75f;
        else Adjust = 1.0f;

        /* Send bundled packets */
        if (net_lan == LAN) {
            if (MainTickCounter % (int)roundf(30.0f * Adjust) == 0)
                ServerSpriteSnapshot(NETW);
            if ((MainTickCounter % (int)roundf(15.0f * Adjust) == 0) &&
                (MainTickCounter % (int)roundf(30.0f * Adjust) != 0))
                ServerSpriteSnapshotMajor(NETW);
            if (MainTickCounter % (int)roundf(20.0f * Adjust) == 0)
                ServerSkeletonSnapshot(NETW);
            if (MainTickCounter % (int)roundf(59.0f * Adjust) == 0)
                ServerHeartBeat();
            if ((MainTickCounter % (int)roundf(4.0f * Adjust) == 0) &&
                (MainTickCounter % (int)roundf(30.0f * Adjust) != 0) &&
                (MainTickCounter % (int)roundf(60.0f * Adjust) != 0)) {
                for (j = 1; j <= MAX_SPRITES; ++j)
                    if (Sprite[j].Active && Sprite[j].Player->ControlMethod == BOT)
                        ServerSpriteDeltas(j);
            }
        } else if (net_lan == INTERNET) {
            if (MainTickCounter % (int)roundf(CVar_GetInteger("net_t1_snapshot") * Adjust) == 0)
                ServerSpriteSnapshot(NETW);
            if ((MainTickCounter % (int)roundf(CVar_GetInteger("net_t1_majorsnapshot") * Adjust) == 0) &&
                (MainTickCounter % (int)roundf(CVar_GetInteger("net_t1_snapshot") * Adjust) != 0))
                ServerSpriteSnapshotMajor(NETW);
            if (MainTickCounter % (int)roundf(CVar_GetInteger("net_t1_deadsnapshot") * Adjust) == 0)
                ServerSkeletonSnapshot(NETW);
            if (MainTickCounter % (int)roundf(CVar_GetInteger("net_t1_heartbeat") * Adjust) == 0)
                ServerHeartBeat();
            if ((MainTickCounter % (int)roundf(CVar_GetInteger("net_t1_delta") * Adjust) == 0) &&
                (MainTickCounter % (int)roundf(CVar_GetInteger("net_t1_snapshot") * Adjust) != 0) &&
                (MainTickCounter % (int)roundf(CVar_GetInteger("net_t1_majorsnapshot") * Adjust) != 0)) {
                for (j = 1; j <= MAX_SPRITES; ++j)
                    if (Sprite[j].Active && Sprite[j].Player->ControlMethod == BOT)
                        ServerSpriteDeltas(j);
            }
        }

        for (j = 1; j <= MAX_SPRITES; ++j) {
            if (Sprite[j].Active &&
                Sprite[j].Player->ControlMethod == HUMAN &&
                Sprite[j].Player->Port > 0) {
                /* connection problems */
                if (MapChangeCounter < 0)
                    NoClientupdateTime[j]++;

                if (NoClientupdateTime[j] > DISCONNECTION_TIME) {
                    char buf[256];
                    snprintf(buf, sizeof(buf), "%s could not respond",
                             Sprite[j].Player->Name);
                    MainConsole_Console(buf, WARNING_MESSAGE_COLOR);
                    ServerPlayerDisconnect(Sprite[j].Player, KICK_NORESPONSE, true);
                    continue;
                }
                if (NoClientupdateTime[j] < 0) NoClientupdateTime[j] = 0;

#if defined(ENABLE_FAE)
                if (CVar_GetInteger("ac_enable")) {
                    Sprite[j].Player->FaeTicks++;
                    if (Sprite[j].Player->FaeTicks > (SECOND * 20)) {
                        char buf[256];
                        snprintf(buf, sizeof(buf), "%s no anti-cheat response",
                                 Sprite[j].Player->Name);
                        MainConsole_Console(buf, WARNING_MESSAGE_COLOR);
                        KickPlayer(j, false, KICK_AC, 0, "No Anti-Cheat Response");
                        continue;
                    } else if ((MainTickCounter % (SECOND * 3) == 0) &&
                               !Sprite[j].Player->FaeResponsePending) {
                        ServerSendFaeChallenge(Sprite[j].Player->Peer, false);
                    }
                }
#endif

                if (MainTickCounter % MINUTE == 0)
                    ServerSyncMsg();

                if (net_lan == LAN) {
                    if (MainTickCounter % (int)roundf(21.0f * Adjust) == 0)
                        ServerPing(j);
                    if (MainTickCounter % (int)roundf(12.0f * Adjust) == 0)
                        ServerThingSnapshot(j);
                } else if (net_lan == INTERNET) {
                    if (MainTickCounter % (int)roundf(CVar_GetInteger("net_t1_ping") * Adjust) == 0)
                        ServerPing(j);
                    if (MainTickCounter % (int)roundf(CVar_GetInteger("net_t1_thingsnapshot") * Adjust) == 0)
                        ServerThingSnapshot(j);
                }
            }
        }

        /* Launcher connection */
        if (CVar_GetInteger("launcher_ipc_enable")) {
            if (!/*LauncherIPC.ThreadAlive*/ false &&
                (MainTickCounter % CVar_GetInteger("launcher_ipc_reconnect_rate") == 0)) {
                /* LauncherIPC.Connect(launcher_ipc_port.Value); */
            }
        }

        /* UDP.FlushMsg; // TODO: is this necessary? */
    }
}

/* ------------------------------------------------------------------ */

void UpdateFrame(void)
{
    int i, j;
    TVector2 M = {0.0f, 0.0f};
    long _x;

    Trace("UpdateFrame");

    if (MapChangeCounter < 0) {
        for (j = 1; j <= MAX_SPRITES; ++j)
            if (Sprite[j].Active && !Sprite[j].DeadMeat)
                if (TSprite_IsNotSpectator(&Sprite[j])) {
                    for (i = MAX_OLDPOS; i >= 1; --i)
                        OldSpritePos[j][i] = OldSpritePos[j][i - 1];
                    OldSpritePos[j][0] = SpriteParts.Pos[j];
                }

        for (j = 1; j <= MAX_SPRITES; ++j)
            if (Sprite[j].Active)
                if (TSprite_IsNotSpectator(&Sprite[j]))
                    SpriteParts_DoEulerTimeStepFor(j);   /* integrate sprite particles */

        for (j = 1; j <= MAX_SPRITES; ++j)
            if (Sprite[j].Active)
                TSprite_Update(&Sprite[j]);   /* update sprite */

        /* Bullets update */
        for (j = 1; j <= MAX_BULLETS; ++j)
            if (Bullet[j].Active)
                TBullet_Update(&Bullet[j]);

        BulletParts_DoEulerTimeStep();

        /* update Things */
        for (j = 1; j <= MAX_THINGS; ++j)
            if (Thing[j].Active)
                TThing_Update(&Thing[j]);

        /* Bonuses spawn */
        if (!CVar_GetInteger("sv_survivalmode") && !CVar_GetInteger("sv_realisticmode")) {
            if (CVar_GetInteger("sv_bonus_frequency") > 0) {
                int BonusFreq = 0;
                switch (CVar_GetInteger("sv_bonus_frequency")) {
                    case 1: BonusFreq = 7400; break;
                    case 2: BonusFreq = 4300; break;
                    case 3: BonusFreq = 2500; break;
                    case 4: BonusFreq = 1600; break;
                    case 5: BonusFreq = 800; break;
                }

                if (CVar_GetInteger("sv_bonus_berserker"))
                    if (MainTickCounter % BonusFreq == 0)
                        if (rand() % BERSERKERBONUS_RANDOM == 0)
                            SpawnThings(OBJECT_BERSERK_KIT, 1);

                if (CVar_GetInteger("sv_bonus_flamer"))
                    if (MainTickCounter % 444 == 0)
                        if (rand() % FLAMERBONUS_RANDOM == 0)
                            SpawnThings(OBJECT_FLAMER_KIT, 1);

                if (CVar_GetInteger("sv_bonus_predator"))
                    if (MainTickCounter % BonusFreq == 0)
                        if (rand() % PREDATORBONUS_RANDOM == 0)
                            SpawnThings(OBJECT_PREDATOR_KIT, 1);

                if (CVar_GetInteger("sv_bonus_vest"))
                    if (MainTickCounter % (BonusFreq / 2) == 0)
                        if (rand() % VESTBONUS_RANDOM == 0)
                            SpawnThings(OBJECT_VEST_KIT, 1);

                int j_rand = CLUSTERBONUS_RANDOM;
                if (sv_gamemode == GAMESTYLE_CTF) j_rand = (int)roundf(CLUSTERBONUS_RANDOM * 0.75f);
                if (sv_gamemode == GAMESTYLE_INF)  j_rand = (int)roundf(CLUSTERBONUS_RANDOM * 0.75f);
                if (sv_gamemode == GAMESTYLE_HTF)  j_rand = (int)roundf(CLUSTERBONUS_RANDOM * 0.75f);
                if (CVar_GetInteger("sv_bonus_cluster"))
                    if (MainTickCounter % (BonusFreq / 2) == 0)
                        if (rand() % j_rand == 0)
                            SpawnThings(OBJECT_CLUSTER_KIT, 1);
            }
        }
    }

    /* bullet timer */
    if (BulletTimeTimer > -1) BulletTimeTimer--;

    if (BulletTimeTimer == 0) {
        ToggleBulletTime(false, 0);
        BulletTimeTimer = -1;
    } else if (BulletTimeTimer < 1) {
        /* MapChange counter update */
        if ((MapChangeCounter > -60) && (MapChangeCounter < 99999999))
            MapChangeCounter--;
        if ((MapChangeCounter < 0) && (MapChangeCounter > -59))
            ChangeMap();

        /* Game Stats save */
        if (MainTickCounter % CVar_GetInteger("log_filesupdate") == 0) {
            if (CVar_GetInteger("log_enable")) {
                UpdateGameStats();
                WriteLogFile("KillLog", "KillLogFileName");
                WriteLogFile("GameLog", "ConsoleLogFileName");
                if (CheckFileSize("KillLogFileName") > MAX_LOGFILESIZE ||
                    CheckFileSize("ConsoleLogFileName") > MAX_LOGFILESIZE)
                    NewLogFiles();
            }
        }

        /* Anti‑Hack for Mass‑Flag Cheat */
        if (MainTickCounter % SECOND == 0)
            if (CVar_GetInteger("sv_antimassflag"))
                for (j = 1; j <= MAX_SPRITES; ++j) {
                    if (Sprite[j].Active &&
                        Sprite[j].Player->GrabsPerSecond > 0 &&
                        Sprite[j].Player->ScoresPerSecond > 0 &&
                        Sprite[j].Player->GrabbedInBase) {
                        CheatTag[j] = 1;
#if defined(SCRIPT)
                        if (!/*ScrptDispatcher.OnVoteKickStart*/(255, j, "Server: Possible cheating")) {
#endif
                            StartVote(255, VOTE_KICK, "j", "Server: Possible cheating");
                            ServerSendVoteOn(VoteType, 255, "j", "Server: Possible cheating");
#if defined(SCRIPT)
                        }
#endif
                        char buf[256];
                        snprintf(buf, sizeof(buf), "** Detected possible Mass-Flag cheating from %s",
                                 Sprite[j].Player->Name);
                        MainConsole_Console(buf, WARNING_MESSAGE_COLOR);
                    }
                    if (Sprite[j].Active) {
                        Sprite[j].Player->GrabsPerSecond = 0;
                        Sprite[j].Player->ScoresPerSecond = 0;
                        Sprite[j].Player->GrabbedInBase = false;
                    }
                }

        if (CVar_GetInteger("sv_healthcooldown") > 0)
            if (MainTickCounter % (CVar_GetInteger("sv_healthcooldown") * SECOND) == 0)
                for (i = 1; i <= MAX_SPRITES; ++i)
                    if (Sprite[i].Active && Sprite[i].HasPack)
                        Sprite[i].HasPack = false;

        /* Anti‑Chat Flood */
        if (MainTickCounter % SECOND == 0)
            for (j = 1; j <= MAX_SPRITES; ++j)
                if (Sprite[j].Active) {
                    if (Sprite[j].Player->ChatWarnings > 5)
                        KickPlayer(j, true, KICK_FLOODING, FIVE_MINUTES, "Chat Flood");
                    if (Sprite[j].Player->ChatWarnings > 0)
                        Sprite[j].Player->ChatWarnings--;
                }

        if (MainTickCounter % SECOND == 0)
            if (LastReqIP[0] && LastReqIP[0][0] &&
                strcmp(LastReqIP[0], LastReqIP[1]) == 0 &&
                strcmp(LastReqIP[1], LastReqIP[2]) == 0 &&
                strcmp(LastReqIP[2], LastReqIP[3]) == 0) {
                DropIP = LastReqIP[0];
                char buf[256];
                snprintf(buf, sizeof(buf), "Firewalled IP %s", DropIP);
                MainConsole_Console(buf, 0);
            }

        if (MainTickCounter % (SECOND * 3) == 0)
            for (j = 0; j <= 3; ++j) LastReqIP[j][0] = '\0';

        if (MainTickCounter % (SECOND * 30) == 0)
            DropIP[0] = '\0';

        if (MainTickCounter % MINUTE == 0)
            for (j = 1; j <= MAX_SPRITES; ++j)
                if (Sprite[j].Active)
                    Sprite[j].Player->PlayTime++;

        /* Leftover from old Ban Timers code */
        if (MainTickCounter % (SECOND * 10) == 0)
            if (PlayersNum == 0)
                if (MapChangeCounter > 99999999)
                    MapChangeCounter = -60;

        SinusCounter += ILUMINATESPEED;

        /* Wave respawn count */
        WaveRespawnCounter--;
        if (WaveRespawnCounter < 1) WaveRespawnCounter = WaveRespawnTime;

        for (j = 1; j <= MAX_SPRITES; ++j)
            if (VoteCooldown[j] > -1) VoteCooldown[j]--;

        /* Time Limit decrease */
        if (MapChangeCounter < 99999999)
            if (TimeLimitCounter > 0) TimeLimitCounter--;
        if (TimeLimitCounter == 1) NextMap();

        TimeLeftMin = TimeLimitCounter / MINUTE;
        TimeLeftSec = (TimeLimitCounter - TimeLeftMin * MINUTE) / 60;

        if (TimeLimitCounter > 0) {
            if (TimeLimitCounter < FIVE_MINUTES + 1) {
                if (TimeLimitCounter % MINUTE == 0) {
                    char buf[128];
                    snprintf(buf, sizeof(buf), "Time Left: %d minutes",
                             TimeLimitCounter / MINUTE);
                    MainConsole_Console(buf, GAME_MESSAGE_COLOR);
                }
            } else {
                if (TimeLimitCounter % FIVE_MINUTES == 0) {
                    char buf[128];
                    snprintf(buf, sizeof(buf), "Time Left: %d minutes",
                             TimeLimitCounter / MINUTE);
                    MainConsole_Console(buf, GAME_MESSAGE_COLOR);
                }
            }
        }

        Trace("UpdateFrame 2");

        /* voting timer */
        TimerVote();

        /* Consoles Update */
        MainConsole->ScrollTick++;
        if (MainConsole->ScrollTick == MainConsole->ScrollTickMax)
            MainConsole_ScrollConsole(MainConsole);

        if (MainConsole->AlphaCount > 0) MainConsole->AlphaCount--;

        if (!CVar_GetInteger("sv_advancemode"))
            for (j = 1; j <= MAX_SPRITES; ++j)
                for (i = 1; i <= 10; ++i)
                    WeaponSel[j][i] = 1;
    }   /* bullettime off */

    Trace("UpdateFrame 3");

    /* Infiltration mode blue team score point */
    j = CVar_GetInteger("sv_inf_bluelimit") * SECOND;
    if (PlayersTeamNum[1] < PlayersTeamNum[2])
        j = CVar_GetInteger("sv_inf_bluelimit") * SECOND +
            2 * SECOND * (PlayersTeamNum[2] - PlayersTeamNum[1]);

    if (sv_gamemode == GAMESTYLE_INF)
        if (MapChangeCounter < 0)
            if (TeamFlag[2] > 0 && Thing[TeamFlag[2]].InBase)
                if (PlayersTeamNum[1] > 0 && PlayersTeamNum[2] > 0)
                    if (MainTickCounter % j == 0) {
                        TeamScore[2]++;
                        SortPlayers();
                    }

    /* HTF mode team score point */
    if (PlayersTeamNum[2] == PlayersTeamNum[1])
        HTFTime = CVar_GetInteger("sv_htf_pointstime") * 60;

    if (sv_gamemode == GAMESTYLE_HTF)
        if (MapChangeCounter < 0)
            if (PlayersTeamNum[1] > 0 && PlayersTeamNum[2] > 0)
                if (MainTickCounter % HTFTime == 0) {
                    for (i = 1; i <= MAX_SPRITES; ++i)
                        if (Sprite[i].Active && Sprite[i].HoldedThing > 0)
                            if (Thing[Sprite[i].HoldedThing].Style == OBJECT_POINTMATCH_FLAG) {
                                TeamScore[Sprite[i].Player->Team]++;
                                if (Sprite[i].Player->Team == TEAM_ALPHA)
                                    HTFTime = HTF_SEC_POINT + 2 * SECOND * (PlayersTeamNum[1] - PlayersTeamNum[2]);
                                if (Sprite[i].Player->Team == TEAM_BRAVO)
                                    HTFTime = HTF_SEC_POINT + 2 * SECOND * (PlayersTeamNum[2] - PlayersTeamNum[1]);
                                if (HTFTime < HTF_SEC_POINT) HTFTime = HTF_SEC_POINT;
                                SortPlayers();
                            }
                }

    /* Spawn Rambo bow if nobody has it and not on map */
    if (sv_gamemode == GAMESTYLE_RAMBO)
        if (MainTickCounter % SECOND == 0) {
            _x = 0;
            for (j = 1; j <= MAX_THINGS; ++j)
                if (Thing[j].Active && Thing[j].Style == OBJECT_RAMBO_BOW) _x = 1;
            for (j = 1; j <= MAX_PLAYERS; ++j)
                if (Sprite[j].Active &&
                    (Sprite[j].Weapon.Num == Guns[BOW].Num ||
                     Sprite[j].Weapon.Num == Guns[BOW2].Num))
                    _x = 1;
            if (_x == 0) {
                M = (TVector2){0,0};
                RandomizeStart(&M, 15);
                j = CreateThing(M, 255, OBJECT_RAMBO_BOW, 255);
            }
        }

    /* Destroy flags if > 1 */
    if ((sv_gamemode == GAMESTYLE_CTF) || (sv_gamemode == GAMESTYLE_INF))
        if (MainTickCounter % (SECOND * 2) == 0) {
            _x = 0;
            for (j = 1; j <= MAX_THINGS; ++j)
                if (Thing[j].Active && Thing[j].Style == OBJECT_ALPHA_FLAG) ++_x;
            if (_x > 1)
                for (j = MAX_THINGS; j >= 1; --j)
                    if (Thing[j].Active && Thing[j].Style == OBJECT_ALPHA_FLAG) {
                        Thing_Kill(&Thing[j]);
                        break;
                    }
            if (_x == 0)
                if (RandomizeStart(&M, 5))
                    TeamFlag[1] = CreateThing(M, 255, OBJECT_ALPHA_FLAG, 255);

            _x = 0;
            for (j = 1; j <= MAX_THINGS; ++j)
                if (Thing[j].Active && Thing[j].Style == OBJECT_BRAVO_FLAG) ++_x;
            if (_x > 1)
                for (j = MAX_THINGS; j >= 1; --j)
                    if (Thing[j].Active && Thing[j].Style == OBJECT_BRAVO_FLAG) {
                        Thing_Kill(&Thing[j]);
                        break;
                    }
            if (_x == 0)
                if (RandomizeStart(&M, 6))
                    TeamFlag[2] = CreateThing(M, 255, OBJECT_BRAVO_FLAG, 255);
        }

    if ((sv_gamemode == GAMESTYLE_POINTMATCH) || (sv_gamemode == GAMESTYLE_HTF))
        if (MainTickCounter % (SECOND * 2) == 0) {
            _x = 0;
            for (j = 1; j <= MAX_THINGS; ++j)
                if (Thing[j].Active && Thing[j].Style == OBJECT_POINTMATCH_FLAG) ++_x;
            if (_x > 1)
                for (j = MAX_THINGS; j >= 1; --j)
                    if (Thing[j].Active && Thing[j].Style == OBJECT_POINTMATCH_FLAG) {
                        Thing_Kill(&Thing[j]);
                        break;
                    }
            if (_x == 0)
                if (RandomizeStart(&M, 14))
                    TeamFlag[1] = CreateThing(M, 255, OBJECT_POINTMATCH_FLAG, 255);
        }

    if (CVar_GetInteger("demo_autorecord") && DemoRecorder && !DemoRecorder->Active && Map_Name && Map_Name[0]) {
        char path[512];
        time_t t = time(NULL);
        struct tm *tm = localtime(&t);
        char timestamp[64];
        strftime(timestamp, sizeof(timestamp), "%Y-%m-%d_%H-%M-%S_", tm);
        snprintf(path, sizeof(path), "%sdemos/%s%s.sdm",
                 UserDirectory, timestamp, Map_Name);
        /* DemoRecorder.StartRecord(path); */
    }
}