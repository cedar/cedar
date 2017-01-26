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

    File:        ThreadWrapper.h

    Maintainer:  Jean-Stephane Jokeit
    Email:       jean-stephane.jokeit@ini.rub.de
    Date:        2013 02 01

    Description: Header for the @em cedar::aux::ThreadWrapper class.

    Credits:

======================================================================================================================*/

#ifndef CEDAR_AUX_LOOPED_THREAD_WORKER_H
#define CEDAR_AUX_LOOPED_THREAD_WORKER_H

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/auxiliaries/Configurable.h"
#include "cedar/auxiliaries/DoubleParameter.h"
#include "cedar/auxiliaries/BoolParameter.h"
#include "cedar/auxiliaries/EnumParameter.h"
#include "cedar/auxiliaries/LoopMode.h"
#include "cedar/units/Time.h"
#include "cedar/units/prefixes.h"

// FORWARD DECLARATIONS
#include "cedar/auxiliaries/ThreadWrapper.fwd.h"
#include "cedar/auxiliaries/detail/ThreadWorker.fwd.h"

// SYSTEM INCLUDES
#include <QThread>
#include <QMutex>
#include <QReadWriteLock>
#include <QCoreApplication>
#ifndef Q_MOC_RUN
  #include <boost/date_time/posix_time/posix_time_types.hpp>
  #include <boost/signals2/signal.hpp>
  #include <boost/signals2/connection.hpp>
  #include <boost/function.hpp>
#endif
#include <string>
#include <iostream>
#include <climits>
#include <limits>


/*!@brief Base class for utility classes that start their own worker in a thread
 * 
 * Implementers should not use this class.
 *
 *@see LoopedThread, CallFunctionInThread
 */
