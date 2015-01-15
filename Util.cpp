#include "Util.h"

using std::list;

bool detect_collision(Coord c, const list<Coord>& s) {
    for (auto & part : s) {
        if (part == c) {
            return true;
        }
    }
    return false;
}

bool is_out_of_bounds(Coord c, int num_cells_x, int num_cells_y) {
    return (c.x >= num_cells_x) || (c.x < 0) || (c.y >= num_cells_y) || (c.y < 0);
}

