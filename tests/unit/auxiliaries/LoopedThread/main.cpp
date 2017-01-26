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

    Maintainer:  Bjoern Weghenkel
    Email:       bjoern.weghenkel@ini.rub.de
    Date:        2010 11 17

    Description: Example for the behavior of the @em cedar::aux::Thread class.

    Credits:

======================================================================================================================*/


#include "cedar/auxiliaries/LoopedThread.h"
#include "cedar/auxiliaries/CallFunctionInThread.h"
#include "cedar/auxiliaries/sleepFunctions.h"

#include <cstdlib>
#include <iostream>
#include <random>

#include <QCoreApplication>

#ifndef Q_MOC_RUN
  #include <boost/lexical_cast.hpp>
#endif

// global variables
int errors = 0;
QCoreApplication* app;

//!@brief threaded test class
class MyTestThread : public cedar::aux::LoopedThread
{

private:

  //!@brief decides whether to add an artificial delay
  bool mArtificialDelay;

public:

  //!@brief constructor
  MyTestThread(cedar::unit::Time stepSize, bool delay = false) : LoopedThread(stepSize)
  {
    mArtificialDelay = delay;
    srand(boost::posix_time::microsec_clock::universal_time().time_of_day().total_milliseconds());
  }

  void step(cedar::unit::Time time)
  {
    boost::posix_time::ptime now = boost::posix_time::microsec_clock::universal_time();
    std::cout << "current time (sec/usec): " << now.time_of_day().seconds()
              << " / " << now.time_of_day().total_microseconds() % 1000000
              << " (time in step(): " << time << ")" << std::endl;

    if (mArtificialDelay)
    {
      cedar::unit::Time milli_second(1.0 * cedar::unit::milli * cedar::unit::second);

      std::default_random_engine generator;
      std::uniform_real_distribution<double> distribution(0.0, 3.0 * this->getStepSize() / milli_second);

      cedar::unit::Time sleep_time(distribution(generator) * cedar::unit::milli * cedar::unit::seconds);
      cedar::aux::sleep(sleep_time);
    }
  }

  /*!@brief get info whether an artificial delay is added
   *
   * @return    state of mArtificialDelay
   */
  bool getArtificialDelay() 
  {
    return mArtificialDelay;
  }

  /*!@brief set whether an artificial delay is added
   *
   * @param delay    state of mArtificialDelay
   */
  void setArtificalDelay(bool delay)
  {
    mArtificialDelay = delay;
  }

};

class CountingThread : public cedar::aux::LoopedThread
{
  public:
    //!@brief constructor
    CountingThread
    (
      cedar::unit::Time stepSize,
      cedar::unit::Time idleTime,
      cedar::unit::Time simulatedTime,
      cedar::aux::LoopMode::Id mode
    )
    :
    cedar::aux::LoopedThread(stepSize, idleTime, simulatedTime, mode),
    mCounter(0)
    {
    }

    void step(cedar::unit::Time /* time */)
    {
      ++mCounter;
      cedar::aux::sleep(cedar::unit::Time(0.1 * cedar::unit::milli * cedar::unit::seconds));;
    }

    unsigned int mCounter;
};


int testConfiguration
    (
        cedar::unit::Time stepSize,
        cedar::unit::Time idleTime,
        cedar::unit::Time simulatedTime,
      cedar::aux::LoopMode::Id mode
    )
{
  int errors = 0;

  CountingThread thread(stepSize, idleTime, simulatedTime, mode);

  thread.start();
  //thread.wait(cedar::unit::Time(1.0 * cedar::unit::second));
  cedar::aux::sleep(cedar::unit::Time(1.0 * cedar::unit::second));

  // the thread should at least run for two iterations
  if (thread.mCounter < 2)
  {
    std::cout << "ERROR: the thread didn't iterate often enough, only " 
              << boost::lexical_cast<std::string>(thread.mCounter) 
              << " times." << std::endl;
    ++errors;
  }
  thread.stop();
  //thread.wait(cedar::unit::Time(1.0 * cedar::unit::second));
  cedar::aux::sleep(cedar::unit::Time(1.0 * cedar::unit::second));
  if (thread.isRunningNolocking())
  {
    std::cout << "ERROR: the thread didn't exit properly." << std::endl;
    ++errors;
  }

  return errors;
}



void runTests()
{
  MyTestThread thread(cedar::unit::Time(100.0 * cedar::unit::milli * cedar::unit::seconds));
  //thread.useFixedStepSize(false);
  //thread.setSimulatedTime(50);

  std::cout << "Starting a thread and let it run for 1 seconds ..." << std::endl;
  thread.start();
  //thread.wait(cedar::unit::Time(1.0 * cedar::unit::second));
  cedar::aux::sleep(cedar::unit::Time(1.0 * cedar::unit::second));
  std::cout << "Stopping thread ..." << std::endl;
  thread.stop();
  if (thread.isRunningNolocking())
  {
    std::cout << "... Thread NOT stopped! error" << std::endl;
    errors++;
    return;
  }
  else
  {
    //std::cout << "... thread stopped." << std::endl;
  }

  std::cout << "Immediately stopping a thread should invalidate a start ..." << std::endl;
  thread.start();
  thread.stop();

  if (thread.isRunningNolocking())
  {
    std::cout << "... Thread NOT stopped! error" << std::endl;
    errors++;
    return;
  }
  else
  {
    std::cout << "... thread immediately stopped. OK." << std::endl;
  }

  std::cout << std::endl;
  std::cout << "Starting thread again with an artificially unreliable execution time ..." << std::endl;
  thread.setArtificalDelay(true);
  thread.start();
  //thread.wait(cedar::unit::Time(1.0 * cedar::unit::second));
  cedar::aux::sleep(cedar::unit::Time(1.0 * cedar::unit::second));
  std::cout << "Stopping thread ..." << std::endl;
  thread.stop();

  thread.setArtificalDelay(false);

  std::cout << std::endl;
  std::cout << "Starting thread again with step size 0 for one millisecond ..." << std::endl;
  thread.setStepSize(cedar::unit::Time(0.0 * cedar::unit::second));
  thread.start();
  //thread.wait(cedar::unit::Time(1.0 * cedar::unit::milli * cedar::unit::second));
  cedar::aux::sleep(cedar::unit::Time(1.0 * cedar::unit::milli * cedar::unit::second));
  std::cout << "Stopping thread ..." << std::endl;
  thread.stop();

  errors += testConfiguration
            (
              cedar::unit::Time(0.0 * cedar::unit::milli * cedar::unit::second),
              cedar::unit::Time(0.01 * cedar::unit::milli * cedar::unit::second),
              cedar::unit::Time(1.0 * cedar::unit::milli * cedar::unit::second),
              cedar::aux::LoopMode::Fixed
            );

  std::cout << "Test finished, there were " << errors << " error(s)." << std::endl;
}

int main(int argc, char* argv[])
{
  app = new QCoreApplication(argc,argv);

  auto testThread = new cedar::aux::CallFunctionInThread(runTests);

  QObject::connect( testThread, SIGNAL(finishedThread()), app, SLOT(quit()), Qt::QueuedConnection );  // alternatively: call app->quit() in runTests()

  testThread->start();
  app->exec();

  delete testThread;
  delete app;

  return errors;
}
