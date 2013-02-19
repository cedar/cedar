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

    File:        ThreadWrapper.h

    Maintainer:  Stephan Zibner
    Email:       stephan.zibner@ini.rub.de
    Date:        2010 12 02

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
#include <QThread>
#include <QMutex>
#include <QReadWriteLock>
#include <QCoreApplication>
#include <boost/date_time/posix_time/posix_time_types.hpp>


/*!@brief Cedar interface for looped threads
 *
 * Use this interface if your class should be executable as a Qt thread.
 * Just inherit from this class and implement a step function with the
 * given parameter. This step function is called in a loop until the thread is
 * stopped again.
 *
 * - to start your thread, call start()
 * - to stop your thread, call stop()
 * - before you delete your object, make sure that there was enough time
 *   to stop the current execution of step()
 *
 * ThreadWrapper has several different working modes regarding the timing and the
 * handling of unexpected time delays. By default the step() function is called
 * in fixed intervals controlled by stepTime. If the system becomes too slow to
 * stay to the scheduled time interval the affected steps are skipped and for
 * the next regular step the step() function is called with the corresponding
 * multiple of stepSize. If you do not want to wait for the next regular step in
 * cases of delay but want to execute the next step as soon as possible, this
 * behavior can be controlled through useFixedStepSize().
 *
 * Also the behavior changes if a stepSize of zero is given. In this case step()
 * is called in a loop as fast as possible. There's only a short sleep in
 * between to keep the system responsive. This idle time can be controlled by
 * setIdleTime().
 *
 * An additional way to use ThreadWrapper is the simulation mode. If
 * simulatedTime is set to a value above zero, the step function is called with
 * this simulated time instead of the real (measured) time. This can be useful
 * if only the behavior of a dynamical system is of interest but not it's
 * relation to real time.
 *
 * Note that every child class must implement step(). If you design your own wrapper
 * thread with multiple objects that inherit the thread interface, you can call
 * all step functions consecutively and also pass measured time to each step function
 * to fulfill real-time constraints.
 *
 * \todo Use units instead of doubles
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

  /*!@brief Constructor with step size parameter.
   *
   * This constructor creates a ThreadWrapper with certain time interval
   * (stepSize) in which the step() function is called.
   *
   * If stepSize == 0 the step() function is called as fast as possible with a
   * short idle time in between to keep the system responsive.
   *
   * @param stepSize time window for each step function in milliseconds
   * @param idleTime idle time (in milliseconds) used in fast running mode (i.e. stepSize = 0)
   * @param simulatedTime a fixed time that is sent to all connected Triggerables regardless of real execution time
   * @param mode the operational mode of this trigger
   */
  ThreadWrapper();

  //!@brief Destructor
  virtual ~ThreadWrapper();

  //----------------------------------------------------------------------------
  // public methods
  //----------------------------------------------------------------------------
public:

  /*!@brief Stops the thread.
   *
   * Since the thread might be busy executing step(), it makes sense to wait a
   * moment for the thread to finish its work.
   *
   * @param timeout the max. time to wait for the thread (in milliseconds).
   * @param suppressWarning by default a warning about occurring timing problems will be given
   */
  void stop(unsigned int timeout = 500, bool suppressWarning = false); // TODO: say it is thread-safe and re-entry will abort with a warning

  void start(); // TODO: manpage TODO: say it is thread-safe and re-entry will abort with a warning
  bool isRunning() const; // TODO manpage, TODO: say it is thread-safe

  // TODO: manpage
  void requestStop();
  /*!@brief Returns true if the thread is running but stop() was called.
   *
   */
  bool stopRequested();


// js: changed signature. now more like QT
//  inline void wait(int i = 0)
  inline bool wait(unsigned long time = 0)
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
  void finishedThreadSlot(); // thread-safe needs to be TODO
  void startedThreadSlot(); // dito TODO

  //----------------------------------------------------------------------------
  // protected methods
  //----------------------------------------------------------------------------
protected:

  //----------------------------------------------------------------------------
  // private methods
  //----------------------------------------------------------------------------
private:
  virtual cedar::aux::detail::ThreadWorker* resetWorker() = 0;
  void deleteWorkerUnlocked();

  virtual void applyStop(bool suppressWarning);
  virtual void applyStart();

  //----------------------------------------------------------------------------
  // members
  //----------------------------------------------------------------------------
protected:
  bool validWorker() const; // thread-UN-safe
  bool validThread() const; // thread-UN-safe
  bool isRunningUnlocked() const; // is NOT thread-safe

private:

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

private:
  QThread* mpThread;
  mutable bool mDestructing; 
  mutable QMutex mDestructingMutex;
  mutable QMutex mFinishedThreadMutex;

  //!@brief stop is requested
  volatile bool mStopRequested; // note: volatile disables some optimizations, but doesn't add thread-safety

protected:
  cedar::aux::detail::ThreadWorker* mpWorker;
private:
  mutable QReadWriteLock mGeneralAccessLock;

}; // class cedar::aux::ThreadWrapper

#endif // CEDAR_AUX_LOOPED_THREAD_H
