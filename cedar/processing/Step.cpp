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
#include "cedar/processing/sources/GroupSource.h"
#include "cedar/processing/Step.h"
#include "cedar/processing/Arguments.h"
#include "cedar/processing/exceptions.h"
#include "cedar/processing/Group.h"
#include "cedar/processing/Trigger.h"
#include "cedar/processing/LoopedTrigger.h"
#include "cedar/auxiliaries/BoolParameter.h"
#include "cedar/auxiliaries/systemFunctions.h"
#include "cedar/auxiliaries/assert.h"
#include "cedar/auxiliaries/stringFunctions.h"
#include "cedar/auxiliaries/Log.h"
#include "cedar/units/Time.h"
#include "cedar/units/prefixes.h"
#include "cedar/defines.h"
#include "cedar/auxiliaries/CallFunctionInThreadALot.h"

// SYSTEM INCLUDES
#include <QMutexLocker>
#include <QReadLocker>
#include <QWriteLocker>
#include <opencv2/opencv.hpp>
#include <boost/make_shared.hpp>
#include <iostream>
#include <ctime>
#include <string>

// MACROS
// Enable to show information on locking/unlocking
//#define DEBUG_LOCKS
// Show information about execution/triggering of steps
//#define DEBUG_RUNNING
// Show information about triggering
//#define DEBUG_TRIGGERING
// Check for NaNs after every compute call
//#define CEDAR_ENABLE_NAN_CHECK

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------
cedar::proc::Step::Step(bool isLooped)
:
Triggerable(isLooped),
// initialize parameters
mAutoLockInputsAndOutputs(true)
{
  this->mComputeTimeId = this->registerTimeMeasurement("compute call");
  this->mLockingTimeId = this->registerTimeMeasurement("locking");
  this->mRoundTimeId = this->registerTimeMeasurement("round time");


  // create the finished trigger singleton.
  this->getFinishedTrigger();

  // When the name changes, we need to tell the manager about this.
  QObject::connect(this->_mName.get(), SIGNAL(valueChanged()), this, SLOT(onNameChanged()));

  //this->registerFunction("reset", boost::bind(&cedar::proc::Step::callReset, this), false);
}

