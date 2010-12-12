/*======================================================================================================================

    Copyright 2011 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany

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

    Maintainer:  Bjoern Weghenkel
    Email:       bjoern.weghenkel@ini.rub.de
    Date:        2010 12 02

    Description: Header for the @em cedar::aux::LoopedThread class.

    Credits:

======================================================================================================================*/


#ifndef CEDAR_AUX_LOOPED_THREAD_H
#define CEDAR_AUX_LOOPED_THREAD_H

// LOCAL INCLUDES
#include "namespace.h"
#include "ConfigurationInterface.h"

// PROJECT INCLUDES

// SYSTEM INCLUDES
#include <string>
#include <iostream>
#include <QThread>
#include "boost/date_time/posix_time/posix_time_types.hpp" //no i/o just types


/*!@brief Cedar interface for looped threads
 *
 * Use this interface if your class should be executable as a Qt thread.
 * Just inherit from this class and implement a step function with the
 * given parameter. This step function is called in an loop until the thread is
 * stopped again.
 *
 * - to start your thread, call start()
 * - to stop your thread, call stop()
 * - before you delete your object, make sure that there was enough time
 *   to stop the current execution of step()
 *
 * LoopedThread has several different working modes regarding the timing and the
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
 * An additional way to use LoopedThread is the simulation mode. If
 * simulatedTime is set to a value above zero, the step function is called with
 * this simulated time instead of the real (measured) time. This can be useful
 * if only a the behavior of a dynamical system is of interest but not it's
 * relation to real time.
 *
 * Note that every child class must implement step(). If you design your own wrapper
 * thread with multiple objects that inherit the thread interface, you can call
 * all step functions consecutively and also pass measured time to each step function
 * to fulfill real-time constraints.
 */
class cedar::aux::LoopedThread : public cedar::aux::ConfigurationInterface, public QThread
{
  //----------------------------------------------------------------------------
  // macros
  //----------------------------------------------------------------------------
  //----------------------------------------------------------------------------
  // constructors and destructor
  //----------------------------------------------------------------------------
public:

  /*!@brief Constructor with step size parameter.
   *
   * This constructor creates a LoopedThread with certain time interval
   * (stepSize) in which the step() function is called.
   *
   * If stepSize == 0 the step() function is called as fast as possible with a
   * short idle time in between to keep the system responsive.
   *
   * @param stepSize time window for each step function in milliseconds
   * @param idleTime idle time (in milliseconds) used in fast running mode (i.e. stepSize = 0)
   * @param configFileName an optional configuration file for reading and writing thread configurations
   */
  LoopedThread(double stepSize = 1.0, double idleTime = 0.001, const std::string& configFileName = "");

  //!@brief Destructor
  virtual ~LoopedThread() = 0;

  //----------------------------------------------------------------------------
  // public methods
  //----------------------------------------------------------------------------
public:
  /*!@brief Executes step() in a while loop in fixed time intervals.
   *
   */
  virtual void run();

  /*!@brief All calculations for each time step are put into step().
   *
   * @param time length of the time step to be calculated in milliseconds
   */
  virtual void step(double time) = 0;

  /*!@brief Stops the thread.
   *
   * Since the thread might be busy executing step(), it makes sense to wait a
   * moment for the thread to finish its work.
   *
   * @param timeout the max. time to wait for the thread (in milliseconds).
   * @param suppressWarning by default a warning about occurring timing problems will be given
   */
  void stop(unsigned int timeout = 1000, bool suppressWarning = false);

  /*!@brief Performs a single step with default step size (or simulated time).
   *
   * This has no effect if the thread is already running.
   */
  void singleStep();

  /*!@brief Sets a new step size
   *
   * Sets the a new time interval for calling the step() function. If the
   * thread is running, the new value has its effect only after restarting it.
   *
   * @param stepSize the new step size in milliseconds
   */
  void setStepSize(double stepSize) { mStepSize = boost::posix_time::microseconds( static_cast<unsigned int>( 1000*stepSize+.5 ) ); };

  /*!@brief Sets a new idle time.
   *
   * The given idle time controls how long the thread sleeps each round when
   * running in fast running mode (step size == 0). If your system becomes
   * unresponsive try a larger idle time.
   *
   * @param idleTime the new idle time in milliseconds
   */
  void setIdleTime(double idleTime = 0.001) { mIdleTime = static_cast<unsigned int>( 1000*idleTime+0.5 ); };

  /*!@brief Decide if a fixed step size is used in cases of delay.
   *
   * Depending on whether a fixed step size is used or not, LoopedThread
   * behaves differently in cases where the system is not fast enough for the
   * desired step size. If useFixedStepSize = true (default) and a scheduled
   * execution of step() was missed, then a complete time step is skipped and
   * LoopedThread tries to wake up for the next time step. By this, all time
   * steps are executed on a predictable time. If useFixedStepSize = false and
   * the scheduled execution of step() was missed, the next execution happens
   * as soon as possible. Through this behavior a little time is saved but the
   * executions of step() do not happen to predictable times any more.
   *
   * @param useFixedStepSize
   */
  void useFixedStepSize(bool useFixedStepSize = true) { mUseFixedStepSize = useFixedStepSize; };


  /*!@brief Sets a simulated time to be used in step().
   *
   * If simulatedTime == 0 step() is called with the measured time.
   *
   * If simulatedTime > 0 instead of the real (measured) time, the given
   * simulated time is used to call step().
   *
   * @param simulatedTime the desired simulated step size in milliseconds
   */
  void setSimulatedTime(double simulatedTime = 0.0) { mSimulatedTime = boost::posix_time::microseconds( static_cast<unsigned>( 1000*simulatedTime+.5 ) ); };


  /*!@brief Returns the last timesteps start time.
   *
   */
  boost::posix_time::ptime getLastTimeStepStart() const { return mLastTimeStepStart; };


  /*!@brief Returns the last timesteps end time.
   *
   */
  boost::posix_time::ptime getLastTimeStepEnd() const { return mLastTimeStepEnd; };


  /*!@brief Returns the last timesteps duration.
   *
   */
  boost::posix_time::time_duration getLastTimeStepDuration() const { return mLastTimeStepStart - mLastTimeStepEnd; };


  //----------------------------------------------------------------------------
  // protected methods
  //----------------------------------------------------------------------------
protected:
  // none yet

  //----------------------------------------------------------------------------
  // private methods
  //----------------------------------------------------------------------------
private:
  void initStatistics(void);
  inline void updateStatistics(double stepsTaken);

  //----------------------------------------------------------------------------
  // members
  //----------------------------------------------------------------------------
public:
  // none yet (hopefully never!)
protected:
  boost::posix_time::time_duration mStepSize;
private:
  bool mStop;
  unsigned int mIdleTime; // in microseconds
  bool mUseFixedStepSize;
  boost::posix_time::time_duration mSimulatedTime;
  // gather some statistics
  unsigned long mNumberOfSteps;
  double mSumOfStepsTaken;
  double mMaxStepsTaken;
  //remeber time stamps of last step
  boost::posix_time::ptime mLastTimeStepStart;
  boost::posix_time::ptime mLastTimeStepEnd;


  //----------------------------------------------------------------------------
  // parameters
  //----------------------------------------------------------------------------
public:
  // none yet
protected:
  // none yet
private:
  // none yet

}; // class cedar::aux::Thread

#endif // CEDAR_AUX_THREAD_H
