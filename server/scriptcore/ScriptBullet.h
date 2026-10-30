#ifndef SCRIPT_BULLET_H
#define SCRIPT_BULLET_H

#include <stdint.h>
#include <stdbool.h>

/* Forward declarations of engine structures */
struct TBullet;
struct BulletPartsData;

typedef struct ScriptActiveBullet ScriptActiveBullet;

/* Creation */
ScriptActiveBullet* script_active_bullet_create(uint8_t id, struct TBullet* bullet);

/* Property getters */
bool script_active_bullet_get_active(const ScriptActiveBullet* self);
uint8_t script_active_bullet_get_id(const ScriptActiveBullet* self);
uint8_t script_active_bullet_get_style(const ScriptActiveBullet* self);
float script_active_bullet_get_x(const ScriptActiveBullet* self);
float script_active_bullet_get_y(const ScriptActiveBullet* self);
float script_active_bullet_get_vel_x(const ScriptActiveBullet* self);
float script_active_bullet_get_vel_y(const ScriptActiveBullet* self);
uint8_t script_active_bullet_get_owner(const ScriptActiveBullet* self);

/* Method */
int script_active_bullet_get_owner_weapon_id(const ScriptActiveBullet* self);

/* API registration (optional) */
void script_bullet_compiler_register(void* compiler);
void script_bullet_runtime_register(void* exec);

#endif /* SCRIPT_BULLET_H */