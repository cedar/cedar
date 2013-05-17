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
#include "cedar/processing/Manager.h"
#include "cedar/processing/exceptions.h"
#include "cedar/processing/Network.h"
#include "cedar/auxiliaries/BoolParameter.h"
#include "cedar/auxiliaries/systemFunctions.h"
#include "cedar/auxiliaries/assert.h"
#include "cedar/auxiliaries/stringFunctions.h"
#include "cedar/auxiliaries/Log.h"
#include "cedar/units/Time.h"
#include "cedar/units/prefixes.h"
#include "cedar/defines.h"

// SYSTEM INCLUDES
#include <QMutexLocker>
#include <QReadLocker>
#include <QWriteLocker>
#include <iostream>
#include <ctime>
#include <opencv2/opencv.hpp>

// MACROS
// Enable to show information on locking/unlocking
//#define DEBUG_LOCKS
// Show information about execution/triggering of steps
//#define DEBUG_RUNNING

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::proc::Step::Step(bool runInThread, bool isLooped)
:
Triggerable(isLooped),
// initialize members
mpArgumentsLock(new QReadWriteLock()),
// average the last 100 iteration times
mMovingAverageIterationTime(100),
// average the last 100 iteration times
mLockingTime(100),
mRoundTime(100), // average the last 100 iteration times
mLastComputeCall(0),
// initialize parameters
mRNGState(0),
mAutoLockInputsAndOutputs(true),
_mRunInThread(new cedar::aux::BoolParameter(this, "threaded", runInThread))
{
  cedar::aux::LogSingleton::getInstance()->allocating(this);

  this->_mRunInThread->markAdvanced();

  // create the finished trigger singleton.
  this->getFinishedTrigger();

  // When the name changes, we need to tell the manager about this.
  QObject::connect(this->_mName.get(), SIGNAL(valueChanged()), this, SLOT(onNameChanged()));

  this->registerFunction("reset", boost::bind(&cedar::proc::Step::callReset, this));
}

