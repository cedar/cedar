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
#include "cedar/devices/robot/gl/KukaArm.h"
#include "cedar/devices/robot/gui/KinematicChainWidget.h"
#include "cedar/auxiliaries/systemFunctions.h"
#include "cedar/auxiliaries/gl/Scene.h"
#include "cedar/auxiliaries/gui/Viewer.h"
#include "cedar/auxiliaries/gui/SceneWidget.h"
#include "cedar/auxiliaries/sleepFunctions.h"

// SYSTEM INCLUDES
#include <QApplication>


int main(int argc, char **argv)
{
  std::string trunk_configuration_file = cedar::aux::locateResource("configs/caren_trunk.json");
  std::string arm_configuration_file = cedar::aux::locateResource("configs/kuka_lwr4.json");
  std::string head_configuration_file = cedar::aux::locateResource("configs/caren_head.json");

  QApplication a(argc, argv);

  // create simulated kinematic chains
  cedar::dev::robot::KinematicChainPtr caren_trunk(new cedar::dev::robot::SimulatedKinematicChain());
  caren_trunk->readJson(trunk_configuration_file);
  cedar::dev::robot::KinematicChainPtr caren_arm(new cedar::dev::robot::SimulatedKinematicChain());
  caren_arm->readJson(arm_configuration_file);
  caren_trunk->setEndEffector(caren_arm->getRootCoordinateFrame());
  cedar::dev::robot::KinematicChainPtr caren_head(new cedar::dev::robot::SimulatedKinematicChain());
  caren_head->readJson(head_configuration_file);

  // create scene and viewer to display the arm
  cedar::aux::gl::ScenePtr scene(new cedar::aux::gl::Scene());
  scene->setSceneLimit(2);
  scene->drawFloor(true);
  cedar::aux::gui::Viewer viewer(scene);
  viewer.show();
  viewer.setSceneRadius(scene->getSceneLimit());

  // create visualization objects
  cedar::dev::robot::gl::KinematicChainPtr caren_trunk_visualization(new cedar::dev::robot::gl::KinematicChain(caren_trunk));
  cedar::dev::robot::gl::KinematicChainPtr caren_arm_visualization(new cedar::dev::robot::gl::KukaArm(caren_arm));
  cedar::dev::robot::gl::KinematicChainPtr caren_head_visualization(new cedar::dev::robot::gl::KinematicChain(caren_head));
//  arm_visualization->setDisplayEndEffectorVelocity(false);
  scene->addObjectVisualization(caren_trunk_visualization);
  scene->addObjectVisualization(caren_arm_visualization);
  scene->addObjectVisualization(caren_head_visualization);

  // create control widgets for the scene and the arm
  cedar::aux::gui::SceneWidgetPtr scene_widget(new cedar::aux::gui::SceneWidget(scene));
  cedar::dev::robot::gui::KinematicChainWidget trunk_widget(caren_trunk);
  cedar::dev::robot::gui::KinematicChainWidget arm_widget(caren_arm);
  cedar::dev::robot::gui::KinematicChainWidget head_widget(caren_head);

  scene_widget->show();
  trunk_widget.show();
  arm_widget.show();
  head_widget.show();

  caren_trunk->startTimer(50.0);
  caren_arm->startTimer(50.0);
  caren_head->startTimer(50.0);
  viewer.startTimer(50);
  a.exec();

  caren_trunk->stop();
  caren_arm->stop();
  caren_head->stop();
  cedar::aux::sleep(cedar::unit::Seconds(1));

  return 0;
}
