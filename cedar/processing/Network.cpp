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
#include "cedar/processing/ElementDeclaration.h"
#include "cedar/processing/DeclarationRegistry.h"
#include "cedar/processing/exceptions.h"
#include "cedar/processing/LoopedTrigger.h"
#include "cedar/processing/sinks/GroupSink.h"
#include "cedar/processing/sources/GroupSource.h"
#include "cedar/auxiliaries/StringVectorParameter.h"
#include "cedar/auxiliaries/Parameter.h"
#include "cedar/auxiliaries/Log.h"
#include "cedar/auxiliaries/Data.h"
#include "cedar/auxiliaries/sleepFunctions.h"
#include "cedar/auxiliaries/Log.h"
#include "cedar/auxiliaries/assert.h"
#include "cedar/auxiliaries/Recorder.h"
#include "cedar/units/prefixes.h"

#include "cedar/processing/consistency/LoopedStepNotConnected.h"

// SYSTEM INCLUDES
#include <boost/make_shared.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <algorithm>

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
Triggerable(false),
_mConnectors(new ConnectorMapParameter(this, "connectors", ConnectorMap()))
{
  cedar::aux::LogSingleton::getInstance()->allocating(this);
  _mConnectors->setHidden(true);
  QObject::connect(this->_mName.get(), SIGNAL(valueChanged()), this, SLOT(onNameChanged()));
}

cedar::proc::Network::~Network()
{
  cedar::aux::LogSingleton::getInstance()->freeing(this);

  // stop all triggers.
  this->stopTriggers();

  // read out all elements and call this->remove for each element
  this->removeAll();

  mDataConnections.clear();
  mTriggerConnections.clear();
  mElements.clear();
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

std::string cedar::proc::Network::getNewConnectorName(bool inputConnector) const
{
  std::string base = "input";
  if (!inputConnector)
  {
    base = "output";
  }

  if (!this->hasConnector(base))
  {
    return base;
  }

  unsigned int ctr = 2;
  while (this->hasConnector(base + " " + cedar::aux::toString(ctr)))
  {
    ++ctr;
  }

  return base + " " + cedar::aux::toString(ctr);
}

//!@todo This should be done in a separate class.
std::vector<cedar::proc::ConsistencyIssuePtr> cedar::proc::Network::checkConsistency() const
{
  std::vector<cedar::proc::ConsistencyIssuePtr> issues;
  std::vector<cedar::proc::LoopedTriggerPtr> looped_triggers = listLoopedTriggers();

  // Check for looped steps that are not connected to looped triggers
  for (auto iter = this->getElements().begin(); iter != this->getElements().end(); ++iter)
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
    } // end is looped
  }

  return issues;
}

std::vector<cedar::proc::LoopedTriggerPtr> cedar::proc::Network::listLoopedTriggers() const
{
  std::vector<cedar::proc::LoopedTriggerPtr> triggers;

  for (auto iter = this->getElements().begin(); iter != this->getElements().end(); ++iter)
  {
    cedar::proc::ElementPtr element = iter->second;
    if (cedar::proc::LoopedTriggerPtr trigger = boost::dynamic_pointer_cast<cedar::proc::LoopedTrigger>(element))
    {
      triggers.push_back(trigger);
    }
  }

  return triggers;
}

void cedar::proc::Network::startTriggers(bool wait)
{
  std::vector<cedar::proc::LoopedTriggerPtr> triggers = this->listLoopedTriggers();

  for (auto iter = triggers.begin(); iter != triggers.end(); ++iter)
  {
    auto trigger = *iter;
    if (!trigger->isRunning())
    {
      trigger->start();
    }
  }

  if (wait)
  {
    for (auto iter = triggers.begin(); iter != triggers.end(); ++iter)
    {
      auto trigger = *iter;
      while (!trigger->isRunning())
      {
        cedar::aux::sleep(0.005 * cedar::unit::seconds);
      }
    }
  }
}

void cedar::proc::Network::stopTriggers(bool wait)
{
  std::vector<cedar::proc::LoopedTriggerPtr> triggers = this->listLoopedTriggers();

  for (auto iter = triggers.begin(); iter != triggers.end(); ++iter)
  {
    auto trigger = *iter;
    if (trigger->isRunning())
    {
      trigger->stop();
    }
  }

  if (wait)
  {
    for (auto iter = triggers.begin(); iter != triggers.end(); ++iter)
    {
      auto trigger = *iter;
      while (trigger->isRunning())
      {
        cedar::aux::sleep(0.005 * cedar::unit::seconds);
      }
    }
  }
}

void cedar::proc::Network::stepTriggers()
{
  std::vector<cedar::proc::LoopedTriggerPtr> triggers = this->listLoopedTriggers();
  double time_step = std::numeric_limits<double>::max();
  // find the shortest time step of all triggers
  for (auto iter = triggers.begin(); iter != triggers.end(); ++iter)
  {
    auto trigger = *iter;
    if (trigger->getSimulatedTimeParameter() < time_step)
    {
      time_step = trigger->getSimulatedTimeParameter();
    }
  }

  this->stepTriggers(time_step);
}

void cedar::proc::Network::stepTriggers(cedar::unit::Time stepTime)
{
  double time_milli = stepTime / cedar::unit::seconds * 1000;
  this->stepTriggers(time_milli);
}

void cedar::proc::Network::stepTriggers(double timeStep)
{
  std::vector<cedar::proc::LoopedTriggerPtr> triggers = this->listLoopedTriggers();
  // step all triggers with this time step
  for (auto iter = triggers.begin(); iter != triggers.end(); ++iter)
  {
    auto trigger = *iter;
    if (!trigger->isRunning())
    {
      trigger->step(timeStep);
    }
  }
}

