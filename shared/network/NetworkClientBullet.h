/* NetworkClientBullet — C port of shared/network/NetworkClientBullet.pas */

#ifndef NETWORKCLIENTBULLET_H
#define NETWORKCLIENTBULLET_H

#include "Constants.h"

void ClientSendBullet(unsigned char i);
void ClientHandleBulletSnapshot(void *msg);

#endif /* NETWORKCLIENTBULLET_H */