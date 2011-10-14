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

    File:        ListenerManagement.cpp

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

// PROJECT INCLUDES
#include "cedarUnits.h"
#include "auxiliaries/LogFile.h"

// SYSTEM INCLUDES
#include <iostream>
#include <cmath>

void testStreamOutput(cedar::unit::Duration time)
{
  std::cout << "Time output test: " << std::endl;
  std::cout << cedar::unit::Microseconds(time)
            << " = "
            << cedar::unit::Milliseconds(time)
            << " = "
            << cedar::unit::Seconds(time)
            << std::endl;
}

int main(int /* argc */, char** /* argv */)
{
  using namespace cedar::aux;
  unsigned int errors = 0;

  LogFile log_file("Time.log");
  log_file.addTimeStamp();
  log_file << std::endl;

  cedar::unit::Milliseconds ms(50);

  // ---- Conversion tests ---------------------------------------------------------------------------------------------
  log_file << "Conversion from milliseconds to microseconds ";

  cedar::unit::Microseconds us = ms;
  if (ms.getRawTime() != us.getRawTime())
  {
    log_file << "failed";
    ++errors;
  }
  else
  {
    log_file << "succeeded";
  }
  log_file << ": ms ~ " << ms.getRawTime() << ", us ~ " << us.getRawTime() << std::endl;


  // ---- String tests -------------------------------------------------------------------------------------------------
  log_file << "Testing stream operator & function argument passing ... ";
  testStreamOutput(cedar::unit::Microseconds(12345));
  log_file << "succeeded." << std::endl;

  // ---- operator tests -----------------------------------------------------------------------------------------------
  log_file << "Testing operators ... " << std::endl;
  cedar::unit::Milliseconds ms_once(50);
  cedar::unit::Milliseconds ms_twice = 2.0 * ms_once;

  log_file << "Multiplication operator ... ";
  if (2.0 * ms_once.getRawTime() != ms_twice.getRawTime())
  {
    log_file << "failed: twice " << ms_once.getRawTime() << " is " << ms_twice.getRawTime() << std::endl;
    ++errors;
  }
  else
  {
    log_file << "succeeded." << std::endl;
  }

  log_file << "Division operator time/time with milliseconds... ";
  double division = ms_once/ms_twice;
  if (fabs(division - 0.5) > 0.00001)
  {
    log_file << "failed: operator / returns " << division << " instead of 0.5" << std::endl;
    ++errors;
  }
  else
  {
    log_file << "succeeded." << std::endl;
  }

  log_file << "Division operator time/time with seconds/milliseconds... ";
  division = cedar::unit::Seconds(0.05)/ms_twice;
  if (fabs(division - 0.5) > 0.00001)
  {
    log_file << "failed: operator / returns " << division << " instead of 0.5" << std::endl;
    ++errors;
  }
  else
  {
    log_file << "succeeded." << std::endl;
  }


  log_file << "Done. There were " << errors << " errors." << std::endl;
  return errors;
}
