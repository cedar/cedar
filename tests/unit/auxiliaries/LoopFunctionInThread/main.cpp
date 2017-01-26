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
    Email:       jean-stephane.jokeit@ini.rub.de
    Date:        2013 01 07

    Description: Example for the behavior of the @em cedar::aux::Thread class.

    Credits:

======================================================================================================================*/


#include "cedar/auxiliaries/CallFunctionInThread.h"
#include "cedar/auxiliaries/LoopFunctionInThread.h"
#include "cedar/auxiliaries/sleepFunctions.h"

#include <cstdlib>
#include <iostream>

#include <QCoreApplication>

#ifndef Q_MOC_RUN
  #include <boost/lexical_cast.hpp>
#endif

// global variables
int errors = 0;
int count_loops = 0;

cedar::aux::LoopFunctionInThread* loop;

#define MAX_LOOPS 5

void loopedFun(cedar::unit::Time)
{
  count_loops++;

  if (count_loops == MAX_LOOPS)
    loop->requestStop();
}

void runTests()
{
  loop = new cedar::aux::LoopFunctionInThread(loopedFun);

  using namespace cedar::unit;
  Time step_size(100.0 * milli * seconds);
  loop->setStepSize(step_size);

  loop->start();

  Time waiting_time(2.0 * seconds);
  loop->wait();
  cedar::aux::sleep(waiting_time);

  if (count_loops != MAX_LOOPS)
    errors++;

  delete loop;
}

int main(int argc, char* argv[])
{
  QCoreApplication* app;
  app = new QCoreApplication(argc,argv);

  auto testThread = new cedar::aux::CallFunctionInThread(runTests);

  QObject::connect(testThread, SIGNAL(finishedThread()), app, SLOT(quit()), Qt::QueuedConnection);  // alternatively: call app->quit() in runTests()

  testThread->start();
  app->exec();

  delete testThread;
  delete app;

  return errors;
}


