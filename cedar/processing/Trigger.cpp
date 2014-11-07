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
#include "cedar/processing/Group.h"
#include "cedar/processing/ElementDeclaration.h"
#include "cedar/processing/DataConnection.h"
#include "cedar/processing/DeclarationRegistry.h"
#include "cedar/processing/sources/GroupSource.h"
#include "cedar/processing/sinks/GroupSink.h"
#include "cedar/auxiliaries/GraphTemplate.h"
#include "cedar/auxiliaries/Log.h"
#include "cedar/auxiliaries/stringFunctions.h"

// SYSTEM INCLUDES
#include <QReadLocker>
#include <QWriteLocker>
#include <algorithm>

// for debugging
#include "cedar/auxiliaries/NamedConfigurable.h"

//#define DEBUG_TRIGGER_TREE_EXPLORATION
//#define DEBUG_TRIGGERING


//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::proc::Trigger::Trigger(const std::string& name, bool isLooped)
:
Triggerable(isLooped),
mpOwner(nullptr)
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

std::string nameTriggerable(cedar::proc::Triggerable* toName)
{
  if (auto named = dynamic_cast<cedar::aux::NamedConfigurable*>(toName))
  {
    return named->getName();
  }
  else
  {
    return std::string("unnamed triggerable");
  }
}

std::string nameTriggerable(cedar::proc::TriggerablePtr toName)
{
  return nameTriggerable(toName.get());
}

std::string nameTrigger(cedar::proc::Trigger* trigger)
{
  std::string str;

  if (auto named = dynamic_cast<cedar::aux::NamedConfigurable*>(trigger))
  {
    str += named->getName();
  }
  str += " (";
  str += cedar::aux::toString(trigger);
  str += ")";
  if (trigger->getOwner())
  {
    str += " owned by ";
    if (auto named = dynamic_cast<cedar::aux::NamedConfigurable*>(trigger->getOwner()))
    {
      str += named->getName();
    }
    str += " (";
    str += cedar::aux::toString(trigger->getOwner());
    str += ")";
  }
  return str;
}

std::string nameTrigger(cedar::proc::TriggerPtr trigger)
{
  return nameTrigger(trigger.get());
}

std::map<unsigned int, std::set<cedar::proc::TriggerablePtr>> cedar::proc::Trigger::getTriggeringOrder() const
{
  std::map<unsigned int, std::set<cedar::proc::TriggerablePtr>> copy;

  QReadLocker locker(this->mTriggeringOrder.getLockPtr());
  copy = this->mTriggeringOrder.member();
  return copy;
}


void cedar::proc::Trigger::exploreSink
     (
       cedar::proc::TriggerablePtr source,
       cedar::aux::GraphTemplate<cedar::proc::TriggerablePtr>::NodePtr sourceNode,
       cedar::proc::sinks::GroupSinkPtr startSink,
       cedar::aux::GraphTemplate<cedar::proc::TriggerablePtr>& graph,
       std::vector<cedar::proc::TriggerablePtr>& to_explore
     )
{
#ifdef DEBUG_TRIGGER_TREE_EXPLORATION
/* DEBUG_TRIGGER_TREE_EXPLORATION */  std::cout << "+ exploreSink called for " << nameTriggerable(source.get()) << std::endl;
#endif
  auto listener_group = startSink->getGroup();
  if (!listener_group)
  {
#ifdef DEBUG_TRIGGER_TREE_EXPLORATION
/* DEBUG_TRIGGER_TREE_EXPLORATION */ std::cout << "  + no listener group found." << std::endl;
#endif
    // this can happen during deletion of a group; thus, just ignore the step
    return;
  }

  auto output_slot = listener_group->getOutputSlot(startSink->getName());
  CEDAR_ASSERT(output_slot);

  auto parent_group = listener_group->getGroup();
  if (!parent_group)
  {
#ifdef DEBUG_TRIGGER_TREE_EXPLORATION
/* DEBUG_TRIGGER_TREE_EXPLORATION */ std::cout << "  + no parent group found." << std::endl;
#endif
    // this can happen during deletion of a group; thus, just ignore the step
    return;
  }

  std::vector<cedar::proc::DataConnectionPtr> connections;
  parent_group->getDataConnectionsFrom(listener_group, output_slot->getName(), connections);

#ifdef DEBUG_TRIGGER_TREE_EXPLORATION
/* DEBUG_TRIGGER_TREE_EXPLORATION */ std::cout << "  + exploring " << connections.size() << " connections" << std::endl;
#endif

  for (auto connection : connections)
  {
    auto target_slot = connection->getTarget();
    auto target = boost::dynamic_pointer_cast<cedar::proc::Triggerable>(target_slot->getParentPtr()->shared_from_this());
    CEDAR_ASSERT(target);

    if (auto sink = boost::dynamic_pointer_cast<cedar::proc::sinks::GroupSink>(target))
    {
      // if the target is a group sink, we need to explore further
      this->exploreSink(source, sourceNode, sink, graph, to_explore);
    }
    else if (auto group = boost::dynamic_pointer_cast<cedar::proc::Group>(target))
    {
      // if the target is a group, we need to explore its inputs
      this->exploreGroupTarget(listener_group, group, sourceNode, graph, to_explore);
    }
    else
    {
      if (target->isLooped())
      {
        continue;
      }

      if (!graph.hasNodeForPayload(target))
      {
        graph.addNodeForPayload(target);
        to_explore.push_back(target);
      }

      auto target_node = graph.getNodeByPayload(target);
      if (!graph.edgeExists(sourceNode, target_node))
      {
        graph.addEdge(sourceNode, target_node);
      }
    }
  }
}


