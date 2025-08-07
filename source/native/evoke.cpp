#include <iostream>
#include <fstream>

#include "../defs.h"

#include "emp/config/config.hpp"
#include "emp/geometry/Physics2D.hpp"

#include "../main/World.hpp"
#include "../organisms/OrgControl.hpp"

class EvokeInterface {
private:
  evoke::World world;  // Everything unrelated to the interface should be in the world.

public:
  EvokeInterface() : world() {
    world.org_radius=3.0;
    world.drift = 0.0;
    world.repro_prob=0.01;
    world.Init();
  }
  ~EvokeInterface() {}

  size_t GetNumOrgs() const { return world.physics.NumBodies(); }
  void Step() { world.Update(); }
};

int main() {
  DEBUG_STACK();
  std::cout << "Welcome to E\\/OKE!" << std::endl;
  
  // emp::Config config;
  // std::cout << config.RANDOM_SEED() << std::endl;
  // config.RANDOM_SEED(40);
  // std::cout << config.RANDOM_SEED() << std::endl;
  // config.WriteMacros("test_config.h");

  EvokeInterface evoke;
  for (int i = 0; i < 10000; i++) {
    std::cout << "Update: " << i
      << "  NumOrgs: " << evoke.GetNumOrgs()
      << std::endl;
    evoke.Step();
  }

  return 0;
}


