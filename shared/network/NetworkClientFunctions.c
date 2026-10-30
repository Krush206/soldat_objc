/* NetworkClientFunctions — C port of shared/network/NetworkClientFunctions.pas */
#include "NetworkClientFunctions.h"
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

void ClientHandleVoteOn(void *net_message) {
    (void)net_message;
}

void ClientHandleVoteOff(void) {
}

void ClientHandleServerSyncMsg(void *net_message) {
    (void)net_message;
}

void ClientHandleForcePosition(void *net_message) {
    (void)net_message;
}

void ClientHandleForceVelocity(void *net_message) {
    (void)net_message;
}

void ClientHandleForceWeapon(void *net_message) {
    (void)net_message;
}

void ClientHandleWeaponActiveMessage(void *net_message) {
    (void)net_message;
}

void ClientHandleClientFreeCam(void *net_message) {
    (void)net_message;
}

void ClientHandleJoinServer(void *net_message) {
    (void)net_message;
}

void ClientHandlePlaySound(void *net_message) {
    (void)net_message;
}