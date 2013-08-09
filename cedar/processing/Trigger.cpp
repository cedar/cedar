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
#include "cedar/auxiliaries/Log.h"
#include "cedar/auxiliaries/stringFunctions.h"

// SYSTEM INCLUDES
#include <QReadLocker>
#include <QWriteLocker>
#include <algorithm>

// MACROS
//#define DEBUG_TRIGGERING


//----------------------------------------------------------------------------------------------------------------------
// register the trigger class
//----------------------------------------------------------------------------------------------------------------------

// for now, we don't need to use the trigger as it is not useful (and just leads to confusion)
//namespace
//{
//  bool declare()
//  {
//    using cedar::proc::ElementDeclarationPtr;
//    using cedar::proc::ElementDeclarationTemplate;
//
//    ElementDeclarationPtr trigger_declaration
//    (
//      new ElementDeclarationTemplate<cedar::proc::Trigger>
//      (
//        "Triggers",
//        "cedar.processing.Trigger"
//      )
//    );
//    trigger_declaration->setIconPath(":/triggers/trigger.svg");
//    cedar::aux::Singleton<cedar::proc::DeclarationRegistry>::getInstance()->declareClass(trigger_declaration);
//
//    return true;
//  }
//
//  bool declared = declare();
//}

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

  QReadLocker lock(&mpListenersLock);

  for (size_t i = 0; i < this->mListeners.size(); ++i)
  {
    this->mListeners.at(i)->noLongerTriggeredBy(boost::static_pointer_cast<cedar::proc::Trigger>(this->shared_from_this()));
  }

  this->mListeners.clear();
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::proc::Trigger::updateTriggeringOrder(bool recurseUp, bool recurseDown)
{
  //!@todo Can this take pre-computed results from other triggers into account? Otherwise, multiple triggers might calculate the same thing over and over again.
  QReadLocker lock(&mpListenersLock);
  std::map<cedar::proc::TriggerablePtr, unsigned int> distance_map;

  std::vector<cedar::proc::TriggerablePtr> to_explore;

  // append all listeners of this step; they all have a distance of one, because they follow this step directly
  for (auto iter = this->mListeners.begin(); iter != this->mListeners.end(); ++iter)
  {
    auto listener = *iter;
    to_explore.push_back(listener);
    distance_map[listener] = 1;
  }

  while (!to_explore.empty())
  {
    // take out first element still to be explored
    cedar::proc::TriggerablePtr current = to_explore.back();
    to_explore.pop_back();
    auto current_dist_iter = distance_map.find(current);
    CEDAR_DEBUG_ASSERT(current_dist_iter != distance_map.end());
    auto current_distance = current_dist_iter->second;

    // iterate over all its done-listeners
    cedar::proc::TriggerPtr done = current->getFinishedTrigger();

    for (auto iter = done->mListeners.begin(); iter != done->mListeners.end(); ++iter)
    {
      auto done_listening = *iter;
      auto dist_iter = distance_map.find(done_listening);

      if (dist_iter == distance_map.end())
      {
        distance_map[done_listening] = current_distance + 1;
      }
      else
      {
        dist_iter->second = std::max(current_distance + 1, dist_iter->second);
      }

      //!@todo This might run into an infinite loop, but only if there is a cycle in the triggering anyway
      to_explore.push_back(done_listening);
    }
  }

  // we (should) now have a map assigning a distance to all triggerables triggered by this trigger
  // now we need to transform it to a usable structures
  this->mTriggeringOrder.clear();

  for (auto dist_iter = distance_map.begin(); dist_iter != distance_map.end(); ++dist_iter)
  {
    auto triggerable = dist_iter->first;
    auto distance = dist_iter->second;
    auto iter = this->mTriggeringOrder.find(distance);
    if (iter == this->mTriggeringOrder.end())
    {
      this->mTriggeringOrder[distance] = std::set<cedar::proc::TriggerablePtr>();
      iter = this->mTriggeringOrder.find(distance);
      CEDAR_DEBUG_ASSERT(iter != this->mTriggeringOrder.end());
    }

    iter->second.insert(triggerable);
  }

  // recursively update the triggering order for all triggers triggered by this one
  if (recurseDown)
  {
    for (auto iter = this->mListeners.begin(); iter != this->mListeners.end(); ++iter)
    {
      auto listener = *iter;

      cedar::proc::TriggerPtr done = listener->getFinishedTrigger();
      done->updateTriggeringOrder(false, true);
    }
  }

  // recursively update the triggering order for all triggers by which this one is triggered
  if (recurseUp && this->mpOwner != NULL)
  {
    for (auto iter = this->mpOwner->mTriggersListenedTo.begin(); iter != this->mpOwner->mTriggersListenedTo.end(); ++iter)
    {
      auto trigger = iter->lock();
      CEDAR_ASSERT(trigger);
      trigger->updateTriggeringOrder(true, false);
    }
  }
}

