#include <SDL2/SDL.h>
#include <iostream>
#include "Screen.h"
#include "Game.h"

static void error(char *name) {
  printf("Usage: %s [Cells_x Cells_y] [Screen_x Screen_y]\n", name);
  exit(1);
}

int main(int argc, char *argv[]) {
  SDL_DisplayMode display;
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    exit(1);
  }
  SDL_GetCurrentDisplayMode(0, &display);
  atexit(SDL_Quit);
  srand(time(NULL));

  int screen_width = display.w;
  int screen_height = display.h;
  int num_cells_x = screen_width / 25;
  int num_cells_y = screen_height / 25;

  switch (argc) {
  case 5:
    screen_width = atoi(argv[3]);
    screen_height = atoi(argv[4]);
    if (screen_width <= 0 || screen_height <= 0) {
      error(argv[0]);
    }
  case 3:
    num_cells_x = atoi(argv[1]);
    num_cells_y = atoi(argv[2]);
    if (num_cells_x <= 0 || num_cells_y <= 0) {
      error(argv[0]);
    }
  case 1:
    break;
  default:
    error(argv[0]);
  }

  bool full_screen;
  if (screen_width == display.w && screen_height == display.h) {
    full_screen = true;
  } else {
    full_screen = false;
  }

  SoftScreen scr(screen_width, screen_height, "Snake", full_screen, true);
  Game g(num_cells_x, num_cells_y, &scr);
  g.play();
  return 0;
}
