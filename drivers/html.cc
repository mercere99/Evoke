#include <emscripten.h>

#include "tools/config.h"
#include "tools/callbacks.h"
#include "kinetic/Kinetic.h"
#include "geometry/Body2D.h"
#include "geometry/Physics2D.h"
#include "../organisms/OrgControl.h"
#include "../web/Viewport.h"

typedef int dEvokeBase;
typedef evoke::OrgControl dEvokeControl;
typedef emp::CircleBody2D< dEvokeControl, dEvokeBase > dEvokeBody;

class EvokeInterface {
private:
  emp::Stage stage;
  emp::Layer layer_anim;
  emp::cConfig config;

  emp::Physics2D<dEvokeBody, dEvokeControl, dEvokeBase> physics;
  evoke::Viewport<dEvokeBody, dEvokeControl, dEvokeBase> viewport;
public:
  EvokeInterface()
    : stage(1200, 600, "container")
    , physics(1000, 1000)
    , viewport(0, 0, 300, 300, physics)
  {
    
    auto new_org = new dEvokeBody(emp::Circle<dEvokeBase>(emp::Point<dEvokeBase>(123,456), 10), NULL);
    physics.AddActiveBody(new_org);


    const int min_stage_size = 300;
    stage.ResizeMax(min_stage_size, min_stage_size);

    layer_anim.Add(viewport);
    stage.Add(layer_anim);
  }

  ~EvokeInterface() { ; }
};

EvokeInterface * evoke_interface;

extern "C" int evokeMain()
{
  evoke_interface = new EvokeInterface();

  return 0;
}
