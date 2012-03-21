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

 ----- Description: Implements all unit tests for the @em cedar::aux::LocalCoordinateFrame class.

 ----- Credits:
 ---------------------------------------------------------------------------------------------------------------------*/

// LOCAL INCLUDES

// PROJECT INCLUDES
#include "cedar/auxiliaries/LocalCoordinateFrame.h"
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
  cedar::aux::LocalCoordinateFrame local_coordinate_frame;
  
  //--------------------------------------------------------------------------------------------------------------------
  // translation
  //--------------------------------------------------------------------------------------------------------------------
  std::cout << "test: translation" << std::endl;
  local_coordinate_frame.setTranslation(1337, 0, cedar::aux::math::pi);
  if (
      local_coordinate_frame.getTranslationX() != 1337.0
      || local_coordinate_frame.getTranslationY() != 0.0
      || local_coordinate_frame.getTranslationZ() != cedar::aux::math::pi
      )
  {
    errors++;
    std::cout << "ERROR with setTranslation(double, double, double) or getTranslationX/Y/Z()" << std::endl;
  }

  cv::Mat p1 = cv::Mat::ones(4, 1, CV_64FC1);
  p1.at<double>(0, 0) = 555.555;
  p1.at<double>(1, 0) = 2;
  p1.at<double>(2, 0) = sqrt(3.0);
  local_coordinate_frame.setTranslation(p1);
  cv::Mat p2 = local_coordinate_frame.getTranslation();
  if (
      p2.at<double>(0, 0) != 555.555
      || p2.at<double>(1, 0) != 2.0
      || p2.at<double>(2, 0) != sqrt(3.0)
      || p2.at<double>(3, 0) != 1.0
      )
  {
    errors++;
    std::cout << "ERROR with setTranslation(Mat) or getTranslation()" << std::endl;
  }

  std::vector<double> translation;
  translation.push_back(1.2);
  translation.push_back(3.4);
  translation.push_back(5.6);
  local_coordinate_frame.setTranslation(translation);
  cv::Mat p3 = local_coordinate_frame.getTranslation();
  if (
      p3.at<double>(0, 0) != 1.2
      || p3.at<double>(1, 0) != 3.4
      || p3.at<double>(2, 0) != 5.6
      || p3.at<double>(3, 0) != 1.0
      )
  {
    errors++;
    std::cout << "ERROR with setTranslation(std::vector<double>) or getTranslation()" << std::endl;
  }

  local_coordinate_frame.translate(1.2, 3.4, 5.6);
  if (
      local_coordinate_frame.getTranslationX() != 2.4
      || local_coordinate_frame.getTranslationY() != 6.8
      || local_coordinate_frame.getTranslationZ() != 11.2
      )
  {
    errors++;
    std::cout << "ERROR with translate(double, double, double)" << std::endl;
  }

  p1 = cv::Mat::ones(4, 1, CV_64FC1);
  p1.at<double>(0, 0) = -0.4;
  p1.at<double>(1, 0) = -0.8;
  p1.at<double>(2, 0) = 0.8;
  local_coordinate_frame.translate(p1);
  p2 = local_coordinate_frame.getTranslation();
  if (
      p2.at<double>(0, 0) != 2.0
      || p2.at<double>(1, 0) != 6.0
      || p2.at<double>(2, 0) != 12.0
      || p2.at<double>(3, 0) != 1.0
      )
  {
    errors++;
    std::cout << "ERROR with translate(Mat)" << std::endl;
  }

  local_coordinate_frame.translate(translation);
  p3 = local_coordinate_frame.getTranslation();
  if (
      p3.at<double>(0, 0) != 3.2
      || p3.at<double>(1, 0) != 9.4
      || p3.at<double>(2, 0) != 17.6
      || p3.at<double>(3, 0) != 1.0
      )
  {
    errors++;
    std::cout << "ERROR with translate(std::vector<double>)" << std::endl;
  }

  //--------------------------------------------------------------------------------------------------------------------
  // rotate
  //--------------------------------------------------------------------------------------------------------------------
  std::cout << "test: rotate" << std::endl;
  local_coordinate_frame.setTranslation(0, 0, 0);
  local_coordinate_frame.rotate(0, cedar::aux::math::pi/2);
  local_coordinate_frame.rotate(1, cedar::aux::math::pi/4);

  cv::Mat D = local_coordinate_frame.getTransformation();
  if
  (
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

  cv::Mat R = cv::Mat::zeros(4, 4, CV_64FC1);
  R.at<double>(0, 1) = -1;
  R.at<double>(1, 2) = -1;
  R.at<double>(2, 0) = 1;
  local_coordinate_frame.setRotation(R);
  cv::Mat R_prime = local_coordinate_frame.getRotation();
  if
  (
    !IsZero(R_prime.at<double>(0, 0) - 0)
    || !IsZero(R_prime.at<double>(0, 1) - -1)
    || !IsZero(R_prime.at<double>(0, 2) - 0)

    || !IsZero(R_prime.at<double>(1, 0) - 0)
    || !IsZero(R_prime.at<double>(1, 1) - 0)
    || !IsZero(R_prime.at<double>(1, 2) - -1)

    || !IsZero(R_prime.at<double>(2, 0) - 1)
    || !IsZero(R_prime.at<double>(2, 1) - 0)
    || !IsZero(R_prime.at<double>(2, 2) - 0)
  )
  {
    errors++;
    std::cout << "ERROR with setRotation(Mat)" << std::endl;
  }

  cedar::aux::math::write(R_prime);

  std::vector<double> rotation;
  rotation.push_back(0);
  rotation.push_back(0);
  rotation.push_back(1);
  rotation.push_back(0);
  rotation.push_back(1);
  rotation.push_back(0);
  rotation.push_back(1);
  rotation.push_back(0);
  rotation.push_back(0);
  local_coordinate_frame.setRotation(rotation);
  R_prime = local_coordinate_frame.getRotation();
  if (
      !IsZero(R_prime.at<double>(0, 0) - 0)
      || !IsZero(R_prime.at<double>(0, 1) - 0)
      || !IsZero(R_prime.at<double>(0, 2) - 1)

      || !IsZero(R_prime.at<double>(1, 0) - 0)
      || !IsZero(R_prime.at<double>(1, 1) - 1)
      || !IsZero(R_prime.at<double>(1, 2) - 0)

      || !IsZero(R_prime.at<double>(2, 0) - 1)
      || !IsZero(R_prime.at<double>(2, 1) - 0)
      || !IsZero(R_prime.at<double>(2, 2) - 0)
      )
  {
    errors++;
    std::cout << "ERROR with setRotation(std::vector<double>)" << std::endl;
  }


  cedar::aux::math::write(R_prime);

  //--------------------------------------------------------------------------------------------------------------------
  // transformation
  //--------------------------------------------------------------------------------------------------------------------
  std::cout << "test: transformation" << std::endl;
  cv::Mat T = cv::Mat::ones(4, 4, CV_64FC1);
  T.at<double>(0, 0) = 0;
  T.at<double>(1, 0) = 1;
  T.at<double>(2, 0) = 0;
  T.at<double>(0, 1) = -1;
  T.at<double>(1, 1) = 0;
  T.at<double>(2, 1) = 0;
  T.at<double>(0, 2) = 0;
  T.at<double>(1, 2) = 0;
  T.at<double>(2, 2) = 1;
  T.at<double>(0, 3) = 5;
  T.at<double>(1, 3) = 6;
  T.at<double>(2, 3) = 7;
  local_coordinate_frame.setTransformation(T);
  if (
      local_coordinate_frame.getTransformation().at<double>(0, 0) != 0.0
      || local_coordinate_frame.getTransformation().at<double>(1, 0) != 1.0
      || local_coordinate_frame.getTransformation().at<double>(2, 0) != 0.0
      || local_coordinate_frame.getTransformation().at<double>(0, 1) != -1.0
      || local_coordinate_frame.getTransformation().at<double>(1, 1) != 0.0
      || local_coordinate_frame.getTransformation().at<double>(2, 1) != 0.0
      || local_coordinate_frame.getTransformation().at<double>(0, 2) != 0.0
      || local_coordinate_frame.getTransformation().at<double>(1, 2) != 0.0
      || local_coordinate_frame.getTransformation().at<double>(2, 2) != 1.0
      || local_coordinate_frame.getTransformation().at<double>(0, 3) != 5.0
      || local_coordinate_frame.getTransformation().at<double>(1, 3) != 6.0
      || local_coordinate_frame.getTransformation().at<double>(2, 3) != 7.0
      )
  {
    errors++;
    std::cout << "ERROR with setTransformation(cv::Mat) or getTransformation()" << std::endl;
  }

  //--------------------------------------------------------------------------------------------------------------------
  // readConfiguration
  //--------------------------------------------------------------------------------------------------------------------
  std::cout << "test: readConfiguration" << std::endl;
  cedar::aux::LocalCoordinateFrame configured_local_coordinate_frame;
  std::cout << "constructed cofigured_local_coordinate_frame" << std::endl;
  configured_local_coordinate_frame.readJson("test.json");
  std::cout << "called cofigured_local_coordinate_frame.readJson()" << std::endl;
  cv::Mat C = configured_local_coordinate_frame.getTransformation();
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
    std::cout << "ERROR with configured_local_coordinate_frame(const std::string& configFileName)" << std::endl;
  }

  std::cout << "test finished, there were " << errors << " errors" << std::endl;
  if (errors > 255)
  {
    errors = 255;
  }
  return errors;
}











