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
#include "unit/devices/Robot/TestChannel.h"

// SYSTEM INCLUDES
#include <string>

const std::string component_slot_1_name = "component slot 1";

int testConfiguration
    (
      const std::string& configFileName,
      const std::string& expectedChannel,
      const std::string& expectedChannelInfo,
      unsigned int expectedParameter1Value
    )
{
  int errors = 0;
  cedar::dev::RobotPtr robot(new cedar::dev::Robot());
  robot->readJson(configFileName);

  cedar::tests::unit::dev::Robot::TestComponentPtr component
    = robot->getComponent<cedar::tests::unit::dev::Robot::TestComponent>(component_slot_1_name);

  if (!component)
  {
    std::cout << "Error: could not retrieve \"" << component_slot_1_name << "\"" << std::endl;
    ++errors;
  }
  else
  {
    if (component->_mParameter1->getValue() != expectedParameter1Value)
    {
      std::cout << "Error: parameter 1 has the wrong value (should be " << expectedParameter1Value << ", is "
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

  cedar::dev::ChannelPtr channel = robot->getChannel(expectedChannel);
  if (!channel)
  {
    std::cout << "ERROR: channel was not instantiated properly." << std::endl;
    ++errors;
  }

  cedar::tests::unit::dev::Robot::TestChannelPtr test_channel
    = boost::dynamic_pointer_cast<cedar::tests::unit::dev::Robot::TestChannel>(channel);
  if (!test_channel)
  {
    std::cout << "ERROR: wrong channel type was instantiated." << std::endl;
    ++errors;
  }
  else
  {
    if (test_channel->_mInfo->getValue() != expectedChannelInfo)
    {
      std::cout << "ERROR: Wrong channel info; expceted: \"" << expectedChannelInfo
          << "\", got: \"" << test_channel->_mInfo->getValue() << "\"." << std::endl;
      ++errors;
    }
  }

  std::cout << "Final robot:" << std::endl;
  std::cout << robot << std::endl;

  std::cout << "Configuration \"" << configFileName << "\" revealed " << errors << " error(s)." << std::endl;
  return errors;
}

int main()
{
  int errors = 0;

  errors += testConfiguration("test://unit/devices/Robot/RobotConfiguration_1.json", "channel 1", "channel 1 info 1", 1);
  errors += testConfiguration("test://unit/devices/Robot/RobotConfiguration_2.json", "channel 2", "channel 2 info 2", 2);

  return errors;
}
