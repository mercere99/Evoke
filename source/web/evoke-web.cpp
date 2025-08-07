#include <emscripten.h>

#include "../defs.h"

#include "emp/web/JSWrap.hpp"
#include "emp/geometry/Physics2D.hpp"

#include "emp/web/Animate.hpp"
#include "emp/web/canvas_utils.hpp"
#include "emp/web/Document.hpp"
#include "emp/web/KeypressManager.hpp"
#include "emp/web/web.hpp"

#include "../main/World.hpp"
#include "../organisms/OrgControl.hpp"

namespace UI = emp::web;

class EvokeInterface {
private:
  evoke::World world;  // Everything unrelated to the interface should be in the world.

  size_t view_id = 0;  // Which cell is currently being viewed?

  UI::Document doc;
  UI::Animate anim;
  UI::KeypressManager keypress_manager;

  enum class MapMode { BLANK, MAKE_BLANK, BASIC } map_mode;

  using key_event_t = emp::web::KeyboardEvent;

public:
  EvokeInterface()
    : doc("emp_base")
    , anim([this](){ EvokeInterface::Animate(anim); })
    , map_mode(MapMode::BASIC)
  {
    DEBUG_STACK();
    // Link keypresses to the proper handlers
    keypress_manager.AddKeydownCallback([this](const key_event_t & evt){ return OnKeydown(evt); });

    // Add a canvas to draw the world.
    doc << UI::Canvas(world.size, "pop_view") << "<br>";

    // Add buttons.
    auto control_set = doc.AddDiv("buttons");
    // control_set.SetPosition(10, 260+world.height);
    control_set << UI::Button([this](){ DoStart(); }, "Start", "start_but");
    control_set << UI::Button([this](){ DoStep();  }, "Step", "step_but");
    control_set << UI::Button([this](){ DoReset(); }, "Reset", "reset_but");

    UI::Selector map_sel("map_sel");
    map_sel.SetOption("Basic Map",         [this](){map_mode = MapMode::BASIC;} );
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
    size_sel.SetOption("Cell Size 2",  [this](){world.org_radius=2.0;} );
    size_sel.SetOption("Cell Size 3",  [this](){world.org_radius=3.0;} );
    size_sel.SetOption("Cell Size 4",  [this](){world.org_radius=4.0;} );
    size_sel.SetOption("Cell Size 6",  [this](){world.org_radius=6.0;} );
    size_sel.SetOption("Cell Size 8",  [this](){world.org_radius=8.0;} );
    size_sel.SetOption("Cell Size 10", [this](){world.org_radius=10.0;} );
    size_sel.SetOption("Cell Size 15", [this](){world.org_radius=15.0;} );
    world.org_radius=3.0;
    size_sel.SelectID(1);
    control_set << size_sel;

    UI::Selector drift_sel("drift_sel");
    drift_sel.SetOption("Flow Off",  [this](){world.drift=0.0;} );
    drift_sel.SetOption("Flow Low",  [this](){world.drift=0.05;} );
    drift_sel.SetOption("Flow High", [this](){world.drift=0.15;} );
    world.drift = 0.0;
    drift_sel.SelectID(0);
    control_set << drift_sel;

    UI::Selector repro_sel("repro_sel");
    repro_sel.SetOption("Copy Off",  [this](){world.repro_prob=0.0;} );
    repro_sel.SetOption("Copy Slow", [this](){world.repro_prob=0.003;} );
    repro_sel.SetOption("Copy Normal", [this](){world.repro_prob=0.01;} );
    repro_sel.SetOption("Copy Fast", [this](){world.repro_prob=0.03;} );
    world.repro_prob=0.01;
    repro_sel.SelectID(2);
    control_set << repro_sel;

    // And stats (next to canvas)
    auto stats_set = doc.AddDiv("stats");
    stats_set.SetPosition(world.size.Width()+40, 80);

    stats_set << "Update: " << UI::Live( [this]() { return anim.GetFrameCount(); } ) << "<br>";
    stats_set << "Org Count: " << UI::Live( [this](){ return world.physics.NumBodies(); } ) << "<br>";
    stats_set << "Array Size: " << UI::Live( [this](){ return world.physics.GetBodySet().size(); } ) << "<br>";
    stats_set << "Empty: " << UI::Live( [this](){ return world.physics.NumOpen(); } ) << "<br>";

    stats_set << "<br>"
              << "Sample Org: " << UI::Live( [this](){
                    return world.physics.NumBodies() ? world.physics.GetActiveBody().GetID() : 1000000;
                  } ) << "<br>"
              << "Center: " << UI::Live( [this](){
                    return world.physics.NumBodies() ? world.physics.GetActiveBody().GetCenter() : emp::Point{-1,-1};
                  } ) << "<br>"
              << "Links: " << UI::Live( [this](){
                return world.physics.NumBodies() ? world.physics.GetActiveBody().GetLinkIDs() : emp::vector<size_t>{};
              } ) << "<br>";

    stats_set << "<br>"
      "Each circle represents a <b>single cell</b>.<br>"
      "Cells are <b>attached</b> during gestation; in some setups, they may stay attached after birth.<br>"
      "<b>Colors</b> are (mostly) meaningless, but have a 5% chance of changing at birth.<br>"
      "<br>"
      "Press <b>Start/Stop</b> to begin or pause a run; <b>Step</b> advances a run by a single update.<br>"
      "Use <b>Reset</b> to restart a run from the beginning (using current settings).<br>"
      "Freeze the <b>Map</b> to speed up processing by more than 10-fold.<br>"
      "Cells can be <b>Individuals</b> or linked into clusters like <b>Snowflake</b> Yeast.<br>"
      "<b>Cell Sizes</b> can be changed, but you need to <b>Reset</b> the run to see the results.<br>"
      "<b>Flow</b> indicates the ammount of Brownian motion in the run.<br>"
      "<b>Copy</b> rate determines how quickly cells should be reproducing.<br>"
      "<br>"
      "<b>Keyboard Shortcuts</b>:<br>"
      "&nbsp;&nbsp;<b>[SPACE]</b>: Start/Stop.<br>"
      "&nbsp;&nbsp;<b>[ARROWS]</b>: Move a cell around (forward and turns).<br>"
      "&nbsp;&nbsp;<b>[M]</b>: Toggle Map Mode (Basic vs. Blank).<br>"
      "&nbsp;&nbsp;<b>[R]</b>: Reset Run.<br>"
      "&nbsp;&nbsp;<b>[S]</b>: Step a single update.<br>"
      ;


    world.Init(); // Startup the world.

    // Draw initial state of the world.
    UI::Draw( doc.Canvas("pop_view"), world.physics);
  }

