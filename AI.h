#ifndef AI_H
#define AI_H

#include <list>
#include <vector>
#include <SDL2/SDL.h>
#include "Util.h"

const Uint32 ai_turn_time = 10; // milliseconds

class AI {
private:
    std::vector<Uint8> grid;
    std::vector<Uint8> snake_lookup;
    std::list<Coord> snake;
    const int num_cells_x;
    const int num_cells_y;
    Coord fruit;
    bool times_up;
    Uint32 ai_start_time;

    Uint8& lookup_at(Coord c);
    Uint8& grid_at(Coord c);
    bool out_of_bounds(Coord c);
    int num_empty_spaces(Coord c);
    Direction search_for_move();
    
public:
    AI(int num_x, int num_y);
    ~AI();
    Direction move(Coord orig_fruit, Direction last_move, const std::list<Coord>& orig_snake);
};

#endif
