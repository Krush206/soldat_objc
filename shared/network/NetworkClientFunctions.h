/* NetworkClientFunctions — C port of shared/network/NetworkClientFunctions.pas */

#ifndef NETWORKCLIENTFUNCTIONS_H
#define NETWORKCLIENTFUNCTIONS_H

#include "Net.h"

void ClientHandleVoteOn(void *net_message);
void ClientHandleVoteOff(void);
void ClientHandleServerSyncMsg(void *net_message);
void ClientHandleForcePosition(void *net_message);
void ClientHandleForceVelocity(void *net_message);
void ClientHandleForceWeapon(void *net_message);
void ClientHandleWeaponActiveMessage(void *net_message);
void ClientHandleClientFreeCam(void *net_message);
void ClientHandleJoinServer(void *net_message);
void ClientHandlePlaySound(void *net_message);

#endif /* NETWORKCLIENTFUNCTIONS_H */