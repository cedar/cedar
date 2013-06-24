/*======================================================================================================================

    Copyright 2011, 2012, 2013 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany
 
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
#include "cedar/processing/Step.h"
#include "cedar/processing/DataConnection.h"
#include "cedar/processing/DataSlot.h"
#include "cedar/processing/ExternalData.h"
#include "cedar/processing/ElementDeclaration.h"
#include "cedar/processing/TriggerConnection.h"
#include "cedar/processing/PromotedExternalData.h"
#include "cedar/processing/PromotedOwnedData.h"
#include "cedar/processing/ElementDeclaration.h"
#include "cedar/processing/DeclarationRegistry.h"
#include "cedar/processing/exceptions.h"
#include "cedar/processing/LoopedTrigger.h"
#include "cedar/auxiliaries/StringVectorParameter.h"
#include "cedar/auxiliaries/Parameter.h"
#include "cedar/auxiliaries/Log.h"
#include "cedar/auxiliaries/Data.h"
#include "cedar/auxiliaries/assert.h"
#include "cedar/auxiliaries/Log.h"

#include "cedar/processing/consistency/LoopedStepNotConnected.h"

// SYSTEM INCLUDES
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/string_path.hpp>
#include <boost/make_shared.hpp>
#include <algorithm>

// Define that helps to debug file reading.
//#define DEBUG_FILE_READING
//#define DEBUG_FILE_WRITING

//----------------------------------------------------------------------------------------------------------------------
// register the class
//----------------------------------------------------------------------------------------------------------------------
namespace
{
  bool declare()
  {
    using cedar::proc::ElementDeclarationPtr;
    using cedar::proc::ElementDeclarationTemplate;

    ElementDeclarationPtr network_decl
                          (
                            new ElementDeclarationTemplate<cedar::proc::Network>
                            (
                              "Utilities",
                              "cedar.processing.Network"
                            )
                          );
    network_decl->setIconPath(":/network.svg");
    network_decl->setDescription
    (
      "A grouping element for steps."
    );
    network_decl->declare();

    return true;
  }

  bool declared = declare();
}

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::proc::Network::Network()
:
_mPromotedSlots(new cedar::aux::StringVectorParameter(this, "promotedSlots", std::vector<std::string>()))
{
  cedar::aux::LogSingleton::getInstance()->allocating(this);

  // promoted slots should not appear in user interfaces.
  _mPromotedSlots->setHidden(true);

  QObject::connect(this->_mName.get(), SIGNAL(valueChanged()), this, SLOT(onNameChanged()));
}

cedar::proc::Network::~Network()
{
  cedar::aux::LogSingleton::getInstance()->freeing(this);

  // read out all elements and call this->remove for each element
  this->removeAll();

  mDataConnections.clear();
  mTriggerConnections.clear();
  mElements.clear();
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

std::vector<cedar::proc::ConsistencyIssuePtr> cedar::proc::Network::checkConsistency() const
{
  std::vector<cedar::proc::ConsistencyIssuePtr> issues;

  // generate a list of all looped triggers in the architecture
  //!@todo There should probably be a function for this.
  std::vector<cedar::proc::LoopedTriggerPtr> looped_triggers;
  for (auto iter = this->elements().begin(); iter != this->elements().end(); ++iter)
  {
    cedar::proc::ElementPtr element = iter->second;
    if (cedar::proc::LoopedTriggerPtr trigger = boost::dynamic_pointer_cast<cedar::proc::LoopedTrigger>(element))
    {
      looped_triggers.push_back(trigger);
    }
  }

  // == Check for looped steps that are not connected to looped triggers ===============================================
  for (auto iter = this->elements().begin(); iter != this->elements().end(); ++iter)
  {
    cedar::proc::StepPtr step = boost::dynamic_pointer_cast<cedar::proc::Step>(iter->second);

    if (!step)
    {
      continue;
    }

    if (step->isLooped())
    {
      bool is_triggered = false;
      // check if there is a looped trigger to which this element is connected
      for (size_t i = 0; i < looped_triggers.size(); ++i)
      {
        cedar::proc::LoopedTriggerPtr trigger = looped_triggers[i];
        if (trigger->isListener(step))
        {
          is_triggered = true;
          break;
        }
      }

      if (!is_triggered)
      {
        issues.push_back(boost::make_shared<cedar::proc::LoopedStepNotConnected>(step));
      }
    } // is looped
  }

  return issues;
}

void cedar::proc::Network::startTriggers()
{
  for (auto iter = this->elements().begin(); iter != this->elements().end(); ++iter)
  {
    cedar::proc::ElementPtr element = iter->second;
    if (cedar::proc::LoopedTriggerPtr trigger = boost::dynamic_pointer_cast<cedar::proc::LoopedTrigger>(element))
    {
      if (!trigger->isRunning())
      {
        trigger->startTrigger();
      }
    }
  }
}

void cedar::proc::Network::stopTriggers()
{
  for (auto iter = this->elements().begin(); iter != this->elements().end(); ++iter)
  {
    cedar::proc::ElementPtr element = iter->second;
    if (cedar::proc::LoopedTriggerPtr trigger = boost::dynamic_pointer_cast<cedar::proc::LoopedTrigger>(element))
    {
      if (trigger->isRunning())
      {
        trigger->stopTrigger();
      }
    }
  }
}

void cedar::proc::Network::onNameChanged()
{
  if (cedar::proc::ElementPtr parent_network = this->mRegisteredAt.lock())
  {
    // update the name in the parent network
    boost::static_pointer_cast<cedar::proc::Network>(parent_network)->updateObjectName(this);
  }
}

boost::signals2::connection cedar::proc::Network::connectToElementAdded
                            (
                              boost::function<void (cedar::proc::Network*, cedar::proc::ElementPtr)> slot
                            )
{
  return this->mElementAddedSignal.connect(slot);
}

std::string cedar::proc::Network::getUniqueIdentifier(const std::string& identifier) const
{
  if (!this->nameExists(identifier))
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
  while (this->nameExists(result));

  return result;
}

bool cedar::proc::Network::nameExists(const std::string& name) const
{
  return this->mElements.find(name) != this->mElements.end();
}

void cedar::proc::Network::listSubnetworks(std::set<cedar::proc::ConstNetworkPtr>& subnetworks) const
{
  subnetworks.clear();
  for (ElementMap::const_iterator iter = this->mElements.begin(); iter != this->mElements.end(); ++iter)
  {
    if (cedar::proc::ConstNetworkPtr network = boost::dynamic_pointer_cast<const cedar::proc::Network>(iter->second))
    {
      subnetworks.insert(network);
    }
  }
}

void cedar::proc::Network::reset()
{
  for (ElementMap::iterator iter = this->mElements.begin(); iter != this->mElements.end(); ++iter)
  {
    if (cedar::proc::StepPtr step = boost::dynamic_pointer_cast<cedar::proc::Step>(iter->second))
    {
      step->callReset();
    }
  }
}

const cedar::proc::Network::ElementMap& cedar::proc::Network::elements() const
{
  return this->mElements;
}

void cedar::proc::Network::removeAllConnectionsFromSlot(cedar::proc::ConstDataSlotPtr slot)
{
  for
  (
    cedar::proc::Network::DataConnectionVector::iterator data_con = mDataConnections.begin();
    data_con != mDataConnections.end();
    // empty
  )
  {
    if ((*data_con)->getSource() == slot
        || (*data_con)->getTarget() == slot)
    {
      mDataConnectionChanged
      (
        this->getElement<cedar::proc::Connectable>
        (
          (*data_con)->getSource()->getParent())->getOutputSlot((*data_con)->getSource()->getName()
        ),
        this->getElement<cedar::proc::Connectable>
        (
          (*data_con)->getTarget()->getParent())->getInputSlot((*data_con)->getTarget()->getName()
        ),
        false
      );
      data_con = this->removeDataConnection(data_con);
    }
    else
    {
      ++data_con;
    }
  }
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
    if ((*data_con)->getSource()->isParent(boost::dynamic_pointer_cast<const cedar::proc::Connectable>(element))
        || (*data_con)->getTarget()->isParent(boost::dynamic_pointer_cast<const cedar::proc::Connectable>(element)))
    {
      mDataConnectionChanged
      (
        this->getElement<cedar::proc::Connectable>
        (
          (*data_con)->getSource()->getParent()
        )->getOutputSlot((*data_con)->getSource()->getName() ),
        this->getElement<cedar::proc::Connectable>
        (
          (*data_con)->getTarget()->getParent()
        )->getInputSlot((*data_con)->getTarget()->getName()),
        false
      );
      data_con = this->removeDataConnection(data_con);
    }
    else
    {
      ++data_con;
    }
  }
  // including the ones to promoted slots (this only works if the parent network is accessible)
  if (cedar::proc::NetworkPtr parent = this->mRegisteredAt.lock())
  {
    std::vector<cedar::proc::DataRole::Id> data_roles;
    data_roles.push_back(DataRole::INPUT);
    data_roles.push_back(DataRole::OUTPUT);
    // go through all slot types
    for (unsigned int i = 0; i < data_roles.size(); ++i)
    {
      try
      {
        const SlotMap& current_map = this->getDataSlots(data_roles.at(i));
        for // go through all slots
        (
          SlotMap::const_iterator map = current_map.begin();
          map != current_map.end();
          ++map
        )
        {
          if // check if this is promoted data
          (
            boost::dynamic_pointer_cast<cedar::proc::ConstPromotedExternalData>(map->second)
            || boost::dynamic_pointer_cast<cedar::proc::ConstPromotedOwnedData>(map->second)
          )
          {
            // if so, compare the name of the current element with the slot owner
            std::string element_name;
            std::string rest;
            cedar::aux::splitFirst(map->first, ".", element_name, rest);
            if (element->getName() == element_name) // this slot will be removed later, delete connections now
            {
              // remove all connections at parent network
              parent->removeAllConnectionsFromSlot(map->second);
            }
          }
        }
      }
      catch (cedar::proc::InvalidRoleException& exception)
      {
        continue;
      }
    }
  }

  // then, delete all trigger connections to and from this Element
  for (
        cedar::proc::Network::TriggerConnectionVector::iterator trigger_con = mTriggerConnections.begin();
        trigger_con != mTriggerConnections.end();
        // empty
      )
  {
    if ((*trigger_con)->getSourceTrigger() == boost::dynamic_pointer_cast<const cedar::proc::Trigger>(element)
        || (*trigger_con)->getTarget() == boost::dynamic_pointer_cast<const cedar::proc::Triggerable>(element))
    {
      cedar::proc::TriggerPtr source_trigger;
      try
      {
        source_trigger = this->getElement<cedar::proc::Trigger>((*trigger_con)->getSourceTrigger()->getName());
        mTriggerConnectionChanged
        (
          source_trigger,
          this->getElement<cedar::proc::Triggerable>
          (
            boost::dynamic_pointer_cast<const cedar::proc::Element>((*trigger_con)->getTarget())->getName()
          ),
          false
        );
      }
      catch (cedar::proc::InvalidNameException& exc)
      {
        CEDAR_DEBUG_ASSERT((*trigger_con)->getSourceTrigger()->getName() == "processingDone");
        CEDAR_DEBUG_ASSERT
        (
          this->getElement<cedar::proc::Triggerable>
          (
            boost::dynamic_pointer_cast<const cedar::proc::Element>((*trigger_con)->getTarget())->getName()
          )
        );
      }
      trigger_con = mTriggerConnections.erase(trigger_con);
    }
    else
    {
      ++trigger_con;
    }
  }
  // now, if the removed element is a network, delete all its children
  if (cedar::proc::NetworkPtr network = this->getElement<cedar::proc::Network>(element->getName()))
  {
    network->removeAll();
  }

  cedar::proc::Network::ElementMap::iterator it = mElements.find(element->getName());
  if (it != this->mElements.end())
  {
    mElements.erase(it);
  }
  this->mElementRemovedSignal(element);
  // process promoted slots that may have to be removed
  this->processPromotedSlots();
}

void cedar::proc::Network::add(std::string className, std::string instanceName)
{
  cedar::proc::ElementPtr element = cedar::proc::ElementManagerSingleton::getInstance()->allocate(className);
  this->add(element, instanceName);
}

void cedar::proc::Network::add(cedar::proc::ElementPtr element, std::string instanceName)
{
  element->setName(instanceName);
  this->add(element);
}

void cedar::proc::Network::add(std::list<cedar::proc::ElementPtr> elements)
{
  typedef std::list<cedar::proc::ElementPtr>::iterator iterator;
  typedef std::list<cedar::proc::ElementPtr>::const_iterator const_iterator;
  for (iterator it = elements.begin(); it != elements.end(); )
  {
    try
    {
      this->getElement<cedar::proc::Element>((*it)->getName());
      it = elements.erase(it);
      cedar::aux::LogSingleton::getInstance()->warning
      (
        "An element of name " + (*it)->getName() + " already exists in network " + this->getName() + ".",
        "cedar::proc::Network::addElements(std::list<cedar::proc::ElementPtr> elements)"
      );
    }
    catch(cedar::proc::InvalidNameException& exc)
    {
      ++it;
    }
  }
  // are any elements left?
  if (elements.size() == 0)
  {
    return;
  }
  // check old connections
  //todo use structs here?
  std::vector<std::string> data_from;
  std::vector<std::string> data_to;
  std::vector<cedar::proc::TriggerPtr> trigger_from;
  std::vector<cedar::proc::TriggerablePtr> trigger_to;
  std::vector<cedar::proc::DataSlotPtr> promoted_slots;
  std::vector<std::string> promoted_data_from;
  std::vector<std::string> promoted_data_to;
  cedar::proc::NetworkPtr old_network;
  for
  (
    iterator it = elements.begin(); it != elements.end(); ++it
  )
  {
    // need two parentheses here because otherwise clang throws a warning
    if ((old_network = (*(elements.begin()))->getNetwork()))
    {
      break;
    }
  }
  // remember all data connections between moved elements
  if (old_network)
  {
    for
    (
      cedar::proc::Network::DataConnectionVector::const_iterator it = old_network->getDataConnections().begin();
      it != old_network->getDataConnections().end();
      ++it
    )
    {
      cedar::proc::ElementPtr source = old_network->getElement<cedar::proc::Element>((*it)->getSource()->getParent());
      cedar::proc::ElementPtr target = old_network->getElement<cedar::proc::Element>((*it)->getTarget()->getParent());
      iterator source_it = std::find(elements.begin(), elements.end(), source);
      iterator target_it = std::find(elements.begin(), elements.end(), target);
      if (source_it != elements.end() && target_it != elements.end())
      {
        // this connection must be stored (note that connections are automatically deleted if elements are removed)
        data_from.push_back((*it)->getSource()->getParent() + "." + (*it)->getSource()->getName());
        data_to.push_back((*it)->getTarget()->getParent() + "." + (*it)->getTarget()->getName());
      }
      else if (source_it != elements.end())
      {
        promoted_slots.push_back
        (
          boost::dynamic_pointer_cast<cedar::proc::Connectable>(source)->getOutputSlot((*it)->getSource()->getName())
        );
        promoted_data_from.push_back(this->getName() + "." + (*it)->getSource()->getParent() + "." + (*it)->getSource()->getName());
        promoted_data_to.push_back((*it)->getTarget()->getParent() + "." + (*it)->getTarget()->getName());
      }
      else if (target_it != elements.end())
      {
        promoted_slots.push_back
        (
          boost::dynamic_pointer_cast<cedar::proc::Connectable>(target)->getInputSlot((*it)->getTarget()->getName())
        );
        promoted_data_from.push_back((*it)->getSource()->getParent() + "." + (*it)->getSource()->getName());
        promoted_data_to.push_back(this->getName() + "." + (*it)->getTarget()->getParent() + "." + (*it)->getTarget()->getName());
      }
    }
  }
  // remember all trigger connections between moved elements
  for
  (
    iterator it = elements.begin(); it != elements.end(); ++it
  )
  {
    if (cedar::proc::TriggerPtr source_trigger = boost::dynamic_pointer_cast<cedar::proc::Trigger>(*it))
    {
      for
      (
        iterator target = elements.begin(); target != elements.end(); ++target
      )
      {
        cedar::proc::TriggerablePtr target_triggerable = boost::dynamic_pointer_cast<cedar::proc::Triggerable>(*target);
        if (target_triggerable && source_trigger->isListener(target_triggerable))
        {
          trigger_from.push_back(source_trigger);
          trigger_to.push_back(target_triggerable);
        }
      }
    }
  }

  // now add each element to the new network
  for (iterator it = elements.begin(); it != elements.end(); ++it)
  {
    this->add(*it);
  }

  for (unsigned int i = 0; i < promoted_slots.size(); ++i)
  {
    try
    {
      this->promoteSlot(promoted_slots.at(i));
    }
    catch (const cedar::proc::DuplicateNameException& exc)
    {
      // nothing to to here, promoted slot already exists
    }
  }

  // restore data connections
  for (unsigned int i = 0; i < data_from.size(); ++i)
  {
    this->connectSlots(data_from.at(i), data_to.at(i));
  }
  // restore promoted data connections
  for (unsigned int i = 0; i < promoted_data_from.size(); ++i)
  {
    old_network->connectSlots(promoted_data_from.at(i), promoted_data_to.at(i));
  }
  // restore trigger connections
  for (unsigned int i = 0; i < trigger_from.size(); ++i)
  {
    this->connectTrigger(trigger_from.at(i), trigger_to.at(i));
  }
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
    CEDAR_THROW
    (
      cedar::proc::DuplicateNameException,
      "Duplicate element name entry \"" + instanceName + "\" in network \"" + this->getName() + "\""
    )
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
  element->setNetwork(boost::static_pointer_cast<cedar::proc::Network>(this->shared_from_this()));

  this->mNewElementAddedSignal(element);
  this->mElementAddedSignal(this, element);
}

void cedar::proc::Network::duplicate(const std::string& elementName, const std::string& newName)
{
  try
  {
    // determine class
    cedar::proc::ElementPtr elem = this->getElement(elementName);
    std::string class_name = cedar::proc::ElementManagerSingleton::getInstance()->getTypeId(elem);
    // allocate object
    cedar::proc::ElementPtr new_elem = cedar::proc::ElementManagerSingleton::getInstance()->allocate(class_name);
    // copy configuration tree
    new_elem->copyFrom(elem);
    // get unique name
    std::string modified_name;
    if (!newName.empty()) // desired name given
    {
      modified_name = this->getUniqueName(newName);
    }
    else // default name
    {
      modified_name = this->getUniqueName(elementName);
    }
    // set unique name
    new_elem->setName(modified_name);
    // add to network
    this->add(new_elem);
  }
  catch (cedar::proc::InvalidNameException& exc)
  {
    CEDAR_THROW
    (
      cedar::proc::InvalidNameException,
      "cannot duplicate element of name " + elementName + ", it does not exist in network" + this->getName()
    )
  }
}

std::string cedar::proc::Network::getUniqueName(const std::string& unmodifiedName) const
{
  std::string adjusted_name;
  try
  {
    unsigned int new_id = 1;
    adjusted_name = unmodifiedName;
    while (this->getElement(adjusted_name))
    {
      std::stringstream str;
      str << unmodifiedName << " " << new_id;
      adjusted_name = str.str();
      ++new_id;
    }
  }
  catch(cedar::proc::InvalidNameException& exc)
  {
    // nothing to do here, name not duplicate, use this as a name
  }
  return adjusted_name;
}

cedar::proc::ConstElementPtr cedar::proc::Network::getElement(const std::string& name) const
{
  ElementMap::const_iterator iter;
  std::string first;
  std::string rest;
  cedar::aux::splitFirst(name, ".", first, rest);
  if (first.length() != 0 && rest.length() != 0)
  {
    cedar::proc::ConstElementPtr element = this->getElement(first);
    cedar::proc::ConstNetworkPtr network = boost::dynamic_pointer_cast<cedar::proc::ConstNetwork>(element);

    if (!network)
    {
      CEDAR_THROW(cedar::proc::InvalidNameException, "The given name does not specify a proper path in this network.");
    }

    return network->getElement(rest);
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
    CEDAR_THROW
    (
      cedar::proc::InvalidNameException, "No element of the name \"" + name
        + "\" was found in the network \"" + this->getName() + "\"."
    );
  }
}

cedar::proc::ElementPtr cedar::proc::Network::getElement(const std::string& name)
{
  return boost::const_pointer_cast<Element>(static_cast<const Network*>(this)->getElement(name));
}

void cedar::proc::Network::connectSlots(const std::string& source, const std::string& target)
{
  //!@todo Shouldn't large parts of this code be in DataConnection?
  // parse element and slot name
  std::string source_name;
  std::string source_slot_name;
  std::string target_name;
  std::string target_slot_name;
  cedar::aux::splitFirst(source, ".", source_name, source_slot_name);
  cedar::aux::splitFirst(target, ".", target_name, target_slot_name);
  // check connection
  if (this->isConnected(source, target))
  {
    CEDAR_THROW(cedar::proc::DuplicateConnectionException, "This connection already exists!")
  }
  std::string real_source_name;
  std::string real_source_slot;
  cedar::proc::Connectable::parseDataNameNoRole(source, real_source_name, real_source_slot);

  std::string real_target_name;
  std::string real_target_slot;
  cedar::proc::Connectable::parseDataNameNoRole(target, real_target_name, real_target_slot);
  cedar::proc::TriggerablePtr p_source = this->getElement<cedar::proc::Triggerable>(real_source_name);
  cedar::proc::TriggerablePtr p_target = this->getElement<cedar::proc::Triggerable>(real_target_name);

  cedar::proc::OwnedDataPtr source_slot
    = this->getElement<cedar::proc::Connectable>(source_name)->getOutputSlot(source_slot_name);

  cedar::proc::DataSlotPtr target_slot
    = this->getElement<cedar::proc::Connectable>(target_name)->getInputSlot(target_slot_name);

  cedar::proc::ConnectablePtr target_connectable = boost::dynamic_pointer_cast<cedar::proc::Connectable>(p_target);
  CEDAR_DEBUG_ASSERT(target_connectable);

  if (target_connectable->ownsDataOf(source_slot))
  {
    CEDAR_THROW(cedar::proc::DeadlockException, "This connection would lead to a deadlock.");
  }

  // create connection
  mDataConnections.push_back
  (
    cedar::proc::DataConnectionPtr
    (
      new DataConnection
      (
        source_slot,
        target_slot
      )
    )
  );
  CEDAR_DEBUG_ASSERT(p_target);
  if (!p_target->isLooped())
  {
    try
    {
      this->connectTrigger(p_source->getFinishedTrigger(), p_target);
    }
    catch(const cedar::proc::DuplicateConnectionException&)
    {
      // if the triggers are already connected, that's ok.
    }

    // trigger the connected target once, establishing a validity of the target
    p_target->onTrigger();
  }
  // inform any interested listeners of this new connection
  mDataConnectionChanged
  (
    this->getElement<cedar::proc::Connectable>(source_name)->getOutputSlot(source_slot_name),
    this->getElement<cedar::proc::Connectable>(target_name)->getInputSlot(target_slot_name),
    true
  );
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
  mTriggerConnectionChanged
  (
    source,
    target,
    true
  );
}

void cedar::proc::Network::disconnectOutputSlot(cedar::proc::ConnectablePtr connectable, const std::string& slot)
{
  std::vector<cedar::proc::DataConnectionPtr> connections;
  this->getDataConnections(connectable, slot, connections);

  for (size_t i = 0; i < connections.size(); ++i)
  {
    cedar::proc::DataConnectionPtr connection = connections[i];
    this->disconnectSlots(connection->getSource(), connection->getTarget());
  }
}

void cedar::proc::Network::disconnectSlots(const std::string& source, const std::string& target)
{
  // parse element and slot name
  std::string source_name;
  std::string source_slot_name;
  std::string target_name;
  std::string target_slot_name;
  cedar::aux::splitFirst(source, ".", source_name, source_slot_name);
  cedar::aux::splitFirst(target, ".", target_name, target_slot_name);

  cedar::proc::ConnectablePtr source_connectable = this->getElement<cedar::proc::Connectable>(source_name);
  cedar::proc::ConnectablePtr target_connectable = this->getElement<cedar::proc::Connectable>(target_name);

  cedar::proc::DataSlotPtr source_slot = source_connectable->getOutputSlot(source_slot_name);
  cedar::proc::DataSlotPtr target_slot = target_connectable->getInputSlot(target_slot_name);

  this->disconnectSlots(source_slot, target_slot);
}

void cedar::proc::Network::disconnectSlots
                           (
                             cedar::proc::ConstDataSlotPtr sourceSlot,
                             cedar::proc::ConstDataSlotPtr targetSlot
                           )
{
  for (DataConnectionVector::iterator it = mDataConnections.begin(); it != mDataConnections.end(); ++it)
  {
    if ((*it)->equals(sourceSlot, targetSlot))
    {
      this->removeDataConnection(it);
      // inform any interested listeners of this new connection
      mDataConnectionChanged(sourceSlot, targetSlot, false);
      return;
    }
  }

  CEDAR_THROW
  (
    cedar::proc::MissingConnectionException,
    "The data connection between in network " + this->getName() + " does not exist!"
  );
}

void cedar::proc::Network::disconnectTrigger(cedar::proc::TriggerPtr source, cedar::proc::TriggerablePtr target)
{
  for (TriggerConnectionVector::iterator it = mTriggerConnections.begin(); it != mTriggerConnections.end(); ++it)
  {
    if ((*it)->equals(source, target))
    {
      mTriggerConnections.erase(it);
      mTriggerConnectionChanged
      (
        source,
        target,
        false
      );
      return;
    }
  }
  CEDAR_THROW
  (
    cedar::proc::MissingConnectionException,
    "Network \"" + this->getName() + "\": This trigger connection does not exist!"
  );
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

  this->cedar::aux::Configurable::writeConfiguration(root);
}

void cedar::proc::Network::writeMetaData(cedar::aux::ConfigurationNode& meta)
{
  meta.put("format", 1);
}

void cedar::proc::Network::readFrom(const cedar::aux::ConfigurationNode& root)
{
  std::vector<std::string> exceptions;
  this->readFrom(root, exceptions);

  if (!exceptions.empty())
  {
    cedar::proc::ArchitectureLoadingException exception(exceptions);
    CEDAR_THROW_EXCEPTION(exception);
  }
}

void cedar::proc::Network::readFrom(const cedar::aux::ConfigurationNode& root, std::vector<std::string>& exceptions)
{
  unsigned int format_version = 1; // default value is the current format
  try
  {
    const cedar::aux::ConfigurationNode& meta = root.get_child("meta");
    format_version = meta.get<unsigned int>("format");
  }
  catch (const boost::property_tree::ptree_bad_path&)
  {
    cedar::aux::LogSingleton::getInstance()->warning
    (
      "Could not recognize network format: format or meta node missing. Defaulting to current version.",
      "network reading",
      "cedar::proc::Network::readFrom(const cedar::aux::ConfigurationNode&)"
    );
  }
  // store latest
  if (root.find("ui") != root.not_found())
  {
    mLastReadUINode = root.find("ui")->second;
  }
  switch (format_version)
  {
    default:
      cedar::aux::LogSingleton::getInstance()->warning
      (
        "Could not recognize format: Unknown format version "
           + cedar::aux::toString(format_version)
           + ". Defaulting to current version.",
        "network reading",
        "cedar::proc::Network::readFrom(const cedar::aux::ConfigurationNode&)"
      );
    case 1:
      this->readFromV1(root, exceptions);
      break;
  }
}

void cedar::proc::Network::readFromV1
     (
       const cedar::aux::ConfigurationNode& root,
       std::vector<std::string>& exceptions
     )
{
  this->cedar::aux::Configurable::readConfiguration(root);

  try
  {
    const cedar::aux::ConfigurationNode& steps = root.get_child("steps");
    this->readSteps(steps, exceptions);
  }
  catch (const boost::property_tree::ptree_bad_path&)
  {
    // no steps declared -- this is ok.
#ifdef DEBUG_FILE_READING
    cedar::aux::LogSingleton::getInstance()->debugMessage
    (
      "No steps present while reading configuration.",
      "cedar::proc::Network::readFromV1(const cedar::aux::ConfigurationNode&)"
    );
#endif // DEBUG_FILE_READING
  }

  try
  {
    const cedar::aux::ConfigurationNode& networks = root.get_child("networks");
    this->readNetworks(networks, exceptions);
  }
  catch (const boost::property_tree::ptree_bad_path&)
  {
    // no networks declared -- this is ok.
#ifdef DEBUG_FILE_READING
    cedar::aux::LogSingleton::getInstance()->debugMessage
    (
      "No networks present while reading configuration.",
      "cedar::proc::Network::readFromV1(const cedar::aux::ConfigurationNode&)"
    );
#endif // DEBUG_FILE_READING
  }
  // post-process networks (load promoted slots)
  for (ElementMap::iterator iter = this->mElements.begin(); iter != this->mElements.end(); ++iter)
  {
    if (cedar::proc::NetworkPtr network = boost::dynamic_pointer_cast<cedar::proc::Network>(iter->second))
    {
      network->processPromotedSlots();
    }
  }

  try
  {
    const cedar::aux::ConfigurationNode& connections = root.get_child("connections");
    this->readDataConnections(connections, exceptions);
  }
  catch (const boost::property_tree::ptree_bad_path&)
  {
    // no connections declared -- this is ok.
#ifdef DEBUG_FILE_READING
    cedar::aux::LogSingleton::getInstance()->debugMessage
    (
      "No data connections present while reading configuration.",
      "cedar::proc::Network::readFromV1(const cedar::aux::ConfigurationNode&)"
    );
#endif // DEBUG_FILE_READING
  }

  try
  {
    const cedar::aux::ConfigurationNode& triggers = root.get_child("triggers");
    this->readTriggers(triggers, exceptions);
  }
  catch (const boost::property_tree::ptree_bad_path&)
  {
#if defined DEBUG || defined DEBUG_FILE_READING
    cedar::aux::LogSingleton::getInstance()->debugMessage
    (
      "No triggers present while reading configuration.",
      "cedar::proc::Network::readFromV1(const cedar::aux::ConfigurationNode&)"
    );
#endif // defined DEBUG || defined DEBUG_FILE_READING
  }
}

void cedar::proc::Network::writeSteps(cedar::aux::ConfigurationNode& steps)
{
  for (ElementMap::iterator iter = this->mElements.begin(); iter != this->mElements.end(); ++iter)
  {
    // if this is a step, write this to the configuration tree
    if (cedar::proc::StepPtr step = boost::dynamic_pointer_cast<cedar::proc::Step>(iter->second))
    {
#ifdef DEBUG_FILE_WRITING
      std::cout << "Saving " << iter->first << "." << std::endl;
#endif
      std::string class_name = cedar::proc::ElementManagerSingleton::getInstance()->getTypeId(step);
      cedar::aux::ConfigurationNode step_node;
      step->writeConfiguration(step_node);
      steps.push_back(cedar::aux::ConfigurationNode::value_type(class_name, step_node));
    }
  }
}

void cedar::proc::Network::readSteps
     (
       const cedar::aux::ConfigurationNode& root,
       std::vector<std::string>& exceptions
     )
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

    cedar::proc::ElementPtr step;
    try
    {
      step = cedar::proc::ElementManagerSingleton::getInstance()->allocate(class_id);
    }
    catch (cedar::aux::ExceptionBase& e)
    {
      exceptions.push_back(e.exceptionInfo());
    }

    if (step)
    {
      try
      {
        step->readConfiguration(step_node);
      }
      catch (cedar::aux::ExceptionBase& e)
      {
        exceptions.push_back(e.exceptionInfo());
      }

      try
      {
        this->add(step);
      }
      catch (cedar::aux::ExceptionBase& e)
      {
        exceptions.push_back(e.exceptionInfo());
      }

      step->resetChangedStates(false);
    }
  }
}

void cedar::proc::Network::writeTriggers(cedar::aux::ConfigurationNode& triggers)
{
  for (ElementMap::iterator iter = this->mElements.begin(); iter != this->mElements.end(); ++iter)
  {
    // if this is a trigger, write this to the configuration tree
    if (cedar::proc::TriggerPtr trigger = boost::dynamic_pointer_cast<cedar::proc::Trigger>(iter->second))
    {
#ifdef DEBUG_FILE_WRITING
      std::cout << "Saving " << iter->first << "." << std::endl;
#endif
      std::string class_name = cedar::proc::ElementManagerSingleton::getInstance()->getTypeId(trigger);
      cedar::aux::ConfigurationNode trigger_node;
      trigger->writeConfiguration(trigger_node);
      triggers.push_back(cedar::aux::ConfigurationNode::value_type(class_name, trigger_node));
    }
  }
}

void cedar::proc::Network::readTriggers
     (
       const cedar::aux::ConfigurationNode& root,
       std::vector<std::string>& exceptions
     )
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

    cedar::proc::TriggerPtr trigger;
    try
    {
      trigger = boost::dynamic_pointer_cast<cedar::proc::Trigger>
                (
                  cedar::proc::ElementManagerSingleton::getInstance()->allocate(class_id)
                );
    }
    catch (cedar::aux::ExceptionBase& e)
    {
      exceptions.push_back(e.exceptionInfo());
      continue;
    }

    try
    {
      trigger->readConfiguration(trigger_node);
    }
    catch (cedar::aux::ExceptionBase& e)
    {
      exceptions.push_back(e.exceptionInfo());
    }

    try
    {
      this->add(trigger);
    }
    catch (cedar::aux::ExceptionBase& e)
    {
      exceptions.push_back(e.exceptionInfo());
    }

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

        cedar::proc::TriggerablePtr triggerable;
        try
        {
          triggerable = this->getElement<Triggerable>(listener_name);
        }
        catch (cedar::aux::ExceptionBase& e)
        {
          exceptions.push_back(e.exceptionInfo());
        }

        if (!triggerable)
        {
          continue; // target not found, cannot recover
        }

        try
        {
          this->connectTrigger(trigger, triggerable);
        }
        catch (cedar::aux::ExceptionBase& e)
        {
          exceptions.push_back(e.exceptionInfo());
        }
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
    if (cedar::proc::NetworkPtr network = boost::dynamic_pointer_cast<cedar::proc::Network>(iter->second))
    {
#ifdef DEBUG_FILE_WRITING
      std::cout << "Saving " << iter->first << "." << std::endl;
#endif
      cedar::aux::ConfigurationNode network_node;
      network->writeTo(network_node);
      networks.push_back(cedar::aux::ConfigurationNode::value_type(iter->first, network_node));
    }
  }
}

void cedar::proc::Network::readNetworks
     (
       const cedar::aux::ConfigurationNode& root,
       std::vector<std::string>& exceptions
     )
{
#ifdef DEBUG_FILE_READING
  std::cout << "Reading networks." << std::endl;
#endif // DEBUG_FILE_READING

  for (cedar::aux::ConfigurationNode::const_iterator iter = root.begin();
      iter != root.end();
      ++iter)
  {
    const std::string& network_name = iter->first;
    const cedar::aux::ConfigurationNode& network_node = iter->second;

#ifdef DEBUG_FILE_READING
    std::cout << "Reading network named " << network_name << std::endl;
#endif // DEBUG_FILE_READING

    cedar::proc::NetworkPtr network;

    try
    {
      network
        = boost::dynamic_pointer_cast<cedar::proc::Network>
          (
            cedar::proc::ElementManagerSingleton::getInstance()->allocate("cedar.processing.Network")
          );
    }
    catch (cedar::aux::ExceptionBase& e)
    {
      exceptions.push_back(e.exceptionInfo());
      continue;
    }

    try
    {
      network->setName(network_name);
    }
    catch (cedar::aux::ExceptionBase& e)
    {
      exceptions.push_back(e.exceptionInfo());
    }

    try
    {
      this->add(network);
    }
    catch (cedar::aux::ExceptionBase& e)
    {
      exceptions.push_back(e.exceptionInfo());
    }

    network->readFrom(network_node, exceptions);
    // is this enough to recursively read in the network?
    network->resetChangedStates(false);
  }
}

void cedar::proc::Network::writeDataConnection
     (
       cedar::aux::ConfigurationNode& root,
       const cedar::proc::DataConnectionPtr connection
     )
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

void cedar::proc::Network::readDataConnections
     (
       const cedar::aux::ConfigurationNode& root,
       std::vector<std::string>& exceptions
     )
{
#ifdef DEBUG_FILE_READING
  std::cout << "Reading data connections." << std::endl;
#endif // DEBUG_FILE_READING

  for (cedar::aux::ConfigurationNode::const_iterator iter = root.begin();
      iter != root.end();
      ++iter)
  {
    try
    {
      this->readDataConnection(iter->second);
    }
    catch (cedar::aux::ExceptionBase& e)
    {
      exceptions.push_back(e.exceptionInfo());
    }
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
  //!@todo check if source is in any way part of this network (may be a done trigger of a step)
  //!@todo reactivate this check
//  CEDAR_DEBUG_ASSERT
//  (
//    target
//      == this->getElement<cedar::proc::Triggerable>(boost::dynamic_pointer_cast<cedar::proc::Element>(target)->getName())
//  );
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
  std::string old_name = old_iter->first;
  // if new and old name are the same, do nothing
  if (element->getName() == old_name)
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

  // check all promoted items
  bool slots_changed = false;
  for (unsigned int index = 0; index < _mPromotedSlots->size(); ++index)
  {
    std::string name;
    std::string rest;
    cedar::aux::splitFirst(_mPromotedSlots->at(index), ".", name, rest);
    if (name == old_name)
    {
      std::string full_slot_name;
      std::string type;
      cedar::aux::splitLast(_mPromotedSlots->at(index), ".", full_slot_name, type);
      std::string step_name;
      std::string slot_name;
      cedar::aux::splitLast(full_slot_name, ".", step_name, slot_name);
      std::string new_slot_name = object->getName() + "." + slot_name;
      if (type == "Input")
      {
        this->renameInput(full_slot_name, new_slot_name);
      }
      else if (type == "Output")
      {
        this->renameOutput(full_slot_name, new_slot_name);
      }
      else
      {
        CEDAR_ASSERT(false);
      }
      std::string updated_name = object->getName() + "." + rest;
      _mPromotedSlots->set(index, updated_name);
      slots_changed = true;
    }
  }
  if (slots_changed)
  {
    this->mSlotChanged();
  }
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
  //!@todo This code needs to be cleaned up, simplified and commented
  std::string source_name = (*it)->getSource()->getParent();
  std::string target_name = (*it)->getTarget()->getParent();
  std::string real_source_name = source_name;
  if (cedar::proc::ConstPromotedExternalDataPtr ext = boost::dynamic_pointer_cast<const cedar::proc::PromotedExternalData>((*it)->getSource()))
  {
    real_source_name = ext->getPromotionPath();
  }
  else if (cedar::proc::ConstPromotedOwnedDataPtr owned = boost::dynamic_pointer_cast<const cedar::proc::PromotedOwnedData>((*it)->getSource()))
  {
    real_source_name = owned->getPromotionPath();
  }

  std::string real_target_name;
  if (cedar::proc::ConstPromotedExternalDataPtr ext = boost::dynamic_pointer_cast<const cedar::proc::PromotedExternalData>((*it)->getTarget()))
  {
    real_target_name = ext->getPromotionPath();
  }
  else if (cedar::proc::ConstPromotedOwnedDataPtr owned = boost::dynamic_pointer_cast<const cedar::proc::PromotedOwnedData>((*it)->getTarget()))
  {
    real_target_name = owned->getPromotionPath();
  }

  if (real_source_name != "") // the source is a nested element
  {
    source_name = real_source_name;
  }
  if (real_target_name != "") // the target is a nested element
  {
    target_name = real_target_name;
  }

  cedar::proc::TriggerablePtr triggerable_target = this->getElement<cedar::proc::Triggerable>(target_name);
  CEDAR_DEBUG_ASSERT(triggerable_target);
  if (!triggerable_target->isLooped())
  {
    target_name = (*it)->getTarget()->getParent(); // reset target_name
    // check that both Connectables are not connected through some other DataSlots
    cedar::proc::ConnectablePtr target_connectable = this->getElement<cedar::proc::Connectable>(target_name);
    for (DataConnectionVector::iterator iter = mDataConnections.begin(); iter != mDataConnections.end(); ++iter)
    {
      if
      (
        // check if this is the connection being erased
        iter != it &&
        // if it isn't, check if it connects to the ssame target
        (*iter)->connects
        (
          this->getElement<cedar::proc::Connectable>(source_name),
          target_connectable
        )
      )
      {
        it = mDataConnections.erase(it);

        // recheck if the inputs of the target are still valid
        triggerable_target->onTrigger();

        // found another connection between those two Connectables, do not delete done trigger and return
        return it;
      }
    }
    // found no other connection, delete the TriggerConnection first to avoid trigger calls with unconnected input
    if (!real_target_name.empty()) // the target is a nested element
    {
      target_name = real_target_name;
    }
    this->disconnectTrigger(
                             this->getElement<cedar::proc::Triggerable>(real_source_name)->getFinishedTrigger(),
                             this->getElement<cedar::proc::Triggerable>(target_name)
                           );

    // then, erase the connection
    it = mDataConnections.erase(it);

    // recheck if the inputs of the target are still valid
    triggerable_target->onTrigger();
  }
  else
  {
    it = mDataConnections.erase(it);
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
    if (cedar::proc::ConstNetworkPtr network = boost::dynamic_pointer_cast<cedar::proc::Network>(iter->second))
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
  std::string enum_name = cedar::proc::DataRole::type().get(promotedSlot->getRole()).prettyString();
  std::string promoted_name = promotedSlot->getParent() + "." + promotedSlot->getName() + "." + enum_name;
  if (std::find(_mPromotedSlots->begin(), _mPromotedSlots->end(), promoted_name) == _mPromotedSlots->end())
  {
    _mPromotedSlots->pushBack
    (
      promoted_name
    );
  }
  this->mSlotChanged();
}

void cedar::proc::Network::demoteSlot(cedar::proc::DataRole::Id role, const std::string& name)
{
  this->removePromotedData(role, name);
  std::string enum_name = cedar::proc::DataRole::type().get(role).prettyString();
  std::string promoted_name = name + "." + enum_name;
  _mPromotedSlots->eraseAll(promoted_name);
  std::string connectable;
  std::string child_slot_name;
  cedar::aux::splitFirst(name, ".", connectable, child_slot_name);
  try
  {
    this->getElement<cedar::proc::Connectable>(connectable)->getSlot(role, child_slot_name)->demote();
  }
  catch (cedar::aux::ExceptionBase& exc) // check that element does not exist in network
  {
    if (typeid(exc) != typeid(cedar::proc::InvalidNameException))
    {
      throw exc;
    }
  }
  this->mSlotChanged();
}

boost::signals2::connection cedar::proc::Network::connectToSlotChangedSignal(boost::function<void ()> slot)
{
  return mSlotChanged.connect(slot);
}

boost::signals2::connection cedar::proc::Network::connectToTriggerConnectionChanged
                            (
                              boost::function<void (cedar::proc::TriggerPtr, cedar::proc::TriggerablePtr, bool)> slot
                            )
{
  return mTriggerConnectionChanged.connect(slot);
}

boost::signals2::connection cedar::proc::Network::connectToDataConnectionChanged
                            (
                              boost::function<void (cedar::proc::ConstDataSlotPtr, cedar::proc::ConstDataSlotPtr, bool)> slot
                            )
{
  return mDataConnectionChanged.connect(slot);
}

boost::signals2::connection cedar::proc::Network::connectToNewElementAddedSignal
                            (
                              boost::function<void (cedar::proc::ElementPtr)> slot
                            )
{
  return mNewElementAddedSignal.connect(slot);
}

boost::signals2::connection cedar::proc::Network::connectToElementRemovedSignal
                            (
                              boost::function<void (cedar::proc::ConstElementPtr)> slot
                            )
{
  return mElementRemovedSignal.connect(slot);
}

void cedar::proc::Network::processPromotedSlots()
{
  std::vector<cedar::aux::Enum> types_delete_later;
  std::vector<std::string> slots_delete_later;
  for
  (
    cedar::aux::StringVectorParameter::const_iterator iter = _mPromotedSlots->begin();
    iter != _mPromotedSlots->end();
    ++iter
  )
  {
    // check first, if this slot already exists
    std::string full_name = *iter;
    std::string slot_name; // the name of the slot, including step and network prefixes
    std::string slot_role; // input, output, buffer
    cedar::aux::splitLast(full_name, ".", slot_name, slot_role);
    try // is this already a slot of this network?
    {
      this->getSlot(cedar::proc::DataRole::type().getFromPrettyString(slot_role), slot_name);
      std::string child;
      std::string rest;
      cedar::aux::splitFirst(slot_name, ".", child, rest);
      try
      {
        this->getElement<cedar::proc::Element>(child);
      }
      catch (cedar::aux::ExceptionBase& exc) // remove promoted slot
      {
        if (typeid(exc) != typeid(cedar::proc::InvalidNameException))
        {
          throw exc;
        }
        types_delete_later.push_back(cedar::proc::DataRole::type().getFromPrettyString(slot_role));
        slots_delete_later.push_back(slot_name);
      }
    }
    catch (cedar::aux::ExceptionBase& exc) // it's not, create a new promoted slot
    {
      if
      (
        typeid(exc) != typeid(cedar::proc::InvalidNameException)
          && typeid(exc) != typeid(cedar::proc::InvalidRoleException)
      )
      {
        throw exc;
      }
      std::string child;
      std::string rest;
      cedar::aux::splitFirst(slot_name, ".", child, rest);
      if (cedar::proc::NetworkPtr network = this->getElement<cedar::proc::Network>(child))
      {
        network->processPromotedSlots();
        //!@todo not tested yet! please check if the following line works
        this->promoteSlot(network->getSlot(cedar::proc::DataRole::type().getFromPrettyString(slot_role), slot_name));
      }
      else if (cedar::proc::StepPtr step = this->getElement<cedar::proc::Step>(child))
      {
        this->promoteSlot(step->getSlot(cedar::proc::DataRole::type().getFromPrettyString(slot_role), rest));
      }
    }
  }
  for (unsigned int i = 0; i < types_delete_later.size(); ++i)
  {
    this->demoteSlot(types_delete_later.at(i), slots_delete_later.at(i));
  }
}

const cedar::proc::Network::DataConnectionVector& cedar::proc::Network::getDataConnections() const
{
  return this->mDataConnections;
}

void cedar::proc::Network::removeAll()
{
  // read out all elements and call this->remove for each element
  std::vector<cedar::proc::ElementPtr> elements;
  for (ElementMapIterator it = mElements.begin(); it != mElements.end(); ++it)
  {
    elements.push_back(it->second);
  }
  for (unsigned int i = 0; i < elements.size(); ++i)
  {
    this->remove(elements.at(i));
  }
}
