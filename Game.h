#ifndef GAME_H
#define GAME_H

#include <list>
#include "Screen.h"

struct Coord {
    int x;
    int y;
};

typedef enum {UP, DOWN, LEFT, RIGHT, NONE} Direction;

class Game {
private:
    int * stage;
    Screen * const scr;
    bool game_running;
    bool game_paused;
    Coord fruit;
    std::list<Coord> snake;
    Direction direction;
    Direction last_move;

    int& cell_at(int x, int y);
    void handle_input();
    void get_pixel_bounds(int cell_x, int cell_y, int* x1, int* y1, int* x2, int* y2);
    void draw_grid();
    void draw_snake();
    void draw_fruit();
    void draw_game();
    bool collides_with_snake(Coord c);
    bool out_of_bounds(Coord c);
    void place_new_fruit();
    void init_game();
    void step_game();

public:
    const int num_cells_x;
    const int num_cells_y;
    Game(int num_x, int num_y, Screen * screen);
    ~Game();
    void play();
};

#endif
