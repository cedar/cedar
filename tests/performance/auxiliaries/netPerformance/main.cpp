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
    Date:        2013 06 21

    Description: Test NetReader/Writer performance

    Credits:

======================================================================================================================*/


// LOCAL INCLUDES
#include "cedar/configuration.h"

#ifdef CEDAR_USE_YARP

#include "cedar/testingUtilities/measurementFunctions.h"
#include "cedar/auxiliaries/net/BlockingReader.h"
#include "cedar/auxiliaries/net/Writer.h"
#include "cedar/auxiliaries/CallFunctionInThread.h"

// SYSTEM INCLUDES



// global variables
int errors = 0;

#define SIZE 15000
#define MYPORT "CEDAR-PERFORMANCE-TEST"

void mat_read_write()
{
  unsigned int i;

  cv::Mat mat = cv::Mat::eye(SIZE, 2, CV_64F);
  cv::Mat mat2 = cv::Mat::eye(SIZE, 2, CV_64F);

  i = 0;
  for (; i < SIZE; i++)
  {
    mat.at<double>(i,0)= i; // fill with anything
  }

  cedar::aux::net::Writer<cv::Mat> myMatWriter(MYPORT);
  cedar::aux::net::BlockingReader<cv::Mat> myMatReader(MYPORT);

  myMatWriter.write(mat);
  mat2 = myMatReader.read();

  // dont need to check all the results, just the last

  if (mat2.at<double>(SIZE - 1 , 0) != i - 1)
  {
    errors++;
  }
}

void run_test()
{
  errors = 0;

  cedar::test::test_time("read/write cycle (cv::Mat)", mat_read_write);
}

int main(int argc, char* argv[])
{
  QCoreApplication* app;
  app = new QCoreApplication(argc,argv);

  auto test_thread = new cedar::aux::CallFunctionInThread(run_test);

  QObject::connect(test_thread, SIGNAL(finishedThread()), app, SLOT(quit()), Qt::QueuedConnection);
      // alternatively: call app->quit() in runTests()

  test_thread->start();
  app->exec();

  delete test_thread;
  delete app;

  return errors;
}

#endif // CEDAR_USE_YARP