void cedar::proc::Network::onNameChanged()
{
  if (auto parent_network = this->getNetwork())
  {
    // update the name in the parent network
    parent_network->updateObjectName(this);
  }
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

const cedar::proc::Network::ElementMap& cedar::proc::Network::getElements() const
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
        cedar::proc::Network::CONNECTION_REMOVED
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
        cedar::proc::Network::CONNECTION_REMOVED
      );
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
      catch (cedar::aux::InvalidNameException& exc)
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

  cedar::proc::Network::ElementMap::iterator it = mElements.find(element->getName());
  if (it != this->mElements.end())
  {
    // disconnect from revalidation signal
    if (cedar::proc::ConnectablePtr connectable = boost::dynamic_pointer_cast<cedar::proc::Connectable>(it->second))
    {
      this->mRevalidateConnections[connectable->getName()].disconnect();
      std::map<std::string, boost::signals2::connection>::iterator conn_it
        = this->mRevalidateConnections.find(connectable->getName());
      this->mRevalidateConnections.erase(conn_it);
    }
    mElements.erase(it);
  }
  this->mElementRemovedSignal(element);
}

void cedar::proc::Network::create(std::string className, std::string instanceName)
{
  cedar::proc::ElementPtr element = cedar::proc::ElementManagerSingleton::getInstance()->allocate(className);
  this->add(element, instanceName);
}

void cedar::proc::Network::add(cedar::proc::ElementPtr element, std::string instanceName)
{
  element->setName(instanceName);
  this->add(element);
}

// part of the cedar::proc::Network::add(std::list<cedar::proc::ElementPtr> elements) function
// put here because older gcc versions won't be able to compile this otherwise
//!@cond SKIPPED_DOCUMENTATION
struct DataConnectionInfo
{
  DataConnectionInfo(cedar::proc::DataSlotPtr from, cedar::proc::DataSlotPtr to)
  :
  from(from),
  to(to)
  {
  }

  cedar::proc::DataSlotPtr from;
  cedar::proc::DataSlotPtr to;
};

struct PromotedConnectionInfo
{
  PromotedConnectionInfo(const std::string& dataFrom, const std::string& dataTo)
  :
  from(dataFrom),
  to(dataTo)
  {
  }

  std::string from;
  std::string to;
};

struct TriggerConnectionInfo
{
  TriggerConnectionInfo(cedar::proc::TriggerPtr dataFrom, cedar::proc::TriggerablePtr dataTo)
  :
  from(dataFrom),
  to(dataTo)
  {
  }

  cedar::proc::TriggerPtr from;
  cedar::proc::TriggerablePtr to;
};
//!@endcond

