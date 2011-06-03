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

#ifndef CEDAR_UNITS_TIMzE_UNIT_H
#define CEDAR_UNITS_TIME_UNIT_H

// LOCAL INCLUDES
#include <units/namespace.h>
#include "Time.h"

// PROJECT INCLUDES

// SYSTEM INCLUDES
#include <iostream>


/*!@brief Abstract description of the class.
 *
 * More detailed description of the class.
 *
 * @todo Read units from strings.
 */
template <unsigned int factor, const char* suffix>
class cedar::units::TimeUnit : public Time
{
  //--------------------------------------------------------------------------------------------------------------------
  // macros
  //--------------------------------------------------------------------------------------------------------------------

  //--------------------------------------------------------------------------------------------------------------------
  // friends
  //--------------------------------------------------------------------------------------------------------------------
//  template <int outFactor, const char* outSuffix>
  friend std::ostream& operator <<(std::ostream &stream, const TimeUnit& unit)
  {
    stream << unit.mAmountInMicroSeconds / static_cast<double>(factor) << " " << suffix;
    return stream;
  }

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  /*!@brief The standard constructor.
   *
   */
  TimeUnit(double amount = 1.0)
  :
  Time(amount * static_cast<double>(factor))
  {
  }

  /*!@brief Constructor that takes a base time object.
   *
   */
  TimeUnit(const cedar::units::Time& time)
  :
  Time(time)
  {
  }

  //!@brief Destructor

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
  template <unsigned int otherFactor, const char* otherSuffix>
  operator TimeUnit<otherFactor, otherSuffix>()
  {
    return this->mAmountInMicroSeconds * static_cast<double>(otherFactor);
  }

  operator Time()
  {
    return Time(this->mAmountInMicroSeconds);
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

}; // class cedar::units::TimeUnit

#endif // CEDAR_UNITS_TIME_UNIT_H

