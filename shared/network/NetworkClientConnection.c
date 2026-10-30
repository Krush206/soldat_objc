/* NetworkClientConnection — C port of shared/network/NetworkClientConnection.pas */
#include "NetworkClientConnection.h"
#include "Net.h"
#include "NetworkUtils.h"
#include "Client.h"
#include "Sprites.h"
#include "Bullets.h"
#include "Parts.h"
#include "Weapons.h"
#include "Constants.h"
#include "Vector.h"
#include "Game.h"
#include "Sound.h"
#include "LogFile.h"
#include "TraceLog.h"
#include <stdlib.h>

void ClientRequestGame(void) {
}

void ClientSendPlayerInfo(void) {
}

void ClientDisconnect(void) {
}

void ClientPong(unsigned char ping_num) {
    (void)ping_num;
}

void ClientHandlePlayersList(void *net_message) {
    (void)net_message;
}

void ClientHandleUnAccepted(void *net_message) {
    (void)net_message;
}

void ClientHandleServerDisconnect(void *net_message) {
    (void)net_message;
}

void ClientHandlePing(void *net_message) {
    (void)net_message;
}

void ClientHandleServerVars(void *net_message) {
    (void)net_message;
}

void ClientHandleSyncCvars(void *net_message) {
    (void)net_message;
}