class cedar::aux::ThreadWrapper : public QObject,
                                  virtual public cedar::aux::Configurable
{
  //----------------------------------------------------------------------------
  // macros
  //----------------------------------------------------------------------------

  Q_OBJECT

  //----------------------------------------------------------------------------
  // constructors and destructor
  //----------------------------------------------------------------------------
public:

  //! constructor
  ThreadWrapper();

  //!@brief Destructor
  virtual ~ThreadWrapper();

  //----------------------------------------------------------------------------
  // public methods
  //----------------------------------------------------------------------------
public:

  /*! stop the thread and wait for the worker to finish.
   *
   * stop() will block the execution and return after the worker thread
   * has finished (or the timeout expired).
   *
   * This method is thread-safe.
   * Re-entry or calling stop() twice will be detected and will 
   * be silently aborted.
   *
   * Note, the method can be made in a pseudo-non-blocking way if
   * timeout is set to a ridiculously small interval, but the preferred
   * way is to use requestStop() for this.
   * For stopping the thread from the thread's own context, it is also preferred
   * to use requestStop().
   *
   *@param timeout: a value != 0 will abort waiting on the worker thread after
   *        the timeout-period. Use with caution.
   * @see requestStop()
   */
  void stop(unsigned int timeout = UINT_MAX); 

  /*! start the thread and initialize the worker
   *
   * This method is thread-safe.
   * Re-entry or calling start() twice will be detected and will 
   * abort with a cedar-warning.
   */
  void start();

  /*!Tell the thread to abort as soon as possible
   *
   * This method does not block as it only registers the stop request.
   * If you wish to really stop the thread, use stop().
   * This method is thread-safe.
   * This is the preferred way to stop the thread from the thread itself.
   * 
   *@see: stopRequested(), stop()
   */
  void requestStop();

  /*!@brief Returns true if a stop request was sent. 
   *
   * This method is thread-safe.
   *@see: requestStop
   */
  bool stopRequested();


  //! wait for the thread to finish. The calling thread will pause! deprecated: use without timing parameter or use requestStop() and sleep() to avoid subtle bugs.
  CEDAR_DECLARE_DEPRECATED(bool wait(cedar::unit::Time time));
  //! wait for the thread to finish. The calling thread will pause!
  bool wait();
  
  /*! connect to the signal that is sent when the thread will be stopped via stop()
   * In most cases, you would want to call connectToQuitSignal()!
   *
   * The Signal will not be called if the thread already stopped before on its own (i.e. via a requestStop() or it finished).
   *
   * The signal is:
   * Called in context of the holding thread. 
   * The thread (and worker) may still be executing, but the requestStop
   * has been set.
   * Blocks the caller of stop(), but not the thread.
   *
   * Preconditions: the worker exists and its pointer is still valid.
   * @see: connectToQuitSignal
   */
  boost::signals2::connection connectToStopSignal(boost::function<void ()> slot);

  /*! connect to the signal that is sent when Thread quits (terminates normally)
   *
   * The signal is:
   * Called in context of the quitting thread. The thread and worker are
   * still allocated.
   * Blocks the quitting Thread.
   *
   * Preconditions: the worker exists and its pointer is still valid.
   */
  boost::signals2::connection connectToQuitSignal(boost::function<void ()> slot);

  /*! connect to the signal that is sent when the thread will start
   *
   * The signal is:
   * Called in context of the thread that called start().
   * Blocks that thread.
   * Called before the thread does anything else, i.e. before it really starts running.
   *
   * Preconditions: the worker exists and its pointer is still valid.
   */
  boost::signals2::connection connectToStartSignal(boost::function<void ()> slot);

  //! is the thread still running? (This method IS thread safe.)
  bool isRunning() const;

  //! is the thread still running? (This method is NOT thread safe.)
  bool isRunningNolocking() const;

  //! does this object represent the current Thread?
  bool isCurrentThread() const;

  //! Reallocate the worker and thread when re-starting? Default: yes
  void setReallocateOnStart(bool);
  //! Reallocate the worker and thread when re-starting?
  bool getReallocateOnStart();

public slots:
  //! slot called when thread finishes. context: the new thread
  void quittedThreadSlot(); 
  //! slot called when thread starts, context: the calling thread
  void startedThreadSlot();
  //! slot called when the worker finishes, context: the new thread
  virtual void finishedWorkSlot();

signals:
  //! signal is emitted when the worker normally finished its work.
  void finishedThread();

  //----------------------------------------------------------------------------
  // protected methods
  //----------------------------------------------------------------------------
protected:
  //! helper function to quit the thread
  void forceQuitThread(); // intentionally protected!

  //----------------------------------------------------------------------------
  // private methods
  //----------------------------------------------------------------------------
private:
  /*!@brief create a new Worker (base class) pointer
   * 
   * Creates a new Worker base class pointer and returns it to be used
   * as the new Worker class.
   *
   * Override in children and create the appropriate Worker class
   * (i.e. LoopedThreadWorker for a LoopedThread).
   *
   * Children also need to reset their own pointers to the 
   * Worker if they are holding one.
   *
   */
  virtual cedar::aux::detail::ThreadWorker* resetWorker() = 0;

  //! is the worker (still) in memory? thread-un-safe
  bool validWorker() const;
  //! is the thread (still) in memory? thread-un-safe
  bool isValidThread() const; 

  //! schedule the thread object for deletion
  void scheduleThreadDeletion();

  //----------------------------------------------------------------------------
  // members
  //----------------------------------------------------------------------------
private:
  //! the new thread's QThread holding object
  QThread* mpThread;
    // inentionally a raw pointer. will be destroyed via QT's deleteLater()
  //! whether the thread and worker will be reallocated on restart.
  bool mReallocateOnStart;

  //! are we currently destructing?
  mutable bool mDestructing; 
  //! catch double destructs
  mutable QMutex mDestructingMutex;
  //! lock for the quittedThreadSlot
  mutable QMutex mFinishedThreadMutex;
  //! lock for start() and stop()
  mutable QMutex mGeneralAccessLock;
  //! lock for mStopRequested
  mutable QReadWriteLock mStopRequestedLock;
  //! lock pointer ops
  mutable QMutex mThreadPointerDeletionMutex;

  //! Lock for mpThread and mpWorker
  mutable QReadWriteLock mThreadAndWorkerLock;

  mutable QReadWriteLock mReallocateOnStartLock;


  //!@brief stop is requested
  bool mStopRequested; 
  //! keep a raw pointer of my worker
  cedar::aux::detail::ThreadWorker* mpWorker;
    // inentionally a raw pointer. will be destroyed via QT's deleteLater()

  //! signal for thread has been stopped via stop()
  boost::signals2::signal<void ()> mStopSignal;
  //! signal for thread has quitted
  boost::signals2::signal<void ()> mQuitSignal;
  //! signal for thread will start
  boost::signals2::signal<void ()> mStartSignal;

}; // class cedar::aux::ThreadWrapper

#endif // CEDAR_AUX_LOOPED_THREAD_H
