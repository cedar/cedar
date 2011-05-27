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
#include "auxiliaries/computation/Trigger.h"
#include "auxiliaries/computation/ProcessingStep.h"

// PROJECT INCLUDES

// SYSTEM INCLUDES
#include <algorithm>

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::aux::comp::Trigger::Trigger()
{
}

cedar::aux::comp::Trigger::~Trigger()
{
  this->mListeners.clear();
  this->mTriggers.clear();
}



//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::aux::comp::Trigger::trigger()
{
  for (size_t i = 0; i < this->mListeners.size(); ++i)
  {
    this->mListeners.at(i)->onTrigger();
  }
  for (size_t i = 0; i < this->mTriggers.size(); ++i)
  {
    this->mTriggers.at(i)->onTrigger(this);
  }
}

void cedar::aux::comp::Trigger::addListener(cedar::aux::comp::ProcessingStepPtr step)
{
  std::vector<cedar::aux::comp::ProcessingStepPtr>::iterator iter;
  iter = this->find(step);
  if (iter == this->mListeners.end())
  {
    this->mListeners.push_back(step);
  }
}

void cedar::aux::comp::Trigger::addTrigger(cedar::aux::comp::TriggerPtr trigger)
{
  std::vector<cedar::aux::comp::TriggerPtr>::iterator iter;
  iter = this->find(trigger);
  if (iter == this->mTriggers.end())
  {
    this->mTriggers.push_back(trigger);
    trigger->notifyConnected(this);
  }
}

void cedar::aux::comp::Trigger::notifyConnected(cedar::aux::comp::Trigger* trigger)
{
}

void cedar::aux::comp::Trigger::removeListener(cedar::aux::comp::ProcessingStepPtr step)
{
  std::vector<cedar::aux::comp::ProcessingStepPtr>::iterator iter;
  iter = this->find(step);
  if (iter != this->mListeners.end())
  {
    this->mListeners.erase(iter);
  }
}

std::vector<cedar::aux::comp::ProcessingStepPtr>::iterator
  cedar::aux::comp::Trigger::find(cedar::aux::comp::ProcessingStepPtr step)
{
  return std::find(this->mListeners.begin(), this->mListeners.end(), step);
}

std::vector<cedar::aux::comp::TriggerPtr>::iterator cedar::aux::comp::Trigger::find(cedar::aux::comp::TriggerPtr step)
{
  return std::find(this->mTriggers.begin(), this->mTriggers.end(), step);
}

const std::vector<cedar::aux::comp::ProcessingStepPtr>& cedar::aux::comp::Trigger::getListeners() const
{
  return this->mListeners;
}

