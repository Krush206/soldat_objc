/* NetworkClientSprite — C port of shared/network/NetworkClientSprite.pas */
#include "NetworkClientSprite.h"
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

void ClientHandleServerSpriteSnapshot(void *net_message) {
    (void)net_message;
}

void ClientHandleServerSpriteSnapshotMajor(void *net_message) {
    (void)net_message;
}

void ClientHandleServerSkeletonSnapshot(void *net_message) {
    (void)net_message;
}

void ClientSpriteSnapshot(void) {
}

void ClientSpriteSnapshotMov(void) {
}

void ClientSpriteSnapshotDead(void) {
}

void ClientHandleSpriteDeath(void *net_message) {
    (void)net_message;
}

void ClientHandleDeltaMovement(void *net_message) {
    (void)net_message;
}

void ClientHandleDeltaMouseAim(void *net_message) {
    (void)net_message;
}

void ClientHandleDeltaWeapons(void *net_message) {
    (void)net_message;
}

void ClientHandleDeltaHelmet(void *net_message) {
    (void)net_message;
}

void ClientHandleClientSpriteSnapshotDead(void *net_message) {
    (void)net_message;
}