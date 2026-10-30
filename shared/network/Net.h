/* Net — C port of shared/network/Net.pas */

#ifndef NET_H
#define NET_H

#include <stdint.h>
#include <stdbool.h>

/* Forward declarations of GameNetworkingSockets types */
struct SteamNetworkingIPAddr {
    unsigned short m_port;
    char pad[126];
};
typedef struct SteamNetworkingIPAddr SteamNetworkingIPAddr;

struct SteamNetConnectionRealTimeStatus_t {
    int m_nPing;
    float m_flConnectionQualityLocal;
    char pad[120];
};
typedef struct SteamNetConnectionRealTimeStatus_t SteamNetConnectionRealTimeStatus_t;

/* Binary ops */
#define B1 1
#define B2 2
#define B3 4
#define B4 8
#define B5 16
#define B6 32
#define B7 64
#define B8 128
#define B9 256
#define B10 512
#define B11 1024
#define B12 2048
#define B13 4096
#define B14 8192
#define B15 16384
#define B16 32768

/* MESSAGE IDs */
#define MsgID_Custom 0
#define MsgID_HeartBeat (MsgID_Custom + 2)
#define MsgID_ServerSpriteSnapshot (MsgID_Custom + 3)
#define MsgID_ClientSpriteSnapshot (MsgID_Custom + 4)
#define MsgID_BulletSnapshot (MsgID_Custom + 5)
#define MsgID_ChatMessage (MsgID_Custom + 6)
#define MsgID_ServerSkeletonSnapshot (MsgID_Custom + 7)
#define MsgID_MapChange (MsgID_Custom + 8)
#define MsgID_ServerThingSnapshot (MsgID_Custom + 9)
#define MsgID_ThingTaken (MsgID_Custom + 12)
#define MsgID_SpriteDeath (MsgID_Custom + 13)
#define MsgID_PlayerInfo (MsgID_Custom + 15)
#define MsgID_PlayersList (MsgID_Custom + 16)
#define MsgID_NewPlayer (MsgID_Custom + 17)
#define MsgID_ServerDisconnect (MsgID_Custom + 18)
#define MsgID_PlayerDisconnect (MsgID_Custom + 19)
#define MsgID_Delta_Movement (MsgID_Custom + 21)
#define MsgID_Delta_Weapons (MsgID_Custom + 25)
#define MsgID_Delta_Helmet (MsgID_Custom + 26)
#define MsgID_Delta_MouseAim (MsgID_Custom + 29)
#define MsgID_Ping (MsgID_Custom + 30)
#define MsgID_Pong (MsgID_Custom + 31)
#define MsgID_FlagInfo (MsgID_Custom + 32)
#define MsgID_ServerThingMustSnapshot (MsgID_Custom + 33)
#define MsgID_IdleAnimation (MsgID_Custom + 37)
#define MsgID_ServerSpriteSnapshot_Major (MsgID_Custom + 41)
#define MsgID_ClientSpriteSnapshot_Mov (MsgID_Custom + 42)
#define MsgID_ClientSpriteSnapshot_Dead (MsgID_Custom + 43)
#define MsgID_UnAccepted (MsgID_Custom + 44)
#define MsgID_VoteOn (MsgID_Custom + 45)
#define MsgID_VoteMap (MsgID_Custom + 46)
#define MsgID_VoteMapReply (MsgID_Custom + 47)
#define MsgID_VoteKick (MsgID_Custom + 48)
#define MsgID_RequestThing (MsgID_Custom + 51)
#define MsgID_ServerVars (MsgID_Custom + 52)
#define MsgID_ServerSyncMsg (MsgID_Custom + 54)
#define MsgID_ClientFreeCam (MsgID_Custom + 55)
#define MsgID_VoteOff (MsgID_Custom + 56)
#define MsgID_FaeData (MsgID_Custom + 57)
#define MsgID_RequestGame (MsgID_Custom + 58)
#define MsgID_ForcePosition (MsgID_Custom + 60)
#define MsgID_ForceVelocity (MsgID_Custom + 61)
#define MsgID_ForceWeapon (MsgID_Custom + 62)
#define MsgID_ChangeTeam (MsgID_Custom + 63)
#define MsgID_SpecialMessage (MsgID_Custom + 64)
#define MsgID_WeaponActiveMessage (MsgID_Custom + 65)
#define MsgID_JoinServer (MsgID_Custom + 68)
#define MsgID_PlaySound (MsgID_Custom + 70)
#define MsgID_SyncCvars (MsgID_Custom + 71)
#define MsgID_VoiceData (MsgID_Custom + 72)

