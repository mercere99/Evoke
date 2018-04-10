#include <iostream>
#include <fstream>

#include "../defs.h"

#include "config/config.h"
#include "geometry/Body2D.h"
#include "geometry/Physics2D.h"
#include "tools/functions.h"

#include "../main/World.h"
#include "../organisms/OrgControl.h"

class EvokeInterface {
private:
  evoke::World world;  // Everything unrelated to the interface should be in the world.

public:
  EvokeInterface() { world.Init(); }
  ~EvokeInterface() { ; }

  void Step() { world.Update(); }
};

int main() {
  std::cout << "Welcome to E\\/OKE!" << std::endl;
  
  // emp::Config config;
  // std::cout << config.RANDOM_SEED() << std::endl;
  // config.RANDOM_SEED(40);
  // std::cout << config.RANDOM_SEED() << std::endl;
  // config.WriteMacros("test_config.h");

  EvokeInterface evoke;
  for (int i = 0; i < 10000; i++) {
    std::cout << "Update: " << i << std::endl;
    evoke.Step();
  }

  return 0;
}


