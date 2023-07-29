#import <Foundation/Foundation.h>
#import <SDL2/SDL.h>
#import "GameMenus.h"
#import "Sound.h"
#import "../shared/Weapons.h"

@implementation TGameButton
- (void) setActive: (BOOL) o
{
	active = o;
}

- (void) setX: (int) o
{
	x = o;
}

- (void) setY: (int) o
{
	y = o;
}

- (void) setX2: (int) o
{
	x2 = o;
}

- (void) setY2: (int) o
{
	y2 = o;
}

- (void) setCaption: (NSString *) o
{
	caption = o;
}

- (BOOL) getActive
{
	return active;
}

- (int) getX
{
	return x;
}

- (int) getY
{
	return y;
}

- (int) getX2
{
	return x2;
}

- (int) getY2
{
	return y2;
}

- (NSString *) getCaption
{
	return caption;
}
@end

@implementation TGameMenu
- (void) setActive: (BOOL) o
{
	active = o;
}

- (void) setX: (int) o
{
	x = o;
}

- (void) setY: (int) o
{
	y = o;
}

- (void) setW: (int) o
{
	w = o;
}

- (void) setH: (int) o
{
	h = o;
}

- (void) setCaption: (NSString *) o
{
	caption = o;
}

- (void) setButton: (TGameButton *) o
{
	if(!button)
		button = [[NSMutableArray alloc] initWithObjects: o];
	else
		[button addObject: o];
}

- (BOOL) getActive
{
	return active;
}

- (int) getX
{
	return x;
}

- (int) getY
{
	return y;
}

- (int) getW
{
	return w;
}

- (int) getH
{
	return h;
}

- (NSString *) getCaption
{
	return caption;
}

- (NSArray *) getButton
{
	return button;
}
@end

@implementation GameMenus
- (TGameButton *) init_button: (TGameMenu *) menu caption: (NSString *) caption x: (int) x y: (int) y w: (int) w h: (int) h
{
	TGameButton *button = [TGameButton new];
	[button setActive: YES];
	[button setX: [menu getX] + x];
	[button setY: [menu getY] + y];
	[button setX2: [menu getX] + x + w];
	[button setY2: [menu getY] + y + h];
	[button setCaption: caption];

	return button;
}

- (void) init_game_menus
{
	int i;

	hoveredmenu = NULL;
	hoveredbutton = NULL;
	hoveredbuttonindex = 0;

	escmenu = [TGameMenu new];
	teammenu = [TGameMenu new];
	limbomenu = [TGameMenu new];
	kickmenu = [TGameMenu new];
	mapmenu = [TGameMenu new];
	gamemenu = [[NSArray alloc] initWithObjects: escmenu, teammenu, limbomenu, kickmenu, mapmenu];

	[escmenu setW: 300];
	[escmenu setH: 200];

	if(r_scaleinterface.value)
	{
		[escmenu setW: (gamewidth - [escmenu getW]) / 2];
		[escmenu setH: (gameheight - [escmenu getH]) / 2];
	}
	else
	{
		[escmenu setX: (renderwidth - [escmenu getW]) / 2];
		[escmenu setY: (renderheight - [escmenu getH]) / 2];
	}
	
	[escmenu setButton: [self init_button: escmenu
	caption: @"1 Exit to menu" x: 5 y: 1 * 25 w: 240 h: 25]];
	[escmenu setButton: [self init_button: escmenu
	caption: @"2 Change map" x: 5 y: 2 * 25 w: 240 h: 25]];
	[escmenu setButton: [self init_button: escmenu
	caption: @"3 Kick player" x: 5 y: 3 * 25 w: 240 h: 25]];
	[escmenu setButton: [self init_button: escmenu
	caption: @"4 Change team" x: 5 y: 4 * 25 w: 240 h: 25]];

	[teammenu setW: 0];
	[teammenu setH: 0];
	[teammenu setX: 0];
	[teammenu setY: 0];

	[teammenu setButton: [self init_button: teammenu
	caption: @"0 Player" x: 40 y: 140 + 40 * 1 w: 215 h: 35]];
	[teammenu setButton: [self init_button: teammenu
	caption: @"1 Alpha Team" x: 40 y: 140 + 40 * 1 w: 215 h: 35]];
	[teammenu setButton: [self init_button: teammenu
	caption: @"2 Bravo Team" x: 40 y: 140 + 40 * 2 w: 215 h: 35]];
	[teammenu setButton: [self init_button: teammenu
	caption: @"3 Charlie Team" x: 40 y: 140 + 40 * 3 w: 215 h: 35]];
	[teammenu setButton: [self init_button: teammenu
	caption: @"4 Delta Team" x: 40 y: 140 + 40 * 4 w: 215 h: 35]];
	[teammenu setButton: [self init_button: teammenu
	caption: @"5 Spectator" x: 40 y: 140 + 40 * 5 w: 215 h: 35]];

	[limbomenu setW: 0];
	[limbomenu setH: 0];
	[limbomenu setX: 0];
	[limbomenu setY: 0];

	for(i = 0; i < MAIN_WEAPONS; i++)
	{
		NSString *s;

		if(i < PRIMARY_WEAPONS)
			s = [[NSString alloc] initWithFormat: @"%d %@", i, [[TGun getGuns] objectAtIndex: i]];
		else
			s = [[NSString alloc] initWithString: [[TGun getGuns] objectAtIndex: i]];

		[limbomenu setButton: [self init_button: limbomenu
		caption: s x: 35 y: 154 + 18 * i w: 235 h: 15]];
	}

	[kickmenu setW: 370];
	[kickmenu setH: 90];
	[kickmenu setX: 125];
	[kickmenu setY: 355];

	[kickmenu setButton: [self init_button: kickmenu
	caption: @"<<<<" x: 15 y: 35 w: 90 h: 25]];
	[kickmenu setButton: [self init_button: kickmenu
	caption: @">>>>" x: 265 y: 35 w: 90 h: 25]];
	[kickmenu setButton: [self init_button: kickmenu
	caption: @"Kick" x: 105 y: 55 w: 90 h: 25]];
	[kickmenu setButton: [self init_button: kickmenu
	caption: @"Ban" x: 195 y: 55 w: 80 h: 25]];

	[mapmenu setW: 370];
	[mapmenu setH: 90];
	[mapmenu setX: 125];
	[mapmenu setW: 355];

	[mapmenu setButton: [self init_button: mapmenu
	caption: @"<<<<" x: 15 y: 35 w: 90 h: 25]];
	[mapmenu setButton: [self init_button: mapmenu
	caption: @">>>>" x: 265 y: 35 w: 90 h: 25]];
	[mapmenu setButton: [self init_button: mapmenu
	caption: @"Select" x: 120 y: 55 w: 90 h: 25]];
}

