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

    Maintainer:  Christian Bodenstein
    Email:       christian.bodenstein@ini.ruhr-uni-bochum.de
    Date:        2013 07 10

    Description: Test Recorder performance

    Credits:

======================================================================================================================*/


// LOCAL INCLUDES
#include "cedar/configuration.h"
#include "cedar/auxiliaries/Recorder.h"
#include "cedar/auxiliaries/MatData.h"
#include "cedar/auxiliaries/CallFunctionInThread.h"
#include "cedar/testingUtilities/measurementFunctions.h"

//SYSTEM INCLUDES
#include <iostream>

// global variables
int errors = 0;

void record()
{

  int sz[] = {40, 40, 40};
  cv::Mat mat1(3, sz, CV_64F, cv::Scalar::all(0));
  cv::Mat mat2(3, sz, CV_8S, cv::Scalar::all(-42));
  cv::Mat mat3(100,100,CV_8UC3);

  int timestep =  20;

  cedar::aux::MatDataPtr dataPtr = cedar::aux::MatDataPtr( new cedar::aux::MatData( mat1));
  cedar::aux::MatDataPtr dataPtr2 = cedar::aux::MatDataPtr( new cedar::aux::MatData( mat2));
  cedar::aux::MatDataPtr dataPtr3 = cedar::aux::MatDataPtr( new cedar::aux::MatData( mat3));

  cedar::aux::RecorderSingleton::getInstance()->registerData(dataPtr,timestep ,"Mat1");
  cedar::aux::RecorderSingleton::getInstance()->registerData(dataPtr2,timestep ,"Mat2");
  cedar::aux::RecorderSingleton::getInstance()->registerData(dataPtr3,timestep ,"Mat3");


  cedar::aux::RecorderSingleton::getInstance()->start();
  usleep(1000*1000*5);
  cedar::aux::RecorderSingleton::getInstance()->stop();


}

void run_test()
{
  errors= 0;

  cedar::test::test_time("Recording different Mats", record );
}



int main(int argc, char* argv[])
{
  QCoreApplication* app;
  app = new QCoreApplication(argc,argv);

  auto testThread = new cedar::aux::CallFunctionInThread(run_test);

  QObject::connect( testThread, SIGNAL(finishedThread()), app, SLOT(quit()), Qt::QueuedConnection );  // alternatively: call app->quit() in runTests()

  testThread->start();
  app->exec();

  delete testThread;
  delete app;

  return errors;
}