void cedar::proc::Trigger::waitForProcessing()
{
  QReadLocker lock(&mpListenersLock);
  for (size_t i = 0; i < this->mListeners.size(); ++i)
  {
    this->mListeners.at(i)->waitForProcessing();
  }
}

void cedar::proc::Trigger::trigger(cedar::proc::ArgumentsPtr arguments)
{
  auto this_ptr = boost::static_pointer_cast<cedar::proc::Trigger>(this->shared_from_this());
  QReadLocker lock(&mpListenersLock);

  for (auto iter = this->mTriggeringOrder.begin(); iter != this->mTriggeringOrder.end(); ++iter)
  {
    auto triggerables = iter->second;
    for (auto triggerable_iter = triggerables.begin(); triggerable_iter != triggerables.end(); ++triggerable_iter)
    {
      cedar::proc::TriggerablePtr triggerable = *triggerable_iter;

      triggerable->onTrigger(arguments, this_ptr);
    }
  }
//
//  for (size_t i = 0; i < this->mListeners.size(); ++i)
//  {
//#ifdef DEBUG_TRIGGERING
//        std::cout << "Trigger " << this->getName() << " triggers " << this->mListeners.at(i)->getName() << std::endl;
//#endif // DEBUG_TRIGGERING
//    this->mListeners.at(i)->onTrigger
//                            (
//                              arguments,
//                              boost::static_pointer_cast<cedar::proc::Trigger>(this->shared_from_this())
//                            );
//  }
}

void cedar::proc::Trigger::onTrigger(cedar::proc::ArgumentsPtr, cedar::proc::TriggerPtr)
{
}

void cedar::proc::Trigger::addListener(cedar::proc::TriggerablePtr triggerable)
{
  auto this_ptr = boost::static_pointer_cast<cedar::proc::Trigger>(this->shared_from_this());

  QWriteLocker lock(&mpListenersLock);
  std::vector<cedar::proc::TriggerablePtr>::iterator iter;
  iter = this->find(triggerable);
  if (iter == this->mListeners.end())
  {
    this->mListeners.push_back(triggerable);
    triggerable->triggeredBy(this_ptr);
    if (cedar::proc::TriggerPtr trigger = boost::dynamic_pointer_cast<cedar::proc::Trigger>(triggerable))
    {
      trigger->notifyConnected(this_ptr);
    }

    lock.unlock();
    this->updateTriggeringOrder();
  }
}

bool cedar::proc::Trigger::isListener(cedar::proc::TriggerablePtr triggerable) const
{
  QReadLocker lock(&mpListenersLock);
  return this->find(triggerable) != this->mListeners.end();
}

void cedar::proc::Trigger::removeListener(cedar::proc::TriggerablePtr triggerable)
{
  this->removeListener(triggerable.get());
}

void cedar::proc::Trigger::removeListener(cedar::proc::Triggerable* triggerable)
{
  auto this_ptr = boost::static_pointer_cast<cedar::proc::Trigger>(this->shared_from_this());

  QWriteLocker lock(&mpListenersLock);
  std::vector<cedar::proc::TriggerablePtr>::iterator iter;
  iter = this->find(triggerable);
  if (iter != this->mListeners.end())
  {
    if (cedar::proc::Trigger* trigger = dynamic_cast<cedar::proc::Trigger*>(triggerable))
    {
      trigger->notifyDisconnected(this_ptr);
    }
    this->mListeners.erase(iter);
    triggerable->noLongerTriggeredBy(this_ptr);

    lock.unlock();
    this->updateTriggeringOrder();
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
  for (auto iter = this->mListeners.begin(); iter != this->mListeners.end(); ++iter)
  {
    if (iter->get() == triggerable)
    {
      return iter;
    }
  }
  return this->mListeners.end();
}

std::vector<cedar::proc::TriggerablePtr>::iterator cedar::proc::Trigger::find(cedar::proc::TriggerablePtr triggerable)
{
  return std::find(this->mListeners.begin(), this->mListeners.end(), triggerable);
}

std::vector<cedar::proc::TriggerablePtr>::const_iterator
  cedar::proc::Trigger::find(cedar::proc::TriggerablePtr step) const
{
  return std::find(this->mListeners.begin(), this->mListeners.end(), step);
}

const std::vector<cedar::proc::TriggerablePtr>& cedar::proc::Trigger::getListeners() const
{
  return this->mListeners;
}

void cedar::proc::Trigger::writeConfiguration(cedar::aux::ConfigurationNode& node)
{
  this->cedar::aux::Configurable::writeConfiguration(node);

  QReadLocker lock(&mpListenersLock);
  cedar::aux::ConfigurationNode listeners;
  for (size_t i = 0; i < this->mListeners.size(); ++i)
  {
    cedar::proc::TriggerablePtr& step = this->mListeners.at(i);
    cedar::aux::ConfigurationNode listener(boost::dynamic_pointer_cast<cedar::proc::Element>(step)->getName());
    listeners.push_back(cedar::aux::ConfigurationNode::value_type("", listener));
  }
  if (!listeners.empty())
    node.add_child("listeners", listeners);
}

