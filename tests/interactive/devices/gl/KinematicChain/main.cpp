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
#include "cedar/devices/robot/SimulatedKinematicChain.h"
#include "cedar/devices/robot/KinematicChainModel.h"
#include "cedar/devices/robot/gl/KinematicChain.h"
#include "cedar/auxiliaries/gl/Scene.h"
#include "cedar/auxiliaries/gl/Block.h"
#include "cedar/auxiliaries/gl/Viewer.h"

// SYSTEM INCLUDES
#include <QApplication>

using namespace std;
using namespace cv;
using namespace cedar::aux::gl;
using namespace cedar::dev::robot::gl;
using namespace cedar::dev::robot;


int main(int argc, char **argv)
{
  QApplication a(argc, argv);

  // create simulated arm
  ReferenceGeometryPtr p_reference_geometry(new ReferenceGeometry("/Users/reimajbi/src/cedar.dev/tests/interactive/devices/gl/KinematicChain/test_arm.conf"));
//  ReferenceGeometryPtr p_reference_geometry(new ReferenceGeometry("/home/reimajbi/srcMercurial/cedar.dev/tests/interactive/devices/gl/KinematicChain/test_arm.conf"));
  cedar::dev::robot::KinematicChainPtr p_test_arm(new SimulatedKinematicChain(p_reference_geometry));

  // create model of simulated arm
  KinematicChainModelPtr p_test_arm_model(new KinematicChainModel(p_test_arm));

  // create gl visualization object
  cedar::dev::robot::gl::KinematicChain test_arm_visualization(p_test_arm_model);

  // create scene and viewer to display the arm
  Scene scene;
  scene.setSceneLimit(2);
  scene.drawFloor(true);
  scene.addObject(&test_arm_visualization);

  // create a simple viewer for the scene
  Viewer viewer(&scene);
  viewer.show();
  viewer.setSceneRadius(2);
  viewer.startTimer(50);

  p_test_arm->setJointAngle(0, 1);
  p_test_arm->setJointAngle(1, M_PI/2);
//  p_test_arm_model->startTimer(50);
  
  a.exec();
  return 0;
}
