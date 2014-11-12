#include <emscripten.h>

#include "tools/config.h"
#include "tools/callbacks.h"
#include "kinetic/Kinetic.h"
#include "geometry/Body2D.h"
#include "geometry/Physics2D.h"
#include "../web/Viewport.h"

class EvokeInterface {
private:
  evoke::Viewport viewport;
  emp::Layer layer_anim;
  emp::Stage stage;
  emp::cConfig config;

public:
  EvokeInterface()
    : viewport(0, 0, 300, 300)
    , stage(1200, 600, "container")
  {
    const int min_stage_size = 300;
    stage.ResizeMax(min_stage_size, min_stage_size);

    layer_anim.Add(viewport);
    stage.Add(layer_anim);
  }

  ~EvokeInterface() { ; }
};

// @CAO Move elsewhere!
class OrgControl {
private:
public:
  OrgControl() { ; }
  ~OrgControl() { ; }
};

EvokeInterface * evoke_interface;

extern "C" int evokeMain()
{
  emp::Physics2D< emp::CircleBody2D< OrgControl, int >, OrgControl, int > physics(1000, 1000);

  auto new_org = new emp::CircleBody2D<OrgControl, int>(emp::Circle<int>(emp::Point<int>(123,456), 10), NULL);
  physics.AddActiveBody(new_org);

  evoke_interface = new EvokeInterface();

  return 0;
}
