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

#include "auxiliaries/sleepFunctions.h"
#include "auxiliaries/macros.h"

#ifdef WINDOWS
  #include <Windows.h>
#else
  #include <unistd.h>
#endif // WINDOWS

void cedar::aux::sleep(cedar::unit::Duration time)
{
  cedar::unit::Microseconds us(time);
  cedar::aux::usleep(static_cast<unsigned int>(us.getRawTime()));
}

#include <iostream>
void cedar::aux::usleep(unsigned int microseconds)
{
#ifdef WINDOWS

#ifdef MSVC
#pragma message("Warning: Windows can only sleep for milliseconds. Anything lower will be ignored!")
#elif defined GCC
#warning "Warning: Windows can only sleep for milliseconds. Anything lower will be ignored!"
#endif // MSVC/GCC

  Sleep(static_cast<DWORD>(microseconds/1000));

#else // WINDOWS

  ::usleep(microseconds);

#endif // WINDOWS
}

void cedar::aux::sleep(unsigned int seconds)
{
#ifdef WINDOWS
  Sleep(1000 * static_cast<DWORD>(seconds));
#else
  ::sleep(seconds);
#endif
}
