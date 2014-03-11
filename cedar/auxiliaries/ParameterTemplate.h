/*======================================================================================================================

    Copyright 2011, 2012, 2013, 2014 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany
 
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
#include "cedar/auxiliaries/Parameter.h"
#include "cedar/auxiliaries/Log.h"

// FORWARD DECLARATIONS
#include "cedar/auxiliaries/Configurable.fwd.h"
#include "cedar/auxiliaries/ParameterTemplate.fwd.h"

// SYSTEM INCLUDES
#ifndef Q_MOC_RUN
  #include <boost/function.hpp>
#endif

/*!@brief  A generic template for parameters stored in a cedar::aux::Configurable.
 *
 *         This class stores the value of a parameter and offers access to the value via standard functions. All
 *         parameter implementations should inherit this as a base class.
 *
 * @tparam T Type of the value stored as a parameter.
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
   * @param value The value to set.
   * @param lock  Whether the method should take care of properly locking the parameter.
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
    try
    {
      this->mValue = node.get_value<T>();
    }
    catch (const boost::property_tree::ptree_bad_path& e)
    {
      cedar::aux::LogSingleton::getInstance()->debugMessage
      (
        "Error while setting parameter to value: " + std::string(e.what()),
        "void cedar::aux::ParameterTemplate<T>::readFromNode(const cedar::aux::ConfigurationNode& node)"
      );
    }
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

  /*!@brief Sets the validator function that is called before setting a new value.
   *
   *        This validator function should throw an exception when it detects an error in the new value.
   *
   * @param validator Validator function.
   */
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
