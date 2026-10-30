/* NetworkClientThing — C port of shared/network/NetworkClientThing.pas */
#include "NetworkClientThing.h"
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
#include <stdlib.h>

void ClientHandleServerThingSnapshot(void *net_message) {
    (void)net_message;
}

void ClientHandleServerThingMustSnapshot(void *net_message) {
    (void)net_message;
}

void ClientHandleThingTaken(void *net_message) {
    (void)net_message;
}