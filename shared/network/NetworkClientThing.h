/* NetworkClientThing — Objective-C port of shared/network/NetworkClientThing.pas
 * (client branch; no {$IFDEF SERVER} blocks in this unit). */

#ifndef NETWORKCLIENTTHING_H
#define NETWORKCLIENTTHING_H

void ClientHandleServerThingSnapshot(void *net_message);
void ClientHandleServerThingMustSnapshot(void *net_message);
void ClientHandleThingTaken(void *net_message);

#endif /* NETWORKCLIENTTHING_H */
