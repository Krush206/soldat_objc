#ifndef NETWORKSERVERFAE_H
#define NETWORKSERVERFAE_H

#include <stdint.h>
#include "Net.h"

/* Fae anti-cheat related functions */
void ServerSendFaeChallenge(int peer, int inOrder);
void ServerHandleFaeResponse(void *netMessage);
void KickForFaeViolation(int player, const char *reason);

#endif /* NETWORKSERVERFAE_H */