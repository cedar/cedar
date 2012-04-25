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

    File:        NumericVectorParameter.h

    Maintainer:  Oliver Lomp,
                 Mathis Richter,
                 Stephan Zibner
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de,
                 mathis.richter@ini.ruhr-uni-bochum.de,
                 stephan.zibner@ini.ruhr-uni-bochum.de
    Date:        2011 07 20

    Description:

    Credits:

======================================================================================================================*/

#ifndef CEDAR_PROC_NUMERIC_VECTOR_PARAMETER_H
#define CEDAR_PROC_NUMERIC_VECTOR_PARAMETER_H

// CEDAR INCLUDES
#include "cedar/auxiliaries/VectorParameter.h"
#include "cedar/auxiliaries/math/Limits.h"

// SYSTEM INCLUDES


/*!@brief A template for parameters that are vectors of numbers.
 */
template <typename T>
class cedar::aux::NumericVectorParameter : public cedar::aux::VectorParameter<T>
{
  //--------------------------------------------------------------------------------------------------------------------
  // nested types
  //--------------------------------------------------------------------------------------------------------------------
public:
  typedef cedar::aux::math::Limits<T> LimitType;

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The constructor.
  NumericVectorParameter(
                          cedar::aux::Configurable *pOwner,
                          const std::string& name,
                          const std::vector<T>& defaultValues,
                          const T& minimum,
                          const T& maximum
                        )
  :
  cedar::aux::VectorParameter<T>(pOwner, name, defaultValues),
  mLimits(minimum, maximum)
  {
  }

  //!@brief The constructor.
  NumericVectorParameter(
                          cedar::aux::Configurable *pOwner,
                          const std::string& name,
                          const std::vector<T>& defaultValues,
                          const LimitType& limits = LimitType::full()
                        )
  :
  cedar::aux::VectorParameter<T>(pOwner, name, defaultValues),
  mLimits(limits)
  {
  }

  //!@brief The constructor.
  NumericVectorParameter(
                          cedar::aux::Configurable *pOwner,
                          const std::string& name,
                          size_t defaultSize,
                          T defaultValue,
                          const T& minimum,
                          const T& maximum
                        )
  :
  cedar::aux::VectorParameter<T>(pOwner, name, defaultSize, defaultValue),
  mLimits(minimum, maximum)
  {
  }

  //!@brief The constructor.
  NumericVectorParameter(
                          cedar::aux::Configurable *pOwner,
                          const std::string& name,
                          size_t defaultSize,
                          T defaultValue,
                          const LimitType& limits = LimitType::full()
                        )
  :
  cedar::aux::VectorParameter<T>(pOwner, name, defaultSize, defaultValue),
  mLimits(limits)
  {
  }

  //!@brief The constructor.
  NumericVectorParameter(
                          cedar::aux::Configurable *pOwner,
                          const std::string& name,
                          const T& minimum,
                          const T& maximum
                        )
  :
  cedar::aux::VectorParameter<T>(pOwner, name),
  mLimits(minimum, maximum)
  {
  }

  //!@brief The constructor.
  NumericVectorParameter(
                          cedar::aux::Configurable *pOwner,
                          const std::string& name,
                          const LimitType& limits = LimitType::full()
                        )
  :
  cedar::aux::VectorParameter<T>(pOwner, name),
  mLimits(limits)
  {
  }

  //!@brief Destructor
  ~NumericVectorParameter()
  {
  }

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief set the minimum value of this parameter
  void setMinimum(const T& minimum)
  {
    this->mLimits.setLower(minimum);

    this->emitPropertyChangedSignal();
  }

  //!@brief get the minimum value of this parameter
  const T& getMinimum() const
  {
    return this->mLimits.getLower();
  }

  //!@brief set the maximum value of this parameter
  void setMaximum(const T& maximum)
  {
    this->mLimits.setUpper(maximum);

    this->emitPropertyChangedSignal();
  }

  //!@brief get the maximum value of this parameter
  const T& getMaximum() const
  {
    return this->mLimits.getUpper();
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
  //!@brief The range of allowed values for the vector's entries.
  LimitType mLimits;

}; // class cedar::aux::NumericVectorParameter

#endif // CEDAR_PROC_NUMERIC_VECTOR_PARAMETER_H
