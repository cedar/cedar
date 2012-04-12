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
  Limits(const T& newLowerLimit, const T& newUpperLimit)
  :
  mLowerLimit(newLowerLimit),
  mUpperLimit(newUpperLimit)
  {
  }
  
  //!@brief Copy constructor
  template <typename U>
  Limits(const Limits<U> &otherLimits)
  :
  mLowerLimit(otherLimits.mLowerLimit),
  mUpperLimit(otherLimits.mUpperLimit)
  {
  }

  //--------------------------------------------------------------------------------------------------------------------
  // methods
  //--------------------------------------------------------------------------------------------------------------------

  /*!@brief Checks whether a given value is within the limits.
   * @param[in] value the value to be checked
   */
  bool isInLimits(const T& value)
  {
    if (value < mLowerLimit || value > mUppperLimit)
    {
      return false;
    }
    return true;
  }

  /*!@brief Tresholds a value if it is outside of the limits.
   * If the value is below the lower limit, the method will set the value to the lower limit.
   * If the value is above the upper limit, the method will set the value to the upper limit.
   * If the value is within the limits, the method will not change the value.
   * @param[in] value the value to be thresholded
   */
  void threshold(const T& value)
  {
    if (value < mLowerLimit)
    {
      value = mLowerLimit;
    }
    else if (value > mUpperLimit)
    {
      value = mUpperLimit;
    }
  }

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
  //! minimum limit
  T mLowerLimit;
  //! maximum limit
  T mUpperLimit;
}; // class cedar::aux::math::Limits

#endif // CEDAR_AUX_MATH_LIMITS_H
