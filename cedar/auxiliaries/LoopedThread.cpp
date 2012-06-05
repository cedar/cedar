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

    Maintainer:  Stephan Zibner
    Email:       stephan.zibner@ini.rub.de
    Date:        2010 12 02

    Description: Implementation of the @em cedar::aux::LoopedThread class.

    Credits:

======================================================================================================================*/

// CEDAR INCLUDES
#include "cedar/auxiliaries/LoopedThread.h"
#include "cedar/auxiliaries/Log.h"
#include "cedar/auxiliaries/stringFunctions.h"

// SYSTEM INCLUDES

//------------------------------------------------------------------------------
// constructors and destructor
//------------------------------------------------------------------------------
cedar::aux::LoopedThread::LoopedThread
(
  double stepSize,
  double idleTime
#ifdef CEDAR_LIBCONFIG_LEGACY_MODE
  ,const std::string& configFileName
#endif // CEDAR_LIBCONFIG_LEGACY_MODE
)
:
#ifdef CEDAR_LIBCONFIG_LEGACY_MODE
cedar::aux::ConfigurationInterface(configFileName)
#else
_mStepSize(new cedar::aux::DoubleParameter(this, "step size", stepSize)),
_mIdleTime(new cedar::aux::DoubleParameter(this, "idle time", idleTime)),
_mSimulatedTime(new cedar::aux::DoubleParameter(this, "simulated time", 0.0)),
_mUseFixedStepSize(new cedar::aux::BoolParameter(this, "use fixed step size", true))
#endif // CEDAR_LIBCONFIG_LEGACY_MODE
{
  mStop  = false;
  initStatistics();

  mStepSize = boost::posix_time::microseconds(static_cast<unsigned int>(1000 * stepSize + 0.5));
  mIdleTime = static_cast<unsigned int>(1000 * idleTime + 0.5);
#ifdef CEDAR_LIBCONFIG_LEGACY_MODE
  mUseFixedStepSize = true;
#endif // CEDAR_LIBCONFIG_LEGACY_MODE
  mSimulatedTime = boost::posix_time::microseconds(0);
}

#ifdef CEDAR_LIBCONFIG_LEGACY_MODE
cedar::aux::LoopedThread::LoopedThread(const std::string& configFileName)
:
cedar::aux::ConfigurationInterface(configFileName)
{
  mStop = false;
  initStatistics();
  readParamsFromConfigFile();
}

cedar::aux::LoopedThread::LoopedThread(const char* pConfigFileName)
:
cedar::aux::ConfigurationInterface(pConfigFileName)
{
  mStop = false;
  initStatistics();
  readParamsFromConfigFile();
}
#endif // CEDAR_LIBCONFIG_LEGACY_MODE

cedar::aux::LoopedThread::~LoopedThread()
{
}

//------------------------------------------------------------------------------
// methods
//------------------------------------------------------------------------------

void cedar::aux::LoopedThread::stop(unsigned int time, bool suppressWarning)
{
  if (this->isRunning())
  {
    mStop = true;
    wait(time);

    if (this->isRunning())
    {
      cedar::aux::LogSingleton::getInstance()->warning
      (
        "Thread is still running after call of stop()!",
        "cedar::aux::LoopedThread::stop(unsigned int, bool)"
      );
    }

    if (suppressWarning == false && mMaxStepsTaken > 1.01 && mSimulatedTime.total_microseconds() == 0)
    {
      std::string message = "The system was not fast enough to stay to scheduled thread timing. ";
      message += "Consider using a larger step size.\n";
      message += "Execution stats:\n";
      message += "  avg. time steps between execution: ";
      message += cedar::aux::toString(mSumOfStepsTaken / static_cast<double>(mNumberOfSteps));
      message += "\n";
      message += "  max. time steps between execution: ";
      message += cedar::aux::toString(mMaxStepsTaken);

      cedar::aux::LogSingleton::getInstance()->warning
      (
        message,
        "cedar::aux::LoopedThread::stop(unsigned int, bool)"
      );
    }
  }
  return;
}

