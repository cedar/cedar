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

    File:        ThreadWrapper.cpp

    Maintainer:  Stephan Zibner
    Email:       stephan.zibner@ini.rub.de
    Date:        2010 12 02

    Description: Implementation of the @em cedar::aux::ThreadWrapper class.

    Credits:

======================================================================================================================*/

// CEDAR INCLUDES
#include "cedar/auxiliaries/ThreadWrapper.h"
#include "cedar/auxiliaries/ThreadWorker.h"
#include "cedar/auxiliaries/Log.h"
#include "cedar/auxiliaries/stringFunctions.h"
#include "cedar/auxiliaries/exceptions.h"

// SYSTEM INCLUDES
#include <QWriteLocker>
#include <QReadLocker>
#include <QMutexLocker>

//------------------------------------------------------------------------------
// constructors and destructor
//------------------------------------------------------------------------------
cedar::aux::ThreadWrapper::ThreadWrapper()
:
mpThread(NULL),
mDestructing(false),
mStopRequested(false),
mpWorker(NULL)
{
}

cedar::aux::ThreadWrapper::~ThreadWrapper()
{
  if (!mDestructingMutex.tryLock())
  {
    // You have a BIG problem.
    // this check is just performed for user-convenience, it tests
    // for a major flaw in the ownership-handling of this object and
    // may help finding errors. the destructor should not be called twice!
    CEDAR_THROW(cedar::aux::ThreadingErrorException, 
                "cedar::aux::ThreadWrapper::~ThreadWrapper() called twice!");
    // note: never unlock
  }

  // If other threads enter start() or stop() after this point,
  // they will test mDestructing and abort.
  mDestructing = true;
    // note: you don't want them to wait for the destructor, but to abort
    // quickly
    // note: mDestructing is only written-to here!

  // If other threads had already entered start() or stop(), _we_ wait:
  QWriteLocker locker(&mGeneralAccessLock);
  locker.unlock(); // see above, dont permanently lock _anything_
                   // in the destructor!

  // we also wait for finishedThread() to execute:
  mFinishedThreadMutex.lock(); 

  ////////////////////////////////////////////////////////////////////////////
  //
  // We are now sure that there is no concurrent running start(), stop() or
  // finishedThread() after this point. this means, only we will work on
  // the mpWorker and mpThread objects.
  //
  ///////////////////////////////////////////////////////////////////////////

  if (validWorker())
  {
    requestStop(); // set stopped state first, 
                   // note: this may exit cleanly for some workers
  }

  if (validThread())
  {
    mpThread->disconnect(this); // dont execute finishedThread()
    mpThread->quit(); // end the event loop

    // avoid dead-locking if called from the same thread:
    if (QThread::currentThread() != mpThread)
    {
      // need to wait for the thread to finish of its own accord, to make sure
      // the worker doesnt continue with work() (or step()) after we 
      // have been detructed
      mpThread->wait();
    }
    else
    {
      // The execution of work() (or step()) led to the destruction of this.

      // This will only go well, if step() immediately aborts afterwards.
      // note: requestStop() has already been sent, so test for that
      //       if you need such a behavior.

      cedar::aux::LogSingleton::getInstance()->warning
      (
        "Destroying ThreadWrapper from thread it is holding. "
        "This may lead to obscure errors.",
        "cedar::aux::ThreadWrapper::~ThreadWrapper()"
      );
    }
  }

  // free worker data
  if (validWorker())
  {
    // delete after thread stopped
    mpWorker->deleteLater();
      // TODO: this may leak if the currents thread event loop wont
      //       get executed
    mpWorker = NULL;
  }

  // free thread data
  if (validThread())
  {
    // delete after thread stopped
    mpThread->deleteLater();
      // TODO: this may leak if the currents thread event loop wont
      //       get executed
    mpThread = NULL;
  }
}

//------------------------------------------------------------------------------
// methods
//------------------------------------------------------------------------------

bool cedar::aux::ThreadWrapper::isRunningUnlocked() const
{
  if (mpThread == NULL)
    return false;

  return mpThread->isRunning();
}

bool cedar::aux::ThreadWrapper::isRunning() const
{
  QReadLocker locker(&mGeneralAccessLock);

  return isRunningUnlocked();
}

