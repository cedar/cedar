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
#include "cedar/auxiliaries/utilities.h"

// SYSTEM INCLUDES
#ifndef Q_MOC_RUN
  #include <boost/function.hpp>
#endif

namespace cedar
{
  namespace aux
  {
    /*!@brief A namespace for some implementation details of cedar::aux::ParameterTemplate.
     */
    namespace ParameterTemplateDetails
    {
      /*!@brief Standard policy for reading and writing parameters.
       */
      template <typename T>
      class ValuePolicy
      {
      public:
        //! Specifies the type to be read from the configuration tree.
        typedef T ReadType;

        //! Stores the given value.
        ValuePolicy(const T& value = T())
        :
        mValue(value)
        {
        }

      protected:
        //! Returns the stored value.
        const T& getValuePrivate() const
        {
          return this->mValue;
        }

        //! Sets the value, using the read type.
        void setValuePrivate(const ReadType& value)
        {
          this->mValue = value;
        }

      protected:
        //! The stored value.
        T mValue;
      };
    }
  }
}

/*!@brief  A generic template for parameters stored in a cedar::aux::Configurable.
 *
 *         This class stores the value of a parameter and offers access to the value via standard functions. All
 *         parameter implementations should inherit this as a base class.
 *
 * @tparam T Type of the value stored as a parameter.
 * @tparam ValuePolicy Specifies how the value in this parameter is stored.
 */
template
<
  typename T,
  typename ValuePolicy = cedar::aux::ParameterTemplateDetails::ValuePolicy<T>
>
class cedar::aux::ParameterTemplate : public cedar::aux::Parameter, public ValuePolicy
{
  //--------------------------------------------------------------------------------------------------------------------
  // nested types
  //--------------------------------------------------------------------------------------------------------------------
private:
  typedef cedar::aux::ParameterTemplate<T, ValuePolicy> SelfType;
  CEDAR_GENERATE_POINTER_TYPES_INTRUSIVE(SelfType);

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The constructor with default value.
  ParameterTemplate(cedar::aux::Configurable *pOwner, const std::string& name, const T& defaultValue)
  :
  cedar::aux::Parameter(pOwner, name, true),
  ValuePolicy(defaultValue),
  mDefault(defaultValue)
  {
  }

  //!@brief The constructor.
  ParameterTemplate(cedar::aux::Configurable *pOwner = nullptr, const std::string& name = "")
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
    cedar::aux::Parameter::ReadLockerPtr read_locker;
    if (lock)
    {
      read_locker = cedar::aux::Parameter::ReadLockerPtr(new cedar::aux::Parameter::ReadLocker(this));
    }
    T old_value = this->mValue;
    if (lock)
    {
      read_locker->unlock();
    }

    if (value == old_value)
    {
      // nothing to do, value hasn't changed
      return;
    }

    if (mValidator)
    {
      mValidator(value);
    }

    cedar::aux::Parameter::WriteLockerPtr write_locker;
    if (lock)
    {
      write_locker = cedar::aux::Parameter::WriteLockerPtr(new cedar::aux::Parameter::WriteLocker(this));
    }
    this->mValue = value;

    if (lock)
    {
      write_locker->unlock();
    }

    this->emitChangedSignal();
  }

  //!@brief store the current value of type T in a configuration tree
  void writeToNode(cedar::aux::ConfigurationNode& node) const
  {
    node.put(this->getName(), this->getValuePrivate());
  }

  //!@brief load a value of type T from a configuration tree
  void readFromNode(const cedar::aux::ConfigurationNode& node)
  {
    try
    {
      this->setValuePrivate(node.get_value<typename ValuePolicy::ReadType>());
    }
    catch (const boost::property_tree::ptree_bad_path& e)
    {
      cedar::aux::LogSingleton::getInstance()->debugMessage
      (
        "Error while reading parameter '" + this->getName() + "': " + std::string(e.what()) + ". Inserting default value instead.",
        "void cedar::aux::ParameterTemplate<T>::readFromNode(const cedar::aux::ConfigurationNode& node)"
      );
      this->makeDefault();
    }
    catch (const boost::property_tree::ptree_bad_data& e)
    {
      cedar::aux::LogSingleton::getInstance()->debugMessage
      (
        "Error while reading parameter '" + this->getName() + "': " + std::string(e.what()) + ". Inserting default value instead.",
        "void cedar::aux::ParameterTemplate<T>::readFromNode(const cedar::aux::ConfigurationNode& node)"
      );
      this->makeDefault();
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

  //! Checks if the parameter can be copied from the other parameter.
  bool canCopyFrom(cedar::aux::ConstParameterPtr other) const
  {
    return static_cast<bool>(boost::dynamic_pointer_cast<ConstSelfType>(other));
  }

  //! Copies the parameter value from another parameter.
  void copyValueFrom(cedar::aux::ConstParameterPtr other)
  {
    if (auto other_self = boost::dynamic_pointer_cast<ConstSelfType>(other))
    {
      this->setValue(other_self->getValue());
    }
    else
    {
      CEDAR_THROW
      (
        cedar::aux::UnhandledTypeException,
        "Cannot copy parameter value: types don't match. Type of this: " + cedar::aux::objectTypeToString(this)
        + ", type of other: " + cedar::aux::objectTypeToString(other)
      );
    }
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
  //! The default value of the parameter. Ignored if mHasDefault is false.
  T mDefault;

  /*! A validator function for setValue. Checks if a value fulfills all restrictions of this parameter.
   * (e.g., if a string contains any invalid characters). Throws a ValidationFailedException
   * if restrictions are violated.
   */
  boost::function<void(const T&)> mValidator;
}; // class cedar::aux::ParameterTemplate

#endif // CEDAR_PROC_PARAMETER_TEMPLATE_H
