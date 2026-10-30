#ifndef NETWORKSERVERGAME_H
#define NETWORKSERVERGAME_H

#include <stdint.h>
#include "Net.h"

/* Server game management */
void ServerMapChange(unsigned char id);
void ServerFlagInfo(unsigned char style, unsigned char who);
void ServerIdleAnimation(unsigned char num, short style);
void ServerSendVoteOn(unsigned char voteStyle, int voter);
void ServerSendVoteOff(void);
void ServerHandleVoteKick(void *netMessage);
void ServerHandleVoteMap(void *netMessage);
void ServerHandleChangeTeam(void *netMessage);
void ServerSyncMsg(int tonum);
void ServerHandleVoiceData(void *netMessage);
void ServerHandlePlayerDisconnect(void *netMessage);

#endif /* NETWORKSERVERGAME_H */