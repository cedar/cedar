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

    File:        ParameterTemplate.h

    Maintainer:  Oliver Lomp,
                 Mathis Richter,
                 Stephan Zibner
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de,
                 mathis.richter@ini.ruhr-uni-bochum.de,
                 stephan.zibner@ini.ruhr-uni-bochum.de
    Date:        2011 07 01

    Description:

    Credits:

======================================================================================================================*/

#ifndef CEDAR_PROC_PARAMETER_TEMPLATE_H
#define CEDAR_PROC_PARAMETER_TEMPLATE_H

// LOCAL INCLUDES
#include "cedar/auxiliaries/namespace.h"
#include "cedar/auxiliaries/Parameter.h"

// PROJECT INCLUDES

// SYSTEM INCLUDES
#include <iostream>

/*!@brief Abstract description of the class.
 *
 * More detailed description of the class.
 */
template <typename T>
class cedar::aux::ParameterTemplate : public cedar::aux::Parameter
{
  //--------------------------------------------------------------------------------------------------------------------
  // macros
  //--------------------------------------------------------------------------------------------------------------------

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The constructor.
  ParameterTemplate(cedar::aux::Configurable *pOwner, const std::string& name, const T& defaultValue)
  :
  cedar::aux::Parameter(pOwner, name, true),
  mValue(defaultValue),
  mDefault(defaultValue)
  {
  }

  ParameterTemplate(cedar::aux::Configurable *pOwner, const std::string& name)
  :
  cedar::aux::Parameter(pOwner, name, false)
  {
  }

  //!@brief Destructor
  ~ParameterTemplate()
  {
  }

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  const T& getValue() const
  {
    return this->mValue;
  }

  void setValue(const T& value)
  {
    this->mValue = value;
    emit valueChanged();
  }

  void putTo(cedar::aux::ConfigurationNode& root)
  {
    root.put(this->getName(), this->mValue);
  }

  void setTo(const cedar::aux::ConfigurationNode& node)
  {
#ifdef DEBUG
    try
    {
#endif
      this->mValue = node.get_value<T>();
#ifdef DEBUG
    }
    catch (const boost::property_tree::ptree_bad_path& e)
    {
      std::cout << "Error while setting parameter to value: " << e.what() << std::endl;
    }
#endif
  }

  void makeDefault()
  {
    this->setValue(mDefault);
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
  //! The current parameter value.
  T mValue;

  //! The default value of the parameter. Ignored if mHasDefault is false.
  T mDefault;

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

private:
  // none yet

}; // class cedar::aux::ParameterTemplate

#endif // CEDAR_PROC_PARAMETER_TEMPLATE_H