void cedar::proc::Trigger::exploreGroupTarget
     (
       cedar::proc::TriggerablePtr source,
       cedar::proc::GroupPtr targetGroup,
       cedar::aux::GraphTemplate<cedar::proc::TriggerablePtr>::NodePtr sourceNode,
       cedar::aux::GraphTemplate<cedar::proc::TriggerablePtr>& graph,
       std::vector<cedar::proc::TriggerablePtr>& to_explore
     )
{
#ifdef DEBUG_TRIGGER_TREE_EXPLORATION
/* DEBUG_TRIGGER_TREE_EXPLORATION */ std::cout << "+ exploreGroupTarget called for " << nameTriggerable(source.get()) << std::endl;
/* DEBUG_TRIGGER_TREE_EXPLORATION */ std::cout << "+   targetGroup is " << targetGroup->getName() << std::endl;
#endif
  auto source_connectable = boost::dynamic_pointer_cast<cedar::proc::Connectable>(source);
  if (!source_connectable)
  {
#ifdef DEBUG_TRIGGER_TREE_EXPLORATION
/* DEBUG_TRIGGER_TREE_EXPLORATION */ std::cout << "+ returning: source is not connectable. " << std::endl;
#endif
    return;
  }
  auto parent_group = targetGroup->getGroup();
  if (!parent_group)
  {
#ifdef DEBUG_TRIGGER_TREE_EXPLORATION
/* DEBUG_TRIGGER_TREE_EXPLORATION */ std::cout << "+ returning: no parent group. " << std::endl;
#endif
    // this can happen during deletion of a group; thus, just ignore the step
    return;
  }

  for (auto slot : source_connectable->getOrderedDataSlots(cedar::proc::DataRole::OUTPUT))
  {
#ifdef DEBUG_TRIGGER_TREE_EXPLORATION
/* DEBUG_TRIGGER_TREE_EXPLORATION */ std::cout << "+ treating slot " << slot->getName() << std::endl;
#endif
    std::vector<cedar::proc::DataConnectionPtr> connections;
    parent_group->getDataConnectionsFrom(source_connectable, slot->getName(), connections);

    for (auto connection : connections)
    {
      auto target_slot = connection->getTarget();
      if (target_slot->getParentPtr() != targetGroup.get())
      {
        continue;
      }

      auto group_source = targetGroup->getElement<cedar::proc::sources::GroupSource>(target_slot->getName());

      if (!graph.hasNodeForPayload(group_source))
      {
        graph.addNodeForPayload(group_source);
        to_explore.push_back(group_source);
      }

      auto listener_node = graph.getNodeByPayload(group_source);
      if (!graph.edgeExists(sourceNode, listener_node))
      {
        graph.addEdge(sourceNode, listener_node);
      }
    }
  }
}

