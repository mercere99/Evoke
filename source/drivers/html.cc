#include <emscripten.h>

#include "../defs.h"

#include "tools/callbacks.h"
#include "kinetic/Kinetic.h"
#include "emtools/keypress.h"
#include "geometry/Body2D.h"
#include "geometry/Physics2D.h"

#include "../main/World.h"
#include "../organisms/OrgControl.h"
#include "../webtools/Viewport.h"

#include "UI/UI.h"

namespace evoke {
  typedef Viewport<dBody, dControl, dBase> dViewport;
}

class EvokeInterface {
private:
  evoke::World world;
  emp::Kinetic::Stage stage;
  emp::Kinetic::Layer layer_anim;
  evoke::dViewport viewport;

  emp::Kinetic::Animation<EvokeInterface> anim_interface;
  emp::KeypressManager keypress_manager;
public:
  EvokeInterface()
    : stage(world.width, world.height, "container")
    , viewport(0, 0, world)
  {    
    // Link keypresses to the proper handlers
    keypress_manager.AddKeydownCallback(std::bind(&evoke::dViewport::OnKeydown, &viewport, _1));
    keypress_manager.AddKeydownCallback(std::bind(&EvokeInterface::OnKeydown, this, _1));

    // Link button callbacks to the proper handlers.
    auto play_cb = new emp::MethodCallback<EvokeInterface>(this, &EvokeInterface::DoPlay);
    auto step_cb = new emp::MethodCallback<EvokeInterface>(this, &EvokeInterface::DoStep);
    auto repro_cb = new emp::MethodCallback<EvokeInterface>(this, &EvokeInterface::DoRepro);
    EM_ASM_ARGS({
        emp_kinetic.callbacks.play_cb = $0;
        emp_kinetic.callbacks.step_cb = $1;
        emp_kinetic.callbacks.repro_cb = $2;
      }, (int) play_cb, (int) step_cb, (int) repro_cb);


    // Initialize organisms in the world.
    // const int base_radius = 4;
    const int base_radius = 6;

    auto org = new evoke::dBody(evoke::dCircle(evoke::dPoint(world.width/2,world.height/2), base_radius), NULL);
    world.physics.AddBody(org);

    // auto org1 = new evoke::dBody(evoke::dCircle(evoke::dPoint(123,456), base_radius), NULL);
    // world.physics.AddBody(org1);
    // auto org2 = new evoke::dBody(evoke::dCircle(evoke::dPoint(423,456), base_radius), NULL);
    // world.physics.AddBody(org2);
    // auto org3 = new evoke::dBody(evoke::dCircle(evoke::dPoint(300,300), base_radius), NULL);
    // world.physics.AddBody(org3);
 
    layer_anim.Add(viewport);
    stage.Add(layer_anim);

    anim_interface.Setup(this, &EvokeInterface::Animate, layer_anim);
    anim_interface.Start();
  }

  ~EvokeInterface() { ; }

  void Animate(const emp::Kinetic::AnimationFrame & frame) {
    world.physics.Update();

    // Test which organisms should replicate.
    const double avg_repro_time = 10.0; // In seconds
    double repro_prob = ((double) frame.time_diff) / (avg_repro_time * 1000.0);

    repro_prob = 0.003;

    auto & body_set = world.physics.GetBodySet();
    if (body_set.size())

    for (auto * body : body_set) {
      // Bodies that are reproducing cannot produce a second offspring until they are done.
      // Bodies under pressure do not produce offspring.
//      if (body->IsReproducing() || body->GetPressure() > 0.0) continue;
      if (body->IsReproducing() || body->GetPressure() > 1.0) continue;
      if (world.random.P(repro_prob) || body_set.size() == 1) {
        emp::Angle repro_angle(world.random.GetDouble(2.0 * emp::PI));
        auto * new_body = body->BuildOffspring( repro_angle.GetPoint(0.1) );
        if (world.random.P(0.95)) {
          new_body->SetColorID(body->GetColorID());
        }
        else {
          new_body->SetColorID( world.random.GetInt(360) );
        }
        world.physics.AddBody( new_body );
      }
    }

  }

  void DoPlay() {
    if (anim_interface.IsRunning()) {
      anim_interface.Stop();
      EM_ASM({
          document.getElementById("play_button").innerHTML = "[P]lay";
          document.getElementById("step_button").disabled = false;
        });
    }
    else {
      anim_interface.Start();
      EM_ASM({
          document.getElementById("play_button").innerHTML = "[P]ause";
          document.getElementById("step_button").disabled = true;
        });
    }
  }
  void DoStep() {
    // Step is only meaningful if the run is paused.
    if (anim_interface.IsRunning() == false) {
      world.physics.Update();
      layer_anim.Draw();
    }
  }

  void DoRepro() {
    evoke::dBody * body = viewport.GetUserBody();
    if (!body) return;
    emp::Angle repro_angle(world.random.GetDouble(2.0 * emp::PI));
    world.physics.AddBody( body->BuildOffspring( repro_angle.GetPoint(0.01) ) );
  }

  bool OnKeydown(const emp::EventInfo & evt_info) {
    const int key_code = evt_info.key_code;
    bool return_value = true;
    
    switch (key_code) {
    case 'p':                                     // P => Play / Pause
    case 'P':
      DoPlay();
      break;
    case 'r':                                     // R => Reproduce
    case 'R':
      DoRepro();
      break;
    case 's':                                     // S => Step
    case 'S':
      DoStep();
      break;
    default:
      return_value = false;
    };

    return return_value;
  }

};

EvokeInterface * evoke_interface;

// extern "C" int evokeMain()
extern "C" int main()
{
  EMP_SETUP();
  evoke_interface = new EvokeInterface();

  return 0;
}
