#ifndef AI_H
#define AI_H

#include <list>
#include "Util.h"

class AI {
public:
  virtual ~AI() { }
  virtual Direction move(Coord orig_fruit, Direction last_move,
                         const std::list<Coord> &orig_snake,
                         int snake_growing) = 0;
};

#endif
