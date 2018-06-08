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

    File:        sleepFunctions.cpp

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2011 07 24

    Description:

    Credits:

======================================================================================================================*/

// CEDAR INCLUDES
#include "cedar/auxiliaries/sleepFunctions.h"
#include "cedar/defines.h"
#include "cedar/units/Time.h"
#include "cedar/units/prefixes.h"

// SYSTEM INCLUDES
#ifdef CEDAR_OS_WINDOWS
  #include <Windows.h>
#else
  #include <unistd.h>
#endif // CEDAR_OS_WINDOWS
#include <iostream>

void cedar::aux::sleep(cedar::unit::Time time)
{
  cedar::unit::Time microsecond(1.0 * cedar::unit::micro * cedar::unit::second);
  double time_in_microseconds = time / microsecond;

  if (time_in_microseconds < 0) // this saves lives!
    return;

  cedar::aux::usleep(static_cast<unsigned int>(time_in_microseconds));
}

void cedar::aux::usleep(unsigned int microseconds)
{
#ifdef CEDAR_OS_WINDOWS

#ifdef CEDAR_COMPILER_MSVC
#pragma message("Warning: Windows can only sleep for milliseconds. Anything lower will be ignored!")
#elif defined CEDAR_COMPILER_GCC
#warning "Warning: Windows can only sleep for milliseconds. Anything lower will be ignored!"
#endif // CEDAR_COMPILER_MSVC/CEDAR_COMPILER_GCC

  Sleep(static_cast<DWORD>(microseconds/1000));

#else // CEDAR_OS_WINDOWS

  ::usleep(microseconds);

#endif // CEDAR_OS_WINDOWS
}
