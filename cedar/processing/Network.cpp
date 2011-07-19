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

    File:        Network.cpp

    Maintainer:  Oliver Lomp,
                 Mathis Richter,
                 Stephan Zibner
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de,
                 mathis.richter@ini.ruhr-uni-bochum.de,
                 stephan.zibner@ini.ruhr-uni-bochum.de
    Date:        2011 07 19

    Description:

    Credits:

======================================================================================================================*/

// LOCAL INCLUDES
#include "processing/Network.h"
#include "processing/Manager.h"
#include "processing/Step.h"

// PROJECT INCLUDES

// SYSTEM INCLUDES
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/string_path.hpp>

// Define that helps to debug file reading.
//#define DEBUG_FILE_READING
#define DEBUG_FILE_WRITING

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::proc::Network::Network()
{
}

cedar::proc::Network::~Network()
{
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::proc::Network::add(cedar::proc::StepPtr step)
{
#ifdef DEBUG_FILE_WRITING
    std::cout << "Adding step " << step->getName() << " to network." << std::endl;
#endif
  this->mSteps.push_back(step);
}

void cedar::proc::Network::add(cedar::proc::TriggerPtr trigger)
{
#ifdef DEBUG_FILE_WRITING
    std::cout << "Adding trigger " << trigger->getName() << " to network." << std::endl;
#endif
  this->mTriggers.push_back(trigger);
}

void cedar::proc::Network::readFile(const std::string& filename)
{
#ifdef DEBUG_FILE_READING
  std::cout << "Reading configuration file " << filename << std::endl;
#endif // DEBUG_FILE_READING
  cedar::aux::ConfigurationNode cfg;
  boost::property_tree::read_json(filename, cfg);
  this->readFrom(cfg);
}

void cedar::proc::Network::saveTo(cedar::aux::ConfigurationNode& root)
{
  cedar::aux::ConfigurationNode steps;
  this->saveSteps(steps);
  root.add_child("steps", steps);

  cedar::aux::ConfigurationNode triggers;
  this->saveTriggers(triggers);
  root.add_child("triggers", triggers);
  // TODO connections
}

void cedar::proc::Network::readFrom(const cedar::aux::ConfigurationNode& root)
{
  try
  {
    const cedar::aux::ConfigurationNode& steps = root.get_child("steps");
    this->readSteps(steps);
  }
  catch (const boost::property_tree::ptree_bad_path&)
  {
    // no steps declared -- this is ok.
#if defined DEBUG || defined DEBUG_FILE_READING
    std::cout << "No steps present while reading configuration." << std::endl;
#endif // defined DEBUG || defined DEBUG_FILE_READING
  }

  try
  {
    const cedar::aux::ConfigurationNode& connections = root.get_child("connections");
    this->readDataConnections(connections);
  }
  catch (const boost::property_tree::ptree_bad_path&)
  {
    // no connections declared -- this is ok.
#if defined DEBUG || defined DEBUG_FILE_READING
    std::cout << "No data connections present while reading configuration." << std::endl;
#endif // defined DEBUG || defined DEBUG_FILE_READING
  }

  try
  {
    const cedar::aux::ConfigurationNode& triggers = root.get_child("triggers");
    this->readTriggers(triggers);
  }
  catch (const boost::property_tree::ptree_bad_path&)
  {
#if defined DEBUG || defined DEBUG_FILE_READING
    std::cout << "No triggers present while reading configuration." << std::endl;
#endif // defined DEBUG || defined DEBUG_FILE_READING
  }
}

void cedar::proc::Network::saveSteps(cedar::aux::ConfigurationNode& steps)
{
#ifdef DEBUG_FILE_WRITING
    std::cout << "Saving " << this->mSteps.size() << " steps." << std::endl;
#endif

  for (size_t i = 0; i < this->mSteps.size(); ++i)
  {
    cedar::proc::StepPtr& step = this->mSteps.at(i);
    cedar::proc::StepDeclarationPtr decl = cedar::proc::Manager::getInstance().steps().getDeclarationOf(step);

    cedar::aux::ConfigurationNode step_node;
    step->saveConfiguration(step_node);
    steps.push_back(cedar::aux::ConfigurationNode::value_type(decl->getClassId(), step_node));
  }
}

void cedar::proc::Network::readSteps(const cedar::aux::ConfigurationNode& root)
{
#ifdef DEBUG_FILE_READING
  std::cout << "Reading steps." << std::endl;
#endif // DEBUG_FILE_READING

  for (cedar::aux::ConfigurationNode::const_iterator iter = root.begin();
      iter != root.end();
      ++iter)
  {
    const std::string class_id = iter->first;
    const cedar::aux::ConfigurationNode& step_node = iter->second;

  #ifdef DEBUG_FILE_READING
    std::cout << "Reading step of type " << class_id << std::endl;
  #endif // DEBUG_FILE_READING

    cedar::proc::StepPtr step = cedar::proc::Manager::getInstance().steps().allocateClass(class_id);
    step->readConfiguration(step_node);
    cedar::proc::Manager::getInstance().steps().registerObject(step);
    mSteps.push_back(step);
  }
}

void cedar::proc::Network::saveTriggers(cedar::aux::ConfigurationNode& triggers)
{
#ifdef DEBUG_FILE_WRITING
    std::cout << "Saving " << this->mTriggers.size() << " triggers." << std::endl;
#endif

  for (size_t i = 0; i < this->mTriggers.size(); ++i)
  {
    cedar::proc::TriggerPtr& trigger = this->mTriggers.at(i);
    cedar::proc::TriggerDeclarationPtr decl = cedar::proc::Manager::getInstance().triggers().getDeclarationOf(trigger);

    cedar::aux::ConfigurationNode trigger_node;
    trigger->saveConfiguration(trigger_node);
    triggers.push_back(cedar::aux::ConfigurationNode::value_type(decl->getClassId(), trigger_node));
  }
}

void cedar::proc::Network::readTriggers(const cedar::aux::ConfigurationNode& root)
{
#ifdef DEBUG_FILE_READING
  std::cout << "Reading triggers." << std::endl;
#endif // DEBUG_FILE_READING

  for (cedar::aux::ConfigurationNode::const_iterator iter = root.begin();
      iter != root.end();
      ++iter)
  {
    const std::string& class_id = iter->first;
    const cedar::aux::ConfigurationNode& trigger_node = iter->second;

#ifdef DEBUG_FILE_READING
    std::cout << "Reading trigger of type " << class_id << std::endl;
#endif // DEBUG_FILE_READING

    cedar::proc::TriggerPtr trigger = cedar::proc::Manager::getInstance().triggers().allocateClass(class_id);
    trigger->readConfiguration(trigger_node);
    cedar::proc::Manager::getInstance().triggers().registerObject(trigger);
    this->mTriggers.push_back(trigger);
  }
}

void cedar::proc::Network::saveDataConnection(cedar::aux::ConfigurationNode& root)
{
  // FIXME
}

void cedar::proc::Network::readDataConnection(const cedar::aux::ConfigurationNode& root)
{
  std::string source = root.get<std::string>("source");
  std::string target = root.get<std::string>("target");

#ifdef DEBUG_FILE_READING
  std::cout << "Connecting data: source = \"" << source
            << "\", target = \"" << target << "\"" << std::endl;
#endif // DEBUG_FILE_READING

  std::string source_step, source_data;
  std::string target_step, target_data;

  cedar::proc::Step::parseDataName(source, source_step, source_data);
  cedar::proc::Step::parseDataName(target, target_step, target_data);

  cedar::proc::Step::connect(
                              cedar::proc::Manager::getInstance().steps().get(source_step),
                              source_data,
                              cedar::proc::Manager::getInstance().steps().get(target_step),
                              target_data
                            );
}

void cedar::proc::Network::saveDataConnections(cedar::aux::ConfigurationNode& root)
{
  // FIXME
}

void cedar::proc::Network::readDataConnections(const cedar::aux::ConfigurationNode& root)
{
#ifdef DEBUG_FILE_READING
  std::cout << "Reading data connections." << std::endl;
#endif // DEBUG_FILE_READING

  for (cedar::aux::ConfigurationNode::const_iterator iter = root.begin();
      iter != root.end();
      ++iter)
  {
    this->readDataConnection(iter->second);
  }
}
