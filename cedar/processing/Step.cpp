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

    File:        Step.cpp

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
#include "cedar/processing/Step.h"
#include "cedar/processing/Arguments.h"
#include "cedar/processing/exceptions.h"
#include "cedar/processing/Network.h"
#include "cedar/processing/Trigger.h"
#include "cedar/auxiliaries/BoolParameter.h"
#include "cedar/auxiliaries/systemFunctions.h"
#include "cedar/auxiliaries/assert.h"
#include "cedar/auxiliaries/stringFunctions.h"
#include "cedar/auxiliaries/Log.h"
#include "cedar/units/Time.h"
#include "cedar/units/prefixes.h"
#include "cedar/defines.h"
#include "cedar/auxiliaries/Recorder.h"

// SYSTEM INCLUDES
#include <QMutexLocker>
#include <QReadLocker>
#include <QWriteLocker>
#include <opencv2/opencv.hpp>
#include <iostream>
#include <ctime>
#include <string>

// MACROS
// Enable to show information on locking/unlocking
//#define DEBUG_LOCKS
// Show information about execution/triggering of steps
//#define DEBUG_RUNNING

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::proc::Step::Step(bool /*runInThread*/, bool isLooped)
:
Triggerable(isLooped),
// initialize members
// average the last 100 iteration times
mMovingAverageIterationTime(100),
// average the last 100 iteration times
mLockingTime(100),
mRoundTime(100), // average the last 100 iteration times
// initialize parameters
mAutoLockInputsAndOutputs(true)
{
  // create the finished trigger singleton.
  this->getFinishedTrigger();

  // When the name changes, we need to tell the manager about this.
  QObject::connect(this->_mName.get(), SIGNAL(valueChanged()), this, SLOT(onNameChanged()));

  this->registerFunction("reset", boost::bind(&cedar::proc::Step::callReset, this), false);
}