  ~EvokeInterface() {}

  void Animate([[maybe_unused]] const UI::Animate & anim) {
    DEBUG_STACK();
    world.Update();

    switch (map_mode) {
    case MapMode::MAKE_BLANK:
      doc.Canvas("pop_view").Clear();
      map_mode = MapMode::BLANK;
    case MapMode::BLANK:
      break;
    case MapMode::BASIC:
      UI::Draw( doc.Canvas("pop_view"), world.physics);
      break;
    }

    doc.Div("stats").Redraw();
  }

  void DoStart() {
    DEBUG_STACK();
    anim.ToggleActive();
    auto start_but = doc.Button("start_but");
    auto step_but = doc.Button("step_but");

    if (anim.GetActive()) {
      start_but.SetLabel("Stop");    // If animation is running, button should read "Stop"
      step_but.SetDisabled(true);    // Cannot step animation already running.
    }
    else {
      start_but.SetLabel("Start");     // If animation is stopped, button should read "Start"
      step_but.SetDisabled(false);    // Can step stopped animation.
    }
  }

  void DoStep() {
    DEBUG_STACK();
    emp_assert(anim.GetActive() == false); // Step is only meaningful if the run is stopped.
    anim.Step();
  }

  void DoReset() {
    DEBUG_STACK();
    world.Reset();

    // Redraw the world.
    UI::Draw(doc.Canvas("pop_view"), world.physics);
  }


  bool OnKeydown(const emp::web::KeyboardEvent & evt_info) {
    DEBUG_STACK();
    // Reject most modified keypresses.
    if (evt_info.altKey || evt_info.ctrlKey || evt_info.metaKey) return false;

    const int key_code = evt_info.keyCode;
    bool return_value = true;
    auto & user_body = world.physics.GetActiveBody();

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
      if (user_body.IsActive()) user_body.RotateDegrees(45.0);
      break;
    case 38:                                      // UP ARROW (Accelerate)
      if (user_body.IsActive()) user_body.IncSpeed();
      break;
    case 39:                                      // RIGHT ARROW (Turn Right)
      if (user_body.IsActive()) user_body.RotateDegrees(-45.0);
      break;
    case 40:                                      // DOWN ARROW (Breaks)
      if (user_body.IsActive()) user_body.DecSpeed();
      break;

    default:
      return_value = false;
    };

    return return_value;
  }

};

EvokeInterface * evoke_interface;

int emp_main()
{
  DEBUG_STACK();
  evoke_interface = new EvokeInterface();
  return 0;
}