- (void) hide_all
{
	int i;

	for(i = 0; i < [gamemenu count]; i++)
		[[gamemenu objectAtIndex: i] setActive: NO];
}

- (void) game_menu_show: (TGameMenu *) menu show: (BOOL) show
{
	if([menu isEqualTo: escmenu])
	{
		if(show)
		{
			int i;

			if([limbomenu getActive])
				limbowasactive = YES;

			[self hide_all];
			fragsmenushow = NO;
			statsmenushow = NO;

			for(i = 0; i < MAX_PLAYERS; i++)
				if(sprite[i].active)
				{
					stop_sound(sprite[i].reloadsoundchannel);
					stop_sound(sprite[i].jetssoundchannel);
					stop_sound(sprite[i].gattlingsoundchannel);
					stop_sound(sprite[i].gattlingsoundchannel2);
				}

			if(cl_runs.value < 3)
				noobshow = YES;
		}
		else
		{
			[self hide_all];
			noobshow = NO;

			if(limbowasactive)
				[limbomenu setActive: YES];
		}
	}
	else if([menu isEqualTo: teammenu] && show)
	{
		[self hide_all];

		if(show)
			switch(sv_gamemode.value)
			{
				case GAMESTYLE_CTF:
				case GAMESTYLE_INF:
				case GAMESTYLE_HTF:
					[[[menu getButton] objectAtIndex: 0] setActive: NO];
					[[[menu getButton] objectAtIndex: 1] setActive: YES];
					[[[menu getButton] objectAtIndex: 2] setActive: YES];
					[[[menu getButton] objectAtIndex: 3] setActive: NO];
					[[[menu getButton] objectAtIndex: 4] setActive: NO];
					break;
				case GAMESTYLE_TEAMMATCH:
					[[[menu getButton] objectAtIndex: 0] setActive: NO];
					[[[menu getButton] objectAtIndex: 1] setActive: YES];
					[[[menu getButton] objectAtIndex: 2] setActive: YES];
					[[[menu getButton] objectAtIndex: 3] setActive: YES];
					[[[menu getButton] objectAtIndex: 4] setActive: YES];
					break;
				default:
					[[[menu getButton] objectAtIndex: 0] setActive: YES];
					[[[menu getButton] objectAtIndex: 1] setActive: NO];
					[[[menu getButton] objectAtIndex: 2] setActive: NO];
					[[[menu getButton] objectAtIndex: 3] setActive: NO];
					[[[menu getButton] objectAtIndex: 4] setActive: NO];
			}
	}
	else if([menu isEqualTo: mapmenu] && show)
	{
		client_vote_map(mapmenuindex);
		[kickmenu setActive: NO];
	}
	else if([menu isEqualTo: mapmenu] && show)
	{
		kickmenuindex = 1;
		[mapmenu setActive: NO];

		if(players_num < 1)
			menu = NULL;
	}
	else if([menu isEqualTo: limbomenu])
	{
		[menu setActive: NO];

		if(!show) limbowasactive = NO;
		else if(!weaponsingame) menu = NULL;
	}

	if(menu) [menu setActive: show];

	[self game_menu_mouse_move];
}

