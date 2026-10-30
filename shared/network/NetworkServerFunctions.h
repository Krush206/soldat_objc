#ifndef NETWORKSERVERFUNCTIONS_H
#define NETWORKSERVERFUNCTIONS_H

#include <stdint.h>
#include "Net.h"

/* Server management functions */
void SetWeaponActive(unsigned char id, unsigned char weaponNum, int state);
void ForceWeapon(unsigned char id, unsigned char primary, unsigned char secondary,
                 unsigned char ammo, unsigned char secAmmo);
void MovePlayer(unsigned char id, float x, float y);
void ModifyPlayerVelocity(unsigned char id, float velX, float velY);
void ForwardClient(unsigned char id, const char *targetIP, int targetPort, const char *showMsg);
void PlaySound(unsigned char id, const char *name, float x, float y);
void ServerHandleClientFreeCam(void *netMessage);

/* Server send functions */
void ServerSendFreeCam(unsigned char tonum, int freecam, float posX, float posY);

#endif /* NETWORKSERVERFUNCTIONS_H */