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

// LOCAL INCLUDES
#include "processing/Trigger.h"
#include "processing/Step.h"
#include "processing/Manager.h"

// PROJECT INCLUDES

// SYSTEM INCLUDES
#include <algorithm>

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::proc::Trigger::Trigger(const std::string& name)
{
  this->setName(name);
}

cedar::proc::Trigger::~Trigger()
{
#ifdef DEBUG
  std::cout << "> freeing data (Trigger)" << std::endl;
#endif
  this->mListeners.clear();
  this->mTriggers.clear();
}



//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::proc::Trigger::trigger(cedar::proc::ArgumentsPtr arguments)
{
  for (size_t i = 0; i < this->mListeners.size(); ++i)
  {
    this->mListeners.at(i)->setNextArguments(arguments);
    this->mListeners.at(i)->onTrigger();
  }
  for (size_t i = 0; i < this->mTriggers.size(); ++i)
  {
    this->mTriggers.at(i)->onTrigger(this);
  }
}

void cedar::proc::Trigger::onTrigger(Trigger*)
{
}

void cedar::proc::Trigger::addListener(cedar::proc::StepPtr step)
{
  std::vector<cedar::proc::StepPtr>::iterator iter;
  iter = this->find(step);
  if (iter == this->mListeners.end())
  {
    this->mListeners.push_back(step);
  }
}

bool cedar::proc::Trigger::isListener(cedar::proc::StepPtr step)
{
  return this->find(step) != this->mListeners.end();
}

bool cedar::proc::Trigger::isListener(cedar::proc::TriggerPtr trigger)
{
  return this->find(trigger) != this->mTriggers.end();
}

void cedar::proc::Trigger::addTrigger(cedar::proc::TriggerPtr trigger)
{
  std::vector<cedar::proc::TriggerPtr>::iterator iter;
  iter = this->find(trigger);
  if (iter == this->mTriggers.end())
  {
    this->mTriggers.push_back(trigger);
    trigger->notifyConnected(this);
  }
}

void cedar::proc::Trigger::removeListener(cedar::proc::StepPtr step)
{
  std::vector<cedar::proc::StepPtr>::iterator iter;
  iter = this->find(step);
  if (iter != this->mListeners.end())
  {
    this->mListeners.erase(iter);
  }
}

void cedar::proc::Trigger::removeTrigger(cedar::proc::TriggerPtr trigger)
{
  std::vector<cedar::proc::TriggerPtr>::iterator iter;
  iter = this->find(trigger);
  if (iter != this->mTriggers.end())
  {
    this->mTriggers.erase(iter);
    trigger->notifyDisconnected(this);
  }
}

void cedar::proc::Trigger::notifyConnected(cedar::proc::Trigger* /* trigger */)
{
}

void cedar::proc::Trigger::notifyDisconnected(cedar::proc::Trigger* /* trigger */)
{
}

std::vector<cedar::proc::StepPtr>::iterator
  cedar::proc::Trigger::find(cedar::proc::StepPtr step)
{
  return std::find(this->mListeners.begin(), this->mListeners.end(), step);
}

std::vector<cedar::proc::TriggerPtr>::iterator cedar::proc::Trigger::find(cedar::proc::TriggerPtr step)
{
  return std::find(this->mTriggers.begin(), this->mTriggers.end(), step);
}

const std::vector<cedar::proc::StepPtr>& cedar::proc::Trigger::getListeners() const
{
  return this->mListeners;
}

const std::vector<cedar::proc::TriggerPtr>& cedar::proc::Trigger::getTriggerListeners() const
{
  return this->mTriggers;
}

void cedar::proc::Trigger::saveConfiguration(cedar::aux::ConfigurationNode& node)
{
  this->cedar::aux::Configurable::saveConfiguration(node);

  cedar::aux::ConfigurationNode listeners;
  for (size_t i = 0; i < this->mListeners.size(); ++i)
  {
    cedar::proc::StepPtr& step = this->mListeners.at(i);
    cedar::aux::ConfigurationNode listener(step->getName());
    listeners.push_back(cedar::aux::ConfigurationNode::value_type("", listener));
  }
  if (!listeners.empty())
    node.add_child("listeners", listeners);

  cedar::aux::ConfigurationNode trigger_listeners;
  for (size_t i = 0; i < this->mTriggers.size(); ++i)
  {
    cedar::proc::TriggerPtr& trigger = this->mTriggers.at(i);
    cedar::aux::ConfigurationNode trigger_listener(trigger->getName());
    trigger_listeners.push_back(cedar::aux::ConfigurationNode::value_type("", trigger_listener));
  }
  if (!trigger_listeners.empty())
    node.add_child("triggerListeners", trigger_listeners);
}

void cedar::proc::Trigger::readConfiguration(const cedar::aux::ConfigurationNode& node)
{
  this->cedar::aux::Configurable::readConfiguration(node);
  // listeners
  try
  {
    const cedar::aux::ConfigurationNode& listeners = node.get_child("listeners");

    for (cedar::aux::ConfigurationNode::const_iterator iter = listeners.begin();
        iter != listeners.end();
        ++iter)
    {
      std::string listener_name = iter->second.data();

#ifdef DEBUG_FILE_READING
  std::cout << "Adding listener " << listener_name << std::endl;
#endif // DEBUG_FILE_READING

      cedar::proc::StepPtr step = cedar::proc::Manager::getInstance().steps().get(listener_name);
      this->addListener(step);
    }
  }
  catch (const boost::property_tree::ptree_bad_path&)
  {
    // no listeners declared -- this is ok.
  }

  // trigger listeners
  try
  {
    const cedar::aux::ConfigurationNode& trigger_listeners = node.get_child("triggerListeners");

    for (cedar::aux::ConfigurationNode::const_iterator iter = trigger_listeners.begin();
        iter != trigger_listeners.end();
        ++iter)
    {
      std::string listener_name = iter->second.data();

#ifdef DEBUG_FILE_READING
  std::cout << "Adding trigger listener " << listener_name << std::endl;
#endif // DEBUG_FILE_READING

      cedar::proc::TriggerPtr trigger = cedar::proc::Manager::getInstance().triggers().get(listener_name);
      this->addTrigger(trigger);
    }
  }
  catch (const boost::property_tree::ptree_bad_path&)
  {
    // no trigger listeners declared -- this is ok.
  }
}

