#include <vector>
#include <iostream>
#include <assert.h>
#include "AI.h"

using namespace std;

AI::AI(int num_x, int num_y) :
    num_cells_x(num_x),
    num_cells_y(num_y) {
    grid = new bool[num_cells_x * num_cells_y];
    snake_lookup = new bool[num_cells_x * num_cells_y];
}

AI::~AI() {
    delete [] snake_lookup;
    delete [] grid;
}

bool& AI::lookup_at(Coord c) {
    return snake_lookup[c.y * num_cells_x + c.x];
}

bool& AI::grid_at(Coord c) {
    return grid[c.y * num_cells_x + c.x];
}

bool AI::out_of_bounds(Coord c) {
    return (c.x >= num_cells_x) || (c.x < 0) || (c.y >= num_cells_y) || (c.y < 0);
}

int AI::num_empty_spaces(Coord c) {
    int size = 0;
    memcpy(grid, snake_lookup, num_cells_x * num_cells_y);
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
            //if (!out_of_bounds(right) && !already_checked(right) && !detect_collision(right, snake_lookup)) {
            if (!out_of_bounds(right) && !lookup_at(right)) {
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
            //if (!out_of_bounds(left) && !already_checked(left) && !detect_collision(left, snake_lookup)) {
            if (!out_of_bounds(left) && !lookup_at(left)) {
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
            //if (!out_of_bounds(up) && !already_checked(up) && !detect_collision(up, snake_lookup)) {
            if (!out_of_bounds(up) && !lookup_at(up)) {
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
            //if (!out_of_bounds(down) && !already_checked(down) && !detect_collision(down, snake_lookup)) {
            if (!out_of_bounds(down) && !lookup_at(down)) {
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
    snake = list<Coord>(orig_snake);
    assert(snake == orig_snake);
    memset(snake_lookup, 0, num_cells_x * num_cells_y);
    memset(grid, 0, num_cells_x * num_cells_y);
    for (auto& part: snake) {
        lookup_at(part) = true;
        grid_at(part) = true;
    }
    ai_start_time = SDL_GetTicks();
    times_up = false;
    Direction d = search_for_move();
    assert(snake == orig_snake);
    if (d != NONE) {
        return d;
    } else {
        if (times_up) {
            static int i = 0;
            cout << "TIME'S UP: " << i << endl;
            i++;
        }
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
        /*std::string s;
        cout << "num_empty_spaces LEFT : " << num_empty_spaces_left << endl;
        cout << "num_empty_spaces RIGHT: " << num_empty_spaces_right << endl;
        cout << "num_empty_spaces UP   : " << num_empty_spaces_up << endl;
        cout << "num_empty_spaces DOWN : " << num_empty_spaces_down << endl;
        cout << "num_empty_spaces NEXT : " << num_empty_spaces_next << endl;
        std::cin >> s;*/
        switch (last_move) {
        case UP: {
            if (num_empty_spaces_right >= num_empty_spaces_left && num_empty_spaces_right >= num_empty_spaces_up) {
                return RIGHT;
            } else if (num_empty_spaces_left >= num_empty_spaces_up) {
                return LEFT;
            } else {
                return last_move;
            }
            break;
        } 
        case DOWN: {
            if (num_empty_spaces_right >= num_empty_spaces_left && num_empty_spaces_right >= num_empty_spaces_down) {
                return RIGHT;
            } else if (num_empty_spaces_left >= num_empty_spaces_down) {
                return LEFT;
            } else {
                return last_move;
            }
            break;
        }
        case LEFT: {
            if (num_empty_spaces_up >= num_empty_spaces_down && num_empty_spaces_up >= num_empty_spaces_left) {
                return UP;
            } else if (num_empty_spaces_down >= num_empty_spaces_left) {
                return DOWN;
            } else {
                return last_move;
            }
        }
        case RIGHT: {
            if (num_empty_spaces_up >= num_empty_spaces_down && num_empty_spaces_up >= num_empty_spaces_right) {
                return UP;
            } else if (num_empty_spaces_down >= num_empty_spaces_right) {
                return DOWN;
            } else {
                return last_move;
            }
        }
        default: {
            return last_move;
            break;
        }
        }
    }
}