cedar::proc::Step::~Step()
{
  cedar::aux::LogSingleton::getInstance()->freeing(this);

  if (!this->QThread::wait(5000))
  {
    cedar::aux::LogSingleton::getInstance()->warning
    (
      "Step \"" + this->getName() + " is being destroyed while it is still running!",
      "cedar::proc::Step::~Step()"
    );
  }

  CEDAR_DEBUG_ASSERT(mpArgumentsLock != NULL);
  delete mpArgumentsLock;
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

void cedar::proc::Step::registerFunction(const std::string& actionName, boost::function<void()> function)
{
  //!@todo Check for restrictions on the name, e.g., no dots, ...
  if (this->mActions.find(actionName) != this->mActions.end())
  {
    CEDAR_THROW(cedar::proc::InvalidNameException, "Duplicate action name: " + actionName);
  }
  this->mActions[actionName] = function;
}

void cedar::proc::Step::callAction(const std::string& name)
{
  // find the action
  ActionMap::iterator iter = this->mActions.find(name);
  if (iter == this->mActions.end())
  {
    // if it doesn't exist, throw
    CEDAR_THROW(cedar::proc::InvalidNameException, "Unknown action name: " + name);
  }

  // get the functor
  boost::function<void()>& function = iter->second;

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

void cedar::proc::Step::onTrigger(cedar::proc::ArgumentsPtr args, cedar::proc::TriggerPtr)
{
#ifdef DEBUG_RUNNING
  std::cout << "DEBUG_RUNNING> " << this->getName() << ".onTrigger()" << std::endl;
#endif // DEBUG_RUNNING
  // if an exception has previously happened, do nothing.
  if (this->mState == cedar::proc::Triggerable::STATE_EXCEPTION)
  {
    // reset the arguments (we could not process them)
    this->mpArgumentsLock->lockForWrite();
    this->mNextArguments.reset();
    this->mpArgumentsLock->unlock();
    return;
  }

  // if there are invalid inputs, stop
  if (!this->allInputsValid())
  {
    std::string invalid_inputs = cedar::aux::join(mInvalidInputNames, "\", \"");

    this->setState(cedar::proc::Triggerable::STATE_NOT_RUNNING,
                   "Invalid inputs prevent the step from running. These are: \"" + invalid_inputs + "\"");
    // reset the arguments (we could not process them)
    this->mpArgumentsLock->lockForWrite();
    this->mNextArguments.reset();
    this->mpArgumentsLock->unlock();
    return;
  }

  if (!this->mandatoryConnectionsAreSet())
  {
    std::string errors = cedar::aux::join(mInvalidInputNames, ", ");

    this->setState(cedar::proc::Triggerable::STATE_NOT_RUNNING,
                   "Unconnected mandatory inputs prevent the step from running. These inputs are:" + errors);
    return;
  } // this->mMandatoryConnectionsAreSet

  if (!this->setNextArguments(args))
  {
    this->mpArgumentsLock->lockForWrite();
    this->mNextArguments.reset();
    this->mpArgumentsLock->unlock();
    //!@todo This may happen a lot when running multiple looped triggers -- investigate
    /*
    cedar::aux::LogSingleton::getInstance()->warning
    (
      "Step \"" + this->getName() + " did not succeed in setting arguments, skipping onTrigger().",
      "cedar::proc::Step::onTrigger(cedar::proc::ArgumentsPtr, cedar::proc::TriggerPtr)"
    );*/
    return;
  }

  // if we get to this point, set the state to running
  this->setState(cedar::proc::Triggerable::STATE_RUNNING, "");

  if (this->_mRunInThread->getValue())
  {
    // if the thread was already running, start() would have no effect, so that should not happen
    CEDAR_DEBUG_NON_CRITICAL_ASSERT(!this->isRunning());
    // start this step's thread (this will eventually call the run method)
    this->start();
  }
  else
  {
    // call the run function directly
    this->run();
  }
}


void cedar::proc::Step::run()
{
  if (this->mBusy.tryLock())
  {
    // Read out the arguments
    cedar::proc::ArgumentsPtr arguments;

    // if no arguments have been set, create default ones.
    this->mpArgumentsLock->lockForWrite();

    // check if arguments are set for the step.
    if (!this->mNextArguments)
    {
      // if not, use empty ones.
      arguments = cedar::proc::ArgumentsPtr(new cedar::proc::Arguments());
    }
    else
    {
      // read out the next arguments
      arguments = this->mNextArguments;

      this->mNextArguments.reset();
    }
    this->mpArgumentsLock->unlock();

    // start measuring the execution time.
    clock_t lock_start = clock();

    // lock the step
    this->lock(cedar::aux::LOCK_TYPE_READ);

    clock_t lock_end = clock();
    clock_t lock_elapsed = lock_end - lock_start;
    double lock_elapsed_s = static_cast<double>(lock_elapsed) / static_cast<double>(CLOCKS_PER_SEC);
    this->setLockTimeMeasurement(cedar::unit::Seconds(lock_elapsed_s));

    if (this->mLastComputeCall != 0)
    {
      clock_t last = this->mLastComputeCall;
      this->mLastComputeCall = clock();
      clock_t elapsed = this->mLastComputeCall - last;
      double elapsed_s = static_cast<double>(elapsed) / static_cast<double>(CLOCKS_PER_SEC);
      this->setRoundTimeMeasurement(cedar::unit::Seconds(elapsed_s));
    }
    else
    {
      this->mLastComputeCall = clock();
    }

    // start measuring the execution time.
    clock_t run_start = clock();

    try
    {
      /* since the cv::RNG is initialized once for every thread, we have to store its state for subsequent calls
       * to run() - otherwise, the sequence of generated random numbers will be identical for each execution of run()
       */
      if (this->isThreaded() && mRNGState != 0)
      {
        cv::RNG& my_rng = cv::theRNG();
        my_rng.state = mRNGState;
      }
      // call the compute function with the given arguments
      this->compute(*(arguments.get()));
      if (this->isThreaded())
      {
        cv::RNG& my_rng = cv::theRNG();
        mRNGState = my_rng.state;
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

    clock_t run_end = clock();
    clock_t run_elapsed = run_end - run_start;
    double run_elapsed_s = static_cast<double>(run_elapsed) / static_cast<double>(CLOCKS_PER_SEC);

    // unlock the step
    this->unlock();

    // take time measurements
    this->setRunTimeMeasurement(cedar::unit::Seconds(run_elapsed_s));

    // remove the argumens, as they have been processed.
    this->getFinishedTrigger()->trigger();

    this->mBusy.unlock();
  }
  else
  {
    cedar::aux::LogSingleton::getInstance()->warning
    (
      "Step \"" + this->getName() + "\" was busy during its run() call, computation skipped.",
      "cedar::proc::Step::run()"
    );
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
    return this->mMovingAverageIterationTime.getNewest();
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
    return this->mLockingTime.getNewest();
  }
  else
  {
    CEDAR_THROW(cedar::proc::NoMeasurementException, "No measurements, yet.");
  }
}

cedar::unit::Time cedar::proc::Step::getRunTimeAverage() const
{
  QReadLocker locker(&this->mLastIterationTimeLock);
  if (this->mMovingAverageIterationTime.size() > 0)
  {
    return this->mMovingAverageIterationTime.getAverage();
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
    return this->mLockingTime.getAverage();
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
    return this->mRoundTime.getNewest();
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
    return this->mRoundTime.getAverage();
  }
  else
  {
    CEDAR_THROW(cedar::proc::NoMeasurementException, "No measurements, yet.");
  }
}

void cedar::proc::Step::setThreaded(bool isThreaded)
{
  this->_mRunInThread->setValue(isThreaded);
}

/*!
 * @returns True, if the arguments were set successfully, false otherwise.
 *
 * @remarks The function returns false only if arguments have previously been set that were not yet processed by the
 *          run function.
 */
bool cedar::proc::Step::setNextArguments(cedar::proc::ArgumentsPtr arguments)
{
  // first, check if new arguments have already been set.
  {
    QReadLocker arg_lock(mpArgumentsLock);
    if (this->isRunning() || !this->mBusy.tryLock() || this->mNextArguments)
    {
      return false;
    }
  }

  QWriteLocker arg_lock(mpArgumentsLock);
  this->mNextArguments = arguments;
  arg_lock.unlock();
  this->mBusy.unlock();
  return true;
}

bool cedar::proc::Step::isThreaded() const
{
  return this->_mRunInThread->getValue();
}
