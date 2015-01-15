#ifndef GAME_H
#define GAME_H

#include <list>
#include "Screen.h"

#define DO_AI 1

const int grow_factor = 3;
#if DO_AI == 1
const int frames_per_update = 1;
#else
const int frames_per_update = 2;
#endif

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

const Uint32 ai_turn_time = 10;

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
    Uint32 ai_time_start;

    void handle_move(Direction d);
    void handle_input();
    void draw_world();
    void draw_cell(Coord cell, Color color);
    bool detect_collision(Coord c, std::list<Coord> s);
    bool collides_with_snake(Coord c);
    int enclosed(Coord c, std::list<Coord> s);
    bool out_of_bounds(Coord c);
    void place_new_fruit();
    void init_game();
    void step_game();
    Direction ai_move(std::list<Coord> s);
    void do_ai_turn();

public:
    const int num_cells_x;
    const int num_cells_y;
    Game(int num_x, int num_y, Screen* screen);
    ~Game();
    void play();
};

#endif
