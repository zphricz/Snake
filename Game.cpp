#include <iostream>
#include <vector>
#include <sstream>
#include <string>
#include <iomanip>
#include "Game.h"

using namespace std;

Color bg_color{25, 25, 25};
Color grid_color{150, 150, 150};
Color snake_color{0, 200, 0};
Color fruit_color{200, 0, 0};
Color dead_color{100, 0, 0};
Color head_color{0, 0, 200};

Game::Game(int num_x, int num_y, Screen * screen) :
    num_cells_x(num_x),
    num_cells_y(num_y),
    scr(screen),
    game_running(true),
    ai_plays(false),
    ai_speed(1),
    ai_player(num_x, num_y) {
    if (num_cells_x < 4 || num_cells_y < 4) {
        cout << "ERROR: Too few cells to play with" << endl;
        exit(1);
    }
    if (num_cells_x > scr->width / 2 || num_cells_y > scr->height / 2) {
        cout << "ERROR: Too many cells to display on this screen" << endl;
        exit(1);
    }
    scr->set_recording_style("images", 5);
}

Game::~Game() {
}

void Game::handle_input() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
        case SDL_QUIT: {
            game_running = false;
            break;
        }
        case SDL_KEYDOWN: {
            switch (event.key.keysym.sym) {
            case SDLK_ESCAPE: {
                game_running = false;
                break;
            }
            case SDLK_LEFT: {
                if (ai_plays && ai_speed > 1) {
                    ai_speed /= 2;
                }
                if (last_move != RIGHT && !game_paused) {
                    direction = LEFT;
                }
                break;
            }
            case SDLK_RIGHT: {
                if (ai_plays && ai_speed < 1024) {
                    ai_speed *= 2;
                }
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
                if (game_over) {
                    init_game();
                    game_paused = true;
                } else {
                    game_paused = !game_paused;
                }
                break;
            }
            case SDLK_1: {
                ai_plays = !ai_plays;
                break;
            }
            case SDLK_0: {
                scr->toggle_recording();
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

void Game::draw_cell(Coord cell, Color color) {
    int x1, y1, x2, y2;
    x1 = cell.x * scr->width / num_cells_x + 1;
    x2 = (cell.x + 1) * scr->width / num_cells_x - 1;
    y1 = cell.y * scr->height / num_cells_y + 1;
    y2 = (cell.y + 1) * scr->height / num_cells_y - 1;
    scr->fill_rect(x1, y1, x2, y2, color);
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
        for (auto& part : snake) {
            draw_cell(part, dead_color);
        }
        cout << "Game Over" << endl;
        game_over = true;
        game_paused = true;
        return;
    }
    draw_cell(snake.front(), snake_color);
    snake.push_front(next);
    draw_cell(next, head_color);
    if (next == fruit) {
        place_new_fruit();
        draw_cell(fruit, fruit_color);
        score++;
        cout << "Score: " << score << endl;
    }
}

void Game::init_game() {
    direction = RIGHT;
    last_move = NONE;
    snake_growing = 0;
    game_paused = true;
    game_over = false;
    score = 0;
    snake.clear();
    snake.push_back({num_cells_x / 2, num_cells_y / 2});
    snake.push_back({num_cells_x / 2 - 1, num_cells_y / 2});
    snake.push_back({num_cells_x / 2 - 2, num_cells_y / 2});
    draw_world();
    for (auto& part : snake) {
        draw_cell(part, snake_color);
    }
    draw_cell(snake.front(), head_color);
    place_new_fruit();
    draw_cell(fruit, fruit_color);
    cout << "Game Start" << endl;
}

// Runs the game loop
void Game::play() {
    init_game();
    int i = 0;
    while (game_running) {
        handle_input();
        if (!game_paused) {
            if (ai_plays) {
                for (int j = 0; j < ai_speed; ++j) {
                    direction = ai_player.move(fruit, last_move, snake);
                    step_game();
                    if (game_over) {
                        break;
                    }
                }
            } else if (i % frames_per_update == 0) {
                step_game();
            }
            i++;
        }
        scr->commit();
    }
}

