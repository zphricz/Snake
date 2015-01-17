#ifndef UTIL_H
#define UTIL_H

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

#endif
