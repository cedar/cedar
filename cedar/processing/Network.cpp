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

// CEDAR INCLUDES
#include "cedar/processing/Network.h"
#include "cedar/processing/Manager.h"
#include "cedar/processing/Step.h"
#include "cedar/processing/DataConnection.h"
#include "cedar/processing/DataSlot.h"
#include "cedar/processing/DeclarationRegistry.h"
#include "cedar/processing/ExternalData.h"
#include "cedar/processing/ElementDeclaration.h"
#include "cedar/processing/TriggerConnection.h"
#include "cedar/processing/PromotedExternalData.h"
#include "cedar/auxiliaries/Data.h"
#include "cedar/auxiliaries/assert.h"

// SYSTEM INCLUDES
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/string_path.hpp>
#include <algorithm>

// Define that helps to debug file reading.
#define DEBUG_FILE_READING
#define DEBUG_FILE_WRITING

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

std::string cedar::proc::Network::getUniqueIdentifier(const std::string& identifier) const
{
  if (this->mElements.find(identifier) == this->mElements.end())
  {
    return identifier;
  }

  unsigned int count = 2;
  std::string result;
  do
  {
    result = identifier +  " " + cedar::aux::toString(count);
    ++count;
  }
  while(this->mElements.find(result) != this->mElements.end());

  return result;
}

void cedar::proc::Network::listSubnetworks(std::set<cedar::proc::ConstNetworkPtr>& subnetworks) const
{
  subnetworks.clear();
  for (ElementMap::const_iterator iter = this->mElements.begin(); iter != this->mElements.end(); ++iter)
  {
    if (cedar::proc::ConstNetworkPtr network = boost::shared_dynamic_cast<const cedar::proc::Network>(iter->second))
    {
      subnetworks.insert(network);
    }
  }
}

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
//      if (!this->getElement<cedar::proc::Triggerable>((*data_con)->getTarget()->getParent())->isLooped())
//      {
//        this->disconnectTrigger(
//                                 this->getElement<cedar::proc::Triggerable>((*data_con)->getSource()->getParent())->getFinishedTrigger(),
//                                 this->getElement<cedar::proc::Triggerable>((*data_con)->getTarget()->getParent())
//                               );
//      data_con = mDataConnections.erase(data_con);
//      }
        data_con = this->removeDataConnection(data_con);
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
  cedar::proc::ElementPtr element = DeclarationRegistrySingleton::getInstance()->allocateClass(className);
  this->add(element, instanceName);
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
    element->resetChangedStates(true);
  }
  if (cedar::proc::NetworkPtr old_network = element->getNetwork()) // element was registered elsewhere
  {
    old_network->remove(element);
  }
  element->setNetwork(boost::shared_static_cast<cedar::proc::Network>(this->shared_from_this()));
}

cedar::proc::ConstElementPtr cedar::proc::Network::getElement(const std::string& name) const
{
  ElementMap::const_iterator iter;
  std::string first;
  std::string rest;
  cedar::aux::splitFirst(name, ".", first, rest);
  if (first.length() != 0 && rest.length() != 0)
  {
    return this->getElement<Network>(first)->getElement(rest);
  }
  else
  {
    iter = this->mElements.find(name);
  }

  if (iter != this->mElements.end())
  {
    return iter->second;
  }
  else
  {
    CEDAR_THROW(cedar::proc::InvalidNameException, "No element of the name " + name + " was found.");
  }
}

