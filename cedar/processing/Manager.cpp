/*======================================================================================================================

    Copyright 2011 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany
 
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

    File:        StepManager.cpp

    Maintainer:  Oliver Lomp,
                 Mathis Richter,
                 Stephan Zibner
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de,
                 mathis.richter@ini.ruhr-uni-bochum.de,
                 stephan.zibner@ini.ruhr-uni-bochum.de
    Date:        2011 06 24

    Description:

    Credits:

======================================================================================================================*/

// LOCAL INCLUDES
#include "processing/Manager.h"
#include "processing/Step.h"
#include "processing/exceptions.h"

// PROJECT INCLUDES

// SYSTEM INCLUDES
#include <boost/property_tree/json_parser.hpp>

cedar::proc::Manager cedar::proc::Manager::mpManager;

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::proc::Manager::Manager()
{
}

cedar::proc::Manager::~Manager()
{
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

cedar::proc::StepPtr cedar::proc::Manager::allocateClass(const std::string& classId) const
{
  std::map<std::string, StepDeclarationPtr>::const_iterator iter;
  iter = mDeclarations.find(classId);

  if (iter != mDeclarations.end())
  {
    return iter->second->getStepFactory()->allocate();
  }
  else
  {
    return cedar::proc::StepPtr();
  }
}

void cedar::proc::Manager::registerStep(cedar::proc::StepPtr step)
{
  if (this->mSteps.find(step->getName()) != this->mSteps.end())
  {
    CEDAR_THROW(cedar::proc::InvalidNameException, "Duplicate step entry: " + step->getName());
  }
  mSteps[step->getName()] = step;
}

void cedar::proc::Manager::renameStep(const std::string& oldName,
                                          const std::string& newName)
{
  cedar::proc::StepPtr step = this->getStep(oldName);

  mSteps.erase(mSteps.find(oldName));

  mSteps[newName] = step;
  step->setName(newName);
}

cedar::proc::StepPtr cedar::proc::Manager::getStep(const std::string& name)
{
  std::map<std::string, StepPtr>::iterator iter = this->mSteps.find(name);
  if (iter != this->mSteps.end())
  {
    return iter->second;
  }
  else
  {
    CEDAR_THROW(cedar::proc::InvalidNameException, "Unknown step: " + name);
    return cedar::proc::StepPtr();
  }
}


cedar::proc::Manager& cedar::proc::Manager::getInstance()
{
  return cedar::proc::Manager::mpManager;
}

void cedar::proc::Manager::declareStepClass(StepDeclarationPtr pStepDeclaration)
{
  const std::string& class_id = pStepDeclaration->getClassId();
  if (this->mDeclarations.find(class_id) != this->mDeclarations.end())
  {
    CEDAR_THROW(cedar::proc::InvalidNameException, "Duplicate class declaration: " + class_id);
  }
  this->mDeclarations[class_id] = pStepDeclaration;
}


void cedar::proc::Manager::readFile(const std::string& filename)
{
  ConfigurationNode cfg;
  boost::property_tree::read_json(filename, cfg);
  this->readAll(cfg);
}

void cedar::proc::Manager::readAll(const ConfigurationNode& root)
{
  try
  {
    const ConfigurationNode& steps = root.get_child("steps");
    this->readSteps(steps);
  }
  catch (const boost::property_tree::ptree_bad_path&)
  {
    // no steps declared -- this is ok.
  }

  try
  {
    const ConfigurationNode& connections = root.get_child("connections");
    this->readConnections(connections);
  }
  catch (const boost::property_tree::ptree_bad_path&)
  {
    // no connections declared -- this is ok.
  }
}

void cedar::proc::Manager::readStep(const std::string& classId, const ConfigurationNode& root)
{
  cedar::proc::StepPtr step = this->allocateClass(classId);
  step->readConfiguration(root);
  this->registerStep(step);
}

void cedar::proc::Manager::readSteps(const ConfigurationNode& root)
{
  for (ConfigurationNode::const_iterator iter = root.begin();
      iter != root.end();
      ++iter)
  {
    this->readStep(iter->first, iter->second);
  }
}

void cedar::proc::Manager::readConnection(const ConfigurationNode& root)
{
  std::string source = root.get<std::string>("source");
  std::string target = root.get<std::string>("target");

  std::string source_step, source_data;
  std::string target_step, target_data;

  parseDataName(source, source_step, source_data);
  parseDataName(target, target_step, target_data);

  cedar::proc::Step::connect(
                              this->getStep(source_step),
                              source_data,
                              this->getStep(target_step),
                              target_data
                            );
}

void cedar::proc::Manager::readConnections(const ConfigurationNode& root)
{
  for (ConfigurationNode::const_iterator iter = root.begin();
      iter != root.end();
      ++iter)
  {
    this->readConnection(iter->second);
  }
}

void cedar::proc::Manager::parseDataName(const std::string& instr, std::string& stepName, std::string& dataName)
{
  size_t dot_idx = instr.rfind('.');
  if (dot_idx == std::string::npos || dot_idx == 0 || dot_idx == instr.length()-1)
  {
    CEDAR_THROW(cedar::proc::InvalidNameException, "Invalid data name for step. Path is: " + instr);
  }

  stepName = instr.substr(0, dot_idx);
  dataName = instr.substr(dot_idx+1, instr.length() - dot_idx - 1);
}