cedar::proc::Step::Step(bool isLooped)
:
Triggerable(isLooped),
// initialize members
// average the last 100 iteration times
mMovingAverageIterationTime(100),
// average the last 100 iteration times
mLockingTime(100),
mRoundTime(100), // average the last 100 iteration times
// initialize parameters
mAutoLockInputsAndOutputs(true)
{
  // create the finished trigger singleton.
  this->getFinishedTrigger();

  // When the name changes, we need to tell the manager about this.
  QObject::connect(this->_mName.get(), SIGNAL(valueChanged()), this, SLOT(onNameChanged()));

  this->registerFunction("reset", boost::bind(&cedar::proc::Step::callReset, this), false);
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::proc::Step::lock(cedar::aux::LOCK_TYPE parameterAccessType) const
{
  this->mpConnectionLock->lockForRead();
  this->lockData();
  this->lockParameters(parameterAccessType);
}

void cedar::proc::Step::unlock() const
{
  this->mpConnectionLock->unlock();
  this->unlockParameters();
  this->unlockData();
}

void cedar::proc::Step::lockData() const
{
  if (this->mAutoLockInputsAndOutputs)
  {
    this->lockAll();
  }
  else
  {
    this->lockBuffers();
  }
}

void cedar::proc::Step::unlockData() const
{
  if (this->mAutoLockInputsAndOutputs)
  {
    // unlock all data
    this->unlockAll();
  }
  else
  {
    this->unlockBuffers();
  }
}

void cedar::proc::Step::callReset()
{
  // first, reset the current state of the step (i.e., clear any exception etc. state)
  this->resetState();

  // lock everything
  this->lock(cedar::aux::LOCK_TYPE_READ);

  // reset the step
  this->reset();

  // unlock everything
  this->unlock();

  this->getFinishedTrigger()->trigger();
}

void cedar::proc::Step::reset()
{
  // empty as default implementation
}

void cedar::proc::Step::setAutoLockInputsAndOutputs(bool autoLock)
{
  this->mAutoLockInputsAndOutputs = autoLock;
}

void cedar::proc::Step::registerFunction(const std::string& actionName, boost::function<void()> function, bool autoLock)
{
  if (this->mActions.find(actionName) != this->mActions.end())
  {
    CEDAR_THROW(cedar::aux::InvalidNameException, "Duplicate action name: " + actionName);
  }
  this->mActions[actionName] = std::make_pair(function, autoLock);
}

void cedar::proc::Step::callAction(const std::string& name)
{
  // find the action
  ActionMap::iterator iter = this->mActions.find(name);
  if (iter == this->mActions.end())
  {
    // if it doesn't exist, throw
    CEDAR_THROW(cedar::aux::InvalidNameException, "Unknown action name: " + name);
  }

  // get the functor
  boost::function<void()>& function = iter->second.first;

  bool autolock = iter->second.second;
  cedar::aux::Lockable::LockerPtr locker;
  if (autolock)
  {
    // lock the step
    //!@todo Should this be a read lock?
    locker = cedar::aux::Lockable::LockerPtr(new cedar::aux::Lockable::Locker(this, cedar::aux::LOCK_TYPE_WRITE));
  }

  // call it
  function();
}

const cedar::proc::Step::ActionMap& cedar::proc::Step::getActions() const
{
  return this->mActions;
}

/*! This method takes care of changing the step's name in the registry as well.
 */
void cedar::proc::Step::onNameChanged()
{
  if (cedar::proc::ElementPtr parent_network = this->mRegisteredAt.lock())
  {
    // update the name
    boost::static_pointer_cast<cedar::proc::Network>(parent_network)->updateObjectName(this);

    // emit a signal to notify anyone interested in this
    emit nameChanged();
  }
}

size_t cedar::proc::Step::getTriggerCount() const
{
  return this->mTriggers.size();
}

cedar::proc::TriggerPtr cedar::proc::Step::getTrigger(size_t index)
{
  CEDAR_ASSERT(index < this->getTriggerCount());
  return this->mTriggers.at(index);
}

void cedar::proc::Step::addTrigger(cedar::proc::TriggerPtr trigger)
{
  this->mTriggers.push_back(trigger);
}

void cedar::proc::Step::onTrigger(cedar::proc::ArgumentsPtr arguments, cedar::proc::TriggerPtr trigger)
{
#ifdef DEBUG_RUNNING
  std::cout << "DEBUG_RUNNING> " << this->getName() << ".onTrigger()" << std::endl;
#endif // DEBUG_RUNNING
  // if an exception has previously happened, do nothing.
  if
  (
    this->mState == cedar::proc::Triggerable::STATE_EXCEPTION
      || this->mState == cedar::proc::Triggerable::STATE_EXCEPTION_ON_START
  )
  {
    return;
  }

  // if there are invalid inputs, stop
  if (!this->allInputsValid())
  {
    std::string invalid_inputs = cedar::aux::join(mInvalidInputNames, "\", \"");

    this->setState(cedar::proc::Triggerable::STATE_NOT_RUNNING,
                   "Invalid inputs prevent the step from running. These are: \"" + invalid_inputs + "\"");
    return;
  }

  // if we get to this point, set the state to running
  this->setState(cedar::proc::Triggerable::STATE_RUNNING, "");

  // do the work!
  if (this->mBusy.tryLock())
  {
    // start measuring the lock time.
    boost::posix_time::ptime lock_start = boost::posix_time::microsec_clock::universal_time();

    // lock the step
    this->lock(cedar::aux::LOCK_TYPE_READ);

    boost::posix_time::ptime lock_end = boost::posix_time::microsec_clock::universal_time();
    boost::posix_time::time_duration lock_elapsed = lock_end - lock_start;
    cedar::unit::Time lock_elapsed_s(lock_elapsed.total_microseconds() * cedar::unit::micro * cedar::unit::seconds);
    this->setLockTimeMeasurement(lock_elapsed_s);

    if (!this->mandatoryConnectionsAreSet())
    {
      std::string errors = cedar::aux::join(mInvalidInputNames, ", ");

      this->setState(cedar::proc::Triggerable::STATE_NOT_RUNNING,
                     "Unconnected mandatory inputs prevent the step from running. These inputs are:" + errors);
      this->unlock();
      this->mBusy.unlock();
      return;
    } // this->mMandatoryConnectionsAreSet

    if (this->mPreciseLastComputeCall.is_not_a_date_time()) // was not called before, initialize time
    {
      this->mPreciseLastComputeCall = boost::posix_time::microsec_clock::universal_time();
    }
    else
    {
      boost::posix_time::ptime last_precise = this->mPreciseLastComputeCall;
      this->mPreciseLastComputeCall = boost::posix_time::microsec_clock::universal_time();
      boost::posix_time::time_duration elapsed_precise = this->mPreciseLastComputeCall - last_precise;
      cedar::unit::Time precise_time(elapsed_precise.total_microseconds() * cedar::unit::micro * cedar::unit::seconds);
      this->setRoundTimeMeasurement(precise_time);
    }

    // start measuring the execution time.
    boost::posix_time::ptime run_start = boost::posix_time::microsec_clock::universal_time();

    try
    {
      // call the compute function with the given arguments
      this->compute(*(arguments.get()));
    }
    // catch exceptions and translate them to the given state/message
    catch(const cedar::aux::ExceptionBase& e)
    {
      cedar::aux::LogSingleton::getInstance()->error
      (
        "An exception occurred in step \"" + this->getName() + "\": " + e.exceptionInfo(),
        "cedar::proc::Step::run()",
        this->getName()
      );
      this->setState(cedar::proc::Step::STATE_EXCEPTION, "An exception occurred:\n" + e.exceptionInfo());
    }
    catch(const std::exception& e)
    {
      cedar::aux::LogSingleton::getInstance()->error
      (
        "An exception occurred in step \"" + this->getName() + "\": " + std::string(e.what()),
        "cedar::proc::Step::run()",
        this->getName()
      );
      this->setState(cedar::proc::Step::STATE_EXCEPTION, "An exception occurred:\n" + std::string(e.what()));
    }
    catch(...)
    {
      cedar::aux::LogSingleton::getInstance()->error
      (
        "An exception of unknown type occurred in step \"" + this->getName() + "\".",
        "cedar::proc::Step::run()",
        this->getName()
      );
      this->setState(cedar::proc::Step::STATE_EXCEPTION, "An unknown exception type occurred.");
    }

    boost::posix_time::ptime run_end = boost::posix_time::microsec_clock::universal_time();
    boost::posix_time::time_duration run_elapsed = run_end - run_start;
    cedar::unit::Time run_elapsed_s(run_elapsed.total_microseconds() * cedar::unit::micro * cedar::unit::seconds);

    // take time measurements
    this->setRunTimeMeasurement(run_elapsed_s);

    // unlock the step
    this->unlock();

    //!@todo This is code that really belongs in Trigger(able). But it can't be moved there as it is, because Trigger(able) doesn't know about loopiness etc.
    // subsequent steps are triggered if one of the following conditions is met:
    // a) This step has not been triggered as part of a trigger chain. This is the case if trigger is NULL.
    // b) The step is looped. In this case it is the start of a trigger chain
    // c) The step is not triggered by anyone. This can happen, e.g., if it has no inputs. This also makes it the start
    //    of a trigger chain.
    if (!trigger || this->isLooped() || !this->isTriggered())
    {
      this->getFinishedTrigger()->trigger();
    }

    //!@todo Should busy be unlocked before triggering subsequent steps?
    this->mBusy.unlock();
  }
}

void cedar::proc::Step::setRunTimeMeasurement(const cedar::unit::Time& time)
{
  QWriteLocker locker(&this->mLastIterationTimeLock);
  this->mMovingAverageIterationTime.append(time);
}

void cedar::proc::Step::setLockTimeMeasurement(const cedar::unit::Time& time)
{
  QWriteLocker locker(&this->mLockTimeLock);
  this->mLockingTime.append(time);
}

void cedar::proc::Step::setRoundTimeMeasurement(const cedar::unit::Time& time)
{
  QWriteLocker locker(&this->mRoundTimeLock);
  this->mRoundTime.append(time);
}

cedar::unit::Time cedar::proc::Step::getRunTimeMeasurement() const
{
  QReadLocker locker(&this->mLastIterationTimeLock);
  if (this->mMovingAverageIterationTime.size() > 0)
  {
    cedar::unit::Time copy = this->mMovingAverageIterationTime.getNewest();
    return copy;
  }
  else
  {
    CEDAR_THROW(cedar::proc::NoMeasurementException, "No measurements, yet.");
  }
}

cedar::unit::Time cedar::proc::Step::getLockTimeMeasurement() const
{
  QReadLocker locker(&this->mLockTimeLock);
  if (this->mLockingTime.size() > 0)
  {
    cedar::unit::Time copy = this->mLockingTime.getNewest();
    return copy;
  }
  else
  {
    CEDAR_THROW(cedar::proc::NoMeasurementException, "No measurements, yet.");
  }
}

bool cedar::proc::Step::hasRunTimeMeasurement() const
{
  QReadLocker locker(&this->mLastIterationTimeLock);
  bool has_measurement = this->mMovingAverageIterationTime.size() > 0;
  return has_measurement;
}

bool cedar::proc::Step::hasLockTimeMeasurement() const
{
  QReadLocker locker(&this->mLockTimeLock);
  bool has_measurement = this->mLockingTime.size() > 0;
  return has_measurement;
}

bool cedar::proc::Step::hasRoundTimeMeasurement() const
{
  QReadLocker locker(&this->mRoundTimeLock);
  bool has_measurement = this->mRoundTime.size() > 0;
  return has_measurement;
}

cedar::unit::Time cedar::proc::Step::getRunTimeAverage() const
{
  QReadLocker locker(&this->mLastIterationTimeLock);
  if (this->mMovingAverageIterationTime.size() > 0)
  {
    cedar::unit::Time copy = this->mMovingAverageIterationTime.getAverage();
    return copy;
  }
  else
  {
    CEDAR_THROW(cedar::proc::NoMeasurementException, "No measurements, yet.");
  }
}

cedar::unit::Time cedar::proc::Step::getLockTimeAverage() const
{
  QReadLocker locker(&this->mLockTimeLock);
  if (this->mLockingTime.size() > 0)
  {
    cedar::unit::Time copy = this->mLockingTime.getAverage();
    return copy;
  }
  else
  {
    CEDAR_THROW(cedar::proc::NoMeasurementException, "No measurements, yet.");
  }
}

cedar::unit::Time cedar::proc::Step::getRoundTimeMeasurement() const
{
  QReadLocker locker(&this->mRoundTimeLock);
  if (this->mRoundTime.size() > 0)
  {
    cedar::unit::Time copy = this->mRoundTime.getNewest();
    return copy;
  }
  else
  {
    CEDAR_THROW(cedar::proc::NoMeasurementException, "No measurements, yet.");
  }
}

cedar::unit::Time cedar::proc::Step::getRoundTimeAverage() const
{
  QReadLocker locker(&this->mRoundTimeLock);
  if (this->mRoundTime.size() > 0)
  {
    cedar::unit::Time copy = this->mRoundTime.getAverage();
    return copy;
  }
  else
  {
    CEDAR_THROW(cedar::proc::NoMeasurementException, "No measurements, yet.");
  }
}


bool cedar::proc::Step::isRecorded() const
{
  std::vector<cedar::proc::DataRole::Id> slotTypes;
  slotTypes.push_back(cedar::proc::DataRole::BUFFER);
  slotTypes.push_back(cedar::proc::DataRole::OUTPUT);

  for (unsigned int s = 0; s < slotTypes.size(); s++)
  {

    if (this->hasRole(slotTypes[s]))
    {
      cedar::proc::Connectable::SlotList dataSlots = this->getOrderedDataSlots(slotTypes[s]);
      for (unsigned int i = 0; i < dataSlots.size(); i++)
      {
        if (cedar::aux::RecorderSingleton::getInstance()->isRegistered(dataSlots[i]->getData()))
        {
          return true;
        }
      }
    }
  }
  return false;
}

void cedar::proc::Step::setThreaded(bool /*isThreaded*/)
{
  // does nothing
}

bool cedar::proc::Step::isThreaded() const
{
  return false;
}

void cedar::proc::Step::callInputConnectionChanged(const std::string& slot)
{
  this->revalidateInputSlot(slot);
}

void cedar::proc::Step::revalidateInputSlot(const std::string& slot)
{
  this->setState(cedar::proc::Triggerable::STATE_UNKNOWN, "");
  this->getInputSlot(slot)->setValidity(cedar::proc::DataSlot::VALIDITY_UNKNOWN);
  this->inputConnectionChanged(slot);
  this->getInputValidity(slot);
}
