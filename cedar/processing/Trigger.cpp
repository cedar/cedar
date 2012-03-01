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

// CEDAR INCLUDES
#include "cedar/processing/Trigger.h"
#include "cedar/processing/Step.h"
#include "cedar/processing/Manager.h"
#include "cedar/processing/Element.h"
#include "cedar/auxiliaries/Log.h"
#include "cedar/auxiliaries/stringFunctions.h"

// SYSTEM INCLUDES
#include <algorithm>

// MACROS
//#define DEBUG_TRIGGERING

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::proc::Trigger::Trigger(const std::string& name, bool isLooped)
:
Triggerable(isLooped)
{
  cedar::aux::LogSingleton::getInstance()->allocating(this);

  this->setName(name);
}

cedar::proc::Trigger::~Trigger()
{
  cedar::aux::LogSingleton::getInstance()->freeing(this);

  this->mListeners.clear();
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------
void cedar::proc::Trigger::trigger(cedar::proc::ArgumentsPtr arguments)
{
  for (size_t i = 0; i < this->mListeners.size(); ++i)
  {
    // If the arguments can be set, trigger the step.
    //!@todo For dynamics, this can mean that some step times are discarded rather than accumulated.
    //!@todo cedar::proc::Step::setNextArguments should take a const pointer.
    if (cedar::proc::StepPtr step = boost::shared_dynamic_cast<cedar::proc::Step>(this->mListeners.at(i)))
    {
      if (step->setNextArguments(arguments))
      {
#ifdef DEBUG_TRIGGERING
        std::cout << "Trigger " << this->getName() << " triggers " << this->mListeners.at(i)->getName() << std::endl;
#endif // DEBUG_TRIGGERING
        this->mListeners.at(i)->onTrigger(boost::shared_static_cast<cedar::proc::Trigger>(this->shared_from_this()));
      }
    }
    else
    {
      this->mListeners.at(i)->onTrigger(boost::shared_static_cast<cedar::proc::Trigger>(this->shared_from_this()));
    }
  }
}

void cedar::proc::Trigger::onTrigger(cedar::proc::TriggerPtr /* pSender */)
{
}

void cedar::proc::Trigger::addListener(cedar::proc::TriggerablePtr step)
{
  std::vector<cedar::proc::TriggerablePtr>::iterator iter;
  iter = this->find(step);
  if (iter == this->mListeners.end())
  {
    this->mListeners.push_back(step);
    if (cedar::proc::TriggerPtr trigger = boost::shared_dynamic_cast<cedar::proc::Trigger>(step))
    {
      trigger->notifyConnected(boost::shared_static_cast<cedar::proc::Trigger>(this->shared_from_this()));
    }
  }
}

bool cedar::proc::Trigger::isListener(cedar::proc::TriggerablePtr step) const
{
  return this->find(step) != this->mListeners.end();
}

void cedar::proc::Trigger::removeListener(cedar::proc::TriggerablePtr step)
{
  std::vector<cedar::proc::TriggerablePtr>::iterator iter;
  iter = this->find(step);
  if (iter != this->mListeners.end())
  {
    if (cedar::proc::TriggerPtr trigger = boost::shared_dynamic_cast<cedar::proc::Trigger>(step))
    {
      trigger->notifyDisconnected(boost::shared_static_cast<cedar::proc::Trigger>(this->shared_from_this()));
    }
    this->mListeners.erase(iter);
  }
}

void cedar::proc::Trigger::notifyConnected(cedar::proc::TriggerPtr /* trigger */)
{
}

void cedar::proc::Trigger::notifyDisconnected(cedar::proc::TriggerPtr /* trigger */)
{
}

std::vector<cedar::proc::TriggerablePtr>::iterator cedar::proc::Trigger::find(cedar::proc::TriggerablePtr step)
{
  return std::find(this->mListeners.begin(), this->mListeners.end(), step);
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

  cedar::aux::ConfigurationNode listeners;
  for (size_t i = 0; i < this->mListeners.size(); ++i)
  {
    cedar::proc::TriggerablePtr& step = this->mListeners.at(i);
    cedar::aux::ConfigurationNode listener(boost::shared_dynamic_cast<cedar::proc::Element>(step)->getName());
    listeners.push_back(cedar::aux::ConfigurationNode::value_type("", listener));
  }
  if (!listeners.empty())
    node.add_child("listeners", listeners);
}

