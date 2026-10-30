/* NetworkServerThing — C port of shared/network/NetworkServerThing.pas */

#include "NetworkServerThing.h"
#include "Net.h"
#include "Game.h"
#include "Constants.h"
#include "Vector.h"
#include "Sprites.h"

/* Server thing management implementations */

void ServerThingSnapshot(unsigned char tonum)
{
    (void)tonum;
    /* In Pascal: ServerThingSnapshot(ToNum: Byte);
     * Would send thing snapshot */
}

void ServerThingMustSnapshot(unsigned char thingNum)
{
    (void)thingNum;
    /* In Pascal: ServerThingMustSnapshot(ThingNum: Byte);
     * Would force thing snapshot */
}

void ServerThingMustSnapshotOnConnect(unsigned char tonum)
{
    (void)tonum;
    /* In Pascal: ServerThingMustSnapshotOnConnect({$IFDEF SERVER}ToNum: Byte{$ENDIF});
     * Would force thing snapshot on connect */
}

void ServerThingMustSnapshotOnConnectTo(unsigned char thingNum, unsigned char tonum)
{
    (void)thingNum; (void)tonum;
    /* In Pascal: ServerThingMustSnapshotOnConnectTo(ThingNum, ToNum: Byte);
     * Would force thing snapshot on connect to */
}

void ServerThingTaken(unsigned char thingNum, unsigned char who)
{
    (void)thingNum; (void)who;
    /* In Pascal: ServerThingTaken(ThingNum, Who: Byte);
     * Would handle thing being taken */
}

void ServerHandleRequestThing(void *netMessage)
{
    (void)netMessage;
    /* In Pascal: ServerHandleRequestThing(NetMessage: PSteamNetworkingMessage_t);
     * Would handle request for thing */
}