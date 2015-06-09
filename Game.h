#ifndef GAME_H
#define GAME_H

#include "Screen.h"
#include "AI.h"
#include "Util.h"

const int grow_factor = 3;
const int frames_per_update = 2;

class Game {
private:
  const int num_cells_x;
  const int num_cells_y;
  SoftScreen *const scr;
  bool game_running;
  bool game_paused;
  bool game_over;
  int snake_growing;
  Coord fruit;
  std::list<Coord> snake;
  Direction direction;
  Direction last_move;
  bool ai_plays;
  int ai_speed;
  std::unique_ptr<AI> ai_player;
  int score;

  void handle_input();
  void draw_game();
  void draw_cell(Coord cell, SDL_Color color);
  bool collides_with_snake(Coord c);
  bool out_of_bounds(Coord c);
  void place_new_fruit();
  void init_game();
  void step_game();

public:
  Game(int num_x, int num_y, SoftScreen *screen);
  ~Game();
  void play();
};

#endif
