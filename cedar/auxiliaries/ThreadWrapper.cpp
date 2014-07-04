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

    File:        ThreadWrapper.cpp

    Maintainer:  Stephan Zibner
    Email:       stephan.zibner@ini.rub.de
    Date:        2010 12 02

    Description: Implementation of the @em cedar::aux::ThreadWrapper class.

    Credits:

======================================================================================================================*/

// CEDAR INCLUDES
#include "cedar/auxiliaries/ThreadWrapper.h"
#include "cedar/auxiliaries/detail/ThreadWorker.h"
#include "cedar/auxiliaries/Log.h"
#include "cedar/auxiliaries/stringFunctions.h"
#include "cedar/auxiliaries/exceptions.h"

// SYSTEM INCLUDES
#include <QWriteLocker>
#include <QReadLocker>
#include <QMutexLocker>
#ifndef Q_MOC_RUN
  #include <boost/lexical_cast.hpp>
#endif

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
  //std::cout << "destructor for this: " << this << " and thread " << mpThread << std::endl;  

  // If other threads enter start() or stop() after this point,
  // they will test mDestructing and abort.
  mDestructing = true;
    // note: you don't want them to wait for the destructor, but to abort
    //       as quickly as possible. Thats why we dont use a lock, here.
    // note: mDestructing is only written-to here!

  if (!mDestructingMutex.tryLock())
  {
    // You have a BIG problem.
    // This check is just performed for user-convenience, it tests
    // for a major flaw in the ownership-handling of this object and
    // may help finding errors. The destructor should not be called twice!
    CEDAR_THROW(cedar::aux::ThreadingErrorException, 
                "cedar::aux::ThreadWrapper::~ThreadWrapper() called twice! "
                "Ownership is probably being released by two separate "
                "threads at the same time.");
    // note: never unlock
  }

  // If other threads had already entered start() or stop(), _we_ wait:
  QMutexLocker locker(&mGeneralAccessLock);
  locker.unlock(); // never permanently lock _anything_ in the destructor!
                   // (there is always the possibility that a concurrent
                   // thread will wait for that lock to be released
                   // and then continue at the destructed objects address
                   // or spin the destructed lock)

  // we also wait for finishedThread() to execute:
  mFinishedThreadMutex.lock(); 
  mFinishedThreadMutex.unlock();

  // //////////////////////////////////////////////////////////////////////////
  //
  // We are now sure that there is no concurrent running start(), stop() or
  // finishedThread() after this point. this means, only we will work on
  // the mpWorker and mpThread objects.
  //
  // /////////////////////////////////////////////////////////////////////////

  if (validWorker())
  {
    requestStop(); // set stopped state first, 
                   // note: this may help some workers to exit cleanly/quickly
  }

  if (isValidThread())
  {
    mpThread->disconnect(this); // dont execute directly connected slots
    this->disconnect(mpThread); // ... (especially the one quit() calls)
    mpThread->quit(); // end the event loop

    // avoid dead-locking if called from the same thread:
    if (QThread::currentThread() != mpThread)
    {
      // need to wait for the thread to finish of its own accord, to make sure
      // the worker doesnt continue with work() (or step()) after we 
      // have been detructed
      mpThread->wait(); // blocks until finished
    }
    else
    {
      // The execution of work() (or step()) led to the destruction of this.

      // This will only go well, if step() immediately aborts afterwards.
      // note: requestStop() has already been sent, so test for that

      cedar::aux::LogSingleton::getInstance()->warning
      (
        "Destroying ThreadWrapper from thread it is representing. "
        "This may lead to obscure errors.",
        "cedar::aux::ThreadWrapper::~ThreadWrapper()"
      );
    }
  }

  // free worker data
  if (validWorker())
  {
    delete mpWorker;
  }

  // free thread data
  if (isValidThread())
  {
    delete mpThread;
  }
}

//------------------------------------------------------------------------------
// methods
//------------------------------------------------------------------------------

bool cedar::aux::ThreadWrapper::isRunning() const
{
  // no locks, because they really slow things down, here ...
  //!@todo: can I have a lockless-version that only is called from mpThread?

  if (mpThread == NULL)
    return false;

  return mpThread->isRunning();
}

