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

    File:        Thread.h

    Maintainer:  Bjoern Weghenkel
    Email:       bjoern.weghenkel@ini.rub.de
    Date:        2010 12 02

    Description: Header for the @em cedar::aux::LoopedThread class.

    Credits:

======================================================================================================================*/


#ifndef CEDAR_AUX_THREAD_H
#define CEDAR_AUX_THREAD_H

// LOCAL INCLUDES
#include "namespace.h"
#include "Base.h"

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
 * Note that every child class must implement step(). If you design your own wrapper
 * thread with multiple objects that inherit the thread interface, you can call
 * all step functions consecutively and also pass measured time to each step function
 * to fulfill real-time constraints.
 */
class cedar::aux::LoopedThread : public Base, public QThread
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
   * This constructor sets the time between two executions of step() to a
   * certain value (in microseconds).
   *
   * @param stepSize time window for each step function in microseconds
   */
  LoopedThread(unsigned long stepSize = 1000);

  //!@brief Destructor
  virtual ~LoopedThread(void) = 0;

  //----------------------------------------------------------------------------
  // public methods
  //----------------------------------------------------------------------------
public:
  /*!@brief Executes step() in a while loop in fixed time intervals.
   *
   * If the system was not fast enough to execute step(time) in the desired
   * time, step(time) is called with the proper multiple of mStepSize to allow
   * you making up for the lost steps.
   */
  virtual void run();

  /*!@brief All calculations for each time step are put into step().
   *
   * @param time length of the time step to be calculated in microseconds
   */
  virtual void step(unsigned long time) = 0;

  /*!@brief Stops the thread.
   *
   * Since the thread might be busy executing step(), it makes sense to wait a
   * moment for the thread to finish its work.
   *
   * @param timeout the max. time to wait for the thread (in milliseconds).
   * @param suppressWarning by default a warning about occurring timing problems will be given
   */
  void stop( unsigned int timeout = 1000, bool suppressWarning = false );

  /*!@brief Performs a single step with default step size.
   *
   * This has no effect if the thread is already running.
   */
  void singleStep();

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
  inline void updateStatistics(unsigned stepsTaken);

  //----------------------------------------------------------------------------
  // members
  //----------------------------------------------------------------------------
public:
  // none yet (hopefully never!)
protected:
  boost::posix_time::time_duration mStepSize;
private:
  bool mStop;
  // gather some statistics
  long unsigned mNumberOfSteps;
  long unsigned mSumOfStepsTaken;
  unsigned mMaxStepsTaken;

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
