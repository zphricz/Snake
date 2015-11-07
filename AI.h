#ifndef AI_H
#define AI_H

#include <deque>
#include "Util.h"

class AI {
public:
  virtual ~AI() {}
  virtual Direction move(Coord fruit, Direction last_move,
                         const std::deque<Coord> &snake, int snake_growing) = 0;
};

#endif
