/*======================================================================================================================

    Copyright 2011, 2012, 2013, 2014, 2015, 2016, 2017 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany
 
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

// CEDAR INCLUDES
#include "cedar/processing/Triggerable.h"
#include "cedar/processing/Trigger.h"
#include "cedar/processing/Group.h"
#include "cedar/processing/exceptions.h"
#include "cedar/auxiliaries/NamedConfigurable.h"
#include "cedar/auxiliaries/assert.h"

// SYSTEM INCLUDES
#include <QReadLocker>
#include <QWriteLocker>
#include <QMutexLocker>


//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------
cedar::proc::Triggerable::Triggerable(bool isLooped)
:
mIsLooped(isLooped),
mState(cedar::proc::Triggerable::STATE_UNKNOWN),
mStartCalls(0),
mpStartCallsLock(new QMutex())
{
}

cedar::proc::Triggerable::~Triggerable()
{
  QReadLocker reader(this->mTriggersListenedTo.getLockPtr());

  while (!this->mTriggersListenedTo.member().empty())
  {
    cedar::proc::TriggerPtr trigger = this->mTriggersListenedTo.member().begin()->lock();
    CEDAR_ASSERT(trigger);
    trigger->removeListener(this);
  }
  delete this->mpStartCallsLock;
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

bool cedar::proc::Triggerable::isTriggerSource() const
{
  return !this->isTriggered();
}

void cedar::proc::Triggerable::updateTriggeringOrder(std::set<cedar::proc::Trigger*>& visited, bool recurseUp, bool recurseDown)
{
  QReadLocker locker(this->mFinished.getLockPtr());
  if (this->mFinished.member())
  {
    this->mFinished.member()->updateTriggeringOrder(visited, recurseUp, recurseDown);
  }
}

bool cedar::proc::Triggerable::isStarted() const
{
  QMutexLocker locker(this->mpStartCallsLock);
  bool started = (this->mStartCalls > 0);
  return started;
}

bool cedar::proc::Triggerable::exceptionWrappedCall
     (
       const boost::function<void()>& call,
       const std::string& messagePreface,
       cedar::proc::Triggerable::State state
     )
     throw()
{
  try
  {
    // execute the function
    call();
  }
  // catch exceptions and translate them to the given state/message
  catch(const cedar::aux::ExceptionBase& e)
  {
    std::string message = messagePreface + ": " + e.exceptionInfo();
    cedar::aux::LogSingleton::getInstance()->error
    (
      message,
      "cedar::proc::Triggerable::exceptionWrappedCall()"
    );
    this->setState(state, message);
    return true;
  }
  catch(const std::exception& e)
  {
    std::string message = messagePreface + ": " + std::string(e.what());
    cedar::aux::LogSingleton::getInstance()->error
    (
      message,
      "cedar::proc::Triggerable::exceptionWrappedCall()"
    );
    this->setState(state, message);
    return true;
  }
  catch(...)
  {
    std::string message = messagePreface + ": Unknown exception type.";
    cedar::aux::LogSingleton::getInstance()->error
    (
      message,
      "cedar::proc::Triggerable::exceptionWrappedCall()"
    );
    this->setState(state, message);
    return true;
  }
  return false;
}

void cedar::proc::Triggerable::triggeredBy(cedar::proc::TriggerPtr trigger)
{
  QWriteLocker locker(this->mTriggersListenedTo.getLockPtr());
  this->mTriggersListenedTo.member().insert(trigger);

  if (trigger->getOwner() != nullptr)
  {
    if (trigger->getOwner()->isStarted())
    {
      this->callOnStart();
    }
  }
}

void cedar::proc::Triggerable::noLongerTriggeredBy(cedar::proc::TriggerPtr trigger)
{
  QWriteLocker locker(this->mTriggersListenedTo.getLockPtr());
  auto iter = this->mTriggersListenedTo.member().find(trigger);
  CEDAR_ASSERT(iter != this->mTriggersListenedTo.member().end());
  this->mTriggersListenedTo.member().erase(iter);

  if (trigger->getOwner() != nullptr)
  {
    if (trigger->getOwner()->isStarted())
    {
      this->callOnStop();
    }
  }
}

boost::signals2::connection cedar::proc::Triggerable::connectToStateChanged(boost::function<void ()> slot)
{
  return mStateChanged.connect(slot);
}

void cedar::proc::Triggerable::setLoopedTrigger(cedar::proc::LoopedTriggerPtr parent)
{
  if (!this->isLooped())
  {
    CEDAR_THROW(cedar::proc::LoopStateException, "Setting a looped trigger is only allowed for looped triggerables.");
  }
  this->mLoopedTrigger = parent;
  this->signalLoopedTriggerChanged();
}

cedar::proc::LoopedTriggerPtr cedar::proc::Triggerable::getLoopedTrigger()
{
  if (!this->isLooped())
  {
    CEDAR_THROW(cedar::proc::LoopStateException, "Only looped triggerables have a looped trigger.");
  }
  return this->mLoopedTrigger.lock();
}

cedar::proc::ConstLoopedTriggerPtr cedar::proc::Triggerable::getLoopedTrigger() const
{
  if (!this->isLooped())
  {
    CEDAR_THROW(cedar::proc::LoopStateException, "Only looped triggerables have a looped trigger.");
  }
  return this->mLoopedTrigger.lock();
}

void cedar::proc::Triggerable::callOnStart()
{
  // make sure no other thread can start the step
  QMutexLocker locker(this->mpStartCallsLock);

  // only call onStart if this triggerable hasn't been started yet
  bool exception_occurred = false;
  if (this->mStartCalls == 0)
  {
    exception_occurred
      = this->exceptionWrappedCall
        (
          boost::bind(&cedar::proc::Triggerable::onStart, this),
          "An exception occurred while calling onStart(). You can try to fix this by restarting the trigger. The exception is",
          cedar::proc::Triggerable::STATE_EXCEPTION_ON_START
        );
  }

  // count how often this triggerable was started
  if (!exception_occurred)
  {
    ++this->mStartCalls;
  }

  locker.unlock();

  if (this->mStartCalls == 1)
  {
    this->signalStarted();
    this->setState(cedar::proc::Triggerable::STATE_RUNNING, "");

    QReadLocker lock_r(this->mFinished.getLockPtr());
    if (mFinished.member())
    {
      for (auto listener : this->mFinished.member()->getListeners())
      {
        listener->callOnStart();
      }
    }
  }
}

void cedar::proc::Triggerable::callOnStop()
{
  // only call onStop if there is only one trigger left that started this triggerable
  QMutexLocker locker(this->mpStartCallsLock);
  // count how often this was stopped
  if (this->mStartCalls == 0 && this->getState() != cedar::proc::Triggerable::STATE_EXCEPTION_ON_START)
  {
    // this is ok, it can happen in some circumstances
    // e.g., cyclically connected groups (see unit test GroupOnStartStop)
    return;
  }
  else if (this->mStartCalls == 0 && this->getState() == cedar::proc::Triggerable::STATE_EXCEPTION_ON_START)
  {
    // reset state to unknown
    this->setState(cedar::proc::Triggerable::STATE_UNKNOWN, "");
    return;
  }
  --this->mStartCalls;

  if (this->mStartCalls == 0)
  {
    this->exceptionWrappedCall
    (
      boost::bind(&cedar::proc::Triggerable::onStop, this),
      "An exception occurred while calling onStop()"
    );

    locker.unlock();

    this->signalStopped();
  }
  else
  {
    locker.unlock();
  }

  this->setState(cedar::proc::Triggerable::STATE_UNKNOWN, "");

  QReadLocker lock_r(this->mFinished.getLockPtr());
  if (this->mFinished.member())
  {
    for (auto listener : this->mFinished.member()->getListeners())
    {
      listener->callOnStop();
    }
  }
}

void cedar::proc::Triggerable::setState(cedar::proc::Triggerable::State newState, const std::string& annotation)
{
  // Only act if the state actually changes.
  QWriteLocker locker(this->mState.getLockPtr());
  if (newState != this->mState.member().mState || annotation != this->mState.member().mStateReason)
  {
    this->mState.member().mState = newState;
    this->mState.member().mStateReason = annotation;
    locker.unlock();
    this->signalStateChanged();
  }
}

void cedar::proc::Triggerable::signalStateChanged() const
{
  this->mStateChanged();
}

cedar::proc::Triggerable::State cedar::proc::Triggerable::getState() const
{
  QReadLocker locker(this->mState.getLockPtr());
  cedar::proc::Triggerable::State copy = this->mState.member().mState;
  return copy;
}

std::string cedar::proc::Triggerable::getStateAnnotation() const
{
  QReadLocker locker(this->mState.getLockPtr());
  return this->mState.member().mStateReason;
}

void cedar::proc::Triggerable::onStart()
{
  // empty as a default implementation
}

void cedar::proc::Triggerable::onStop()
{
  // empty as a default implementation
}

cedar::proc::TriggerPtr cedar::proc::Triggerable::getFinishedTrigger()
{
  QReadLocker lock_r(this->mFinished.getLockPtr());
  if (!this->mFinished.member())
  {
    lock_r.unlock();
    QWriteLocker lock_w(this->mFinished.getLockPtr());
    this->mFinished.member() = cedar::proc::TriggerPtr(new cedar::proc::Trigger("processingDone"));
    this->mFinished.member()->setOwner(this);
    lock_w.unlock();
    lock_r.relock();
  }
  return this->mFinished.member();
}

unsigned int cedar::proc::Triggerable::numberOfStartCalls() const
{
  QMutexLocker locker(this->mpStartCallsLock);
  unsigned int calls = this->mStartCalls;
  return calls;
}

void cedar::proc::Triggerable::resetLoopedTrigger()
{
  this->mLoopedTrigger.reset();
  this->signalLoopedTriggerChanged();
}
