#include <SDL2/SDL.h>
#include <iostream>
#include "Screen.h"
#include "Game.h"

static void error(char * name) {
    printf("Usage: %s [Cells_x Cells_y] [Screen_x Screen_y]\n", name);
    exit(1);
}

int main(int argc, char* argv[])
{
    int screen_width;
    int screen_height;
    int cells_x;
    int cells_y;
    bool default_screen;
    bool default_cells;
    bool full_screen;
    SDL_DisplayMode display;

    // Initialize SDL.
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        exit(1);
    }
    SDL_GetCurrentDisplayMode(0, &display);
    atexit(SDL_Quit);
    srand(time(NULL));

    switch (argc) {
    case 1:
        default_screen = true;
        default_cells = true;
        break;
    case 3:
        default_screen = true;
        default_cells = false;
        cells_x = atoi(argv[1]);
        cells_y = atoi(argv[2]);
        if (cells_x <= 0 || cells_y <= 0) {
            error(argv[0]);
        }
        break;
    case 5:
        default_screen = false;
        default_cells = false;
        cells_x = atoi(argv[1]);
        cells_y = atoi(argv[2]);
        screen_width = atoi(argv[3]);
        screen_height = atoi(argv[4]);
        if (screen_width <= 0 || screen_height <= 0 || cells_x <= 0 || cells_y <= 0) {
            error(argv[0]);
        }
        break;
    default:
        error(argv[0]);
    }
    if (default_screen) {
        screen_width = display.w;
        screen_height = display.h;
    }
    if (default_cells) {
        cells_x = screen_width / 25;
        cells_y = screen_height / 25;
    }
    if (screen_width == display.w && screen_height == display.h) {
        full_screen = true;
    } else {
        full_screen = false;
    }
    Screen scr(screen_width, screen_height, full_screen, "Snake", true);
    Game g(cells_x, cells_y, &scr);
    g.play();
    return 0;
}

