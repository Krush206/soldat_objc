#ifndef NETWORKSERVERTHING_H
#define NETWORKSERVERTHING_H

#include <stdint.h>
#include "Net.h"

/* Server thing management functions */
void ServerThingSnapshot(unsigned char tonum);
void ServerThingMustSnapshot(unsigned char thingNum);
void ServerThingMustSnapshotOnConnect(unsigned char tonum);
void ServerThingMustSnapshotOnConnectTo(unsigned char thingNum, unsigned char tonum);
void ServerThingTaken(unsigned char thingNum, unsigned char who);
void ServerHandleRequestThing(void *netMessage);

#endif /* NETWORKSERVERTHING_H */