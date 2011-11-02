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

    File:        DurationUnit.h

    Maintainer:  Oliver Lomp,
                 Mathis Richter,
                 Stephan Zibner
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de,
                 mathis.richter@ini.ruhr-uni-bochum.de,
                 stephan.zibner@ini.ruhr-uni-bochum.de
    Date:        2011 05 23

    Description:

    Credits:

======================================================================================================================*/

#ifndef CEDAR_UNITS_DURATION_UNIT_H
#define CEDAR_UNITS_DURATION_UNIT_H

// LOCAL INCLUDES
#include "cedar/units/namespace.h"
#include "cedar/units/Duration.h"

// PROJECT INCLUDES

// SYSTEM INCLUDES
#include <iostream>


/*!@brief Abstract description of the class.
 *
 * More detailed description of the class.
 *
 * @todo Read units from strings.
 */
template <unsigned int T_factor, const char* T_suffix>
class cedar::unit::DurationUnit : public Duration
{
  //--------------------------------------------------------------------------------------------------------------------
  // macros
  //--------------------------------------------------------------------------------------------------------------------

  //--------------------------------------------------------------------------------------------------------------------
  // friends
  //--------------------------------------------------------------------------------------------------------------------

  friend std::ostream& operator <<(std::ostream &stream, const DurationUnit& unit)
  {
    stream << unit.mAmountInMicroSeconds / static_cast<double>(T_factor) << " " << T_suffix;
    return stream;
  }

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  /*!@brief The standard constructor.
   *
   */
  DurationUnit(double amount = 1.0)
  :
  Duration(amount * static_cast<double>(T_factor))
  {
  }

  /*!@brief Constructor that takes a base time object.
   *
   */
  DurationUnit(const cedar::unit::Duration& time)
  :
  Duration(time)
  {
  }

  //!@brief Destructor

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
  /*!@brief conversion method from one TimeUnit to another
   * @return converted TimeUnit
   */
  template <unsigned int otherFactor, const char* otherSuffix>
  operator DurationUnit<otherFactor, otherSuffix>()
  const
  {
    double value = this->getRawTime() / otherFactor;
    return DurationUnit<otherFactor, otherSuffix>(value);
  }

  /*!@brief star operator for multiplying a TimeUnit with a scalar value on right side
   * @return result of multiplication
   */
  DurationUnit operator* (double factor) const
  {
    DurationUnit ret;
    ret.mAmountInMicroSeconds = factor * this->mAmountInMicroSeconds;
    return ret;
  }

  /*!@brief star operator for multiplying a TimeUnit with a scalar value on left side
   * @return result of multiplication
   */
  friend DurationUnit operator*(double real, const DurationUnit& time)
  {
    DurationUnit ret;
    ret.mAmountInMicroSeconds = real * time.mAmountInMicroSeconds;
    return ret;
  }

  /*!@brief star operator for multiplying a TimeUnit with a scalar value using *=
   *
   */
  void operator*= (double factor)
  {
    this->mAmountInMicroSeconds *= factor;
  }

  /*!@brief slash operator for dividing a TimeUnit by a scalar value
   * @return result of division
   */
  DurationUnit operator/ (double divisor) const
  {
    DurationUnit ret;
    ret.mAmountInMicroSeconds = this->mAmountInMicroSeconds / divisor;
    return ret;
  }

  /*!@brief slash operator for dividing a TimeUnit by another TimeUnit
   * @return the scalar, unit-less result of the division
   */
  template<unsigned int otherFactor, const char* otherSuffix>
  double operator/ (const DurationUnit<otherFactor, otherSuffix>& time) const
  {
    return this->mAmountInMicroSeconds / time.getRawTime();
  }

  /*!@brief slash operator for dividing a scalar value by a TimeUnit
   * @return result of division
   *
   */
  friend DurationUnit operator/(double real, const DurationUnit& time)
  {
    DurationUnit ret;
    ret.mAmountInMicroSeconds = real / time.mAmountInMicroSeconds;
    return ret;
  }

  /*!@brief slash operator for dividing a TimeUnit by a scalar value using /=
   * @return result of division
   */
  DurationUnit operator/= (double divisor)
  {
    this->mAmountInMicroSeconds /= divisor;
  }

  /*!@brief comparison of two TimeUnits
   *
   */
  template <unsigned int otherFactor, const char* otherSuffix>
  bool operator==(const DurationUnit<otherFactor, otherSuffix>& comp)
  {
    return this->mAmountInMicroSeconds == comp.mAmountInMicroSeconds;
  }

public:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
public:
  // none yet (hopefully never!)
protected:
  // none yet
private:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
public:
  // none yet (hopefully never!)
protected:
  // none yet

private:
  // none yet

}; // class cedar::unit::DurationUnit

#endif // CEDAR_UNITS_DURATION_UNIT_H

