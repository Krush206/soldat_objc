#include "ScriptWeapon.h"
#include <stdlib.h>
#include <string.h>

/* External symbols assumed to be defined in other modules */
extern TGun Guns[];               /* Array of weapon definitions */
extern void ForceWeapon(int spriteNum, uint8_t primary, uint8_t secondary,
                        uint8_t primaryAmmo, uint8_t secondaryAmmo);

/* Weapon number constants (should match Pascal definitions) */
extern const uint8_t NOWEAPON;
extern const uint8_t EAGLE_NUM, MP5_NUM, AK74_NUM, STEYRAUG_NUM, SPAS12_NUM,
                     RUGER77_NUM, M79_NUM, BARRETT_NUM, M249_NUM, MINIGUN_NUM,
                     COLT_NUM, KNIFE_NUM, CHAINSAW_NUM, LAW_NUM, FLAMER_NUM,
                     BOW_NUM, BOW2_NUM;

/* Real weapon identifiers used by Guns[] */
extern const uint8_t EAGLE, MP5, AK74, STEYRAUG, SPAS12, RUGER77, M79,
                     BARRETT, M249, MINIGUN, COLT, KNIFE, CHAINSAW,
                     LAW, FLAMER, BOW, BOW2;

/* ---------- TScriptWeapon ---------- */
TScriptWeapon* ScriptWeapon_Create(void)
{
    TScriptWeapon* self = (TScriptWeapon*)malloc(sizeof(TScriptWeapon));
    if (self) {
        self->FWeapon = NULL;
    }
    return self;
}

void ScriptWeapon_Destroy(TScriptWeapon* self)
{
    if (self) {
        free(self);
    }
}

TGun ScriptWeapon_GetGun(const TScriptWeapon* self)
{
    TGun empty = {0};
    if (!self || !self->FWeapon) return empty;
    return *self->FWeapon;   /* struct copy */
}

uint8_t ScriptWeapon_GetType(const TScriptWeapon* self)
{
    if (!self || !self->FWeapon) return 0;
    return self->FWeapon->Num;
}

const char* ScriptWeapon_GetName(const TScriptWeapon* self)
{
    if (!self || !self->FWeapon) return "";
    return self->FWeapon->Name;
}

uint8_t ScriptWeapon_GetBulletStyle(const TScriptWeapon* self)
{
    if (!self || !self->FWeapon) return 0;
    return self->FWeapon->BulletStyle;
}

uint8_t ScriptWeapon_GetAmmo(const TScriptWeapon* self)
{
    if (!self || !self->FWeapon) return 0;
    return self->FWeapon->AmmoCount;
}

/* Abstract – base implementation does nothing */
void ScriptWeapon_SetAmmo(TScriptWeapon* self, uint8_t ammo)
{
    (void)self;
    (void)ammo;
    /* Derived types must override */
}

/* ---------- TScriptPlayerWeapon ---------- */
TScriptPlayerWeapon* ScriptPlayerWeapon_Create(TSprite* sprite)
{
    TScriptPlayerWeapon* self = (TScriptPlayerWeapon*)malloc(sizeof(TScriptPlayerWeapon));
    if (self) {
        self->base.FWeapon = NULL;
        self->FSprite = sprite;
    }
    return self;
}

void ScriptPlayerWeapon_Destroy(TScriptPlayerWeapon* self)
{
    if (self) free(self);
}

/* ---------- TScriptPrimaryPlayerWeapon ---------- */
TScriptPrimaryPlayerWeapon* ScriptPrimaryPlayerWeapon_Create(TSprite* sprite)
{
    TScriptPrimaryPlayerWeapon* self = (TScriptPrimaryPlayerWeapon*)malloc(sizeof(TScriptPrimaryPlayerWeapon));
    if (self) {
        self->base.base.FWeapon = &sprite->Weapon;
        self->base.FSprite = sprite;
    }
    return self;
}

void ScriptPrimaryPlayerWeapon_Destroy(TScriptPrimaryPlayerWeapon* self)
{
    if (self) free(self);
}

void ScriptPrimaryPlayerWeapon_SetAmmo(TScriptPrimaryPlayerWeapon* self, uint8_t ammo)
{
    if (!self) return;
    /* ForceWeapon(spriteNum, primaryType, secondaryType, primaryAmmo, secondaryAmmo) */
    ForceWeapon(self->base.FSprite->Num,
                self->base.base.FWeapon->Num,
                self->base.FSprite->SecondaryWeapon.Num,
                ammo,
                self->base.FSprite->SecondaryWeapon.AmmoCount);
}

/* ---------- TScriptSecondaryPlayerWeapon ---------- */
TScriptSecondaryPlayerWeapon* ScriptSecondaryPlayerWeapon_Create(TSprite* sprite)
{
    TScriptSecondaryPlayerWeapon* self = (TScriptSecondaryPlayerWeapon*)malloc(sizeof(TScriptSecondaryPlayerWeapon));
    if (self) {
        self->base.base.FWeapon = &sprite->SecondaryWeapon;
        self->base.FSprite = sprite;
    }
    return self;
}