void cedar::proc::Network::add(std::list<cedar::proc::ElementPtr> elements)
{
  typedef std::list<cedar::proc::ElementPtr>::iterator iterator;
  typedef std::list<cedar::proc::ElementPtr>::const_iterator const_iterator;
  for (iterator it = elements.begin(); it != elements.end(); )
  {
    // check if the name already exists
    if (this->nameExists((*it)->getName()))
    {
      // if so, output a warning ...
      cedar::aux::LogSingleton::getInstance()->warning
      (
        "An element of name " + (*it)->getName() + " already exists in network " + this->getName() + ".",
        "cedar::proc::Network::addElements(std::list<cedar::proc::ElementPtr> elements)"
      );

      // ... and remove the element from the list of elements to be moved
      //!@todo Can we deal with this in a better way than just ignoring the element? Maybe rename it?
      it = elements.erase(it);
    }
    // otherwise, continue to the next element
    else
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
  std::vector<DataConnectionInfo> data_connections;
  std::vector<TriggerConnectionInfo> trigger_connections;

  // sanity check if all elements have the same parent network
  cedar::proc::NetworkPtr old_network = (*(elements.begin()))->getNetwork();
  for
  (
    iterator it = elements.begin(); it != elements.end(); ++it
  )
  {
    // need two parentheses here because otherwise clang throws a warning
    if (old_network != (*it)->getNetwork())
    {
      cedar::aux::LogSingleton::getInstance()->warning
      (
        "You cannot move elements from different parent networks into network " + this->getName() + ".",
        "cedar::proc::Network::addElements(std::list<cedar::proc::ElementPtr> elements)"
      );
      return;
    }
  }


  // remember all data connections between moved elements
  if (old_network)
  {
    cedar::proc::NetworkPtr common_parent;

    // detect if we move elements up or down the group hierarchy
    if (this->contains(old_network)) // new group contains old group - moving up the hierarchy
    {
      std::vector<DataConnectionPtr> slots_deleted_later;
      for
      (
        cedar::proc::Network::DataConnectionVector::const_iterator it = old_network->getDataConnections().begin();
        it != old_network->getDataConnections().end();
        ++it
      )
      {
        // find connections that we have to restore after moving elements
        cedar::proc::ElementPtr source = old_network->getElement<cedar::proc::Element>((*it)->getSource()->getParent());
        cedar::proc::ElementPtr target = old_network->getElement<cedar::proc::Element>((*it)->getTarget()->getParent());
        iterator source_it = std::find(elements.begin(), elements.end(), source);
        iterator target_it = std::find(elements.begin(), elements.end(), target);

        // these connections connect elements that are moved
        if (source_it != elements.end() && target_it != elements.end())
        {
          // this connection must be stored (note that connections are automatically deleted if elements are removed)
          data_connections.push_back
          (
            DataConnectionInfo
            (
              (*it)->getSource(),
              (*it)->getTarget()
            )
          );
        }
        // these connections can be reduced, since they span multiple groups
        else if (source_it != elements.end() && target_it == elements.end())
        {
          std::vector<cedar::proc::DataSlotPtr> targets
            = old_network->getRealTargets
                           (
                             *it,
                             boost::static_pointer_cast<cedar::proc::ConstNetwork>(this->shared_from_this())
                           );
          slots_deleted_later.push_back(*it);
          for (unsigned int i = 0; i < targets.size(); ++i)
          {
            data_connections.push_back
            (
              DataConnectionInfo
              (
                (*it)->getSource(),
                targets.at(i)
              )
            );
          }
        }
        else if (source_it == elements.end() && target_it != elements.end())
        {
          cedar::proc::DataSlotPtr source_slot
            = old_network->getRealSource
                           (
                             *it,
                             boost::static_pointer_cast<cedar::proc::ConstNetwork>(this->shared_from_this())
                           );
          slots_deleted_later.push_back(*it);
          data_connections.push_back
          (
            DataConnectionInfo
            (
              source_slot,
              (*it)->getTarget()
            )
          );
        }
      }
      for (auto it = slots_deleted_later.begin(); it != slots_deleted_later.end(); ++it)
      {
        old_network->deleteConnectorsAlongConnection
                     (
                       *it,
                       boost::static_pointer_cast<cedar::proc::ConstNetwork>(this->shared_from_this())
                     );
      }
    }
    else if (old_network->contains(this->shared_from_this())) // old group contains new group - moving down the hierarchy
    {
      for
      (
        cedar::proc::Network::DataConnectionVector::iterator it = old_network->mDataConnections.begin();
        it != old_network->getDataConnections().end();
        ++it
      )
      {
        // find connections that we have to restore after moving elements
        cedar::proc::ElementPtr source = old_network->getElement<cedar::proc::Element>((*it)->getSource()->getParent());
        cedar::proc::ElementPtr target = old_network->getElement<cedar::proc::Element>((*it)->getTarget()->getParent());
        auto source_it = std::find(elements.begin(), elements.end(), source);
        auto target_it = std::find(elements.begin(), elements.end(), target);

        cedar::proc::DataSlotPtr source_slot = (*it)->getSource();
        std::vector<cedar::proc::DataSlotPtr> target_slots;

        // these connections connect elements that are moved
        if (source_it != elements.end() || target_it != elements.end())
        {
          // this connection must be stored (note that connections are automatically deleted if elements are removed)
          if (source_it == elements.end())
          {
            source_slot = old_network->getRealSource(*it, boost::dynamic_pointer_cast<cedar::proc::Network>(this->shared_from_this()));
            target_slots.push_back((*it)->getTarget());
          }
          else if (target_it == elements.end())
          {
            auto more_slots = old_network->getRealTargets(*it, boost::dynamic_pointer_cast<cedar::proc::Network>(this->shared_from_this()));
            target_slots.insert(target_slots.end(), more_slots.begin(), more_slots.end());
          }
          else
          {
            target_slots.push_back((*it)->getTarget());
          }
          for (unsigned i = 0; i < target_slots.size(); ++i)
          {
            data_connections.push_back
            (
              DataConnectionInfo
              (
                source_slot,
                target_slots.at(i)
              )
            );
          }
        }
      }
    }
    else // old and new network have a common parent, try to find it
    {
      // this should never be root, but just in case!
      CEDAR_ASSERT(!this->isRoot());
      common_parent = this->getNetwork();
      while (!common_parent->contains(old_network))
      {
        // this should never be root, but just in case!
        CEDAR_ASSERT(!common_parent->isRoot());
        common_parent = common_parent->getNetwork();
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
          trigger_connections.push_back(TriggerConnectionInfo(source_trigger, target_triggerable));
        }
      }
    }
  }

  // now add each element to the new network
  for (iterator it = elements.begin(); it != elements.end(); ++it)
  {
    this->add(*it);
  }

  // restore data connections
  for (unsigned int i = 0; i < data_connections.size(); ++i)
  {
    cedar::proc::Network::connectAcrossGroups(data_connections.at(i).from, data_connections.at(i).to);
  }

  // restore trigger connections
  for (unsigned int i = 0; i < trigger_connections.size(); ++i)
  {
    this->connectTrigger(trigger_connections.at(i).from, trigger_connections.at(i).to);
  }
}

