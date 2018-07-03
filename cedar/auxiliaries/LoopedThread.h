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

    File:        LoopedThread.h

    Maintainer:  Stephan Zibner
    Email:       stephan.zibner@ini.rub.de
    Date:        2010 12 02

    Description: Header for the @em cedar::aux::LoopedThread class.

    Credits:

======================================================================================================================*/

#ifndef CEDAR_AUX_LOOPED_THREAD_H
#define CEDAR_AUX_LOOPED_THREAD_H

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/auxiliaries/detail/LoopedThreadWorker.h"
#include "cedar/auxiliaries/Configurable.h"
#include "cedar/auxiliaries/DoubleParameter.h"
#include "cedar/auxiliaries/BoolParameter.h"
#include "cedar/auxiliaries/EnumParameter.h"
#include "cedar/auxiliaries/LoopMode.h"
#include "cedar/auxiliaries/ThreadWrapper.h"
#include "cedar/auxiliaries/TimeParameter.h"
#include "cedar/units/prefixes.h"
#include "cedar/units/Time.h"

// FORWARD DECLARATIONS
#include "cedar/auxiliaries/LoopedThread.fwd.h"

// SYSTEM INCLUDES
#include <string>
#include <iostream>
#include <QThread>
#include <QMutex>
#include <QReadWriteLock>
#ifndef Q_MOC_RUN
  #include <boost/date_time/posix_time/posix_time_types.hpp>
#endif


/*!@brief Cedar interface for looped threads
 *
 * Use this interface if your class should execute a step() method
 * you provide in discrete time intervals in a thread.
 *
 * Just inherit from this class and implement a step() method with the
 * given parameter. This step() method is called in a loop until the thread is
 * stopped again.
 *
 * - to start ticking step(), call start()
 * - to set the time intervals the step() method is to be called, use 
 *   setTimeStep(). But do this before calling start()!
 * - to stop ticking step(), call stop() - note that this will abort the loop
 *   but not terminate the current iteration until it is finished.
 *
 * The current implementation of LoopedThread uses QT threads and needs a
 * running main event loop, i.e. you must start a Q(Core)Application's exec().
 * 
 * LoopedThread has several different working modes regarding the timing and the
 * handling of unexpected time delays. By default the step() method is called
 * in fixed intervals controlled by stepTime. If the system becomes too slow to
 * stay to the scheduled time interval the affected steps are skipped and for
 * the next regular step the step() method is called with the corresponding
 * multiple of stepSize as an argument. 
 * If you do not want to wait for the next regular step in cases of delay
 * but want to execute the next step as soon as possible, this
 * behavior can be controlled through useFixedStepSize().
 *
 * Also the behavior changes if a stepSize of zero is given. In this case step()
 * is called in a loop as fast as possible. There's only a short sleep in
 * between to keep the system responsive. This idle time can be controlled by
 * setIdleTime().
 *
 * An additional way to use LoopedThread is the simulation mode. If
 * simulatedTime is set to a value above zero, the step method is called with
 * this simulated time instead of the real (measured) time. This can be useful
 * if only the behavior of a dynamical system is of interest but not it's
 * relation to real time.
 *
 * Note that every child class must implement step(). If you design your own wrapper
 * thread with multiple objects that inherit the thread interface, you can call
 * all step functions consecutively and also pass measured time to each step function
 * to fulfill real-time constraints.
 *
 * The preferred way to stop the thread from itself is to call requestStop().
 */
class cedar::aux::LoopedThread : public cedar::aux::ThreadWrapper
{
  Q_OBJECT

  //----------------------------------------------------------------------------
  // friends
  //----------------------------------------------------------------------------
 
  friend class cedar::aux::detail::LoopedThreadWorker;

  //----------------------------------------------------------------------------
  // constructors and destructor
  //----------------------------------------------------------------------------
public:

  /*!@brief Constructor with step size parameter.
   *
   * This constructor creates a LoopedThread with certain time interval
   * (stepSize) in which the step() method is called.
   *
   * If stepSize == 0 the step() method is called as fast as possible with a
   * short idle time in between to keep the system responsive.
   *
   * @param stepSize time window for each step() method in milliseconds
   * @param idleTime idle time (in milliseconds) used in fast running mode (i.e. stepSize = 0)
   * @param simulatedTime a fixed time that is sent to all connected Triggerables regardless of real execution time
   * @param mode the operational mode of this trigger
   */
  LoopedThread
  (
    cedar::unit::Time stepSize = cedar::unit::Time(20 * cedar::unit::milli * cedar::unit::seconds),
    cedar::unit::Time  idleTime = cedar::unit::Time(0.01 * cedar::unit::milli * cedar::unit::seconds),
    cedar::unit::Time  simulatedTime = cedar::unit::Time(1.0 * cedar::unit::milli * cedar::unit::seconds),
    cedar::aux::EnumId mode = cedar::aux::LoopMode::RealDT
                                              // changed, cedar 6.1
  );

