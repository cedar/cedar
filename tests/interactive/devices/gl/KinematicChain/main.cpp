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
#include "cedar/auxiliaries/gl/SceneWidget.h"


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
  ReferenceGeometryPtr p_reference_geometry(new ReferenceGeometry("../../../tests/interactive/devices/gl/KinematicChain/test_arm.conf"));
//  ReferenceGeometryPtr p_reference_geometry(new ReferenceGeometry("../../../tests/unit/devices/robot/KinematicChainModel/test.conf"));
  cedar::dev::robot::KinematicChainPtr p_test_arm(new SimulatedKinematicChain(p_reference_geometry));

  // create model of simulated arm
  KinematicChainModelPtr p_test_arm_model(new KinematicChainModel(p_test_arm));
  p_test_arm_model->startTimer(50);

  // create gl visualization object
  cedar::dev::robot::gl::KinematicChainPtr p_test_arm_visualization(new cedar::dev::robot::gl::KinematicChain(p_test_arm_model));

  // create scene and viewer to display the arm
  ScenePtr p_scene(new cedar::aux::gl::Scene);
  p_scene->setSceneLimit(2);
  p_scene->drawFloor(true);

  cedar::aux::gl::ObjectPtr object = p_test_arm_visualization;
  p_scene->addObject(object);

  // create a simple viewer for the scene
  Viewer viewer(p_scene);
  viewer.show();
  viewer.setSceneRadius(p_scene->getSceneLimit());
  viewer.startTimer(50);

  // create a widget to control the scene
  SceneWidgetPtr p_scene_widget(new SceneWidget(p_scene));
  p_scene_widget->show();

  p_test_arm->start();
  p_test_arm->setJointAngle(2, -M_PI*0.5);
  p_test_arm->setJointAngle(3, M_PI*0.5);
  p_test_arm->setJointVelocity(0, .3);
  p_test_arm->setJointVelocity(1, -1);
  p_test_arm->setJointVelocity(2, -.5);
  p_test_arm->setJointVelocity(3, .3);

  a.exec();
  return 0;
}
