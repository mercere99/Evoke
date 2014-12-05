#ifndef EVOKE_VIEWPORT_H
#define EVOKE_VIEWPORT_H

#include "kinetic/Kinetic.h"
#include "geometry/Physics2D.h"

namespace evoke {

  // @CAO For now we are assuming that:
  //   BODY_TYPE is emp::CircleBody2D
  //   BODY_INFO is evoke::OrgControl

  // Viewport displays a physics environment on the screen and allows some manipulation.
  template <typename BODY_TYPE, typename BODY_INFO, typename BASE_TYPE> 
  class Viewport : public emp::CustomShape {
  private:
    typedef emp::Physics2D<BODY_TYPE, BODY_INFO, BASE_TYPE> dViewportPhysics;
    dViewportPhysics & physics;
    std::vector<emp::Color> color_map;

    BODY_TYPE * player_body;  // Which body, if any, is the player controlling?

  public:
    Viewport(int _x, int _y, int _width, int _height, dViewportPhysics & _physics)
      : CustomShape(_x, _y, _width, _height, this, &Viewport::Draw)
      , physics(_physics)
      , player_body(NULL)
    {
      On("click", this, &Viewport<BODY_TYPE, BODY_INFO, BASE_TYPE>::OnClick);
      color_map.push_back("white");
      color_map.push_back("yellow");
      color_map.push_back("#CCFFCC");
    }
    ~Viewport() { ; }

    void Draw(emp::Canvas & canvas) {
      // Setup the black background
      canvas.SetFill("black");
      canvas.Rect(0, 0, GetWidth(), GetHeight(), true);

      // Draw all shapes in the physics.
      canvas.SetStroke("white");
      canvas.SetFill("yellow");
      // const std::unordered_set<BODY_TYPE *> & active_body_set = physics.GetBodySet();
      const std::vector<BODY_TYPE *> & active_body_set = physics.GetBodySet();
      for (const auto body : active_body_set) {
        if (body->GetColorID() < 0 || body->GetColorID() > 2) emp::Alert((int) active_body_set.size());
        canvas.SetStroke(color_map[body->GetColorID()]);
        canvas.BeginPath();
        canvas.Circle(body->GetPerimeter());
        if (body == player_body) {
          canvas.Fill();
        }
        canvas.Stroke();
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

    void OnClick(const emp::EventInfo & evt) {
      const emp::Point<BASE_TYPE> mouse_pos(evt.layer_x - GetX(), evt.layer_y - GetY());

      // Figure out which circle was clicked in.
      // const std::unordered_set<BODY_TYPE *> & active_body_set = physics.GetBodySet();
      const std::vector<BODY_TYPE *> & active_body_set = physics.GetBodySet();
      for (const auto body : active_body_set) {
        if (body->GetPerimeter().Contains(mouse_pos)) {
          body->SetColorID(1);
          DrawLayer();
          player_body = body;
        }
      }
    }
    
  };

};

#endif
