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
  const dEvokeBase world_x = 512;
  const dEvokeBase world_y = 512;
  emp::Stage stage;
  emp::Layer layer_anim;
  emp::cConfig config;

  emp::Physics2D<dEvokeBody, dEvokeControl, dEvokeBase> physics;
  evoke::Viewport<dEvokeBody, dEvokeControl, dEvokeBase> viewport;

  emp::Animation<EvokeInterface> anim_interface;
public:
  EvokeInterface()
    : stage(world_x, world_y, "container")
    , physics(world_x, world_y)
    , viewport(0, 0, world_x, world_y, physics)
  {    
    emp::MethodCallback<EvokeInterface> * play_cb = new emp::MethodCallback<EvokeInterface>(this, &EvokeInterface::DoPlay);
    emp::MethodCallback<EvokeInterface> * step_cb = new emp::MethodCallback<EvokeInterface>(this, &EvokeInterface::DoStep);
    EM_ASM_ARGS({
        emp_info.callbacks.play_cb = $0;
        emp_info.callbacks.step_cb = $1;
      }, (int) play_cb, (int) step_cb);

    auto org1 = new dEvokeBody(emp::Circle<dEvokeBase>(emp::Point<dEvokeBase>(123,456), 8), NULL);
    physics.AddBody(org1);
    auto org2 = new dEvokeBody(emp::Circle<dEvokeBase>(emp::Point<dEvokeBase>(423,456), 8), NULL);
    physics.AddBody(org2);
    auto org3 = new dEvokeBody(emp::Circle<dEvokeBase>(emp::Point<dEvokeBase>(300,300), 8), NULL);
    physics.AddBody(org3);
    // org1->SetVelocity(7,3);
    // org1->SetTargetRadius(200);

    const int base_radius = 8;
    for (int i = base_radius+1; i < world_x-base_radius-1; i += 2*base_radius) {
      // for (int j = 200; j < 250; j += 2*base_radius + 1) {
      for (int j = 25; j < 250; j += 2*base_radius + 1) {
        auto org = new dEvokeBody(emp::Circle<dEvokeBase>(emp::Point<dEvokeBase>(i,j), base_radius), NULL);
        // org->SetVelocity(0,1);
        physics.AddBody(org);
      }
    }

    // stage.ResizeMax(world_x, world_y);

    layer_anim.Add(viewport);
    stage.Add(layer_anim);

    anim_interface.Setup(this, &EvokeInterface::Animate, layer_anim);
    anim_interface.Start();
  }

  ~EvokeInterface() { ; }

  void Animate(const emp::AnimationFrame & frame) {
    physics.Update();
  }

  void DoPlay() {
    if (anim_interface.IsRunning()) {
      anim_interface.Stop();
      EM_ASM({
          document.getElementById("play_button").innerHTML = "Play";
          document.getElementById("step_button").disabled = false;
        });
    }
    else {
      anim_interface.Start();
      EM_ASM({
          document.getElementById("play_button").innerHTML = "Pause";
          document.getElementById("step_button").disabled = true;
        });
    }
  }
  void DoStep() {
    // Step is only meaningful if the run is paused.
    if (anim_interface.IsRunning() == false) {
      physics.Update();
      layer_anim.Draw();
    }
  }
};

EvokeInterface * evoke_interface;

extern "C" int evokeMain()
{
  evoke_interface = new EvokeInterface();

  return 0;
}