#define MAX_PLAYERS 32

/* ControlMethod */
#define HUMAN 1
#define BOT 2

/* Request Reply States */
#define OK 1
#define WRONG_VERSION 2
#define WRONG_PASSWORD 3
#define BANNED_IP 4
#define SERVER_FULL 5
#define INVALID_HANDSHAKE 8
#define WRONG_CHECKSUM 9
#define ANTICHEAT_REQUIRED 10
#define ANTICHEAT_REJECTED 11
#define STEAM_ONLY 12

#define LAN 1
#define INTERNET 0

/* FLAG INFO */
#define RETURNRED 1
#define RETURNBLUE 2
#define CAPTURERED 3
#define CAPTUREBLUE 4

/* Kick/Ban Why's */
#define KICK_UNKNOWN 0
#define KICK_NORESPONSE 1
#define KICK_NOCHEATRESPONSE 2
#define KICK_CHANGETEAM 3
#define KICK_PING 4
#define KICK_FLOODING 5
#define KICK_CONSOLE 6
#define KICK_CONNECTCHEAT 7
#define KICK_CHEAT 8
#define KICK_LEFTGAME 9
#define KICK_VOTED 10
#define KICK_AC 11
#define KICK_SILENT 12
#define KICK_STEAMTICKET 13
#define _KICK_END 14

/* Join types */
#define JOIN_NORMAL 0
#define JOIN_SILENT 1

/* RECORD */
#define NETW 0
#define REC 1

#define CLIENTPLAYERRECIEVED_TIME (3 * 60)

#define FLOODIP_MAX 18
#define MAX_FLOODIPS 1000
#define MAX_BANIPS 1000

#define PLAYERNAME_CHARS 24
#define PLAYERHWID_CHARS 11
#define MAPNAME_CHARS 64
#define REASON_CHARS 26

#define ACTYPE_NONE 0
#define ACTYPE_FAE 1

#define MSGTYPE_CMD 0
#define MSGTYPE_PUB 1
#define MSGTYPE_TEAM 2
#define MSGTYPE_RADIO 3

/* ------------------------------------------------------------------ */
/* TPlayer — network player struct (client branch)                      */
/* ------------------------------------------------------------------ */

typedef struct TPlayer TPlayer;

struct TPlayer {
    char *Name;
    unsigned int ShirtColor, PantsColor, SkinColor, HairColor, JetColor;
    int Kills, Deaths;
    unsigned char Flags;
    int PingTicks, PingTicksB, PingTime, Ping;
    unsigned short RealPing;
    unsigned char ConnectionQuality;
    unsigned char Team;
    unsigned char ControlMethod;
    unsigned char Chain, HeadCap, HairStyle;
    unsigned char SecWep;
    unsigned char Camera;
    unsigned char Muted;
    unsigned char SpriteNum; /* 0 if no sprite exists yet */
    bool DemoPlayer;
};

TPlayer *TPlayer_Create(void);
void TPlayer_Free(TPlayer *p);
void TPlayer_ApplyShirtColorFromTeam(TPlayer *p);
TPlayer *TPlayer_Clone(const TPlayer *p);

/* TPlayers = dynamic array */
extern TPlayer **Players;
extern int PlayersCount;
void Players_Add(TPlayer *player);
void Players_Clear(void);

typedef char TStatsString[2048 + 1];
typedef char TIPString[128 + 1];

/* ------------------------------------------------------------------ */
/* TNetwork / TClientNetwork — network classes (client branch)        */
/* ------------------------------------------------------------------ */

