#include <emscripten.h>

#include "../defs.h"

#include "emtools/JSWrap.h"
#include "geometry/Body2D.h"
#include "geometry/Physics2D.h"

#include "web/Animate.h"
#include "web/canvas_utils.h"
#include "web/keypress.h"
#include "web/web.h"

#include "../main/World.h"
#include "../organisms/OrgControl.h"

namespace UI = emp::web;

class EvokeInterface {
private:
  evoke::World world;  // Everything unrelated to the interface should be in the world.

  UI::Document doc;
  UI::Animate anim;
  UI::KeypressManager keypress_manager;

  enum class MapMode { BLANK, STATIC, BASIC } map_mode;
  
public:
  EvokeInterface()
    : doc("emp_base")
    , anim([this](){ EvokeInterface::Animate(anim); })
    , map_mode(MapMode::BASIC)
  {    
    // Link keypresses to the proper handlers
    keypress_manager.AddKeydownCallback(std::bind(&EvokeInterface::OnKeydown, this, _1));

    // Add a welcome.
    doc << "<h1>Welcome to Evoke::Avida!</h1>";

    // Add a canvas to draw the world.
    doc << UI::Canvas(world.width, world.height, "pop_view").SetPosition(10, 60) << "<br>";

    // Add buttons.
    auto button_set = doc.AddSlate("buttons");
    button_set.SetPosition(10, 70+world.height);
    button_set << UI::Button([this](){DoPlay();}, "Play", "play_but");
    button_set << UI::Button([this](){DoStep();}, "Step", "step_but");
    button_set << UI::Button([this](){DoReset();}, "Reset", "reset_but");
    button_set << UI::Button([this](){map_mode = MapMode::BLANK;}, "Blank", "blank_but");
    button_set << UI::Button([this](){map_mode = MapMode::BASIC;}, "Basic", "basic_but");
    
    // And stats (next o canvas)
    auto stats_set = doc.AddSlate("stats");
    stats_set.SetPosition(world.width+40, 60);
    auto & body_set = world.physics.GetBodySet();

    stats_set << "Update: " << UI::Live( [this]() { return anim.GetFrameCount(); } ) << "<br>";
    stats_set << "Org Count: " << UI::Live( [&body_set](){ return body_set.size(); } );

    world.Init(); // Sartup the world.    

    // Draw initial state of the world.
    UI::Draw( doc.Canvas("pop_view"),
              world.physics.GetSurface(),
              emp::GetHueMap(360));
  }

  ~EvokeInterface() { ; }

  void Animate(const UI::Animate & anim) {
    world.Update();

    switch (map_mode) {
    case MapMode::BLANK:
    case MapMode::STATIC:
      break;
    case MapMode::BASIC: 
      UI::Draw( doc.Canvas("pop_view"),
                world.physics.GetSurface(),
                emp::GetHueMap(360));
      break;
    }

    doc.Slate("stats").Redraw();
  }

  void DoPlay() {
    anim.ToggleActive();
    auto play_but = doc.Button("play_but");
    auto step_but = doc.Button("step_but");

    if (anim.GetActive()) {
      play_but.Label("Pause");    // If animation is running, button should read "Pause"
      step_but.Disabled(true);    // Cannot step animation already running.
    }
    else {
      play_but.Label("Play");     // If animation is paused, button should read "Play"
      step_but.Disabled(false);    // Can step paused animation.
    }
  }

  void DoStep() {
    emp_assert(anim.GetActive() == false); // Step is only meaningful if the run is paused.
    anim.Step();
  }

  void DoReset() {
    // Clear everything currently in the world.
    world.physics.Clear();

    // Initialize a new organism in the middle of the world.
    const evoke::dPoint mid_point( world.width / 2.0, world.height / 2.0 );
    const int org_radius = 6;
    auto org = new evoke::dBody(evoke::dCircle(mid_point, org_radius), NULL);
    world.physics.AddBody(org);

    // Redraw the world.
    UI::Draw( doc.Canvas("pop_view"),
              world.physics.GetSurface(),
              emp::GetHueMap(360));
  }


  bool OnKeydown(const emp::html5::KeyboardEvent & evt_info) {
    const int key_code = evt_info.keyCode;
    bool return_value = true;
    auto * user_body = world.physics.GetBodySet()[0];
    
    switch (key_code) {
    case 'B':                                     // B => Blank
      map_mode = MapMode::BLANK;
      break;
    case 'P':                                     // P => Play / Pause
      DoPlay();
      break;
    case 'R':                                     // R => Reset population
      DoReset();
      break;
    case 'S':                                     // S => Step
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

    case 37:                                      // LEFT ARROW (Turn Left)
      if (user_body) user_body->TurnLeft();
      break;
    case 38:                                      // UP ARROW (Accellerate)
      if (user_body) user_body->IncSpeed();
      break;
    case 39:                                      // RIGHT ARROW (Turn Right)
      if (user_body) user_body->TurnRight();
      break;
    case 40:                                      // DOWN ARROW (Breaks)
      if (user_body) user_body->DecSpeed();
      break;

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
