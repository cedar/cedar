/*======================================================================================================================

    Copyright 2011, 2012, 2013, 2014 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany

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

 ----- Maintainer:  Stephan Zibner
 ----- Email:       stephan.zibner@ini.rub.de
 ----- Date:        2014 07 23

 ----- Description: unit test for robot configurations

 ----- Credits:
 -----------------------------------------------------------------------------*/

// CEDAR INCLUDES
#include "cedar/devices/SimulatedKinematicChain.h"
#include "cedar/devices/gui/KinematicChainWidget.h"
#include "cedar/auxiliaries/systemFunctions.h"
#include "cedar/auxiliaries/sleepFunctions.h"
#include "cedar/units/Time.h"

// SYSTEM INCLUDES
#include <QApplication>


int main(int argc, char **argv)
{
  int errors = 0;

  QApplication a(argc, argv);

  // test loading CoRA
  {
    std::string arm_configuration_file = cedar::aux::locateResource("configs/cora_arm.json");
    std::string head_configuration_file = cedar::aux::locateResource("configs/cora_head.json");

    // create simulated kinematic chains
    cedar::dev::KinematicChainPtr p_cora_arm(new cedar::dev::SimulatedKinematicChain());
    p_cora_arm->readJson(arm_configuration_file);
    if (p_cora_arm->getNumberOfJoints() != 8)
    {
      std::cout << "Configuration in path " << arm_configuration_file << " did not have the right amount of joints!"<< std::endl;
      ++errors;
    }
    cedar::dev::KinematicChainPtr p_cora_head(new cedar::dev::SimulatedKinematicChain());
    p_cora_head->readJson(head_configuration_file);
    if (p_cora_head->getNumberOfJoints() != 2)
    {
      std::cout << "Configuration in path " << head_configuration_file << " did not have the right amount of joints!"<< std::endl;
      ++errors;
    }
  }

  // test loading CAREN
  {
    std::string trunk_configuration_file = cedar::aux::locateResource("configs/caren_trunk.json");
    std::string arm_configuration_file = cedar::aux::locateResource("configs/kuka_lwr4.json");
    std::string head_configuration_file = cedar::aux::locateResource("configs/caren_head.json");
    std::string camera_middle_configuration_file = cedar::aux::locateResource("configs/caren_camera_middle.json");
    std::string finger_one_configuration_file = cedar::aux::locateResource("configs/sdh_finger_one.json");
    std::string finger_two_configuration_file = cedar::aux::locateResource("configs/sdh_finger_two.json");
    std::string finger_three_configuration_file = cedar::aux::locateResource("configs/sdh_finger_three.json");
    std::string palm_configuration_file = cedar::aux::locateResource("configs/sdh_palm.json");

    // create simulated kinematic chains
    cedar::dev::KinematicChainPtr caren_trunk(new cedar::dev::SimulatedKinematicChain());
    caren_trunk->readJson(trunk_configuration_file);
    if (caren_trunk->getNumberOfJoints() != 1)
    {
      std::cout << "Configuration in path " << trunk_configuration_file << " did not have the right amount of joints!"<< std::endl;
      ++errors;
    }
    cedar::dev::KinematicChainPtr caren_arm(new cedar::dev::SimulatedKinematicChain());
    caren_arm->readJson(arm_configuration_file);
    if (caren_arm->getNumberOfJoints() != 7)
    {
      std::cout << "Configuration in path " << arm_configuration_file << " did not have the right amount of joints!"<< std::endl;
      ++errors;
    }
    cedar::dev::KinematicChainPtr caren_head(new cedar::dev::SimulatedKinematicChain());
    caren_head->readJson(head_configuration_file);
    if (caren_head->getNumberOfJoints() != 2)
    {
      std::cout << "Configuration in path " << head_configuration_file << " did not have the right amount of joints!"<< std::endl;
      ++errors;
    }
    cedar::dev::KinematicChainPtr finger_one(new cedar::dev::SimulatedKinematicChain());
    finger_one->readJson(finger_one_configuration_file);
    if (finger_one->getNumberOfJoints() != 2)
    {
      std::cout << "Configuration in path " << finger_one_configuration_file << " did not have the right amount of joints!"<< std::endl;
      ++errors;
    }
    cedar::dev::KinematicChainPtr finger_two(new cedar::dev::SimulatedKinematicChain());
    finger_two->readJson(finger_two_configuration_file);
    if (finger_two->getNumberOfJoints() != 2)
    {
      std::cout << "Configuration in path " << finger_two_configuration_file << " did not have the right amount of joints!"<< std::endl;
      ++errors;
    }
    cedar::dev::KinematicChainPtr finger_three(new cedar::dev::SimulatedKinematicChain());
    finger_three->readJson(finger_three_configuration_file);
    if (finger_three->getNumberOfJoints() != 2)
    {
      std::cout << "Configuration in path " << finger_three_configuration_file << " did not have the right amount of joints!"<< std::endl;
      ++errors;
    }
    cedar::dev::KinematicChainPtr palm(new cedar::dev::SimulatedKinematicChain());
    palm->readJson(palm_configuration_file);
    if (palm->getNumberOfJoints() != 1)
    {
      std::cout << "Configuration in path " << palm_configuration_file << " did not have the right amount of joints!"<< std::endl;
      ++errors;
    }

    // link kinematic chains to each other
    caren_trunk->setEndEffector(caren_arm->getRootCoordinateFrame());
    caren_arm->setEndEffector(palm->getRootCoordinateFrame());
  }

  return errors;
}
