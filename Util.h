#ifndef UTIL_H
#define UTIL_H

#include <list>

struct Coord {
    int x;
    int y;
    bool operator==(const Coord& rhs) const {
        return x == rhs.x && y == rhs.y;
    }
    bool operator!=(const Coord& rhs) const {
        return !(*this == rhs);
    }
};

typedef enum {UP, DOWN, LEFT, RIGHT, NONE} Direction;

bool detect_collision(Coord c, const std::list<Coord>& s);
bool is_out_of_bounds(Coord c, int num_cells_x, int num_cells_y);

#endif