cedar::proc::Step::~Step()
{
  this->unregisterRecordedData();
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::proc::Step::updateTriggerChains(std::set<cedar::proc::Trigger*>& visited)
{
  this->getFinishedTrigger()->updateTriggeringOrder(visited);
}

unsigned int cedar::proc::Step::registerTimeMeasurement(const std::string& measurement)
{
  unsigned int id = this->mTimeMeasurementNames.size();
  // average the last 100 iteration times
  this->mTimeMeasurements.push_back(cedar::aux::MovingAverage<cedar::unit::Time>(100));
  this->mTimeMeasurementNames[id] = measurement;

  return id;
}

void cedar::proc::Step::revalidateInputSlot(const std::string& slot)
{
  this->setState(cedar::proc::Triggerable::STATE_UNKNOWN, "");

  this->cedar::proc::Connectable::revalidateInputSlot(slot);
}

void cedar::proc::Step::lock(cedar::aux::LOCK_TYPE parameterAccessType) const
{
  this->mpConnectionLock->lockForRead();
  this->lockData();
  this->lockParameters(parameterAccessType);
}

void cedar::proc::Step::unlock(cedar::aux::LOCK_TYPE parameterAccessType) const
{
  this->mpConnectionLock->unlock();
  this->unlockParameters(parameterAccessType);
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
  cedar::proc::Step::ReadLocker locker(this);

  // reset the step
  this->reset();

  // unlock everything
  locker.unlock();

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

void cedar::proc::Step::unregisterFunction(const std::string& actionName)
{
  this->mActions.erase(mActions.find(actionName));
}

bool cedar::proc::Step::isRegistered(const std::string& actionName)
{
  if (this->mActions.find(actionName) != this->mActions.end())
  {
    return true;
  }

  return false;
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
  cedar::aux::Lockable::WriteLockerPtr locker;
  if (autolock)
  {
    // lock the step
    locker = cedar::aux::Lockable::WriteLockerPtr(new cedar::aux::Lockable::WriteLocker(this));
  }

  // call it
  function();
}

const cedar::proc::Step::ActionMap& cedar::proc::Step::getActions() const
{
  return this->mActions;
}

bool cedar::proc::Step::hasAction(const std::string& action) const
{
  return this->mActions.find(action) != this->mActions.end();
}

/*! This method takes care of changing the step's name in the registry as well.
 *
 * @todo Unify in element using boost signals/slots
 */
void cedar::proc::Step::onNameChanged()
{
  if (cedar::proc::GroupPtr parent_group = this->getGroup())
  {
    // update the name
    parent_group->updateObjectName(this);

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

cedar::proc::TriggerPtr cedar::proc::Step::getTrigger(size_t index) const
{
  CEDAR_ASSERT(index < this->getTriggerCount());
  return this->mTriggers.at(index);
}

void cedar::proc::Step::addTrigger(cedar::proc::TriggerPtr trigger)
{
  this->mTriggers.push_back(trigger);
}

#include "cedar/auxiliaries/MatData.h"

void cedar::proc::Step::onTrigger(cedar::proc::ArgumentsPtr arguments, cedar::proc::TriggerPtr trigger)
{
#ifdef DEBUG_RUNNING
  std::cout << "DEBUG_RUNNING> " << this->getName() << ".onTrigger()" << std::endl;
#endif // DEBUG_RUNNING
  // if an exception has previously happened, do nothing.
  switch (this->getState())
  {
    case cedar::proc::Triggerable::STATE_EXCEPTION:
    case cedar::proc::Triggerable::STATE_EXCEPTION_ON_START:
      return;

    default:
      break; // nothing to do, continue triggering
  }

  // make sure noone changes the connections while the trigger call is being processed
  QReadLocker connections_locker(this->mpConnectionLock);

  // if there are invalid inputs, stop
  if (!this->allInputsValid())
  {
    std::string invalid_inputs = cedar::aux::join(this->getInvalidInputNames(), "\", \"");

    this->setState
    (
      cedar::proc::Triggerable::STATE_NOT_RUNNING,
      "Invalid inputs prevent the step from running. These are: \"" + invalid_inputs + "\""
    );
    return;
  }

  // do the work!
  if (!this->mBusy.tryLock())
  {
    return;
  }
  // start measuring the lock time.
  boost::posix_time::ptime lock_start = boost::posix_time::microsec_clock::universal_time();

  connections_locker.unlock();

  // lock the step
  cedar::proc::Step::ReadLocker step_locker(this);

  boost::posix_time::ptime lock_end = boost::posix_time::microsec_clock::universal_time();
  boost::posix_time::time_duration lock_elapsed = lock_end - lock_start;
  cedar::unit::Time lock_elapsed_s(lock_elapsed.total_microseconds() * cedar::unit::micro * cedar::unit::seconds);
  this->setLockTimeMeasurement(lock_elapsed_s);

  if (!this->mandatoryConnectionsAreSet())
  {
    std::string errors = cedar::aux::join(this->getInvalidInputNames(), ", ");

    this->setState
    (
      cedar::proc::Triggerable::STATE_NOT_RUNNING,
      "Unconnected mandatory inputs prevent the step from running. These inputs are:" + errors
    );
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
    if (arguments.get() != nullptr)
    {
      // call the compute function with the given arguments
      this->compute(*(arguments.get()));
    }
    else
    {
      // call the compute function with empty arguments
      cedar::proc::Arguments args;
      this->compute(args);

      if (this->getState() == cedar::proc::Triggerable::STATE_UNKNOWN)
      {
        this->setState(cedar::proc::Triggerable::STATE_RUNNING, "");
      }
    }

    auto looped_trigger= boost::dynamic_pointer_cast< cedar::proc::LoopedTrigger >( trigger );
    if (looped_trigger)
    {
      mNumberOfStepsMissed= looped_trigger->getNumberOfStepsMissed();
    }

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
    this->setState(cedar::proc::Triggerable::STATE_EXCEPTION, "An exception occurred:\n" + e.exceptionInfo());
  }
  catch(const std::exception& e)
  {
    cedar::aux::LogSingleton::getInstance()->error
    (
      "An exception occurred in step \"" + this->getName() + "\": " + std::string(e.what()),
      "cedar::proc::Step::run()",
      this->getName()
    );
    this->setState(cedar::proc::Triggerable::STATE_EXCEPTION, "An exception occurred:\n" + std::string(e.what()));
  }
  catch(...)
  {
    cedar::aux::LogSingleton::getInstance()->error
    (
      "An exception of unknown type occurred in step \"" + this->getName() + "\".",
      "cedar::proc::Step::run()",
      this->getName()
    );
    this->setState(cedar::proc::Triggerable::STATE_EXCEPTION, "An unknown exception type occurred.");
  }

  boost::posix_time::ptime run_end = boost::posix_time::microsec_clock::universal_time();
  boost::posix_time::time_duration run_elapsed = run_end - run_start;
  cedar::unit::Time run_elapsed_s(run_elapsed.total_microseconds() * cedar::unit::micro * cedar::unit::seconds);

  // take time measurements
  this->setRunTimeMeasurement(run_elapsed_s);

#ifdef CEDAR_ENABLE_NAN_CHECK
  if (this->hasSlotForRole(cedar::proc::DataRole::OUTPUT))
  {
    for (auto name_slot_pair : this->getDataSlots(cedar::proc::DataRole::OUTPUT))
    {
      auto data = name_slot_pair.second->getData();
      if (auto mat_data = boost::dynamic_pointer_cast<cedar::aux::MatData>(data))
      {
        if (!cv::checkRange(mat_data->getData(), true))
        {
          cv::Mat clone = mat_data->getData().clone();
          clone.setTo(0.0);
          mat_data->setData(clone);
          std::cout << "NaN detected! " << this->getFullPath() << std::endl;
          this->setState(cedar::proc::Triggerable::STATE_EXCEPTION, "NaN detected.");
        }
      }
    }
  }
#endif // CEDAR_ENABLE_NAN_CHECK

  // unlock the step
  step_locker.unlock();

  // process slots whose properties have changed during the compute call
  this->processChangedSlots();

  // finally, the step is now no longer busy
  this->mBusy.unlock();

  //!@todo This is code that really belongs in Trigger(able). But it can't be moved there as it is, because Trigger(able) doesn't know about loopiness etc.
  // subsequent steps are triggered if one of the following conditions is met:
  // a) This step has not been triggered as part of a trigger chain. This is the case if trigger is NULL.
  // b) The step is looped. In this case it is the start of a trigger chain
  // c) The step is a trigger source. This can happen, e.g., if it has no inputs. This also makes it the start
  //    of a trigger chain. The exception here are group sources because they are triggered from the outside (but via a
  //    special mechanism in Trigger::buildTriggerGraph)
  if
  (
    this->getState() != cedar::proc::Triggerable::STATE_INITIALIZING &&
    (
      !trigger
      || this->isLooped()
      ||
      (this->isTriggerSource() && !dynamic_cast<cedar::proc::sources::GroupSource*>(this))
    )
  )
  {
#ifdef DEBUG_TRIGGERING
    std::cout << "Step " << this->getName() << " was computed and thus triggers its done trigger." << std::endl;
#endif // DEBUG_TRIGGERING
    // trigger subsequent steps in a non-blocking manner
    if (this->isLooped())
    {
      if (!this->mFinishedChainResult.isStarted() || this->mFinishedChainResult.isFinished())
      {
        this->mFinishedChainResult = QtConcurrent::run(boost::bind(&cedar::proc::Trigger::trigger, this->getFinishedTrigger(), cedar::proc::ArgumentsPtr()));
      }
    }
    else
    {
      this->getFinishedTrigger()->trigger();
    }
  }
}

void cedar::proc::Step::callComputeWithoutTriggering(cedar::proc::ArgumentsPtr args)
{
  // pass a dummy trigger into the onTrigger function; this prevents subsequents steps from being triggered
  this->onTrigger(args, cedar::proc::TriggerPtr(new cedar::proc::Trigger()));
}


void cedar::proc::Step::processChangedSlots()
{
  QWriteLocker locker(this->mSlotsChangedDuringComputeCall.getLockPtr());
  auto& queue = this->mSlotsChangedDuringComputeCall.member();
  while (!queue.empty())
  {
    auto slot = queue.front();
    queue.pop_front();
    this->cedar::proc::Connectable::emitOutputPropertiesChangedSignal(slot);
  }
  locker.unlock();
}

void cedar::proc::Step::emitOutputPropertiesChangedSignal(const std::string& slot)
{
  if (!this->mBusy.tryLock())
  {
    QWriteLocker locker(this->mSlotsChangedDuringComputeCall.getLockPtr());
    this->mSlotsChangedDuringComputeCall.member().push_back(slot);
    locker.unlock();
  }
  else
  {
    // this makes sure that if an exception occurs during emitOutputPropertiesChangedSignal, we do not end with a dangling lock
    cedar::aux::CallOnScopeExit unlocker(boost::bind(&QMutex::unlock, &this->mBusy));

    this->cedar::proc::Connectable::emitOutputPropertiesChangedSignal(slot);

    // to be sure, unlock now
    unlocker.callNow();
  }
}

unsigned int cedar::proc::Step::getNumberOfTimeMeasurements() const
{
  return this->mTimeMeasurements.size();
}

const std::string& cedar::proc::Step::getTimeMeasurementName(unsigned int id) const
{
  auto iter = this->mTimeMeasurementNames.find(id);
  CEDAR_ASSERT(iter != this->mTimeMeasurementNames.end());
  return iter->second;
}

void cedar::proc::Step::setTimeMeasurement(unsigned int id, const cedar::unit::Time& time)
{
  CEDAR_DEBUG_ASSERT(id < this->mTimeMeasurements.size());
  auto& measurement = this->mTimeMeasurements.at(id);
  QWriteLocker locker(measurement.getLockPtr());
  measurement.member().append(time);
}

void cedar::proc::Step::setRunTimeMeasurement(const cedar::unit::Time& time)
{
  this->setTimeMeasurement(this->mComputeTimeId, time);
}

void cedar::proc::Step::setLockTimeMeasurement(const cedar::unit::Time& time)
{
  this->setTimeMeasurement(this->mLockingTimeId, time);
}

void cedar::proc::Step::setRoundTimeMeasurement(const cedar::unit::Time& time)
{
  this->setTimeMeasurement(this->mRoundTimeId, time);
}

cedar::unit::Time cedar::proc::Step::getLastTimeMeasurement(unsigned int id) const
{
  CEDAR_DEBUG_ASSERT(id < this->mTimeMeasurements.size());
  auto& measurement = this->mTimeMeasurements.at(id);
  QReadLocker locker(measurement.getLockPtr());
  if (measurement.member().size() > 0)
  {
    cedar::unit::Time copy = measurement.member().getNewest();
    return copy;
  }
  else
  {
    CEDAR_THROW(cedar::proc::NoMeasurementException, "No measurements, yet.");
  }
}


double cedar::proc::Step::getNumberOfStepsMissed() const
{
  return mNumberOfStepsMissed;
}

cedar::unit::Time cedar::proc::Step::getRunTimeMeasurement() const
{
  return this->getLastTimeMeasurement(this->mComputeTimeId);
}

cedar::unit::Time cedar::proc::Step::getLockTimeMeasurement() const
{
  return this->getLastTimeMeasurement(this->mLockingTimeId);
}

cedar::unit::Time cedar::proc::Step::getRoundTimeMeasurement() const
{
  return this->getLastTimeMeasurement(this->mRoundTimeId);
}

bool cedar::proc::Step::hasTimeMeasurement(unsigned int id) const
{
  CEDAR_DEBUG_ASSERT(id < this->mTimeMeasurements.size());
  auto& measurement = this->mTimeMeasurements.at(id);

  QReadLocker locker(measurement.getLockPtr());
  bool has_measurement = measurement.member().size() > 0;
  return has_measurement;
}


bool cedar::proc::Step::hasRunTimeMeasurement() const
{
  return this->hasTimeMeasurement(this->mComputeTimeId);
}

bool cedar::proc::Step::hasLockTimeMeasurement() const
{
  return this->hasTimeMeasurement(this->mLockingTimeId);
}

bool cedar::proc::Step::hasRoundTimeMeasurement() const
{
  return this->hasTimeMeasurement(this->mRoundTimeId);
}

cedar::unit::Time cedar::proc::Step::getTimeMeasurementAverage(unsigned int id) const
{
  CEDAR_DEBUG_ASSERT(id < this->mTimeMeasurements.size());
  auto& measurement = this->mTimeMeasurements.at(id);

  QReadLocker locker(measurement.getLockPtr());
  if (measurement.member().size() > 0)
  {
    cedar::unit::Time copy = measurement.member().getAverage();
    return copy;
  }
  else
  {
    CEDAR_THROW(cedar::proc::NoMeasurementException, "No measurements, yet.");
  }
}

cedar::unit::Time cedar::proc::Step::getRunTimeAverage() const
{
  return this->getTimeMeasurementAverage(this->mComputeTimeId);
}

cedar::unit::Time cedar::proc::Step::getLockTimeAverage() const
{
  return this->getTimeMeasurementAverage(this->mLockingTimeId);
}

cedar::unit::Time cedar::proc::Step::getRoundTimeAverage() const
{
  return this->getTimeMeasurementAverage(this->mRoundTimeId);
}

//!@todo This method is probably superfluous
void cedar::proc::Step::callInputConnectionChanged(const std::string& slot)
{
  this->cedar::proc::Connectable::callInputConnectionChanged(slot);
}
