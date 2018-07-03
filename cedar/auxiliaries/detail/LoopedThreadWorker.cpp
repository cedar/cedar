/*=============================================================================

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

// CEDAR INCLUDES
#include "cedar/auxiliaries/detail/LoopedThreadWorker.h"
#include "cedar/auxiliaries/LoopedThread.h"
#include "cedar/auxiliaries/Log.h"
#include "cedar/auxiliaries/Settings.h"
#include "cedar/auxiliaries/stringFunctions.h"
#include "cedar/auxiliaries/sleepFunctions.h"
#include "cedar/units/Time.h"
#include "cedar/units/prefixes.h"

// SYSTEM INCLUDES
#include <algorithm>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/date_time/posix_time/posix_time_io.hpp>


cedar::aux::detail::LoopedThreadWorker::LoopedThreadWorker(cedar::aux::LoopedThread *wrapper) 
:
mpWrapper(wrapper),
mTimeFactor(cedar::aux::SettingsSingleton::getInstance()->getGlobalTimeFactor()),
mDebugMe(false)
{
  this->mGlobalTimeFactorConnection = cedar::aux::SettingsSingleton::getInstance()->connectToGlobalTimeFactorChangedSignal
  (
    boost::bind(&cedar::aux::detail::LoopedThreadWorker::globalTimeFactorChanged, this, _1)
  );
  initStatistics();
}

cedar::aux::detail::LoopedThreadWorker::~LoopedThreadWorker() 
{
}

void cedar::aux::detail::LoopedThreadWorker::globalTimeFactorChanged(double newFactor)
{
  QWriteLocker locker(this->mTimeFactor.getLockPtr());
  this->mTimeFactor.member() = newFactor;
}

void cedar::aux::detail::LoopedThreadWorker::work()
{
  if (safeStopRequested())
    return; // stop() was called right after start() 

  // we do not want to change the step size while running
  boost::posix_time::time_duration orig_step_size
    = boost::posix_time::microseconds(static_cast<unsigned int>(1000.0 * (mpWrapper->getStepSize()/cedar::unit::Time(1.0 * cedar::unit::milli * cedar::unit::second)) + 0.5));//mStepSize;
  initStatistics();

  // which mode?
  const auto loop_mode= mpWrapper->getLoopModeParameter(); 
  switch (loop_mode)
  {


    // DEPRECATED:
    case cedar::aux::LoopMode::RealTime:
    {
      setLastTimeStepStart( boost::posix_time::microsec_clock::universal_time() );
      setLastTimeStepEnd( boost::posix_time::microsec_clock::universal_time() );

      boost::posix_time::time_duration time_difference_unitless;

      while (!safeStopRequested())
      {
        QReadLocker locker(this->mTimeFactor.getLockPtr());
        double time_factor = this->mTimeFactor.member();
        locker.unlock();

        setLastTimeStepStart( getLastTimeStepEnd() );
        setLastTimeStepEnd( boost::posix_time::microsec_clock::universal_time() );
        time_difference_unitless = getLastTimeStepEnd() - getLastTimeStepStart();
        cedar::unit::Time time_difference(time_difference_unitless.total_microseconds() * cedar::unit::micro * cedar::unit::seconds);
        
        mpWrapper->step(time_difference * time_factor);

        // TODO: comment-in updateStatistics(1);
        cedar::unit::Time idle_time = mpWrapper->getIdleTimeParameter();
        cedar::aux::sleep(idle_time);
      }
      break;
    }
    // DEPRECATED:
    case cedar::aux::LoopMode::Simulated:
    {
      while (!safeStopRequested())
      {
        QReadLocker locker(this->mTimeFactor.getLockPtr());
        double time_factor = this->mTimeFactor.member();
        locker.unlock();

        mpWrapper->step(mpWrapper->getSimulatedTimeParameter() * time_factor);
        cedar::unit::Time idle_time = mpWrapper->getIdleTimeParameter();
        cedar::aux::sleep(idle_time);
      }
      break;
    }
    // DEPRECATED:
    case cedar::aux::LoopMode::Fixed:
    {
      // this might happen because the widget cuts off non-displayable decimals
      if (orig_step_size.total_milliseconds() == 0)
      {
        cedar::aux::LogSingleton::getInstance()->warning
        (
          "Step size is zero in cedar::aux::LoopMode::Fixed, defaulting to one millisecond.",
          "cedar::aux::LoopedThread::run()"
        );
        orig_step_size = boost::posix_time::milliseconds(1);
      }

      // initialization
      boost::posix_time::ptime scheduled_wakeup;
      setLastTimeStepStart( boost::posix_time::microsec_clock::universal_time() );
      setLastTimeStepEnd( getLastTimeStepStart() );
      scheduled_wakeup = getLastTimeStepEnd() + orig_step_size;

      // this makes sure that every thread has a different random seed
      this->initRngs();

      // some auxiliary variables
      boost::posix_time::time_duration effective_sleep_duration = boost::posix_time::microseconds(0);
      boost::posix_time::time_duration step_duration = boost::posix_time::microseconds(0);

      while (!safeStopRequested())
      {
        QReadLocker locker(this->mTimeFactor.getLockPtr());
        double time_factor = this->mTimeFactor.member();
        locker.unlock();

        // sleep until next wake up time
        effective_sleep_duration= scheduled_wakeup - boost::posix_time::microsec_clock::universal_time();
        double time = std::max<double>(0.0, static_cast<double>(effective_sleep_duration.total_microseconds()));
        cedar::unit::Time seconds(time * cedar::unit::micro * cedar::unit::seconds);
        cedar::aux::sleep(seconds);
      
        if (safeStopRequested())
          break;

        // determine time since last run
        setLastTimeStepStart( getLastTimeStepEnd() );
        setLastTimeStepEnd( scheduled_wakeup );
        step_duration = getLastTimeStepEnd() - getLastTimeStepStart();

        // calculate number of time steps taken
        double steps_taken
          = static_cast<double>(step_duration.total_microseconds())
            / static_cast<double>(orig_step_size.total_microseconds());
        unsigned int full_steps_taken = static_cast<unsigned int>(steps_taken + 0.5);

        // update statistics
        updateStatistics(full_steps_taken);

        // call step function
        cedar::unit::Time step_time(full_steps_taken * orig_step_size.total_microseconds() * cedar::unit::micro * cedar::unit::seconds);
        mpWrapper->step(step_time * time_factor);

        if (safeStopRequested())
          break;

        // schedule the next wake up
        while (scheduled_wakeup < boost::posix_time::microsec_clock::universal_time())
        {
          scheduled_wakeup = scheduled_wakeup + orig_step_size;
        }
      } // while(!mStop) 

      break;
    }
    // DEPRECATED:
    case cedar::aux::LoopMode::FixedAdaptive:
    {
      // initialization
      boost::posix_time::ptime scheduled_wakeup;
      
      setLastTimeStepStart( boost::posix_time::microsec_clock::universal_time() );
      setLastTimeStepEnd( boost::posix_time::microsec_clock::universal_time() );
      scheduled_wakeup = getLastTimeStepEnd() + orig_step_size;

      this->initRngs();

      // some auxiliary variables
      boost::posix_time::time_duration effective_sleep_duration = boost::posix_time::microseconds(0);
      boost::posix_time::time_duration step_duration = boost::posix_time::microseconds(0);

      while (!safeStopRequested())
      {
        QReadLocker locker(this->mTimeFactor.getLockPtr());
        double time_factor = this->mTimeFactor.member();
        locker.unlock();

        // sleep until next wake up time
        effective_sleep_duration = scheduled_wakeup - boost::posix_time::microsec_clock::universal_time();
        usleep(std::max<int>(0, effective_sleep_duration.total_microseconds()));

        if (safeStopRequested()) // a lot can happen in a few us 
          break;

        // determine time since last run
        setLastTimeStepStart( getLastTimeStepEnd() );
        setLastTimeStepEnd( scheduled_wakeup );
        step_duration = getLastTimeStepEnd() - getLastTimeStepStart();

        // calculate number of time steps taken
        double steps_taken
          = static_cast<double>(step_duration.total_microseconds())
            / static_cast<double>(orig_step_size.total_microseconds());

        // update statistics
        updateStatistics(steps_taken);

        // call step function
        cedar::unit::Time step_time(steps_taken * orig_step_size.total_microseconds() * cedar::unit::micro * cedar::unit::seconds);
        mpWrapper->step(step_time * time_factor);

        if (safeStopRequested()) // a lot can happen in a step() //!@todo locking
          break;

        // schedule the next wake up
        scheduled_wakeup = std::max<boost::posix_time::ptime>
                           (
                             scheduled_wakeup + orig_step_size,
                             boost::posix_time::microsec_clock::universal_time()
                           );

      } // while(!mStop)

      break;
    }
















    // NEW AND SHINY:
    case cedar::aux::LoopMode::RealDT:
    case cedar::aux::LoopMode::FakeDT:
    {
      // ALL NEW LOOP MODES HERE, share as much code as possible:

      this->initRngs();

      QWriteLocker locker(&mLastTimeStepEndLock); // lock once for loop, only

      boost::posix_time::time_duration measured_iteration_time_unitless;
      boost::posix_time::time_duration effective_sleep_duration = boost::posix_time::microseconds(0);
      auto start_time = boost::posix_time::microsec_clock::universal_time();

      mLastTimeStepStart= start_time;
      mLastTimeStepEnd= start_time;

      boost::posix_time::time_duration modified_sleep_time= orig_step_size;
      boost::posix_time::ptime scheduled_wakeup= start_time + orig_step_size;

      // get some locked vars early:
      boost::posix_time::ptime last_time_locked_vars_checked= start_time;
      double time_factor= 1.0;
      if (1)
      {
        QReadLocker locker(this->mTimeFactor.getLockPtr());
        time_factor = this->mTimeFactor.member();
      }
      auto minimum_step_size= mpWrapper->getMinimumStepSize();
      auto orig_fake_step_size= mpWrapper->getFakeStepSize();

      //////// START LOOP
      while (!safeStopRequested())
      {
        auto current_time_before_sleep= boost::posix_time::microsec_clock::universal_time();
        bool check_locked_vars= false;

        // gelockte vars nur alle halbe sec pruefen, das reicht
        if ( (current_time_before_sleep 
              - last_time_locked_vars_checked).total_microseconds() 
             > 500000 )
        {
          check_locked_vars= true;
//std::cout << "   checking locked vars " << boost::posix_time::to_simple_string(current_time_before_sleep) << " " << this << std::endl;          
//std::cout <<       (current_time_before_sleep 
//              - last_time_locked_vars_checked).total_microseconds()  << std::endl;
          last_time_locked_vars_checked= current_time_before_sleep;
        }


        // SLEEP first
        effective_sleep_duration = scheduled_wakeup 
                                   - current_time_before_sleep;
//std::cout << " current time before sleep: " << boost::posix_time::to_simple_string(current_time_before_sleep) << std::endl; 
//std::cout << "   will sleep for " << effective_sleep_duration.total_microseconds() << std::endl;
        // ensure a minimum sleep time is always respected, even under load
        long effective_sleep_duration_safe_mus= std::max<long>(0, 
                          effective_sleep_duration.total_microseconds());
#if 1
        if (check_locked_vars)
        {
          minimum_step_size= mpWrapper->getMinimumStepSize();
        }

        effective_sleep_duration_safe_mus= std::max<long>( 
                         effective_sleep_duration_safe_mus,
                         static_cast<long>(
                         ( minimum_step_size
                           / cedar::unit::Time(1.0 * cedar::unit::micro * cedar::unit::second) ) + 0.5) );
#endif        

#if 0
        if (mDebugMe)
        {
          std::cout << "       will sleep for " << effective_sleep_duration_safe_mus << std::endl;
          std::cout << "                   a: " << effective_sleep_duration << std::endl;
          if (check_locked_vars)
          std::cout << "                   check locked vars " << std::endl;
        }
#endif        

        //////////// SLEEP
        usleep( static_cast<unsigned long>(effective_sleep_duration_safe_mus) );

        if (safeStopRequested()) // abort faster
        {
          continue;
        }

        auto current_time_after_sleep= boost::posix_time::microsec_clock::universal_time();

        mLastTimeStepStart= mLastTimeStepEnd;
        mLastTimeStepEnd= current_time_after_sleep;

        measured_iteration_time_unitless = mLastTimeStepEnd
                                          - mLastTimeStepStart;

//std::cout << "       measured step time: " << measured_iteration_time_unitless.total_microseconds() << std::endl;
        cedar::unit::Time measured_iteration_time( measured_iteration_time_unitless.total_microseconds() * cedar::unit::micro * cedar::unit::seconds);

        ///////// STEP
        if (loop_mode == cedar::aux::LoopMode::FakeDT)
        {
          if (check_locked_vars)
          {
            orig_fake_step_size= mpWrapper->getFakeStepSize();
            QReadLocker locker(this->mTimeFactor.getLockPtr());
            time_factor = this->mTimeFactor.member();
          }

          auto modified_fake_step_size= 
            cedar::unit::Time( 
              static_cast<long>( orig_fake_step_size / cedar::unit::Time(1.0 * cedar::unit::micro * cedar::unit::second) 
              * time_factor )
              * cedar::unit::micro * cedar::unit::seconds);

          mpWrapper->step( modified_fake_step_size );
        }
        else
        {
          mpWrapper->step( measured_iteration_time );

#if 0
          if (mDebugMe)
          {
            std::cout << "             after step" << std::endl;

            std::cout << "       measured step time: " << measured_iteration_time_unitless.total_microseconds() << " (intended: " << orig_step_size  << ")" << std::endl;
            std::cout << std::endl;
          }
#endif          

        }
        // note, Stepping takes time!!! esp. for large architectures

        if (safeStopRequested()) // abort faster
        {
          continue;
        }

        auto current_time_after_step= boost::posix_time::microsec_clock::universal_time();

//std::cout << "   orig step size: " << orig_step_size << " modif step size: " << modified_sleep_time << std::endl         ;
//std::cout << " current time after step: " << boost::posix_time::to_simple_string(current_time_after_step) << std::endl; 
//std::cout << " old sched wakeup: " << boost::posix_time::to_simple_string(scheduled_wakeup) << std::endl; 


        auto old_scheduled_wakeup= scheduled_wakeup;

        // schedule the next wake up:
        // normally try to hit a consistent step time:
        scheduled_wakeup = old_scheduled_wakeup
                           + modified_sleep_time;

        long steps_missed= 0;

        // if we already know that we will not make the next step() in time:
        // (our last step() was too long)
        if (scheduled_wakeup < current_time_after_step)
        {
          
          steps_missed= ( current_time_after_step
                          - old_scheduled_wakeup 
                        ).total_microseconds()
                        / modified_sleep_time.total_microseconds();

//if (mDebugMe)                        
//{
//std::cout << "      skipped steps: " << steps_missed << " now: " << boost::posix_time::to_simple_string(current_time_after_step) << std::endl;                        
//}

          // try to hit the next step size, starting from before we
          // dived into step() (important!)
          scheduled_wakeup = old_scheduled_wakeup
                             + modified_sleep_time;

          // js: there was a while loop here, that could spin a lot. removed
        }

        updateStatistics(steps_missed + 1);

//std::cout << " new sched wakeup: " << boost::posix_time::to_simple_string(scheduled_wakeup) << std::endl; 

      } // end while
      break;
    } // end new nodes
    default:
    {
      // this should never happen - unrecognized enum case
      CEDAR_ASSERT(false);
    }
  } // end big switch

  safeRequestStop();
  return;
}

void cedar::aux::detail::LoopedThreadWorker::initRngs()
{
  auto seed = boost::posix_time::microsec_clock::universal_time().time_of_day().total_milliseconds();
  srand(seed);
  cv::theRNG().state = seed;
}

void cedar::aux::detail::LoopedThreadWorker::initStatistics()
{
  QWriteLocker locker1(&mNumberOfStepsLock);
  QWriteLocker locker2(&mSumOfStepsTakenLock);
  QWriteLocker locker3(&mMaxStepsTakenLock);

  mNumberOfSteps = 0;
  mSumOfStepsTaken = 0.0;
  mMaxStepsTaken = 0.0;
}

void cedar::aux::detail::LoopedThreadWorker::updateStatistics(double stepsTaken)
{
  QWriteLocker locker1(&mNumberOfStepsLock);
  QWriteLocker locker2(&mSumOfStepsTakenLock);
  QWriteLocker locker3(&mMaxStepsTakenLock);

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

    // init statistics locks these again
    locker3.unlock();
    locker2.unlock();
    locker1.unlock();

    initStatistics();
    return;
  }
}

boost::posix_time::ptime cedar::aux::detail::LoopedThreadWorker::getLastTimeStepStart() const
{
  QReadLocker locker(&mLastTimeStepStartLock);
  return mLastTimeStepStart;
}

void cedar::aux::detail::LoopedThreadWorker::setLastTimeStepStart(const boost::posix_time::ptime& time)
{
  QWriteLocker locker(&mLastTimeStepStartLock);
  mLastTimeStepStart = time;
}

boost::posix_time::ptime cedar::aux::detail::LoopedThreadWorker::getLastTimeStepEnd() const
{
  QReadLocker locker(&mLastTimeStepEndLock);
  return mLastTimeStepEnd;
}

void cedar::aux::detail::LoopedThreadWorker::setLastTimeStepEnd(const boost::posix_time::ptime& time)
{
  QWriteLocker locker(&mLastTimeStepEndLock);
  mLastTimeStepEnd = time;
}

boost::posix_time::time_duration cedar::aux::detail::LoopedThreadWorker::getLastTimeStepDuration() const
{
  QReadLocker locker1(&mLastTimeStepStartLock);
  QReadLocker locker2(&mLastTimeStepEndLock);
  
  boost::posix_time::time_duration difference = mLastTimeStepStart - mLastTimeStepEnd;

  return difference;
}

unsigned long cedar::aux::detail::LoopedThreadWorker::getNumberOfSteps()
{
  QReadLocker locker(&mNumberOfStepsLock);
  unsigned long value = mNumberOfSteps;
  return value;
}

double cedar::aux::detail::LoopedThreadWorker::getMaxStepsTaken()
{
  QReadLocker locker(&mMaxStepsTakenLock);
  double value = mMaxStepsTaken;
  return value;
}

double cedar::aux::detail::LoopedThreadWorker::getSumOfStepsTaken()
{
  QReadLocker locker(&this->mSumOfStepsTakenLock);
  double value = this->mSumOfStepsTaken;
  return value;
}

double cedar::aux::detail::LoopedThreadWorker::getSumOfStepsMissed()
{
  QReadLocker locker(&this->mSumOfStepsTakenLock);
  QReadLocker locker2(&this->mNumberOfStepsLock);

  double value = this->mSumOfStepsTaken - this->mNumberOfSteps;
  return value;
}

void cedar::aux::detail::LoopedThreadWorker::safeRequestStop()
{
  if (mpWrapper != nullptr)
  {
    mpWrapper->requestStop();
  }
}

bool cedar::aux::detail::LoopedThreadWorker::safeStopRequested()
{
  if (mpWrapper != nullptr)
  {
    return mpWrapper->stopRequested();
  }

  return true; // (!) no wrapper means, we are not running
}

