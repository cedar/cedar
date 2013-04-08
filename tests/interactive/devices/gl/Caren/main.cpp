/*======================================================================================================================

    Copyright 2011, 2012 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany

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

 ----- Description: interactive test for the Caren simulation

 ----- Credits:
 -----------------------------------------------------------------------------*/

// CEDAR INCLUDES
#include "cedar/devices/robot/SimulatedKinematicChain.h"
#include "cedar/devices/robot/gl/Caren.h"
#include "cedar/devices/robot/gui/KinematicChainWidget.h"
#include "cedar/devices/robot/gui/MountedCameraViewer.h"
#include "cedar/auxiliaries/systemFunctions.h"
#include "cedar/auxiliaries/gl/Scene.h"
#include "cedar/auxiliaries/gui/Viewer.h"
#include "cedar/auxiliaries/gui/SceneWidget.h"
#include "cedar/auxiliaries/sleepFunctions.h"
#include "cedar/auxiliaries/gl/Cylinder.h"

// SYSTEM INCLUDES
#include <QApplication>


int main(int argc, char **argv)
{
  QApplication a(argc, argv);

  // find resources
  std::string trunk_configuration_file = cedar::aux::locateResource("configs/caren_trunk.json");
  std::string arm_configuration_file = cedar::aux::locateResource("configs/kuka_lwr4.json");
  std::string head_configuration_file = cedar::aux::locateResource("configs/caren_head.json");
  std::string camera_middle_configuration_file = cedar::aux::locateResource("configs/caren_camera_middle.json");
  std::string finger_one_configuration_file = cedar::aux::locateResource("configs/sdh_finger_one.json");
  std::string finger_two_configuration_file = cedar::aux::locateResource("configs/sdh_finger_two.json");
  std::string finger_three_configuration_file = cedar::aux::locateResource("configs/sdh_finger_three.json");
  std::string palm_configuration_file = cedar::aux::locateResource("configs/sdh_palm.json");

  // create simulated kinematic chains
  cedar::dev::robot::KinematicChainPtr caren_trunk(new cedar::dev::robot::SimulatedKinematicChain());
  caren_trunk->readJson(trunk_configuration_file);
  cedar::dev::robot::KinematicChainPtr caren_arm(new cedar::dev::robot::SimulatedKinematicChain());
  caren_arm->readJson(arm_configuration_file);
  cedar::dev::robot::KinematicChainPtr caren_head(new cedar::dev::robot::SimulatedKinematicChain());
  caren_head->readJson(head_configuration_file);
  cedar::dev::robot::KinematicChainPtr finger_one(new cedar::dev::robot::SimulatedKinematicChain());
  finger_one->readJson(finger_one_configuration_file);
  cedar::dev::robot::KinematicChainPtr finger_two(new cedar::dev::robot::SimulatedKinematicChain());
  finger_two->readJson(finger_two_configuration_file);
  cedar::dev::robot::KinematicChainPtr finger_three(new cedar::dev::robot::SimulatedKinematicChain());
  finger_three->readJson(finger_three_configuration_file);
  cedar::dev::robot::KinematicChainPtr palm(new cedar::dev::robot::SimulatedKinematicChain());
  palm->readJson(palm_configuration_file);

  // link kinematic chains to each other
  caren_trunk->setEndEffector(caren_arm->getRootCoordinateFrame());
  caren_arm->setEndEffector(palm->getRootCoordinateFrame());

  // create scene and viewer to display the arm
  cedar::aux::gl::ScenePtr scene(new cedar::aux::gl::Scene());
  scene->setSceneLimit(2);
  scene->drawFloor(true);
  cedar::aux::gui::Viewer viewer(scene);
  viewer.setSceneRadius(scene->getSceneLimit());

  // create visualization objects
  cedar::dev::robot::gl::CarenPtr caren_visualization
  (
    new cedar::dev::robot::gl::Caren
    (
      caren_trunk,
      caren_arm,
      caren_head,
      palm,
      finger_one,
      finger_two,
      finger_three
    )
  );

  // add visualization objects to scene
  scene->addObjectVisualization(caren_visualization);

  // create control widgets for the scene and the arm
  cedar::aux::gui::SceneWidgetPtr scene_widget(new cedar::aux::gui::SceneWidget(scene));
  cedar::dev::robot::gui::KinematicChainWidget widget_trunk(caren_trunk);
  cedar::dev::robot::gui::KinematicChainWidget widget_arm(caren_arm);
  cedar::dev::robot::gui::KinematicChainWidget widget_head(caren_head);
  cedar::dev::robot::gui::KinematicChainWidget widget_finger_one(finger_one);
  cedar::dev::robot::gui::KinematicChainWidget widget_finger_two(finger_two);
  cedar::dev::robot::gui::KinematicChainWidget widget_finger_three(finger_three);
  cedar::dev::robot::gui::KinematicChainWidget widget_palm(palm);

  // create a cylinder visualization and add it to the scene
  cedar::aux::LocalCoordinateFramePtr cylinder_local_coordinate_frame(new cedar::aux::LocalCoordinateFrame());
  cylinder_local_coordinate_frame->setName("cylinder");
  cylinder_local_coordinate_frame->setTranslation(.3, .0, 1.0);
  cedar::aux::gl::ObjectVisualizationPtr cylinder
  (
    new cedar::aux::gl::Cylinder(cylinder_local_coordinate_frame, .1, .2, 0, 0.8, 0)
  );
  scene->addObjectVisualization(cylinder);

  // create a mounted camera viewer
  cedar::dev::robot::gui::MountedCameraViewer camera_viewer(scene, caren_head, false);
  camera_viewer.readJson(camera_middle_configuration_file);
  camera_viewer.setSceneRadius(scene->getSceneLimit());

//  std::cout << "camera type = " << camera_viewer.camera()->type() << std::endl;
//  std::cout << "camera zNear = " << camera_viewer.camera()->zNear() << std::endl;
//  camera_viewer.camera()->setZNearCoefficient(0.025);
//  std::cout << "camera zNear = " << camera_viewer.camera()->zNear() << std::endl;

  // show widgets
  scene_widget->show();
  widget_trunk.show();
  widget_arm.show();
  widget_head.show();
  widget_finger_one.show();
  widget_finger_two.show();
  widget_finger_three.show();
  widget_palm.show();
  viewer.show();
  camera_viewer.show();

  caren_trunk->startTimer(50.0);
  caren_arm->startTimer(50.0);
  caren_head->startTimer(50.0);
  viewer.startTimer(50);
  camera_viewer.startTimer(50);
  a.exec();

  caren_trunk->stop();
  caren_arm->stop();
  caren_head->stop();
  caren_trunk->wait();
  caren_arm->wait();
  caren_head->wait();

  return 0;
}
