/* NetworkClientMessages — C port of shared/network/NetworkClientMessages.pas */
#include "NetworkClientMessages.h"
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
#include "LogFile.h"
#include "TraceLog.h"
#include "InterfaceGraphics.h"
#include <stdlib.h>

void ClientSendStringMessage(const char *text, unsigned char msg_type) {
    (void)text; (void)msg_type;
}

void ClientHandleChatMessage(void *net_message) {
    (void)net_message;
}

void ClientHandleSpecialMessage(void *net_message) {
    (void)net_message;
}