typedef int HSteamNetConnection;
typedef int HSteamListenSocket;

#define k_HSteamNetConnection_Invalid 0
#define k_HSteamNetListenSocket_Invalid 0

typedef struct TNetwork TNetwork;
typedef struct TClientNetwork TClientNetwork;

struct TNetwork {
    bool FInit;
    bool FActive;
    SteamNetworkingIPAddr FAddress;
    HSteamNetConnection FPeer;
    HSteamListenSocket FHost;
    void *NetworkingSockets;
    void *NetworkingUtils;
};

TNetwork *TNetwork_Create(void);
void TNetwork_Free(TNetwork *net);
bool TNetwork_Disconnect(TNetwork *net, bool now);
void TNetwork_FlushMsg(TNetwork *net);
char *TNetwork_GetDetailedConnectionStatus(TNetwork *net, HSteamNetConnection h_conn);
SteamNetConnectionRealTimeStatus_t TNetwork_GetConnectionRealTimeStatus(TNetwork *net, HSteamNetConnection h_conn);
void TNetwork_SetConnectionName(TNetwork *net, HSteamNetConnection h_conn, const char *name);
char *TNetwork_GetStringAddress(TNetwork *net, const SteamNetworkingIPAddr *p_address, bool port);
bool TNetwork_SetGlobalConfigValueInt32(TNetwork *net, int e_value, int val);
bool TNetwork_SetGlobalConfigValueFloat(TNetwork *net, int e_value, float val);
bool TNetwork_SetGlobalConfigValueString(TNetwork *net, int e_value, const char *val);
bool TNetwork_SetConnectionConfigValueInt32(TNetwork *net, HSteamNetConnection h_conn, int e_value, int val);
bool TNetwork_SetConnectionConfigValueFloat(TNetwork *net, HSteamNetConnection h_conn, int e_value, float val);
bool TNetwork_SetConnectionConfigValueString(TNetwork *net, HSteamNetConnection h_conn, int e_value, int val);
void TNetwork_SetDebugLevel(TNetwork *net, int level);
HSteamListenSocket TNetwork_Host(TNetwork *net);
HSteamNetConnection TNetwork_Peer(TNetwork *net);
void *TNetwork_NetworkingSocket(TNetwork *net);
void *TNetwork_NetworkingUtil(TNetwork *net);
unsigned short TNetwork_Port(TNetwork *net);
const SteamNetworkingIPAddr *TNetwork_Address(TNetwork *net);

struct TClientNetwork {
    TNetwork base;
};

TClientNetwork *TClientNetwork_Create(void);
void TClientNetwork_Free(TClientNetwork *net);
void TClientNetwork_ProcessLoop(TClientNetwork *net);
void TClientNetwork_ProcessEvents(TClientNetwork *net, void *p_info);
bool TClientNetwork_Connect(TClientNetwork *net, const char *host, unsigned short port);
void TClientNetwork_HandleMessages(TClientNetwork *net, void *incoming_msg);
bool TClientNetwork_SendData(TClientNetwork *net, void *data, int size, int flags);

extern int MainTickCounter;
extern int ClientTickCount, LastHeartBeatCounter;
extern int ClientPlayerReceivedCounter;
extern bool ClientPlayerReceived, ClientPlayerSent;
extern bool ClientVarsRecieved;
extern bool RequestingGame;
extern int NoHeartbeatTime;
extern bool ReceivedUnAccepted;
extern char *VoteMapName;
extern unsigned short VoteMapCount;
extern TPlayer *DummyPlayer;
extern int PlayersNum, BotsNum, SpectatorsNum;
extern int PlayersTeamNum[4 + 1];
extern int PingTicksAdd;

/* Message structures */
typedef struct {
    unsigned char ID;
} TMsgHeader;

typedef struct {
    TMsgHeader Header;
    unsigned char InOrder;
    /* TFaeChallenge Challenge; */
} TMsg_FaeChallenge;

typedef struct {
    TMsgHeader Header;
    /* TFaeResponseBox Response; */
} TMsg_FaeResponse;

#endif /* NET_H */