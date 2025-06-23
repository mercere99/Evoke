#include <iostream>
#include <fstream>

#include "../defs.h"

#include "emp/config/config.hpp"
#include "emp/geometry/Body2D.hpp"
#include "emp/geometry/Physics2D.hpp"
// #include "emp/tools/functions.hpp"

#include "../main/World.h"
#include "../organisms/OrgControl.h"

class EvokeInterface {
private:
  evoke::World world;  // Everything unrelated to the interface should be in the world.

public:
  EvokeInterface() : world() { world.Init(); }
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


