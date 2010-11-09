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
TestComponent::TestComponent(void)
:
cedar::dev::robot::Component()
{
}

//! destructor
TestComponent::~TestComponent(void)
{
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

