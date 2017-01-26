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

    File:        main.cpp

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2011 07 24

    Description: 

    Credits:

======================================================================================================================*/


// LOCAL INCLUDES

// PROJECT INCLUDES
#include "cedar/auxiliaries/sleepFunctions.h"
#include "cedar/units/Time.h"
#include "cedar/units/prefixes.h"

// SYSTEM INCLUDES
#ifndef Q_MOC_RUN
  #include <boost/date_time/posix_time/posix_time_types.hpp>
#endif
#include <iostream>
#include <time.h>
#include <vector>
#include <boost/units/io.hpp>

bool checkTime(long expiredMilliseconds, cedar::unit::Time expectedTime)
{
  double expected_milliseconds = expectedTime / cedar::unit::Time(1.0 * cedar::unit::milli * cedar::unit::second);
  double threshold;
  if (expected_milliseconds > 1000)
  {
    threshold = 0.005 * expected_milliseconds; // be more tolerant with larger wait times
  }
  else
  {
    threshold = 8; // deviations of up to 5 milliseconds are allowed
                   // jokeit: the timer seems to be less accurate on Macs
  }
  return fabs(static_cast<double>(expiredMilliseconds) - expected_milliseconds) < threshold;
}

int main(int, char**)
{
  // the number of errors encountered in this test
  int errors = 0;

  // Test waiting for seconds
  std::vector<cedar::unit::Time> wait_times_seconds;
  wait_times_seconds.push_back(cedar::unit::Time(1.0 * cedar::unit::second));
  wait_times_seconds.push_back(cedar::unit::Time(10.0 * cedar::unit::seconds));
  
  std::cout << "Testing waiting for seconds." << std::endl;
  for (size_t i = 0; i < wait_times_seconds.size(); ++i)
  {
    cedar::unit::Time wait_time = wait_times_seconds.at(i);
    std::cout << "Waiting for " << wait_time << std::endl;
    boost::posix_time::ptime start = boost::posix_time::microsec_clock::universal_time();
    cedar::aux::sleep(wait_time);
    boost::posix_time::ptime end = boost::posix_time::microsec_clock::universal_time();

    boost::posix_time::time_duration expired_time = end - start;
    if (!checkTime(expired_time.total_milliseconds(), wait_time))
    {
      std::cout << "Sleep time deviated by more than the expected amount; " << expired_time.total_milliseconds() << " milliseconds have passed." << std::endl;
      errors++;
    }
    else
    {
      std::cout << "Ok: " << expired_time.total_milliseconds() << " milliseconds have passed." << std::endl;
    }
  }

  std::vector<cedar::unit::Time> wait_times_milliseconds;
  wait_times_milliseconds.push_back(cedar::unit::Time(10.0 * cedar::unit::milli * cedar::unit::seconds));
  wait_times_milliseconds.push_back(cedar::unit::Time(100.0 * cedar::unit::milli * cedar::unit::seconds));
  wait_times_milliseconds.push_back(cedar::unit::Time(1000.0 * cedar::unit::milli * cedar::unit::seconds));

  std::cout << "Testing waiting for milliseconds." << std::endl;
  for (size_t i = 0; i < wait_times_milliseconds.size(); ++i)
  {
    cedar::unit::Time wait_time = wait_times_milliseconds.at(i);
    std::cout << "Waiting for " << wait_time << std::endl;
    boost::posix_time::ptime start = boost::posix_time::microsec_clock::universal_time();
    cedar::aux::sleep(wait_time);
    boost::posix_time::ptime end = boost::posix_time::microsec_clock::universal_time();

    boost::posix_time::time_duration expired_time = end - start;
    if (!checkTime(expired_time.total_milliseconds(), wait_time))
    {
      std::cout << "Sleep time deviated by more than the expected amount; only " << expired_time.total_milliseconds() << " milliseconds have passed."
               << " (should have been " << wait_time << ")" << std::endl;
      errors++;
    }
    else
    {
      std::cout << "Ok: " << expired_time.total_milliseconds() << " milliseconds have passed." << std::endl;
    }
  }

  std::cout << "test finished, there were " << errors << " errors" << std::endl;
  if (errors > 255)
  {
    errors = 255;
  }
  return errors;
}
