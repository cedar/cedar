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
#include "cedar/processing/Network.h"
#include "cedar/processing/Manager.h"
#include "cedar/processing/Step.h"
#include "cedar/processing/DataConnection.h"
#include "cedar/processing/DataSlot.h"
#include "cedar/processing/Group.h"
#include "cedar/processing/Connection.h"
#include "cedar/processing/DeclarationRegistry.h"
#include "cedar/processing/ExternalData.h"
#include "cedar/processing/ElementDeclaration.h"
#include "cedar/processing/TriggerConnection.h"
#include "cedar/auxiliaries/Data.h"
#include "cedar/auxiliaries/assert.h"

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

const cedar::proc::Network::ElementMap& cedar::proc::Network::elements() const
{
  return this->mElements;
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

void cedar::proc::Network::add(std::string className, std::string instanceName)
{
  this->add(DeclarationRegistrySingleton::getInstance()->allocateClass(className), instanceName);
}

void cedar::proc::Network::add(cedar::proc::ElementPtr element, std::string instanceName)
{
  element->setName(instanceName);
  this->add(element);
}

void cedar::proc::Network::add(cedar::proc::ElementPtr element)
{
  std::string instanceName = element->getName();
  if (instanceName.empty())
  {
    CEDAR_THROW(cedar::proc::InvalidNameException, "no name present for given element in this module")
  }
  else if (mElements.find(instanceName) != mElements.end())
  {
    CEDAR_THROW(cedar::proc::DuplicateNameException, "duplicate entry of elements in this module")
  }
  else
  {
    mElements[instanceName] = element;
  }
}

cedar::proc::ElementPtr cedar::proc::Network::getElement(const std::string& name)
{
  ElementMap::iterator iter = this->mElements.find(name);

  if (iter != this->mElements.end())
  {
    return iter->second;
  }
  else
  {
    CEDAR_THROW(cedar::proc::InvalidNameException, "No element of the name " + name + " was found.");
  }
}


void cedar::proc::Network::add(cedar::proc::GroupPtr group)
{
#ifdef DEBUG_FILE_WRITING
    std::cout << "Adding group " << group->getName() << " to network." << std::endl;
#endif
  this->mGroups.push_back(group);
}

void cedar::proc::Network::connectSlots(const std::string& source, const std::string& target)
{
  // parse element and slot name
  std::string source_name;
  std::string source_slot;
  std::string target_name;
  std::string target_slot;
  cedar::proc::Connectable::parseDataNameNoRole(source, source_name, source_slot);
  cedar::proc::Connectable::parseDataNameNoRole(target, target_name, target_slot);
  // check connection
  if (this->isConnected(source, target))
  {
    CEDAR_THROW(cedar::proc::DuplicateConnectionException, "This connection already exists!")
  }
  // create connection
  mDataConnections.push_back(cedar::proc::DataConnectionPtr(
                               new DataConnection(
                                                   this->getElement<cedar::proc::Connectable>(source_name)->getOutputSlot(source_slot),
                                                   this->getElement<cedar::proc::Connectable>(target_name)->getInputSlot(target_slot)
                                                 )
                                                       )
                        );
  if (!this->getElement<cedar::proc::Triggerable>(target_name)->isLooped())
  {
    this->connectTrigger(this->getElement<cedar::proc::Triggerable>(source_name)->getFinishedTrigger(), this->getElement<cedar::proc::Step>(target_name));
  }
}

void cedar::proc::Network::connectTrigger(cedar::proc::TriggerPtr source, cedar::proc::TriggerablePtr target)
{
//  if (this->isConnected(source, target))
//  {
//    CEDAR_THROW(cedar::proc::DuplicateConnectionException, "This connection already exists!")
//  }
  // create connection
  mTriggerConnections.push_back(cedar::proc::TriggerConnectionPtr(new TriggerConnection(source, target)));
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
  for (ElementMap::iterator iter = this->mElements.begin(); iter != this->mElements.end(); ++iter)
  {
    // if this is a step, write this to the configuration tree
    if (cedar::proc::StepPtr step = boost::shared_dynamic_cast<cedar::proc::Step>(iter->second))
    {
#ifdef DEBUG_FILE_WRITING
      std::cout << "Saving " << iter->first << "." << std::endl;
#endif
      cedar::proc::ElementDeclarationPtr decl = DeclarationRegistrySingleton::getInstance()->getDeclarationOf(step);
      cedar::aux::ConfigurationNode step_node;
      step->writeConfiguration(step_node);
      steps.push_back(cedar::aux::ConfigurationNode::value_type(decl->getClassId(), step_node));
    }
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

    cedar::proc::ElementPtr step = cedar::proc::DeclarationRegistrySingleton::getInstance()->allocateClass(class_id);
    step->readConfiguration(step_node);
    mElements[step->getName()] = step;
  }
}

void cedar::proc::Network::saveTriggers(cedar::aux::ConfigurationNode& triggers)
{
  for (ElementMap::iterator iter = this->mElements.begin(); iter != this->mElements.end(); ++iter)
  {
    // if this is a trigger, write this to the configuration tree
    if (cedar::proc::TriggerPtr trigger = boost::shared_dynamic_cast<cedar::proc::Trigger>(iter->second))
    {
#ifdef DEBUG_FILE_WRITING
      std::cout << "Saving " << iter->first << "." << std::endl;
#endif
      cedar::proc::ElementDeclarationPtr decl = DeclarationRegistrySingleton::getInstance()->getDeclarationOf(trigger);
      cedar::aux::ConfigurationNode trigger_node;
      trigger->writeConfiguration(trigger_node);
      triggers.push_back(cedar::aux::ConfigurationNode::value_type(decl->getClassId(), trigger_node));
    }
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

    cedar::proc::TriggerPtr trigger = boost::shared_dynamic_cast<cedar::proc::Trigger>(cedar::proc::DeclarationRegistrySingleton::getInstance()->allocateClass(class_id));
    trigger->readConfiguration(trigger_node);
    this->mElements[trigger->getName()] = trigger;
    try
    {
      const cedar::aux::ConfigurationNode& listeners = trigger_node.get_child("listeners");

      for (cedar::aux::ConfigurationNode::const_iterator listener_iter = listeners.begin();
          listener_iter != listeners.end();
          ++listener_iter)
      {
        std::string listener_name = listener_iter->second.data();

        cedar::proc::TriggerablePtr triggerable = this->getElement<Triggerable>(listener_name);
        this->connectTrigger(trigger, triggerable);
      }
    }
    catch (const boost::property_tree::ptree_bad_path&)
    {
      // no listeners declared -- this is ok.
    }
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

void cedar::proc::Network::saveDataConnection(cedar::aux::ConfigurationNode& root, const cedar::proc::DataConnectionPtr connection)
{
  std::string source_str = connection->getSource()->getParent() + "." + connection->getSource()->getName();
  std::string target_str = connection->getTarget()->getParent() + "." + connection->getTarget()->getName();

  cedar::aux::ConfigurationNode connection_node;
  connection_node.put("source", source_str);
  connection_node.put("target", target_str);
  root.push_back(cedar::aux::ConfigurationNode::value_type("", connection_node));
}

void cedar::proc::Network::readDataConnection(const cedar::aux::ConfigurationNode& root)
{
  std::string source = root.get<std::string>("source");
  std::string target = root.get<std::string>("target");

#ifdef DEBUG_FILE_READING
  std::cout << "Connecting data: source = \"" << source
            << "\", target = \"" << target << "\"" << std::endl;
#endif // DEBUG_FILE_READING
  this->connectSlots(source, target);
}

void cedar::proc::Network::saveDataConnections(cedar::aux::ConfigurationNode& root)
{
  for (DataConnectionVector::const_iterator iter = mDataConnections.begin(); iter != mDataConnections.end(); ++iter)
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

bool cedar::proc::Network::isConnected(const std::string& source, const std::string& target)
{
  // parse element and slot name
  std::string source_name;
  std::string source_slot;
  std::string target_name;
  std::string target_slot;
  cedar::proc::Connectable::parseDataNameNoRole(source, source_name, source_slot);
  cedar::proc::Connectable::parseDataNameNoRole(target, target_name, target_slot);
  for (size_t i = 0; i < mDataConnections.size(); ++i)
  {
    if (mDataConnections.at(i)->equals(
                                    this->getElement<cedar::proc::Connectable>(source_name)->getOutputSlot(source_slot),
                                    this->getElement<cedar::proc::Connectable>(target_name)->getInputSlot(target_slot)
                                  )
       )
    {
      return true;
    }
  }
  return false;
}