- (BOOL) game_menu_action: (TGameMenu *) menu buttonIndex: (int) buttonindex
{
	int i, ret = 0;

	if(buttonindex >= 0 && [[[menu getButton] objectAtIndex: buttonindex] getActive])
	{
		if((ret = [menu isEqualTo: escmenu]))
		{
			switch(buttonindex)
			{
				case 1:
					[self game_menu_show: mapmenu show: ![mapmenu getActive]];
					break;
				case 2:
					[self game_menu_show: kickmenu show: ![kickmenu getActive]];
					break;
				case 3:
					if((ret = (mysprite > 0 && mapchangecounter < 0)))
					{
						[self game_menu_show: teammenu show: YES];
						mapchangecounter = -60;
						selteam = 0;
					}
					else if((ret = (!mysprite && is_teamgame))) [self game_menu_show: teammenu show: YES];
			}
		}
		else if((ret = [menu isEqualTo: teammenu]))
		{
			[self game_menu_show: menu show: NO];
			selteam = buttonindex;

			if(!mysprite || buttonindex != sprite[mysprite].player.team) client_send_player_info();
		}
		else if([menu isEqualTo: kickmenu])
		{
			i = kickmenuindex;

			if(playersnum < 1) [self game_menu_show: kickmenu show: NO];
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
						[self game_menu_show: escmenu show: NO];
						chattext = "";
						chatchanged = 1;
						votekickreasontype = 1;
						SDL_StartTextInput();
					}
			}
		}
		else if([menu isEqualTo: mapmenu])
		{
			if(playersnum < 1) [self game_menu_show: kickmenu show: NO];
			else switch(buttonindex)
			{
				case 0:
					if(mapmenuindex < 0) client_vote_map(--mapmenuindex);

					ret = kickmenuindex != 0;
					break;
				case 1:
					if(mapmenuindex < votemapcount - 1) client_vote_map(--mapmenuindex);

					ret = mapmenuindex <= votemapcount - 1;
					break;
				case 2:
				{
					NSString *s;

					[self game_menu_show: escmenu show: NO];
					s = [[NSString alloc] initWithFormat: @"votemap %@", votemapname];
					client_send_string_message(s, MSGTYPE_CMD);
				}
			}
		}
		else if((ret = ([menu isEqualTo: limbomenu] && mysprite > 0)))
		{
			i = buttonindex + 1;

			if(weaponactive[i] == 1 && weaponsel[mysprite][i] == 1)
			{
				if(i < 10)
				{
					if(weaponactive[i] == 1 && weaponsel[mysprite][i] == 1)
						sprite[mysprite].selweapon = guns[i].num;

					if(sprite[mysprite].selweapon > 0)
					{
						[self game_menu_show: limbomenu show: NO];

						if(!sprite[mysprite].deadmeat && (sprite[mysprite].weapon.num != guns[bow].num || sprite[mysprite].weapon.num != guns[bow2].num))
						{
							sprite[mysprite].apply_weapon_by_num(sprite[mysprite].selweapon, 1);
							client_sprite_snapshot();
						}
					}
				}
				else
				{
					int count = 0;

					cl_player_secwep(i - 11);
					sprite[mysprite].player.secwep = i - 11;
					sprite[mysprite].apply_weapon_by_num(guns[i].num, 2);

					for(i = 0; i < PRIMARY_WEAPONS; i++)
						count += weaponactive[i];

					if(!count)
					{
						[self game_menu_show: limbomenu show: NO];
						sprite[mysprite].weapon = sprite[mysprite].secondaryweapon;
						sprite[mysprite].secondaryweapon = guns[NOWEAPON];
					}

					if(!sprite[mysprite].deadmeat) client_sprite_snapshot();
				}
			}
		}

		if(ret) play_sound(SFX_MENUCLICK);
	}

	return ret;
}

- (void) game_menu_mouse_move
{
	int i;
	float x, y;
	TGameButton *btn;

	hoveredmenu = NULL;
	hoveredbutton = NULL;
	hoveredbuttonindex = 0;

	x = mx * _rscala.x;
	y = my * _rscala.y;

	for(i = 0; i < [gamemenu count]; i++)
		if([[gamemenu objectAtIndex: i] getActive])
		{
			int j;

			for(j = 0; j < [[[gamemenu objectAtIndex: i] getButton] count]; j++)
			{
				btn = [[[gamemenu objectAtIndex: i] getButton] objectAtIndex: j];

				if([btn getActive] && x > [btn getX] && x < [btn getX2] && y > [btn getY] && y < [btn getY2])
				{
					hoveredmenu = [gamemenu objectAtIndex: i];
					hoveredbutton = btn;
					hoveredbuttonindex = i;
					return;
				}
			}
		}
}

- (BOOL) game_menu_click
{
	int ret = 0;

	if(hoveredbutton)
		return ret = [self game_menu_action: hoveredmenu buttonIndex: hoveredbuttonindex];
}
@end
