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
#include "LoopedThread.h"

// PROJECT INCLUDES

// SYSTEM INCLUDES
//#include <QTime>


using namespace std;
using namespace boost::posix_time;

//------------------------------------------------------------------------------
// constructors and destructor
//------------------------------------------------------------------------------

cedar::aux::LoopedThread::LoopedThread(unsigned long stepSize)
{
  _mName = string("thread");
  mStepSize = microseconds(stepSize);
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
  mStop = true;
  wait(time);
  if (suppressWarning == false && mMaxStepsTaken > 1)
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
  // some auxiliary variables
  ptime last_wakeup = microsec_clock::universal_time();
  ptime scheduled_wakeup = last_wakeup + mStepSize;
  srand(microsec_clock::universal_time().time_of_day().total_milliseconds());

  while(!mStop)
  {
    // sleep until next wake up time
    time_duration time_diff = scheduled_wakeup - microsec_clock::universal_time();
    usleep(std::max<int>(0, time_diff.total_microseconds()));

    // determine number of time steps since last run
    ptime wakeup_max = scheduled_wakeup + mStepSize; // end of current time window
    ptime tmp_time = last_wakeup + mStepSize;
    unsigned steps_taken = 0;
    while (tmp_time < wakeup_max)
    {
      tmp_time = tmp_time + mStepSize;
      steps_taken++;
    }
    last_wakeup = scheduled_wakeup; // remember the current wakeup time
    scheduled_wakeup = tmp_time;
    updateStatistics(steps_taken);

#ifdef DEBUG
    // print warning if time steps have been skipped
    if( steps_taken > 1 )
    	cerr << "WARNING: " << steps_taken << " time steps taken at once! "
    	<< "Your system might be too slow for an execution interval of "
    	<< mStepSize.total_microseconds() << " microseconds. Consider using a "
    	<< "longer interval!"	<< endl;
#endif

    // call step function
    step(steps_taken * mStepSize.total_microseconds());

    // if the execution lasted unexpectedly long, we'd like to wake up for
    // the next regular time step
    while(scheduled_wakeup <= microsec_clock::universal_time())
    {
      scheduled_wakeup = scheduled_wakeup + mStepSize;
    }

  } // while(!mStop)

  mStop = false;
  return;
}

void cedar::aux::LoopedThread::initStatistics()
{
  mNumberOfSteps   = 0;
  mSumOfStepsTaken = 0;
  mMaxStepsTaken   = 0;
}

void cedar::aux::LoopedThread::updateStatistics(unsigned stepsTaken)
{

  unsigned long old_sum = mSumOfStepsTaken;

  mNumberOfSteps++;
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
    step(mStepSize.total_microseconds());
  }
}
