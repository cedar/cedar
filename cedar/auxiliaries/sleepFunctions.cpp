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

// SYSTEM INCLUDES
#ifdef _WIN32
  #include <Windows.h>
#else
  #include <unistd.h>
#endif // _WIN32

void cedar::aux::sleep(cedar::unit::Time time)
{
  cedar::unit::Microseconds us(time);
  cedar::aux::usleep(static_cast<unsigned int>(us.getRawTime()));
}

#include <iostream>
void cedar::aux::usleep(unsigned int microseconds)
{
#ifdef _WIN32

#ifdef _MSC_VER
#pragma message("Warning: Windows can only sleep for milliseconds. Anything lower will be ignored!")
#elif defined __GNUG__
#warning "Warning: Windows can only sleep for milliseconds. Anything lower will be ignored!"
#endif // _MSC_VER/__GNUG__

  Sleep(static_cast<DWORD>(microseconds/1000));

#else // _WIN32

  ::usleep(microseconds);

#endif // _WIN32
}

void cedar::aux::sleep(unsigned int seconds)
{
#ifdef _WIN32
  Sleep(1000 * static_cast<DWORD>(seconds));
#else
  ::sleep(seconds);
#endif
}
