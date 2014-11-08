#include <iostream>
#include <fstream>

#include "../../Empirical/tools/config.h"
#include "../../Empirical/tools/callbacks.h"
#include "../../Empirical/geometry/Body2D.h"
#include "../../Empirical/geometry/Physics2D.h"

#include "control.h"

int main() {
  std::cout << "Welcome to E.voke!" << std::endl;

  emp::cConfig config;
  std::cout << config.RANDOM_SEED() << std::endl;
  config.RANDOM_SEED(40);
  std::cout << config.RANDOM_SEED() << std::endl;

  emp::Physics2D< emp::CircleBody2D< OrgControl, int >, OrgControl, int > physics(1000, 1000);

  auto new_org = new emp::CircleBody2D<OrgControl, int>(emp::Circle<int>(emp::Point<int>(123,456), 10), NULL);
  physics.AddActiveBody(new_org);
}
