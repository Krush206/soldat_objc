#ifndef NETWORKSERVERMESSAGES_H
#define NETWORKSERVERMESSAGES_H

#include <stdint.h>
#include "Net.h"

/* Server message functions */
void ServerSendStringMessage(const char *text, unsigned char tonum, unsigned char from, unsigned char msgtype);
void ServerHandleChatMessage(void *netMessage);
void ServerSendSpecialMessage(const char *text, unsigned char msgtype);

#endif /* NETWORKSERVERMESSAGES_H */