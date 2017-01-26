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

    File:        measurementFunctions.h

    Maintainer:  Jean-Stephane Jokeit
    Email:       jean-stephane.jokeit@ini.ruhr-uni-bochum.de
    Date:        2013 06 21

    Description: Shared testing (unit-tests) code for writing (dart) measurements that can be interpreted by the build
                 server.

    Credits:

======================================================================================================================*/

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

#ifndef CEDAR_TESTING_UTILITIES_MEASUREMENT_FUNCTIONS_H
#define CEDAR_TESTING_UTILITIES_MEASUREMENT_FUNCTIONS_H

// CEDAR INCLUDES
#include "cedar/testingUtilities/lib.h"

// SYSTEM INCLUDES
#include <functional>
#include <string>

namespace cedar
{
  namespace test
  {
    /*! Writes a test measurement with the given id and duration to the cout stream in a manner that can automatically
     *  be parsed by the build server.
     */
    CEDAR_TESTING_UTILITIES_LIB_EXPORT void write_measurement
                                       (
                                         const std::string& id,
                                         double duration
                                       );

    /*! Measures the time it takes to run the given function and writes the measurement to the cout stream in a manner
     *  that can automatically be parsed by the build server.
     *
     *  @see write_measurement
     */
    CEDAR_TESTING_UTILITIES_LIB_EXPORT void test_time
                                       (
                                         std::string id,
                                         std::function<void()> fun,
                                         unsigned int repetitions = 1
                                       );
  }
}

#endif // CEDAR_TESTING_UTILITIES_MEASUREMENT_FUNCTIONS_H
