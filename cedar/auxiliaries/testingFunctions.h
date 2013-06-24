/*======================================================================================================================

    Copyright 2011, 2012, 2013 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany
 
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

    File:        testingFunctions.h

    Maintainer:  Jean-Stephane Jokeit
    Email:       jean-stephane.jokeit@ini.ruhr-uni-bochum.de
    Date:        2013 06 21

    Description: Shared testing (unit-tests) code.

    Credits:

======================================================================================================================*/

// CEDAR CONFIGURATION
#include "cedar/configuration.h"
#include "cedar/auxiliaries/namespace.h"

#ifndef CEDAR_AUX_TESTING_FUNCTIONS_H
#define CEDAR_AUX_TESTING_FUNCTIONS_H

// CEDAR INCLUDES

// SYSTEM INCLUDES
namespace cedar
{
  namespace aux
  {
    namespace testing
    {
      //!@brief fills a matrix with a Gaussian
      CEDAR_AUX_LIB_EXPORT void write_measurement
                                (
                                  const std::string& id,
                                  double duration
                                );
      CEDAR_AUX_LIB_EXPORT void test_time
                                (
                                  std::string id, 
                                  std::function< void() > fun
                                );
    }
  }
}

#endif // CEDAR_AUX_STRING_FUNCTIONS_H