void cedar::proc::Trigger::explore
     (
       cedar::proc::TriggerablePtr source,
       cedar::proc::TriggerPtr trigger,
       cedar::aux::GraphTemplate<cedar::proc::TriggerablePtr>& graph,
       std::vector<cedar::proc::TriggerablePtr>& to_explore,
       bool sourceIsTrigger,
       std::set<cedar::proc::TriggerablePtr>& explored
     )
{
#ifdef DEBUG_TRIGGER_TREE_EXPLORATION
/* DEBUG_TRIGGER_TREE_EXPLORATION */ std::cout << "+ explore called for " << nameTriggerable(source.get()) << std::endl;
#endif

  if (explored.find(source) != explored.end())
  {
#ifdef DEBUG_TRIGGER_TREE_EXPLORATION
/* DEBUG_TRIGGER_TREE_EXPLORATION */ std::cout << "- leaving explore for " << nameTriggerable(source.get()) << " (already visited)" << std::endl;
#endif
    // triggerable was already treated, skip it.
    return;
  }
  else
  {
    explored.insert(source);
  }

  cedar::aux::GraphTemplate<cedar::proc::TriggerablePtr>::NodePtr source_node;
  if (sourceIsTrigger)
  {
    if (!graph.hasNodeForPayload(trigger))
    {
      graph.addNodeForPayload(trigger);
    }

    source_node = graph.getNodeByPayload(trigger);
  }
  else
  {
    if (!graph.hasNodeForPayload(source))
    {
      graph.addNodeForPayload(source);
    }

    source_node = graph.getNodeByPayload(source);
  }

  QReadLocker lock(trigger->mListeners.getLockPtr());
  // append all listeners of this step; they all have a distance of one, because they follow this step directly
  for (auto listener : trigger->mListeners.member())
  {
    // Special case: listener is a group. Groups are ignored in building the triggering graph. Rather, a direct
    // connection is made to the GroupSources inside the group in order to make processing inside the group more
    // efficient.
    // If this were not done, each trigger signal to a group would trigger all the steps inside the group, potentially
    // leading to lots of redundant computation.
    if (auto listener_group = boost::dynamic_pointer_cast<cedar::proc::Group>(listener))
    {
      this->exploreGroupTarget(source, listener_group, source_node, graph, to_explore);
    }
    // Special case: listener is a group sink. Analogous to the case above, group sinks lead to a trigger connection to
    // the steps outside of the group.
    else if (auto listener_sink = boost::dynamic_pointer_cast<cedar::proc::sinks::GroupSink>(listener))
    {
      this->exploreSink(source, source_node, listener_sink, graph, to_explore);
    }
    else
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
      if (!graph.edgeExists(source_node, listener_node))
      {
        graph.addEdge(source_node, listener_node);
      }
    }
  }
}

void cedar::proc::Trigger::buildTriggerGraph(cedar::aux::GraphTemplate<cedar::proc::TriggerablePtr>& graph)
{
#ifdef DEBUG_TRIGGER_TREE_EXPLORATION
/* DEBUG_TRIGGER_TREE_EXPLORATION */ std::cout << " U " << nameTrigger(this) << "->buildTriggerGraph()" << std::endl;
#endif
  // root node corresponding to this trigger
  std::vector<cedar::proc::TriggerablePtr> to_explore;
  std::set<cedar::proc::TriggerablePtr> explored;

  auto this_ptr = boost::static_pointer_cast<cedar::proc::Trigger>(this->shared_from_this());

  // This is a workaround for Triggerable not being able to use shared_from_this. Element has it, Triggerable does not.
  if (this->mpOwner != nullptr)
  {
    auto element = dynamic_cast<cedar::proc::Element*>(this->mpOwner);
    CEDAR_DEBUG_ASSERT(element != nullptr);
    auto owner_ptr = boost::dynamic_pointer_cast<cedar::proc::Triggerable>(element->shared_from_this());
    CEDAR_DEBUG_ASSERT(owner_ptr);

    this->explore(owner_ptr, this_ptr, graph, to_explore, true, explored);
  }
  else
  {
    this->explore(this_ptr, this_ptr, graph, to_explore, true, explored);
  }

  while (!to_explore.empty())
  {
    cedar::proc::TriggerablePtr source = to_explore.back();
    to_explore.pop_back();

    // get the done trigger
    //!@todo Is getting the finished trigger thread-safe?
    cedar::proc::TriggerPtr done_trigger = source->getFinishedTrigger();

    this->explore(source, done_trigger, graph, to_explore, false, explored);
  }

#ifdef DEBUG_TRIGGER_TREE_EXPLORATION
/* DEBUG_TRIGGER_TREE_EXPLORATION */ std::cout << " U done " << nameTrigger(this) << "->buildTriggerGraph()" << std::endl;
#endif
}

