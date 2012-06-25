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

    Maintainer:  Stephan Zibner
    Email:       stephan.zibner@ini.rub.de
    Date:        2010 12 02

    Description: Header for the @em cedar::aux::LoopedThread class.

    Credits:

======================================================================================================================*/

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

#ifndef CEDAR_AUX_LOOPED_THREAD_H
#define CEDAR_AUX_LOOPED_THREAD_H

// CEDAR INCLUDES
#include "cedar/auxiliaries/namespace.h"
#include "cedar/auxiliaries/Configurable.h"
#include "cedar/auxiliaries/DoubleParameter.h"
#include "cedar/auxiliaries/BoolParameter.h"
#include "cedar/auxiliaries/LoopMode.h"

// SYSTEM INCLUDES
#include <string>
#include <iostream>
#include <QThread>
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
 * if only the behavior of a dynamical system is of interest but not it's
 * relation to real time.
 *
 * Note that every child class must implement step(). If you design your own wrapper
 * thread with multiple objects that inherit the thread interface, you can call
 * all step functions consecutively and also pass measured time to each step function
 * to fulfill real-time constraints.
 *
 * \todo fill in doxygen comments for all member variables
 *
 * \todo Revise the parameters of the looped thread
 * \todo Use units instead of doubles
 */
class cedar::aux::LoopedThread : virtual public cedar::aux::Configurable,
                                 public QThread
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
   */
  LoopedThread
  (
    double stepSize = 1.0,
    double idleTime = 0.01,
    double simulatedTime = 1.0,
    cedar::aux::EnumId mode = cedar::aux::LoopMode::Fixed
  );

  //!@brief Destructor
  virtual ~LoopedThread();

  //----------------------------------------------------------------------------
  // public methods
  //----------------------------------------------------------------------------
public:

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
  void stop(unsigned int timeout = 500, bool suppressWarning = false);

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
  void setStepSize(double stepSize);

  /*!@brief Sets a new idle time.
   *
   * The given idle time controls how long the thread sleeps each round when
   * running in fast running mode (step size == 0). If your system becomes
   * unresponsive try a larger idle time.
   *
   * @param idleTime the new idle time in milliseconds
   */
  void setIdleTime(double idleTime = 0.01);

  /*!@brief Sets a simulated time to be used in step().
   *
   * If simulatedTime == 0 step() is called with the measured time.
   *
   * If simulatedTime > 0 instead of the real (measured) time, the given
   * simulated time is used to call step().
   *
   * @param simulatedTime the desired simulated step size in milliseconds
   */
  void setSimulatedTime(double simulatedTime = 0.0);


  /*!@brief Returns the last timesteps start time.
   *
   */
  boost::posix_time::ptime getLastTimeStepStart() const;


  /*!@brief Returns the last timesteps end time.
   *
   */
  boost::posix_time::ptime getLastTimeStepEnd() const;


  /*!@brief Returns the last timesteps duration.
   *
   */
  boost::posix_time::time_duration getLastTimeStepDuration() const;


  /*!@brief Returns true if the thread is running but stop() was called.
   *
   */
  bool stopRequested();


  //----------------------------------------------------------------------------
  // protected methods
  //----------------------------------------------------------------------------
protected:
  CEDAR_DECLARE_DEPRECATED(inline double getStepSizeParameter() const)
  {
    return this->getStepSize();
  }

  inline double getStepSize() const
  {
    return this->_mStepSize->getValue();
  }

  inline double getIdleTimeParameter() const
  {
    return this->_mIdleTime->getValue();
  }

  inline double getSimulatedTimeParameter() const
  {
    return this->_mSimulatedTime->getValue();
  }

  //----------------------------------------------------------------------------
  // private methods
  //----------------------------------------------------------------------------
private:
  virtual void run(); // the thread does its work here!

  void initStatistics();

  inline void updateStatistics(double stepsTaken);

  //----------------------------------------------------------------------------
  // members
  //----------------------------------------------------------------------------
protected:

private:
  bool mStop;
  unsigned int mIdleTime; //!< in microseconds
//  boost::posix_time::time_duration mSimulatedTime;
  // gather some statistics
  unsigned long mNumberOfSteps;
  double mSumOfStepsTaken;
  double mMaxStepsTaken;
  // remeber time stamps of last step
  boost::posix_time::ptime mLastTimeStepStart;
  boost::posix_time::ptime mLastTimeStepEnd;

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

private:
  //!@brief desired length of a single step, in milliseconds
  cedar::aux::DoubleParameterPtr _mStepSize;

  //! parameter version of mIdleTime
  cedar::aux::DoubleParameterPtr _mIdleTime;

  //! parameter version of mSimulatedTime
  cedar::aux::DoubleParameterPtr _mSimulatedTime;

  //! The loop mode of the trigger
  cedar::aux::EnumParameterPtr _mLoopMode;
}; // class cedar::aux::LoopedThread

#endif // CEDAR_AUX_LOOPED_THREAD_H
