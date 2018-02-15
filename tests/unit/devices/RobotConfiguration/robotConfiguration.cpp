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
#include "cedar/auxiliaries/systemFunctions.h"
#include "cedar/testingUtilities/helpers.h"

// SYSTEM INCLUDES
#include <QApplication>


int main(int argc, char **argv)
{
  int errors = 0;

  QApplication a(argc, argv);

  // test loading CoRA
  {
    std::string arm_configuration_file = cedar::aux::locateResource("robots/cora_arm.json", false);
    std::string head_configuration_file = cedar::aux::locateResource("robots/cora_head.json", false);

    // create simulated kinematic chains
    cedar::dev::KinematicChainPtr cora_arm(new cedar::dev::SimulatedKinematicChain());
    cora_arm->readJson(arm_configuration_file);
    CEDAR_UNIT_TEST_CONDITION_CUSTOM_MESSAGE(errors, cora_arm->getNumberOfJoints() == 8, "Configuration in path " + arm_configuration_file + " did not have the right amount of joints!");
    cedar::dev::KinematicChainPtr cora_head(new cedar::dev::SimulatedKinematicChain());
    cora_head->readJson(head_configuration_file);
    CEDAR_UNIT_TEST_CONDITION_CUSTOM_MESSAGE(errors, cora_head->getNumberOfJoints() == 2, "Configuration in path " + head_configuration_file + " did not have the right amount of joints!");
  }

  // test loading CAREN
  {
    std::string trunk_configuration_file = cedar::aux::locateResource("robots/caren_trunk.json", false);
    std::string arm_configuration_file = cedar::aux::locateResource("robots/kuka_lwr4.json", false);
    std::string head_configuration_file = cedar::aux::locateResource("robots/caren_head.json", false);
    std::string camera_middle_configuration_file = cedar::aux::locateResource("robots/caren_camera_middle.json", false);
    std::string finger_one_configuration_file = cedar::aux::locateResource("robots/sdh_finger_one.json", false);
    std::string finger_two_configuration_file = cedar::aux::locateResource("robots/sdh_finger_two.json", false);
    std::string finger_three_configuration_file = cedar::aux::locateResource("robots/sdh_finger_three.json", false);
    std::string palm_configuration_file = cedar::aux::locateResource("robots/sdh_palm.json", false);

    // create simulated kinematic chains
    cedar::dev::KinematicChainPtr caren_trunk(new cedar::dev::SimulatedKinematicChain());
    caren_trunk->readJson(trunk_configuration_file);
    CEDAR_UNIT_TEST_CONDITION_CUSTOM_MESSAGE(errors, caren_trunk->getNumberOfJoints() == 1, "Configuration in path " + trunk_configuration_file + " did not have the right amount of joints!");

    cedar::dev::KinematicChainPtr caren_arm(new cedar::dev::SimulatedKinematicChain());
    caren_arm->readJson(arm_configuration_file);
    CEDAR_UNIT_TEST_CONDITION_CUSTOM_MESSAGE(errors, caren_arm->getNumberOfJoints() == 7, "Configuration in path " + arm_configuration_file + " did not have the right amount of joints!");

    cedar::dev::KinematicChainPtr caren_head(new cedar::dev::SimulatedKinematicChain());
    caren_head->readJson(head_configuration_file);
    CEDAR_UNIT_TEST_CONDITION_CUSTOM_MESSAGE(errors, caren_head->getNumberOfJoints() == 2, "Configuration in path " + head_configuration_file + " did not have the right amount of joints!");

    cedar::dev::KinematicChainPtr finger_one(new cedar::dev::SimulatedKinematicChain());
    finger_one->readJson(finger_one_configuration_file);
    CEDAR_UNIT_TEST_CONDITION_CUSTOM_MESSAGE(errors, finger_one->getNumberOfJoints() == 2, "Configuration in path " + finger_one_configuration_file + " did not have the right amount of joints!");

    cedar::dev::KinematicChainPtr finger_two(new cedar::dev::SimulatedKinematicChain());
    finger_two->readJson(finger_two_configuration_file);
    CEDAR_UNIT_TEST_CONDITION_CUSTOM_MESSAGE(errors, finger_two->getNumberOfJoints() == 2, "Configuration in path " + finger_two_configuration_file + " did not have the right amount of joints!");

    cedar::dev::KinematicChainPtr finger_three(new cedar::dev::SimulatedKinematicChain());
    finger_three->readJson(finger_three_configuration_file);
    CEDAR_UNIT_TEST_CONDITION_CUSTOM_MESSAGE(errors, finger_three->getNumberOfJoints() == 2, "Configuration in path " + finger_three_configuration_file + " did not have the right amount of joints!");

    cedar::dev::KinematicChainPtr palm(new cedar::dev::SimulatedKinematicChain());
    palm->readJson(palm_configuration_file);
    CEDAR_UNIT_TEST_CONDITION_CUSTOM_MESSAGE(errors, palm->getNumberOfJoints() == 1, "Configuration in path " + palm_configuration_file + " did not have the right amount of joints!");

    // link kinematic chains to each other
    caren_trunk->setEndEffector(caren_arm->getRootCoordinateFrame());
    caren_arm->setEndEffector(palm->getRootCoordinateFrame());
  }

  return errors;
}
