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

    File:        LoopedThread.cpp

    Maintainer:  Bjoern Weghenkel
    Email:       bjoern.weghenkel@ini.rub.de
    Date:        2010 12 02

    Description: Implementation of the @em cedar::aux::LoopedThread class.

    Credits:

======================================================================================================================*/


// LOCAL INCLUDES
#include "auxiliaries/LoopedThread.h"

// PROJECT INCLUDES

// SYSTEM INCLUDES
//#include <QTime>


using namespace std;
using namespace boost::posix_time;


//------------------------------------------------------------------------------
// constructors and destructor
//------------------------------------------------------------------------------

cedar::aux::LoopedThread::LoopedThread(unsigned long stepSize, unsigned int idleTime)
{
  _mName = string("thread");
  mStepSize = microseconds(stepSize);
  mIdleTime = idleTime;
  mStop  = false;
  mUseFixedStepSize = true;
  mSimulatedTime = microseconds(0);
  initStatistics();
}


cedar::aux::LoopedThread::~LoopedThread()
{
}


//------------------------------------------------------------------------------
// methods
//------------------------------------------------------------------------------

void cedar::aux::LoopedThread::stop(unsigned int time, bool suppressWarning)
{
  mStop = true;
  wait(time);
  if (suppressWarning == false && mMaxStepsTaken > 1.01 && mSimulatedTime.total_microseconds() == 0)
  {
    cout << "Warning: The system was not fast enough to stay to scheduled thread timing. ";
    cout << "Consider using a larger step size." << endl;
    cout << "Execution stats:" << endl;
    cout << "  avg. time steps between execution: " << (double) mSumOfStepsTaken / (double) mNumberOfSteps << endl;
    cout << "  max. time steps between execution: " << mMaxStepsTaken << endl;
  }
}


void cedar::aux::LoopedThread::run(void)
{
  // we do not want to change the step size while running
  time_duration step_size = mStepSize;
  bool use_fixed_step_size = mUseFixedStepSize;
  initStatistics();

  // which mode?
  if(step_size.total_microseconds() == 0)
  {
    // fast running mode

    mLastTimeStepStart = microsec_clock::universal_time();
    mLastTimeStepEnd = microsec_clock::universal_time();
    time_duration time_difference;

    while(!mStop)
    {
      usleep(mIdleTime);
      mLastTimeStepStart = mLastTimeStepEnd;
      mLastTimeStepEnd = microsec_clock::universal_time();

      if(mSimulatedTime.total_microseconds() == 0)
      {
        time_difference = mLastTimeStepEnd - mLastTimeStepStart;
        step(time_difference.total_microseconds());
      }
      else
      {
        step(mSimulatedTime.total_microseconds());
      }
    }
  }
  else // if(step_size == 0)
  {
    // regular mode with time slots

    // initialization
    mLastTimeStepStart = microsec_clock::universal_time();
    mLastTimeStepEnd = microsec_clock::universal_time();
    ptime scheduled_wakeup = mLastTimeStepEnd + step_size;
    srand(microsec_clock::universal_time().time_of_day().total_milliseconds());

    // some auxiliary variables
    time_duration sleep_duration = microseconds(0);
    time_duration step_duration = microseconds(0);

    while(!mStop)
    {
      // sleep until next wake up time
      sleep_duration = scheduled_wakeup - microsec_clock::universal_time();
      usleep(std::max<int>(0, sleep_duration.total_microseconds()));

      // determine time since last run
      mLastTimeStepStart = mLastTimeStepEnd;
      mLastTimeStepEnd = scheduled_wakeup;
      step_duration = mLastTimeStepEnd - mLastTimeStepStart;

      // calculate number of time steps taken
      double steps_taken = static_cast<double>(step_duration.total_microseconds()) / static_cast<double>(step_size.total_microseconds());
      unsigned int full_steps_taken = static_cast<unsigned int>( steps_taken + 0.5 );

      #ifdef DEBUG
      // print warning if time steps have been skipped
      if( steps_taken > 1.01 )
        cerr << "WARNING: " << steps_taken << " time steps taken at once! "
        << "Your system might be too slow for an execution interval of "
        << step_size.total_microseconds() << " microseconds. Consider using a "
        << "longer interval!" << endl;
      #endif

      // here we have to distinguish between the different working modes of the
      // thread. either we want to wake up on the next fixed time step or as
      // soon as possible if we are already late.
      if(use_fixed_step_size)
      {
        // update statistics
        updateStatistics(full_steps_taken);

        // call step function
        if(mSimulatedTime.total_microseconds() == 0)
        {
          step(full_steps_taken * step_size.total_microseconds());
        }
        else
        {
          step(mSimulatedTime.total_microseconds());
        }

        // schedule the next wake up
        while(scheduled_wakeup < microsec_clock::universal_time())
        {
          scheduled_wakeup = scheduled_wakeup + step_size;
        }

      }
      else // if(mUseFixedStepSize)
      {
        // update statistics
        updateStatistics(steps_taken);

        // call step function
        if(mSimulatedTime.total_microseconds() == 0)
        {
          step(steps_taken * step_size.total_microseconds());
        }
        else
        {
          step(mSimulatedTime.total_microseconds());
        }

        // schedule the next wake up
        scheduled_wakeup = max<ptime>(scheduled_wakeup + step_size, microsec_clock::universal_time());

      } // if(mUseFixedStepSize)

    } // while(!mStop)

  } // if(step_size != 0)

  mStop = false;
  return;
}


void cedar::aux::LoopedThread::initStatistics()
{
  mNumberOfSteps = 0;
  mSumOfStepsTaken = 0.0;
  mMaxStepsTaken = 0.0;
}


void cedar::aux::LoopedThread::updateStatistics(double stepsTaken)
{

  double old_sum = mSumOfStepsTaken;

  mNumberOfSteps += 1.0;
  mSumOfStepsTaken += stepsTaken;
  if(stepsTaken > mMaxStepsTaken)
  {
    mMaxStepsTaken = stepsTaken;
  }

  if(old_sum > mSumOfStepsTaken)
  {
    cerr << "Warning: Value overflow in thread statistics. Statistics will be reseted." << endl;
    initStatistics();
  }

  return;
}


void cedar::aux::LoopedThread::singleStep() {
  if(!isRunning())
  {
    if(mSimulatedTime.total_microseconds() == 0)
    {
      step(mStepSize.total_microseconds());
    }
    else
    {
      step(mSimulatedTime.total_microseconds());
    }
  }
}