  LoopedThread
  (
    cedar::aux::EnumId mode,
    cedar::unit::Time stepSize = cedar::unit::Time(20.0 * cedar::unit::milli * cedar::unit::seconds),
    cedar::unit::Time fakedStepTime = cedar::unit::Time(20.0 * cedar::unit::milli * cedar::unit::seconds)
  );

  //!@brief Destructor
  virtual ~LoopedThread();

  //----------------------------------------------------------------------------
  // public methods
  //----------------------------------------------------------------------------
public:
  /*!@brief Performs a single step with default step size (or simulated time).
   *
   * This has no effect if the thread is already running.
   * 
   * This method is not re-entrant. It is thread-safe if your step() implementation is thread-safe.
   */
  void singleStep(); 

  /*!@brief Sets a new step size
   *
   * Sets the a new time interval for calling the step() method. If the
   * thread is already running, the new value has its effect only after restarting the thread, i.e. calling stop() and start().
   *
   * @param stepSize the new step size in milliseconds
   */
  void setStepSize(cedar::unit::Time stepSize);

  /*!@brief Sets a new fake step size, that will be passed as the Euler step time*/
  void setFakeStepSize(cedar::unit::Time stepSize);

  /*!@brief Sets a minimum sleep time*/
  void setMinimumStepSize(cedar::unit::Time minSleep);


  /*!@brief Sets a new idle time.
   * 
   * WILL BE DEPRECATED SOON
   *
   * The given idle time controls how long the thread sleeps each round when
   * running in fast running mode (step size == 0). If your system becomes
   * unresponsive try a larger idle time.
   *
   * @param idleTime the new idle time in milliseconds
   */
  void setIdleTime(cedar::unit::Time idleTime = cedar::unit::Time(0.01 * cedar::unit::milli * cedar::unit::seconds));

  /*!@brief Sets a simulated time to be used in step().
   *
   * WILL BE DEPRECATED SOON
   *
   * If simulatedTime == 0 step() is called with the measured time.
   *
   * If simulatedTime > 0 instead of the real (measured) time, the given
   * simulated time is used to call step().
   *
   * @param simulatedTime the desired simulated step size in milliseconds
   */
  void setSimulatedTime(cedar::unit::Time simulatedTime
                          = cedar::unit::Time(0.0 * cedar::unit::milli * cedar::unit::seconds));



  //! get the duration of the fixed trigger step
  inline cedar::unit::Time getStepSize() const
  {
    QReadLocker locker(this->_mStepSize->getLock());
    // make a copy of the step time
    // otherwise the lock would unlock before the return statement
    cedar::unit::Time step_size = this->_mStepSize->getValue();
    return step_size;
  }

  //! get the duration of the fixed trigger step
  inline cedar::unit::Time getStepSize()
  {
    QReadLocker locker(this->_mStepSize->getLock());
    // make a copy of the step time
    // otherwise the lock would unlock before the return statement
    cedar::unit::Time step_size = this->_mStepSize->getValue();
    return step_size;
  }

  //! get the duration of the fake Euler step
  inline cedar::unit::Time getFakeStepSize() const
  {
    QReadLocker locker(this->_mFakeStepSize->getLock());
    // make a copy of the step time
    // otherwise the lock would unlock before the return statement
    cedar::unit::Time step_size = this->_mFakeStepSize->getValue();
    return step_size;
  }

  //! get the duration of the fixed trigger step
  inline cedar::unit::Time getFakeStepSize()
  {
    QReadLocker locker(this->_mFakeStepSize->getLock());
    // make a copy of the step time
    // otherwise the lock would unlock before the return statement
    cedar::unit::Time step_size = this->_mFakeStepSize->getValue();
    return step_size;
  }

  //! get the duration of the fake Euler step
  inline cedar::unit::Time getMinimumStepSize() const
  {
    QReadLocker locker(this->_mMinimumStepSize->getLock());
    // make a copy of the step time
    // otherwise the lock would unlock before the return statement
    cedar::unit::Time step_size = this->_mMinimumStepSize->getValue();
    return step_size;
  }

