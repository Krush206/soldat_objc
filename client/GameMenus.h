@interface TGameButton: NSObject
{
        int active, x, y, x2, y2;
        NSString *caption;
}

- (void) setActive: (BOOL) o
- (void) setX: (int) o
- (void) setY: (int) o
- (void) setX2: (int) o
- (void) setY2: (int) o
- (void) setCaption: (NSString *) o
- (BOOL) getActive
- (int) getX
- (int) getY
- (int) getX2
- (int) getY2
- (NSString *) getCaption
@end

@interface TGameMenu: NSObject
{
        int active, x, y, w, h;
        NSString *caption;
        NSMutableArray *button;
}

- (void) setActive: (BOOL) o
- (void) setX: (int) o
- (void) setY: (int) o
- (void) setW: (int) o
- (void) setH: (int) o
- (void) setCaption: (NSString *) o
- (void) setButton: (TGameButton *) o
- (BOOL) getActive
- (int) getX
- (int) getY
- (int) getW
- (int) getH
- (NSString *) getCaption
- (TGameButton *) getButton
@end

@interface GameMenus: NSObject
- (void) init_button: (NSString *) caption x: (int) x y: (int) y w: (int) w h: (int) h
- (void) init_game_menus;
- (void) game_menu_show: (TGameMenu *) menu show: (BOOL) show;
- (BOOL) game_menu_action: (TGameMenu *) menu buttonIndex: (int) buttonindex;
- (void) game_menu_mouse_move;
- (BOOL) game_menu_click;
@end

TGameMenu *hoveredmenu, *escmenu, *teammenu, *limbomenu, *kickmenu, *mapmenu, *gamemenu;
TGameButton *hoveredbutton;
int hoveredbuttonindex, kickmenuindex = 0, mapmenuindex = 0;
BOOL limbowasactive;
