/*------------------------------------------------------------------------------
 ----- Institute:   Ruhr-Universitaet Bochum
                    Institut fuer Neuroinformatik

 ----- File:        main.cpp

 ----- Maintainer:  Hendrik Reimann
 ----- Email:       hendrik.reimann@ini.rub.de
 ----- Date:        2010 11 17

 ----- Description: interactive test for the kinematic chain simulation

 ----- Credits:
 -----------------------------------------------------------------------------*/

// LOCAL INCLUDES

// PROJECT INCLUDES
#include "cedar/devices/robot/ReferenceGeometry.h"
#include "cedar/devices/robot/KinematicChainSimulation.h"

// SYSTEM INCLUDES
#include <QApplication>

using namespace std;
using namespace cv;
//using namespace cedar::aux::gl;
//using namespace cedar::dev::robot;


int main(int argc, char **argv)
{
  QApplication a(argc, argv);

//  ReferenceGeometry reference_geometry("/Users/reimajbi/src/cedar/tests/interactive/devices/KinematicChainSimulation/test_arm.conf");
//  ReferenceGeometry reference_geometry("test_arm.conf");
//  KinematicChainModel test_arm_model(&reference_geometry);
//  KinematicChainSimulation test_arm(&reference_geometry);

//  KinematicChainSimulation test_arm;
  
  a.exec();
  return 0;
}
