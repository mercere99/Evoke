#ifndef EVOKE_ORG_CONTROL_H
#define EVOKE_ORG_CONTROL_H

namespace evoke {

  template <typename HARDWARE>
  class OrgControl {
  private:
    HARDWARE hardware;
  public:
    OrgControl(const HARDWARE & hw_prototype) : hardware(hw_prototype) { ; }
    OrgControl(const OrgControl & prototype) : OrgControl(prototype.hardware) { ; }
    ~OrgControl() { ; }
  };

};

#endif
