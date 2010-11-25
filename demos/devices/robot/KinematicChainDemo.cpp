/*------------------------------------------------------------------------------
 ----- Institute:   Ruhr-Universitaet Bochum
                    Institut fuer Neuroinformatik

 ----- File:        KinematicChainTest.cpp

 ----- Author:      Bjoern Weghenkel
 ----- Email:       bjoern.weghenkel@ini.rub.de
 ----- Date:        2010 11 23

 ----- Description:

 ----- Credits:
 -----------------------------------------------------------------------------*/

#include "devices/robot/KinematicChain.h"

class MyDemoChain : public cedar::dev::robot::KinematicChain
{

public:

  MyDemoChain() : KinematicChain(2) {
    return;
  }

  double getJointAngle(unsigned int index) const {
    return 0.0;
  }

};


int main() {
  //MyDemoChain myDemoChain;
  return 0;
}
