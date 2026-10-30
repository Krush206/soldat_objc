/* NetworkClientGame — C port of shared/network/NetworkClientGame.pas */

#ifndef NETWORKCLIENTGAME_H
#define NETWORKCLIENTGAME_H

#include "Net.h"

void ClientHandleNewPlayer(void *net_message);
void ClientVoteKick(unsigned char num, bool ban, const char *reason);
void ClientVoteMap(unsigned short map_id);
void ClientHandleVoteResponse(void *net_message);
void ClientFreeCamTarget(void);
void ClientHandlePlayerDisconnect(void *net_message);
void ClientHandleMapChange(void *net_message);
void ClientHandleFlagInfo(void *net_message);
void ClientHandleIdleAnimation(void *net_message);

#endif /* NETWORKCLIENTGAME_H */