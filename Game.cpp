#include <iostream>
#include "Game.h"

using std::cout;
using std::endl;

Color bg_color{25, 25, 25};
Color grid_color{150, 150, 150};
Color snake_color{0, 200, 0};
Color fruit_color{200, 0, 0};

Game::Game(int num_x, int num_y, Screen * screen) :
    scr(screen),
    num_cells_x(num_x),
    num_cells_y(num_y) {
    if (num_cells_x < 4 || num_cells_y < 4) {
        cout << "ERROR: Too few cells to play with" << endl;
        exit(1);
    }
    if (num_cells_x > scr->width / 2 || num_cells_y > scr->height / 2) {
        cout << "ERROR: Too many cells to display on this screen" << endl;
        exit(1);
    }
}

Game::~Game() {
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
            case SDLK_SPACE:
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

void Game::draw_world() {
    scr->fill_screen(bg_color);
    scr->set_color(grid_color);
    for (int y = 1; y < num_cells_y; ++y) {
        scr->hor_line(y * scr->height / num_cells_y, 0, scr->width - 1);
    }
    for (int x = 1; x < num_cells_x; ++x) {
        scr->ver_line(x * scr->width / num_cells_x, 0, scr->height - 1);
    }
}

bool Game::collides_with_snake(Coord c) {
    for (auto & part : snake) {
        if (part == c) {
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

void Game::draw_cell(Coord cell, Color color) {
    int x1, y1, x2, y2;
    x1 = cell.x * scr->width / num_cells_x + 1;
    x2 = (cell.x + 1) * scr->width / num_cells_x - 1;
    y1 = cell.y * scr->height / num_cells_y + 1;
    y2 = (cell.y + 1) * scr->height / num_cells_y - 1;
    scr->fill_rect(x1, y1, x2, y2, color);
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
    if (next == fruit) {
        snake_growing += grow_factor;
    } 
    if (!snake_growing) {
        Coord c = snake.back();
        snake.pop_back();
        draw_cell(c, bg_color);
    } else {
        snake_growing--;
    }
    if (collides_with_snake(next) || out_of_bounds(next)) {
        init_game(); // game over
        return;
    }
    snake.push_front(next);
    draw_cell(next, snake_color);
    if (next == fruit) {
        place_new_fruit();
        draw_cell(fruit, fruit_color);
    }
}

void Game::init_game() {
    direction = RIGHT;
    last_move = NONE;
    snake_growing = 0;
    game_paused = true;
    game_running = true;
    snake.clear();
    snake.push_back({num_cells_x / 2, num_cells_y / 2});
    snake.push_back({num_cells_x / 2 - 1, num_cells_y / 2});
    snake.push_back({num_cells_x / 2 - 2, num_cells_y / 2});
    draw_world();
    for (auto& part : snake) {
        draw_cell(part, snake_color);
    }
    place_new_fruit();
    draw_cell(fruit, fruit_color);
}

// Runs the game loop
void Game::play() {
    init_game();
    int i = 0;
    while (game_running) {
        handle_input();
        if (!game_paused) {
            if (i % frames_per_update == 0) {
                step_game();
            }
            i++;
        }
        scr->commit_screen();
    }
}