void ScriptSecondaryPlayerWeapon_Destroy(TScriptSecondaryPlayerWeapon* self)
{
    if (self) free(self);
}

void ScriptSecondaryPlayerWeapon_SetAmmo(TScriptSecondaryPlayerWeapon* self, uint8_t ammo)
{
    if (!self) return;
    ForceWeapon(self->base.FSprite->Num,
                self->base.FSprite->Weapon.Num,
                self->base.base.FWeapon->Num,
                self->base.FSprite->Weapon.AmmoCount,
                ammo);
}

/* ---------- TScriptNewWeapon ---------- */
TScriptNewWeapon* ScriptNewWeapon_Create(void)
{
    TScriptNewWeapon* self = (TScriptNewWeapon*)malloc(sizeof(TScriptNewWeapon));
    if (self) {
        self->base.FWeapon = (TGun*)malloc(sizeof(TGun));
        if (self->base.FWeapon) {
            memset(self->base.FWeapon, 0, sizeof(TGun));
            self->base.FWeapon->Num = NOWEAPON;
        }
    }
    return self;
}

void ScriptNewWeapon_Destroy(TScriptNewWeapon* self)
{
    if (self) {
        if (self->base.FWeapon) free(self->base.FWeapon);
        free(self);
    }
}

void ScriptNewWeapon_SetAmmo(TScriptNewWeapon* self, uint8_t ammo)
{
    if (self && self->base.FWeapon) {
        self->base.FWeapon->AmmoCount = ammo;
    }
}

void ScriptNewWeapon_SetType(TScriptNewWeapon* self, uint8_t wtype)
{
    if (self && self->base.FWeapon) {
        self->base.FWeapon->Num = wtype;
    }
}

uint8_t ScriptNewWeapon_GetType(const TScriptNewWeapon* self)
{
    if (!self || !self->base.FWeapon) return 0;
    return self->base.FWeapon->Num;
}

/* ---------- TScriptWeaponChange ---------- */
TScriptWeaponChange* ScriptWeaponChange_Create(void)
{
    TScriptWeaponChange* self = (TScriptWeaponChange*)malloc(sizeof(TScriptWeaponChange));
    if (self) {
        self->base.FWeapon = (TGun*)malloc(sizeof(TGun));
        if (self->base.FWeapon) {
            memset(self->base.FWeapon, 0, sizeof(TGun));
            *self->base.FWeapon = Guns[NOWEAPON];
        }
    }
    return self;
}

void ScriptWeaponChange_Destroy(TScriptWeaponChange* self)
{
    if (self) {
        if (self->base.FWeapon) free(self->base.FWeapon);
        free(self);
    }
}

void ScriptWeaponChange_SetAmmo(TScriptWeaponChange* self, uint8_t ammo)
{
    if (self && self->base.FWeapon) {
        self->base.FWeapon->AmmoCount = ammo;
    }
}

void ScriptWeaponChange_SetGun(TScriptWeaponChange* self, uint8_t weapon, uint8_t ammo)
{
    if (!self || !self->base.FWeapon) return;

    uint8_t realWeapon = NOWEAPON;

    switch (weapon) {
        case EAGLE_NUM:    realWeapon = EAGLE;     break;
        case MP5_NUM:      realWeapon = MP5;       break;
        case AK74_NUM:     realWeapon = AK74;      break;
        case STEYRAUG_NUM: realWeapon = STEYRAUG;  break;
        case SPAS12_NUM:   realWeapon = SPAS12;    break;
        case RUGER77_NUM:  realWeapon = RUGER77;   break;
        case M79_NUM:      realWeapon = M79;       break;
        case BARRETT_NUM:  realWeapon = BARRETT;   break;
        case M249_NUM:     realWeapon = M249;      break;
        case MINIGUN_NUM:  realWeapon = MINIGUN;   break;
        case COLT_NUM:     realWeapon = COLT;      break;
        case KNIFE_NUM:    realWeapon = KNIFE;     break;
        case CHAINSAW_NUM: realWeapon = CHAINSAW;  break;
        case LAW_NUM:      realWeapon = LAW;       break;
        case FLAMER_NUM:   realWeapon = FLAMER;    break;
        case BOW_NUM:      realWeapon = BOW;       break;
        case BOW2_NUM:     realWeapon = BOW2;      break;
        default:           realWeapon = NOWEAPON;  break;
    }

    *self->base.FWeapon = Guns[realWeapon];
    self->base.FWeapon->AmmoCount = ammo;
}

/* ---------- TScriptWeaponAPI ---------- */
/* The Pascal compiler/runtime registration is not applicable in pure C.
   Provide empty implementations to satisfy the interface. */
void ScriptWeaponAPI_CompilerRegister(TScriptWeaponAPI* self, void* compiler)
{
    (void)self;
    (void)compiler;
    /* No‑op in C port */
}

void ScriptWeaponAPI_RuntimeRegisterApi(TScriptWeaponAPI* self, void* exec)
{
    (void)self;
    (void)exec;
    /* No‑op in C port */
}