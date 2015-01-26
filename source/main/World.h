#ifndef EVOKE_WORLD_H
#define EVOKE_WORLD_H

#include "tools/config.h"
#include "tools/Random.h"

namespace evoke {
  
  struct World {
    emp::Config config;
    emp::Random random;    
  };

};

#endif
