/* NetworkClientGame — C port of shared/network/NetworkClientGame.pas */
#include "NetworkClientGame.h"
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
#include "InterfaceGraphics.h"
#include <stdlib.h>

void ClientHandleNewPlayer(void *net_message) {
    (void)net_message;
}

void ClientVoteKick(unsigned char num, bool ban, const char *reason) {
    (void)num; (void)ban; (void)reason;
}

void ClientVoteMap(unsigned short map_id) {
    (void)map_id;
}

void ClientHandleVoteResponse(void *net_message) {
    (void)net_message;
}

void ClientFreeCamTarget(void) {
}

void ClientHandlePlayerDisconnect(void *net_message) {
    (void)net_message;
}

void ClientHandleMapChange(void *net_message) {
    (void)net_message;
}

void ClientHandleFlagInfo(void *net_message) {
    (void)net_message;
}

void ClientHandleIdleAnimation(void *net_message) {
    (void)net_message;
}