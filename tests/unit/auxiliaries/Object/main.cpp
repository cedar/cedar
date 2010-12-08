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

 ----- Institute:   Ruhr-Universitaet Bochum
                    Institut fuer Neuroinformatik

 ----- File:        main.cpp

 ----- Author:      Hendrik Reimann
 ----- Email:       hendrik.reimann@ini.rub.de
 ----- Date:        2010 12 04

 ----- Description: Implements all unit tests for the @em cedar::aux::Object class.

 ----- Credits:
 ---------------------------------------------------------------------------------------------------------------------*/

// LOCAL INCLUDES

// PROJECT INCLUDES
#include "cedar/auxiliaries/Object.h"
#include "cedar/auxiliaries/math/tools.h"
#include "cedar/auxiliaries/LogFile.h"

// SYSTEM INCLUDES
#include <string>
#include <math.h>

using namespace cedar::aux;

int main()
{
  LogFile log_file("Object.log");
  log_file.addTimeStamp();
  log_file << std::endl;
  // the number of errors encountered in this test
  int errors = 0;
  
  // create instance of test class
  cedar::aux::Object object;
  
  //--------------------------------------------------------------------------------------------------------------------
  // position
  //--------------------------------------------------------------------------------------------------------------------
  log_file << "test: position" << std::endl;
  object.setPosition(1337, 0, M_PI);
  if (
      object.getPositionX() != 1337.0
      || object.getPositionY() != 0.0
      || object.getPositionZ() != M_PI
      )
  {
    errors++;
    log_file << "ERROR with setPosition(double, double, double) or getPositionX/Y/Z()" << std::endl;
  }

  cv::Mat p1 = cv::Mat::ones(4, 1, CV_64FC1);
  p1.at<double>(0, 0) = 555.555;
  p1.at<double>(1, 0) = 2;
  p1.at<double>(2, 0) = sqrt(3);
  object.setPosition(p1);
  cv::Mat p2 = object.getPosition();
  if (
      p2.at<double>(0, 0) != 555.555
      || p2.at<double>(1, 0) != 2.0
      || p2.at<double>(2, 0) != sqrt(3)
      || p2.at<double>(3, 0) != 1.0
      )
  {
    errors++;
    log_file << "ERROR with setPosition(Mat) or getPosition()" << std::endl;
  }
  
  //--------------------------------------------------------------------------------------------------------------------
  // orientation
  //--------------------------------------------------------------------------------------------------------------------
  log_file << "test: orientation" << std::endl;
  cv::Mat q1 = cv::Mat::zeros(4, 1, CV_64FC1);
  q1.at<double>(1, 0) = 3.3;
  q1.at<double>(2, 0) = 4.4;
  q1.at<double>(3, 0) = 0.5;
  object.setOrientationQuaternion(q1);
  cv::Mat q2 = object.getOrientationQuaternion();
  if (
      !IsZero(q2.at<double>(0, 0) - 0.0)
      || q2.at<double>(1, 0) != 3.3
      || q2.at<double>(2, 0) != 4.4
      || q2.at<double>(3, 0) != 0.5
      )
  {
    errors++;
    log_file << "ERROR with setOrientationQuaternion(Mat) or getOrientationQuaternion()" << std::endl;
  }

  //--------------------------------------------------------------------------------------------------------------------
  // transformation
  //--------------------------------------------------------------------------------------------------------------------
  log_file << "test: transformation" << std::endl;
  object.setPosition(1, 10, 100);
  cv::Mat q3 = cv::Mat::zeros(4, 1, CV_64FC1);
  q3.at<double>(0, 0) = -0.923879532511287;
  q3.at<double>(2, 0) = 0.38268343236509;
  object.setOrientationQuaternion(q3);

  cv::Mat T = object.getTransformation();
  if (
      !IsZero(T.at<double>(0, 0) - sqrt(2)/2)
      || !IsZero(T.at<double>(0, 1) - 0.0)
      || !IsZero(T.at<double>(0, 2) - -sqrt(2)/2)
      || !IsZero(T.at<double>(1, 0) - 0)
      || !IsZero(T.at<double>(1, 1) - 1)
      || !IsZero(T.at<double>(1, 2) - 0)
      || !IsZero(T.at<double>(2, 0) - sqrt(2)/2)
      || !IsZero(T.at<double>(2, 1) - 0)
      || !IsZero(T.at<double>(2, 2) - sqrt(2)/2)
      || !IsZero(T.at<double>(0, 3) - 1)
      || !IsZero(T.at<double>(1, 3) - 10)
      || !IsZero(T.at<double>(2, 3) - 100)
      || !IsZero(T.at<double>(3, 0) - 0)
      || !IsZero(T.at<double>(3, 1) - 0)
      || !IsZero(T.at<double>(3, 2) - 0)
      || !IsZero(T.at<double>(3, 3) - 1)
      )
  {
    errors++;
    log_file << "ERROR with getTransformation()" << std::endl;
  }

  //--------------------------------------------------------------------------------------------------------------------
  // rotate
  //--------------------------------------------------------------------------------------------------------------------
  log_file << "test: rotate" << std::endl;
  object.setPosition(0, 0, 0);
  cv::Mat q4 = cv::Mat::zeros(4, 1, CV_64FC1);
  q4.at<double>(0, 0) = 1.0;
  object.setOrientationQuaternion(q4);
  object.rotate(0, M_PI/2);
  object.rotate(1, M_PI/4);

  cv::Mat D = object.getTransformation();
  if (
      !IsZero(D.at<double>(0, 0) - sqrt(2)/2)
      || !IsZero(D.at<double>(0, 1) - 0.0)
      || !IsZero(D.at<double>(0, 2) - sqrt(2)/2)

      || !IsZero(D.at<double>(1, 0) - sqrt(2)/2)
      || !IsZero(D.at<double>(1, 1) - 0)
      || !IsZero(D.at<double>(1, 2) - -sqrt(2)/2)

      || !IsZero(D.at<double>(2, 0) - 0)
      || !IsZero(D.at<double>(2, 1) - 1)
      || !IsZero(D.at<double>(2, 2) - 0)

      || !IsZero(D.at<double>(0, 3) - 0)
      || !IsZero(D.at<double>(1, 3) - 0)
      || !IsZero(D.at<double>(2, 3) - 0)
      || !IsZero(D.at<double>(3, 0) - 0)
      || !IsZero(D.at<double>(3, 1) - 0)
      || !IsZero(D.at<double>(3, 2) - 0)
      || !IsZero(D.at<double>(3, 3) - 1)
      )
  {
    errors++;
    log_file << "ERROR with rotate()" << std::endl;
  }

  log_file << D.at<double>(0, 0) << " " << D.at<double>(0, 1) << " " << D.at<double>(0, 2) << " " << D.at<double>(0, 3) << std::endl;
  log_file << D.at<double>(1, 0) << " " << D.at<double>(1, 1) << " " << D.at<double>(1, 2) << " " << D.at<double>(1, 3) << std::endl;
  log_file << D.at<double>(2, 0) << " " << D.at<double>(2, 1) << " " << D.at<double>(2, 2) << " " << D.at<double>(2, 3) << std::endl;
  log_file << D.at<double>(3, 0) << " " << D.at<double>(3, 1) << " " << D.at<double>(3, 2) << " " << D.at<double>(3, 3) << std::endl;

  //--------------------------------------------------------------------------------------------------------------------
  // configuration file constructor
  //--------------------------------------------------------------------------------------------------------------------
  log_file << "test: configuration file constructor" << std::endl;
  cedar::aux::Object configured_object("test.conf");
  cv::Mat C = configured_object.getTransformation();
  if (
      !IsZero(C.at<double>(0, 0) - cos(M_PI/6))
      || !IsZero(C.at<double>(0, 1) - 0)
      || !IsZero(C.at<double>(0, 2) - -0.5)
      || !IsZero(C.at<double>(1, 0) - 0)
      || !IsZero(C.at<double>(1, 1) - 1)
      || !IsZero(C.at<double>(1, 2) - 0)
      || !IsZero(C.at<double>(2, 0) - 0.5)
      || !IsZero(C.at<double>(2, 1) - 0)
      || !IsZero(C.at<double>(2, 2) - cos(M_PI/6))
      || !IsZero(C.at<double>(0, 3) - 2.0)
      || !IsZero(C.at<double>(1, 3) - 2.2)
      || !IsZero(C.at<double>(2, 3) - 2.5)
      || !IsZero(C.at<double>(3, 0) - 0)
      || !IsZero(C.at<double>(3, 1) - 0)
      || !IsZero(C.at<double>(3, 2) - 0)
      || !IsZero(C.at<double>(3, 3) - 1)
      )
  {
    errors++;
    log_file << "ERROR with configured_object(const std::string& configFileName)" << std::endl;
  }

  log_file << "test finished, there were " << errors << " errors" << std::endl;
  if (errors > 255)
  {
    errors = 255;
  }
  return errors;
}











