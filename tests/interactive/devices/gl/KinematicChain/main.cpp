/*======================================================================================================================

    Copyright 2011, 2012, 2013, 2014, 2015 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany

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
#include "cedar/devices/SimulatedKinematicChain.h"
#include "cedar/devices/gl/KinematicChain.h"
#include "cedar/auxiliaries/gl/ObjectVisualization.h"
#include "cedar/auxiliaries/gl/Scene.h"
#include "cedar/auxiliaries/gui/Viewer.h"
#include "cedar/auxiliaries/gui/SceneWidget.h"
#include "cedar/auxiliaries/sleepFunctions.h"
#include "cedar/auxiliaries/systemFunctions.h"


// SYSTEM INCLUDES
#include <QApplication>


int main(int argc, char **argv)
{
  QApplication a(argc, argv);

  // create simulated arm
  std::string finger_one_configuration_file = cedar::aux::locateResource("configs/test_arm.json");
  cedar::dev::KinematicChainPtr test_arm(new cedar::dev::SimulatedKinematicChain());
  test_arm->readJson(finger_one_configuration_file);

  // create gl visualization objects
  cedar::dev::gl::KinematicChainPtr test_arm_visualization
  (
    new cedar::dev::gl::KinematicChain(test_arm)
  );
//  cedar::aux::gl::ConePtr end_effector_visualization(new cedar::aux::gl::Cone (test_arm_model->getEndEffector(), .03, .1 ));

  // create scene and viewer to display the arm
  cedar::aux::gl::ScenePtr scene(new cedar::aux::gl::Scene);
  scene->setSceneLimit(2);
  scene->drawFloor(true);
  scene->addObjectVisualization(test_arm_visualization);
//  scene->addObjectVisualization(end_effector_visualization);

  // create a simple viewer for the scene
  cedar::aux::gui::Viewer viewer(scene);
  viewer.show();
  viewer.setSceneRadius(scene->getSceneLimit());

  // create a widget to control the scene
  cedar::aux::gui::SceneWidgetPtr scene_widget(new cedar::aux::gui::SceneWidget(scene));
  scene_widget->show();

  test_arm->start();
  test_arm->setJointVelocity(0, 0.31);
  test_arm->setJointVelocity(1, -.045);
  test_arm->setJointVelocity(2, -.015);
  test_arm->setJointVelocity(3, .025);


  // create everything for a second arm that's connected to the end-effector of the first one
  cedar::dev::KinematicChainPtr second_arm(new cedar::dev::SimulatedKinematicChain());
  second_arm->readJson(finger_one_configuration_file);

  cedar::dev::gl::KinematicChainPtr second_arm_visualization
  (
    new cedar::dev::gl::KinematicChain(second_arm)
  );
  second_arm_visualization->setDisplayBase(false);
  scene->addObjectVisualization(second_arm_visualization);
  test_arm->setEndEffector(second_arm->getRootCoordinateFrame());
  second_arm->setJointVelocity(0, 0.31);
  second_arm->setJointVelocity(1, -.45);
  second_arm->setJointVelocity(2, -.15);
  second_arm->setJointVelocity(3, .25);
  second_arm->setJointVelocity(0, .1);
  second_arm->setJointVelocity(1, .1);
  second_arm->setJointVelocity(2, .1);
  second_arm->setJointVelocity(3, .1);

  test_arm->start();
  test_arm->startTimer(20);
  second_arm->start();
  second_arm->startTimer(20);
  viewer.startTimer(20);
  a.exec();

  test_arm->stop();
  test_arm->wait();

  return 0;
}