void cedar::aux::ThreadWrapper::start()
{
  if (mDestructing) // quick abort if already destructing
    return;         // this if is not strictly necessary
                    // but may abort faster

  // make sure we only enter one and one only of start(), stop() at a time:
  QMutexLocker locker(&mGeneralAccessLock);
       
  if (mDestructing) // dont start the thread if we are in the destructor
    return;         // this if is neceassary
      // note:
      // mDestructing is a bool and is only written-to in the destructor.
      // A corrupted read here will either result in a:
      //   correct FALSE: no problem
      //   correct TRUE:  no problem
      //   incorrect TRUE: doesnt happen, since is only changed to TRUE,
      //                    and that only once
      //   incorrect FALSE: (i.e. changed to TRUE but we incorrectly
      //                   read as FALSE) the Destructor will wait for the 
      //                   GeneralAccessLock released at the end of start

  if (true)
  {
    QWriteLocker locker(&mStopRequestedLock);
    mStopRequested = false;
  }

  // EXPLANATION:  
  //              We initialize a new thread (a QThread object) and
  //              a worker object (a QObject with a custom work() method).
  //              The worker is then pushed into the QThreads' object thread.
  //
  //              Communication is done via the QT event loops (also of the
  //              QThread object).

  CEDAR_ASSERT(validWorker() == isValidThread());

  if (this->isRunning())
  {
    cedar::aux::LogSingleton::getInstance()->warning
    (
      "Thread has already been started. Ignoring.",
      "cedar::aux::ThreadWrapper::start()"
    );
    return; // already running, don't do anything
  }
  else if (!this->isValidThread())
  {
    CEDAR_ASSERT(!this->validWorker());

    // we need a new worker object:
    mpWorker = resetWorker(); // overridden by children
    mpThread = new QThread();

    mpWorker->moveToThread(mpThread); // workers event loop belongs to
                                      // the new thread

    // //////// connect slots:
    // An important note: incoming slots to *this need to be connected
    // via Qt:DirectConnection and not be queued(!), because ...
    //   There is an obscure case that the thread holding (starting) *this 
    //   will wait() on this* later on.
    //   Then this' EventLoop will not be executed if the holding thread 
    //   waits and the slots will not be called when mpThread finishes)
    //

    // notify me when thread starts, as I would like to react to it:
    connect( mpThread, SIGNAL(started()), this, SLOT(startedThreadSlot()), Qt::DirectConnection );
    // when the thread starts, start the work() in the worker:
    connect( mpThread, SIGNAL(started()), mpWorker, SLOT(workSlot()), Qt::QueuedConnection );

    // when the thread finishes (returns from run()), react to it:
    connect( mpWorker, SIGNAL(finishedWorking()), this, SLOT(finishedWorkSlot()), Qt::DirectConnection );
    // this will be called on a thread termination (should not happen):
    connect( mpThread, SIGNAL(finished()), this, SLOT(quittedThreadSlot()), Qt::DirectConnection );
  }
  else
  {
    CEDAR_THROW(cedar::aux::ThreadingErrorException, 
      "Re-starting a not cleanly finished thread. Continuing.");
  }

  mStartSignal();

  // start the thread 
  mpThread->start();
}

void cedar::aux::ThreadWrapper::startedThreadSlot()
{
  // dummy slot. only here for debuggin purposes atm

  //std::cout << "called startedThread() for " << this << " thread: " << mpThread << std::endl;  
}

void cedar::aux::ThreadWrapper::finishedWorkSlot()
{
  // is called in the new thread's context(!)

  // std::cout << "finished work" << std::endl;

  QThread::currentThread()->quit();
}

void cedar::aux::ThreadWrapper::quittedThreadSlot()
{
  CEDAR_ASSERT( QThread::currentThread() == mpThread );
  // is called in the new thread's context(!)

  // we land here after the thread ended
  if (mDestructing) // quick abort, see below
    return;

  //std::cout << "called quittedThreadSlot() for " << this << std::endl;  

  // make sure we enter this function only once:
  QMutexLocker locker(&mFinishedThreadMutex);
    // note, mGeneralAccessLock can already be held by stop(), so dont use it

  // note, we cannot test isRunning(), here, per premise that we
  // are operating without locks
  //!@todo isRunning() has no locks anymore. still valid?

  if (mDestructing) // always test after locking, see start()
    return;

  if (isValidThread())
  {
    // if my own stop() is not running, then delete the pointers:
    // (this is a fall-back, if the thread was destroyed by any other means)
    if (mGeneralAccessLock.tryLock())
    {
      //std::cout << "deleting thread: " << mpThread << " ( current thread: " << QThread::currentThread() << ") in: " << this << std::endl;

      scheduleThreadDeletion();

      mGeneralAccessLock.unlock();
    }
  }
  else
  {
    CEDAR_ASSERT( !validWorker() );
  }

  emit finishedThread();
}

