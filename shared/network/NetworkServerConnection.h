#ifndef NETWORKSERVERCONNECTION_H
#define NETWORKSERVERCONNECTION_H

#include <stdint.h>
#include "Net.h"

/* Client -> Server message handlers */
void ServerHandleRequestGame(void *netMessage);
void ServerHandlePlayerInfo(void *netMessage);
void ServerHandlePong(void *netMessage);

/* Server -> Client message handlers */
void ServerSendPlayList(int peer);
void ServerSendNewPlayerInfo(unsigned char num, unsigned char joinType);
void ServerSendUnAccepted(unsigned char peer, unsigned char state, const char *message);
void ServerDisconnect(void);
void ServerPlayerDisconnect(int player, unsigned char why, int now);
void ServerPing(unsigned char tonum);
void ServerSyncCvars(unsigned char tonum, int fullSync);
void ServerVars(unsigned char tonum);

/* Server initialization / main loop */
void ServerInitialize(void);
void ServerTick(void);

#endif /* NETWORKSERVERCONNECTION_H */