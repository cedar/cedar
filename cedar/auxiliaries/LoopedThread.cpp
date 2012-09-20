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
#include "cedar/auxiliaries/EnumParameter.h"

// SYSTEM INCLUDES

//------------------------------------------------------------------------------
// constructors and destructor
//------------------------------------------------------------------------------
cedar::aux::LoopedThread::LoopedThread
(
  double stepSize,
  double idleTime,
  double simulatedTime,
  cedar::aux::EnumId mode

)
:
_mStepSize(new cedar::aux::DoubleParameter(this, "step size", stepSize)),
_mIdleTime(new cedar::aux::DoubleParameter(this, "idle time", idleTime)),
_mSimulatedTime(new cedar::aux::DoubleParameter(this, "simulated time", simulatedTime)),
_mLoopMode
(
  new cedar::aux::EnumParameter
  (
    this,
    "loop mode",
    cedar::aux::LoopMode::typePtr(),
    mode
  )
)
{
  mStop  = false;
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

    if (suppressWarning == false && mMaxStepsTaken > 1.01 && _mSimulatedTime->getValue() <= 0.0)
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

void cedar::aux::LoopedThread::run()
{
  // we do not want to change the step size while running
  boost::posix_time::time_duration step_size
    = boost::posix_time::microseconds(static_cast<unsigned int>(1000 * this->getStepSize() + 0.5));//mStepSize;
  initStatistics();

  // which mode?
  switch (_mLoopMode->getValue())
  {
    case cedar::aux::LoopMode::RealTime:
    {
      mLastTimeStepStart = boost::posix_time::microsec_clock::universal_time();
      mLastTimeStepEnd = boost::posix_time::microsec_clock::universal_time();
      boost::posix_time::time_duration time_difference;

      while (!mStop)
      {
        mLastTimeStepStart = mLastTimeStepEnd;
        mLastTimeStepEnd = boost::posix_time::microsec_clock::universal_time();

        time_difference = mLastTimeStepEnd - mLastTimeStepStart;
        this->step(time_difference.total_microseconds() * 0.001);
        usleep(static_cast<unsigned int>(1000 * _mIdleTime->getValue() + 0.5));
      }
      break;
    }
    case cedar::aux::LoopMode::Simulated:
    {
      while (!mStop)
      {
        step(_mSimulatedTime->getValue());
        usleep(static_cast<unsigned int>(1000 * _mIdleTime->getValue() + 0.5));
      }
      break;
    }
    case cedar::aux::LoopMode::Fixed:
    {
      if (step_size.total_milliseconds() == 0)
      {
        //!@todo We may want to just prevent setting this value
        cedar::aux::LogSingleton::getInstance()->warning
        (
          "Step size is zero in cedar::aux::LoopMode::Fixed, defaulting to one millisecond.",
          "cedar::aux::LoopedThread::run()"
        );
        step_size = boost::posix_time::milliseconds(1);
      }

      // initialization
      mLastTimeStepStart = boost::posix_time::microsec_clock::universal_time();
      mLastTimeStepEnd = mLastTimeStepStart;
      boost::posix_time::ptime scheduled_wakeup = mLastTimeStepEnd + step_size;
      //!@todo Should this really be here?
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
          = static_cast<double>(step_duration.total_microseconds())
            / static_cast<double>(step_size.total_microseconds());
        unsigned int full_steps_taken = static_cast<unsigned int>(steps_taken + 0.5);

        // update statistics
        updateStatistics(full_steps_taken);

        // call step function
        step(full_steps_taken * step_size.total_microseconds() * 0.001);

        // schedule the next wake up
        while (scheduled_wakeup < boost::posix_time::microsec_clock::universal_time())
        {
          scheduled_wakeup = scheduled_wakeup + step_size;
        }
      } // while(!mStop)

      break;
    }
    case cedar::aux::LoopMode::FixedAdaptive:
    {
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
          = static_cast<double>(step_duration.total_microseconds())
            / static_cast<double>(step_size.total_microseconds());

        // update statistics
        updateStatistics(steps_taken);

        // call step function
        step(steps_taken * step_size.total_microseconds() * 0.001);


        // schedule the next wake up
        scheduled_wakeup = std::max<boost::posix_time::ptime>
                           (
                             scheduled_wakeup + step_size,
                             boost::posix_time::microsec_clock::universal_time()
                           );

      } // while(!mStop)

      break;
    }
    default:
    {
      // this should never happen - unrecognized enum case
      CEDAR_ASSERT(false);
    }
  }

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
    switch (_mLoopMode->getValue())
    {
      case cedar::aux::LoopMode::Simulated:
      {
        step(_mSimulatedTime->getValue());
        break;
      }
      default:
      {
        step(_mStepSize->getValue());
      }
    }
  }
  else
  {
    cedar::aux::LogSingleton::getInstance()->warning
    (
      "LoopedThread is already running, singleStep has no effect",
      "cedar::aux::LoopedThread::singleStep()"
    );
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

void cedar::aux::LoopedThread::setStepSize(double stepSize)
{
  this->_mStepSize->setValue(stepSize);
}

void cedar::aux::LoopedThread::setIdleTime(double idleTime)
{
  _mIdleTime->setValue(idleTime);
}

void cedar::aux::LoopedThread::setSimulatedTime(double simulatedTime)
{
  _mSimulatedTime->setValue(simulatedTime);
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
