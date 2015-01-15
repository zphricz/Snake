#include <vector>
#include "AI.h"

using std::list;
using std::vector;

AI::AI(int num_x, int num_y) :
    num_cells_x(num_x),
    num_cells_y(num_y) {
    grid = new bool[num_cells_x * num_cells_y];
}

AI::~AI() {
    delete [] grid;
}

bool AI::already_checked(Coord c) {
    bool rval = grid[c.y * num_cells_x + c.x];
    grid[c.y * num_cells_x + c.x] = true;
    return rval;
}

bool AI::out_of_bounds(Coord c) {
    return is_out_of_bounds(c, num_cells_x, num_cells_y);
}

int AI::enclosed(Coord c, const list<Coord>& s) {
    int size = 0;
    memset(grid, 0, num_cells_x * num_cells_y);
    for (auto& part : s) {
        grid[num_cells_x * part.y + part.x] = true;
    }
    vector<Coord> v;
    v.push_back(c);
    while (!v.empty()) {
        size++;
        Coord new_c = v.back();
        v.pop_back();
        grid[new_c.y * num_cells_x + new_c.x] = true;
        Coord c1{new_c.x - 1, new_c.y};
        Coord c2{new_c.x + 1, new_c.y};
        Coord c3{new_c.x, new_c.y - 1};
        Coord c4{new_c.x, new_c.y + 1};
        if (!out_of_bounds(c1) && !grid[c1.y * num_cells_x + c1.x]) {
            v.push_back(c1);
        }
        if (!out_of_bounds(c2) && !grid[c2.y * num_cells_x + c2.x]) {
            v.push_back(c2);
        }
        if (!out_of_bounds(c3) && !grid[c3.y * num_cells_x + c3.x]) {
            v.push_back(c3);
        }
        if (!out_of_bounds(c4) && !grid[c4.y * num_cells_x + c4.x]) {
            v.push_back(c4);
        }
    }
    return size;
}

Direction AI::search_for_move(Coord fruit, const list<Coord>& s) {
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
    if (SDL_GetTicks() - ai_start_time >= ai_turn_time) {
        times_up = true;
        return NONE;
    }
    auto new_snake = list<Coord>(s);
    new_snake.pop_back();
    for (int i = 0; i < 4; ++i) {
        if (times_up) {
            return NONE;
        }
        switch (dirs[i]) {
        case RIGHT: {
            if (!out_of_bounds(right) && !already_checked(right) && !detect_collision(right, new_snake)) {
                new_snake.push_front(right);
                if (search_for_move(fruit, new_snake) != NONE) {
                    return RIGHT;
                }
                new_snake.pop_front();
            }
            break;
        }
        case LEFT: {
            if (!out_of_bounds(left) && !already_checked(left) && !detect_collision(left, new_snake)) {
                new_snake.push_front(left);
                if (search_for_move(fruit, new_snake) != NONE) {
                    return LEFT;
                }
                new_snake.pop_front();
            }
            break;
        }
        case UP: {
            if (!out_of_bounds(up) && !already_checked(up) && !detect_collision(up, new_snake)) {
                new_snake.push_front(up);
                if (search_for_move(fruit, new_snake) != NONE) {
                    return UP;
                }
                new_snake.pop_front();
            }
            break;
        }
        case DOWN: {
            if (!out_of_bounds(down) && !already_checked(down) && !detect_collision(down, new_snake)) {
                new_snake.push_front(down);
                if (search_for_move(fruit, new_snake) != NONE) {
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

Direction AI::move(Coord fruit, Direction last_move, const list<Coord>& snake) {
    // AI fails to take into account that the snake can be growing
    // TODO: Fix this
    ai_start_time = SDL_GetTicks();
    times_up = false;
    memset(grid, 0, num_cells_x * num_cells_y);
    Direction d = search_for_move(fruit, snake);
    if (d != NONE) {
        return d;
    } else {
        auto new_snake = list<Coord>(snake);
        new_snake.pop_back();
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
        int enclosed_left = 0;
        int enclosed_up = 0;
        int enclosed_down = 0;
        int enclosed_right = 0;
        int enclosed_next = 0;
        if (!out_of_bounds(left) && !detect_collision(left, new_snake)) {
            enclosed_left = enclosed(left, new_snake);
        }
        if (!out_of_bounds(right) && !detect_collision(right, new_snake)) {
            enclosed_right = enclosed(right, new_snake);
        }
        if (!out_of_bounds(down) && !detect_collision(down, new_snake)) {
            enclosed_down = enclosed(down, new_snake);
        }
        if (!out_of_bounds(up) && !detect_collision(up, new_snake)) {
            enclosed_up = enclosed(up, new_snake);
        }
        if (!out_of_bounds(next) && !detect_collision(next, new_snake)) {
            enclosed_next = enclosed(next, new_snake);
        }
        if (last_move == UP || last_move == DOWN) {
            if (enclosed_right >= enclosed_left && enclosed_right >= enclosed_next) {
                return RIGHT;
            } else if (enclosed_left >= enclosed_next) {
                return LEFT;
            } else {
                return last_move;
            }
        } else {
            if (enclosed_up >= enclosed_down && enclosed_up >= enclosed_next) {
                return UP;
            } else if (enclosed_down >= enclosed_next) {
                return DOWN;
            } else {
                return last_move;
            }
        }
    }
}
