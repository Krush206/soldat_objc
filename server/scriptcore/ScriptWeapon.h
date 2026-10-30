#ifndef SCRIPTWEAPON_H
#define SCRIPTWEAPON_H

#include <stddef.h>
#include <stdint.h>

/* Forward declarations of types defined elsewhere */
typedef struct TGun TGun;
typedef struct TSprite TSprite;

/* Base weapon script object */
typedef struct TScriptWeapon {
    TGun* FWeapon;
} TScriptWeapon;

/* Player weapon base */
typedef struct TScriptPlayerWeapon {
    TScriptWeapon base;
    TSprite* FSprite;
} TScriptPlayerWeapon;

/* Primary weapon */
typedef struct TScriptPrimaryPlayerWeapon {
    TScriptPlayerWeapon base;
} TScriptPrimaryPlayerWeapon;

/* Secondary weapon */
typedef struct TScriptSecondaryPlayerWeapon {
    TScriptPlayerWeapon base;
} TScriptSecondaryPlayerWeapon;

/* New weapon (script created) */
typedef struct TScriptNewWeapon {
    TScriptWeapon base;
} TScriptNewWeapon;

/* Weapon change helper */
typedef struct TScriptWeaponChange {
    TScriptWeapon base;
} TScriptWeaponChange;

/* API registration object */
typedef struct TScriptWeaponAPI {
    /* No data needed for C port */
    int dummy;
} TScriptWeaponAPI;

/* ---------- TScriptWeapon ---------- */
TScriptWeapon* ScriptWeapon_Create(void);
void ScriptWeapon_Destroy(TScriptWeapon* self);
TGun ScriptWeapon_GetGun(const TScriptWeapon* self);
uint8_t ScriptWeapon_GetType(const TScriptWeapon* self);
const char* ScriptWeapon_GetName(const TScriptWeapon* self);
uint8_t ScriptWeapon_GetBulletStyle(const TScriptWeapon* self);
uint8_t ScriptWeapon_GetAmmo(const TScriptWeapon* self);
void ScriptWeapon_SetAmmo(TScriptWeapon* self, uint8_t ammo); /* abstract – must be overridden */

/* ---------- TScriptPlayerWeapon ---------- */
TScriptPlayerWeapon* ScriptPlayerWeapon_Create(TSprite* sprite);
void ScriptPlayerWeapon_Destroy(TScriptPlayerWeapon* self);

/* ---------- TScriptPrimaryPlayerWeapon ---------- */
TScriptPrimaryPlayerWeapon* ScriptPrimaryPlayerWeapon_Create(TSprite* sprite);
void ScriptPrimaryPlayerWeapon_Destroy(TScriptPrimaryPlayerWeapon* self);
void ScriptPrimaryPlayerWeapon_SetAmmo(TScriptPrimaryPlayerWeapon* self, uint8_t ammo);

/* ---------- TScriptSecondaryPlayerWeapon ---------- */
TScriptSecondaryPlayerWeapon* ScriptSecondaryPlayerWeapon_Create(TSprite* sprite);
void ScriptSecondaryPlayerWeapon_Destroy(TScriptSecondaryPlayerWeapon* self);
void ScriptSecondaryPlayerWeapon_SetAmmo(TScriptSecondaryPlayerWeapon* self, uint8_t ammo);

/* ---------- TScriptNewWeapon ---------- */
TScriptNewWeapon* ScriptNewWeapon_Create(void);
void ScriptNewWeapon_Destroy(TScriptNewWeapon* self);
void ScriptNewWeapon_SetAmmo(TScriptNewWeapon* self, uint8_t ammo);
void ScriptNewWeapon_SetType(TScriptNewWeapon* self, uint8_t wtype);
uint8_t ScriptNewWeapon_GetType(const TScriptNewWeapon* self);

/* ---------- TScriptWeaponChange ---------- */
TScriptWeaponChange* ScriptWeaponChange_Create(void);
void ScriptWeaponChange_Destroy(TScriptWeaponChange* self);
void ScriptWeaponChange_SetAmmo(TScriptWeaponChange* self, uint8_t ammo);
void ScriptWeaponChange_SetGun(TScriptWeaponChange* self, uint8_t weapon, uint8_t ammo);

/* ---------- TScriptWeaponAPI ---------- */
void ScriptWeaponAPI_CompilerRegister(TScriptWeaponAPI* self, void* compiler);
void ScriptWeaponAPI_RuntimeRegisterApi(TScriptWeaponAPI* self, void* exec);

#endif /* SCRIPTWEAPON_H */