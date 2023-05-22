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

void init_game_menus(void);
void game_menu_show(PGameMenu, int);
int game_menu_action(PGameMenu, int);
void game_menu_mouse_move(void);
int game_menu_click(void);
