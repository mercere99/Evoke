#include <emscripten.h>

#include "kinetic/Kinetic.h"
#include "../web/Viewport.h"

class EvokeInterface {
private:
  evoke::Viewport viewport;
  emp::Layer layer_anim;
  emp::Stage stage;

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


EvokeInterface * evoke_interface;

extern "C" int evokeMain()
{
  evoke_interface = new EvokeInterface();

  return 0;
}
