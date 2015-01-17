#ifndef UTIL_H
#define UTIL_H

#include <list>
#include <unordered_set>
#include <functional>

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

namespace std {
    template <> struct hash<Coord> {
        size_t operator() (const Coord& c) const {
            return (size_t) ((c.x << 16) | c.y);
        }
    };
}

typedef enum {UP, DOWN, LEFT, RIGHT, NONE} Direction;

#endif
