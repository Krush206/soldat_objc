/* Input — C port of client/Input.pas */

#ifndef INPUT_H
#define INPUT_H

#include <SDL2/SDL.h>
#include "Constants.h"
#include "Weapons.h"
#include "Classes.h"

typedef enum {
    ACTION_NONE = 0,
    ACTION_LEFT,
    ACTION_RIGHT,
    ACTION_JUMP,
    ACTION_CROUCH,
    ACTION_FIRE,
    ACTION_JET,
    ACTION_RELOAD,
    ACTION_CHANGEWEAPON,
    ACTION_VOICECHAT,
    ACTION_THROWGRENADE,
    ACTION_DROPWEAPON,
    ACTION_PRONE,
    ACTION_FLAGTHROW,
    ACTION_STATSMENU,
    ACTION_GAMESTATS,
    ACTION_MINIMAP,
    ACTION_PLAYERNAME,
    ACTION_FRAGSLIST,
    ACTION_SNIPERLINE,
    ACTION_RADIO,
    ACTION_RECORDDEMO,
    ACTION_VOLUMEUP,
    ACTION_VOLUMEDOWN,
    ACTION_MOUSESENSITIVITYUP,
    ACTION_MOUSESENSITIVITYDOWN,
    ACTION_CMD,
    ACTION_CHAT,
    ACTION_TEAMCHAT,
    ACTION_SNAP,
    ACTION_WEAPONS,
    ACTION_BIND
} TAction;

#define KM_NONE  0
#define KM_ALT   (1 << 0)
#define KM_CTRL  (1 << 1)
#define KM_SHIFT (1 << 2)

typedef struct TBind {
    TAction Action;
    unsigned int keyId;
    unsigned short keyMod;
    char *Command;
} TBind;
typedef TBind *PBind;

extern bool KeyStatus[512 + 1];
extern TBind *Binds;
extern int BindsCount;
extern void *GameWindow;
extern void *GameGLContext;

bool Input_BindKey(const char *key, const char *action, const char *command, unsigned short modifier);
TBind *Input_FindKeyBind(unsigned short key_mods, SDL_Scancode key_code);
void Input_StartInput(void);
void Input_UnbindAll(void);

#endif /* INPUT_H */