// this method needs to be exception safe
void cedar::aux::ThreadWrapper::scheduleThreadDeletion()
{
  QMutexLocker locker(&mThreadPointerDeletionMutex);

  if (mpThread != NULL)
  {
    CEDAR_ASSERT( mpWorker != NULL );

    auto tmp_thread = mpThread;
    auto tmp_worker = mpWorker;

    // allow me to restart a new thread/worker
    mpThread = NULL;
    mpWorker = NULL;

  //!@todo this will leak IF the upper thread will not have time to tick its event loop (ie when shutting down the app): not that important
  //!@todo it will also lead to a destruction of a held mutex (in helgrind). not that important
    tmp_thread->deleteLater();
    tmp_worker->deleteLater();
  }
}

bool cedar::aux::ThreadWrapper::validWorker() const
{
  // this is intentionally not thread-safe
  return mpWorker != NULL;
}

bool cedar::aux::ThreadWrapper::isValidThread() const
{
  // this is intentionally not thread-safe
  return mpThread != NULL;
}


void cedar::aux::ThreadWrapper::stop(unsigned int time, bool suppressWarning)
{
  if (mDestructing) // quick abort, see below
    return;

  //std::cout << "called stop() for " << this << " ( current thread: " << QThread::currentThread() << ")" << std::endl;

  // make sure we wait for a running start() or finishedThread() or
  // only enther stop() once:
  QMutexLocker lockerGeneral(&mGeneralAccessLock);

  if (mDestructing) // see start()
    return;

  requestStop(); // change internal state, will abort the thread earlier
 
  // cant wait for mFinishedThreadMutex here, because that will dead-lock

  if (isRunning())
  {
    mStopSignal(suppressWarning);
      // intentionally called while the thread may still be running. 
      // we need to guarantee that the worker class hasn't been destroyed, yet.
      // This is only possible here or in quittedThreadSlot(). 
      // But historically, stop() also carries the suppressWarning parameter,
      // which we only have access too, so we call applyStop() here.

    // avoid dead-locking if called from the same thread:
    if (QThread::currentThread() != mpThread)
    {
      CEDAR_ASSERT(this->mpThread != NULL);
      // std::cout << "  waiting for thread: " << mpThread << std::endl;      
      // std::cout << "  (current thread: " << QThread::currentThread() << std::endl;

      mpThread->wait(time);
      //std::cout << "  resuming from wait." << std::endl;      
    }
  }

  if (this->isRunning())
  {
    if (QThread::currentThread() != mpThread)
    {
      CEDAR_DEBUG_ASSERT(this->mpThread != NULL);

      cedar::aux::LogSingleton::getInstance()->warning
      (
#ifdef DEBUG
        "Thread " 
        + boost::lexical_cast<std::string>(this)
        + " is still running after call of stop()!",
#else
        "Thread is still running after call of stop()!",
#endif      
        "cedar::aux::ThreadWrapper::stop(unsigned int, bool)"
      );
    }
    else
    {
      cedar::aux::LogSingleton::getInstance()->warning
      (
        "Trying to stop() the currently running thread will not work as "
        "expected, prefer calling requestStop() to quit thread after "
        "current execution yields (current LoopedThread::step() finishes).",
        "cedar::aux::ThreadWrapper::stop(unsigned int, bool)"
      );
    }
  }

  scheduleThreadDeletion();
}

void cedar::aux::ThreadWrapper::requestStop()
{
  QWriteLocker locker(&mStopRequestedLock);

  mStopRequested = true;
}

bool cedar::aux::ThreadWrapper::stopRequested()
{
  QReadLocker locker(&mStopRequestedLock);
	bool value = mStopRequested;
  return value;
}

boost::signals2::connection cedar::aux::ThreadWrapper::connectToStopSignal(boost::function<void (bool)> slot)
{
  return mStopSignal.connect(slot);
}

boost::signals2::connection cedar::aux::ThreadWrapper::connectToStartSignal(boost::function<void ()> slot)
{
  return mStartSignal.connect(slot);
}

void cedar::aux::ThreadWrapper::applyStart()
{
  // deprecated
}

void cedar::aux::ThreadWrapper::applyStop(bool)
{
  // deprecated
}

