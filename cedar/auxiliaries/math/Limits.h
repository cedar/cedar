/*======================================================================================================================

    Copyright 2011, 2012, 2013 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany

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
#include "cedar/auxiliaries/assert.h"
#include "cedar/auxiliaries/Log.h"

// SYSTEM INCLUDES
#ifndef Q_MOC_RUN
  #include <boost/numeric/conversion/bounds.hpp>
#endif
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
  //!@brief formatted output of a Limit to a stream
  friend std::ostream& operator<<(std::ostream& stream, const cedar::aux::math::Limits<T>& limits)
  {
    stream << "[" << limits.getLower() << ", " << limits.getUpper() << "]";
    return stream;
  }

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
  //!@brief Default constructor
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
  //! Returns the length of the interval, i.e., upper - lower.
  inline T getLength() const
  {
    CEDAR_DEBUG_ASSERT(this->getUpper() >= this->getLower());

    return this->getUpper() - this->getLower();
  }

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

  //! Returns an interval from lower to infinity, i.e., the interval [lower, inf)
  static Limits fromLower(const T& lower)
  {
    return Limits(lower, boost::numeric::bounds<T>::highest());
  }

  /*! Returns an interval from -infinity to upper, i.e., the interval (-Inf, lower]
   *
   * @remarks For unsigned values, -Inf is replaced by zero.
   */
  static Limits toUpper(const T& upper)
  {
    return Limits(boost::numeric::bounds<T>::lowest(), upper);
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
  inline const T& limit(const T& value) const
  {
    if (value < getLower())
    {
      return getLower();
    }
    else if (value > getUpper())
    {
      return getUpper();
    }

    return value;
  }

  /*!@brief Tresholds a vector of values against the limits.
   * @param[in,out] values the vector of values to be thresholded
   */
  inline void limit(std::vector<T>& values) const
  {
    for
    (
      typename std::vector<T>::iterator it = values.begin();
      it != values.end();
      ++it
    )
    {
      *it = this->limit(*it);
    }
  }

  //--------------------------------------------------------------------------------------------------------------------
  // operators
  //--------------------------------------------------------------------------------------------------------------------
  //!@brief != operator for limits
  bool operator!= (const Limits<T>& other) const
  {
    return !(other == *this);
  }

  //!@brief == operator for limits
  bool operator== (const Limits<T>& other) const
  {
    return (other.getLower() == this->getLower()) && (other.getUpper() == this->getUpper());
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
