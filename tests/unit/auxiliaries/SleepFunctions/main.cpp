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
#include "cedar/auxiliaries/LogFile.h"

// SYSTEM INCLUDES
#include <boost/date_time/posix_time/posix_time_types.hpp>
#include <iostream>
#include <time.h>
#include <vector>

int main()
{
  using cedar::aux::LogFile;
  LogFile log_file("SleepFunctions.log");
  log_file.addTimeStamp();
  log_file << std::endl;
  // the number of errors encountered in this test
  int errors = 0;

  // Test waiting for seconds
  std::vector<double> wait_times_seconds;
  wait_times_seconds.push_back(1);
  wait_times_seconds.push_back(10);
  
  log_file << "Testing waiting for seconds." << std::endl;
  for (size_t i = 0; i < wait_times_seconds.size(); ++i)
  {
    cedar::unit::Seconds wait_time(wait_times_seconds.at(i));
    log_file.addTimeStamp();
    log_file << "Waiting for " << wait_time << std::endl;
    boost::posix_time::ptime start = boost::posix_time::microsec_clock::universal_time();
    cedar::aux::sleep(wait_time);
    boost::posix_time::ptime end = boost::posix_time::microsec_clock::universal_time();

    boost::posix_time::time_duration expired_time = end - start;
    if (expired_time.total_milliseconds() < static_cast<long>(1000.0 * wait_times_seconds.at(i)))
    {
      log_file << "Sleep didn't wait long enough; only " << expired_time.total_milliseconds() << " milliseconds have passed." << std::endl;
      errors++;
    }
    else
    {
      log_file << "Ok: " << expired_time.total_milliseconds() << " milliseconds have passed." << std::endl;
    }
  }

  std::vector<double> wait_times_milliseconds;
  wait_times_milliseconds.push_back(10);
  wait_times_milliseconds.push_back(100);
  wait_times_milliseconds.push_back(1000);

  log_file << "Testing waiting for milliseconds." << std::endl;
  for (size_t i = 0; i < wait_times_milliseconds.size(); ++i)
  {
    cedar::unit::Milliseconds wait_time(wait_times_milliseconds.at(i));
    log_file.addTimeStamp();
    log_file << "Waiting for " << wait_time << std::endl;
    boost::posix_time::ptime start = boost::posix_time::microsec_clock::universal_time();
    cedar::aux::sleep(wait_time);
    boost::posix_time::ptime end = boost::posix_time::microsec_clock::universal_time();

    boost::posix_time::time_duration expired_time = end - start;
    if (expired_time.total_milliseconds() < static_cast<long>(wait_times_milliseconds.at(i)))
    {
      log_file << "Sleep didn't wait long enough; only " << expired_time.total_milliseconds() << " milliseconds have passed."
               << " (should have been " << wait_times_milliseconds.at(i) / 1000.0 << ")" << std::endl;
      errors++;
    }
    else
    {
      log_file << "Ok: " << expired_time.total_milliseconds() << " milliseconds have passed." << std::endl;
    }
  }

  log_file << "test finished, there were " << errors << " errors" << std::endl;
  if (errors > 255)
  {
    errors = 255;
  }
  return errors;
}
