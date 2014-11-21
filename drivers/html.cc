#include <emscripten.h>

#include "tools/config.h"
#include "tools/callbacks.h"
#include "kinetic/Kinetic.h"
#include "geometry/Body2D.h"
#include "geometry/Physics2D.h"
#include "../organisms/OrgControl.h"
#include "../web/Viewport.h"

typedef double dEvokeBase;
typedef evoke::OrgControl dEvokeControl;
typedef emp::CircleBody2D< dEvokeControl, dEvokeBase > dEvokeBody;

class EvokeInterface {
private:
  emp::Stage stage;
  emp::Layer layer_anim;
  emp::cConfig config;

  emp::Physics2D<dEvokeBody, dEvokeControl, dEvokeBase> physics;
  evoke::Viewport<dEvokeBody, dEvokeControl, dEvokeBase> viewport;

  emp::Animation<EvokeInterface> anim_interface;
public:
  EvokeInterface()
    : stage(1200, 600, "container")
    , physics(600, 600)
    , viewport(0, 0, 600, 600, physics)
  {
    
    auto org1 = new dEvokeBody(emp::Circle<dEvokeBase>(emp::Point<dEvokeBase>(123,456), 100), NULL);
    physics.AddBody(org1);
    auto org2 = new dEvokeBody(emp::Circle<dEvokeBase>(emp::Point<dEvokeBase>(423,456), 100), NULL);
    physics.AddBody(org2);
    auto org3 = new dEvokeBody(emp::Circle<dEvokeBase>(emp::Point<dEvokeBase>(300,300), 50), NULL);
    org1->SetVelocity(7,3);
    physics.AddBody(org3);

    const int min_stage_size = 500;
    stage.ResizeMax(min_stage_size, min_stage_size);

    layer_anim.Add(viewport);
    stage.Add(layer_anim);

    anim_interface.Setup(this, &EvokeInterface::Animate, layer_anim);
    anim_interface.Start();
  }

  ~EvokeInterface() { ; }

  void Animate(const emp::AnimationFrame & frame) {
    physics.Update();
    std::cout << frame.time << std::endl;
  }
};

EvokeInterface * evoke_interface;

extern "C" int evokeMain()
{
  evoke_interface = new EvokeInterface();

  return 0;
}
