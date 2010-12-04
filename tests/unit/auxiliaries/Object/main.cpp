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
//  log_file << "test: position" << std::endl;
//  object.setPosition(1337, 0, M_PI);
//  if (
//      object.positionX() != 1337.0
//      || object.positionY() != 0.0
//      || object.positionZ() != M_PI
//      )
//  {
//    errors++;
//    log_file << "ERROR with setPosition(double, double, double) or positionX/Y/Z()" << std::endl;
//  }
//
//  cv::Mat p = cv::Mat::ones(4, 1, CV_64FC1);
//  p.at<double>(0, 0) = 555.555;
//  p.at<double>(1, 0) = 2;
//  p.at<double>(2, 0) = sqrt(3);
//  object.setPosition(p);
//  cv::Mat q = object.position();
//  if (
//      q.at<double>(0, 0) != 555.555
//      || q.at<double>(1, 0) != 2.0
//      || q.at<double>(2, 0) != sqrt(3)
//      || q.at<double>(3, 0) != 1.0
//      )
//  {
//    errors++;
//    log_file << "ERROR with setPosition(Mat) or position()" << std::endl;
//  }
  
  //--------------------------------------------------------------------------------------------------------------------
  // orientation
  //--------------------------------------------------------------------------------------------------------------------
//  log_file << "test: orientation" << std::endl;
//  object.setOrientationAngles(1.0, 0, M_PI/4);
//  if (
//      object.orientationAngleAlpha() != 1.0
//      || object.orientationAngleBeta() != 0.0
//      || object.orientationAngleGamma() != M_PI/4
//      )
//  {
//    errors++;
//    log_file << "ERROR with setOrientationAngles(double, double, double) or orientationAngleAlpha/Beta/Gamma()" << std::endl;
//  }
//
//  cv::Mat o = cv::Mat::ones(3, 1, CV_64FC1);
//  o.at<double>(0, 0) = 9*M_PI;
//  o.at<double>(1, 0) = 2;
//  o.at<double>(2, 0) = 0.0;
//  object.setOrientationAngles(o);
//  cv::Mat r = object.orientationAngles();
//  if (
//      !IsZero(r.at<double>(0, 0) - M_PI)
//      || r.at<double>(1, 0) != 2.0
//      || r.at<double>(2, 0) != 0.0
//      )
//  {
//    errors++;
//    log_file << "ERROR with setOrientationAngles(Mat) or orientationAngles()" << std::endl;
//  }
//  log_file << r.at<double>(0, 0) << std::endl;
  
  //--------------------------------------------------------------------------------------------------------------------
  // transformation
  //--------------------------------------------------------------------------------------------------------------------
//  log_file << "test: transformation" << std::endl;
//  object.setPosition(1, 10, 100);
//  object.setOrientationAngles(M_PI/2, M_PI/2, M_PI/4);
//  cv::Mat T = object.transformation();
//  if (
//      !IsZero(T.at<double>(0, 0) - -sqrt(2)/2)
//      || !IsZero(T.at<double>(0, 1) - -sqrt(2)/2)
//      || !IsZero(T.at<double>(0, 2) - 0)
//      || !IsZero(T.at<double>(1, 0) - 0)
//      || !IsZero(T.at<double>(1, 1) - 0)
//      || !IsZero(T.at<double>(1, 2) - 1)
//      || !IsZero(T.at<double>(2, 0) - -sqrt(2)/2)
//      || !IsZero(T.at<double>(2, 1) - sqrt(2)/2)
//      || !IsZero(T.at<double>(2, 2) - 0)
//      || !IsZero(T.at<double>(0, 3) - 1)
//      || !IsZero(T.at<double>(1, 3) - 10)
//      || !IsZero(T.at<double>(2, 3) - 100)
//      || !IsZero(T.at<double>(3, 0) - 0)
//      || !IsZero(T.at<double>(3, 1) - 0)
//      || !IsZero(T.at<double>(3, 2) - 0)
//      || !IsZero(T.at<double>(3, 3) - 1)
//      )
//  {
//    errors++;
//    log_file << "ERROR with transformation()" << std::endl;
//  }
  
  
//  log_file << T.at<double>(0, 0) << " " << T.at<double>(0, 1) << " " << T.at<double>(0, 2) << " " << T.at<double>(0, 3) << std::endl;
//  log_file << T.at<double>(1, 0) << " " << T.at<double>(1, 1) << " " << T.at<double>(1, 2) << " " << T.at<double>(1, 3) << std::endl;
//  log_file << T.at<double>(2, 0) << " " << T.at<double>(2, 1) << " " << T.at<double>(2, 2) << " " << T.at<double>(2, 3) << std::endl;
//  log_file << T.at<double>(3, 0) << " " << T.at<double>(3, 1) << " " << T.at<double>(3, 2) << " " << T.at<double>(3, 3) << std::endl;
  
  

  log_file << "test finished, there were " << errors << " errors" << std::endl;
  if (errors > 255)
  {
    errors = 255;
  }
  return errors;
}











