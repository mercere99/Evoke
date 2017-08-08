#include <emscripten.h>

#include "../defs.h"

#include "web/JSWrap.h"
#include "geometry/Body2D.h"
#include "geometry/Physics2D.h"

#include "web/Animate.h"
#include "web/canvas_utils.h"
#include "web/KeypressManager.h"
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

  enum class MapMode { BLANK, MAKE_BLANK, BASIC } map_mode;
  
public:
  EvokeInterface()
    : doc("emp_base")
    , anim([this](){ EvokeInterface::Animate(anim); })
    , map_mode(MapMode::BASIC)
  {    
    // Link keypresses to the proper handlers
    keypress_manager.AddKeydownCallback(std::bind(&EvokeInterface::OnKeydown, this, _1));

    // Add a welcome.
    doc << "<h1>Welcome to Evoke!</h1>";

    // Add a canvas to draw the world.
    doc << UI::Canvas(world.width, world.height, "pop_view").SetPosition(10, 60) << "<br>";

    // Add buttons.
    auto control_set = doc.AddDiv("buttons");
    control_set.SetPosition(10, 70+world.height);
    control_set << UI::Button([this](){DoStart();}, "Start", "start_but");
    control_set << UI::Button([this](){DoStep();}, "Step", "step_but");
    control_set << UI::Button([this](){DoReset();}, "Reset", "reset_but");

    UI::Selector map_sel("map_sel");
    map_sel.SetOption("Basic Map",            [this](){map_mode = MapMode::BASIC;} );
    map_sel.SetOption("Blank Map (fast!)", [this](){map_mode = MapMode::MAKE_BLANK;} );
    control_set << map_sel;

    UI::Selector mode_sel("mode_sel");
    mode_sel.SetOption("Individual Orgs",
                       [this](){world.physics.SetDetach(true); world.max_link_count=10; } );
    mode_sel.SetOption("Snowflake Clusters",
                       [this](){world.physics.SetDetach(false); world.max_link_count=3; } );
    // mode_sel.SetOption("Aggregative Clusters",
    //                    [this](){world.physics.SetDetach(false); world.max_link_count=10; } );
    control_set << mode_sel;

    control_set << "<br>";
    
    UI::Selector size_sel("size_sel");
    size_sel.SetOption("Cell Size 3",  [this](){world.org_radius=3.0;} );
    size_sel.SetOption("Cell Size 4",  [this](){world.org_radius=4.0;} );
    size_sel.SetOption("Cell Size 5",  [this](){world.org_radius=5.0;} );
    size_sel.SetOption("Cell Size 6",  [this](){world.org_radius=6.0;} );
    size_sel.SetOption("Cell Size 8",  [this](){world.org_radius=8.0;} );
    size_sel.SetOption("Cell Size 10", [this](){world.org_radius=10.0;} );
    size_sel.SelectID(1);
    control_set << size_sel;

    UI::Selector drift_sel("drift_sel");
    drift_sel.SetOption("Flow Off",  [this](){world.drift=0.0;} );
    drift_sel.SetOption("Flow Low",  [this](){world.drift=0.05;} );
    drift_sel.SetOption("Flow High", [this](){world.drift=0.15;} );
    drift_sel.SelectID(2);
    control_set << drift_sel;

    UI::Selector repro_sel("repro_sel");
    repro_sel.SetOption("Copy Off",  [this](){world.repro_prob=0.0;} );
    repro_sel.SetOption("Copy Slow", [this](){world.repro_prob=0.003;} );
    repro_sel.SetOption("Copy Fast", [this](){world.repro_prob=0.01;} );
    repro_sel.SelectID(1);
    control_set << repro_sel;

    // And stats (next o canvas)
    auto stats_set = doc.AddDiv("stats");
    stats_set.SetPosition(world.width+40, 60);
    auto & body_set = world.physics.GetBodySet();

    stats_set << "Update: " << UI::Live( [this]() { return anim.GetFrameCount(); } ) << "<br>";
    stats_set << "Org Count: " << UI::Live( [&body_set](){ return body_set.size(); } );

    stats_set << "<br><br>"
              << "Each circle represents a <b>single cell</b>.<br>"
              << "Cells are <b>attached</b> during gestation; in some setups, they may stay attached after birth.<br>"
              << "<b>Colors</b> are (mostly) meaningless, but have a 5% chance of changing at birth.<br>"
              << "<br>"
              << "Press <b>Start/Stop</b> to begin or pause a run; <b>Step</b> advances a run by a single update.<br>"
              << "Use <b>Reset</b> to restart a run from the beginning (using current settings).<br>"
              << "Freeze the <b>Map</b> to speed up processing by more than 10-fold.<br>"
              << "Cells can be <b>Individuals</b> or linked into clusters like <b>Snowflake</b> Yeast.<br>"
              << "<b>Cell Sizes</b> can be changed, but you need to <b>Reset</b> the run to see the results.<br>"
              << "<b>Flow</b> indicates the ammount of Brownian motion in the run.<br>"
              << "<b>Copy</b> rate determines how quickly cells should be reproducing.<br>"
              << "<br>"
              << "<b>Keyboard Shortcuts</b>:<br>"
              << "&nbsp;&nbsp;<b>[SPACE]</b>: Start/Stop.<br>"
              << "&nbsp;&nbsp;<b>[ARROWS]</b>: Move a cell around.<br>"
              << "&nbsp;&nbsp;<b>[M]</b>: Toggle Map Mode (Basic vs. Blank).<br>"
              << "&nbsp;&nbsp;<b>[R]</b>: Reset Run.<br>"
              << "&nbsp;&nbsp;<b>[S]</b>: Step a single update.<br>"
      ;


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
    case MapMode::MAKE_BLANK:
      doc.Canvas("pop_view").Clear();
      map_mode = MapMode::BLANK;
    case MapMode::BLANK:
      break;
    case MapMode::BASIC: 
      UI::Draw( doc.Canvas("pop_view"),
                world.physics.GetSurface(),
                emp::GetHueMap(360));
      break;
    }

    doc.Div("stats").Redraw();
  }

  void DoStart() {
    anim.ToggleActive();
    auto start_but = doc.Button("start_but");
    auto step_but = doc.Button("step_but");

    if (anim.GetActive()) {
      start_but.Label("Stop");    // If animation is running, button should read "Stop"
      step_but.Disabled(true);    // Cannot step animation already running.
    }
    else {
      start_but.Label("Start");     // If animation is stopped, button should read "Start"
      step_but.Disabled(false);    // Can step stopped animation.
    }
  }

  void DoStep() {
    emp_assert(anim.GetActive() == false); // Step is only meaningful if the run is stopped.
    anim.Step();
  }

  void DoReset() {
    world.Reset();
    
    // Redraw the world.
    UI::Draw( doc.Canvas("pop_view"),
              world.physics.GetSurface(),
              emp::GetHueMap(360));
  }


  bool OnKeydown(const emp::web::KeyboardEvent & evt_info) {
    // Reject most modified keypresses.
    if (evt_info.altKey || evt_info.ctrlKey || evt_info.metaKey) return false;

    const int key_code = evt_info.keyCode;
    bool return_value = true;
    auto * user_body = world.physics.GetBodySet()[0];
    
    switch (key_code) {
    case ' ':                                     // [SPACE] => Start / Stop a run
      DoStart();
      break;
    case 'M':                                     // M => Map Mode
      switch (map_mode) {
      case MapMode::BLANK:
      case MapMode::MAKE_BLANK:
        map_mode = MapMode::BASIC;
        break;
      case MapMode::BASIC:
        map_mode = MapMode::MAKE_BLANK;
        break;
      }
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
  emp::Initialize();
  evoke_interface = new EvokeInterface();

  return 0;
}
