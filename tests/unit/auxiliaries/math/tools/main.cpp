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

    File:        main.cpp

    Maintainer:  Stephan Zibner
    Email:       stephan.zibner@rub.de
    Date:        2010 11 11

    Description: Tests all math tools.

    Credits:

======================================================================================================================*/


// LOCAL INCLUDES

// PROJECT INCLUDES
#include "cedar/auxiliaries/math/tools.h"
#include "cedar/auxiliaries/LogFile.h"

// SYSTEM INCLUDES
#include <cv.h>

using namespace cedar::aux;
using namespace cedar::aux::math;

int main()
{
  LogFile log_file("UnitTestSigmoids.log");
  log_file.addTimeStamp();
  log_file << std::endl;
  // the number of errors encountered in this test
  int errors = 0;
  // the current test number
  int test_number = 0;

  // test stuff
  log_file << "test no" << test_number++ << std::endl;
  cv::Mat identity = cv::Mat::eye(3, 3, CV_64FC1);
  log_file << "this should be the 3x3 identity matrix:" << std::endl;
  write(identity);

  log_file << "test no" << test_number++ << std::endl;
  if (normalizeAngle(5.9) <= -M_PI || normalizeAngle(5.9) > M_PI)
  {
    log_file << "error in normalizeAngle(double)" << std::endl;
    errors++;
  }
  if (normalizeAngle(-M_PI) <= -M_PI || normalizeAngle(-M_PI) > M_PI)
  {
    log_file << "error in normalizeAngle(double)" << std::endl;
    errors++;
  }
  if (normalizeAngle(10000) <= -M_PI || normalizeAngle(10000) > M_PI)
  {
    log_file << "error in normalizeAngle(double)" << std::endl;
    errors++;
  }

  log_file << "test finished, there were " << errors << " errors" << std::endl;
  if (errors > 255)
  {
    errors = 255;
  }
  return errors;
}
