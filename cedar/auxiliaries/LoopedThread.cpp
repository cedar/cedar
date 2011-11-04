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
#include "cedar/auxiliaries/LoopedThread.h"

// PROJECT INCLUDES

// SYSTEM INCLUDES
//#include <QTime>


using namespace std;
using namespace boost::posix_time;

// #define SHOW_TIMING_HELPERS

//------------------------------------------------------------------------------
// constructors and destructor
//------------------------------------------------------------------------------
cedar::aux::LoopedThread::LoopedThread(double stepSize, double idleTime,const std::string& configFileName)
:cedar::aux::ConfigurationInterface(configFileName)
{
  mStop  = false;
  initStatistics();

  mStepSize = microseconds(static_cast<unsigned int>(1000 * stepSize + 0.5));
  mIdleTime = static_cast<unsigned int>(1000 * idleTime + 0.5);
  mUseFixedStepSize = true;
  mSimulatedTime = microseconds(0);
}

cedar::aux::LoopedThread::LoopedThread(const std::string& configFileName)
  :cedar::aux::ConfigurationInterface(configFileName)
{
  mStop = false;
  initStatistics();
  readParamsFromConfigFile();
}

cedar::aux::LoopedThread::LoopedThread(const char* pConfigFileName)
  :cedar::aux::ConfigurationInterface(pConfigFileName)
{
  mStop = false;
  initStatistics();
  readParamsFromConfigFile();
}

cedar::aux::LoopedThread::~LoopedThread()
{
}

//------------------------------------------------------------------------------
// methods
//------------------------------------------------------------------------------

void cedar::aux::LoopedThread::stop(unsigned int time, bool suppressWarning)
{
  if(isRunning())
  {
    mStop = true;
    wait(time);

    if(isRunning())
    {
      cout << "Warning: Thread is still running after call of stop()!" << endl;
    }

    if(suppressWarning == false && mMaxStepsTaken > 1.01 && mSimulatedTime.total_microseconds() == 0)
    {
      cout << "Warning: The system was not fast enough to stay to scheduled thread timing. ";
      cout << "Consider using a larger step size." << endl;
      cout << "Execution stats:" << endl;
      cout << "  avg. time steps between execution: " << (double) mSumOfStepsTaken / (double) mNumberOfSteps << endl;
      cout << "  max. time steps between execution: " << mMaxStepsTaken << endl;
    }
  }
  return;
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
        step(time_difference.total_microseconds() * 0.001);
      }
      else
      {
        step(mSimulatedTime.total_microseconds() * 0.001);
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

      #if defined DEBUG && defined SHOW_TIMING_HELPERS
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
          step(full_steps_taken * step_size.total_microseconds() * 0.001);
        }
        else
        {
          step(mSimulatedTime.total_microseconds() * 0.001);
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
          step(steps_taken * step_size.total_microseconds() * 0.001);
        }
        else
        {
          step(mSimulatedTime.total_microseconds() * 0.001);
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

void cedar::aux::LoopedThread::singleStep()
{
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

bool cedar::aux::LoopedThread::stopRequested()
{
  if(isRunning() && mStop == true)
  {
    return true;
  }

  return false;
}

void cedar::aux::LoopedThread::readParamsFromConfigFile()
{
  _mStepSize = 100.0;
  _mIdleTime = 0.001;
  _mSimulatedTime = 0.0;

  // mStepSize

  if(addParameter(&_mStepSize, "threadStepSize", 100.0) != CONFIG_SUCCESS)
  {
    cout << "LoopedThread: Error reading parameter 'stepSize' from config file!" << endl;
  }

  // mIdleTime

  if(addParameter(&_mIdleTime, "threadIdleTime", 0.001) != CONFIG_SUCCESS)
  {
    cout << "LoopedThread: Error reading parameter 'idleTime' from config file!" << endl;
  }

  // mUseFixedStepSize

  if(addParameter(&mUseFixedStepSize, "threadUseFixedStepSize", true) != CONFIG_SUCCESS)
  {
    cout << "LoopedThread: Error reading parameter 'useFixedStepSize' from config file!" << endl;
  }

  // mSimulatedTime

  if(addParameter(&_mSimulatedTime, "threadSimulatedTime", 0.0) != CONFIG_SUCCESS)
  {
    cout << "LoopedThread: Error reading parameter 'simulatedTime' from config file!" << endl;
  }

  readOrDefaultConfiguration();

  mStepSize = microseconds(static_cast<unsigned int>(1000 * _mStepSize + 0.5));
  mIdleTime = static_cast<unsigned int>(1000 * _mIdleTime + 0.5);
  mSimulatedTime = microseconds(static_cast<unsigned>(1000 * _mSimulatedTime + 0.5));

  return;
}

void cedar::aux::LoopedThread::setStepSize(double stepSize)
{
  mStepSize = boost::posix_time::microseconds(static_cast<unsigned int>(1000 * stepSize + 0.5));
  _mStepSize = stepSize;
}

void cedar::aux::LoopedThread::setIdleTime(double idleTime)
{
  mIdleTime = static_cast<unsigned int>(1000 * idleTime + 0.5);
}

void cedar::aux::LoopedThread::useFixedStepSize(bool useFixedStepSize)
{
  mUseFixedStepSize = useFixedStepSize;
}

void cedar::aux::LoopedThread::setSimulatedTime(double simulatedTime)
{
  mSimulatedTime = boost::posix_time::microseconds(static_cast<unsigned>(1000 * simulatedTime + 0.5));
}

boost::posix_time::ptime cedar::aux::LoopedThread::getLastTimeStepStart() const
{
  return mLastTimeStepStart;
}

boost::posix_time::ptime cedar::aux::LoopedThread::getLastTimeStepEnd() const
{
  return mLastTimeStepEnd;
}

boost::posix_time::time_duration cedar::aux::LoopedThread::getLastTimeStepDuration() const
{
  return mLastTimeStepStart - mLastTimeStepEnd;
}
