/* NetworkServerMessages — C port of shared/network/NetworkServerMessages.pas */

#include "NetworkServerMessages.h"
#include "Net.h"
#include "Game.h"
#include "Constants.h"
#include "Vector.h"

/* Server message function implementations */
void ServerSendStringMessage(const char *text, unsigned char tonum, unsigned char from, unsigned char msgtype)
{
    (void)text; (void)tonum; (void)from; (void)msgtype;
    /* In Pascal: ServerSendStringMessage(Text: WideString; ToNum: Byte; From: Byte; MsgType: Byte);
     * Would send a string message to a client */
}

void ServerHandleChatMessage(void *netMessage)
{
    (void)netMessage;
    /* In Pascal: ServerHandleChatMessage(NetMessage: PSteamNetworkingMessage_t);
     * Would handle chat message from client */
}

void ServerSendSpecialMessage(const char *text, unsigned char msgtype)
{
    (void)text; (void)msgtype;
    /* In Pascal: ServerSendSpecialMessage(Text: string; MsgType: Byte);
     * Would send a special message */
}