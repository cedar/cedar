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
#include "cedar/processing/DeclarationRegistry.h"
#include "cedar/processing/ExternalData.h"
#include "cedar/processing/ElementDeclaration.h"
#include "cedar/processing/TriggerConnection.h"
#include "cedar/auxiliaries/Data.h"
#include "cedar/auxiliaries/assert.h"
#include "cedar/processing/Element.h"

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
#endif // DEBUG
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::proc::Network::reset()
{
  for (ElementMap::iterator iter = this->mElements.begin(); iter != this->mElements.end(); ++iter)
  {
    if (cedar::proc::StepPtr step = boost::shared_dynamic_cast<cedar::proc::Step>(iter->second))
    {
      step->callReset();
    }
  }
}

const cedar::proc::Network::ElementMap& cedar::proc::Network::elements() const
{
  return this->mElements;
}

void cedar::proc::Network::remove(cedar::proc::ConstElementPtr element)
{
  // first, delete all data connections to and from this Element
  for (
        cedar::proc::Network::DataConnectionVector::iterator data_con = mDataConnections.begin();
        data_con != mDataConnections.end();
        // empty
      )
  {
    if ((*data_con)->getSource()->isParent(boost::shared_dynamic_cast<const cedar::proc::Connectable>(element))
        || (*data_con)->getTarget()->isParent(boost::shared_dynamic_cast<const cedar::proc::Connectable>(element)))
    {
      // if target is not looped, delete the trigger connection
      if (!this->getElement<cedar::proc::Triggerable>((*data_con)->getTarget()->getParent())->isLooped())
      {
        this->disconnectTrigger(
                                 this->getElement<cedar::proc::Triggerable>((*data_con)->getSource()->getParent())->getFinishedTrigger(),
                                 this->getElement<cedar::proc::Triggerable>((*data_con)->getTarget()->getParent())
                               );
      data_con = mDataConnections.erase(data_con);
      }
    }
    else
    {
      ++data_con;
    }
  }
  // then, delete all trigger connections to and from this Element
  for (
        cedar::proc::Network::TriggerConnectionVector::iterator trigger_con = mTriggerConnections.begin();
        trigger_con != mTriggerConnections.end();
        // empty
      )
  {
    if ((*trigger_con)->getSourceTrigger() == boost::shared_dynamic_cast<const cedar::proc::Trigger>(element)
        || (*trigger_con)->getTarget() == boost::shared_dynamic_cast<const cedar::proc::Triggerable>(element))
    {
      trigger_con = mTriggerConnections.erase(trigger_con);
    }
    else
    {
      ++trigger_con;
    }
  }
  cedar::proc::Network::ElementMap::iterator it = mElements.find(element->getName());
  if (it != this->mElements.end())
  {
    mElements.erase(it);
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
  element->setNetwork(this);
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
  // check connection
  if (this->isConnected(source, target))
  {
    CEDAR_THROW(cedar::proc::DuplicateConnectionException, "This connection already exists!")
  }
  // create connection
  mTriggerConnections.push_back(cedar::proc::TriggerConnectionPtr(new TriggerConnection(source, target)));
}

void cedar::proc::Network::disconnectSlots(const std::string& source, const std::string& target)
{
  // parse element and slot name
  std::string source_name;
  std::string source_slot;
  std::string target_name;
  std::string target_slot;
  cedar::proc::Connectable::parseDataNameNoRole(source, source_name, source_slot);
  cedar::proc::Connectable::parseDataNameNoRole(target, target_name, target_slot);
  for (DataConnectionVector::iterator it = mDataConnections.begin(); it != mDataConnections.end(); ++it)
  {
    if ((*it)->equals(
                    this->getElement<cedar::proc::Connectable>(source_name)->getOutputSlot(source_slot),
                    this->getElement<cedar::proc::Connectable>(target_name)->getInputSlot(target_slot)
                  )
       )
    {
      mDataConnections.erase(it);
      // if target is not looped, also delete the trigger connection
      if (!this->getElement<cedar::proc::Triggerable>(target_name)->isLooped())
      {
        this->disconnectTrigger(
                                 this->getElement<cedar::proc::Triggerable>(source_name)->getFinishedTrigger(),
                                 this->getElement<cedar::proc::Triggerable>(target_name)
                               );
      }
      return;
    }
  }
  CEDAR_THROW(cedar::proc::MissingConnectionException, "This data connection does not exist!");
}

void cedar::proc::Network::disconnectTrigger(cedar::proc::TriggerPtr source, cedar::proc::TriggerablePtr target)
{
  for (TriggerConnectionVector::iterator it = mTriggerConnections.begin(); it != mTriggerConnections.end(); ++it)
  {
    if ((*it)->equals(source, target))
    {
      mTriggerConnections.erase(it);
      return;
    }
  }
  CEDAR_THROW(cedar::proc::MissingConnectionException, "This trigger connection does not exist!");
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
  this->writeTo(cfg);
  boost::property_tree::write_json(filename, cfg);
}

void cedar::proc::Network::writeTo(cedar::aux::ConfigurationNode& root)
{
  cedar::aux::ConfigurationNode meta;
  this->writeMetaData(meta);
  if (!meta.empty())
    root.add_child("meta", meta);

  cedar::aux::ConfigurationNode steps;
  this->writeSteps(steps);
  if (!steps.empty())
    root.add_child("steps", steps);

  cedar::aux::ConfigurationNode triggers;
  this->writeTriggers(triggers);
  if (!triggers.empty())
    root.add_child("triggers", triggers);

  cedar::aux::ConfigurationNode connections;
  this->writeDataConnections(connections);
  if (!connections.empty())
    root.add_child("connections", connections);
}

void cedar::proc::Network::writeMetaData(cedar::aux::ConfigurationNode& meta)
{
  meta.put("format", 1);
}

void cedar::proc::Network::readFrom(const cedar::aux::ConfigurationNode& root)
{
  unsigned int format_version = 1; // default value is the current format
  try
  {
    const cedar::aux::ConfigurationNode& meta = root.get_child("meta");
    format_version = meta.get<unsigned int>("format");
  }
  catch (const boost::property_tree::ptree_bad_path&)
  {
    //!@todo Write this to a proper log once we have such a structure
    std::cout << "Error recognizing file format. Defaulting to current file format version." << std::endl;
  }

  switch (format_version)
  {
    default:
      //!@todo Write this to a proper log once we have such a structure
      std::cout << "Unknown format version " << format_version << ". Defaulting to the current version." << std::endl;
    case 1:
      this->readFromV1(root);
      break;
  }
}

void cedar::proc::Network::readFromV1(const cedar::aux::ConfigurationNode& root)
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

void cedar::proc::Network::writeSteps(cedar::aux::ConfigurationNode& steps)
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
    // quick fix
    step->setName(step->getName());
    mElements[step->getName()] = step;
  }
}

