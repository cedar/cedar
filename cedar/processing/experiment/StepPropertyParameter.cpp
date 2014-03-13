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
#include "cedar/processing/experiment/ExperimentController.h"
#include "cedar/auxiliaries/Log.h"

// SYSTEM INCLUDES

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::proc::experiment::StepPropertyParameter::StepPropertyParameter(
    cedar::aux::Configurable *pOwner, const std::string& name)
:
cedar::aux::Parameter(pOwner, name, false)
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
    this->setStep(node.get_child("Step").get_value<std::string>());
    this->setProperty(node.get_child("Step").get_value<std::string>());
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
  root.add_child(this->getName(), step_node);
}

void cedar::proc::experiment::StepPropertyParameter::makeDefault()
{
  this->mStep ="";
  this->mProperty ="";
}

void cedar::proc::experiment::StepPropertyParameter::copyValueFrom(cedar::aux::ConstParameterPtr other)
{
  if (auto other_self = boost::dynamic_pointer_cast<ConstStepPropertyParameter>(other))
  {
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

