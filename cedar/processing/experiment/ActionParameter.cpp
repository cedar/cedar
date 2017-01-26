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

    File:        ActionParameter.cpp

    Maintainer:  Stephan Zibner
    Email:       stephan.zibner@ini.rub.de
    Date:        2015 01 16

    Description: Source file for the class cedar::proc::experiment::ActionParameter.

    Credits:

======================================================================================================================*/

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/processing/experiment/ActionParameter.h"
#include "cedar/processing/experiment/ConnectableParameter.h"
#include "cedar/processing/experiment/Supervisor.h"
#include "cedar/processing/Connectable.h"
#include "cedar/processing/Step.h"
#include "cedar/auxiliaries/Configurable.h"
#include "cedar/auxiliaries/Log.h"
#include "cedar/auxiliaries/StringParameter.h"
#include "cedar/auxiliaries/ParameterDeclaration.h"

// SYSTEM INCLUDES

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::proc::experiment::ActionParameter::ActionParameter
(
  cedar::aux::Configurable* pOwner,
  const std::string& name,
  cedar::proc::experiment::ConnectableParameterPtr connectableParameter
)
:
cedar::proc::experiment::PropertyParameter(pOwner, name, connectableParameter),
mAction("")
{
  QObject::connect(connectableParameter.get(), SIGNAL(valueChanged()), this, SLOT(connectableChanged()));
}

cedar::proc::experiment::ActionParameter::~ActionParameter()
{
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

bool cedar::proc::experiment::ActionParameter::checkValidity(std::string& error) const
{
  if (const auto connectable = boost::dynamic_pointer_cast<cedar::proc::ConstStep>(this->getConnectable()))
  {
    if (connectable->hasAction(this->getAction()))
    {
      return true;
    }
    error = "Chosen action could not be found for element \"" + connectable->getName() + "\".";
    return false;
  }
  error = "No action or connectable set.";
  return false;
}

void cedar::proc::experiment::ActionParameter::readFromNode(const cedar::aux::ConfigurationNode& node)
{
  try
  {
    std::string action;
    auto action_node = node.find("action");
    if (action_node != node.not_found())
    {
      action = action_node->second.get_value<std::string>();
    }
    this->setAction(action);
  }
  catch (const boost::property_tree::ptree_bad_path& e)
  {
    cedar::aux::LogSingleton::getInstance()->debugMessage
    (
      "Error while setting parameter to value: " + std::string(e.what()),
      "void cedar::proc::experiment::ActionParameter::readFromNode(const cedar::aux::ConfigurationNode& node)"
    );
  }
}

void cedar::proc::experiment::ActionParameter::writeToNode(cedar::aux::ConfigurationNode& root) const
{
  cedar::aux::ConfigurationNode action_node;
  action_node.put("action", this->getAction());
  root.add_child(this->getName(), action_node);
}

void cedar::proc::experiment::ActionParameter::makeDefault()
{
  this->mAction = "";
  cedar::proc::experiment::PropertyParameter::makeDefault();
}

void cedar::proc::experiment::ActionParameter::callAction()
{
  //!@todo this should be a cast to connectable, but actions are currently only associated with steps
  if (auto connectable = boost::dynamic_pointer_cast<cedar::proc::Step>(this->getConnectable()))
  {
    connectable->callAction(this->mAction);
  }
}

std::vector<std::string> cedar::proc::experiment::ActionParameter::getListOfActions()
{
  std::vector<std::string> actions;
  //!@todo this should be a cast to connectable, but actions are currently only associated with steps
  if (auto connectable = boost::dynamic_pointer_cast<cedar::proc::Step>(this->getConnectable()))
  {
    auto action_map = connectable->getActions();
    for (auto action : action_map)
    {
      actions.push_back(action.first);
    }
  }
  return actions;
}

void cedar::proc::experiment::ActionParameter::setAction(const std::string& action)
{
  if (this->mAction != action)
  {
    this->mAction = action;
    emit valueChanged();
  }
}

std::string cedar::proc::experiment::ActionParameter::getAction() const
{
  return this->mAction;
}

void cedar::proc::experiment::ActionParameter::connectableChanged()
{
  auto actions = this->getListOfActions();
  emit actionListChanged();
}
