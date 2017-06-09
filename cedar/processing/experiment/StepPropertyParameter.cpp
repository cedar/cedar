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

cedar::proc::experiment::StepPropertyParameter::StepPropertyParameter
(
  cedar::aux::Configurable* pOwner,
  const std::string& name
)
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

bool cedar::proc::experiment::StepPropertyParameter::checkValidity
     (
       std::string& error
     )
     const
{
  if (this->mElement.lock() == nullptr)
  {
    if (this->mElementPath.empty())
    {
      error = "No element path selected.";
    }
    else
    {
      error = "Element was not found.";
    }
    return false;
  }
  if (this->getParameterPath() == "")
  {
    error = "No parameter path was selected.";
    return false;
  }

  return true;
}

bool cedar::proc::experiment::StepPropertyParameter::isParameterSelected() const
{
  if (this->mElement.lock() == nullptr || this->getParameterPath() == "")
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
    std::string element_path;
    auto element_path_iter = node.find("element path");
    if (element_path_iter != node.not_found())
    {
      element_path = element_path_iter->second.get_value<std::string>();
    }
    else
    {
      // old naming
      auto step_iter = node.find("Step");
      if (step_iter != node.not_found())
      {
        element_path = step_iter->second.get_value<std::string>();
      }
    }
    this->setElementPath(element_path);

    std::string parameter_path;
    auto parameter_path_iter = node.find("parameter path");
    if (parameter_path_iter != node.not_found())
    {
      parameter_path = parameter_path_iter->second.get_value<std::string>();
    }
    else
    {
      // old naming
      auto property_iter = node.find("Property");
      if (property_iter != node.not_found())
      {
        parameter_path = property_iter->second.get_value<std::string>();
      }
    }
    this->setParameterPath(parameter_path);

    switch (mType)
    {
      case PARAMETER_VALUE:
      {
        if (mParameterCopy)
        {
          auto iter = node.find("parameter value");
          if (iter == node.not_found())
          {
            iter = node.find("PropertyParameter");
          }
          CEDAR_ASSERT(iter != node.not_found());
          mParameterCopy->readFromNode(iter->second);
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
  step_node.put("element path", this->getElementPath());
  step_node.put("parameter path", this->getParameterPath());

  switch (mType)
  {
    case PARAMETER_VALUE:
    {
      if (mParameterCopy)
      {
        mParameterCopy->writeToNode(step_node);
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
  this->setParameterPath("");
}

void cedar::proc::experiment::StepPropertyParameter::copyValueFrom(cedar::aux::ConstParameterPtr other)
{
  if (auto other_self = boost::dynamic_pointer_cast<ConstStepPropertyParameter>(other))
  {
    this->setType(other_self->getType());
    this->setStep(other_self->getStep());
    this->setParameterPath(other_self->getParameterPath());
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

void cedar::proc::experiment::StepPropertyParameter::setParameterPath(const std::string& parameterPath)
{
  if (parameterPath != this->mParameterPath)
  {
    this->mParameterPath = parameterPath;
    this->updateParameterCopy();
    this->emitChangedSignal();
  }
}

void cedar::proc::experiment::StepPropertyParameter::setStep(cedar::proc::ConnectablePtr connectable)
{
  this->mElement = connectable;
  auto name_parameter = connectable->getParameter("name");
  this->emitChangedSignal();
}

void cedar::proc::experiment::StepPropertyParameter::setElementPath(const std::string& elementPath)
{
  this->mElementPath = elementPath;
  auto group = SupervisorSingleton::getInstance()->getExperiment()->getGroup();
  if (group && group->nameExists(elementPath))
  {
    auto element = group->getElement<cedar::proc::Connectable>(elementPath);
    this->setStep(element);
  }
  else
  {
    // we have to reset the pointer if the step does not exist
    this->mElement.reset();
  }
}

std::string cedar::proc::experiment::StepPropertyParameter::getElementPath() const
{
  // if a step is set, use its path
  if (auto element = this->mElement.lock())
  {
    return element->getFullPath();
  }
  // if not, use the stored path
  else
  {
    return this->mElementPath;
  }
}

const std::string& cedar::proc::experiment::StepPropertyParameter::getParameterPath() const
{
  return this->mParameterPath;
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
        if (cedar::aux::ConstDataPtr data = step->getData(cedar::proc::DataRole::OUTPUT, this->getParameterPath()))
        {
            return data;
        }
    }

    if (mType == BUFFER)
    {
      if (cedar::aux::ConstDataPtr data = step->getData(cedar::proc::DataRole::BUFFER, this->getParameterPath()))
      {
          return data;
      }

    }
  }
  return cedar::aux::ConstDataPtr();
}

cedar::aux::ParameterPtr cedar::proc::experiment::StepPropertyParameter::getParameter() const
{
  if (!this->isParameterSelected() || (this->mType != PARAMETER_VALUE && this->mType != PARAMETER))
  {
    return cedar::aux::ParameterPtr();
  }
  try
  {
    if (auto step = mElement.lock())
    {
      return step->getParameter(this->getParameterPath());
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

void cedar::proc::experiment::StepPropertyParameter::updateParameterCopy()
{
  if (!this->isParameterSelected())
  {
    mParameterCopy.reset();
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
          cedar::aux::ParameterPtr parameter = step->getParameter(this->mParameterPath);
          std::string type = cedar::aux::ParameterDeclarationManagerSingleton::getInstance()->getTypeId(parameter);
          mParameterCopy = cedar::aux::ParameterDeclarationManagerSingleton::getInstance()->allocate(type);
          mParameterCopy->copyValueFrom(parameter);
          mParameterCopy->setName("PropertyParameter");
        }
      }
      catch (cedar::aux::NotFoundException& exc) // element was not found, reset
      {
        mElement.reset();
        mParameterCopy.reset();
        this->setParameterPath("");
      }
      catch (cedar::aux::UnknownNameException& exc) // parameter not found
      {
        mParameterCopy.reset();
        this->setParameterPath("");
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
    if (step->hasSlotForRole(role))
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
    }
  }
  return list;
}
