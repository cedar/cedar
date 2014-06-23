/*======================================================================================================================

    Copyright 2011, 2012, 2013, 2014 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany

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

    File:        Trigger.cpp


    Maintainer:  Oliver Lomp,
                 Mathis Richter,
                 Stephan Zibner
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de,
                 mathis.richter@ini.ruhr-uni-bochum.de,
                 stephan.zibner@ini.ruhr-uni-bochum.de
    Date:        2011 05 23

    Description:

    Credits:

======================================================================================================================*/

// CEDAR INCLUDES
#include "cedar/processing/Trigger.h"
#include "cedar/processing/Step.h"
#include "cedar/processing/Element.h"
#include "cedar/processing/ElementDeclaration.h"
#include "cedar/processing/DeclarationRegistry.h"
#include "cedar/auxiliaries/GraphTemplate.h"
#include "cedar/auxiliaries/Log.h"
#include "cedar/auxiliaries/stringFunctions.h"

// SYSTEM INCLUDES
#include <QReadLocker>
#include <QWriteLocker>
#include <algorithm>


//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::proc::Trigger::Trigger(const std::string& name, bool isLooped)
:
Triggerable(isLooped),
mpOwner(NULL)
{
  cedar::aux::LogSingleton::getInstance()->allocating(this);

  this->setName(name);
}

