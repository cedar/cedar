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

    Institute:   Ruhr-Universitaet Bochum
                 Institut fuer Neuroinformatik

    File:        main.cpp

    Maintainer:  Mathis Richter
    Email:       mathis.richter@ini.rub.de
    Date:        2010 11 08

    Description: Implements all unit tests for the @em cedar::dev::Robot class.

    Credits:

======================================================================================================================*/


// LOCAL INCLUDES
#include "unit/devices/Robot/namespace.h"

// PROJECT INCLUDES
#include "cedar/devices/Robot.h"
#include "cedar/devices/ComponentSlot.h"
#include "unit/devices/Robot/TestComponent.h"

// SYSTEM INCLUDES
#include <string>


int main()
{
  int errors = 0;


  cedar::dev::RobotPtr robot(new cedar::dev::Robot());
  robot->readJson("RobotConfiguration.json");

  robot->setChannel("channel 1");

  std::vector<std::string> slot_list = robot->listComponentSlots();

  for (auto slot_iter = slot_list.begin(); slot_iter != slot_list.end(); ++slot_iter)
  {
    const std::string& slot_name = *slot_iter;
    cedar::dev::ComponentSlotPtr slot = robot->getComponentSlot(slot_name);
    std::cout << "Slot: " << slot_name << std::endl;
    std::cout << slot << std::endl;
  }

  std::vector<std::string> channel_list = robot->listChannels();

  for (auto channel_iter = channel_list.begin(); channel_iter != channel_list.end(); ++channel_iter)
  {
    std::cout << "Channel: " << (*channel_iter) << std::endl;
  }

  cedar::tests::unit::dev::Robot::TestComponentPtr component
    = robot->getComponent<cedar::tests::unit::dev::Robot::TestComponent>("component 1");

  if (!component)
  {
    std::cout << "Error: could not retrieve \"component 1\"" << std::endl;
    ++errors;
  }
  else
  {
    if (component->_mParameter1->getValue() != 1)
    {
      std::cout << "Error: parameter 1 has the wrong value (should be 1, is "
                << component->_mParameter1->getValue() << ")" << std::endl;
      ++errors;
    }

    if (component->_mParameter2->getValue() != 2)
    {
      std::cout << "Error: parameter 2 has the wrong value (should be 2, is "
                << component->_mParameter2->getValue() << ")" << std::endl;
      ++errors;
    }
  }

  return errors;
}
