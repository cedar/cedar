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

    File:        TestComponent.cpp

    Maintainer:  Mathis Richter
    Email:       mathis.richter@ini.rub.de
    Date:        2010 11 08

    Description: Implementation of the @em TestComponent class.

    Credits:

======================================================================================================================*/


// LOCAL INCLUDES
#include "unit/devices/robot/Robot/TestComponent.h"

// PROJECT INCLUDES
#include "cedar/devices/robot/Component.h"

// SYSTEM INCLUDES
#include <string>
#include <iostream>


using namespace cedar::tests::unit::dev::robot::Robot;
using namespace cedar::dev::robot;

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

//! constructor
TestComponent::TestComponent(const std::string& rTestName)
:
cedar::dev::robot::Component(),
mTestName(rTestName)
{
}

//! destructor
TestComponent::~TestComponent(void)
{
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

const std::string& TestComponent::getTestName(void) const
{
  return mTestName;
}
