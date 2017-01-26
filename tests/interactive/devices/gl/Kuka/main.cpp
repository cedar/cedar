/*======================================================================================================================

    Copyright 2011, 2012, 2013, 2014, 2015, 2016, 2017 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany

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

// CEDAR INCLUDES
#include "cedar/devices/SimulatedKinematicChain.h"
#include "cedar/devices/gl/KukaArm.h"
#include "cedar/devices/gui/KinematicChainWidget.h"
#include "cedar/auxiliaries/systemFunctions.h"
#include "cedar/auxiliaries/gl/Scene.h"
#include "cedar/auxiliaries/gui/Viewer.h"
#include "cedar/auxiliaries/gui/SceneWidget.h"
#include "cedar/auxiliaries/sleepFunctions.h"


// SYSTEM INCLUDES
#include <QApplication>


int main(int argc, char **argv)
{
  // find resources
  std::string configuration_file = cedar::aux::locateResource("robots/kuka_lwr4.json");

  QApplication a(argc, argv);

  // create simulated kinematic chains
  cedar::dev::KinematicChainPtr p_kuka_arm(new cedar::dev::SimulatedKinematicChain());
  p_kuka_arm->readJson(configuration_file);

  // create gl visualization objects
  cedar::aux::gl::ObjectVisualizationPtr p_arm_visualization;
  //cedar::dev::gl::KinematicChainPtr p_kuka_arm_visualization
  //(
  //  new cedar::dev::gl::KukaArm(p_kuka_arm)
  //);
  cedar::dev::gl::KukaArmPtr p_kuka_arm_visualization
  (
    new cedar::dev::gl::KukaArm(p_kuka_arm)
  );
  p_arm_visualization = p_kuka_arm_visualization;

  // create scene and viewer to display the arm
  cedar::aux::gl::ScenePtr p_scene(new cedar::aux::gl::Scene());
  p_scene->setSceneLimit(2);
  p_scene->drawFloor(true);
  p_scene->addObjectVisualization(p_arm_visualization);
  cedar::aux::gui::Viewer viewer(p_scene);
  viewer.show();
  viewer.setSceneRadius(p_scene->getSceneLimit());

  // create control widgets for the scene and the arm
  cedar::aux::gui::SceneWidgetPtr p_scene_widget(new cedar::aux::gui::SceneWidget(p_scene));
  cedar::dev::gui::KinematicChainWidget widget_arm(p_kuka_arm);

  // show and start everything
  p_scene_widget->show();
  widget_arm.show();
  viewer.startTimer(20);
  p_kuka_arm->startCommunication();
  a.exec();

  return 0;
}
