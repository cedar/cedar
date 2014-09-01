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

    File:        TriggerParameter.cpp

    Maintainer:  Christian Bodenstein
    Email:       christian.bodenstein@ini.rub.de
    Date:        2014 05 10

    Description: Source file for the class cedar::proc::experiment::TriggerParameter.

    Credits:

======================================================================================================================*/

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/processing/experiment/TriggerParameter.h"
#include "cedar/auxiliaries/Configurable.h"

// SYSTEM INCLUDES

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::proc::experiment::TriggerParameter::TriggerParameter(
    cedar::aux::Configurable *pOwner, const std::string& name)
:
 cedar::aux::Parameter(pOwner,name,false)
{
}

cedar::proc::experiment::TriggerParameter::~TriggerParameter()
{
}


//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------


void cedar::proc::experiment::TriggerParameter::readFromNode(const cedar::aux::ConfigurationNode& node)
{
  this->setTrigger(node.get_child("Name").get_value<std::string>());
}

void cedar::proc::experiment::TriggerParameter::writeToNode(cedar::aux::ConfigurationNode& root) const
{
  cedar::aux::ConfigurationNode trigger_node;
  trigger_node.put("Name", mTrigger);
  root.add_child(this->getName(), trigger_node);
}

void cedar::proc::experiment::TriggerParameter::makeDefault()
{
  this->mTrigger = "";
}

void cedar::proc::experiment::TriggerParameter::copyValueFrom(
    cedar::aux::ConstParameterPtr other)
{
  if (auto other_self = boost::dynamic_pointer_cast<ConstTriggerParameter>(other))
  {
    this->mTrigger = "";//other_self->getTrigger();
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

bool cedar::proc::experiment::TriggerParameter::canCopyFrom(
    cedar::aux::ConstParameterPtr other) const
{
  return static_cast<bool>(boost::dynamic_pointer_cast<ConstTriggerParameter>(other));
}

void cedar::proc::experiment::TriggerParameter::setTrigger(const std::string& triggerName)
{
  this->mTrigger=triggerName;
}

const std::string& cedar::proc::experiment::TriggerParameter::getTrigger()
{
  return this->mTrigger;
}

