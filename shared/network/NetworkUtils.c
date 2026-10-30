/* NetworkUtils — C port of shared/network/NetworkUtils.pas */
#include "NetworkUtils.h"
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
#include <string.h>

bool IsWrongGameVersion(const char *request_version) {
    (void)request_version;
    return false;
}

bool VerifyPacket(int valid_size, int receive_size, int packet_id) {
    (void)valid_size; (void)receive_size; (void)packet_id;
    return true;
}

bool VerifyPacketLargerOrEqual(int valid_size, int receive_size, int packet_id) {
    (void)valid_size; (void)receive_size; (void)packet_id;
    return true;
}

char *FixPlayerName(const char *name) {
    (void)name;
    return strdup("Player");
}

void PlayRadioSound(unsigned char radio_id) {
    (void)radio_id;
}

void NewPlayerWeapon(void) {
}

char *ReturnFixedPlayerName(const char *name) {
    (void)name;
    return strdup("Player");
}

void EncodeKeys(TSprite *sprite_c, unsigned short *keys16) {
    (void)sprite_c; (void)keys16;
}

void DecodeKeys(TSprite *sprite_c, unsigned short *keys16) {
    (void)sprite_c; (void)keys16;
}

void StringToArray(char *c, int c_count, const char *s) {
    (void)c; (void)c_count; (void)s;
}