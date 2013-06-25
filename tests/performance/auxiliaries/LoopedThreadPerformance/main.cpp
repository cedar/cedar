/*======================================================================================================================

    Copyright 2011, 2012, 2013 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany

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

    File:        main.cpp

    Maintainer:  Jean-Stephane Jokeit
    Email:       jean-stephane.jokeit@ini.ruhr-uni-bochum.de
    Date:        2013 06 19

    Description: Test Thread utility class

    Credits:

======================================================================================================================*/


// LOCAL INCLUDES
#include "cedar/auxiliaries/LoopedThread.h"

// SYSTEM INCLUDES
#include <functional>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <QReadWriteLock>

void write_measurement
     (
       const std::string& id,
       double duration
     )
{
  std::cout
      << "<DartMeasurement"
      << " name=\""
      << id
      << " (seconds)\""
      << " type=\"numeric/double\">"
      << cedar::aux::toString(duration)
      << "</DartMeasurement>"
      << std::endl;
}

void test_time(std::string id, std::function< void() > fun)
{
  using boost::posix_time::ptime;
  using boost::posix_time::microsec_clock;

  double duration;

  auto start = microsec_clock::local_time();

  // execute the test:
  fun();

  auto end = microsec_clock::local_time();

  duration = static_cast<double>((end - start).total_milliseconds()) / 1000.0;
  write_measurement(id, duration);
}

// changing these values will obviously have system and load dependent
// variations

const unsigned int MAX_THREADS = 20;
const unsigned int STEP_SIZE = 20;
const double WORK_SEC = 0.015;

struct MyThread : public cedar::aux::LoopedThread
{
  unsigned long mNumSteps;
  double mTotalRealStep;
  double mMaxRealStep;

  MyThread() 
  : mNumSteps(0), mTotalRealStep(0), mMaxRealStep(0)
  {
  }

private:
  void step(double real_step)
  {
    mNumSteps++;
    mTotalRealStep += real_step;
    if (real_step > mMaxRealStep)
      mMaxRealStep= real_step;

    // stall for a given time. simulate doing work

    using boost::posix_time::ptime;
    using boost::posix_time::microsec_clock;

    auto start = microsec_clock::local_time();
    double duration = 0.0;

    while( duration < WORK_SEC 
           && isRunning() )
    {
      // do nothing

      auto end = microsec_clock::local_time();
      duration = static_cast<double>((end - start).total_milliseconds()) / 1000.0;
    }
    
  }
};

std::list< MyThread* > threads;

void create_test()
{
  unsigned int i= 0;
  for (; i <= MAX_THREADS; i++ )
  {
    threads.push_back( new MyThread() );
    threads.back()->setStepSize( STEP_SIZE );
  }
}

void start_test()
{
  auto it = threads.begin();

  for (; it != threads.end(); it++ )
  {
    (*it)->start();
  }
}

void stop_test()
{
  auto it = threads.begin();

  for (; it != threads.end(); it++ )
  {
    (*it)->stop();
  }
}

void delete_test()
{
  auto it = threads.begin();

  for (; it != threads.end(); it++ )
  {
    delete (*it);
  }
}

int main(int, char**)
{
  test_time("create threads", create_test );
  test_time("start threads", start_test );

  usleep(1000*1000*3);

  test_time("stop threads", stop_test );

  // evaluation statistics for all threads:

  unsigned int num_steps_all7= 0;
  double total_real_step_all= 0;
  double max_real_step_all= 0;

  auto it = threads.begin();
  for (; it != threads.end(); it++ )
  {
    num_steps_all7 = (*it)->mNumSteps;
    total_real_step_all = (*it)->mTotalRealStep;

    if ( (*it)->mMaxRealStep > max_real_step_all )
      max_real_step_all= (*it)->mMaxRealStep;
  }
 
  write_measurement("num steps", num_steps_all7);
  write_measurement("real-step size", total_real_step_all );
  write_measurement("real-step max", max_real_step_all );
  write_measurement("rel deviatiation", ( total_real_step_all 
                                           / num_steps_all7 )
                                        - STEP_SIZE );                                        
  test_time("delete threads", delete_test );
  return 0;
}