void cedar::proc::Network::add(cedar::proc::ElementPtr element)
{
  std::string instanceName = element->getName();
  if (instanceName.empty())
  {
    CEDAR_THROW(cedar::aux::InvalidNameException, "Cannot add an element that has an empty name.");
  }
  else if (mElements.find(instanceName) != mElements.end())
  {
    CEDAR_THROW
    (
      cedar::proc::DuplicateNameException,
      "Duplicate element name entry \"" + instanceName + "\" in network \"" + this->getName() + "\""
    );
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

  // connect to revalidation signal
  if (cedar::proc::ConnectablePtr connectable = boost::dynamic_pointer_cast<cedar::proc::Connectable>(element))
  {
    this->mRevalidateConnections[connectable->getName()]
      = connectable->connectToOutputPropertiesChanged(boost::bind(&cedar::proc::Network::revalidateConnections, this, _1));
  }
}

void cedar::proc::Network::addConnector(const std::string& name, bool input)
{
  if
  (
    this->nameExists(name)
  )
  {
    CEDAR_THROW(cedar::aux::DuplicateNameException, "Cannot add a connector with the name \"" + name + "\". It is already taken.");
  }
  // check if connector is in map of connectors
  if (_mConnectors->find(name) != _mConnectors->end())
  {
    // do nothing for now
  }
  else
  {
    _mConnectors->set(name, input);
  }

  if (input)
  {
    this->declareInput(name, false);
    cedar::proc::sources::GroupSourcePtr source(new cedar::proc::sources::GroupSource());
    this->add(source, name);
  }
  else
  {
    cedar::proc::sinks::GroupSinkPtr sink(new cedar::proc::sinks::GroupSink());
    this->declareSharedOutput(name, sink->getEmptyData());
    this->add(sink, name);
  }
}

void cedar::proc::Network::removeConnector(const std::string& name, bool input)
{
  // check if connector is in map of connectors
  auto it = _mConnectors->find(name);
  if (it != _mConnectors->end())
  {
    CEDAR_ASSERT(it->second == input);
    // here be comments
    if (input)
    {
      this->removeAllConnectionsFromSlot(this->getElement<cedar::proc::Connectable>(name)->getOutputSlot("output"));
      if (this->getNetwork())
      {
        this->getNetwork()->removeAllConnectionsFromSlot(this->getInputSlot(name));
      }
      this->remove(this->getElement(name));
      this->removeInputSlot(name);
    }
    else
    {
      this->removeAllConnectionsFromSlot(this->getElement<cedar::proc::Connectable>(name)->getInputSlot("input"));
      if (this->getNetwork())
      {
        this->getNetwork()->removeAllConnectionsFromSlot(this->getOutputSlot(name));
      }
      this->remove(this->getElement(name));
      this->removeOutputSlot(name);
    }
    _mConnectors->erase(it->first);
  }
  else
  {
    CEDAR_THROW(cedar::aux::NotFoundException, "Could not find the requested connector.");
  }
}

std::string cedar::proc::Network::duplicate(const std::string& elementName, const std::string& newName)
{
  cedar::proc::ElementPtr elem;
  try
  {
    // determine class
    elem = this->getElement(elementName);
  }
  catch (cedar::aux::InvalidNameException& exc)
  {
    CEDAR_THROW
    (
      cedar::aux::InvalidNameException,
      "cannot duplicate element of name " + elementName + ", it does not exist in network" + this->getName()
    )
  }

  std::string class_name = cedar::proc::ElementManagerSingleton::getInstance()->getTypeId(elem);
  // allocate object
  cedar::proc::ElementPtr new_elem = cedar::proc::ElementManagerSingleton::getInstance()->allocate(class_name);
  // copy configuration tree
  new_elem->copyFrom(elem);
  // get unique name
  std::string modified_name;
  if (!newName.empty()) // desired name given
  {
    modified_name = this->getUniqueIdentifier(newName);
  }
  else // default name
  {
    modified_name = this->getUniqueIdentifier(elementName);
  }
  // set unique name
  new_elem->setName(modified_name);
  // add to network
  this->add(new_elem);

  return modified_name;
}

std::string cedar::proc::Network::getUniqueName(const std::string& unmodifiedName) const
{
  return this->getUniqueIdentifier(unmodifiedName);
}

cedar::proc::ConstElementPtr cedar::proc::Network::getElement(const cedar::proc::NetworkPath& name) const
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
      CEDAR_THROW(cedar::aux::InvalidNameException, "The given name does not specify a proper path in this network.");
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
      cedar::aux::InvalidNameException, "No element of the name \"" + name
        + "\" was found in the network \"" + this->getName() + "\"."
    );
  }
}

cedar::proc::ElementPtr cedar::proc::Network::getElement(const cedar::proc::NetworkPath& name)
{
  return boost::const_pointer_cast<Element>(static_cast<const Network*>(this)->getElement(name));
}

void cedar::proc::Network::connectSlots(cedar::proc::ConstDataSlotPtr source, cedar::proc::ConstDataSlotPtr target)
{
  this->connectSlots(source->getParent() + "." + source->getName(), target->getParent() + "." + target->getName());
}

void cedar::proc::Network::connectSlots(const std::string& source, const std::string& target)
{
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
    cedar::proc::Network::CONNECTION_ADDED
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
  this->disconnectSlot(connectable, slot);
}

void cedar::proc::Network::disconnectSlot(cedar::proc::ConnectablePtr connectable, const std::string& slot)
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
      // inform any interested listeners of this removed connection
      mDataConnectionChanged(sourceSlot, targetSlot, cedar::proc::Network::CONNECTION_REMOVED);
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

void cedar::proc::Network::writeConfiguration(cedar::aux::ConfigurationNode& root) const
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

  cedar::aux::ConfigurationNode records;
  this->writeRecords(records);
  if (!records.empty())
    root.add_child("records", records);

  this->cedar::aux::Configurable::writeConfiguration(root);
}

void cedar::proc::Network::writeMetaData(cedar::aux::ConfigurationNode& meta) const
{
  meta.put("format", 1);

  // determine what plugins are used by the network
  std::set<std::string> required_plugins;
  for (auto element_iter = this->getElements().begin(); element_iter != this->getElements().end(); ++element_iter)
  {
    cedar::proc::ElementPtr element = element_iter->second;
    auto declaration = cedar::proc::ElementManagerSingleton::getInstance()->getDeclarationOf(element);
    if (!declaration->getSource().empty())
    {
      required_plugins.insert(declaration->getSource());
    }
  }

  // if plugins are used, write them to the meta node
  if (!required_plugins.empty())
  {
    cedar::aux::ConfigurationNode required_plugins_node;
    for (auto iter = required_plugins.begin(); iter != required_plugins.end(); ++iter)
    {
      cedar::aux::ConfigurationNode value_node;
      value_node.put_value(*iter);
      required_plugins_node.push_back(cedar::aux::ConfigurationNode::value_type("", value_node));
    }

    meta.push_back(cedar::aux::ConfigurationNode::value_type("required plugins", required_plugins_node));
  }
}

