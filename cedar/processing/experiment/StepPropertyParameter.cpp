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
#include "cedar/auxiliaries/Configurable.h"
#include "cedar/processing/experiment/ExperimentSuperviser.h"
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
mType(PARAMETER)
{
}

cedar::proc::experiment::StepPropertyParameter::~StepPropertyParameter()
{
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::proc::experiment::StepPropertyParameter::readFromNode(const cedar::aux::ConfigurationNode& node)
{
  try
  {
    PropertyType type =static_cast<PropertyType>(node.get_child("Type").get_value<int>());
    this->setType(type);
    this->setStep(node.get_child("Step").get_value<std::string>());
    this->setProperty(node.get_child("Property").get_value<std::string>());
    switch (mType)
    {
      case PARAMETER:
      {
        mParameterCopy->readFromNode(node.get_child("PropertyParameter"));
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
  step_node.put("Step", mStep);
  step_node.put("Property", mProperty);
  step_node.put("Type", mType);

  switch (mType)
  {
    case PARAMETER:
    {
      cedar::aux::ConfigurationNode copy_node;
      mParameterCopy->writeToNode(copy_node);
      step_node.add_child("PropertyParameter",copy_node);
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
  this->mStep ="";
  this->mProperty ="";
  this->mType=PARAMETER;
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
  std::string old_value = this->mProperty;
  this->mProperty=property;
  if (old_value != this->mProperty)
  {
    updatePropertyCopy();
    this->emitChangedSignal();
  }
}

const std::string& cedar::proc::experiment::StepPropertyParameter::getProperty() const
{
  return this->mProperty;
}

void cedar::proc::experiment::StepPropertyParameter::setStep(const std::string& step)
{
  std::string old_value = this->mStep;
  this->mStep=step;
  if (old_value != this->mStep)
  {
    this->emitChangedSignal();
  }
}

const std::string& cedar::proc::experiment::StepPropertyParameter::getStep() const
{
  return this->mStep;

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
  Experiment* experiment = ExperimentSuperviserSingleton::getInstance()->getExperiment();
  if (mType==OUTPUT)
  {
      if (cedar::aux::ConstDataPtr data = experiment->
          getStepData(mStep,mProperty,cedar::proc::DataRole::OUTPUT))
      {
          return data;
      }
  }

  if (mType==BUFFER)
  {
    if (cedar::aux::ConstDataPtr data = experiment->
        getStepData(mStep,mProperty,cedar::proc::DataRole::BUFFER))
    {
        return data;
    }

  }
  return cedar::aux::ConstDataPtr();
}

cedar::aux::ParameterPtr cedar::proc::experiment::StepPropertyParameter::getParameter() const
{
  if (mStep == "" || mProperty == "" || mType!=PARAMETER)
  {
    return cedar::aux::ParameterPtr();
  }
  return ExperimentSuperviserSingleton::getInstance()->
         getExperiment()->getStepParameter(mStep,mProperty);
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
    if (allowed_type==type)
    {
      return true;
    }
  }
  return false;
}

void cedar::proc::experiment::StepPropertyParameter::disallowType(const std::string& type)
{
  int i=0;
  for (std::string allowed_type : allowedTypes)
  {
    if (allowed_type==type)
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

  if (mStep == "" || mProperty == "")
  {
    return;
  }
  Experiment* experiment = ExperimentSuperviserSingleton::getInstance()->getExperiment();
  switch (mType)
  {
    case PARAMETER:
    {
      cedar::aux::ParameterPtr parameter = experiment->getStepParameter(mStep,mProperty);
      std::string type = cedar::aux::ParameterDeclarationManagerSingleton::getInstance()->getTypeId(parameter);
      mParameterCopy= cedar::aux::ParameterDeclarationManagerSingleton::getInstance()->allocate(type);
      mParameterCopy->copyValueFrom(parameter);
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
