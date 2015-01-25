#include <iostream>
#include <algorithm>
#include "AI.h"

using namespace std;

AI::AI(int num_x, int num_y) :
    num_cells_x(num_x),
    num_cells_y(num_y) {
    grid.resize(num_cells_x * num_cells_y);
    snake_lookup.resize(num_cells_x * num_cells_y);
}

AI::~AI() {
}

Uint8& AI::lookup_at(Coord c) {
    return snake_lookup[c.y * num_cells_x + c.x];
}

Uint8& AI::grid_at(Coord c) {
    return grid[c.y * num_cells_x + c.x];
}

bool AI::out_of_bounds(Coord c) {
    return (c.x >= num_cells_x) || (c.x < 0) || (c.y >= num_cells_y) || (c.y < 0);
}

int AI::num_empty_spaces(Coord c) {
    int size = 0;
    copy(snake_lookup.begin(), snake_lookup.end(), grid.begin());
    vector<Coord> v;
    v.push_back(c);
    while (!v.empty()) {
        Coord new_c = v.back();
        v.pop_back();
        size++;
        grid_at(new_c) = true;
        Coord c1{new_c.x - 1, new_c.y};
        Coord c2{new_c.x + 1, new_c.y};
        Coord c3{new_c.x, new_c.y - 1};
        Coord c4{new_c.x, new_c.y + 1};
        if (!out_of_bounds(c1) && !grid_at(c1)) {
            v.push_back(c1);
        }
        if (!out_of_bounds(c2) && !grid_at(c2)) {
            v.push_back(c2);
        }
        if (!out_of_bounds(c3) && !grid_at(c3)) {
            v.push_back(c3);
        }
        if (!out_of_bounds(c4) && !grid_at(c4)) {
            v.push_back(c4);
        }
    }
    return size;
}

Direction AI::search_for_move() {
    // TODO: The snake will always attempt to get a fruit, even it it assures
    //       that the snake dies. Fix this
    Coord c = snake.front();
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
    if (SDL_GetTicks() - ai_start_time >= ai_turn_time) {
        times_up = true;
        return NONE;
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
    Coord back = snake.back();
    snake.pop_back();
    lookup_at(back) = false;
    for (int i = 0; i < 4; ++i) {
        if (times_up) {
            snake.push_back(back);
            lookup_at(back) = true;
            return NONE;
        }
        switch (dirs[i]) {
        case RIGHT: {
            if (!out_of_bounds(right) && !grid_at(right) && !lookup_at(right)) {
                grid_at(right) = true;
                snake.push_front(right);
                lookup_at(right) = true;
                if (search_for_move() != NONE) {
                    snake.pop_front();
                    lookup_at(right) = false;
                    snake.push_back(back);
                    lookup_at(back) = true;
                    return RIGHT;
                }
                snake.pop_front();
                lookup_at(right) = false;
            }
            break;
        }
        case LEFT: {
            if (!out_of_bounds(left) && !grid_at(left) && !lookup_at(left)) {
                grid_at(left) = true;
                snake.push_front(left);
                lookup_at(left) = true;
                if (search_for_move() != NONE) {
                    snake.pop_front();
                    lookup_at(left) = false;
                    snake.push_back(back);
                    lookup_at(back) = true;
                    return LEFT;
                }
                snake.pop_front();
                lookup_at(left) = false;
            }
            break;
        }
        case UP: {
            if (!out_of_bounds(up) && !grid_at(up) && !lookup_at(up)) {
                grid_at(up) = true;
                snake.push_front(up);
                lookup_at(up) = true;
                if (search_for_move() != NONE) {
                    snake.pop_front();
                    lookup_at(up) = false;
                    snake.push_back(back);
                    lookup_at(back) = true;
                    return UP;
                }
                snake.pop_front();
                lookup_at(up) = false;
            }
            break;
        }
        case DOWN: {
            if (!out_of_bounds(down) && !grid_at(down) && !lookup_at(down)) {
                grid_at(down) = true;
                snake.push_front(down);
                lookup_at(down) = true;
                if (search_for_move() != NONE) {
                    snake.pop_front();
                    lookup_at(down) = false;
                    snake.push_back(back);
                    lookup_at(back) = true;
                    return DOWN;
                }
                snake.pop_front();
                lookup_at(down) = false;
            }
            break;
        }
        default: {
            break;
        } 
        }
    }
    snake.push_back(back);
    lookup_at(back) = true;
    return NONE;
}

Direction AI::move(Coord orig_fruit, Direction last_move, const list<Coord>& orig_snake) {
    // AI fails to take into account that the snake can be growing
    // TODO: Fix this
    fruit = orig_fruit;
    snake = orig_snake;
    fill(snake_lookup.begin(), snake_lookup.end(), false);
    fill(grid.begin(), grid.end(), false);
    for (auto& part: snake) {
        lookup_at(part) = true;
        grid_at(part) = true;
    }
    times_up = false;
    ai_start_time = SDL_GetTicks();
    Direction d = search_for_move();
    if (d != NONE) {
        return d;
    } else {
        Coord back = snake.back();
        snake.pop_back();
        lookup_at(back) = false;
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
        int num_empty_spaces_left = 0;
        int num_empty_spaces_up = 0;
        int num_empty_spaces_down = 0;
        int num_empty_spaces_right = 0;
        if (!out_of_bounds(left) && !lookup_at(left)) {
            num_empty_spaces_left = num_empty_spaces(left);
        }
        if (!out_of_bounds(right) && !lookup_at(right)) {
            num_empty_spaces_right = num_empty_spaces(right);
        }
        if (!out_of_bounds(down) && !lookup_at(down)) {
            num_empty_spaces_down = num_empty_spaces(down);
        }
        if (!out_of_bounds(up) && !lookup_at(up)) {
            num_empty_spaces_up = num_empty_spaces(up);
        }
        //TODO: Make the snake prioritize moving into enclosed areas where its
        //      tail is (it can follow the tail safely)
        //TODO: This currently space fills only from top to bottom, make it able
        //      to space fill in any of the four directions
        switch (last_move) {
        case UP: {
            if (num_empty_spaces_up >= num_empty_spaces_right && num_empty_spaces_up >= num_empty_spaces_left) {
                return UP;
            } else if (num_empty_spaces_right >= num_empty_spaces_left) {
                return RIGHT;
            } else {
                return LEFT;
            }
            break;
        } 
        case DOWN: {
            if (num_empty_spaces_left >= num_empty_spaces_down && num_empty_spaces_left >= num_empty_spaces_right) {
                return LEFT;
            } else if (num_empty_spaces_right >= num_empty_spaces_down) {
                return RIGHT;
            } else {
                return DOWN;
            }
            break;
        }
        case LEFT: {
            if (num_empty_spaces_up >= num_empty_spaces_left && num_empty_spaces_up >= num_empty_spaces_down) {
                return UP;
            } else if (num_empty_spaces_left >= num_empty_spaces_down) {
                return LEFT;
            } else {
                return DOWN;
            }
        }
        case RIGHT: {
            if (num_empty_spaces_up >= num_empty_spaces_right && num_empty_spaces_up >= num_empty_spaces_down) {
                return UP;
            } else if (num_empty_spaces_right >= num_empty_spaces_down) {
                return RIGHT;
            } else {
                return DOWN;
            }
        }
        default: {
            return last_move;
            break;
        }
        }
    }
}