void cedar::aux::ThreadWrapper::start()
{
  // make sure we only enter one and one of start(), stop() at a time:
  QWriteLocker locker(&mGeneralAccessLock);

  if (mDestructing) // dont start the thread if we are in the destructor
    return;        
    // note:
    // mDestructing is a volatile bool and is only written-to in the destructor
    // A corrupted read here will either result in a:
    //   correct FALSE: no problem
    //   correct TRUE:  no problem
    //   incorrect FALSE: doesnt happen, since is only changed to FALSE,
    //                    and that only once
    //   incorrect TRUE: (i.e. is just being set to FALSE but we incorrectly
    //                   read as TRUE) will be handled
    //                   like as if start() was called before the destructor


  // TODO: locking
  mStopRequested = false;

  applyStart();

  // EXPLANATION: This object (ThreadWrapper) is a wrapper.
  //              users use it and may inherit it, modyfing its step() method.
  //
  //              We initialize a new thread (a QThread object) and
  //              a worker object (a QObject with a custom work() method).
  //              The worker is then pushed into the QThreads' object thread.
  //
  //              The work is done in the workers work() method.
  //
  //              Communication is done via the QT event loops (also of the
  //              QThread object).

  if (validWorker() != validThread())
  {
    CEDAR_THROW(cedar::aux::ThreadingErrorException, 
                "cedar::aux::ThreadWrapper::start() incoherent internal state");
    return; // should not happen
  }
  else if (isRunningUnlocked())
  {
    cedar::aux::LogSingleton::getInstance()->warning
    (
      "Thread has already been started. Ignoring.",
      "cedar::aux::ThreadWrapper::start()"
    );
    return; // already running, don't do anything
  }
  else if (!validThread()) // ==> (!validThread && !validWorker())
  {
    // TODO: CEDAR_ASSERT( !validWorker() )

    // we need a new worker object:
    mpWorker = resetWorker(); // overridden by children

    // we need a new thread object:
    mpThread = new QThread();

    mpWorker->moveToThread(mpThread); // workers event loop belongs to
                                      // the new thread

    ////////// connect slots:
    // an important note: incoming slots to this object need to be connected
    // via Qt:DirectConnection and not be queued, in case that the thread
    // holding this object will wait on the mpThread. (i.e. this' EventLoop
    // will not be executed)
    //

    // notify me when thread starts
    connect( mpThread, SIGNAL(started()), this, SLOT(startedThreadSlot()), Qt::DirectConnection );
    // when the thread starts, start the work() in the worker:
    connect( mpThread, SIGNAL(started()), mpWorker, SLOT(workSlot()) );

    // when the thread finishes (returns from run()), react to it:
    connect( mpWorker, SIGNAL(signalFinishedWorking()), this, SLOT(finishedThreadSlot()), Qt::DirectConnection );
    // this will (hopefully) be called on a termination:
    connect( mpThread, SIGNAL(finished()), this, SLOT(finishedThreadSlot()), Qt::DirectConnection );
  }
  // else: thread and worker already exist, but are stopped

  // start the thread 
  mpThread->start();
}

void cedar::aux::ThreadWrapper::startedThreadSlot()
{
  // only here for debuggin purposes atm
  //std::cout << "called startedThread() for " << this << " thread: " << mpThread << std::endl;  
}

void cedar::aux::ThreadWrapper::applyStart()
{
  // virtual method. do your magic in the child class
}

void cedar::aux::ThreadWrapper::applyStop(bool)
{
  // virtual method. do your magic in the child class
}

void cedar::aux::ThreadWrapper::finishedThreadSlot()
{
  // we land here if the thread ended normally or terminated
  // thus may be called twice

  // std::cout << "called finishedThread() for " << this << std::endl;  

  // make sure we enter this function only once:
  QMutexLocker locker(&mFinishedThreadMutex);
    // note, mGeneralAccessLock can already be held by stop()

  if (mDestructing) // see start()
    return;

  deleteWorkerUnlocked();
}

void cedar::aux::ThreadWrapper::deleteWorkerUnlocked()
{
  // note, we cannot test isRunning(), here, per premise that we
  // are operating without locks

  if (mDestructing) // always test after locking, see start()
    return;

  if (validThread())
  {
    mpThread->deleteLater();
    mpThread = NULL; // allow me to restart a new thread/worker
  }
  if (validWorker())
  {
    requestStop(); // not strictly necessary, but doesnt hurt, either
    mpWorker->deleteLater();
    mpWorker = NULL; // allow me to restart a new thread/worker
  }
}

bool cedar::aux::ThreadWrapper::validWorker() const
{
  // this is intentionally not thread-safe

  if (mpWorker == NULL)
    return false;
  return true;
}

bool cedar::aux::ThreadWrapper::validThread() const
{
  // this is intentionally not thread-safe

  if (mpThread == NULL)
    return false;
  return true;
}


void cedar::aux::ThreadWrapper::stop(unsigned int time, bool suppressWarning)
{
  // make sure we wait for a running start() or finishedThread() or
  // only enther stop() once:
  QWriteLocker locker(&mGeneralAccessLock);

  if (mDestructing) // see start()
    return;

  requestStop(); // change internal state, will abort the thread earlier

  applyStop(suppressWarning);

  if (isRunningUnlocked())
  {
    // avoid dead-locking if called from the same thread:
    if (QThread::currentThread() != mpThread)
    {
      mpThread->wait(time);
        // synchronize with thread exit
    }

    // after wait() has returned we should have already deleted everything
    if (validThread())
    {
      // if not, at least stop the event loop
      mpThread->quit(); // this terminates the event-loop of the thread
    }

  }

  if (this->isRunningUnlocked())
  {
    cedar::aux::LogSingleton::getInstance()->warning
    (
      "Thread is still running after call of stop()!",
      "cedar::aux::ThreadWrapper::stop(unsigned int, bool)"
    );
  }
}

void cedar::aux::ThreadWrapper::requestStop()
{
  // this will stop and exit the loop (not the thread - the thread is stopped
  // by the wrapper LoopedThread)

  // TODO: needs to be locked!
  mStopRequested = true;
}

bool cedar::aux::ThreadWrapper::stopRequested()
{
  // TODO: locking
  return mStopRequested;
}


