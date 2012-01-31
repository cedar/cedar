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
 ----- Date:        2012 1 30

 ----- Description: Testing of the interface for the KUKA LBR

 ----- Credits:
 -----------------------------------------------------------------------------*/

// LOCAL INCLUDES

// PROJECT INCLUDES
#include "cedar/devices/kuka/gui/FriStatusWidget.h"
#include "cedar/devices/robot/gui/KinematicChainWidget.h"
#include "cedar/devices/robot/KinematicChain.h"
#include "cedar/devices/robot/gl/KinematicChain.h"
#include "cedar/devices/robot/KinematicChainModel.h"
#include "cedar/devices/robot/SimulatedKinematicChain.h"
#include "cedar/auxiliaries/gl/Scene.h"
#include "cedar/auxiliaries/gui/Viewer.h"


// SYSTEM INCLUDES
#include <vector>
#include <QApplication>
#include <iostream>

using namespace cedar::dev::kuka;
using cedar::dev::kuka::gui::FriStatusWidget;
using cedar::dev::robot::KinematicChainPtr;

int main(int argc, char **argv)
{
  std::string mode = "0";
  std::string configuration_file_path = "../../examples/kukaMovement/";
  if ((argc == 2) && (std::string(argv[1]) == "-h"))
  { // Check the value of argc. If not enough parameters have been passed, inform user and exit.
    std::cout << "Usage is -c <path to configs> -m <mode>" << std::endl;
    std::cout << "mode 'simulation': simulation only" << std::endl;
    std::cout << "mode 'hardware': hardware only" << std::endl;
    return 0;
  }
  for (int i = 1; i < argc; i++)
  {
    if (i+1 != argc) // Check that we haven't finished parsing already
    {
      if (std::string(argv[i]) == "-c")
      {
        configuration_file_path = std::string(argv[i+1]);
      }
      else if (std::string(argv[i]) == "-m")
      {
        mode = std::string(argv[i+1]);
      }
    }
  }
  bool use_hardware = true;
  bool use_simulation = true;
  if (mode == std::string("simulation"))
  {
    use_hardware = false;
  }
  if (mode == std::string("hardware"))
  {
    use_simulation = false;
  }

  std::cout << mode << std::endl;
  std::cout << use_hardware << std::endl;
  std::cout << use_simulation << std::endl;


  QApplication a(argc, argv);
  FriStatusWidget* p_fri_status_widget = 0;
  cedar::dev::robot::gui::KinematicChainWidget* p_kinematic_chain_widget = 0;
  cedar::aux::gl::ScenePtr p_scene(new cedar::aux::gl::Scene);
  p_scene->setSceneLimit(2);
  p_scene->drawFloor(true);
  cedar::aux::gui::Viewer viewer(p_scene);
  viewer.show();
  viewer.setSceneRadius(p_scene->getSceneLimit());
  viewer.startTimer(50);


  // create the simulation
  if (use_simulation)
  {
    std::cout << "simulator..." << std::endl;
    // kinematic chain and model
    cedar::dev::robot::KinematicChainPtr p_arm_sim(new cedar::dev::robot::SimulatedKinematicChain(configuration_file_path + "kuka_lbr4.conf"));
    cedar::dev::robot::KinematicChainModelPtr p_arm_model(new cedar::dev::robot::KinematicChainModel(p_arm_sim));

    // visualization, scene and viewer
    cedar::dev::robot::gl::KinematicChainPtr p_arm_visualization(new cedar::dev::robot::gl::KinematicChain(p_arm_model));
    cedar::aux::gl::ObjectPtr p_object;
    p_object = p_arm_visualization;
    p_scene->addObject(p_object);
    p_arm_model->startTimer(50.0);

  }

  if (use_hardware)
  {
    // create the hardware interface
    KukaInterfacePtr p_arm(new KukaInterface(configuration_file_path + "kuka_lbr4.conf"));
    p_fri_status_widget = new FriStatusWidget(p_arm);
    p_fri_status_widget->startTimer(100);
    p_fri_status_widget->show();
    p_kinematic_chain_widget
     = new cedar::dev::robot::gui::KinematicChainWidget(p_arm, configuration_file_path + "kuka_lbr4_widget.conf");
    p_kinematic_chain_widget->show();
  }


  std::cout << "executing QApplication" << std::endl;
  a.exec();

  if (use_hardware)
  {
    delete p_fri_status_widget;
  }
  delete p_kinematic_chain_widget;

  return 0;
}
