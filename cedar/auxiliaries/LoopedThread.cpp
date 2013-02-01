/*======================================================================================================================

    Copyright 2011, 2012 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany

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

    File:        LoopedThread.cpp

    Maintainer:  Stephan Zibner
    Email:       stephan.zibner@ini.rub.de
    Date:        2010 12 02

    Description: Implementation of the @em cedar::aux::LoopedThread class.

    Credits:

======================================================================================================================*/

// CEDAR INCLUDES
#include "cedar/auxiliaries/LoopedThread.h"
#include "cedar/auxiliaries/Log.h"
#include "cedar/auxiliaries/stringFunctions.h"

// SYSTEM INCLUDES
#include <QWriteLocker>
#include <QReadLocker>

//------------------------------------------------------------------------------
// constructors and destructor
//------------------------------------------------------------------------------
cedar::aux::LoopedThread::LoopedThread
(
  double stepSize,
  double idleTime,
  double simulatedTime,
  cedar::aux::EnumId mode

)
:
mpThread(NULL),
mDestructing(false),
mpWorker(NULL),
_mStepSize(new cedar::aux::DoubleParameter(this, "step size", stepSize)),
_mIdleTime(new cedar::aux::DoubleParameter(this, "idle time", idleTime)),
_mSimulatedTime(new cedar::aux::DoubleParameter(this, "simulated time", simulatedTime)),
_mLoopMode
(
  new cedar::aux::EnumParameter
  (
    this,
    "loop mode",
    cedar::aux::LoopMode::typePtr(),
    mode
  )
)
{
}

cedar::aux::LoopedThread::~LoopedThread()
{
/*
// TODO: comment-in later
  if (!mDestructingMutex.tryLock())
  {
    // TODO: throw an error. You have a BIG problem.
    // this check is just performed for user-convenience, it tests
    // for a major flaw in the ownership-handling of this object and
    // may help finding errors. the destructor should not be called twice!
    // note: never unlock
  }
*/

  // don't lock mGeneralAccessLock permanently, because we dont want 
  // other threads to wait for the destructor in this thread. 
  // that would be very bad. instead do this:
  mDestructing = true;
    // mDestructing is only written-to here!
    // this will make new calls to start(), stop() and finishedThread()
    // return without modifying the QThread state and pointers.
    // see start() for an explanation why mDestructing doesnt need to be locked

  // already running calls to start(), stop() and finishedThread() that
  // have passed their respective test on mDestructing, have already set
  // the lock mGeneralAccessLock, before

  // now temporarily use the lock to wait for already running calls to
  // start(), stop() and finishedThread():
  QWriteLocker locker(&mGeneralAccessLock);
  locker.unlock(); // see above, dont permanently lock anything 
                   // in the destructor!

  // we are now sure that there is no concurrent running start(), stop() or
  // finishedThread() after this point. this means, only we will work on
  // the mpWorker and mpThread objects

  if (validWorker())
  {
    mpWorker->requestStop(); // set stopped state first, 
                             // do not enter next loop() iteration
                             // note: this may not stop the thread in time,
                             // but will help to exit cleanly for some workers
    // note: mpWorker->requestStop() needs to be thread-safe
  }

  if (validThread())
  {
    // if a thread is running, quit it
    mpThread->disconnect(this); // dont execute finishedThread()
    mpThread->quit();

    // avoid dead-locking if called from the same thread:
    if (QThread::currentThread() != mpThread)
    {
      // need to wait for the thread to quit, to make sure
      // the worker doesnt continue with step() after we are destructed

      mpThread->wait();
    }
    else
    {
      // The execution of step() in mpThread lead to the destruction of this.
      // This will only go well, if step() immediately aborts afterwards.
      // requestStop() has already been sent.

      //!@todo: emanate an warning?
      // destroying thread wrapper from active thread. ignore if not fatal
    }
  }

  // free worker data
  if (validWorker())
  {
    // delete after thread stopped
    mpWorker->deleteLater();
    mpWorker = NULL;
  }

  // free thread data
  if (validThread())
  {
    // delete after thread stopped
    mpThread->deleteLater();
    mpThread = NULL;
  }
}

//------------------------------------------------------------------------------
// methods
//------------------------------------------------------------------------------

bool cedar::aux::LoopedThread::isRunningUnlocked() const
{
  if (mpThread == NULL)
    return false;

  return mpThread->isRunning();
}

bool cedar::aux::LoopedThread::isRunning() const
{
  QReadLocker locker(&mGeneralAccessLock);

  return isRunningUnlocked();
}