cedar::proc::Trigger::~Trigger()
{
  cedar::aux::LogSingleton::getInstance()->freeing(this);

  QReadLocker lock(this->mListeners.getLockPtr());

  for (auto listener : this->mListeners.member())
  {
    listener->noLongerTriggeredBy(boost::static_pointer_cast<cedar::proc::Trigger>(this->shared_from_this()));
  }

  this->mListeners.member().clear();
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::proc::Trigger::buildTriggerGraph(cedar::aux::GraphTemplate<cedar::proc::TriggerablePtr>& graph)
{
  // root node corresponding to this trigger
  std::vector<cedar::proc::TriggerablePtr> to_explore;

  auto this_ptr = boost::static_pointer_cast<cedar::proc::Trigger>(this->shared_from_this());

  auto this_node = graph.addNodeForPayload(this_ptr);

  {
    QReadLocker lock(this->mListeners.getLockPtr());
    for (auto listener : this->mListeners.member())
    {
      if (listener->isLooped())
      {
        continue;
      }

      to_explore.push_back(listener);
      auto listener_node = graph.addNodeForPayload(listener);
      graph.addEdge(this_node, listener_node);
    }
  }

  while(!to_explore.empty())
  {
    cedar::proc::TriggerablePtr source = to_explore.back();
    to_explore.pop_back();

    if (!graph.hasNodeForPayload(source))
    {
      graph.addNodeForPayload(source);
    }

    auto source_node = graph.getNodeByPayload(source);

    // get the done trigger
    //!@todo Is getting the finished trigger thread-safe?
    cedar::proc::TriggerPtr done_trigger = source->getFinishedTrigger();

    QReadLocker lock(done_trigger->mListeners.getLockPtr());
    // append all listeners of this step; they all have a distance of one, because they follow this step directly
    for (auto listener : done_trigger->mListeners.member())
    {
      if (listener->isLooped())
      {
        continue;
      }

      if (!graph.hasNodeForPayload(listener))
      {
        graph.addNodeForPayload(listener);
        to_explore.push_back(listener);
      }

      auto listener_node = graph.getNodeByPayload(listener);
      graph.addEdge(source_node, listener_node);
    }
  }
}

void cedar::proc::Trigger::updateTriggeringOrder(std::set<cedar::proc::Trigger*>& visited, bool recurseUp, bool recurseDown)
{
  // check if this trigger was already visited during the current wave of graph generation
  if (visited.find(this) != visited.end())
  {
    return;
  }
  else
  {
    // we did not visit this trigger yet - add it to the set of visited triggers
    visited.insert(this);
  }
  // build the triggering graph for this trigger
  cedar::aux::GraphTemplate<cedar::proc::TriggerablePtr> graph;
  this->buildTriggerGraph(graph);

  // test for cycle
  auto cycles = graph.getCycleComponents();
  if (!cycles.empty())
  {
    std::vector<std::set<cedar::proc::TriggerablePtr> > triggerable_cycles;
    triggerable_cycles.resize(cycles.size());
    for (size_t i = 0; i < cycles.size(); ++i)
    {
      for (auto iter = cycles.at(i).begin(); iter != cycles.at(i).end(); ++iter)
      {
        auto node = *iter;
        triggerable_cycles.at(i).insert(node->getPayload());
      }
    }

    cedar::proc::TriggerCycleException exception(triggerable_cycles);
    CEDAR_THROW_EXCEPTION(exception);
  }

  // append all listeners of this step; they all have a distance of one, because they follow this step directly
  //!@todo Can this take results from other triggers into account? Otherwise, multiple triggers might calculate the same thing over and over again.
  cedar::proc::TriggerPtr this_ptr = boost::static_pointer_cast<cedar::proc::Trigger>(this->shared_from_this());
  auto this_node = graph.getNodeByPayload(this_ptr);
  auto distances = graph.getMaximumDistance(this_node);

  // we (should) now have a map assigning a distance to all triggerables triggered by this trigger
  // now we need to transform it to a usable structures
  QWriteLocker lock_w(this->mTriggeringOrder.getLockPtr());
  this->mTriggeringOrder.member().clear();

  for (auto dist_iter = distances.begin(); dist_iter != distances.end(); ++dist_iter)
  {
    auto node = dist_iter->first;
    auto triggerable = node->getPayload();

    // root node has no pointer
    if (!triggerable)
    {
      continue;
    }

    auto distance = dist_iter->second;
    auto iter = this->mTriggeringOrder.member().find(distance);
    if (iter == this->mTriggeringOrder.member().end())
    {
      this->mTriggeringOrder.member()[distance] = std::set<cedar::proc::TriggerablePtr>();
      iter = this->mTriggeringOrder.member().find(distance);
      CEDAR_DEBUG_ASSERT(iter != this->mTriggeringOrder.member().end());
    }

    iter->second.insert(triggerable);
  }

  lock_w.unlock();

  {
    // recursively update the triggering order for all triggers triggered by this one
    QReadLocker lock_r(this->mListeners.getLockPtr());
    if (recurseDown)
    {
      for (cedar::proc::TriggerablePtr listener : this->mListeners.member())
      {
        QReadLocker lock_r_finished(listener->mFinished.getLockPtr());
        cedar::proc::TriggerPtr done = listener->mFinished.member();
        if (done)
        {
          done->updateTriggeringOrder(visited, false, true);
        }
      }
    }
  }

  // recursively update the triggering order for all triggers by which this one is triggered
  if (recurseUp && this->mpOwner != nullptr)
  {
    QReadLocker lock_r(this->mpOwner->mTriggersListenedTo.getLockPtr());
    for (const auto& trigger_weak : this->mpOwner->mTriggersListenedTo.member())
    {
      auto trigger = trigger_weak.lock();
      CEDAR_ASSERT(trigger);
      trigger->updateTriggeringOrder(visited, true, false);
    }
  }
}

void cedar::proc::Trigger::trigger(cedar::proc::ArgumentsPtr arguments)
{
  auto this_ptr = boost::static_pointer_cast<cedar::proc::Trigger>(this->shared_from_this());

  QReadLocker lock(this->mTriggeringOrder.getLockPtr());
  for (auto order_triggerables_pair : this->mTriggeringOrder.member())
  {
    auto triggerables = order_triggerables_pair.second;
    for (cedar::proc::TriggerablePtr triggerable : triggerables)
    {
      triggerable->onTrigger(arguments, this_ptr);
    }
  }
}

void cedar::proc::Trigger::onTrigger(cedar::proc::ArgumentsPtr, cedar::proc::TriggerPtr)
{
}

void cedar::proc::Trigger::addListener(cedar::proc::TriggerablePtr triggerable)
{
  auto this_ptr = boost::static_pointer_cast<cedar::proc::Trigger>(this->shared_from_this());

  QWriteLocker lock(this->mListeners.getLockPtr());
  std::vector<cedar::proc::TriggerablePtr>::iterator iter;
  iter = this->find(triggerable);
  if (iter == this->mListeners.member().end())
  {
    this->mListeners.member().push_back(triggerable);
    triggerable->triggeredBy(this_ptr);
    
    //!@todo Can this be removed (because MultiTriggers don't exist anymore)
    if (cedar::proc::TriggerPtr trigger = boost::dynamic_pointer_cast<cedar::proc::Trigger>(triggerable))
    {
      trigger->notifyConnected(this_ptr);
    }

    lock.unlock();
    std::set<cedar::proc::Trigger*> visited;
    this->updateTriggeringOrder(visited);
  }
}

bool cedar::proc::Trigger::isListener(cedar::proc::TriggerablePtr triggerable) const
{
  QReadLocker lock(this->mListeners.getLockPtr());
  return this->find(triggerable) != this->mListeners.member().end();
}

void cedar::proc::Trigger::removeListener(cedar::proc::TriggerablePtr triggerable)
{
  this->removeListener(triggerable.get());
}

void cedar::proc::Trigger::removeListener(cedar::proc::Triggerable* triggerable)
{
  auto this_ptr = boost::static_pointer_cast<cedar::proc::Trigger>(this->shared_from_this());

  QWriteLocker lock(this->mListeners.getLockPtr());
  std::vector<cedar::proc::TriggerablePtr>::iterator iter;
  iter = this->find(triggerable);
  if (iter != this->mListeners.member().end())
  {
    if (cedar::proc::Trigger* trigger = dynamic_cast<cedar::proc::Trigger*>(triggerable))
    {
      trigger->notifyDisconnected(this_ptr);
    }
    this->mListeners.member().erase(iter);
    triggerable->noLongerTriggeredBy(this_ptr);

    lock.unlock();
    std::set<cedar::proc::Trigger*> visited;
    this->updateTriggeringOrder(visited);
  }
}

void cedar::proc::Trigger::notifyConnected(cedar::proc::TriggerPtr /* trigger */)
{
}

void cedar::proc::Trigger::notifyDisconnected(cedar::proc::TriggerPtr /* trigger */)
{
}

std::vector<cedar::proc::TriggerablePtr>::iterator cedar::proc::Trigger::find(cedar::proc::Triggerable* triggerable)
{
  for (auto iter = this->mListeners.member().begin(); iter != this->mListeners.member().end(); ++iter)
  {
    if (iter->get() == triggerable)
    {
      return iter;
    }
  }
  return this->mListeners.member().end();
}

std::vector<cedar::proc::TriggerablePtr>::iterator cedar::proc::Trigger::find(cedar::proc::TriggerablePtr triggerable)
{
  return std::find(this->mListeners.member().begin(), this->mListeners.member().end(), triggerable);
}

std::vector<cedar::proc::TriggerablePtr>::const_iterator
  cedar::proc::Trigger::find(cedar::proc::TriggerablePtr step) const
{
  return std::find(this->mListeners.member().begin(), this->mListeners.member().end(), step);
}

const std::vector<cedar::proc::TriggerablePtr>& cedar::proc::Trigger::getListeners() const
{
  return this->mListeners.member();
}

void cedar::proc::Trigger::writeConfiguration(cedar::aux::ConfigurationNode& node)
{
  this->cedar::aux::Configurable::writeConfiguration(node);

  QReadLocker lock(this->mListeners.getLockPtr());
  cedar::aux::ConfigurationNode listeners;
  for (cedar::proc::TriggerablePtr triggerable : this->mListeners.member())
  {
    cedar::aux::ConfigurationNode listener(boost::dynamic_pointer_cast<cedar::proc::Element>(triggerable)->getName());
    listeners.push_back(cedar::aux::ConfigurationNode::value_type("", listener));
  }
  if (!listeners.empty())
  {
    node.add_child("listeners", listeners);
  }
}

