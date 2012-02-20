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

// CEDAR INCLUDES
#include "cedar/devices/robot/SimulatedKinematicChain.h"
#include "cedar/devices/robot/KinematicChainModel.h"
#include "cedar/devices/robot/gl/KukaArm.h"
#include "cedar/devices/robot/gui/KinematicChainWidget.h"
#include "cedar/auxiliaries/System.h"
#include "cedar/auxiliaries/gl/Scene.h"
#include "cedar/auxiliaries/gui/Viewer.h"
#include "cedar/auxiliaries/gui/SceneWidget.h"
#include "cedar/auxiliaries/sleepFunctions.h"


// SYSTEM INCLUDES
#include <QApplication>


int main(int argc, char **argv)
{
  std::string configuration_file = cedar::aux::System::locateResource("configs/kuka_lwr4.conf");
  std::string polygon_file_path = "";
  // help requested?
  if ((argc == 2) && (std::string(argv[1]) == "-h"))
  { // Check the value of argc. If not enough parameters have been passed, inform user and exit.
    std::cout << "Usage is -c <config file> -p <path to polygon data>" << std::endl;
    return 0;
  }
  // parse arguments
  for (int i = 1; i < argc; i++)
  {
    if (i+1 != argc) // check that we haven't finished parsing already
    {
      if (std::string(argv[i]) == "-c")
      {
        configuration_file = std::string(argv[i+1]);
      }
    }
  }





  QApplication a(argc, argv);

  // create simulated kinematic chains
  cedar::dev::robot::KinematicChainPtr p_kuka_arm(new cedar::dev::robot::SimulatedKinematicChain(configuration_file));

  // create models calculation of the transformation
  cedar::dev::robot::KinematicChainModelPtr p_kuka_arm_model
  (
    new cedar::dev::robot::KinematicChainModel(p_kuka_arm)
  );

  // create gl visualization objects
  cedar::aux::gl::RigidBodyVisualizationPtr p_arm_visualization;
  cedar::dev::robot::gl::KinematicChainPtr p_kuka_arm_visualization
  (
    new cedar::dev::robot::gl::KukaArm(p_kuka_arm_model)
  );
  p_arm_visualization = p_kuka_arm_visualization;

  // create scene and viewer to display the arm
  cedar::aux::gl::ScenePtr p_scene(new cedar::aux::gl::Scene());
  p_scene->setSceneLimit(2);
  p_scene->drawFloor(true);
  p_scene->addRigidBodyVisualization(p_arm_visualization);
  cedar::aux::gui::Viewer viewer(p_scene);
  viewer.show();
  viewer.setSceneRadius(p_scene->getSceneLimit());

  // create control widgets for the scene and the arm
  cedar::aux::gui::SceneWidgetPtr p_scene_widget(new cedar::aux::gui::SceneWidget(p_scene));
  cedar::dev::robot::gui::KinematicChainWidget widget_arm(p_kuka_arm);

  // show and start everything
  p_scene_widget->show();
  widget_arm.show();
  p_kuka_arm_model->startTimer(50.0);
  viewer.startTimer(50);
  a.exec();

  p_kuka_arm->stop();
  p_kuka_arm->wait();
  return 0;
}
