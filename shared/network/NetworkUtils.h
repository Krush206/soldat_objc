/* NetworkUtils — C port of shared/network/NetworkUtils.pas */

#ifndef NETWORKUTILS_H
#define NETWORKUTILS_H

#include "Sprites.h"

bool IsWrongGameVersion(const char *request_version);
bool VerifyPacket(int valid_size, int receive_size, int packet_id);
bool VerifyPacketLargerOrEqual(int valid_size, int receive_size, int packet_id);
char *FixPlayerName(const char *name);
void PlayRadioSound(unsigned char radio_id);
void NewPlayerWeapon(void);
char *ReturnFixedPlayerName(const char *name);
void EncodeKeys(TSprite *sprite_c, unsigned short *keys16);
void DecodeKeys(TSprite *sprite_c, unsigned short *keys16);
void StringToArray(char *c, int c_count, const char *s);

#endif /* NETWORKUTILS_H */