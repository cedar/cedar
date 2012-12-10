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

// SYSTEM INCLUDES
#include <string>


int main()
{
  cedar::dev::RobotPtr robot(new cedar::dev::Robot());
//  robot->setDescriptionPath("RobotDescription.json"); // optional
  robot->readJson("RobotConfiguration.json");

  std::vector<std::string> slot_list = robot->listComponentSlots();

  for (auto slot_iter = slot_list.begin(); slot_iter != slot_list.end(); ++slot_iter)
  {
    std::cout << "Slot: " << (*slot_iter) << std::endl;
  }

  return 0;
}
