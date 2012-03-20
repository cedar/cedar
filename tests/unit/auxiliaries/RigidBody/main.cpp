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

 ----- Description: Implements all unit tests for the @em cedar::aux::RigidBody class.

 ----- Credits:
 ---------------------------------------------------------------------------------------------------------------------*/

// LOCAL INCLUDES

// PROJECT INCLUDES
#include "cedar/auxiliaries/RigidBody.h"
#include "cedar/auxiliaries/math/tools.h"
#include "cedar/auxiliaries/math/constants.h"

// SYSTEM INCLUDES
#include <string>
#include <math.h>


int main()
{
  // the number of errors encountered in this test
  int errors = 0;
  
  // create instance of test class
  cedar::aux::RigidBody rigid_body;
  
  //--------------------------------------------------------------------------------------------------------------------
  // position
  //--------------------------------------------------------------------------------------------------------------------
  std::cout << "test: position" << std::endl;
  rigid_body.setPosition(1337, 0, cedar::aux::math::pi);
  if (
      rigid_body.getPositionX() != 1337.0
      || rigid_body.getPositionY() != 0.0
      || rigid_body.getPositionZ() != cedar::aux::math::pi
      )
  {
    errors++;
    std::cout << "ERROR with setPosition(double, double, double) or getPositionX/Y/Z()" << std::endl;
  }

  cv::Mat p1 = cv::Mat::ones(4, 1, CV_64FC1);
  p1.at<double>(0, 0) = 555.555;
  p1.at<double>(1, 0) = 2;
  p1.at<double>(2, 0) = sqrt(3.0);
  rigid_body.setPosition(p1);
  cv::Mat p2 = rigid_body.getPosition();
  if (
      p2.at<double>(0, 0) != 555.555
      || p2.at<double>(1, 0) != 2.0
      || p2.at<double>(2, 0) != sqrt(3.0)
      || p2.at<double>(3, 0) != 1.0
      )
  {
    errors++;
    std::cout << "ERROR with setPosition(Mat) or getPosition()" << std::endl;
  }
  
  //--------------------------------------------------------------------------------------------------------------------
  // orientation
  //--------------------------------------------------------------------------------------------------------------------
  std::cout << "test: orientation" << std::endl;
  cv::Mat q1 = cv::Mat::zeros(4, 1, CV_64FC1);
  q1.at<double>(1, 0) = 3.3;
  q1.at<double>(2, 0) = 4.4;
  q1.at<double>(3, 0) = 0.5;
  rigid_body.setOrientationQuaternion(q1);
  cv::Mat q2 = rigid_body.getOrientationQuaternion();
  if (
      !IsZero(q2.at<double>(0, 0) - 0.0)
      || q2.at<double>(1, 0) != 3.3
      || q2.at<double>(2, 0) != 4.4
      || q2.at<double>(3, 0) != 0.5
      )
  {
    errors++;
    std::cout << "ERROR with setOrientationQuaternion(Mat) or getOrientationQuaternion()" << std::endl;
  }

  //--------------------------------------------------------------------------------------------------------------------
  // transformation
  //--------------------------------------------------------------------------------------------------------------------
  std::cout << "test: transformation" << std::endl;
  rigid_body.setPosition(1, 10, 100);
  cv::Mat q3 = cv::Mat::zeros(4, 1, CV_64FC1);
  q3.at<double>(0, 0) = -0.923879532511287;
  q3.at<double>(2, 0) = 0.38268343236509;
  rigid_body.setOrientationQuaternion(q3);

  cv::Mat T = rigid_body.getTransformation();
  if (
      !IsZero(T.at<double>(0, 0) - sqrt(2.0)/2)
      || !IsZero(T.at<double>(0, 1) - 0.0)
      || !IsZero(T.at<double>(0, 2) - -sqrt(2.0)/2)
      || !IsZero(T.at<double>(1, 0) - 0)
      || !IsZero(T.at<double>(1, 1) - 1)
      || !IsZero(T.at<double>(1, 2) - 0)
      || !IsZero(T.at<double>(2, 0) - sqrt(2.0)/2)
      || !IsZero(T.at<double>(2, 1) - 0)
      || !IsZero(T.at<double>(2, 2) - sqrt(2.0)/2)
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
    std::cout << "ERROR with getTransformation()" << std::endl;
  }

  //--------------------------------------------------------------------------------------------------------------------
  // rotate
  //--------------------------------------------------------------------------------------------------------------------
  std::cout << "test: rotate" << std::endl;
  rigid_body.setPosition(0, 0, 0);
  cv::Mat q4 = cv::Mat::zeros(4, 1, CV_64FC1);
  q4.at<double>(0, 0) = 1.0;
  rigid_body.setOrientationQuaternion(q4);
  rigid_body.rotate(0, cedar::aux::math::pi/2);
  rigid_body.rotate(1, cedar::aux::math::pi/4);

  cv::Mat D = rigid_body.getTransformation();
  if (
      !IsZero(D.at<double>(0, 0) - sqrt(2.0)/2)
      || !IsZero(D.at<double>(0, 1) - 0.0)
      || !IsZero(D.at<double>(0, 2) - sqrt(2.0)/2)

      || !IsZero(D.at<double>(1, 0) - sqrt(2.0)/2)
      || !IsZero(D.at<double>(1, 1) - 0)
      || !IsZero(D.at<double>(1, 2) - -sqrt(2.0)/2)

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
    std::cout << "ERROR with rotate()" << std::endl;
  }

  //--------------------------------------------------------------------------------------------------------------------
  // readConfiguration
  //--------------------------------------------------------------------------------------------------------------------
  std::cout << "test: readConfiguration" << std::endl;
  cedar::aux::RigidBody configured_rigid_body;
  std::cout << "constructed cofigured_rigid_body" << std::endl;
  configured_rigid_body.readJson("test.json");
  std::cout << "called cofigured_rigid_body.readJson()" << std::endl;
  cv::Mat C = configured_rigid_body.getTransformation();
  cedar::aux::math::write(C);
  if (
      !IsZero(C.at<double>(0, 0) - cos(cedar::aux::math::pi/6))
      || !IsZero(C.at<double>(0, 1) - 0)
      || !IsZero(C.at<double>(0, 2) - -0.5)
      || !IsZero(C.at<double>(1, 0) - 0)
      || !IsZero(C.at<double>(1, 1) - 1)
      || !IsZero(C.at<double>(1, 2) - 0)
      || !IsZero(C.at<double>(2, 0) - 0.5)
      || !IsZero(C.at<double>(2, 1) - 0)
      || !IsZero(C.at<double>(2, 2) - cos(cedar::aux::math::pi/6))
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
    std::cout << "ERROR with configured_rigid_body(const std::string& configFileName)" << std::endl;
  }

  std::cout << "test finished, there were " << errors << " errors" << std::endl;
  if (errors > 255)
  {
    errors = 255;
  }
  return errors;
}











