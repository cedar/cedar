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

    File:        StepPropertyParameter.cpp

    Maintainer:  Christian Bodenstein
    Email:       christian.bodenstein@ini.rub.de
    Date:        2014 03 13

    Description: Source file for the class cedar::proc::experiment::StepPropertyParameter.

    Credits:

======================================================================================================================*/

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/processing/experiment/StepPropertyParameter.h"
#include "cedar/processing/experiment/Supervisor.h"
#include "cedar/processing/Connectable.h"
#include "cedar/auxiliaries/Configurable.h"
#include "cedar/auxiliaries/Log.h"
#include "cedar/auxiliaries/StringParameter.h"
#include "cedar/auxiliaries/ParameterDeclaration.h"

// SYSTEM INCLUDES

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::proc::experiment::StepPropertyParameter::StepPropertyParameter(
    cedar::aux::Configurable *pOwner, const std::string& name)
:
cedar::aux::Parameter(pOwner, name, false)
,
mType(PARAMETER_VALUE)
{
}

cedar::proc::experiment::StepPropertyParameter::~StepPropertyParameter()
{
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

bool cedar::proc::experiment::StepPropertyParameter::isParameterSelected() const
{
  if (this->mElement.lock() == nullptr || this->getProperty() == "")
  {
    return false;
  }
  else
  {
    return true;
  }
}

void cedar::proc::experiment::StepPropertyParameter::readFromNode(const cedar::aux::ConfigurationNode& node)
{
  try
  {
//!@todo This causes a lot of problems, e.g., if a value is added to the enumeration, thus changing the meaning of the integer values. Why was this even read from/written to the configuration? This is usually set by classes using this parameter...
//    PropertyType type = static_cast<PropertyType>(node.get_child("Type").get_value<int>());
//    this->setType(type);
    auto name = node.get_child("Step").get_value<std::string>();
    cedar::proc::ConnectablePtr element;
    auto group = SupervisorSingleton::getInstance()->getExperiment()->getGroup();
    if (group->nameExists(name))
    {
      element = group->getElement<cedar::proc::Connectable>(name);
    }
    this->setStep(element);
    this->setProperty(node.get_child("Property").get_value<std::string>());
    switch (mType)
    {
      case PARAMETER_VALUE:
      {
        if (mParameterCopy)
        {
          mParameterCopy->readFromNode(node.get_child("PropertyParameter"));
        }
        break;
      }
      case PARAMETER:
      {
        break;
      }
      case OUTPUT:
      {
        break;
      }
      case BUFFER:
      {
        break;
      }
    }
  }
  catch (const boost::property_tree::ptree_bad_path& e)
  {
    cedar::aux::LogSingleton::getInstance()->debugMessage
    (
      "Error while setting parameter to value: " + std::string(e.what()),
      "void cedar::proc::experiment::StepPropertyParameter::readFromNode(const cedar::aux::ConfigurationNode& node)"
    );
  }
}

void cedar::proc::experiment::StepPropertyParameter::writeToNode(cedar::aux::ConfigurationNode& root) const
{
  cedar::aux::ConfigurationNode step_node;
  if (auto element = mElement.lock())
  {
    step_node.put("Step", mElement.lock()->getFullPath());
  }
  else
  {
    step_node.put("Step", "");
  }
  step_node.put("Property", mProperty);
  //!@todo see comment in readFromNode
//  step_node.put("Type", mType);

  switch (mType)
  {
    case PARAMETER_VALUE:
    {
      if (mParameterCopy)
      {
        cedar::aux::ConfigurationNode copy_node;
        mParameterCopy->writeToNode(copy_node);
        step_node.add_child("PropertyParameter",copy_node);
      }
      break;
    }
    case PARAMETER:
    {
      break;
    }
    case OUTPUT:
    {
      break;
    }
    case BUFFER:
    {
      break;
    }
  }
  root.add_child(this->getName(), step_node);
}

void cedar::proc::experiment::StepPropertyParameter::makeDefault()
{
  this->mElement.reset();
  this->mProperty = "";
  this->mType = PARAMETER_VALUE;
}

void cedar::proc::experiment::StepPropertyParameter::copyValueFrom(cedar::aux::ConstParameterPtr other)
{
  if (auto other_self = boost::dynamic_pointer_cast<ConstStepPropertyParameter>(other))
  {
    this->setType(other_self->getType());
    this->setStep(other_self->getStep());
    this->setProperty(other_self->getProperty());
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

bool cedar::proc::experiment::StepPropertyParameter::canCopyFrom(cedar::aux::ConstParameterPtr other) const
{
  return static_cast<bool>(boost::dynamic_pointer_cast<ConstStepPropertyParameter>(other));
}

void cedar::proc::experiment::StepPropertyParameter::setProperty(const std::string& property)
{
  if (property != this->mProperty)
  {
    this->mProperty = property;
    updatePropertyCopy();
    this->emitChangedSignal();
  }
}

const std::string& cedar::proc::experiment::StepPropertyParameter::getProperty() const
{
  return this->mProperty;
}

void cedar::proc::experiment::StepPropertyParameter::setStep(cedar::proc::ConnectableWeakPtr step)
{
  auto locked_step = step.lock();
  if (locked_step != this->mElement.lock())
  {
    this->mElement = locked_step;
    auto name_parameter = locked_step->getParameter("name");
    this->emitChangedSignal();
  }
}

void cedar::proc::experiment::StepPropertyParameter::setStep(const std::string& step)
{
  cedar::proc::ConnectablePtr element;
  auto group = SupervisorSingleton::getInstance()->getExperiment()->getGroup();
  if (group->nameExists(step))
  {
    element = group->getElement<cedar::proc::Connectable>(step);
    this->setStep(element);
  }
}

cedar::proc::ConnectablePtr cedar::proc::experiment::StepPropertyParameter::getStep() const
{
  return this->mElement.lock();

}

void cedar::proc::experiment::StepPropertyParameter::setType(PropertyType type)
{
  PropertyType old_value = this->mType;
  this->mType=type;
  if (old_value != this->mType)
  {
    this->emitChangedSignal();
  }
}

cedar::proc::experiment::StepPropertyParameter::PropertyType cedar::proc::experiment::StepPropertyParameter::getType() const
{
  return this->mType;
}

cedar::aux::ConstDataPtr cedar::proc::experiment::StepPropertyParameter::getData() const
{
  if (auto step = mElement.lock())
  {
    if (mType == OUTPUT)
    {
        if (cedar::aux::ConstDataPtr data = step->getData(cedar::proc::DataRole::OUTPUT, mProperty))
        {
            return data;
        }
    }

    if (mType == BUFFER)
    {
      if (cedar::aux::ConstDataPtr data = step->getData(cedar::proc::DataRole::BUFFER, mProperty))
      {
          return data;
      }

    }
  }
  return cedar::aux::ConstDataPtr();
}

cedar::aux::ParameterPtr cedar::proc::experiment::StepPropertyParameter::getParameter() const
{
  if (mElement.lock() == nullptr || mProperty == "" || (this->mType != PARAMETER_VALUE && this->mType != PARAMETER))
  {
    return cedar::aux::ParameterPtr();
  }
  try
  {
    if (auto step = mElement.lock())
    {
      return step->getParameter(mProperty);
    }
  }
  catch (cedar::aux::NotFoundException& exc) // element was probably removed, reset
  {
    return cedar::aux::ParameterPtr();
  }
  catch (cedar::aux::UnknownNameException& exc) // property not found
  {
    return cedar::aux::ParameterPtr();
  }
  return cedar::aux::ParameterPtr();
  //!@todo catch parameter not found exception
}

cedar::aux::ParameterPtr cedar::proc::experiment::StepPropertyParameter::getParameterCopy() const
{
  return this->mParameterCopy;
}

void cedar::proc::experiment::StepPropertyParameter::allowType(const std::string& type)
{
  this->allowedTypes.push_back(type);
}

bool cedar::proc::experiment::StepPropertyParameter::isAllowType(const std::string& type)
{
  for (std::string allowed_type : allowedTypes)
  {
    if (allowed_type == type)
    {
      return true;
    }
  }
  return false;
}

void cedar::proc::experiment::StepPropertyParameter::disallowType(const std::string& type)
{
  int i = 0;
  for (std::string allowed_type : allowedTypes)
  {
    if (allowed_type == type)
    {
      this->allowedTypes.erase(this->allowedTypes.begin()+i);
      break;
    }
    i++;
  }
}

const std::vector<std::string>& cedar::proc::experiment::StepPropertyParameter::getAllowedTypes()
{
  return this->allowedTypes;
}

void cedar::proc::experiment::StepPropertyParameter::updatePropertyCopy()
{
  if (mElement.lock() == nullptr || mProperty == "")
  {
    return;
  }

  switch (mType)
  {
    case PARAMETER_VALUE:
    {
      try
      {
        if (auto step = mElement.lock())
        {
          cedar::aux::ParameterPtr parameter = step->getParameter(mProperty);
          std::string type = cedar::aux::ParameterDeclarationManagerSingleton::getInstance()->getTypeId(parameter);
          mParameterCopy = cedar::aux::ParameterDeclarationManagerSingleton::getInstance()->allocate(type);
          mParameterCopy->copyValueFrom(parameter);
        }
      }
      catch (cedar::aux::NotFoundException& exc) // element was not found, reset
      {
        mElement.reset();
        mProperty = "";
      }
      catch (cedar::aux::UnknownNameException& exc) // property not found
      {
        mProperty = "";
      }
      break;
    }
    case PARAMETER:
    {
      break;
    }
    case OUTPUT:
    {
      break;
    }
    case BUFFER:
    {
      break;
    }
  }
}

std::vector<std::string> cedar::proc::experiment::StepPropertyParameter::getListOfData(cedar::proc::DataRole::Id role) const
{
  std::vector<std::string> list;
  if (auto step = mElement.lock())
  {
    if (step->hasRole(role))
    {
      for (auto data : step->getDataSlots(role))
      {
        list.push_back(data.first);
      }
    }
  }
  return list;
}

std::vector<std::string> cedar::proc::experiment::StepPropertyParameter::getListOfParameters()
{
  std::vector<std::string> list;
  cedar::aux::ConfigurablePtr configurable = this->mElement.lock();
  if (!configurable)
  {
    return list;
  }

  std::vector<std::string> step_parameter_paths = configurable->listAllParameters();
  for (const auto& parameter_path : step_parameter_paths)
  {
    try
    {
      // Check if parameter is registered in the DeclarationManager
      auto parameter = configurable->getParameter(parameter_path);
      // never allow "name" to be in the list of parameters, same goes for hidden and constant parameters
      if (parameter->getName() == "name" || parameter->isHidden() || parameter->isConstant())
      {
        continue;
      }
      std::string parameter_type = cedar::aux::ParameterDeclarationManagerSingleton::getInstance()->getTypeId(parameter);
      if (this->getAllowedTypes().size() > 0)
      {
        for (auto type : this->getAllowedTypes())
        {
          if (type == parameter_type)
          {
            list.push_back(parameter_path);
            break;
          }
        }
      }
      else
      {
        list.push_back(parameter_path);
      }
    }
    catch (cedar::aux::UnknownTypeException e)
    {
       //!@todo handle this!
    }
  }
  return list;
}
