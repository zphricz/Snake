#ifndef ZACKAI_H
#define ZACKAI_H

#include <vector>
#include <SDL2/SDL.h>
#include "AI.h"

const Uint32 ai_turn_time = 10; // milliseconds
const Uint8 MAX_SEARCH = 1;

class ZackAI : public AI {
private:
  std::vector<Uint8> grid;
  std::vector<Uint8> snake_lookup;
  std::list<Coord> snake;
  Coord fruit;
  bool times_up;
  int num_cells_x;
  int num_cells_y;
  Uint32 ai_start_time;
  Direction chosen_direction;
  Direction first_move;
  Uint32 chosen_depth;

  Uint8 &lookup_at(Coord c);
  Uint8 &grid_at(Coord c);
  int num_empty_spaces(Coord c);
  void search_for_move(Uint32 depth);
  bool out_of_bounds(Coord c);

public:
  ZackAI(int num_x, int num_y);
  ~ZackAI() override;
  Direction move(Coord orig_fruit, Direction last_move,
                 const std::list<Coord> &orig_snake,
                 int snake_growing) override;
};

#endif
