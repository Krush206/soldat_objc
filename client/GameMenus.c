#include <stdlib.h>
#include <string.h>
#include <SDL2/SDL.h>

typedef struct {
	int active, x, y, x2, y2;
	char *caption;
} TGameButton;
typedef TGameButton *PGameButton;

typedef struct {
	int active, x, y, w, h;
	char *caption;
	TGameButton *button;
} TGameMenu;
typedef TGameMenu *PGameMenu;

TGameMenu *gamemenu;
PGameMenu hoveredmenu, escmenu, teammenu, limbomenu, kickmenu, mapmenu, *gamemenu_arr;
PGameButton hoveredbutton, *button_arr;
int hoveredbuttonindex, kickmenuindex = 0, mapmenuindex = 0, limbowasactive;

void init_button(PGameMenu menu, int button, char *caption, int x, int y, int w, int h)
{
	menu->button[button].active = 1;
	menu->button[button].x = menu->y + y;
	menu->button[button].y = menu->y + y;
	menu->button[button].x2 = menu->x + x + w;
	menu->button[button].y2 = menu->y + y + h;
	menu->button[button].caption = caption;
}

void init_game_menus(void)
{
	int i;
	char *s;

	hoveredmenu = (PGameMenu) (hoveredbutton = (PGameButton) NULL);
	hoveredbuttonindex = 0;

	gamemenu = (TGameMenu *) malloc(sizeof(TGameMenu) * 5 + 1);
	gamemenu_arr = (PGameMenu *) malloc(sizeof(PGameMenu) * 5 + 1);
	button_arr = (PGameButton *) malloc(sizeof(PGameButton) * 5 + 1);
	gamemenu_arr[0] = escmenu = &gamemenu[0];
	gamemenu_arr[1] = teammenu = &gamemenu[1];
	gamemenu_arr[2] = limbomenu = &gamemenu[2];
	gamemenu_arr[3] = kickmenu = &gamemenu[3];
	gamemenu_arr[4] = mapmenu = &gamemenu[4];
	button_arr[5] = (PGameButton) (gamemenu_arr[5] = (PGameMenu) NULL);

	escmenu->w = 300;
	escmenu->h = 200;

	if(r_scaleinterface.value)
	{
		escmenu->x = (gamewidth - escmenu->w) / 2;
		escmenu->y = (gameheight - escmenu->h) / 2;
	}
	else
	{
		escmenu->x = (renderwidth - escmenu->w) / 2;
		escmenu->y = (renderheight - escmenu->h) / 2;
	}
	
	button_arr[0] = escmenu->button = (TGameButton *) malloc(sizeof(TGameButton) * 4);
	init_button(escmenu, 0, "1 Exit to menu", 5, 1 * 25, 240, 25);
	init_button(escmenu, 1, "2 Change map", 5, 2 * 25, 240, 25);
	init_button(escmenu, 2, "3 Kick player", 5, 3 * 25, 240, 25);
	init_button(escmenu, 3, "4 Change team", 5, 4 * 25, 240, 25);

	teammenu->w = 0;
	teammenu->h = 0;
	teammenu->x = 0;
	teammenu->y = 0;

	button_arr[1] = teammenu->button = (TGameButton *) malloc(sizeof(TGameButton) * 6);
	init_button(teammenu, 0, "0 Player", 40, 140 + 40 * 1, 215, 35);
	init_button(teammenu, 1, "1 Alpha Team", 40, 140 + 40 * 1, 215, 35);
	init_button(teammenu, 2, "2 Bravo Team", 40, 140 + 40 * 2, 215, 35);
	init_button(teammenu, 3, "3 Charlie Team", 40, 140 + 40 * 3, 215, 35);
	init_button(teammenu, 4, "4 Delta Team", 40, 140 + 40 * 4, 215, 35);
	init_button(teammenu, 5, "5 Spectator", 40, 140 + 40 * 5, 215, 35);

	limbomenu->w = 0;
	limbomenu->h = 0;
	limbomenu->x = 0;
	limbomenu->y = 0;

	button_arr[2] = limbomenu->button = (TGameButton *) malloc(sizeof(TGameButton) * MAIN_WEAPONS);
	for(i = 0; i <= MAIN_WEAPONS; i++)
	{
		if(i < PRIMARY_WEAPONS)
		{
			s = (char *) malloc(strlen(gundisplayname[guns[i + 1].num]) + 3);
			sprintf(s, "%d %s", i, gundisplayname[guns[i + 1].num]);
		}
		else
		{
			s = (char *) malloc(strlen(gundisplayname[guns[i + 1].num]) + 1);
			sprintf(s, "%s", gundisplayname[guns[i + 1].num]);
		}

		init_button(limbomenu, i, s, 35, 154 + 18 * i, 235, 16);
		free(s);
	}

	kickmenu->w = 370;
	kickmenu->h = 90;
	kickmenu->x = 125;
	kickmenu->y = 355;

	button_arr[3] = kickmenu->button = (TGameButton *) malloc(sizeof(TGameButton) * 4);
	init_button(kickmenu, 0, "<<<<", 15, 35, 90, 25);
	init_button(kickmenu, 1, ">>>>", 265, 35, 90, 25);
	init_button(kickmenu, 2, "Kick", 105, 55, 90, 25);
	init_button(kickmenu, 3, "Ban", 195, 55, 80, 25);

	kickmenu->button[3].active = 0;

	mapmenu->w = 370;
	mapmenu->h = 90;
	mapmenu->x = 125;
	mapmenu->y = 355;

	button_arr[4] = mapmenu->button = (TGameButton *) malloc(sizeof(TGameButton) * 3);
	init_button(mapmenu, 0, "<<<<", 15, 35, 90, 25);
	init_button(mapmenu, 1, ">>>>", 265, 35, 90, 25);
	init_button(mapmenu, 2, "Select", 120, 55, 90, 25);
}

