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

    Institute:   Ruhr-Universitaet Bochum
                 Institut fuer Neuroinformatik

    File:        TestRobot.cpp

    Maintainer:  Mathis Richter
    Email:       mathis.richter@ini.rub.de
    Date:        2010 11 08

    Description: Implementation of the @em TestRobot class.

    Credits:

======================================================================================================================*/

// LOCAL INCLUDES
#include "unit/devices/robot/Robot/TestRobot.h"
#include "unit/devices/robot/Robot/TestComponent.h"

// PROJECT INCLUDES
#include "cedar/devices/robot/Robot.h"
#include "cedar/devices/robot/Component.h"

// SYSTEM INCLUDES
#include <string>
#include <set>
#include <iostream>


//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

//! constructor
cedar::tests::unit::dev::robot::Robot::TestRobot::TestRobot(void)
:
cedar::dev::robot::Robot()
{
  _mSubComponentNames["TestComponent1"] = std::set<std::string>();
  _mSubComponentNames["TestComponent2"] = std::set<std::string>();
}

//! destructor
cedar::tests::unit::dev::robot::Robot::TestRobot::~TestRobot()
{
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

cedar::dev::robot::ComponentPtr
  cedar::tests::unit::dev::robot::Robot::TestRobot::createComponent(const std::string& rComponentName)
{
  cedar::dev::robot::ComponentPtr test_component
  (
    new cedar::tests::unit::dev::robot::Robot::TestComponent(rComponentName)
  );

  return test_component;
}


