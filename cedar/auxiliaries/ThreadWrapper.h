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
#include "cedar/auxiliaries/namespace.h"
#include "cedar/auxiliaries/Configurable.h"
#include "cedar/auxiliaries/DoubleParameter.h"
#include "cedar/auxiliaries/BoolParameter.h"
#include "cedar/auxiliaries/EnumParameter.h"
#include "cedar/auxiliaries/LoopMode.h"

// SYSTEM INCLUDES
#include <string>
#include <iostream>
#include <climits>
#include <QThread>
#include <QMutex>
#include <QReadWriteLock>
#include <QCoreApplication>
#include <boost/date_time/posix_time/posix_time_types.hpp>


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
   * timeout is set to a rediculously small interval, but the preferred
   * way is to use requestStop() for this.
   * For stopping the thread from the thread's own context, it is also preferred
   * to use requestStop().
   *
   *@param timeout: a value != 0 will abort waiting on the worker thread after
   *        the timeout-period. Use with caution.
   *@param suppressWarning: will be passed to applyStop()
   * @see requestStop()
   */
  void stop(unsigned int timeout = UINT_MAX, bool suppressWarning = false); 

  /*! start the thread and initialize the worker
   *
   * This method is thread-safe.
   * Re-entry or calling start() twice will be detected and will 
   * abort with a cedar-warning.
   */
  void start();

  //! is the thread still running?
  bool isRunning() const; 

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


  //! wait for the thread to finish. The calling thread will pause!
  inline bool wait(unsigned long time = ULONG_MAX)
  {
    bool ret = false;

    if (validThread())
    {
      ret = mpThread->wait(time);
    }
    else
    {
    }

    return ret;
  }

public slots:
  //! slot called when thread finishes. context: the new thread
  void quittedThreadSlot(); 
  //! slot called when thread starts, context: the calling thread
  void startedThreadSlot();
  //! slot called when the worker finishes, context: the new thread
  void finishedWorkSlot();

signals:
  //! signal is emitted when the worker normally finished its work.
  void finishedThread();


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

  /*! you may override in child. is called when a thread is about to finish
   *
   * Called in context of the holding thread. 
   * The thread (and worker) may still be executing, but the requestStop
   * has been set.
   *
   * Preconditions: the worker exists and its pointer is still valid.
   */
  virtual void applyStop(bool suppressWarning); 

  /*! you may override in child. is called when a thread is about to start
   *
   * Called in context of the holding thread. 
   *
   * Preconditions: the worker exists and its pointer is still valid.
   */
  virtual void applyStart();

  //! is the worker (still) in memory? thread-un-safe
  bool validWorker() const;
  //! is the thread (still) in memory? thread-un-safe
  bool validThread() const; 

  //----------------------------------------------------------------------------
  // members
  //----------------------------------------------------------------------------
private:
  //! the new thread's QThread holding object
  QThread* mpThread;
    // inentionally a raw pointer. will be destroyed via QT's deleteLater()

  //! are we currently destructing?
  mutable bool mDestructing; 
  //! catch double destructs
  mutable QMutex mDestructingMutex;
  //! lock for the quittedThreadSlot
  mutable QMutex mFinishedThreadMutex;
  //! lock for start() and stop()
  mutable QReadWriteLock mGeneralAccessLock;
  //! lock for mStopRequested
  mutable QReadWriteLock mStopRequestedLock;

  //!@brief stop is requested
  bool mStopRequested; 
  //! keep a raw pointer of my worker
  cedar::aux::detail::ThreadWorker* mpWorker;
    // inentionally a raw pointer. will be destroyed via QT's deleteLater()

}; // class cedar::aux::ThreadWrapper

#endif // CEDAR_AUX_LOOPED_THREAD_H
