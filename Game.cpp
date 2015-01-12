#include <iostream>
#include "Game.h"

using std::cout;
using std::endl;

static const int slow_factor = 2;

Color bg_color{25, 25, 25};
Color grid_color{150, 150, 150};
Color snake_color{0, 200, 0};
Color fruit_color{200, 0, 0};

Game::Game(int num_x, int num_y, Screen * screen) :
    num_cells_x(num_x),
    num_cells_y(num_y),
    stage(new int[num_x * num_y]),
    scr(screen),
    game_running(true),
    game_paused(true) {
        memset(stage, 0, num_x * num_y * sizeof(int));
}

Game::~Game() {
    delete [] stage;
}

int& Game::cell_at(int x, int y) {
    return stage[y * num_cells_x + x];
}

void Game::handle_input() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
        case SDL_KEYDOWN: {
            switch (event.key.keysym.sym) {
            case SDLK_ESCAPE: {
                game_running = false;
                break;
            }
            case SDLK_LEFT: {
                if (last_move != RIGHT && !game_paused) {
                    direction = LEFT;
                }
                break;
            }
            case SDLK_RIGHT: {
                if (last_move != LEFT && !game_paused) {
                    direction = RIGHT;
                }
                break;
            }
            case SDLK_UP: {
                if (last_move != DOWN && !game_paused) {
                    direction = UP;
                }
                break;
            }
            case SDLK_DOWN: {
                if (last_move != UP && !game_paused) {
                    direction = DOWN;
                }
                break;
            }
            case SDLK_RETURN: {
                game_paused = !game_paused;
                break;
            }
            default: {
                break;
            }
            }
            break;
        }
        default: {
             break;
        }
        }
    }
}

void Game::get_pixel_bounds(int cell_x, int cell_y, int* x1, int* y1, int* x2, int* y2) {
    *x1 = cell_x * scr->width / num_cells_x + 1;
    *x2 = (cell_x + 1) * scr->width / num_cells_x - 1;
    *y1 = cell_y * scr->height / num_cells_y + 1;
    *y2 = (cell_y + 1) * scr->height / num_cells_y - 1;
}

void Game::draw_grid() {
    scr->set_color(grid_color);
    for (int y = 1; y < num_cells_y; ++y) {
        scr->hor_line(y * scr->height / num_cells_y, 0, scr->width - 1);
    }
    for (int x = 1; x < num_cells_x; ++x) {
        scr->ver_line(x * scr->width / num_cells_x, 0, scr->height - 1);
    }
}

void Game::draw_snake() {
    scr->set_color(snake_color);
    for (auto & part : snake) {
        int x1, y1, x2, y2;
        get_pixel_bounds(part.x, part.y, &x1, &y1, &x2, &y2);
        scr->fill_rect(x1, y1, x2, y2);
    }
}

void Game::draw_fruit() {
    int x1, y1, x2, y2;
    get_pixel_bounds(fruit.x, fruit.y, &x1, &y1, &x2, &y2);
    scr->fill_rect(x1, y1, x2, y2, fruit_color);
}

void Game::draw_game() {
    scr->fill_screen(bg_color);
    draw_grid();
    draw_snake();
    draw_fruit();
    scr->commit_screen();
}

bool Game::collides_with_snake(Coord c) {
    for (auto & part : snake) {
        if (part.x == c.x && part.y == c.y) {
            return true;
        }
    }
    return false;
}

bool Game::out_of_bounds(Coord c) {
    return (c.x >= num_cells_x) || (c.x < 0) || (c.y >= num_cells_y) || (c.y < 0);
}

void Game::place_new_fruit() {
    do {
        fruit.x = rand() % num_cells_x;
        fruit.y = rand() % num_cells_y;
    } while (collides_with_snake(fruit));
}

void Game::init_game() {
    direction = RIGHT;
    last_move = NONE;
    snake.clear();
    snake.push_back({num_cells_x / 2, num_cells_y / 2});
    snake.push_back({num_cells_x / 2 - 1, num_cells_y / 2});
    snake.push_back({num_cells_x / 2 - 2, num_cells_y / 2});
    place_new_fruit();
    game_paused = true;
}

void Game::step_game() {
    Coord c = snake.front();
    Coord next = c;
    switch(direction) {
    case UP: {
        next.y--;
        break;
    }
    case DOWN: {
        next.y++;
        break;
    }
    case LEFT: {
        next.x--;
        break;
    }
    case RIGHT: {
        next.x++;
        break;
    }
    default: {
        break;
    }
    }
    last_move = direction;
    if (collides_with_snake(next) || out_of_bounds(next)) {
        init_game();
        return;
    }
    snake.push_front(next);
    if (next.x == fruit.x && next.y == fruit.y) {
        place_new_fruit();
    } else {
        snake.pop_back();
    }
}

// Runs the game loop
void Game::play() {
    bool simulate = true;
    init_game();
    while (game_running) {
        handle_input();
        if (!game_paused) {
            static int i = 0;
            if (i % slow_factor == 0) {
                step_game();
            }
            i++;
        }
        draw_game();
    }
}