std::set<std::string> cedar::proc::Network::getRequiredPlugins(const std::string& architectureFile)
{
  std::set<std::string> plugins;

  cedar::aux::ConfigurationNode configuration;
  try
  {
    boost::property_tree::read_json(architectureFile, configuration);
  }
  catch (boost::property_tree::json_parser::json_parser_error&)
  {
    return plugins;
  }

  auto meta_iter = configuration.find("meta");
  if (meta_iter == configuration.not_found())
  {
    return plugins;
  }

  auto rq_plugins_iter = meta_iter->second.find("required plugins");
  if (rq_plugins_iter == meta_iter->second.not_found())
  {
    return plugins;
  }

  auto rq_plugins_node = rq_plugins_iter->second;
  for (auto iter = rq_plugins_node.begin(); iter != rq_plugins_node.end(); ++iter)
  {
    plugins.insert(iter->second.get_value<std::string>());
  }

  return plugins;
}

void cedar::proc::Network::readConfiguration(const cedar::aux::ConfigurationNode& root)
{
  std::vector<std::string> exceptions;
  this->readConfiguration(root, exceptions);

  if (!exceptions.empty())
  {
    cedar::proc::ArchitectureLoadingException exception(exceptions);
    CEDAR_THROW_EXCEPTION(exception);
  }
}

