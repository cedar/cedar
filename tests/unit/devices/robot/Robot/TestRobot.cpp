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
#include "cedar/devices/robot/Robot.h"
#include "cedar/devices/robot/Component.h"

// SYSTEM INCLUDES
#include <string>
#include <set>
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
  _mSubComponentNames["TestComponent1"] = std::set<std::string>();
  _mSubComponentNames["TestComponent2"] = std::set<std::string>();
}

//! destructor
TestRobot::~TestRobot()
{
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

ComponentPtr TestRobot::createComponent(const std::string& rComponentName)
{
  ComponentPtr test_component(new TestComponent(rComponentName));

  return test_component;
}


