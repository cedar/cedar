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

#ifndef CEDAR_PROC_NUMERIC_PARAMETER_H
#define CEDAR_PROC_NUMERIC_PARAMETER_H

// LOCAL INCLUDES
#include "auxiliaries/ParameterTemplate.h"

// PROJECT INCLUDES

// SYSTEM INCLUDES


/*!@brief Abstract description of the class.
 *
 * More detailed description of the class.
 */
template <typename T>
class cedar::aux::NumericParameter : public cedar::aux::ParameterTemplate<T>
{
  //--------------------------------------------------------------------------------------------------------------------
  // macros
  //--------------------------------------------------------------------------------------------------------------------

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The constructor.
  NumericParameter(cedar::aux::Configurable *pOwner,
                   const std::string& name,
                   const T& defaultValue,
                   const T& minimum,
                   const T& maximum)
  :
  cedar::aux::ParameterTemplate<T>(pOwner, name, defaultValue),
  mMinimum(minimum),
  mMaximum(maximum)
  {
  }

  //!@brief The constructor.
  NumericParameter(cedar::aux::Configurable *pOwner,
                   const std::string& name,
                   const T& minimum,
                   const T& maximum)
  :
  cedar::aux::ParameterTemplate<T>(pOwner, name),
  mMinimum(minimum),
  mMaximum(maximum)
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
  const T& getMinimum() const
  {
    return this->mMinimum;
  }

  void setMinimum(const T& value)
  {
    this->mMinimum = value;

    this->emitPropertyChangedSignal();
  }

  const T& getMaximum() const
  {
    return this->mMaximum;
  }

  void setMaximum(const T& value)
  {
    this->mMaximum = value;

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
public:
  // none yet (hopefully never!)
protected:
  // none yet
private:
  //! The minimum value, if applicable to the type.
  T mMinimum;

  //! The maximum value, if applicable to the type.
  T mMaximum;

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

private:
  // none yet

}; // class cedar::aux::Parameter

#endif // CEDAR_PROC_PARAMETER_H

