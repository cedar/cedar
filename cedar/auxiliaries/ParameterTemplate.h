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

// CEDAR INCLUDES
#include "cedar/auxiliaries/namespace.h"
#include "cedar/auxiliaries/Parameter.h"

// SYSTEM INCLUDES
#include <iostream>
#include <boost/function.hpp>

/*!@brief A generic template for parameters stored in a cedar::aux::Configurable.
 *
 *        This class stores the value of a parameter and offers access to the value via standard functions. All
 *        parameter implementations should inherit this as a base class.
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
  //!@brief The constructor with default value.
  ParameterTemplate(cedar::aux::Configurable *pOwner, const std::string& name, const T& defaultValue)
  :
  cedar::aux::Parameter(pOwner, name, true),
  mValue(defaultValue),
  mDefault(defaultValue)
  {
  }

  //!@brief The constructor.
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
  //!@brief get the current value of type T of this parameter
  const T& getValue() const
  {
    return this->mValue;
  }

  /*!@brief set the value of type T of this parameter
   *
   * @param lock Whether the method should take care of properly locking the parameter.
   */
  virtual void setValue(const T& value, bool lock = false)
  {
    if (mValidator)
    {
      mValidator(value);
    }
    if (lock)
    {
      this->lockForWrite();
    }
    T old_value = this->mValue;
    this->mValue = value;

    if (lock)
    {
      this->unlock();
    }

    if (old_value != this->mValue)
    {
      this->emitChangedSignal();
    }
  }

  //!@brief store the current value of type T in a configuration tree
  void writeToNode(cedar::aux::ConfigurationNode& root) const
  {
    root.put(this->getName(), this->mValue);
  }

  //!@brief load a value of type T from a configuration tree
  void readFromNode(const cedar::aux::ConfigurationNode& node)
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

  //!@brief set value to default
  void makeDefault()
  {
    this->setValue(mDefault);
  }

  //!@brief Set the default value.
  void setDefault(const T& value)
  {
    this->mDefault = value;
  }

  void setValidator(boost::function<void(const T&)> validator)
  {
    mValidator = validator;
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
  //! The current parameter value.
  T mValue;

  //! The default value of the parameter. Ignored if mHasDefault is false.
  T mDefault;

  /*! A validator function for setValue. Checks if a value fulfills all restrictions of this parameter.
   * (e.g., if a string contains any invalid characters). Throws a ValidationFailedException
   * if restrictions are violated.
   */
  boost::function<void(const T&)> mValidator;
}; // class cedar::aux::ParameterTemplate

#endif // CEDAR_PROC_PARAMETER_TEMPLATE_H
