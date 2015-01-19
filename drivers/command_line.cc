#include <iostream>
#include <fstream>

#include "tools/config.h"
#include "tools/callbacks.h"
#include "geometry/Body2D.h"
#include "geometry/Physics2D.h"
#include "tools/functions.h"
#include "../organisms/OrgControl.h"

// #include "control.h"

typedef double dEvokeBase;
typedef evoke::OrgControl dEvokeControl;
typedef emp::CircleBody2D< dEvokeControl, dEvokeBase > dEvokeBody;

class EvokeInterface {
private:
  const dEvokeBase world_x = 512;
  const dEvokeBase world_y = 512;
  emp::Config config;

  emp::Physics2D<dEvokeBody, dEvokeControl, dEvokeBase> physics;
public:
  EvokeInterface()
    : physics(world_x, world_y)
  {    
    auto org1 = new dEvokeBody(emp::Circle<dEvokeBase>(emp::Point<dEvokeBase>(123,456), 8), NULL);
    physics.AddBody(org1);
    auto org2 = new dEvokeBody(emp::Circle<dEvokeBase>(emp::Point<dEvokeBase>(423,456), 8), NULL);
    physics.AddBody(org2);
    auto org3 = new dEvokeBody(emp::Circle<dEvokeBase>(emp::Point<dEvokeBase>(300,300), 8), NULL);
    physics.AddBody(org3);
    org1->SetVelocity(5,3);

    const int base_radius = 3;
    for (int i = base_radius+1; i < world_x-base_radius-1; i += 2*base_radius) {
      for (int j = 50; j < 200; j += 2*base_radius + 1) {
        auto org = new dEvokeBody(emp::Circle<dEvokeBase>(emp::Point<dEvokeBase>(i,j), base_radius), NULL);
        org->SetVelocity(0,1);
        physics.AddBody(org);
      }
    }
  }

  ~EvokeInterface() { ; }

  void Step() {
    physics.Update();
  }
};

int main() {
  std::cout << "Welcome to E\\/OKE!" << std::endl;

  // emp::Config config;
  // std::cout << config.RANDOM_SEED() << std::endl;
  // config.RANDOM_SEED(40);
  // std::cout << config.RANDOM_SEED() << std::endl;
  // config.WriteMacros("test_config.h");

  EvokeInterface evoke;
  for (int i = 0; i < 1000; i++) {
    std::cout << "Update: " << i << std::endl;
    evoke.Step();
  }
}


