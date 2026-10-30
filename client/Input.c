/* Input — C port of client/Input.pas */
#include "Input.h"
#include "Constants.h"
#include "Weapons.h"
#include "Classes.h"
#include <stdlib.h>
#include <string.h>

bool KeyStatus[512 + 1] = {0};
TBind *Binds = NULL;
int BindsCount = 0;
void *GameWindow = NULL;
void *GameGLContext = NULL;

bool Input_BindKey(const char *key, const char *action, const char *command, unsigned short modifier) {
    (void)key; (void)action; (void)command; (void)modifier;
    return false;
}

TBind *Input_FindKeyBind(unsigned short key_mods, SDL_Scancode key_code) {
    (void)key_mods; (void)key_code;
    return NULL;
}

void Input_StartInput(void) {
}

void Input_UnbindAll(void) {
    if (Binds) {
        for (int i = 0; i < BindsCount; i++) {
            free(Binds[i].Command);
        }
        free(Binds);
        Binds = NULL;
        BindsCount = 0;
    }
}