void cedar::proc::Network::readConfiguration(const cedar::aux::ConfigurationNode& root, std::vector<std::string>& exceptions)
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
      "Could not recognize format for group \"" + this->getName()
        + "\": format or meta node missing. Defaulting to current version.",
      "Reading network",
      "cedar::proc::Network::readFrom(const cedar::aux::ConfigurationNode&)"
    );
  }
  // make a copy of the configuration that was read -- the ui may need to read additional information from it
  mLastReadConfiguration = root;

  // select the proper format for reading the network
  switch (format_version)
  {
    default:
      cedar::aux::LogSingleton::getInstance()->warning
      (
        "Could not recognize format for group \"" + this->getName() + "\": "
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

  this->processConnectors();

  try
  {
    const cedar::aux::ConfigurationNode& steps = root.get_child("steps");
    this->readSteps(steps, exceptions);
  }
  catch (const boost::property_tree::ptree_bad_path&)
  {
    // no steps declared -- this is ok.
  }

  try
  {
    const cedar::aux::ConfigurationNode& networks = root.get_child("networks");
    this->readNetworks(networks, exceptions);
  }
  catch (const boost::property_tree::ptree_bad_path&)
  {
    // no networks declared -- this is ok.
  }

  try
  {
    const cedar::aux::ConfigurationNode& connections = root.get_child("connections");
    this->readDataConnections(connections, exceptions);
  }
  catch (const boost::property_tree::ptree_bad_path&)
  {
    // no connections declared -- this is ok.
  }

  try
  {
    const cedar::aux::ConfigurationNode& triggers = root.get_child("triggers");
    this->readTriggers(triggers, exceptions);
  }
  catch (const boost::property_tree::ptree_bad_path&)
  {
    // no triggers declared -- this is ok.
  }

  try
  {
    const cedar::aux::ConfigurationNode& records = root.get_child("records");
    this->readRecords(records, exceptions);
  }
  catch (const boost::property_tree::ptree_bad_path&)
  {
    // no records declared -- this is ok.
  }
}

void cedar::proc::Network::writeSteps(cedar::aux::ConfigurationNode& steps) const
{
  for (auto iter = this->mElements.begin(); iter != this->mElements.end(); ++iter)
  {
    // if this is a step, write this to the configuration tree
    if (cedar::proc::StepPtr step = boost::dynamic_pointer_cast<cedar::proc::Step>(iter->second))
    {
      if
      (
        boost::dynamic_pointer_cast<cedar::proc::sinks::GroupSink>(step)
          || boost::dynamic_pointer_cast<cedar::proc::sources::GroupSource>(step)
      )
      {
        continue;
      }
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
  for (cedar::aux::ConfigurationNode::const_iterator iter = root.begin();
      iter != root.end();
      ++iter)
  {
    const std::string class_id = iter->first;
    const cedar::aux::ConfigurationNode& step_node = iter->second;

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

void cedar::proc::Network::writeTriggers(cedar::aux::ConfigurationNode& triggers) const
{
  for (auto iter = this->mElements.begin(); iter != this->mElements.end(); ++iter)
  {
    // if this is a trigger, write this to the configuration tree
    if (cedar::proc::TriggerPtr trigger = boost::dynamic_pointer_cast<cedar::proc::Trigger>(iter->second))
    {
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
  for (cedar::aux::ConfigurationNode::const_iterator iter = root.begin();
      iter != root.end();
      ++iter)
  {
    const std::string& class_id = iter->first;
    const cedar::aux::ConfigurationNode& trigger_node = iter->second;

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

void cedar::proc::Network::writeRecords(cedar::aux::ConfigurationNode& records) const
{
  std::map<std::string, int> dataMap = cedar::aux::RecorderSingleton::getInstance()->getRegisteredData();
  for (auto iter = dataMap.begin(); iter != dataMap.end(); ++iter)
  {
    cedar::aux::ConfigurationNode recorder_node(std::to_string(iter->second));
    records.push_back(cedar::aux::ConfigurationNode::value_type(iter->first,recorder_node));
  }
}

void cedar::proc::Network::readRecords
     (
       const cedar::aux::ConfigurationNode& root,
       std::vector<std::string>& /* exceptions */
     )
{
  //clear all registered data
  cedar::aux::RecorderSingleton::getInstance()->clear();
  //create a new map to get a better data structure
  std::map<std::string, int> data;
  for (cedar::aux::ConfigurationNode::const_iterator node_iter = root.begin();
       node_iter != root.end();
       ++node_iter)
  {
      data[node_iter->first] = root.get<int>(node_iter->first);
  }

  // check for every slot if it is to register or not
  for (auto iter = this->mElements.begin(); iter != this->mElements.end(); ++iter)
  {
    if (cedar::proc::StepPtr step = boost::dynamic_pointer_cast<cedar::proc::Step>(iter->second))
    {
      std::vector<cedar::proc::DataRole::Id> slotTypes;
      slotTypes.push_back(cedar::proc::DataRole::BUFFER);
      slotTypes.push_back(cedar::proc::DataRole::OUTPUT);
      for (unsigned int s = 0; s < slotTypes.size(); s++)
      {
        if (step->hasRole(slotTypes[s]))
        {
          cedar::proc::Connectable::SlotList dataSlots = step->getOrderedDataSlots(slotTypes[s]);
          for (unsigned int i = 0; i < dataSlots.size(); i++)
          {
            std::string name =  step->getName()+"_"+dataSlots[i]->getName();
            if (data.count(name)==1)
            {
              cedar::aux::RecorderSingleton::getInstance()->registerData(dataSlots[i]->getData(),data[name],name);
            }
          }
        }
      }
    }
  }
}

void cedar::proc::Network::writeNetworks(cedar::aux::ConfigurationNode& networks) const
{
  for (auto iter = this->mElements.begin(); iter != this->mElements.end(); ++iter)
  {
    // if this is a network, write this to the configuration tree
    if (cedar::proc::NetworkPtr network = boost::dynamic_pointer_cast<cedar::proc::Network>(iter->second))
    {
      cedar::aux::ConfigurationNode network_node;
      network->writeConfiguration(network_node);
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
  for (cedar::aux::ConfigurationNode::const_iterator iter = root.begin();
      iter != root.end();
      ++iter)
  {
    const std::string& network_name = iter->first;
    const cedar::aux::ConfigurationNode& network_node = iter->second;

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

    network->readConfiguration(network_node, exceptions);
    // is this enough to recursively read in the network?
    network->resetChangedStates(false);
  }
}

void cedar::proc::Network::writeDataConnection
     (
       cedar::aux::ConfigurationNode& root,
       const cedar::proc::DataConnectionPtr connection
     )
     const
{
  std::string source_str = connection->getSource()->getParent() + "." + connection->getSource()->getName();
  std::string target_str = connection->getTarget()->getParent() + "." + connection->getTarget()->getName();

  cedar::aux::ConfigurationNode connection_node;
  connection_node.put("source", source_str);
  connection_node.put("target", target_str);
  root.push_back(cedar::aux::ConfigurationNode::value_type("", connection_node));
}

void cedar::proc::Network::writeDataConnections(cedar::aux::ConfigurationNode& root) const
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
  for (cedar::aux::ConfigurationNode::const_iterator iter = root.begin();
      iter != root.end();
      ++iter)
  {
    std::string source = iter->second.get<std::string>("source");
    std::string target = iter->second.get<std::string>("target");
    try
    {
      this->connectSlots(source, target);
    }
    catch (cedar::aux::ExceptionBase& e)
    {
      std::string info = "Exception occurred while connecting \"" + source + "\" to \"" + target + "\": "
                         + e.exceptionInfo();
      exceptions.push_back(info);
    }
    catch (const std::exception& e)
    {
      std::string info = "Exception occurred while connecting \"" + source + "\" to \"" + target + "\": "
                         + std::string(e.what());
      exceptions.push_back(info);
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
    // check if con is a valid pointer - during deletion of a step, some deleted connections are still in this list
    if (cedar::proc::DataConnectionPtr con = this->mDataConnections.at(i))
    {
      if
      (
        (this->getElement<cedar::proc::Connectable>(con->getSource()->getParent()) == source
          && con->getSource()->getName() == sourceDataName)
        ||
        (this->getElement<cedar::proc::Connectable>(con->getTarget()->getParent()) == source
          && con->getTarget()->getName() == sourceDataName)
      )
      {
        connections.push_back(con);
      }
    }
  }
}

void cedar::proc::Network::getDataConnections(
                                               cedar::proc::ConstConnectablePtr source,
                                               const std::string& sourceDataName,
                                               std::vector<cedar::proc::ConstDataConnectionPtr>& connections
                                             ) const
{
  connections.clear();
  for (size_t i = 0; i < this->mDataConnections.size(); ++i)
  {
    // check if con is a valid pointer - during deletion of a step, some deleted connections are still in this list
    if (cedar::proc::ConstDataConnectionPtr con = this->mDataConnections.at(i))
    {
      if
      (
        (this->getElement<cedar::proc::ConstConnectable>(con->getSource()->getParent()) == source
          && con->getSource()->getName() == sourceDataName)
        ||
        (this->getElement<cedar::proc::ConstConnectable>(con->getTarget()->getParent()) == source
          && con->getTarget()->getName() == sourceDataName)
      )
      {
        connections.push_back(con);
      }
    }
  }
}

cedar::proc::Network::DataConnectionVector::iterator cedar::proc::Network::removeDataConnection
                                                     (
                                                       cedar::proc::Network::DataConnectionVector::iterator it
                                                     )
{
  //!@todo This code needs to be cleaned up, simplified and commented
  cedar::proc::DataConnectionPtr connection = *it;
  std::string source_name = connection->getSource()->getParent();
  std::string target_name = connection->getTarget()->getParent();

  cedar::proc::TriggerablePtr triggerable_target = this->getElement<cedar::proc::Triggerable>(target_name);
  CEDAR_DEBUG_ASSERT(triggerable_target);
  if (!triggerable_target->isLooped())
  {
    target_name = connection->getTarget()->getParent(); // reset target_name
    // check that both Connectables are not connected through some other DataSlots
    cedar::proc::ConnectablePtr target_connectable = this->getElement<cedar::proc::Connectable>(target_name);
    for (DataConnectionVector::iterator iter = mDataConnections.begin(); iter != mDataConnections.end(); ++iter)
    {
      if
      (
        // check if this is the connection being erased
        iter != it &&
        // if it isn't, check if it connects to the same target
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
    this->disconnectTrigger(
                             this->getElement<cedar::proc::Triggerable>(source_name)->getFinishedTrigger(),
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
  catch (cedar::aux::InvalidNameException& e) // this can happen if element is not found, no problem, see below
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
                              boost::function<void (cedar::proc::ConstDataSlotPtr, cedar::proc::ConstDataSlotPtr, cedar::proc::Network::ConnectionChange)> slot
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

void cedar::proc::Network::revalidateConnections(const std::string& sender)
{
  std::string child;
  std::string output;
  cedar::aux::splitLast(sender, ".", child, output);
  std::vector<cedar::proc::DataConnectionPtr> connections;
  this->getDataConnections(this->getElement<Connectable>(child), output, connections);
  for (unsigned i = 0; i < connections.size(); ++i)
  {
    cedar::proc::ConnectablePtr sender = this->getElement<Connectable>(connections.at(i)->getSource()->getParent());
    cedar::proc::ConnectablePtr receiver = this->getElement<Connectable>(connections.at(i)->getTarget()->getParent());
    receiver->callInputConnectionChanged(connections.at(i)->getTarget()->getName());
  }
}

void cedar::proc::Network::processConnectors()
{
  for (auto it = _mConnectors->begin(); it != _mConnectors->end(); ++it)
  {
    this->addConnector(it->first, it->second);
  }
}

void cedar::proc::Network::removeAllConnectors()
{
  while (!_mConnectors->empty())
  {
    this->removeConnector(_mConnectors->begin()->first, _mConnectors->begin()->second);
  }
}

void cedar::proc::Network::onTrigger(cedar::proc::ArgumentsPtr args, cedar::proc::TriggerPtr /* trigger */)
{
  for (auto iter = this->_mConnectors->begin(); iter != this->_mConnectors->end(); ++iter)
  {
    auto name = iter->first;

    if (iter->second)
    {
      auto source = boost::static_pointer_cast<cedar::proc::sources::GroupSource>(this->getElement(name));
      source->onTrigger(args);
    }
  }
}

void cedar::proc::Network::waitForProcessing()
{
  this->QThread::wait();
}

void cedar::proc::Network::inputConnectionChanged(const std::string& inputName)
{
  cedar::proc::sources::GroupSourcePtr source = this->getElement<cedar::proc::sources::GroupSource>(inputName);
  if (this->getInput(inputName))
  {
    source->setData(boost::const_pointer_cast<cedar::aux::Data>(this->getInput(inputName)));
  }
  else
  {
    source->resetData();
  }
}

const cedar::proc::Network::ConnectorMap& cedar::proc::Network::getConnectorMap()
{
  return this->_mConnectors->getValue();
}

bool cedar::proc::Network::hasConnector(const std::string& name) const
{
  return (this->_mConnectors->find(name) != this->_mConnectors->end());
}

void cedar::proc::Network::revalidateInputSlot(const std::string& slot)
{
  this->setState(cedar::proc::Triggerable::STATE_UNKNOWN, "");
  this->getInputSlot(slot)->setValidity(cedar::proc::DataSlot::VALIDITY_UNKNOWN);
  this->inputConnectionChanged(slot);
  this->getInputValidity(slot);
}

bool cedar::proc::Network::contains(cedar::proc::ConstElementPtr element) const
{
  for (auto it = mElements.begin(); it != mElements.end(); ++it)
  {
    if (it->second == element)
    {
      return true;
    }
    else
    {
      if (cedar::proc::ConstNetworkPtr nested = boost::dynamic_pointer_cast<cedar::proc::ConstNetwork>(it->second))
      {
        if (nested->contains(element))
        {
          return true;
        }
      }
    }
  }
  return false;
}

bool cedar::proc::Network::isRoot() const
{
  return !static_cast<bool>(this->getNetwork());
}

std::vector<cedar::proc::DataSlotPtr> cedar::proc::Network::getRealTargets
                                           (
                                             cedar::proc::DataConnectionPtr connection,
                                             cedar::proc::ConstNetworkPtr targetNetwork
                                           )
{
  std::vector<cedar::proc::DataSlotPtr> real_targets;
  if (this != targetNetwork.get())
  {
    if (this->contains(targetNetwork)) // going down the hierarchy
    {
      if
      (
        cedar::proc::StepPtr step
          = this->getElement<cedar::proc::Step>(connection->getTarget()->getParent())
      )
      {
        real_targets.push_back(step->getInputSlot(connection->getTarget()->getName()));
      }
      else
      {
        std::vector<cedar::proc::DataConnectionPtr> connections;
        cedar::proc::NetworkPtr nested = this->getElement<cedar::proc::Network>(connection->getTarget()->getParent());
        nested->getDataConnections
                (
                  nested->getElement<cedar::proc::Step>(connection->getTarget()->getName()),
                  "output",
                  connections
                );
        for (unsigned int i = 0; i < connections.size(); ++i)
        {
          auto more_targets = nested->getRealTargets(connections.at(i), targetNetwork);
          real_targets.insert(real_targets.end(), more_targets.begin(), more_targets.end());
        }
      }
    }
    else
    {
      std::vector<cedar::proc::DataConnectionPtr> connections;
      this->getNetwork()->getDataConnections
                          (
                            boost::static_pointer_cast<cedar::proc::Connectable>(this->shared_from_this()),
                            connection->getTarget()->getParent(),
                            connections
                          );
      for (unsigned int i = 0; i < connections.size(); ++i)
      {
        if
        (
          cedar::proc::StepPtr step
            = this->getNetwork()->getElement<cedar::proc::Step>(connections.at(0)->getTarget()->getParent())
        )
        {
          real_targets.push_back(step->getInputSlot(connections.at(i)->getTarget()->getName()));
        }
        else
        {
          auto more_targets = this->getNetwork()->getRealTargets(connections.at(i), targetNetwork);
          real_targets.insert(real_targets.end(), more_targets.begin(), more_targets.end());
        }
      }
    }
  }
  else
  {
    real_targets.push_back(connection->getTarget());
  }
  return real_targets;
}

cedar::proc::DataSlotPtr cedar::proc::Network::getRealSource
                         (
                           cedar::proc::DataConnectionPtr connection,
                           cedar::proc::ConstNetworkPtr targetNetwork
                         )
{
  if (this != targetNetwork.get())
  {
    if (this->contains(targetNetwork)) // going down the hierarchy
    {
      if
      (
        cedar::proc::StepPtr step
          = this->getElement<cedar::proc::Step>(connection->getSource()->getParent())
      )
      {
        return step->getOutputSlot(connection->getSource()->getName());
      }
      else
      {
        std::vector<cedar::proc::DataConnectionPtr> connections;
        cedar::proc::NetworkPtr nested = this->getElement<cedar::proc::Network>(connection->getSource()->getParent());
        nested->getDataConnections
                (
                  nested->getElement<cedar::proc::Step>(connection->getSource()->getName()),
                  "input",
                  connections
                );
        CEDAR_ASSERT(connections.size() == 1);
        return nested->getRealSource(connections.at(0), targetNetwork);
      }
    }
    else // going up the hierarchy
    {
      std::vector<cedar::proc::DataConnectionPtr> connections;
      this->getNetwork()->getDataConnections
                          (
                            boost::static_pointer_cast<cedar::proc::Connectable>(this->shared_from_this()),
                            connection->getSource()->getParent(),
                            connections
                          );
      CEDAR_ASSERT(connections.size() == 1);
      if
      (
        cedar::proc::StepPtr step
          = this->getNetwork()->getElement<cedar::proc::Step>(connections.at(0)->getSource()->getParent())
      )
      {
        return step->getOutputSlot(connections.at(0)->getSource()->getName());
      }
      else
      {
        return this->getNetwork()->getRealSource(connections.at(0), targetNetwork);
      }
    }
  }
  else
  {
    return connection->getSource();
  }
}

void cedar::proc::Network::deleteConnectorsAlongConnection
                           (
                             cedar::proc::DataConnectionPtr connection,
                             cedar::proc::ConstNetworkPtr targetNetwork
                           )
{
  if (this->isRoot() || this == targetNetwork.get())
  {
    return;
  }
  else
  {
    ConnectorMap connectors_removed_later;
    if (cedar::proc::sources::GroupSourcePtr source = this->getElement<cedar::proc::sources::GroupSource>(connection->getSource()->getParent()))
    {
      std::vector<cedar::proc::DataConnectionPtr> connections;
      this->getNetwork()->getDataConnections
                          (
                            boost::static_pointer_cast<cedar::proc::Connectable>(this->shared_from_this()),
                            source->getName(),
                            connections
                          );
      for (auto it = connections.begin(); it != connections.end(); ++it)
      {
        this->getNetwork()->deleteConnectorsAlongConnection(*it, targetNetwork);
      }
      connectors_removed_later[source->getName()] = true;
    }
    if (cedar::proc::sinks::GroupSinkPtr sink = this->getElement<cedar::proc::sinks::GroupSink>(connection->getTarget()->getParent()))
    {
      std::vector<cedar::proc::DataConnectionPtr> connections;
      this->getNetwork()->getDataConnections
                          (
                            boost::static_pointer_cast<cedar::proc::Connectable>(this->shared_from_this()),
                            sink->getName(),
                            connections
                          );
      for (auto it = connections.begin(); it != connections.end(); ++it)
      {
        this->getNetwork()->deleteConnectorsAlongConnection(*it, targetNetwork);
      }
      connectors_removed_later[sink->getName()] = false;
    }
    connection->disconnect();
    for (auto it = connectors_removed_later.begin(); it != connectors_removed_later.end(); ++it)
    {
      this->removeConnector(it->first, it->second);
    }
  }
}

void cedar::proc::Network::connectAcrossGroups(cedar::proc::DataSlotPtr source, cedar::proc::DataSlotPtr target)
{
  cedar::proc::Connectable* source_step = source->getParentPtr();
  cedar::proc::Connectable* target_step = target->getParentPtr();
  cedar::proc::NetworkPtr source_group = source_step->getNetwork();
  cedar::proc::NetworkPtr target_group = target_step->getNetwork();
  if (source_group->contains(target_group)) // connection going down the hierarchy
  {
    std::string connector_name = target_group->getUniqueIdentifier("external input");
    target_group->addConnector(connector_name, true);
    target_group->connectSlots
                  (
                    target_group->getElement<cedar::proc::Connectable>(connector_name)->getOutputSlot("output"),
                    target
                  );
    cedar::proc::Network::connectAcrossGroups(source, target_group->getInputSlot(connector_name));
  }
  else if (target_group->contains(source_group)) // connection going up the hierarchy
  {
    std::string connector_name = source_group->getUniqueIdentifier("external output");
    source_group->addConnector(connector_name, false);
    source_group->connectSlots
                  (
                    source,
                    source_group->getElement<cedar::proc::Connectable>(connector_name)->getInputSlot("input")
                  );
    cedar::proc::Network::connectAcrossGroups(source_group->getOutputSlot(connector_name), target);
  }
  else if (source_group == target_group) // connection in same group
  {
    source_group->connectSlots(source, target);
  }
  else // connection going through shared parent
  {
    ;
  }
}
