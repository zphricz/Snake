#ifndef GAME_H
#define GAME_H

#include <list>
#include "Screen.h"

const int grow_factor = 3;
const int frames_per_update = 2;

struct Coord {
    int x;
    int y;
    bool operator==(const Coord& rhs) {
        return x == rhs.x && y == rhs.y;
    }
    bool operator!=(const Coord& rhs) {
        return !(*this == rhs);
    }
};

typedef enum {UP, DOWN, LEFT, RIGHT, NONE} Direction;

class Game {
private:
    Screen* const scr;
    bool game_running;
    bool game_paused;
    int snake_growing;
    Coord fruit;
    std::list<Coord> snake;
    Direction direction;
    Direction last_move;

    void handle_move(Direction d);
    void handle_input();
    void draw_world();
    void draw_cell(Coord cell, Color color);
    bool collides_with_snake(Coord c);
    bool out_of_bounds(Coord c);
    void place_new_fruit();
    void init_game();
    void step_game();

public:
    const int num_cells_x;
    const int num_cells_y;
    Game(int num_x, int num_y, Screen* screen);
    ~Game();
    void play();
};

#endif
