#include <emscripten.h>

#include "../defs.h"

#include "emtools/JSWrap.h"
#include "geometry/Body2D.h"
#include "geometry/Physics2D.h"

#include "UI/Animate.h"
#include "UI/canvas_utils.h"
#include "UI/keypress.h"
#include "UI/UI.h"

#include "../main/World.h"
#include "../organisms/OrgControl.h"

namespace UI = emp::UI;

class EvokeInterface {
private:
  evoke::World world;
  UI::Document doc;
  UI::Animate anim;

  UI::KeypressManager keypress_manager;

public:
  EvokeInterface()
    : doc("emp_base")
    , anim([this](){ EvokeInterface::Animate(anim); })
  {    
    // Link keypresses to the proper handlers
    keypress_manager.AddKeydownCallback(std::bind(&EvokeInterface::OnKeydown, this, _1));

    // Add a canvas to draw the world.
    doc << UI::Canvas(world.width, world.height, "pop_view") << "<br>";

    // Add buttons.
    doc.AddButton([this](){DoPlay();}, "Play", "play_but");
    doc.AddButton([this](){DoStep();}, "Step", "step_but");
    
    doc.Update();

    // Initialize an organism in the middle of the world.
    const evoke::dPoint mid_point( world.width / 2.0, world.height / 2.0 );
    const int org_radius = 6;
    auto org = new evoke::dBody(evoke::dCircle(mid_point, org_radius), NULL);
    world.physics.AddBody(org);


    // Draw initial state of the world.
    UI::Draw( doc.Canvas("pop_view"),
              world.physics.GetSurface(),
              emp::GetHueMap(360));
  }

  ~EvokeInterface() { ; }

  void Animate(const UI::Animate & anim) {
    // Take a single timestep on the world physics.
    world.physics.Update();

    // Test which organisms should replicate.
    double repro_prob = 0.003;

    auto & body_set = world.physics.GetBodySet();

    for (auto * body : body_set) {
      // Bodies that are already reproducing or under pressure cannot produce offspring.
      if (body->IsReproducing() || body->GetPressure() > 1.0) continue;

      if (world.random.P(repro_prob) || body_set.size() == 1) {
        emp::Angle repro_angle(world.random.GetDouble(2.0 * emp::PI));
        auto * new_body = body->BuildOffspring( repro_angle.GetPoint(0.1) );

        // For the moment, assume 95% chance of faithful copy.
        if (world.random.P(0.95)) new_body->SetColorID(body->GetColorID());
        else new_body->SetColorID( world.random.GetInt(360) );

        world.physics.AddBody( new_body );
      }
    }

    UI::Draw( doc.Canvas("pop_view"),
              world.physics.GetSurface(),
              emp::GetHueMap(360));

  }

  void DoPlay() {
    anim.ToggleActive();
    auto & play_but = doc.Button("play_but");
    auto & step_but = doc.Button("step_but");

    if (anim.GetActive()) {
      play_but.Label("Pause");    // If animation is running, button should read "Pause"
      step_but.Disabled(true);    // Cannot step animation already running.
    }
    else {
      play_but.Label("Play");     // If animation is paused, button should read "Play"
      step_but.Disabled(false);    // Can step paused animation.
    }

    play_but.Update();
    step_but.Update();
  }

  void DoStep() {
    // NOTE: Step is only meaningful if the run is paused.
    emp_assert(anim.GetActive() == false);

    world.physics.Update();
    doc.Canvas("pop_view").Refresh();
  }


  bool OnKeydown(const emp::html5::KeyboardEvent & evt_info) {
    const int key_code = evt_info.keyCode;
    bool return_value = true;
    
    switch (key_code) {
    case 'p':                                     // P => Play / Pause
    case 'P':
      DoPlay();
      break;
    case 's':                                     // S => Step
    case 'S':
      DoStep();
      break;

    // case 187:                                     // Plus  (grow)
    //   if (user_body) user_body->SetTargetRadius(user_body->GetTargetRadius() + 1);
    //   break;
    // case 189:                                     // Minus (shrink)
    //   if (user_body) {
    //     int body_size = user_body->GetTargetRadius();
    //     if (body_size > 1) user_body->SetTargetRadius(body_size - 1);
    //   }
    //   break;
    // case 37:                                      // LEFT ARROW (Turn Left)
    //   if (user_body) user_body->TurnLeft();
    //   break;
    // case 38:                                      // UP ARROW (Accellerate)
    //   if (user_body) user_body->IncSpeed();
    //   break;
    // case 39:                                      // RIGHT ARROW (Turn Right)
    //   if (user_body) user_body->TurnRight();
    //   break;
    // case 40:                                      // DOWN ARROW (Breaks)
    //   if (user_body) user_body->DecSpeed();
    //   break;

    default:
      return_value = false;
    };
    
    return return_value;
  }

};

EvokeInterface * evoke_interface;

extern "C" int main()
{  
  UI::Initialize();
  evoke_interface = new EvokeInterface();

  return 0;
}