void cedar::proc::Trigger::updateTriggeringOrder(std::set<cedar::proc::Trigger*>& visited, bool recurseUp, bool recurseDown)
{
  if (this->mpOwner != nullptr)
  {
    auto connectable = dynamic_cast<cedar::proc::Connectable*>(this->mpOwner);
    if (connectable != nullptr)
    {
      auto group = connectable->getGroup();
      if (group && group->holdTriggerChainUpdates())
      {
        return;
      }
    }
  }
  //!@todo Here and in buildTriggerGraph, there are a lot of dynamic casts. Can this be solved better with a bunch of virtual functions?
#ifdef DEBUG_TRIGGER_TREE_EXPLORATION
/* DEBUG_TRIGGER_TREE_EXPLORATION */ std::cout << " U Updating triggering order of " << nameTrigger(this) << std::endl;
#endif

  // check if this trigger was already visited during the current wave of graph generation
  if (visited.find(this) != visited.end())
  {
#ifdef DEBUG_TRIGGER_TREE_EXPLORATION
/* DEBUG_TRIGGER_TREE_EXPLORATION */ std::cout << " U already visited: " << nameTrigger(this) << std::endl;
#endif
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

      // also, if the owner of this trigger is a group, update the triggering order of all sinks
      if (auto group_source = dynamic_cast<cedar::proc::sources::GroupSource*>(this->mpOwner))
      {
        // get all connections leading to the group input that corresponds to the source
        auto group = group_source->getGroup();
        if (group) // can be a nullptr if the group is being destroyed
        {
          auto parent_group = group->getGroup();

          if (parent_group) // can be a nullptr if the group is being destroyed
          {
            std::vector<cedar::proc::DataConnectionPtr> connections;
            parent_group->getDataConnectionsTo(group, group_source->getName(), connections);

            // update the triggering order of all the steps connected to the group's input
            for (auto connection : connections)
            {
              auto source = dynamic_cast<cedar::proc::Triggerable*>(connection->getSource()->getParentPtr());
              CEDAR_DEBUG_ASSERT(source);
              source->getFinishedTrigger()->updateTriggeringOrder(visited, false, true);
            }
          }
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

    // if this trigger is owned by a group source, we also have to explore its group
    if (auto source = dynamic_cast<cedar::proc::sources::GroupSource*>(this->mpOwner))
    {
#ifdef DEBUG_TRIGGER_TREE_EXPLORATION
      /* DEBUG_TRIGGER_TREE_EXPLORATION */ std::cout << " calling updateTriggeringOrder(up=true, down=false) for group source" << std::endl;
#endif
      this->updateTriggeringOrderRecurseUpSource(source, visited);
    }

    // also, if the owner of this trigger is a group, update the triggering order of all sinks
    //!@todo This might do more than needed; in principle, only the slots sinks actually affected need to be updated
    if (auto group = dynamic_cast<cedar::proc::Group*>(this->mpOwner))
    {
      for (auto name_connector_type_pair : group->getConnectorMap())
      {
        if (!name_connector_type_pair.second) // connector is an output/sink
        {
          auto sink = boost::static_pointer_cast<cedar::proc::sinks::GroupSink>(group->getElement(name_connector_type_pair.first));
          sink->getFinishedTrigger()->updateTriggeringOrder(visited, true, false);
        }
      }
    }
  }

#ifdef DEBUG_TRIGGER_TREE_EXPLORATION
/* DEBUG_TRIGGER_TREE_EXPLORATION */ std::cout << " F Final triggering order for " << nameTrigger(this) << std::endl;
/* DEBUG_TRIGGER_TREE_EXPLORATION */  QReadLocker r_lock(this->mTriggeringOrder.getLockPtr());
/* DEBUG_TRIGGER_TREE_EXPLORATION */  for (auto distance_triggerable_set_pair : this->mTriggeringOrder.member())
/* DEBUG_TRIGGER_TREE_EXPLORATION */  {
/* DEBUG_TRIGGER_TREE_EXPLORATION */    std::cout << "Distance: " << distance_triggerable_set_pair.first << std::endl;
/* DEBUG_TRIGGER_TREE_EXPLORATION */
/* DEBUG_TRIGGER_TREE_EXPLORATION */    for (auto triggerable : distance_triggerable_set_pair.second)
/* DEBUG_TRIGGER_TREE_EXPLORATION */    {
/* DEBUG_TRIGGER_TREE_EXPLORATION */      std::cout << "  " << nameTriggerable(triggerable) << std::endl;
/* DEBUG_TRIGGER_TREE_EXPLORATION */    }
/* DEBUG_TRIGGER_TREE_EXPLORATION */  }
#endif
}

void cedar::proc::Trigger::updateTriggeringOrderRecurseUpSource(cedar::proc::sources::GroupSource* source, std::set<cedar::proc::Trigger*>& visited)
{
  auto group = source->getGroup();
  if (!group)
  {
#ifdef DEBUG_TRIGGER_TREE_EXPLORATION
/* DEBUG_TRIGGER_TREE_EXPLORATION */ std::cout << " Returning: no group set." << std::endl;
#endif

    // this can happen during construction and destruction.
    return;
  }

#ifdef DEBUG_TRIGGER_TREE_EXPLORATION
/* DEBUG_TRIGGER_TREE_EXPLORATION */ std::cout << " Iterating connections." << std::endl;
#endif
  for (auto connection : group->getInputSlot(source->getName())->getDataConnections())
  {
    auto source_triggerable = dynamic_cast<cedar::proc::Triggerable*>(connection->getSource()->getParentPtr());
    CEDAR_DEBUG_ASSERT(source_triggerable);

#ifdef DEBUG_TRIGGER_TREE_EXPLORATION
/* DEBUG_TRIGGER_TREE_EXPLORATION */ std::cout << " source_triggerable: " << nameTriggerable(source_triggerable) << std::endl;
#endif

    if (auto source_group = dynamic_cast<cedar::proc::Group*>(source_triggerable))
    {
      auto sink = source_group->getElement<cedar::proc::sinks::GroupSink>(connection->getSource()->getName());
      source_triggerable = boost::dynamic_pointer_cast<cedar::proc::Triggerable>(sink).get();
      CEDAR_DEBUG_ASSERT(source_triggerable != nullptr);
    }
    else if (auto source_group_source = dynamic_cast<cedar::proc::sources::GroupSource*>(source_triggerable))
    {
#ifdef DEBUG_TRIGGER_TREE_EXPLORATION
/* DEBUG_TRIGGER_TREE_EXPLORATION */ std::cout << " following group source" << std::endl;
#endif
      this->updateTriggeringOrderRecurseUpSource(source_group_source, visited);
      return;
    }

#ifdef DEBUG_TRIGGER_TREE_EXPLORATION
/* DEBUG_TRIGGER_TREE_EXPLORATION */ std::cout << " -> updating triggering order of " << nameTriggerable(source_triggerable) << std::endl;
#endif
    source_triggerable->updateTriggeringOrder(visited, true, false);
  }
}


void cedar::proc::Trigger::trigger(cedar::proc::ArgumentsPtr arguments)
{
  //!@todo Remove debug outputs here
  auto this_ptr = boost::static_pointer_cast<cedar::proc::Trigger>(this->shared_from_this());

  QReadLocker lock(this->mTriggeringOrder.getLockPtr());

#ifdef DEBUG_TRIGGERING
/* DEBUG_TRIGGERING */  std::cout << "> Triggering " << nameTrigger(this) << std::endl;
#endif

  for (auto order_triggerables_pair : this->mTriggeringOrder.member())
  {
    auto triggerables = order_triggerables_pair.second;
    for (cedar::proc::TriggerablePtr triggerable : triggerables)
    {
#ifdef DEBUG_TRIGGERING
/* DEBUG_TRIGGERING */ std::cout << "  > Triggering chain item " << nameTriggerable(triggerable) << std::endl;
#endif

      triggerable->onTrigger(arguments, this_ptr);

#ifdef DEBUG_TRIGGERING
/* DEBUG_TRIGGERING */ std::cout << "  < Done triggering chain item " << nameTriggerable(triggerable) << std::endl;
#endif
    }
  }
#ifdef DEBUG_TRIGGERING
/* DEBUG_TRIGGERING */ std::cout << "< Done triggering " << nameTrigger(this) << std::endl;
#endif
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

