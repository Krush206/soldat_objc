/* NetworkServerGame — C port of shared/network/NetworkServerGame.pas */

#include "NetworkServerGame.h"
#include "Net.h"
#include "Constants.h"
#include "Vector.h"
#include "Sprites.h"
#include "Weapons.h"
#include "Game.h"
#include "Demo.h"

/* Server game management implementations */

void ServerMapChange(unsigned char id)
{
    (void)id;
    /* In Pascal: ServerMapChange(ID: Byte);
       Would handle map change voting */
}

void ServerFlagInfo(unsigned char style, unsigned char who)
{
    (void)style; (void)who;
    /* In Pascal: ServerFlagInfo(Style, Who: Byte);
       Would send flag information */
}

void ServerIdleAnimation(unsigned char num, short style)
{
    (void)num; (void)style;
    /* In Pascal: ServerIdleAnimation(Num: Byte; Style: SmallInt);
       Would set idle animation for spectator */
}

void ServerSendVoteOn(unsigned char voteStyle, int voter)
{
    (void)voteStyle; (void)voter;
    /* In Pascal: ServerSendVoteOn(VoteStyle: Byte; Voter: Integer);
     * Would turn a vote on */
}

void ServerSendVoteOff(void)
{
    /* In Pascal: ServerSendVoteOff;
     * Would turn a vote off */
    (void)ServerSendVoteOff;
}

void ServerHandleVoteKick(void *netMessage)
{
    (void)netMessage;
    /* In Pascal: ServerHandleVoteKick(NetMessage: PSteamNetworkingMessage_t);
     * Would handle vote kick */
}

void ServerHandleVoteMap(void *netMessage)
{
    (void)netMessage;
    /* In Pascal: ServerHandleVoteMap(NetMessage: PSteamNetworkingMessage_t);
     * Would handle vote map */
}

void ServerHandleChangeTeam(void *netMessage)
{
    (void)netMessage;
    /* In Pascal: ServerHandleChangeTeam(NetMessage: PSteamNetworkingMessage_t);
     * Would handle team change */
}

void ServerSyncMsg(int tonum)
{
    (void)tonum;
    /* In Pascal: ServerSyncMsg(ToNum: Integer = 0);
     * Would synchronize message to client */
}

void ServerHandleVoiceData(void *netMessage)
{
    (void)netMessage;
    /* In Pascal: ServerHandleVoiceData(NetMessage: PSteamNetworkingMessage_t);
     * Would handle voice data */
}

void ServerHandlePlayerDisconnect(void *netMessage)
{
    (void)netMessage;
    /* In Pascal: ServerHandlePlayerDisconnect(NetMessage: PSteamNetworkingMessage_t);
     * Would handle player disconnect */
}