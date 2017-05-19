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

    File:        sleepFunctions.h

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2011 07 24

    Description:

    Credits:

======================================================================================================================*/

#ifndef CEDAR_AUX_SLEEP_FUNCTIONS_H
#define CEDAR_AUX_SLEEP_FUNCTIONS_H

// CEDAR INCLUDES
#include "cedar/auxiliaries/lib.h"
#include "cedar/units/Time.h"

// SYSTEM INCLUDES


namespace cedar
{
  namespace aux
  {
    //!@brief cedar's own sleep function, which requires a Time object as sleep interval
    CEDAR_AUX_LIB_EXPORT void sleep(cedar::unit::Time time);
    //!@brief Windows does not ship its own usleep function - for compatibility reasons, here's the cedar version
    CEDAR_AUX_LIB_EXPORT void usleep(unsigned int microseconds);
  }
}

#endif // CEDAR_AUX_SLEEP_FUNCTIONS_H
