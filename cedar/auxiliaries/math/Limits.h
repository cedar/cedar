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

    Description: Header for the @em cedar::aux::math::Limits class.

    Credits:

======================================================================================================================*/

#ifndef CEDAR_AUX_MATH_LIMITS_H
#define CEDAR_AUX_MATH_LIMITS_H

#define NOMINMAX // to avoid Windows issues

// CEDAR INCLUDES
#include "cedar/auxiliaries/math/namespace.h"
#include "cedar/auxiliaries/Log.h"

// SYSTEM INCLUDES
#include <boost/numeric/conversion/bounds.hpp>
#include <limits>
#include <iostream>


/*!@brief Structure representing the limits of an interval.
 */
template <typename T>
class cedar::aux::math::Limits
{
public:
  //--------------------------------------------------------------------------------------------------------------------
  // friends
  //--------------------------------------------------------------------------------------------------------------------
  friend std::ostream& operator<<(std::ostream& stream, const cedar::aux::math::Limits<T>& limits)
  {
    stream << "[" << limits.getLower() << ", " << limits.getUpper() << "]";
    return stream;
  }

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
  //@brief Default constructor
  Limits()
  :
  mLowerLimit(0),
  mUpperLimit(1)
  {
  }

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
public:
  /*!@brief Checks whether a given value is within the limits.
   * @param[in] number the value to be checked
   */
  inline bool includes(const T& number)
  {
    return number >= this->getLower() && number <= this->getUpper();
  }

  //!@brief Returns the lower bound of the limits.
  inline const T& getLower() const
  {
    return this->mLowerLimit;
  }

  //!@brief Sets the lower bound of the limits.
  inline void setLower(const T& value)
  {
    this->mLowerLimit = value;
  }

  //!@brief Returns the lower bound of the limits.
  inline const T& getUpper() const
  {
    return this->mUpperLimit;
  }

  //!@brief Sets the upper bound of the limits.
  inline void setUpper(const T& value)
  {
    this->mUpperLimit = value;
  }

  //!@brief Constructs a limits object that covers only the positive interval (including 0).
  static Limits positiveZero(const T& upper = boost::numeric::bounds<T>::highest())
  {
    return Limits(0, upper);
  }

  //!@brief Constructs a limits object that covers only the positive interval, excluding 0.
  static Limits positive(const T& upper = boost::numeric::bounds<T>::highest())
  {
    // because smallest == 0 for integers, we have to differentiate here
    if (std::numeric_limits<T>::is_integer)
    {
      return Limits(1, upper);
    }
    else
    {
      return Limits(boost::numeric::bounds<T>::smallest(), upper);
    }
  }

  //!@brief Constructs a limits object that covers only the negative interval (including 0).
  static Limits negativeZero(const T& lower = boost::numeric::bounds<T>::lowest())
  {
    return Limits(lower, 0);
  }

  //!@brief Constructs a limits object that covers only the negative interval, excluding 0.
  static Limits negative(const T& lower = boost::numeric::bounds<T>::lowest())
  {
    // because smallest == 0 for integers, we have to differentiate here
    if (std::numeric_limits<T>::is_integer)
    {
      return Limits(lower, -1);
    }
    else
    {
      return Limits(lower, -boost::numeric::bounds<T>::smallest());
    }
  }

  //!@brief Returns a limits object that covers the full range of values.
  static Limits full()
  {
    return Limits(boost::numeric::bounds<T>::lowest(), boost::numeric::bounds<T>::highest());
  }

  /*!@brief Tresholds a value if it is outside of the limits.
   * If the value is below the lower limit, the method will set the value to the lower limit.
   * If the value is above the upper limit, the method will set the value to the upper limit.
   * If the value is within the limits, the method will not change the value.
   * @param[in,out] value the value to be thresholded
   */
  void threshold(T& value, bool warnOnThresholding = true) const
  {
    if (value < mLowerLimit)
    {
      value = mLowerLimit;
      if (warnOnThresholding)
      {
        cedar::aux::LogSingleton::getInstance()->warning
        (
          "Thresholding a value to the lower limit.",
          "cedar::aux::math::Limits",
          "Tresholding"
        );
      }
    }
    else if (value > mUpperLimit)
    {
      value = mUpperLimit;
      if (warnOnThresholding)
      {
        cedar::aux::LogSingleton::getInstance()->warning
        (
          "Thresholding a value to the upper limit.",
          "cedar::aux::math::Limits",
          "Tresholding"
        );
      }
    }
  }

  /*!@brief Tresholds a vector of values against the limits.
   * @param[in,out] values the vector of values to be thresholded
   */
  void threshold(std::vector<T>& values, bool warnOnThresholding = true) const
  {
    for
    (
      typename std::vector<T>::iterator it = values.begin();
      it != values.end();
      ++it
    )
    {
      threshold(*it, warnOnThresholding);
    }
  }

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet
private:
  //! minimum limit
  T mLowerLimit;
  //! maximum limit
  T mUpperLimit;
}; // class cedar::aux::math::Limits

#endif // CEDAR_AUX_MATH_LIMITS_H
