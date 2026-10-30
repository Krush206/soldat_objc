#include "ScriptBullet.h"
#include <stdlib.h>

/* Engine global data – declared in other modules */
extern struct TBullet Bullet[256];
extern struct BulletPartsData {
    struct Vec2 { float x, y; } Pos[256];
    struct Vec2 Velocity[256];
} BulletParts;

/* Definition of the active‑bullet wrapper */
struct ScriptActiveBullet {
    uint8_t id;
    struct TBullet* bullet;
};

/* ------------------------------------------------------------------ */
/* Construction                                                       */
/* ------------------------------------------------------------------ */
ScriptActiveBullet* script_active_bullet_create(uint8_t id, struct TBullet* bullet)
{
    ScriptActiveBullet* self = (ScriptActiveBullet*)malloc(sizeof(ScriptActiveBullet));
    if (!self) return NULL;
    self->id = id;
    self->bullet = bullet;
    return self;
}

/* ------------------------------------------------------------------ */
/* Property getters                                                    */
/* ------------------------------------------------------------------ */
bool script_active_bullet_get_active(const ScriptActiveBullet* self)
{
    return self->bullet->Active;
}

uint8_t script_active_bullet_get_id(const ScriptActiveBullet* self)
{
    return self->id;
}

uint8_t script_active_bullet_get_style(const ScriptActiveBullet* self)
{
    return self->bullet->Style;
}

float script_active_bullet_get_x(const ScriptActiveBullet* self)
{
    return BulletParts.Pos[self->id].x;
}

float script_active_bullet_get_y(const ScriptActiveBullet* self)
{
    return BulletParts.Pos[self->id].y;
}

float script_active_bullet_get_vel_x(const ScriptActiveBullet* self)
{
    return BulletParts.Velocity[self->id].x;
}

float script_active_bullet_get_vel_y(const ScriptActiveBullet* self)
{
    return BulletParts.Velocity[self->id].y;
}

uint8_t script_active_bullet_get_owner(const ScriptActiveBullet* self)
{
    return self->bullet->Owner;
}

/* ------------------------------------------------------------------ */
/* Method implementation                                               */
/* ------------------------------------------------------------------ */
int script_active_bullet_get_owner_weapon_id(const ScriptActiveBullet* self)
{
    return Bullet[self->id].OwnerWeapon;
}

/* ------------------------------------------------------------------ */
/* Script‑core registration stubs                                      */
/* ------------------------------------------------------------------ */
void script_bullet_compiler_register(void* compiler)
{
    /* The Pascal version registers a compile‑time class with the
       Pascal compiler.  In the C port this is a no‑op because the
       scripting engine is now driven from C. */
    (void)compiler;
}

void script_bullet_runtime_register(void* exec)
{
    /* The Pascal version registers runtime helpers for the
       TActiveMapBullet class.  The C implementation provides the
       functions above directly, so this function is also a no‑op. */
    (void)exec;
}