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

    File:        ObjectParameter.h

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2012 03 09

    Description: Templated parameter for a dynamically allocatable, configurable object.

    Credits:

======================================================================================================================*/

#ifndef CEDAR_AUX_OBJECT_PARAMETER_TEMPLATE_H
#define CEDAR_AUX_OBJECT_PARAMETER_TEMPLATE_H

// CEDAR INCLUDES
#include "cedar/auxiliaries/ObjectParameter.h"
#include "cedar/auxiliaries/Singleton.h"
#include "cedar/auxiliaries/FactoryManager.h"
#include "cedar/auxiliaries/stringFunctions.h"
#include "cedar/auxiliaries/utilities.h"

// FORWARD DECLARATIONS
#include "cedar/auxiliaries/ObjectParameterTemplate.fwd.h"

// SYSTEM INCLUDES
#include <vector>
#include <string>


/*!@brief A parameter that can hold an object whose type is determined dynamically by user input.
 *
 *        The @em BaseType has to be a type that implements cedar::aux::Configurable.
 *
 * @remarks The objects cannot themselves have a parameter with the name "type".
 */
template <typename BaseType>
class cedar::aux::ObjectParameterTemplate : public cedar::aux::ObjectParameter
{
  //--------------------------------------------------------------------------------------------------------------------
  // nested types
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief a shared pointer of a BaseType object
  typedef boost::shared_ptr<BaseType> BaseTypePtr;
  //!@brief a const shared pointer of a BaseType object
  typedef boost::shared_ptr<const BaseType> ConstBaseTypePtr;
  //!@brief a factory manager for base types
  typedef cedar::aux::FactoryManager<BaseTypePtr> FactoryManager;
  //!@brief a singleton factory manager
  typedef cedar::aux::Singleton<FactoryManager> FactoryManagerSingleton;

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief A constructor.
  ObjectParameterTemplate(cedar::aux::Configurable *pOwner, const std::string& name)
  :
  cedar::aux::ObjectParameter(pOwner, name, false)
  {
  }

  //!@brief A constructor.
  ObjectParameterTemplate
  (
    cedar::aux::Configurable *pOwner,
    const std::string& name,
    BaseTypePtr defaultObject
  )
  :
  cedar::aux::ObjectParameter(pOwner, name, true)
  {
    this->mDefault = cedar::aux::replace(cedar::aux::objectTypeToString(defaultObject), "::", ".");
    this->mDefaultObject = defaultObject;
    this->makeDefault();
  }


  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief set parameter to default
  virtual void makeDefault()
  {
    this->setType(this->mDefault);
    this->mObject->copyFrom(this->mDefaultObject);
  }

  //!@brief Set this parameter to a value read from a configuration node
  virtual void readFromNode(const cedar::aux::ConfigurationNode& node)
  {
    std::string type = node.get_child("type").get_value<std::string>();
    BaseTypePtr object = FactoryManagerSingleton::getInstance()->allocate(type);
    object->readConfiguration(node);
    this->mObject = object;
  }

  //!@brief Write the parameter's value to a configuration node.
  virtual void writeToNode(cedar::aux::ConfigurationNode& root) const
  {
    // only store if the object is set.
    if (this->mObject)
    {
      cedar::aux::ConfigurationNode node;
      const std::string& type_id = FactoryManagerSingleton::getInstance()->getTypeId(this->mObject);
      node.put("type", type_id);
      this->mObject->writeConfiguration(node);
      root.push_back(cedar::aux::ConfigurationNode::value_type(this->getName(), node));
    }
  }

  //!@brief sets the object
  void setValue(BaseTypePtr object)
  {
    this->mObject = object;

    this->emitChangedSignal();
  }

  //!@brief returns the stored object (const)
  ConstBaseTypePtr getValue() const
  {
    return this->mObject;
  }

  //!@brief returns the stored object
  BaseTypePtr getValue()
  {
    return this->mObject;
  }

  //!@brief list all types that are registered at the factory manager
  void listTypes(std::vector<std::string>& types) const
  {
    FactoryManagerSingleton::getInstance()->listTypes(types);
  }

  cedar::aux::ConstConfigurablePtr getSingleConfigurableChild() const
  {
    return this->mObject;
  }

  //!@brief sets a type at this parameter (allocating an object of this type in the process)
  void setType(const std::string& type)
  {
    BaseTypePtr object = FactoryManagerSingleton::getInstance()->allocate(type);
    this->setValue(object);
  }

  //!@brief returns the type id for the represented object
  const std::string& getTypeId() const
  {
    return FactoryManagerSingleton::getInstance()->getTypeId(mObject);
  }

  bool hasSingleConfigurableChild() const
  {
    return true;
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
  void setChangedFlag(bool changed)
  {
    // set the changed flag for the object managed by the parameter
    this->mObject->resetChangedStates(changed);

    // set the changed flag for the parameter itself.
    cedar::aux::Parameter::setChangedFlag(changed);
  }

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet
private:
  //!@brief The current object instance.
  BaseTypePtr mObject;

  //!@brief The default object type.
  std::string mDefault;
  //!@brief The default object instance.
  BaseTypePtr mDefaultObject;

}; // class cedar::aux::ObjectParameterTemplate

#endif // CEDAR_AUX_OBJECT_PARAMETER_TEMPLATE_H

