#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include "Util.h"
#include "Version.h"
#include "PhysFS.h"
#include "SysUtils.h"

int main(int argc, char **argv) {
    (void)argc; (void)argv;

    /* SDL initialization */
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        fprintf(stderr, "SDL_Init failed: %s\n", SDL_GetError());
        return 1;
    }

    /* PhysFS initialization — set base directories so the game can find Data/ */
    const char *base = "Data";

    if (PHYSFS_init(argv[0]) == 0) {
        fprintf(stderr, "PhysFS init failed: %s\n", PHYSFS_getLastError());
        SDL_Quit();
        return 1;
    }

    if (PHYSFS_mount(base, "/", 1) == 0) {
        fprintf(stderr, "PhysFS mount failed: %s\n", PHYSFS_getLastError());
        PHYSFS_deinit();
        SDL_Quit();
        return 1;
    }

    /* Set user directory for config/save games */
    char *userdir = NULL;
    #ifdef _WIN32
    userdir = SDL_GetPrefPath(".", "soldat");
    #else
    userdir = SDL_GetPrefPath(".", "soldat");
    #endif
    (void)userdir; /* PhysFS write dir set later after engine init */

    /* Resolve base and user directories for the rest of the engine */
    extern char *BaseDirectory;
    extern char *UserDirectory;
    BaseDirectory = strdup2(base);
    UserDirectory = strdup2("");

    /* Include version header for SOLDAT_VERSION etc. */

    /* Create window */
    SDL_Window *win = SDL_CreateWindow("Soldat C", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, SDL_WINDOW_SHOWN);
    if (!win) {
        fprintf(stderr, "SDL_CreateWindow failed: %s\n", SDL_GetError());
        PHYSFS_deinit();
        SDL_Quit();
        return 1;
    }

    SDL_Renderer *ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);
    if (!ren) {
        fprintf(stderr, "SDL_CreateRenderer failed: %s\n", SDL_GetError());
        SDL_DestroyWindow(win);
        PHYSFS_deinit();
        SDL_Quit();
        return 1;
    }

    /* Simple event loop – will be replaced by real game loop when Client/Glx are wired up */
    SDL_Event e;
    int running = 1;
    while (running) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) running = 0;
        }
        SDL_SetRenderDrawColor(ren, 0, 0, 0, 255);
        SDL_RenderClear(ren);
        SDL_RenderPresent(ren);
        SDL_Delay(16);
    }

    SDL_DestroyRenderer(ren);
    SDL_DestroyWindow(win);
    PHYSFS_deinit();
    SDL_Quit();
    return 0;
}
