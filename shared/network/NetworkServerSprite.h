#ifndef NETWORKSERVERSPRITE_H
#define NETWORKSERVERSPRITE_H

#include <stdint.h>
#include "Net.h"

/* Server sprite snapshot functions */
void ServerSpriteSnapshot(unsigned char r);
void ServerSpriteSnapshotMajor(unsigned char r);
void ServerSpriteSnapshotMajorSingle(unsigned char who, unsigned char r);
void ServerSkeletonSnapshot(unsigned char r);
void ServerSpriteDeltas(unsigned char i);
void ServerSpriteDeltasMouse(unsigned char i);
void ServerSpriteDeath(int who, int killer, int bulletNum, int where);
void ServerHandleClientSpriteSnapshot(void *netMessage);
void ServerHandleClientSpriteSnapshot_Mov(void *netMessage);
void ServerHandleClientSpriteSnapshot_Dead(void *netMessage);

#endif /* NETWORKSERVERSPRITE_H */