void cedar::aux::LoopedThread::run(void)
{
  // we do not want to change the step size while running
  boost::posix_time::time_duration step_size = mStepSize;
  bool use_fixed_step_size = this->usesFixedStepSize();
  initStatistics();

  // which mode?
  if (step_size.total_microseconds() == 0)
  {
    // fast running mode

    mLastTimeStepStart = boost::posix_time::microsec_clock::universal_time();
    mLastTimeStepEnd = boost::posix_time::microsec_clock::universal_time();
    boost::posix_time::time_duration time_difference;

    while (!mStop)
    {
      usleep(mIdleTime);
      mLastTimeStepStart = mLastTimeStepEnd;
      mLastTimeStepEnd = boost::posix_time::microsec_clock::universal_time();

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
  else // step_size.total_microseconds() != 0
  {
    // regular mode with fixed time

    // initialization
    mLastTimeStepStart = boost::posix_time::microsec_clock::universal_time();
    mLastTimeStepEnd = boost::posix_time::microsec_clock::universal_time();
    boost::posix_time::ptime scheduled_wakeup = mLastTimeStepEnd + step_size;
    srand(boost::posix_time::microsec_clock::universal_time().time_of_day().total_milliseconds());

    // some auxiliary variables
    boost::posix_time::time_duration sleep_duration = boost::posix_time::microseconds(0);
    boost::posix_time::time_duration step_duration = boost::posix_time::microseconds(0);

    while (!mStop)
    {
      // sleep until next wake up time
      sleep_duration = scheduled_wakeup - boost::posix_time::microsec_clock::universal_time();
      usleep(std::max<int>(0, sleep_duration.total_microseconds()));

      // determine time since last run
      mLastTimeStepStart = mLastTimeStepEnd;
      mLastTimeStepEnd = scheduled_wakeup;
      step_duration = mLastTimeStepEnd - mLastTimeStepStart;

      // calculate number of time steps taken
      double steps_taken
        = static_cast<double>(step_duration.total_microseconds()) / static_cast<double>(step_size.total_microseconds());
      unsigned int full_steps_taken = static_cast<unsigned int>(steps_taken + 0.5);

      #if defined DEBUG && defined SHOW_TIMING_HELPERS
      // print warning if time steps have been skipped
      if (steps_taken > 1.01)
      {
        cedar::aux::LogSingleton::getInstance()->debug
        (
          "Warning: " + cedar::aux::toString(steps_taken) + " time steps taken at once! "
          "Your system might be too slow for an execution interval of "
          + cedar::aux::toString(step_size.total_microseconds())
          + " microseconds. Consider using a longer interval!",
          "cedar::aux::LoopedThread::stop(unsigned int, bool)"
        );
      }
      #endif

      // here we have to distinguish between the different working modes of the
      // thread. either we want to wake up on the next fixed time step or as
      // soon as possible if we are already late.
      if (use_fixed_step_size)
      {
        // update statistics
        updateStatistics(full_steps_taken);

        // call step function
        if (mSimulatedTime.total_microseconds() == 0)
        {
          step(full_steps_taken * step_size.total_microseconds() * 0.001);
        }
        else
        {
          step(mSimulatedTime.total_microseconds() * 0.001);
        }

        // schedule the next wake up
        while (scheduled_wakeup < boost::posix_time::microsec_clock::universal_time())
        {
          scheduled_wakeup = scheduled_wakeup + step_size;
        }

      }
      else
      {
        // update statistics
        updateStatistics(steps_taken);

        // call step function
        if (mSimulatedTime.total_microseconds() == 0)
        {
          step(steps_taken * step_size.total_microseconds() * 0.001);
        }
        else
        {
          step(mSimulatedTime.total_microseconds() * 0.001);
        }

        // schedule the next wake up
        scheduled_wakeup = std::max<boost::posix_time::ptime>
                           (
                             scheduled_wakeup + step_size,
                             boost::posix_time::microsec_clock::universal_time()
                           );

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
  if (stepsTaken > mMaxStepsTaken)
  {
    mMaxStepsTaken = stepsTaken;
  }

  if (old_sum > mSumOfStepsTaken)
  {
    std::cerr << "Warning: Value overflow in thread statistics. Statistics will be reseted." << std::endl;
    initStatistics();
  }

  return;
}

void cedar::aux::LoopedThread::singleStep()
{
  if (!this->isRunning())
  {
    if (mSimulatedTime.total_microseconds() == 0)
    {
      step(mStepSize.total_microseconds() * 0.001);
    }
    else
    {
      step(mSimulatedTime.total_microseconds() * 0.001);
    }
  }
}

bool cedar::aux::LoopedThread::stopRequested()
{
  if (this->isRunning() && mStop == true)
  {
    return true;
  }

  return false;
}

void cedar::aux::LoopedThread::readParamsFromConfigFile()
{
  _mStepSize->setValue(100.0);
  _mIdleTime->setValue(0.001);
  _mSimulatedTime->setValue(0.0);

  mStepSize = boost::posix_time::microseconds(static_cast<unsigned int>(1000 * this->getStepSizeParameter() + 0.5));
  mIdleTime = static_cast<unsigned int>(1000 * this->getIdleTimeParameter() + 0.5);
  mSimulatedTime
    = boost::posix_time::microseconds(static_cast<unsigned>(1000 * this->getSimulatedTimeParameter() + 0.5));

  return;
}

void cedar::aux::LoopedThread::setStepSize(double stepSize)
{
  mStepSize = boost::posix_time::microseconds(static_cast<unsigned int>(1000 * stepSize + 0.5));
  this->_mStepSize->setValue(stepSize);
}

void cedar::aux::LoopedThread::setIdleTime(double idleTime)
{
  mIdleTime = static_cast<unsigned int>(1000 * idleTime + 0.5);
}

void cedar::aux::LoopedThread::useFixedStepSize(bool useFixedStepSize)
{
  this->_mUseFixedStepSize->setValue(useFixedStepSize);
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
