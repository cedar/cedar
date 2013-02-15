/*=============================================================================

    Copyright 2013 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany
 
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

===============================================================================

    Institute:   Ruhr-Universitaet Bochum
                 Institut fuer Neuroinformatik

    File:        LoopedThreadWorker.cpp

    Maintainer:  Jean-Stephane Jokeit
    Email:       jean-stephane.jokeit@ini.ruhr-uni-bochum.de
    Date:        Mon 07 Jan 2013 05:50:41 PM CET

    Description:

    Credits:

=============================================================================*/

// LOCAL INCLUDES
#include "cedar/auxiliaries/namespace.h"
#include "cedar/auxiliaries/LoopedThreadWorker.h"
#include "cedar/auxiliaries/LoopedThread.h"

// CEDAR INCLUDES
#include "cedar/auxiliaries/Log.h"
#include "cedar/auxiliaries/stringFunctions.h"

cedar::aux::detail::LoopedThreadWorker::LoopedThreadWorker(cedar::aux::LoopedThread *wrapper) 
: ThreadWorker( wrapper ), mpWrapper(wrapper)
{
  initStatistics();
}

cedar::aux::detail::LoopedThreadWorker::~LoopedThreadWorker() 
{
}

void cedar::aux::detail::LoopedThreadWorker::work()
{
  // TODO: check whether wrapper (mpWrapper) still exists (paranoid)

  mStop = false; // TODO: locking

  // we do not want to change the step size while running
  boost::posix_time::time_duration step_size
    = boost::posix_time::microseconds(static_cast<unsigned int>(1000 * mpWrapper->getStepSize() + 0.5));//mStepSize;
  initStatistics();

  // which mode?
  switch ( mpWrapper->getLoopModeParameter() )
  {
    case cedar::aux::LoopMode::RealTime:
    {
      mLastTimeStepStart = boost::posix_time::microsec_clock::universal_time();
      mLastTimeStepEnd = boost::posix_time::microsec_clock::universal_time();
      boost::posix_time::time_duration time_difference;

      while (!mStop) // TODO: locking // note: mStop ist volatile, which is good
      {
        mLastTimeStepStart = mLastTimeStepEnd;
        mLastTimeStepEnd = boost::posix_time::microsec_clock::universal_time();

        time_difference = mLastTimeStepEnd - mLastTimeStepStart;
        mpWrapper->step(time_difference.total_microseconds() * 0.001);
        usleep(static_cast<unsigned int>(1000 * mpWrapper->getIdleTimeParameter() + 0.5));
      }
      break;
    }
    case cedar::aux::LoopMode::Simulated:
    {
      while (!mStop) // TODO: locking // note: mStop is volatile, which is good
      {
        mpWrapper->step(mpWrapper->getSimulatedTimeParameter());
        usleep(static_cast<unsigned int>(1000 * mpWrapper->getIdleTimeParameter() + 0.5));
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

      while (!mStop) // TODO: locking // note: mStop is volatile, which is good
      {
        // sleep until next wake up time
        sleep_duration = scheduled_wakeup - boost::posix_time::microsec_clock::universal_time();
        usleep(std::max<int>(0, sleep_duration.total_microseconds()));
      
        if (mStop) // a lot can happen in a few us // TODO: locking
          break;

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
        mpWrapper->step(full_steps_taken * step_size.total_microseconds() * 0.001);

        if (mStop) // a lot can happen in a step() // TODO: locking
          break;

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

      while (!mStop) // TODO: locking // note: mStop ist volatile, which is good
      {
        // sleep until next wake up time
        sleep_duration = scheduled_wakeup - boost::posix_time::microsec_clock::universal_time();
        usleep(std::max<int>(0, sleep_duration.total_microseconds()));

        if (mStop) // a lot can happen in a few us // TODO: locking
          break;

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
        mpWrapper->step(steps_taken * step_size.total_microseconds() * 0.001);

        if (mStop) // a lot can happen in a step() // TODO: locking
          break;

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

  mStop = false; // TODO: locking
  return;
}

void cedar::aux::detail::LoopedThreadWorker::initStatistics()
{
  mNumberOfSteps = 0;
  mSumOfStepsTaken = 0.0;
  mMaxStepsTaken = 0.0;
}

void cedar::aux::detail::LoopedThreadWorker::updateStatistics(double stepsTaken)
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
    cedar::aux::LogSingleton::getInstance()->warning
    (
      "Value overflow in thread statistics. Statistics will be reset.",
      "cedar::aux::LoopedThread::updateStatistics(double)"
    );
    initStatistics();
  }

  return;
}

boost::posix_time::ptime cedar::aux::detail::LoopedThreadWorker::getLastTimeStepStart() const
{
  return mLastTimeStepStart;
}

boost::posix_time::ptime cedar::aux::detail::LoopedThreadWorker::getLastTimeStepEnd() const
{
  return mLastTimeStepEnd;
}

boost::posix_time::time_duration cedar::aux::detail::LoopedThreadWorker::getLastTimeStepDuration() const
{
  return mLastTimeStepStart - mLastTimeStepEnd;
}

unsigned long cedar::aux::detail::LoopedThreadWorker::getNumberOfSteps()
{
  return mNumberOfSteps;
}

double cedar::aux::detail::LoopedThreadWorker::getMaxStepsTaken()
{
  return mMaxStepsTaken;
}


