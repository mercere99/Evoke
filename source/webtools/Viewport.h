#ifndef EVOKE_VIEWPORT_H
#define EVOKE_VIEWPORT_H

#include "kinetic/Kinetic.h"
#include "tools/functions.h"

#include "../main/World.h"

namespace evoke {

  // @CAO For now we are assuming that:
  //   BODY_TYPE is emp::CircleBody2D
  //   BODY_INFO is evoke::OrgControl

  // Viewport displays a world on the screen and allows some manipulation.
  template <typename BODY_TYPE, typename BODY_INFO, typename BASE_TYPE> 
  class Viewport : public emp::CustomShape {
  private:
    World & world;
    std::vector<emp::Color> color_map;

    BODY_TYPE * user_body;  // Which body, if any, is the player controlling?

  public:
    Viewport(int _x, int _y, evoke::World & _world)
      : CustomShape(_x, _y, world.width, world.height, this, &Viewport::Draw)
      , world(_world)
      , user_body(NULL)
    {
      On("click", this, &Viewport::OnClick);

      color_map.push_back("white");
      color_map.push_back("yellow");
      color_map.push_back("#CCFFCC");
    }
    ~Viewport() { ; }

    BODY_TYPE * GetUserBody() { return user_body; }
    
    void Draw(emp::Canvas & canvas) {
      // Setup the black background
      canvas.SetFill("black");
      canvas.Rect(0, 0, GetWidth(), GetHeight(), true);

      // Draw all shapes in the physics.
      canvas.SetStroke("white");
      canvas.SetFill("yellow");
      // const std::unordered_set<BODY_TYPE *> & active_body_set = world.physics.GetBodySet();
      const std::vector<BODY_TYPE *> & active_body_set = world.physics.GetBodySet();
      for (const auto body : active_body_set) {
        if (body->GetColorID() < 0 || body->GetColorID() > 2) emp::Alert((int) active_body_set.size());
        canvas.SetStroke(color_map[body->GetColorID()]);
        canvas.BeginPath();
        canvas.Circle(body->GetPerimeter());
        canvas.Stroke();
        if (body == user_body) {
          canvas.Fill();
          canvas.SetStroke("black");
          canvas.BeginPath();
          canvas.MoveTo(body->GetAnchor());
          canvas.LineTo(body->GetAnchor() + emp::Point<double>(body->GetOrientation().Sin(), body->GetOrientation().Cos()) * body->GetRadius());
          canvas.ClosePath();
          canvas.Stroke();
          canvas.SetStroke("white");
        }
      }

      // Make the canvas respond to the mouse.
      canvas.BeginPath();
      canvas.MoveTo(0, 0);
      canvas.LineTo(GetWidth(), 0);
      canvas.LineTo(GetWidth(), GetHeight());
      canvas.LineTo(0, GetHeight());
      canvas.ClosePath();
      canvas.SetupTarget(*this);
    }

    bool OnKeydown(const emp::EventInfo & evt_info) {
      const int key_code = evt_info.key_code;
      bool return_value = true;

      switch (key_code) {
      case 187:
        // @CAO Put a cap on body size?
        if (user_body) user_body->SetTargetRadius(user_body->GetTargetRadius() + 1);
        break;
      case 189:
        if (user_body) {
          int body_size = user_body->GetTargetRadius();
          if (body_size > 1) user_body->SetTargetRadius(body_size - 1);
        }
        break;
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
        // emp::Alert(key_code);
        return_value = false;
      };

      return return_value;
    }

    void OnClick(const emp::EventInfo & evt) {
      const emp::Point<BASE_TYPE> mouse_pos(evt.layer_x - GetX(), evt.layer_y - GetY());

      // Figure out which circle was clicked in.
      // const std::unordered_set<BODY_TYPE *> & active_body_set = world.physics.GetBodySet();
      const std::vector<BODY_TYPE *> & active_body_set = world.physics.GetBodySet();
      for (const auto body : active_body_set) {
        if (body->GetPerimeter().Contains(mouse_pos)) {
          body->SetColorID(1);
          DrawLayer();
          user_body = body;
        }
      }
    }
    
  };

};

#endif
