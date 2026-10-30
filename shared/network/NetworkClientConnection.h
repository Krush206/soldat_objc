#ifndef NETWORKCLIENTCONNECTION_H
#define NETWORKCLIENTCONNECTION_H

#include <stdint.h>
#include "Net.h"

void ClientRequestGame(void);
void ClientSendPlayerInfo(void);
void ClientDisconnect(void);
void ClientPong(uint8_t ping_num);
void ClientHandlePlayersList(void *net_message);
void ClientHandleUnAccepted(void *net_message);
void ClientHandleServerDisconnect(void *net_message);
void ClientHandlePing(void *net_message);
void ClientHandleServerVars(void *net_message);
void ClientHandleSyncCvars(void *net_message);

#endif /* NETWORKCLIENTCONNECTION_H */