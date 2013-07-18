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

    File:        measurementFunctions.cpp

    Maintainer:  Jean-Stephane Jokeit
    Email:       jean-stephane.jokeit@ini.rub.de
    Date:        2013 06 19

    Description: Shared testing code for writing (dart) measurements that can be interpreted by the build server.

    Credits:

======================================================================================================================*/

// CEDAR_INCLUDES
#include "cedar/testing/measurementFunctions.h"
#include "cedar/auxiliaries/assert.h"
#include "cedar/auxiliaries/exceptions.h"
#include "cedar/auxiliaries/stringFunctions.h"

// SYSTEM INCLUDES
#include <functional>
#include <boost/date_time/posix_time/posix_time.hpp>

void cedar::testing::write_measurement
     (
       const std::string& id,
       double duration
     )
{
  std::cout
      << "<DartMeasurement"
      << " name=\""
      << id
      << " (seconds)\""
      << " type=\"numeric/double\">"
      << cedar::aux::toString(duration)
      << "</DartMeasurement>"
      << std::endl;
}

void cedar::testing::test_time(std::string id, std::function< void() > fun)
{
  using boost::posix_time::ptime;
  using boost::posix_time::microsec_clock;

  double duration;

  auto start = microsec_clock::local_time();

  // execute the test:
  fun();

  auto end = microsec_clock::local_time();

  duration = static_cast<double>((end - start).total_milliseconds()) / 1000.0;
  write_measurement(id, duration);
}


