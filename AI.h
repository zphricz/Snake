#ifndef AI_H
#define AI_H

#include "Util.h"
#include <SDL2/SDL.h>

const Uint32 ai_turn_time = 10; // milliseconds

class AI {
private:
    int num_cells_x;
    int num_cells_y;
    Uint32 ai_start_time;
    bool* grid;
    bool times_up;

    bool already_checked(Coord c);
    bool out_of_bounds(Coord c);
    int enclosed(Coord c, const std::list<Coord>& s);
    Direction search_for_move(Coord fruit, const std::list<Coord>& s);
    
public:
    AI(int num_x, int num_y);
    ~AI();
    Direction move(Coord fruit, Direction last_move, const std::list<Coord>& snake);
};

#endif
