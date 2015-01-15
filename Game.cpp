#include <iostream>
#include <vector>
#include "Game.h"

using std::cout;
using std::endl;
using std::list;
using std::vector;

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

bool Game::detect_collision(Coord c, list<Coord> s) {
    for (auto & part : snake) {
        if (part == c) {
            return true;
        }
    }
    return false;
}

bool Game::collides_with_snake(Coord c) {
    return detect_collision(c, snake);
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

int Game::enclosed(Coord c, list<Coord> s) {
    int size = 0;
    vector<bool> arr;
    arr.reserve(num_cells_x * num_cells_y);
    vector<Coord> v;
    for (int i = 0; i < num_cells_x * num_cells_y; ++i) {
        arr[i] = false;
    }
    for (auto& part : s) {
        arr[num_cells_x * part.y + part.x] = true;
    }
    v.push_back(c);
    while (!v.empty()) {
        size++;
        Coord new_c = v.back();
        v.pop_back();
        arr[new_c.y * num_cells_x + new_c.x] = true;
        Coord c1{new_c.x - 1, new_c.y};
        Coord c2{new_c.x + 1, new_c.y};
        Coord c3{new_c.x, new_c.y - 1};
        Coord c4{new_c.x, new_c.y + 1};
        if (!out_of_bounds(c1) && !arr[c1.y * num_cells_x + c1.x]) {
            v.push_back(c1);
        }
        if (!out_of_bounds(c2) && !arr[c2.y * num_cells_x + c2.x]) {
            v.push_back(c2);
        }
        if (!out_of_bounds(c3) && !arr[c3.y * num_cells_x + c3.x]) {
            v.push_back(c3);
        }
        if (!out_of_bounds(c4) && !arr[c4.y * num_cells_x + c4.x]) {
            v.push_back(c4);
        }
    }
    return size;
}

Direction Game::ai_move(list<Coord> s) {
    Coord c = s.front();
    Coord right{c.x + 1, c.y};
    Coord left{c.x - 1, c.y};
    Coord up{c.x, c.y - 1};
    Coord down{c.x, c.y + 1};
    if (right == fruit) {
        return RIGHT;
    } else if (left == fruit) {
        return LEFT;
    } else if (up == fruit) {
        return UP;
    } else if (down == fruit) {
        return DOWN;
    }
    Direction dirs[4];
    if (fruit.x > c.x) {
        dirs[0] = RIGHT;
        if (fruit.y > c.y) {
            dirs[1] = DOWN;
            dirs[2] = LEFT;
            dirs[3] = UP;
        } else {
            dirs[1] = UP;
            dirs[2] = LEFT;
            dirs[3] = DOWN;
        }
    } else if (fruit.x < c.x) {
        dirs[0] = LEFT;
        if (fruit.y > c.y) {
            dirs[1] = DOWN;
            dirs[2] = RIGHT;
            dirs[3] = UP;
        } else {
            dirs[1] = UP;
            dirs[2] = RIGHT;
            dirs[3] = DOWN;
        }
    } else {
        if (fruit.y > c.y) {
            dirs[0] = DOWN;
            dirs[1] = RIGHT;
            dirs[2] = LEFT;
            dirs[3] = UP;
        } else {
            dirs[0] = UP;
            dirs[1] = RIGHT;
            dirs[2] = LEFT;
            dirs[3] = DOWN;
        }
    }
    auto new_snake = list<Coord>(s);
    new_snake.pop_back();
    for (int i = 0; i < 4; ++i) {
        if (SDL_GetTicks() - ai_time_start > ai_turn_time) {
            return NONE;
        }
        switch (dirs[i]) {
        case RIGHT: {
            if (!out_of_bounds(right) && !detect_collision(right, new_snake)) {
                new_snake.push_front(right);
                if (ai_move(new_snake) != NONE) {
                    return RIGHT;
                }
                new_snake.pop_front();
            }
            break;
        }
        case LEFT: {
            if (!out_of_bounds(left) && !detect_collision(left, new_snake)) {
                auto new_snake = list<Coord>(s);
                new_snake.push_front(left);
                if (ai_move(new_snake) != NONE) {
                    return LEFT;
                }
                new_snake.pop_front();
            }
            break;
        }
        case UP: {
            if (!out_of_bounds(up) && !detect_collision(up, new_snake)) {
                auto new_snake = list<Coord>(s);
                new_snake.push_front(up);
                if (ai_move(new_snake) != NONE) {
                    return UP;
                }
                new_snake.pop_front();
            }
            break;
        }
        case DOWN: {
            if (!out_of_bounds(down) && !detect_collision(down, new_snake)) {
                auto new_snake = list<Coord>(s);
                new_snake.push_front(down);
                if (ai_move(new_snake) != NONE) {
                    return DOWN;
                }
                new_snake.pop_front();
            }
            break;
        }
        default: {
            break;
        } 
        }
    }
    return NONE;
}

void Game::do_ai_turn() {
    ai_time_start = SDL_GetTicks();
    Direction d = ai_move(snake);
    if (d != NONE) {
        direction = d;
    } else {
        // should detect collision on the snake minus it's tail
        Coord c = snake.front();
        Coord right{c.x + 1, c.y};
        Coord left{c.x - 1, c.y};
        Coord up{c.x, c.y - 1};
        Coord down{c.x, c.y + 1};
        Coord next;
        switch (last_move) {
        case UP:
            next = up;
            break;
        case DOWN:
            next = down;
            break;
        case LEFT:
            next = left;
            break;
        case RIGHT:
            next = right;
            break;
        default:
            next = c;
            break;
        }
        bool out_of_bounds_left = out_of_bounds(left);
        bool collision_left = detect_collision(left, snake);
        bool out_of_bounds_right = out_of_bounds(right);
        bool collision_right = detect_collision(right, snake);
        bool out_of_bounds_down = out_of_bounds(down);
        bool collision_down = detect_collision(down, snake);
        bool out_of_bounds_up = out_of_bounds(up);
        bool collision_up = detect_collision(up, snake);
        if (last_move == UP || last_move == DOWN) {
            if (out_of_bounds(next) || detect_collision(next, snake)) {
                if (out_of_bounds_right || collision_right) {
                    direction = LEFT;
                } else if (out_of_bounds_left || collision_left) {
                    direction = RIGHT;
                } else {
                    int enclosed_left = enclosed(left, snake);
                    int enclosed_right = enclosed(right, snake);
                    if (enclosed_left > enclosed_right) {
                        direction = LEFT;
                    } else {
                        direction = RIGHT;
                    }
                }
            }
        } else {
            if (out_of_bounds(next) || detect_collision(next, snake)) {
                if (out_of_bounds_up || collision_up) {
                    direction = DOWN;
                } else if (out_of_bounds_down || collision_down) {
                    direction = UP;
                } else {
                    int enclosed_down = enclosed(down, snake);
                    int enclosed_up = enclosed(up, snake);
                    if (enclosed_down > enclosed_up) {
                        direction = DOWN;
                    } else {
                        direction = UP;
                    }
                }
            }
        }
    }
}

// Runs the game loop
void Game::play() {
    init_game();
    int i = 0;
    while (game_running) {
        handle_input();
#if DO_AI == 1
        do_ai_turn();
        
#endif
        if (!game_paused) {
            if (i % frames_per_update == 0) {
                step_game();
            }
            i++;
        }
        scr->commit_screen();
    }
}

