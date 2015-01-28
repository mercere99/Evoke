#ifndef EVOKE_WORLD_H
#define EVOKE_WORLD_H

#include "tools/config.h"
#include "tools/Random.h"

namespace evoke {
  
  struct World {
    const evoke::dBase world_x = 512;
    const evoke::dBase world_y = 512;

    emp::Config config;
    emp::Physics2D<evoke::dBody, evoke::dControl, evoke::dBase> physics;
    emp::Random random;    

    World() :physics(world_x, world_y) { ; }
  };

};

#endif
