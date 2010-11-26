/*----------------------------------------------------------------------------------------------------------------------
 ----- Institute:   Ruhr-Universitaet Bochum
                    Institut fuer Neuroinformatik

 ----- File:        TestComponent.h

 ----- Author:      Mathis Richter
 ----- Email:       mathis.richter@ini.rub.de
 ----- Date:        2010 11 08

 ----- Description: Header for the @em TestComponent class.

 ----- Credits:
 ---------------------------------------------------------------------------------------------------------------------*/

// LOCAL INCLUDES
#include "TestComponent.h"

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