cedar::proc::ElementPtr cedar::proc::Network::getElement(const std::string& name)
{
  return boost::const_pointer_cast<Element>(static_cast<const Network*>(this)->getElement(name));
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
  mDataConnections.push_back
  (
    cedar::proc::DataConnectionPtr
    (
      new DataConnection
      (
        this->getElement<cedar::proc::Connectable>(source_name)->getOutputSlot(source_slot),
        this->getElement<cedar::proc::Connectable>(target_name)->getInputSlot(target_slot)
      )
    )
  );
  cedar::proc::TriggerablePtr p_source = this->getElement<cedar::proc::Triggerable>(source_name);
  cedar::proc::TriggerablePtr p_target = this->getElement<cedar::proc::Triggerable>(target_name);
  // target may be a nested Triggerable, recover!
  if (!p_target)
  {
    cedar::proc::PromotedExternalDataPtr promoted_slot
      = boost::shared_dynamic_cast<cedar::proc::PromotedExternalData>
        (
          this->getElement<cedar::proc::Connectable>(target_name)->getInputSlot(target_slot)
        );
    CEDAR_DEBUG_ASSERT(promoted_slot);
    std::string path = promoted_slot->getPromotionPath();
    p_target = this->getElement<cedar::proc::Triggerable>(path);
  }
  CEDAR_DEBUG_ASSERT(p_target);
  if (!p_target->isLooped())
  {
    try
    {
      this->connectTrigger(
                            this->getElement<cedar::proc::Triggerable>(source_name)->getFinishedTrigger(),
                            p_target
                          );
    }
    catch(const cedar::proc::DuplicateConnectionException&)
    {
      // if the triggers are already connected, that's ok.
    }
    // looped elements send trigger signals only when the architecture is running
    if (!p_source || !p_source->isLooped())
    {
      p_target->onTrigger();
    }
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
    cedar::proc::ConnectablePtr target_connectable = this->getElement<cedar::proc::Connectable>(target_name);
    if ((*it)->equals(
                       this->getElement<cedar::proc::Connectable>(source_name)->getOutputSlot(source_slot),
                       target_connectable->getInputSlot(target_slot)
                     )
       )
    {
      this->removeDataConnection(it);
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

  cedar::aux::ConfigurationNode networks;
  this->writeNetworks(networks);
  if (!networks.empty())
    root.add_child("networks", networks);

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
    const cedar::aux::ConfigurationNode& networks = root.get_child("networks");
    this->readNetworks(networks);
  }
  catch (const boost::property_tree::ptree_bad_path&)
  {
    // no networks declared -- this is ok.
#if defined DEBUG || defined DEBUG_FILE_READING
    std::cout << "No networks present while reading configuration." << std::endl;
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
    this->add(step);
    step->resetChangedStates(false);
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

    cedar::proc::TriggerPtr trigger
      = boost::shared_dynamic_cast<cedar::proc::Trigger>
        (
          cedar::proc::DeclarationRegistrySingleton::getInstance()->allocateClass(class_id)
        );
    trigger->readConfiguration(trigger_node);
    this->add(trigger);
    trigger->resetChangedStates(false);
  }

  for (cedar::aux::ConfigurationNode::const_iterator iter = root.begin();
      iter != root.end();
      ++iter)
  {
    try
    {
      const cedar::aux::ConfigurationNode& trigger_node = iter->second;
      cedar::proc::TriggerPtr trigger
        = this->getElement<cedar::proc::Trigger>(trigger_node.get_child("name").get_value<std::string>());
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

void cedar::proc::Network::writeNetworks(cedar::aux::ConfigurationNode& networks)
{
  for (ElementMap::iterator iter = this->mElements.begin(); iter != this->mElements.end(); ++iter)
  {
    // if this is a network, write this to the configuration tree
    if (cedar::proc::NetworkPtr network = boost::shared_dynamic_cast<cedar::proc::Network>(iter->second))
    {
#ifdef DEBUG_FILE_WRITING
      std::cout << "Saving " << iter->first << "." << std::endl;
#endif
      //cedar::proc::ElementDeclarationPtr decl = DeclarationRegistrySingleton::getInstance()->getDeclarationOf(trigger);
      cedar::aux::ConfigurationNode network_node;
      network->writeTo(network_node);
      networks.push_back(cedar::aux::ConfigurationNode::value_type(iter->first, network_node));
    }
  }
}

void cedar::proc::Network::readNetworks(const cedar::aux::ConfigurationNode& root)
{
#ifdef DEBUG_FILE_READING
  std::cout << "Reading networks." << std::endl;
#endif // DEBUG_FILE_READING

  for (cedar::aux::ConfigurationNode::const_iterator iter = root.begin();
      iter != root.end();
      ++iter)
  {
    //const std::string& class_id = iter->first;
    const std::string& network_name = iter->first;
    const cedar::aux::ConfigurationNode& network_node = iter->second;

#ifdef DEBUG_FILE_READING
    std::cout << "Reading network named " << network_name << std::endl;
#endif // DEBUG_FILE_READING

    cedar::proc::NetworkPtr network
      = boost::shared_dynamic_cast<cedar::proc::Network>
        (
          cedar::proc::DeclarationRegistrySingleton::getInstance()->allocateClass("cedar.processing.Network")
        );
    network->readFrom(network_node);
    network->setName(network_name);
    // is this enough to recursively read in the network?
    this->add(network);
    network->resetChangedStates(false);
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

bool cedar::proc::Network::isConnected(const std::string& source, const std::string& target) const
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

bool cedar::proc::Network::isConnected(cedar::proc::TriggerPtr source, cedar::proc::TriggerablePtr target) const
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
  ElementMap::iterator old_iter;
  //!@todo It might be a good idea to clean up invalid pointers here.
  for (old_iter = this->mElements.begin(); old_iter != this->mElements.end(); ++old_iter)
  {
    if (old_iter->second.get() == object) // found
    {
      break;
    }
  }

  if (old_iter == this->mElements.end())
  {
    CEDAR_THROW
    (
      cedar::proc::InvalidObjectException,
      "Element not registered at this network. Current element name: " + object->getName()
    );
  }

  // exchange the object in the map - put object at key (new name) and erase old entry
  // make a copy of the element pointer
  cedar::proc::ElementPtr element = old_iter->second;

  // if new and old name are the same, do nothing
  if (element->getName() == old_iter->first)
  {
    return;
  }

  // check that the new name is not already in use
  if (this->mElements.find(object->getName()) != this->mElements.end())
  {
    // if it is, restore the old name.
    object->setName(old_iter->first);
    CEDAR_THROW(cedar::proc::DuplicateNameException, "Element name is already in use."
         " Old name of the element: \"" + old_iter->first + "\", requested name: \"" + object->getName() + "\".");
  }

  // erase the iterator
  mElements.erase(old_iter);
  // now we can reinsert the element (this invalidates the iterator)
  mElements[object->getName()] = element;
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
    if
    (
      this->getElement<cedar::proc::Step>(con->getSource()->getParent()) == source
        && con->getSource()->getName() == sourceDataName
    )
    {
      connections.push_back(con);
    }
  }
}

cedar::proc::Network::DataConnectionVector::iterator cedar::proc::Network::removeDataConnection
                                                     (
                                                       cedar::proc::Network::DataConnectionVector::iterator it
                                                     )
{
  std::string source_name = (*it)->getSource()->getParent();
  std::string target_name = (*it)->getTarget()->getParent();
  it = mDataConnections.erase(it);
  // if target is not looped, also delete the trigger connection
  cedar::proc::TriggerablePtr triggerable_target = this->getElement<cedar::proc::Triggerable>(target_name);
  CEDAR_DEBUG_ASSERT(triggerable_target);
  if (!this->getElement<cedar::proc::Triggerable>(target_name)->isLooped())
  {
    // check that both Connectables are not connected through some other DataSlots
    for (DataConnectionVector::iterator iter = mDataConnections.begin(); iter != mDataConnections.end(); ++iter)
    {
      cedar::proc::ConnectablePtr target_connectable = this->getElement<cedar::proc::Connectable>(target_name);
      if ((*iter)->connects(
                             this->getElement<cedar::proc::Connectable>(source_name),
                             target_connectable
                           )
         )
      {
        // found another connection between those two Connectables, do not delete done trigger and return
        return it;
      }
    }
    // found no other connection, delete the TriggerConnection as well
    this->disconnectTrigger(
                             this->getElement<cedar::proc::Triggerable>(source_name)->getFinishedTrigger(),
                             this->getElement<cedar::proc::Triggerable>(target_name)
                           );
    triggerable_target->onTrigger();
  }
  return it;
}

std::string cedar::proc::Network::findPath(cedar::proc::ConstElementPtr findMe) const
{
  // first, try to find element in this network
  try
  {
    if (findMe == this->getElement<const cedar::proc::Element>(findMe->getName()))
    {
      return findMe->getName();
    }
  }
  catch (cedar::proc::InvalidNameException& e) // this can happen if element is not found, no problem, see below
  {
  }
  // if element is not found, search in child networks
  for (ElementMap::const_iterator iter = this->mElements.begin(); iter != this->mElements.end(); ++iter)
  {
    if (cedar::proc::ConstNetworkPtr network = boost::shared_dynamic_cast<cedar::proc::Network>(iter->second))
    {
      std::string found = network->findPath(findMe);
      if (found != "" && findMe == network->getElement<const cedar::proc::Element>(found))
      {
        return network->getName() + std::string(".") + found;
      }
    }
  }
  return std::string("");
}

void cedar::proc::Network::promoteSlot(DataSlotPtr promotedSlot)
{
  this->declarePromotedData(promotedSlot);
  this->mSlotChanged();
}

void cedar::proc::Network::demoteSlot(const std::string& /*name*/)
{
  //!@todo implement demotion of slots
  std::cout << "todo: implement demotion of slots" << std::endl;
  this->mSlotChanged();
}

boost::signals2::connection cedar::proc::Network::connectToSlotChangedSignal(boost::function<void ()> slot)
{
  return mSlotChanged.connect(slot);
}
