/* GameMenus.c — C port stub */
#include "GameMenus.h"

PGameMenu hoveredmenu, escmenu, teammenu, limbomenu, kickmenu, mapmenu, *gamemenu;
PGameButton hoveredbutton, *button_arr;
int hoveredbuttonindex, kickmenuindex, mapmenuindex, limbowasactive;

void init_game_menus(void) {}
void game_menu_show(PGameMenu m, int f) {}
int game_menu_action(PGameMenu m, int a) { return 0; }
void game_menu_mouse_move(void) {}
int game_menu_click(void) { return 0; }