void cedar::aux::LoopedThread::start()
{
  QWriteLocker locker(&mGeneralAccessLock);
  // make sure we only enter one start() at a time
  // the following tests on mDestructing and isRunningUnlocked() will thus yield
  // results that are valid for the rest of the function

  if (mDestructing) // dont start the thread if we are in the destructor
    return;         // always test after having locked
    // mDestructing is a volatile bool and is only written-to in the destructor
    // A corrupted read here will either result in a:
    //   correct FALSE: no problem
    //   correct TRUE:  no problem
    //   incorrect FALSE: doesnt happen, since is only changed to FALSE,
    //                    and that only once
    //   incorrect TRUE: (i.e. is just being set to FALSE but we incorrectly
    //                   read as TRUE) will be handled
    //                   like as if start() was called before the destructor


  // EXPLANATION: This object (LoopedThread) is a wrapper.
  //              users use it and may inherit it, modyfing its step() method.
  //
  //              We initialize a new thread (a QThread object) and
  //              a worker object (a QObject with a custom loop() method).
  //              The worker is then pushed into the QThreads' object thread.
  //
  //              The work is done in the workers loop() method.
  //
  //              Communication is done via the QT event loops (also of the
  //              QThread object).

  if (validWorker() != validThread())
  {
    // report an error/warning TODO 
    return; // should not happen
  }
  else if (isRunningUnlocked()) //!@todo Should output a warning/error
  {
    return; // already running, everything ok
  }
  else if (!validThread()) // ==> (!validThread && !validWorker())
  {
    // TODO: CEDAR_ASSERT( !validWorker() )

    // we need a new worker object
    mpWorker = new cedar::aux::LoopedThreadWorker(this);
    // we need a new thread object
    mpThread = new QThread();

    mpWorker->moveToThread(mpThread);

    // when the thread starts, start the loop() in the worker:
    connect( mpThread, SIGNAL(started()), mpWorker, SLOT(loop()) );
    // when the thread finishes (somebody called quit()), react to it:
    connect( mpThread, SIGNAL(finished()), this, SLOT(finishedThread()) );
  }
  // else: thread and worker already exist, but are stopped

  // start the thread 
  mpThread->start();
}

void cedar::aux::LoopedThread::finishedThread()
{
  // we land here if the thread quitted (ended normally)

  QWriteLocker locker(&mGeneralAccessLock);
  // make sure we enter this function only once

  // TODO: make sure everything is exception-free after this line!
  
  // we cannot test isRunning(), here per premise

  if (mDestructing) // always test after locking, see start()
    return;

  if (validThread())
  {
    mpThread->deleteLater();
    mpThread = NULL; // allow me to restart a new thread/worker
  }
  if (validWorker())
  {
    mpWorker->requestStop(); // not necessary, but doesnt hurt, either
    mpWorker->deleteLater();
    mpWorker = NULL; // allow me to restart a new thread/worker
  }
}

bool cedar::aux::LoopedThread::validWorker() const
{
  // this is intentionally not thread-safe

  if (mpWorker == NULL)
    return false;
  return true;
}

bool cedar::aux::LoopedThread::validThread() const
{
  // this is intentionally not thread-safe

  if (mpThread == NULL)
    return false;
  return true;
}


void cedar::aux::LoopedThread::stop(unsigned int time, bool suppressWarning)
{
  QWriteLocker locker(&mGeneralAccessLock);
  // make sure we wait for a running start() or finishedThread() or
  // only enther stop() once

  if (mDestructing) // see start()
    return;

  stopStatistics(suppressWarning);

  if (validWorker())
    mpWorker->requestStop(); // change internal state, wont destroy the object

  if (isRunningUnlocked())
  {
    mpThread->quit(); // this really quits the thread

    // avoid dead-locking if called from the same thread:
    if (QThread::currentThread() != mpThread)
    {
      mpThread->wait(); // TODO: wait(time)
        // synchronize with thread exit
    }

  }

  if (this->isRunningUnlocked())
  {
    cedar::aux::LogSingleton::getInstance()->warning
    (
      "Thread is still running after call of stop()!",
      "cedar::aux::LoopedThread::stop(unsigned int, bool)"
    );
  }
}

void cedar::aux::LoopedThread::stopStatistics(bool suppressWarning)
{
  // is intentionally thread un-safe

  if (!isRunningUnlocked()) // use thread-UN-safe variant
    return;

  if (!validWorker())
    return;

  unsigned long numberOfSteps = mpWorker->getNumberOfSteps();

  if (suppressWarning == false && numberOfSteps > 1.01 && getSimulatedTimeParameter() <= 0.0)
  {
    std::string message = "The system was not fast enough to stay to scheduled thread timing. ";
    message += "Consider using a larger step size.\n";
    message += "Execution stats:\n";
    message += "  avg. time steps between execution: ";
    message += cedar::aux::toString(numberOfSteps / static_cast<double>(numberOfSteps));
    message += "\n";
    message += "  max. time steps between execution: ";
    message += cedar::aux::toString(mpWorker->getMaxStepsTaken());

    cedar::aux::LogSingleton::getInstance()->warning
    (
      message,
      "cedar::aux::LoopedThread::stop(unsigned int, bool)"
    );
  }
}


void cedar::aux::LoopedThread::singleStep()
{
  if (!this->isRunning()) // use thread-safe variant  
  {
    switch (_mLoopMode->getValue())
    {
      case cedar::aux::LoopMode::Simulated:
      {
        step(_mSimulatedTime->getValue());
        break;
      }
      default:
      {
        step(_mStepSize->getValue());
      }
    }
  }
  else
  {
    cedar::aux::LogSingleton::getInstance()->warning
    (
      "LoopedThread is already running, singleStep has no effect",
      "cedar::aux::LoopedThread::singleStep()"
    );
  }
}

// check if this method is needed, think of deprecating it
bool cedar::aux::LoopedThread::stopRequested()
{
  QReadLocker locker(&mGeneralAccessLock);

  if (this->isRunningUnlocked()
      && this->validWorker() 
      && mpWorker->stopRequested())
  {
    return true;
  }

  return false;
}

void cedar::aux::LoopedThread::setStepSize(double stepSize)
{
  this->_mStepSize->setValue(stepSize);
}

void cedar::aux::LoopedThread::setIdleTime(double idleTime)
{
  _mIdleTime->setValue(idleTime);
}

void cedar::aux::LoopedThread::setSimulatedTime(double simulatedTime)
{
  _mSimulatedTime->setValue(simulatedTime);
}

