/* NetworkClientBullet — C port of shared/network/NetworkClientBullet.pas */
#include "NetworkClientBullet.h"
#include "Net.h"
#include "NetworkUtils.h"
#include "Client.h"
#include "Sprites.h"
#include "Bullets.h"
#include "Parts.h"
#include "Weapons.h"
#include "Constants.h"
#include "Vector.h"
#include "Game.h"
#include "Sound.h"
#include <stdlib.h>

void ClientSendBullet(unsigned char i) {
    (void)i;
}

void ClientHandleBulletSnapshot(void *msg) {
    (void)msg;
}