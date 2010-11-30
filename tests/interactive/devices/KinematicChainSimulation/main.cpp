/*======================================================================================================================

    Copyright 2011 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany

    This file is part of cedar.

    cedar is free software: you can redistribute it and/or modify it under
    the terms of the GNU Lesser General Public License as published by the
    Free Software Foundation, either version 3 of the License, or (at your
    option) any later version.

    cedar is distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public
    License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with cedar. If not, see <http://www.gnu.org/licenses/>.

========================================================================================================================

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
using namespace cedar::aux::gl;
using namespace cedar::dev::robot;


int main(int argc, char **argv)
{
  QApplication a(argc, argv);

  ReferenceGeometry reference_geometry("/Users/reimajbi/src/cedar/tests/interactive/devices/KinematicChainSimulation/test_arm.conf");
//  ReferenceGeometry reference_geometry("test_arm.conf");
//  KinematicChainModel test_arm_model(&reference_geometry);
  KinematicChainSimulation test_arm(&reference_geometry);

//  KinematicChainSimulation test_arm;
  
  a.exec();
  return 0;
}
