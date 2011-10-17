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
#include "processing/DataSlot.h"
#include "processing/Group.h"
#include "processing/Connection.h"
#include "auxiliaries/Data.h"
#include "auxiliaries/assert.h"

// PROJECT INCLUDES

// SYSTEM INCLUDES
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/string_path.hpp>
#include <algorithm>

// Define that helps to debug file reading.
//#define DEBUG_FILE_READING
//#define DEBUG_FILE_WRITING

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::proc::Network::Network()
{
#ifdef DEBUG
  std::cout << "> allocated data (cedar::proc::Network, " << this << ")" << std::endl;
#endif // DEBUG
}

cedar::proc::Network::~Network()
{
#ifdef DEBUG
  std::cout << "> freeing data (cedar::proc::Network, " << this << ")" << std::endl;

  for (size_t i = 0; i < this->steps().size(); ++i)
  {
    std::cout << "> [freeing data] Use count of step " << this->mSteps[i] << " is " << this->mSteps[i].use_count() << std::endl;
  }
#endif // DEBUG
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

cedar::proc::Network::StepVector& cedar::proc::Network::steps()
{
  return this->mSteps;
}

const cedar::proc::Network::StepVector& cedar::proc::Network::steps() const
{
  return this->mSteps;
}

const cedar::proc::Network::TriggerVector& cedar::proc::Network::triggers() const
{
  return this->mTriggers;
}

cedar::proc::Network::TriggerVector& cedar::proc::Network::triggers()
{
  return this->mTriggers;
}

const cedar::proc::Network::GroupVector& cedar::proc::Network::groups() const
{
  return this->mGroups;
}

cedar::proc::Network::GroupVector& cedar::proc::Network::groups()
{
  return this->mGroups;
}

void cedar::proc::Network::add(cedar::proc::StepPtr step)
{
#ifdef DEBUG_FILE_WRITING
    std::cout << "Adding step " << step->getName() << " to network." << std::endl;
#endif
  this->mSteps.push_back(step);
}

void cedar::proc::Network::remove(cedar::proc::StepPtr step)
{
  std::vector<StepPtr>::iterator it;
  // iterator to vector element:
  it = std::find (this->mSteps.begin(), this->mSteps.end(), step);
  if (it != this->mSteps.end())
  {
    mSteps.erase(it);
  }
}

void cedar::proc::Network::add(cedar::proc::TriggerPtr trigger)
{
#ifdef DEBUG_FILE_WRITING
    std::cout << "Adding trigger " << trigger->getName() << " to network." << std::endl;
#endif
  this->mTriggers.push_back(trigger);
}

void cedar::proc::Network::remove(cedar::proc::TriggerPtr trigger)
{
  std::vector<TriggerPtr>::iterator it;
  // iterator to vector element:
  it = std::find (this->mTriggers.begin(), this->mTriggers.end(), trigger);
  if (it != this->mTriggers.end())
  {
    mTriggers.erase(it);
  }
}

void cedar::proc::Network::add(cedar::proc::GroupPtr group)
{
#ifdef DEBUG_FILE_WRITING
    std::cout << "Adding group " << group->getName() << " to network." << std::endl;
#endif
  this->mGroups.push_back(group);
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

void cedar::proc::Network::writeFile(const std::string& filename)
{
#ifdef DEBUG_FILE_READING
  std::cout << "Writing configuration file " << filename << std::endl;
#endif // DEBUG_FILE_READING
  cedar::aux::ConfigurationNode cfg;
  this->saveTo(cfg);
  boost::property_tree::write_json(filename, cfg);
}

void cedar::proc::Network::saveTo(cedar::aux::ConfigurationNode& root)
{
  cedar::aux::ConfigurationNode steps;
  this->saveSteps(steps);
  if (!steps.empty())
    root.add_child("steps", steps);

  cedar::aux::ConfigurationNode triggers;
  this->saveTriggers(triggers);
  if (!triggers.empty())
    root.add_child("triggers", triggers);

  cedar::aux::ConfigurationNode groups;
  this->saveGroups(groups);
  if (!groups.empty())
    root.add_child("groups", groups);

  cedar::aux::ConfigurationNode connections;
  this->saveDataConnections(connections);
  if (!connections.empty())
    root.add_child("connections", connections);
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
    const cedar::aux::ConfigurationNode& groups = root.get_child("groups");
    this->readGroups(groups);
  }
  catch (const boost::property_tree::ptree_bad_path&)
  {
    // no connections declared -- this is ok.
#if defined DEBUG || defined DEBUG_FILE_READING
    std::cout << "No groups present while reading configuration." << std::endl;
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

void cedar::proc::Network::saveGroups(cedar::aux::ConfigurationNode& root)
{
  for (cedar::proc::Manager::GroupRegistry::iterator iter = cedar::proc::Manager::getInstance().groups().begin();
       iter != cedar::proc::Manager::getInstance().groups().end();
       ++iter
       )
  {
    cedar::aux::ConfigurationNode group;
    (*iter)->saveConfiguration(group);
    root.push_back(cedar::aux::ConfigurationNode::value_type("group", group));
  }
}

void cedar::proc::Network::readGroups(const cedar::aux::ConfigurationNode& root)
{
  for (cedar::aux::ConfigurationNode::const_iterator iter = root.begin();
      iter != root.end();
      ++iter)
  {
    //!@todo Should the allocateGroup function be part of the cedar::proc::Network class instead of cedar::proc::Manager?
    cedar::proc::GroupPtr group (cedar::proc::Manager::getInstance().allocateGroup());
    group->readConfiguration(iter->second);
  }
}

void cedar::proc::Network::saveDataConnection(cedar::aux::ConfigurationNode& root, const cedar::proc::Connection* pConnection)
{
  //!@todo move this code to the connection class?

  if (!pConnection->getSource())
  {
    // this happens when the source is a trigger
    CEDAR_DEBUG_ASSERT(pConnection->getSourceTrigger().get() != NULL);
    return;
  }
  if (!pConnection->getTarget())
  {
    // this happens when the target is a trigger
    CEDAR_DEBUG_ASSERT(pConnection->getTargetTrigger().get() != NULL);
    return;
  }
  std::string source_str = pConnection->getSource()->getName() + "." + pConnection->getSourceName();
  std::string target_str = pConnection->getTarget()->getName() + "." + pConnection->getTargetName();

  cedar::aux::ConfigurationNode connection;
  connection.put("source", source_str);
  connection.put("target", target_str);
  root.push_back(cedar::aux::ConfigurationNode::value_type("", connection));
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

  cedar::proc::Manager::getInstance().connect(
                                               cedar::proc::Manager::getInstance().steps().get(source_step),
                                               source_data,
                                               cedar::proc::Manager::getInstance().steps().get(target_step),
                                               target_data
                                             );
}

void cedar::proc::Network::saveDataConnections(cedar::aux::ConfigurationNode& root)
{
  const std::vector<cedar::proc::Connection*>& connections = cedar::proc::Manager::getInstance().getConnections();
  for (std::vector<cedar::proc::Connection*>::const_iterator iter = connections.begin(); iter != connections.end(); ++iter)
  {
    this->saveDataConnection(root, *iter);
  }
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
