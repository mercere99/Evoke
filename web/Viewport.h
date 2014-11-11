#ifndef EVOKE_VIEWPORT_H
#define EVOKE_VIEWPORT_H

#include "kinetic/Kinetic.h"

namespace evoke {
  class Viewport : public emp::CustomShape {
  private:
  public:
    Viewport(int _x, int _y, int _width, int _height)
      : CustomShape(_x, _y, _width, _height, this, &Viewport::Draw)
    { ; }
    ~Viewport() { ; }

    void Draw(emp::Canvas & canvas) {
      // Setup the black background
      canvas.SetFill("black");
      canvas.Rect(0, 0, GetWidth(), GetHeight(), true);
    }
  };

};

#endif