void cedar::proc::Network::writeTriggers(cedar::aux::ConfigurationNode& triggers)
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
  }

  for (cedar::aux::ConfigurationNode::const_iterator iter = root.begin();
      iter != root.end();
      ++iter)
  {
    try
    {
      const cedar::aux::ConfigurationNode& trigger_node = iter->second;
      cedar::proc::TriggerPtr trigger = this->getElement<cedar::proc::Trigger>(trigger_node.get_child("name").get_value<std::string>());
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

void cedar::proc::Network::writeDataConnection(cedar::aux::ConfigurationNode& root, const cedar::proc::DataConnectionPtr connection)
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

void cedar::proc::Network::writeDataConnections(cedar::aux::ConfigurationNode& root)
{
  for (DataConnectionVector::const_iterator iter = mDataConnections.begin(); iter != mDataConnections.end(); ++iter)
  {
    this->writeDataConnection(root, *iter);
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

bool cedar::proc::Network::isConnected(cedar::proc::TriggerPtr source, cedar::proc::TriggerablePtr target)
{
  for (size_t i = 0; i < mTriggerConnections.size(); ++i)
  {
    if (mTriggerConnections.at(i)->equals(source, target))
    {
      return true;
    }
  }
  return false;
}

void cedar::proc::Network::updateObjectName(cedar::proc::Element* object)
{
  //!@todo It might be a good idea to clean up invalid pointers here.
  for (ElementMap::iterator iter = this->mElements.begin(); iter != this->mElements.end(); ++iter)
  {
    if (iter->second.get() == object) // found
    {
      // exchange the object in the map - put object at key (new name) and erase old entry
      mElements[object->getName()] = iter->second;
      mElements.erase(iter);
      // there can never be two instances of the same object in this structure. It's safe to return (and avoids
      // mischief with the iterator)
      return;
    }
  }
  CEDAR_THROW(cedar::proc::InvalidObjectException, "Element not registered at this network. Current element name: " + object->getName());
}

void cedar::proc::Network::getDataConnections(
                                               cedar::proc::ConnectablePtr source,
                                               const std::string& sourceDataName,
                                               std::vector<cedar::proc::DataConnectionPtr>& connections
                                             )
{
  connections.clear();
  for (size_t i = 0; i < this->mDataConnections.size(); ++i)
  {
    cedar::proc::DataConnectionPtr con = this->mDataConnections.at(i);
    if (this->getElement<cedar::proc::Step>(con->getSource()->getParent()) == source && con->getSource()->getName() == sourceDataName)
    {
      connections.push_back(con);
    }
  }
}
