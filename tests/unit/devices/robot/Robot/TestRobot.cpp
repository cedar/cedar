/*----------------------------------------------------------------------------------------------------------------------
 ----- Institute:   Ruhr-Universitaet Bochum
                    Institut fuer Neuroinformatik

 ----- File:        TestRobot.h

 ----- Author:      Mathis Richter
 ----- Email:       mathis.richter@ini.rub.de
 ----- Date:        2010 11 08

 ----- Description: Header for the @em TestRobot class.

 ----- Credits:
 ---------------------------------------------------------------------------------------------------------------------*/

// LOCAL INCLUDES
#include "TestRobot.h"
#include "TestComponent.h"

// PROJECT INCLUDES
#include "src/devices/robot/Robot.h"
#include "src/devices/robot/Component.h"

// SYSTEM INCLUDES
#include <string>
#include <iostream>


using namespace cedar::tests::unit::dev::robot::Robot;
using namespace cedar::dev::robot;

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

//! constructor
TestRobot::TestRobot(void)
:
cedar::dev::robot::Robot()
{
}

//! destructor
TestRobot::~TestRobot()
{
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

ComponentPtr& TestRobot::getComponent(const std::string& componentName)
{
  ComponentPtr test_component(new TestComponent());
  return test_component;
}