void hide_all(void)
{
	int i;

	for(i = 0; gamemenu_arr[i]; i++)
		gamemenu[i].active = 0;
}

void game_menu_show(PGameMenu menu, int show)
{
	int i;

	if(menu == escmenu)
	{
		if(show) {
			if(limbomenu->active)
				limbowasactive = 1;

			hide_all();
			fragsmenushow = 0;
			statsmenushow = 0;

			for(i = 0; i <= MAX_PLAYERS; i++)
				if(sprite[i].active)
				{
					StopSound(sprite[i].reloadsoundchannel);
					StopSound(sprite[i].jetssoundchannel);
					StopSound(sprite[i].gattlingsoundchannel);
					StopSound(sprite[i].gattlingsoundchannel2);
				}

			if(cl_runs.value < 3)
				noobshow = 1;
		}
		else
		{
			hide_all();
			noobshow = 0;

			if(limbowasactive)
				limbomenu->active = 1;
		}
	}
	else if(menu == teammenu && show)
	{
		hide_all();

		if(show)
			switch(sv_gamemode.value)
			{
				case GAMESTYLE_CTF:
				case GAMESTYLE_INF:
				case GAMESTYLE_HTF:
					menu->button[0].active = 0;
					menu->button[1].active = 1;
					menu->button[2].active = 1;
					menu->button[3].active = 0;
					menu->button[4].active = 0;
					break;
				case GAMESTYLE_TEAMMATCH:
					menu->button[0].active = 0;
					menu->button[1].active = 1;
					menu->button[2].active = 1;
					menu->button[3].active = 1;
					menu->button[4].active = 1;
					break;
				default:
					menu->button[0].active = 1;
					menu->button[1].active = 0;
					menu->button[2].active = 0;
					menu->button[3].active = 0;
					menu->button[4].active = 0;
			}
	}
	else if(menu == mapmenu && show)
	{
		client_vote_map(mapmenuindex);
		kickmenu->active = 0;
	}
	else if(menu == kickmenu && show)
	{
		kickmenuindex = 1;
		mapmenu->active = 0;

		if(players_num < 1)
			menu = NULL;
	}
	else if(menu == limbomenu)
	{
		menu->active = 0;

		if(!show) limbowasactive = 0;
		else if(!weaponsingame) menu = NULL;
	}

	if(menu) menu->active = show;

	game_menu_mouse_move();
}

