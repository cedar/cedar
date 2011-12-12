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

    File:        Duration.cpp

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
#include "cedar/units/Time.h"

// PROJECT INCLUDES

// SYSTEM INCLUDES

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

/*! This constructor is protected to avoid the creation of Duration units by the user. This, in turn, protects its
 *  internal workings from being exposed an subject to potential misuse, as the internal representation of the duration
 *  could change at any moment.
 */
cedar::unit::Time::Time(double amount)
:
mAmountInMicroSeconds (amount)
{
}

cedar::unit::Time::~Time()
{
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

/*!@returns The duration, as represented internally in the Duration object.
 * @remarks This function should at most be used for testing, because the internal representation might change at any
 *          time without warning. To access the duration value, use the concrete duration types. E.g., let d be a
 *          duration object obtained from somewhere, then to get an actual duration out of it, use
 *          cedar::unit::Seconds(d).
 */
double cedar::unit::Time::getRawTime() const
{
  return this->mAmountInMicroSeconds;
}
