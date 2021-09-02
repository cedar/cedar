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

    File:        NeuralFieldBoostParameter.cpp

    Maintainer:  Daniel Sabinasz
    Email:       daniel.sabinasz@ini.rub.de
    Date:        2021 08 05

    Description: Source file for the class cedar::proc::experiment::NeuralFieldBoostParameter.

    Credits:

======================================================================================================================*/

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/processing/experiment/NeuralFieldBoostParameter.h"
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

cedar::proc::experiment::NeuralFieldBoostParameter::NeuralFieldBoostParameter
(
  cedar::aux::Configurable* pOwner,
  const std::string& name
)
:
cedar::aux::Parameter(pOwner, name, false)
{
}

cedar::proc::experiment::NeuralFieldBoostParameter::~NeuralFieldBoostParameter()
{
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

bool cedar::proc::experiment::NeuralFieldBoostParameter::checkValidity
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

  return true;
}

bool cedar::proc::experiment::NeuralFieldBoostParameter::isParameterSelected() const
{
    if (this->mElement.lock() == nullptr)
    {
        return false;
    }
    else
    {
        return true;
    }
}

void cedar::proc::experiment::NeuralFieldBoostParameter::readFromNode(const cedar::aux::ConfigurationNode& node)
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
  }
  catch (const boost::property_tree::ptree_bad_path& e)
  {
    cedar::aux::LogSingleton::getInstance()->debugMessage
    (
      "Error while setting step: " + std::string(e.what()),
      "void cedar::proc::experiment::NeuralFieldBoostParameter::readFromNode(const cedar::aux::ConfigurationNode& node)"
    );
  }
}

void cedar::proc::experiment::NeuralFieldBoostParameter::writeToNode(cedar::aux::ConfigurationNode& root) const
{
  cedar::aux::ConfigurationNode step_node;
  step_node.put("element path", this->getElementPath());

  root.add_child(this->getName(), step_node);
}

void cedar::proc::experiment::NeuralFieldBoostParameter::makeDefault()
{
  this->mElement.reset();
}

void cedar::proc::experiment::NeuralFieldBoostParameter::setStep(cedar::proc::ConnectablePtr connectable)
{
  this->mElement = connectable;
  auto name_parameter = connectable->getParameter("name");
  this->emitChangedSignal();
}

void cedar::proc::experiment::NeuralFieldBoostParameter::setElementPath(const std::string& elementPath)
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

std::string cedar::proc::experiment::NeuralFieldBoostParameter::getElementPath() const
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

cedar::proc::ConnectablePtr cedar::proc::experiment::NeuralFieldBoostParameter::getStep() const
{
  return this->mElement.lock();
}

double cedar::proc::experiment::NeuralFieldBoostParameter::getStrength() const
{
    return this->mStrength;
}

void cedar::proc::experiment::NeuralFieldBoostParameter::setStrength(double strength)
{
    this->mStrength = strength;
}