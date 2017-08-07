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

// FORWARD DECLARATIONS
#include "cedar/auxiliaries/NumericParameter.fwd.h"

// SYSTEM INCLUDES
#ifndef Q_MOC_RUN
  #include <boost/numeric/conversion/bounds.hpp>
#endif


/*!@brief A base class template for numeric parameters.
 */
template <typename T>
class cedar::aux::NumericParameter : public cedar::aux::ParameterTemplate<T>
{
  //--------------------------------------------------------------------------------------------------------------------
  // nested types
  //--------------------------------------------------------------------------------------------------------------------
private:
  //! The super class of this class.
  typedef cedar::aux::ParameterTemplate<T> Super;

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
    const T& maximum,
    const double& singlestep = 0.1
  )
  :
  cedar::aux::ParameterTemplate<T>(pOwner, name, defaultValue),
  mLimits(minimum, maximum),
  mSingleStep(singlestep)
  {
  }

  /*!@brief The constructor, with default minimum and maximum.
   */
  NumericParameter
  (
    cedar::aux::Configurable *pOwner = nullptr,
    const std::string& name = "",
    const T& defaultValue = static_cast<T>(0),
    const LimitType& limits = LimitType::full(),
    const double& singlestep = 0.1
  )
  :
  cedar::aux::ParameterTemplate<T>(pOwner, name, defaultValue),
  mLimits(limits),
  mSingleStep(singlestep)
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

    // updates the value if the limits restrict it
    this->setValue(this->getValue());
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

    // updates the value if the limits restrict it
    this->setValue(this->getValue());
  }

  /*!@brief Set the value of type T of this parameter.
   *
   * @param value The value to set.
   * @param lock  Whether the method should take care of properly locking the parameter.
   */
  void setValue(const T& value, bool lock = false)
  {
    this->Super::setValue(this->mLimits.limit(value), lock);
  }

  void setSingleStep(const double& value)
  {
    mSingleStep= value;
  }

  double getSingleStep()
  {
    return mSingleStep;
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
  double mSingleStep;

}; // class cedar::aux::NumericParameter

#endif // CEDAR_AUX_NUMERIC_PARAMETER_H
