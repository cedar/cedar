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
#include "cedar/auxiliaries/gl/RigidBodyVisualization.h"
#include "cedar/auxiliaries/gl/Scene.h"
#include "cedar/auxiliaries/gui/Viewer.h"
#include "cedar/auxiliaries/gui/SceneWidget.h"
#include "cedar/auxiliaries/sleepFunctions.h"


// SYSTEM INCLUDES
#include <QApplication>


int main(int argc, char **argv)
{
  QApplication a(argc, argv);

  // create simulated arm
  cedar::dev::robot::KinematicChainPtr p_test_arm(new cedar::dev::robot::SimulatedKinematicChain("../../../../tests/interactive/devices/gl/KinematicChain/test_arm.conf"));

  // create model of simulated arm
  cedar::dev::robot::KinematicChainModelPtr p_test_arm_model(new cedar::dev::robot::KinematicChainModel(p_test_arm));

  // create gl visualization objects
  cedar::dev::robot::gl::KinematicChainPtr p_test_arm_visualization(new cedar::dev::robot::gl::KinematicChain(p_test_arm_model));
//  cedar::aux::gl::ConePtr p_end_effector_visualization(new cedar::aux::gl::Cone (p_test_arm_model->getEndEffector(), .03, .1 ));

  // create scene and viewer to display the arm
  cedar::aux::gl::ScenePtr p_scene(new cedar::aux::gl::Scene);
  p_scene->setSceneLimit(2);
  p_scene->drawFloor(true);
  p_scene->addRigidBodyVisualization(p_test_arm_visualization);
//  p_scene->addRigidBodyVisualization(p_end_effector_visualization);

  // create a simple viewer for the scene
  cedar::aux::gui::Viewer viewer(p_scene);
  viewer.show();
  viewer.setSceneRadius(p_scene->getSceneLimit());

  // create a widget to control the scene
  cedar::aux::gui::SceneWidgetPtr p_scene_widget(new cedar::aux::gui::SceneWidget(p_scene));
  p_scene_widget->show();

  p_test_arm->setWorkingMode(cedar::dev::robot::KinematicChain::VELOCITY);
  p_test_arm->setJointVelocity(0, 0.31);
  p_test_arm->setJointVelocity(1, -.045);
  p_test_arm->setJointVelocity(2, -.015);
  p_test_arm->setJointVelocity(3, .025);


  // create everything for a second arm that's connected to the end-effector of the first one
  cedar::dev::robot::KinematicChainPtr p_second_arm(new cedar::dev::robot::SimulatedKinematicChain("../../../../tests/interactive/devices/gl/KinematicChain/test_arm.conf"));
  cedar::dev::robot::KinematicChainModelPtr p_second_arm_model(new cedar::dev::robot::KinematicChainModel(p_second_arm));
  cedar::dev::robot::gl::KinematicChainPtr p_second_arm_visualization(new cedar::dev::robot::gl::KinematicChain(p_second_arm_model));
  p_scene->addRigidBodyVisualization(p_second_arm_visualization);
  p_test_arm_model->setEndEffector(p_second_arm_model->getRootCoordinateFrame());
  p_second_arm->setWorkingMode(cedar::dev::robot::KinematicChain::VELOCITY);
  p_second_arm->setJointVelocity(0, 0.31);
  p_second_arm->setJointVelocity(1, -.45);
  p_second_arm->setJointVelocity(2, -.15);
  p_second_arm->setJointVelocity(3, .25);

  p_test_arm->start();
  p_test_arm_model->startTimer(5.0);
  viewer.startTimer(40);
  a.exec();

  p_test_arm->stop();
  p_test_arm->wait();

  return 0;
}
