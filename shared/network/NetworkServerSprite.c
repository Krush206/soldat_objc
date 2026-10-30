/* NetworkServerSprite — C port of shared/network/NetworkServerSprite.pas */

#include "NetworkServerSprite.h"
#include "Net.h"
#include "Sprites.h"
#include "Weapons.h"
#include "Game.h"
#include "Constants.h"
#include "Vector.h"

/* Server sprite snapshot implementations */

void ServerSpriteSnapshot(unsigned char r)
{
    (void)r;
    /* In Pascal: ServerSpriteSnapshot(r: Byte);
     * Would send sprite snapshot data */
}

void ServerSpriteSnapshotMajor(unsigned char r)
{
    (void)r;
    /* In Pascal: ServerSpriteSnapshotMajor(r: Byte);
     * Would send major sprite snapshot */
}

void ServerSpriteSnapshotMajorSingle(unsigned char who, unsigned char r)
{
    (void)who; (void)r;
    /* In Pascal: ServerSpriteSnapshotMajorSingle(Who: Byte; r: Byte);
     * Would send major sprite snapshot for single sprite */
}

void ServerSkeletonSnapshot(unsigned char r)
{
    (void)r;
    /* In Pascal: ServerSkeletonSnapshot(r: Byte);
     * Would send skeleton snapshot */
}

void ServerSpriteDeltas(unsigned char i)
{
    (void)i;
    /* In Pascal: ServerSpriteDeltas(i: Byte);
     * Would send sprite deltas */
}

void ServerSpriteDeltasMouse(unsigned char i)
{
    (void)i;
    /* In Pascal: ServerSpriteDeltasMouse(i: Byte);
     * Would send sprite delta mouse */
}

void ServerSpriteDeath(int who, int killer, int bulletNum, int where)
{
    (void)who; (void)killer; (void)bulletNum; (void)where;
    /* In Pascal: ServerSpriteDeath(Who, Killer, BulletNum, Where: Integer);
     * Would handle sprite death */
}

void ServerHandleClientSpriteSnapshot(void *netMessage)
{
    (void)netMessage;
    /* In Pascal: ServerHandleClientSpriteSnapshot(NetMessage: PSteamNetworkingMessage_t);
     * Would handle client sprite snapshot */
}

void ServerHandleClientSpriteSnapshot_Mov(void *netMessage)
{
    (void)netMessage;
    /* In Pascal: ServerHandleClientSpriteSnapshot_Mov(NetMessage: PSteamNetworkingMessage_t);
     * Would handle client sprite snapshot movement */
}

void ServerHandleClientSpriteSnapshot_Dead(void *netMessage)
{
    (void)netMessage;
    /* In Pascal: ServerHandleClientSpriteSnapshot_Dead(NetMessage: PSteamNetworkingMessage_t);
     * Would handle client sprite snapshot dead */
}