  //! get the duration of the fixed trigger step
  inline cedar::unit::Time getMinimumStepSize()
  {
    QReadLocker locker(this->_mMinimumStepSize->getLock());
    // make a copy of the step time
    // otherwise the lock would unlock before the return statement
    cedar::unit::Time step_size = this->_mMinimumStepSize->getValue();
    return step_size;
  }




  //! DEPRECATED get the idle time that is used in-between sending trigger signals
  inline cedar::unit::Time getIdleTimeParameter() const
  {
    QReadLocker locker(this->_mIdleTime->getLock());
    // make a copy of the step time
    // otherwise the lock would unlock before the return statement
    cedar::unit::Time idle_time = this->_mIdleTime->getValue();
    return idle_time;
  }

  //! DEPRECATED get the duration of the simulated time step
  inline cedar::unit::Time getSimulatedTimeParameter() const
  {
    QReadLocker locker(this->_mSimulatedTime->getLock());
    cedar::unit::Time simulated_time = this->_mSimulatedTime->getValue();
    return simulated_time;
  }




  //! get the loop mode
  inline cedar::aux::Enum getLoopModeParameter() const
  {
    QReadLocker locker(this->_mLoopMode->getLock());
    cedar::aux::Enum value = this->_mLoopMode->getValue();
    return value;
  }

  //! get the the time of the LoopedThread's last step() entry
  inline boost::posix_time::ptime getLastTimeStepStart() const
  {
    CEDAR_ASSERT(mpWorker != NULL);
    return mpWorker->getLastTimeStepStart();
  }

  //! get the end of the LoopedThread's last step() exit
  inline boost::posix_time::ptime getLastTimeStepEnd() const
  {
    CEDAR_ASSERT(mpWorker != NULL);
    return mpWorker->getLastTimeStepEnd();
  }

  //! get the duration of the LoopedThread's last step() execution
  inline boost::posix_time::time_duration getLastTimeStepDuration() const
  {
    CEDAR_ASSERT(mpWorker != NULL);
    return mpWorker->getLastTimeStepDuration();
  }

  void waitUntilStepped() const;
  
  //! Returns the average number of steps that were skipped during iteration.
  double getAverageStepsTaken() const;

  double getNumberOfStepsMissed() const;

  inline void setDebugMe(bool b)
  {
    CEDAR_ASSERT(mpWorker != NULL);
    mpWorker->setDebugMe(b);
  }


  //----------------------------------------------------------------------------
  // protected methods
  //----------------------------------------------------------------------------
protected:


  //----------------------------------------------------------------------------
  // private methods
  //----------------------------------------------------------------------------
private:
  void init();

  //! end counting the statistics. is not thread-safe
  void stopStatistics(); 

  //! overwritten method. return the new worker object for LoopedThread
  cedar::aux::detail::ThreadWorker* resetWorker();

  /*!@brief All calculations for each time step are put into step().
   *
   * @param time length of the time step to be calculated in milliseconds
   */
  virtual void step(cedar::unit::Time time) = 0;

  //! slot called when the thread finishes via stop().
  void processStop();

  //! Called when the thread is started and stopped; marks some parameters that cannot be changed at runtime const.
  void makeParametersConst(bool makeConst);

private slots:
  void modeChanged();
  //----------------------------------------------------------------------------
  // members
  //----------------------------------------------------------------------------
protected:

private:
  //! keep our own pointer of the worker. it is guaranteed that it is still valid when we use it in work() and stopStatistics()
  cedar::aux::detail::LoopedThreadWorker* mpWorker;

  boost::signals2::scoped_connection mStartConnection;
  boost::signals2::scoped_connection mStopConnection;

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

private:

  //!@brief desired length of a single step, in milliseconds
  cedar::aux::TimeParameterPtr _mStepSize;

  //!@brief desired length of fake Euler step (since v6.1) for appropriate modes
  cedar::aux::TimeParameterPtr _mFakeStepSize;

  //!@brief ensure the thread sleeps at least (since v6.1)
  cedar::aux::TimeParameterPtr _mMinimumStepSize;


  // WILL BE DEPRECATED
  //! parameter version of mIdleTime
  cedar::aux::TimeParameterPtr _mIdleTime;

  // WILL BE DEPRECATED
  //! parameter version of mSimulatedTime
  cedar::aux::TimeParameterPtr _mSimulatedTime;

  //! The loop mode of the trigger
  cedar::aux::EnumParameterPtr _mLoopMode;

}; // class cedar::aux::LoopedThread

#endif // CEDAR_AUX_LOOPED_THREAD_H
