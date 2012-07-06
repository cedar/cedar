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

    File:        NumericParameter.h

    Maintainer:  Oliver Lomp,
                 Mathis Richter,
                 Stephan Zibner
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de,
                 mathis.richter@ini.ruhr-uni-bochum.de,
                 stephan.zibner@ini.ruhr-uni-bochum.de
    Date:        2011 07 06

    Description:

    Credits:

======================================================================================================================*/

#ifndef CEDAR_AUX_NUMERIC_PARAMETER_H
#define CEDAR_AUX_NUMERIC_PARAMETER_H

// CEDAR INCLUDES
#include "cedar/auxiliaries/ParameterTemplate.h"
#include "cedar/auxiliaries/math/Limits.h"

// SYSTEM INCLUDES
#include <boost/numeric/conversion/bounds.hpp>


/*!@brief A base class template for numeric parameters.
 */
template <typename T>
class cedar::aux::NumericParameter : public cedar::aux::ParameterTemplate<T>
{
  //--------------------------------------------------------------------------------------------------------------------
  // nested types
  //--------------------------------------------------------------------------------------------------------------------
public:
  //! Type of limits used by this parameter.
  typedef cedar::aux::math::Limits<T> LimitType;

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  /*!@brief The constructor.
   */
  NumericParameter
  (
    cedar::aux::Configurable *pOwner,
    const std::string& name,
    const T& defaultValue,
    const T& minimum,
    const T& maximum
  )
  :
  cedar::aux::ParameterTemplate<T>(pOwner, name, defaultValue),
  mLimits(minimum, maximum)
  {
  }

  /*!@brief The constructor, with default minimum and maximum.
   */
  NumericParameter
  (
    cedar::aux::Configurable *pOwner,
    const std::string& name,
    const T& defaultValue,
    const LimitType& limits = LimitType::full()
  )
  :
  cedar::aux::ParameterTemplate<T>(pOwner, name, defaultValue),
  mLimits(limits)
  {
  }

  //!@brief The constructor.
  NumericParameter
  (
    cedar::aux::Configurable *pOwner,
    const std::string& name,
    const T& minimum,
    const T& maximum
  )
  :
  cedar::aux::ParameterTemplate<T>(pOwner, name),
  mLimits(minimum, maximum)
  {
  }

  //!@brief The constructor.
  NumericParameter
  (
    cedar::aux::Configurable *pOwner,
    const std::string& name,
    const LimitType& limits = LimitType::full()
  )
  :
  cedar::aux::ParameterTemplate<T>(pOwner, name),
  mLimits(limits)
  {
  }

  //!@brief Destructor
  ~NumericParameter()
  {
  }

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief get the minimum value of this parameter
  const T& getMinimum() const
  {
    return this->mLimits.getLower();
  }

  //!@brief set the minimum value of this parameter
  void setMinimum(const T& value)
  {
    this->mLimits.setLower(value);

    this->emitPropertyChangedSignal();
  }

  //!@brief get the maximum value of this parameter
  const T& getMaximum() const
  {
    return this->mLimits.getUpper();
  }

  //!@brief set the maximum value of this parameter
  void setMaximum(const T& value)
  {
    this->mLimits.setUpper(value);

    this->emitPropertyChangedSignal();
  }

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
protected:
  // none yet
private:
  //!@brief The limits for this parameter.
  LimitType mLimits;

}; // class cedar::aux::NumericParameter

#endif // CEDAR_AUX_NUMERIC_PARAMETER_H
