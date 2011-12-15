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

    File:        Limits.h

    Maintainer:  Mathis Richter
    Email:       mathis.richter@ini.rub.de
    Date:        2010 11 11

    Description: Header for the @em cedar::aux::math::Limits struct.

    Credits:

======================================================================================================================*/

#ifndef CEDAR_AUX_MATH_LIMITS_H
#define CEDAR_AUX_MATH_LIMITS_H

#define NOMINMAX // to avoid Windows issues

// CEDAR INCLUDES
#include "cedar/auxiliaries/math/namespace.h"

// SYSTEM INCLUDES


/*!@brief Structure representing the limits of an interval.
 */
template <typename T>
struct cedar::aux::math::Limits
{
  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
  //!@brief The standard constructor
  Limits() {};
  //!@brief Constructor that takes a minimum and maximum value
  Limits(const T& newMin, const T& newMax)
  :
  min(newMin),
  max(newMax)
  {
  }
  
  //!@brief Copy constructor
  template <typename U>
  Limits(const Limits<U> &otherLimits)
  :
  min(otherLimits.min),
  max(otherLimits.max)
  {
  }

  //--------------------------------------------------------------------------------------------------------------------
  // methods
  //--------------------------------------------------------------------------------------------------------------------

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
  //! minimum limit
  T min;
  //! maximum limit
  T max;
}; // class cedar::aux::math::Limits

#endif // CEDAR_AUX_MATH_LIMITS_H
