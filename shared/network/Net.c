/* Net — C port of shared/network/Net.pas */
#include "Net.h"
#include <stdlib.h>
#include <string.h>

/* --- TPlayer --- */

TPlayer *TPlayer_Create(void) {
    TPlayer *p = calloc(1, sizeof(TPlayer));
    return p;
}

void TPlayer_Free(TPlayer *p) {
    if (!p) return;
    free(p->Name);
    free(p);
}

void TPlayer_ApplyShirtColorFromTeam(TPlayer *p) {
    (void)p;
}

TPlayer *TPlayer_Clone(const TPlayer *p) {
    if (!p) return NULL;
    TPlayer *copy = TPlayer_Create();
    if (!copy) return NULL;
    copy->Name = p->Name ? strdup(p->Name) : NULL;
    copy->ShirtColor = p->ShirtColor;
    copy->PantsColor = p->PantsColor;
    copy->SkinColor = p->SkinColor;
    copy->HairColor = p->HairColor;
    copy->JetColor = p->JetColor;
    copy->Kills = p->Kills;
    copy->Deaths = p->Deaths;
    copy->Flags = p->Flags;
    copy->PingTicks = p->PingTicks;
    copy->PingTicksB = p->PingTicksB;
    copy->PingTime = p->PingTime;
    copy->Ping = p->Ping;
    copy->RealPing = p->RealPing;
    copy->ConnectionQuality = p->ConnectionQuality;
    copy->Team = p->Team;
    copy->ControlMethod = p->ControlMethod;
    copy->Chain = p->Chain;
    copy->HeadCap = p->HeadCap;
    copy->HairStyle = p->HairStyle;
    copy->SecWep = p->SecWep;
    copy->Camera = p->Camera;
    copy->Muted = p->Muted;
    copy->SpriteNum = p->SpriteNum;
    copy->DemoPlayer = p->DemoPlayer;
    return copy;
}

/* --- TPlayers --- */

TPlayer **Players = NULL;
int PlayersCount = 0;

void Players_Add(TPlayer *player) {
    if (!player) return;
    Players = realloc(Players, (PlayersCount + 1) * sizeof(TPlayer *));
    Players[PlayersCount++] = player;
}

void Players_Clear(void) {
    for (int i = 0; i < PlayersCount; i++) {
        TPlayer_Free(Players[i]);
    }
    free(Players);
    Players = NULL;
    PlayersCount = 0;
}

/* --- TNetwork --- */

TNetwork *TNetwork_Create(void) {
    TNetwork *net = calloc(1, sizeof(TNetwork));
    return net;
}

void TNetwork_Free(TNetwork *net) {
    if (!net) return;
    free(net);
}

bool TNetwork_Disconnect(TNetwork *net, bool now) {
    (void)net; (void)now;
    return false;
}

void TNetwork_FlushMsg(TNetwork *net) {
    (void)net;
}

char *TNetwork_GetDetailedConnectionStatus(TNetwork *net, HSteamNetConnection h_conn) {
    (void)net; (void)h_conn;
    return strdup("unknown");
}

SteamNetConnectionRealTimeStatus_t TNetwork_GetConnectionRealTimeStatus(TNetwork *net, HSteamNetConnection h_conn) {
    SteamNetConnectionRealTimeStatus_t s = {0};
    (void)net; (void)h_conn;
    return s;
}

void TNetwork_SetConnectionName(TNetwork *net, HSteamNetConnection h_conn, const char *name) {
    (void)net; (void)h_conn; (void)name;
}

char *TNetwork_GetStringAddress(TNetwork *net, const SteamNetworkingIPAddr *p_address, bool port) {
    (void)net; (void)p_address; (void)port;
    return strdup("0.0.0.0");
}

bool TNetwork_SetGlobalConfigValueInt32(TNetwork *net, int e_value, int val) {
    (void)net; (void)e_value; (void)val;
    return false;
}

bool TNetwork_SetGlobalConfigValueFloat(TNetwork *net, int e_value, float val) {
    (void)net; (void)e_value; (void)val;
    return false;
}

bool TNetwork_SetGlobalConfigValueString(TNetwork *net, int e_value, const char *val) {
    (void)net; (void)e_value; (void)val;
    return false;
}

bool TNetwork_SetConnectionConfigValueInt32(TNetwork *net, HSteamNetConnection h_conn, int e_value, int val) {
    (void)net; (void)h_conn; (void)e_value; (void)val;
    return false;
}

bool TNetwork_SetConnectionConfigValueFloat(TNetwork *net, HSteamNetConnection h_conn, int e_value, float val) {
    (void)net; (void)h_conn; (void)e_value; (void)val;
    return false;
}

bool TNetwork_SetConnectionConfigValueString(TNetwork *net, HSteamNetConnection h_conn, int e_value, int val) {
    (void)net; (void)h_conn; (void)e_value; (void)val;
    return false;
}

void TNetwork_SetDebugLevel(TNetwork *net, int level) {
    (void)net; (void)level;
}

HSteamListenSocket TNetwork_Host(TNetwork *net) {
    (void)net;
    return 0;
}

HSteamNetConnection TNetwork_Peer(TNetwork *net) {
    (void)net;
    return 0;
}

void *TNetwork_NetworkingSocket(TNetwork *net) {
    (void)net;
    return NULL;
}

void *TNetwork_NetworkingUtil(TNetwork *net) {
    (void)net;
    return NULL;
}

unsigned short TNetwork_Port(TNetwork *net) {
    (void)net;
    return 0;
}

const SteamNetworkingIPAddr *TNetwork_Address(TNetwork *net) {
    (void)net;
    return NULL;
}

/* --- TClientNetwork --- */

TClientNetwork *TClientNetwork_Create(void) {
    TClientNetwork *net = calloc(1, sizeof(TClientNetwork));
    return net;
}

void TClientNetwork_Free(TClientNetwork *net) {
    if (!net) return;
    TNetwork_Free(&net->base);
    free(net);
}

void TClientNetwork_ProcessLoop(TClientNetwork *net) {
    (void)net;
}

void TClientNetwork_ProcessEvents(TClientNetwork *net, void *p_info) {
    (void)net; (void)p_info;
}

bool TClientNetwork_Connect(TClientNetwork *net, const char *host, unsigned short port) {
    (void)net; (void)host; (void)port;
    return false;
}

void TClientNetwork_HandleMessages(TClientNetwork *net, void *incoming_msg) {
    (void)net; (void)incoming_msg;
}

bool TClientNetwork_SendData(TClientNetwork *net, void *data, int size, int flags) {
    (void)net; (void)data; (void)size; (void)flags;
    return false;
}

/* --- Globals --- */

int MainTickCounter = 0;
int ClientTickCount = 0, LastHeartBeatCounter = 0;
int ClientPlayerReceivedCounter = 0;
bool ClientPlayerReceived = false, ClientPlayerSent = false;
bool ClientVarsRecieved = false;
bool RequestingGame = false;
int NoHeartbeatTime = 0;
bool ReceivedUnAccepted = false;
char *VoteMapName = NULL;
unsigned short VoteMapCount = 0;
TPlayer *DummyPlayer = NULL;
int PlayersNum = 0, BotsNum = 0, SpectatorsNum = 0;
int PlayersTeamNum[4 + 1] = {0};
int PingTicksAdd = 0;