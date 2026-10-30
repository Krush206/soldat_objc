/* NetworkClientMessages — C port of shared/network/NetworkClientMessages.pas */

#ifndef NETWORKCLIENTMESSAGES_H
#define NETWORKCLIENTMESSAGES_H

#include "Net.h"

void ClientSendStringMessage(const char *text, unsigned char msg_type);
void ClientHandleChatMessage(void *net_message);
void ClientHandleSpecialMessage(void *net_message);

#endif /* NETWORKCLIENTMESSAGES_H */