int game_menu_action(PGameMenu menu, int buttonindex)
{
	int i, count, ret = 0;
	char *s;

	if(buttonindex >= 0 && menu->button[buttonindex].active)
	{
		if((ret = menu == escmenu))
		{
			switch(buttonindex)
			{
				case 1:
					game_menu_show(mapmenu, !mapmenu->active);
					break;
				case 2:
					game_menu_show(kickmenu, !kickmenu->active);
					break;
				case 3:
					if((ret = (mysprite > 0 && mapchangecounter < 0)))
					{
						game_menu_show(teammenu, 1);
						mapchangecounter = -60;
						selteam = 0;
					}
					else if((ret = (!mysprite && is_teamgame))) game_menu_show(teammenu, 1);
			}
		}
		else if((ret = menu == teammenu))
		{
			game_menu_show(menu, 0);
			selteam = buttonindex;

			if(!mysprite || buttonindex != sprite[mysprite].player.team) client_send_player_info();
		}
		else if(menu == kickmenu)
		{
			i = kickmenuindex;

			if(playersnum < 1) game_menu_show(kickmenu, 0);
			else switch(buttonindex)
			{
				case 0:
					kickmenuindex = ((MAX_SPRITES + kickmenuindex - 2) % MAX_SPRITES) + 1;
					while(!sprite[kickmenuindex].active || sprite[kickmenuindex].player.demoplayer)
						kickmenuindex = ((MAX_SPRITES + kickmenuindex - 2) % MAX_SPRITES) + 1;

					ret = kickmenuindex != 1;
					break;
				case 1:
					kickmenuindex = (kickmenuindex % MAX_SPRITES) + 1;
					while(!sprite[kickmenuindex].active || sprite[kickmenuindex].player.demoplayer)
						kickmenuindex = (kickmenuindex % MAX_SPRITES) + 1;

					ret = kickmenuindex != 1;
					break;
				case 2:
					if((ret = kickmenuindex != mysprite))
					{
						game_menu_show(escmenu, 0);
						chattext = "";
						chatchanged = 1;
						votekickreasontype = 1;
						SDL_StartTextInput();
					}
			}
		}
		else if(menu == mapmenu)
		{
			if(playersnum < 1) game_menu_show(kickmenu, 0);
			else switch(buttonindex)
			{
				case 0:
					if(mapmenuindex < 0)
					{
						mapmenuindex -= 1;
						client_vote_map(mapmenuindex);
					}

					ret = kickmenuindex != 0;
					break;
				case 1:
					if(mapmenuindex < votemapcount - 1)
					{
						mapmenuindex -= 1;
						client_vote_map(mapmenuindex);
					}

					ret = mapmenuindex <= votemapcount - 1;
					break;
				case 2:
					game_menu_show(escmenu, 0);
					s = (char *) malloc(strlen("votemap") + strlen(votemapname) + 2);
					sprintf(s, "votemap %s", votemapname);
					client_send_string_message(s, MSGTYPE_CMD);
					free(s);
			}
		}
		else if((ret = (menu == limbomenu && mysprite > 0)))
		{
			i = buttonindex + 1;

			if(weaponactive[i] == 1 && weaponsel[mysprite][i] == 1)
			{
				if(i <= 10)
				{
					if(weaponactive[i] == 1 && weaponsel[mysprite][i] == 1)
						sprite[mysprite].selweapon = guns[i].num;

					if(sprite[mysprite].selweapon > 0)
					{
						game_menu_show(limbomenu, 0);

						if(!sprite[mysprite].deadmeat && (sprite[mysprite].weapon.num != guns[bow].num || sprite[mysprite].weapon.num != guns[bow2].num))
						{
							sprite[mysprite].apply_weapon_by_num(sprite[mysprite].selweapon, 1);
							client_sprite_snapshot();
						}
					}
				}
				else
				{
					cl_player_secwep(i - 11);
					sprite[mysprite].player.secwep = i - 11;
					sprite[mysprite].apply_weapon_by_num(guns[i].num, 2);

					count = 0;
					for(i = 0; i <= PRIMARY_WEAPONS; i++)
						count += weaponactive[i];

					if(!count)
					{
						game_menu_show(limbomenu, 0);
						sprite[mysprite].weapon = sprite[mysprite].secondaryweapon;
						sprite[mysprite].secondaryweapon = guns[NOWEAPON];
					}

					if(!sprite[mysprite].deadmeat) client_sprite_snapshot();
				}
			}
		}

		if(ret) PlaySound(SFX_MENUCLICK);
	}

	return ret;
}

void game_menu_mouse_move(void)
{
	int i, j;
	float x, y;
	PGameButton btn;

	hoveredmenu = NULL;
	hoveredbutton = NULL;
	hoveredbuttonindex = 0;

	x = mx * _rscala.x;
	y = my * _rscala.y;

	for(i = 0; gamemenu_arr[i]; i++)
		if(gamemenu->active)
			for(j = 0; button_arr[j]; j++)
			{
				btn = &gamemenu[i].button[j];

				if(btn->active && x > btn->x && x < btn->x2 && y > btn->y && y < btn->y2)
				{
					hoveredmenu = &gamemenu[i];
					hoveredbutton = btn;
					hoveredbuttonindex = i;
					return;
				}
			}
}

int game_menu_click(void)
{
	int ret = 0;

	if(hoveredbutton)
		return ret = game_menu_action(hoveredmenu, hoveredbuttonindex);
}
