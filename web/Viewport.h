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

  public:
    Viewport(int _x, int _y, int _width, int _height, dViewportPhysics & _physics)
      : CustomShape(_x, _y, _width, _height, this, &Viewport::Draw)
      , physics(_physics)
    { ; }
    ~Viewport() { ; }

    void Draw(emp::Canvas & canvas) {
      // Setup the black background
      canvas.SetFill("black");
      canvas.Rect(0, 0, GetWidth(), GetHeight(), true);

      // Draw all shapes in the physics.
      const std::unordered_set<BODY_TYPE *> & active_body_set = physics.GetBodySet();
      canvas.SetStroke("white");
      for (const auto body : active_body_set) {
        canvas.BeginPath();
        canvas.Circle(body->GetPerimeter());
        canvas.Stroke();
      }
    }
  };

};

#endif
