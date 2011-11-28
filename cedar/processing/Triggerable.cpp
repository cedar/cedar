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

    File:        Triggerable.cpp

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2011 11 16

    Description:

    Credits:

======================================================================================================================*/

// LOCAL INCLUDES
#include "cedar/processing/Triggerable.h"
#include "cedar/processing/Trigger.h"

// PROJECT INCLUDES
#include "cedar/auxiliaries/assert.h"
// SYSTEM INCLUDES

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------
cedar::proc::Triggerable::Triggerable(bool isLooped)
:
mIsLooped(isLooped),
mState(cedar::proc::Triggerable::STATE_NONE)
{
}

cedar::proc::Triggerable::~Triggerable()
{
  // empty default implementation
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------
void cedar::proc::Triggerable::setParentTrigger(cedar::proc::TriggerPtr parent)
{
  if (this->isLooped())
  {
    // If there is already a parent trigger for looped steps, disconnect it first!
    CEDAR_ASSERT(!parent || !this->mParentTrigger.lock());
  }
  this->mParentTrigger = parent;
}

cedar::proc::TriggerPtr cedar::proc::Triggerable::getParentTrigger()
{
  return this->mParentTrigger.lock();
}

void cedar::proc::Triggerable::callOnStart()
{
  this->onStart();
  if (mFinished)
  {
    for (size_t i = 0; i < this->mFinished->getListeners().size(); ++i)
    {
      this->mFinished->getListeners().at(i)->callOnStart();
    }
  }
}

void cedar::proc::Triggerable::callOnStop()
{
  this->onStop();
  this->setState(cedar::proc::Triggerable::STATE_NONE, "");
  if (mFinished)
  {
    for (size_t i = 0; i < this->mFinished->getListeners().size(); ++i)
    {
      this->mFinished->getListeners().at(i)->callOnStop();
    }
  }
}

void cedar::proc::Triggerable::setState(cedar::proc::Triggerable::State newState, const std::string& annotation)
{
  // Only act if the state actually changes.
  if (newState != this->mState || annotation != this->mStateAnnotation)
  {
    this->mState = newState;
    this->mStateAnnotation = annotation;
//    emit stateChanged();
  }
}

cedar::proc::Triggerable::State cedar::proc::Triggerable::getState() const
{
  return this->mState;
}

const std::string& cedar::proc::Triggerable::getStateAnnotation() const
{
  return this->mStateAnnotation;
}

void cedar::proc::Triggerable::onStart()
{
  // empty as a default implementation
}

void cedar::proc::Triggerable::onStop()
{
  // empty as a default implementation
}

cedar::proc::TriggerPtr& cedar::proc::Triggerable::getFinishedTrigger()
{
  if (!this->mFinished)
  {
    mFinished = cedar::proc::TriggerPtr(new cedar::proc::Trigger("processingDone"));
  }
  return this->mFinished;
}
