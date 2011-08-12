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

    File:        <filename>

    Maintainer:  Oliver Lomp,
                 Mathis Richter,
                 Stephan Zibner
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de,
                 mathis.richter@ini.ruhr-uni-bochum.de,
                 stephan.zibner@ini.ruhr-uni-bochum.de
    Date:        2011 06 03

    Description:

    Credits:

======================================================================================================================*/

// LOCAL INCLUDES
#include "dynamics/Dynamics.h"
#include "processing/StepTime.h"
#include "processing/exceptions.h"

// PROJECT INCLUDES

// SYSTEM INCLUDES
#include <iostream>

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------
cedar::dyn::Dynamics::Dynamics()
:
cedar::proc::Step(false, false)
{
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------
void cedar::dyn::Dynamics::compute(const cedar::proc::Arguments& arguments)
{
  try
  {
    const cedar::proc::StepTime& step_time = dynamic_cast<const cedar::proc::StepTime&>(arguments);
    this->eulerStep(step_time.getStepTime());
  }
  catch (const std::bad_cast& e)
  {
    //!@todo: Fix this. For now, it is disabled because it causes too many (unnecessary) crashes.
    // CEDAR_THROW(cedar::proc::InvalidArgumentsException, "Bad arguments passed to dynamics. Expected StepTime.");
    std::cout << "Warning: Bad arguments passed to dynamics. Expected StepTime. Skipping compute call." << std::endl;
  }
}
