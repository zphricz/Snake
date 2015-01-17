#ifndef AI_H
#define AI_H

#include <list>
#include <SDL2/SDL.h>
#include "Util.h"

const Uint32 ai_turn_time = 10; // milliseconds

class AI {
private:
    bool* grid;
    bool* snake_lookup;
    std::list<Coord> snake;
    int num_cells_x;
    int num_cells_y;
    Coord fruit;
    bool times_up;
    Uint32 ai_start_time;

    bool& lookup_at(Coord c);
    bool& grid_at(Coord c);
    bool already_checked(Coord c);
    bool out_of_bounds(Coord c);
    int num_empty_spaces(Coord c);
    Direction search_for_move();
    
public:
    AI(int num_x, int num_y);
    ~AI();
    Direction move(Coord orig_fruit, Direction last_move, const std::list<Coord>& orig_snake);
};

#endif
