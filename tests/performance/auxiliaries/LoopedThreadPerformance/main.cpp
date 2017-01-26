/*======================================================================================================================

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
#include "cedar/auxiliaries/CallFunctionInThread.h"
#include "cedar/testingUtilities/measurementFunctions.h"
#include "cedar/auxiliaries/sleepFunctions.h"
#include "cedar/units/Time.h"
#include "cedar/units/prefixes.h"

// SYSTEM INCLUDES
#include <QReadWriteLock>
#include <QCoreApplication>


// global variables
int errors = 0;


// changing these values will obviously have system and load dependent
// variations

const unsigned int MAX_THREADS = 20;
const cedar::unit::Time step_size(20.0 * cedar::unit::milli * cedar::unit::seconds);
const cedar::unit::Time work_time(15.0 * cedar::unit::milli * cedar::unit::seconds);

struct MyThread : public cedar::aux::LoopedThread
{
  unsigned long mNumSteps;
  cedar::unit::Time mTotalRealStep;
  cedar::unit::Time mMaxRealStep;

  MyThread() 
  : mNumSteps(0), mTotalRealStep(0), mMaxRealStep(0)
  {
  }

private:
  void step(cedar::unit::Time real_step)
  {
    mNumSteps++;
    mTotalRealStep += real_step;
    if (real_step > mMaxRealStep)
      mMaxRealStep= real_step;

    // stall for a given time. simulate doing work

    using boost::posix_time::ptime;
    using boost::posix_time::microsec_clock;

    auto start = microsec_clock::local_time();
    cedar::unit::Time duration(0.0 * cedar::unit::seconds);

    while(duration < work_time && this->isRunningNolocking())
    {
      // do nothing

      auto end = microsec_clock::local_time();
      duration = cedar::unit::Time
                 (
                   static_cast<double>((end - start).total_milliseconds()) * cedar::unit::milli * cedar::unit::seconds
                 );
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
    threads.back()->setStepSize(step_size);
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

void run_test()
{
  errors = 0;

  cedar::test::test_time("create threads", create_test);
  cedar::test::test_time("start threads", start_test);

  cedar::aux::sleep(cedar::unit::Time(3.0 * cedar::unit::seconds));

  cedar::test::test_time("stop threads", stop_test);

  // evaluation statistics for all threads:

  unsigned int num_steps_all7 = 0;
  cedar::unit::Time total_real_step_all(0.0 * cedar::unit::seconds);
  cedar::unit::Time max_real_step_all(0.0 * cedar::unit::seconds);

  auto it = threads.begin();
  for (; it != threads.end(); it++)
  {
    num_steps_all7 = (*it)->mNumSteps;
    total_real_step_all = (*it)->mTotalRealStep;

    if ((*it)->mMaxRealStep > max_real_step_all)
    {
      max_real_step_all= (*it)->mMaxRealStep;
    }
  }

  cedar::unit::Time one_second(1.0 * cedar::unit::second);

  cedar::test::write_measurement("num steps", num_steps_all7);
  cedar::test::write_measurement("real-step size", total_real_step_all / one_second);
  cedar::test::write_measurement("real-step max", max_real_step_all / one_second);
  double deviation = ((total_real_step_all / one_second) / num_steps_all7) - (step_size / one_second);
  cedar::test::write_measurement("rel deviatiation", deviation);
  cedar::test::test_time("delete threads", delete_test);
}

int main(int argc, char* argv[])
{
  QCoreApplication* app;
  app = new QCoreApplication(argc,argv);

  auto testThread = new cedar::aux::CallFunctionInThread(run_test);

  QObject::connect(testThread, SIGNAL(finishedThread()), app, SLOT(quit()), Qt::QueuedConnection);  // alternatively: call app->quit() in runTests()

  testThread->start();
  app->exec();

  delete testThread;
  delete app;

  return errors;
}
