/* NetworkClientSprite — C port of shared/network/NetworkClientSprite.pas */

#ifndef NETWORKCLIENTSPRITE_H
#define NETWORKCLIENTSPRITE_H

#include "Net.h"

void ClientHandleServerSpriteSnapshot(void *net_message);
void ClientHandleServerSpriteSnapshotMajor(void *net_message);
void ClientHandleServerSkeletonSnapshot(void *net_message);
void ClientSpriteSnapshot(void);
void ClientSpriteSnapshotMov(void);
void ClientSpriteSnapshotDead(void);
void ClientHandleSpriteDeath(void *net_message);
void ClientHandleDeltaMovement(void *net_message);
void ClientHandleDeltaMouseAim(void *net_message);
void ClientHandleDeltaWeapons(void *net_message);
void ClientHandleDeltaHelmet(void *net_message);
void ClientHandleClientSpriteSnapshotDead(void *net_message);

#endif /* NETWORKCLIENTSPRITE_H */