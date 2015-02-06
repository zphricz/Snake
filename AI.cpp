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

void AI::search_for_move(Uint32 depth) {
    // TODO: The snake will always attempt to get a fruit, even it it assures
    //       that the snake dies. Fix this
    Coord c = snake.front();
    if (c == fruit) {
        chosen_direction = first_move;
        chosen_depth = depth;
        return;
    }
    if (SDL_GetTicks() - ai_start_time >= ai_turn_time) {
        times_up = true;
        return;
    }
    Coord right{c.x + 1, c.y};
    Coord left{c.x - 1, c.y};
    Coord up{c.x, c.y - 1};
    Coord down{c.x, c.y + 1};
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
        if (times_up || depth + 1 >= chosen_depth) {
            break;
        }
        switch (dirs[i]) {
        case RIGHT: {
            if (!out_of_bounds(right) && !lookup_at(right)) {
                if (grid_at(right) == MAX_SEARCH) {
                    break;
                }
                if (depth == 0) {
                    first_move = RIGHT;
                }
                grid_at(right)++;
                snake.push_front(right);
                lookup_at(right) = true;
                search_for_move(depth + 1);
                snake.pop_front();
                lookup_at(right) = false;
            }
            break;
        }
        case LEFT: {
            if (!out_of_bounds(left) && !lookup_at(left)) {
                if (grid_at(left) == MAX_SEARCH) {
                    break;
                }
                if (depth == 0) {
                    first_move = LEFT;
                }
                grid_at(left)++;
                snake.push_front(left);
                lookup_at(left) = true;
                search_for_move(depth + 1);
                snake.pop_front();
                lookup_at(left) = false;
            }
            break;
        }
        case UP: {
            if (!out_of_bounds(up) && !lookup_at(up)) {
                if (grid_at(up) == MAX_SEARCH) {
                    break;
                }
                if (depth == 0) {
                    first_move = UP;
                }
                grid_at(up)++;
                snake.push_front(up);
                lookup_at(up) = true;
                search_for_move(depth + 1);
                snake.pop_front();
                lookup_at(up) = false;
            }
            break;
        }
        case DOWN: {
            if (!out_of_bounds(down) && !lookup_at(down)) {
                if (grid_at(down) == MAX_SEARCH) {
                    break;
                }
                if (depth == 0) {
                    first_move = DOWN;
                }
                grid_at(down)++;
                snake.push_front(down);
                lookup_at(down) = true;
                search_for_move(depth + 1);
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
}

Direction AI::move(Coord orig_fruit, Direction last_move, const list<Coord>& orig_snake) {
    // AI fails to take into account that the snake can be growing
    // TODO: Fix this
    fruit = orig_fruit;
    static Direction tack = NONE;
    snake = orig_snake;
    fill(snake_lookup.begin(), snake_lookup.end(), false);
    fill(grid.begin(), grid.end(), false);
    for (auto& part: snake) {
        lookup_at(part) = true;
        grid_at(part) = true;
    }
    times_up = false;
    ai_start_time = SDL_GetTicks();
    chosen_depth = 0xFFFFFFFF;
    chosen_direction = NONE;
    search_for_move(0);
    if (chosen_direction != NONE) {
        tack = NONE;
        return chosen_direction;
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
        if (tack == UP || (tack != DOWN && c.y < num_cells_y / 2)) {
            tack = UP;
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
        } else {
            tack = DOWN;
            switch (last_move) {
            case DOWN: {
                if (num_empty_spaces_down >= num_empty_spaces_right && num_empty_spaces_down >= num_empty_spaces_left) {
                    return DOWN;
                } else if (num_empty_spaces_right >= num_empty_spaces_left) {
                    return RIGHT;
                } else {
                    return LEFT;
                }
                break;
            } 
            case UP: {
                if (num_empty_spaces_left >= num_empty_spaces_up && num_empty_spaces_left >= num_empty_spaces_right) {
                    return LEFT;
                } else if (num_empty_spaces_right >= num_empty_spaces_up) {
                    return RIGHT;
                } else {
                    return UP;
                }
                break;
            }
            case LEFT: {
                if (num_empty_spaces_down >= num_empty_spaces_left && num_empty_spaces_down >= num_empty_spaces_up) {
                    return DOWN;
                } else if (num_empty_spaces_left >= num_empty_spaces_up) {
                    return LEFT;
                } else {
                    return UP;
                }
            }
            case RIGHT: {
                if (num_empty_spaces_down >= num_empty_spaces_right && num_empty_spaces_down >= num_empty_spaces_up) {
                    return DOWN;
                } else if (num_empty_spaces_right >= num_empty_spaces_up) {
                    return RIGHT;
                } else {
                    return UP;
                }
            }
            default: {
                return last_move;
                break;
            }
            }
        }
    }
}

