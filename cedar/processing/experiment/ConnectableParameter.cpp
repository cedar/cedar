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

    File:        ConnectableParameter.cpp

    Maintainer:  Stephan Zibner
    Email:       stephan.zibner@ini.rub.de
    Date:        2015 01 16

    Description: Source file for the class cedar::proc::experiment::ConnectableParameter.

    Credits:

======================================================================================================================*/

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/processing/experiment/ConnectableParameter.h"
#include "cedar/processing/experiment/Supervisor.h"
#include "cedar/processing/Connectable.h"
#include "cedar/auxiliaries/Configurable.h"
#include "cedar/auxiliaries/Log.h"
#include "cedar/auxiliaries/ParameterDeclaration.h"

// SYSTEM INCLUDES

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::proc::experiment::ConnectableParameter::ConnectableParameter
(
  cedar::aux::Configurable* pOwner,
  const std::string& name
)
:
cedar::aux::Parameter(pOwner, name, false)
{
}

cedar::proc::experiment::ConnectableParameter::~ConnectableParameter()
{
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

bool cedar::proc::experiment::ConnectableParameter::checkValidity
     (
       std::string& error
     )
     const
{
  if (this->mConnectable.lock() == nullptr)
  {
    if (this->mConnectablePath.empty())
    {
      error = "No connectable path selected.";
    }
    else
    {
      error = "Connectable was not found.";
    }
    return false;
  }

  return true;
}

void cedar::proc::experiment::ConnectableParameter::readFromNode(const cedar::aux::ConfigurationNode& node)
{
  try
  {
    std::string connectable_path;
    auto element_path_iter = node.find("connectable path");
    if (element_path_iter != node.not_found())
    {
      connectable_path = element_path_iter->second.get_value<std::string>();
    }
    this->setConnectablePath(connectable_path);
  }
  catch (const boost::property_tree::ptree_bad_path& e)
  {
    cedar::aux::LogSingleton::getInstance()->debugMessage
    (
      "Error while setting parameter to value: " + std::string(e.what()),
      "void cedar::proc::experiment::ConnectableParameter::readFromNode(const cedar::aux::ConfigurationNode& node)"
    );
  }
}

void cedar::proc::experiment::ConnectableParameter::writeToNode(cedar::aux::ConfigurationNode& root) const
{
  cedar::aux::ConfigurationNode connectable_node;
  connectable_node.put("connectable path", this->getConnectablePath());
  root.add_child(this->getName(), connectable_node);
}

void cedar::proc::experiment::ConnectableParameter::setConnectable(cedar::proc::ConnectablePtr connectable)
{
  this->mConnectable = connectable;
  this->emitChangedSignal();
}

std::string cedar::proc::experiment::ConnectableParameter::getConnectablePath() const
{
  // if a connectable is set, use its path
  if (auto element = this->mConnectable.lock())
  {
    return element->getFullPath();
  }
  // if not, use the stored path
  else
  {
    return this->mConnectablePath;
  }
}

cedar::proc::ConstConnectablePtr cedar::proc::experiment::ConnectableParameter::getConnectable() const
{
  return this->mConnectable.lock();
}

cedar::proc::ConnectablePtr cedar::proc::experiment::ConnectableParameter::getConnectable()
{
  return this->mConnectable.lock();
}

void cedar::proc::experiment::ConnectableParameter::setConnectablePath(const std::string& elementPath)
{
  this->mConnectablePath = elementPath;
  auto group = SupervisorSingleton::getInstance()->getExperiment()->getGroup();
  if (group && group->nameExists(elementPath))
  {
    auto element = group->getElement<cedar::proc::Connectable>(elementPath);
    this->setConnectable(element);
  }
  else
  {
    // we have to reset the pointer if the connectable does not exist
    this->mConnectable.reset();
  }
}

void cedar::proc::experiment::ConnectableParameter::makeDefault()
{
  this->mConnectablePath = "";
  this->mConnectable.reset();
}
