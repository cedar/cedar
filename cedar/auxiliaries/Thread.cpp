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

    File:        Thread.cpp

    Maintainer:  Bjoern Weghenkel
    Email:       bjoern.weghenkel@ini.rub.de
    Date:        2010 11 16

    Description: Implementation of the @em cedar::aux::Thread class.

    Credits:

======================================================================================================================*/


// LOCAL INCLUDES
#include "Thread.h"

// PROJECT INCLUDES

// SYSTEM INCLUDES
#include <QTime>

using namespace std;

//------------------------------------------------------------------------------
// constructors and destructor
//------------------------------------------------------------------------------

cedar::aux::Thread::Thread()
{
  _mName    = string("thread");
  mStepSize = 1;
  mStop     = false;
  initStatistics();
}

cedar::aux::Thread::Thread(unsigned stepSize) :
  mStepSize(stepSize)
{
  _mName = string("thread");
  mStop  = false;
  initStatistics();
}

cedar::aux::Thread::~Thread()
{
}

//------------------------------------------------------------------------------
// methods
//------------------------------------------------------------------------------

void cedar::aux::Thread::stop(unsigned time, bool suppressWarning)
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

void cedar::aux::Thread::run(void)
{
  mStop = false;

  // some auxiliary variables
  QTime lastWakeup = QTime::currentTime();
  QTime scheduledWakeup = lastWakeup.addMSecs(mStepSize);
  QTime tmpTime;
  srand(QTime::currentTime().msec());

  while (!mStop)
  {
    cout << "mStop = true" << endl;

    // sleep until next wake up time
    msleep(std::max<int>(0, QTime::currentTime().msecsTo(scheduledWakeup)));

    // determine number of time steps since last run
    QTime wakeupMax = scheduledWakeup.addMSecs(mStepSize); // end of current time window
    tmpTime = lastWakeup.addMSecs(mStepSize);
    unsigned stepsTaken = 0;
    while (tmpTime < wakeupMax)
    {
      tmpTime = tmpTime.addMSecs(mStepSize);
      stepsTaken++;
    }
    lastWakeup = scheduledWakeup; // remember the current wakeup time
    scheduledWakeup = tmpTime;
    updateStatistics(stepsTaken);

    // print warning if time steps have been skipped
    //if( stepsTaken > 1 )
    //	cerr << "WARNING: " << stepsTaken << " time steps taken at once! "
    //	<< "Your system might be too slow for an execution interval of "
    //	<< mStepSize << " milliseconds. Consider using a longer interval!"
    //	<< endl;

    // call step function
    step(stepsTaken * mStepSize);

    // if the execution lasted unexpectedly long, we'd like to wake up for
    // the next regular time step
    while (scheduledWakeup < QTime::currentTime())
      scheduledWakeup = scheduledWakeup.addMSecs(mStepSize);
  }

  mStop = false;
  return;
}

void cedar::aux::Thread::initStatistics()
{
  mNumberOfSteps   = 0;
  mSumOfStepsTaken = 0;
  mMaxStepsTaken   = 0;
}

void cedar::aux::Thread::updateStatistics(unsigned stepsTaken)
{

  unsigned long oldSum = mSumOfStepsTaken;

  mNumberOfSteps++;
  mSumOfStepsTaken += stepsTaken;
  if (stepsTaken > mMaxStepsTaken)
    mMaxStepsTaken = stepsTaken;

  if (oldSum > mSumOfStepsTaken)
  {
    cerr << "Warning: Value overflow in thread statistics. Statistics will be reseted." << endl;
    initStatistics();
  }

  return;
}

void cedar::aux::Thread::singleStep() {
  if( !isRunning() )
    step(mStepSize);
}
