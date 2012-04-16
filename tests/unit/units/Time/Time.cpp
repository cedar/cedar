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


// CEDAR INCLUDES
#include "cedar/units/namespace.h"
#include "cedar/units/Time.h"
#include "cedar/units/TimeUnit.h"

// SYSTEM INCLUDES
#include <iostream>
#include <cmath>

void testStreamOutput(cedar::unit::Time time)
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
  unsigned int errors = 0;

  cedar::unit::Milliseconds ms(50);

  // ---- Conversion tests ---------------------------------------------------------------------------------------------
  std::cout << "Conversion from milliseconds to microseconds ";

  cedar::unit::Microseconds us = ms;
  if (ms.getRawTime() != us.getRawTime())
  {
    std::cout << "failed";
    ++errors;
  }
  else
  {
    std::cout << "succeeded";
  }
  std::cout << ": ms ~ " << ms.getRawTime() << ", us ~ " << us.getRawTime() << std::endl;


  // ---- String tests -------------------------------------------------------------------------------------------------
  std::cout << "Testing stream operator & function argument passing ... ";
  testStreamOutput(cedar::unit::Microseconds(12345));
  std::cout << "succeeded." << std::endl;

  // ---- operator tests -----------------------------------------------------------------------------------------------
  std::cout << "Testing operators ... " << std::endl;
  cedar::unit::Milliseconds ms_once(50);
  cedar::unit::Milliseconds ms_twice = 2.0 * ms_once;

  std::cout << "Multiplication operator ... ";
  if (2.0 * ms_once.getRawTime() != ms_twice.getRawTime())
  {
    std::cout << "failed: twice " << ms_once.getRawTime() << " is " << ms_twice.getRawTime() << std::endl;
    ++errors;
  }
  else
  {
    std::cout << "succeeded." << std::endl;
  }

  std::cout << "Division operator time/time with milliseconds... ";
  double division = ms_once/ms_twice;
  if (fabs(division - 0.5) > 0.00001)
  {
    std::cout << "failed: operator / returns " << division << " instead of 0.5" << std::endl;
    ++errors;
  }
  else
  {
    std::cout << "succeeded." << std::endl;
  }

  std::cout << "Division operator time/time with seconds/milliseconds... ";
  division = cedar::unit::Seconds(0.05)/ms_twice;
  if (fabs(division - 0.5) > 0.00001)
  {
    std::cout << "failed: operator / returns " << division << " instead of 0.5" << std::endl;
    ++errors;
  }
  else
  {
    std::cout << "succeeded." << std::endl;
  }

  std::cout << "Testing t += 1s ... ";
  cedar::unit::Seconds t(1);
  t += cedar::unit::Seconds(1);

  if (t / cedar::unit::Seconds(1) != 2.0)
  {
    std::cout << "failed!" << std::endl;
    ++errors;
  }
  else
  {
    std::cout << "succeeded." << std::endl;
  }


  // Comparison operators ----------------------------------------------------------------------------------------------
  std::cout << "Testing t1 < t2 with seconds ... ";
  cedar::unit::Seconds t1(1);
  cedar::unit::Seconds t2(2);
  if (t1 < t2)
  {
    std::cout << "succeeded." << std::endl;
  }
  else
  {
    std::cout << "failed!" << std::endl;
    ++errors;
  }

  std::cout << "Done. There were " << errors << " errors." << std::endl;
  return errors;
}
