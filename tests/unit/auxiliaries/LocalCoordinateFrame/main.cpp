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
#include "cedar/auxiliaries/utilities.h"
#include "cedar/units/Length.h"
#include "cedar/units/PlaneAngle.h"

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
  local_coordinate_frame.setTranslation
  (
    1337.0 * cedar::unit::meters,
    0.0 * cedar::unit::meters,
    cedar::aux::math::pi * cedar::unit::meters
  );
  if (
         !cedar::aux::math::isZero<float>
          (
            local_coordinate_frame.getTranslationX() / cedar::unit::DEFAULT_LENGTH_UNIT - 1337.0
          )
      || !cedar::aux::math::isZero<float>
          (
            local_coordinate_frame.getTranslationY() / cedar::unit::DEFAULT_LENGTH_UNIT - 0.0
          )
      || !cedar::aux::math::isZero<float>
          (
            local_coordinate_frame.getTranslationZ() / cedar::unit::DEFAULT_LENGTH_UNIT - cedar::aux::math::pi
          )
      )
  {
    errors++;
    std::cout << "ERROR with setTranslation(float, float, float) or getTranslationX/Y/Z()" << std::endl;
  }

  cedar::unit::LengthMatrix p1(cv::Mat::ones(4, 1, CV_64FC1), 1 * cedar::unit::meters);
  p1.matrix.at<float>(0, 0) = 555.555;
  p1.matrix.at<float>(1, 0) = 2;
  p1.matrix.at<float>(2, 0) = sqrt(3.0);

  local_coordinate_frame.setTranslation(p1);
  cedar::unit::LengthMatrix p2 = local_coordinate_frame.getTranslation();
  if (
      p2.matrix.at<float>(0, 0) != 555.555
      || p2.matrix.at<float>(1, 0) != 2.0
      || p2.matrix.at<float>(2, 0) != sqrt(3.0)
      || p2.matrix.at<float>(3, 0) != 1.0
      )
  {
    errors++;
    std::cout << "ERROR with setTranslation(Mat) or getTranslation()" << std::endl;
  }

#if 0
// setTransformation( vector of cedar::unit::Length ) currently unavailable
  std::vector<cedar::unit::Length> translation;
  translation.push_back(1.2 * cedar::unit::meters);
  translation.push_back(3.4 * cedar::unit::meters);
  translation.push_back(5.6 * cedar::unit::meters);
  local_coordinate_frame.setTranslation(translation);
  cedar::unit::LengthMatrix p3 = local_coordinate_frame.getTranslation();
  if (
      p3.matrix.at<float>(0, 0) != 1.2
      || p3.matrix.at<float>(1, 0) != 3.4
      || p3.matrix.at<float>(2, 0) != 5.6
      || p3.matrix.at<float>(3, 0) != 1.0
      )
  {
    errors++;
    std::cout << "ERROR with setTranslation(std::vector<float>) or getTranslation()" << std::endl;
  }


  local_coordinate_frame.translate(1.2 * cedar::unit::meters, 3.4 * cedar::unit::meters, 5.6 * cedar::unit::meters);
  if (
      local_coordinate_frame.getTranslationX() != 2.4 * cedar::unit::meters
      || local_coordinate_frame.getTranslationY() != 6.8 * cedar::unit::meters
      || local_coordinate_frame.getTranslationZ() != 11.2 * cedar::unit::meters
      )
  {
    errors++;
    std::cout << "ERROR with translate(float, float, float)" << std::endl;
  }

  p1.matrix = cv::Mat::ones(4, 1, CV_64FC1);
  p1.matrix.at<float>(0, 0) = -0.4;
  p1.matrix.at<float>(1, 0) = -0.8;
  p1.matrix.at<float>(2, 0) = 0.8;
  local_coordinate_frame.translate(p1);
  p2 = local_coordinate_frame.getTranslation();
  if (
      p2.matrix.at<float>(0, 0) != 2.0
      || p2.matrix.at<float>(1, 0) != 6.0
      || p2.matrix.at<float>(2, 0) != 12.0
      || p2.matrix.at<float>(3, 0) != 1.0
      )
  {
    errors++;
    std::cout << "ERROR with translate(Mat)" << std::endl;
  }

  local_coordinate_frame.translate(translation);
  p3 = local_coordinate_frame.getTranslation();
  if (
      p3.matrix.at<float>(0, 0) != 3.2
      || p3.matrix.at<float>(1, 0) != 9.4
      || p3.matrix.at<float>(2, 0) != 17.6
      || p3.matrix.at<float>(3, 0) != 1.0
      )
  {
    errors++;
    std::cout << "ERROR with translate(std::vector<float>)" << std::endl;
  }
#endif

  //--------------------------------------------------------------------------------------------------------------------
  // rotate
  //--------------------------------------------------------------------------------------------------------------------
  std::cout << "test: rotate" << std::endl;
  local_coordinate_frame.setTranslation
  (
    0.0 * cedar::unit::meters,
    0.0 * cedar::unit::meters,
    0.0 * cedar::unit::meters
  );
  local_coordinate_frame.rotate(0, cedar::aux::math::pi/2 * cedar::unit::radians);
  local_coordinate_frame.rotate(1, cedar::aux::math::pi/4 * cedar::unit::radians);

  cv::Mat D = local_coordinate_frame.getTransformation();
  if
  (
    !cedar::aux::math::isZero(D.at<float>(0, 0) - sqrt(2.0)/2)
    || !cedar::aux::math::isZero(D.at<float>(0, 1) - 0.0)
    || !cedar::aux::math::isZero(D.at<float>(0, 2) - sqrt(2.0)/2)

    || !cedar::aux::math::isZero(D.at<float>(1, 0) - sqrt(2.0)/2)
    || !cedar::aux::math::isZero(D.at<float>(1, 1) - 0)
    || !cedar::aux::math::isZero(D.at<float>(1, 2) - -sqrt(2.0)/2)

    || !cedar::aux::math::isZero(D.at<float>(2, 0) - 0)
    || !cedar::aux::math::isZero(D.at<float>(2, 1) - 1)
    || !cedar::aux::math::isZero(D.at<float>(2, 2) - 0)

    || !cedar::aux::math::isZero(D.at<float>(0, 3) - 0)
    || !cedar::aux::math::isZero(D.at<float>(1, 3) - 0)
    || !cedar::aux::math::isZero(D.at<float>(2, 3) - 0)
    || !cedar::aux::math::isZero(D.at<float>(3, 0) - 0)
    || !cedar::aux::math::isZero(D.at<float>(3, 1) - 0)
    || !cedar::aux::math::isZero(D.at<float>(3, 2) - 0)
    || !cedar::aux::math::isZero(D.at<float>(3, 3) - 1)
  )
  {
    errors++;
    std::cout << "ERROR with rotate()" << std::endl;
  }

  cv::Mat R = cv::Mat::zeros(4, 4, CV_64FC1);
  R.at<float>(0, 1) = -1;
  R.at<float>(1, 2) = -1;
  R.at<float>(2, 0) = 1;
  local_coordinate_frame.setRotation(R);
  cv::Mat R_prime = local_coordinate_frame.getRotation();
  if
  (
    !cedar::aux::math::isZero(R_prime.at<float>(0, 0) - 0)
    || !cedar::aux::math::isZero(R_prime.at<float>(0, 1) - -1)
    || !cedar::aux::math::isZero(R_prime.at<float>(0, 2) - 0)

    || !cedar::aux::math::isZero(R_prime.at<float>(1, 0) - 0)
    || !cedar::aux::math::isZero(R_prime.at<float>(1, 1) - 0)
    || !cedar::aux::math::isZero(R_prime.at<float>(1, 2) - -1)

    || !cedar::aux::math::isZero(R_prime.at<float>(2, 0) - 1)
    || !cedar::aux::math::isZero(R_prime.at<float>(2, 1) - 0)
    || !cedar::aux::math::isZero(R_prime.at<float>(2, 2) - 0)
  )
  {
    errors++;
    std::cout << "ERROR with setRotation(Mat)" << std::endl;
  }

  cedar::aux::write(R_prime);

  std::vector<float> rotation;
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
      !cedar::aux::math::isZero(R_prime.at<float>(0, 0) - 0)
      || !cedar::aux::math::isZero(R_prime.at<float>(0, 1) - 0)
      || !cedar::aux::math::isZero(R_prime.at<float>(0, 2) - 1)

      || !cedar::aux::math::isZero(R_prime.at<float>(1, 0) - 0)
      || !cedar::aux::math::isZero(R_prime.at<float>(1, 1) - 1)
      || !cedar::aux::math::isZero(R_prime.at<float>(1, 2) - 0)

      || !cedar::aux::math::isZero(R_prime.at<float>(2, 0) - 1)
      || !cedar::aux::math::isZero(R_prime.at<float>(2, 1) - 0)
      || !cedar::aux::math::isZero(R_prime.at<float>(2, 2) - 0)
      )
  {
    errors++;
    std::cout << "ERROR with setRotation(std::vector<float>)" << std::endl;
  }

  cedar::aux::write(R_prime);

  //--------------------------------------------------------------------------------------------------------------------
  // transformation
  //--------------------------------------------------------------------------------------------------------------------
  std::cout << "test: transformation" << std::endl;
  cv::Mat T = cv::Mat::ones(4, 4, CV_64FC1);
  T.at<float>(0, 0) = 0;
  T.at<float>(1, 0) = 1;
  T.at<float>(2, 0) = 0;
  T.at<float>(0, 1) = -1;
  T.at<float>(1, 1) = 0;
  T.at<float>(2, 1) = 0;
  T.at<float>(0, 2) = 0;
  T.at<float>(1, 2) = 0;
  T.at<float>(2, 2) = 1;
  T.at<float>(0, 3) = 5;
  T.at<float>(1, 3) = 6;
  T.at<float>(2, 3) = 7;
  local_coordinate_frame.setTransformation(T);
  if (
      local_coordinate_frame.getTransformation().at<float>(0, 0) != 0.0
      || local_coordinate_frame.getTransformation().at<float>(1, 0) != 1.0
      || local_coordinate_frame.getTransformation().at<float>(2, 0) != 0.0
      || local_coordinate_frame.getTransformation().at<float>(0, 1) != -1.0
      || local_coordinate_frame.getTransformation().at<float>(1, 1) != 0.0
      || local_coordinate_frame.getTransformation().at<float>(2, 1) != 0.0
      || local_coordinate_frame.getTransformation().at<float>(0, 2) != 0.0
      || local_coordinate_frame.getTransformation().at<float>(1, 2) != 0.0
      || local_coordinate_frame.getTransformation().at<float>(2, 2) != 1.0
      || local_coordinate_frame.getTransformation().at<float>(0, 3) != 5.0
      || local_coordinate_frame.getTransformation().at<float>(1, 3) != 6.0
      || local_coordinate_frame.getTransformation().at<float>(2, 3) != 7.0
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
  configured_local_coordinate_frame.readJson("test://unit/auxiliaries/LocalCoordinateFrame/test.json");
  std::cout << "called cofigured_local_coordinate_frame.readJson()" << std::endl;
  cv::Mat C = configured_local_coordinate_frame.getTransformation();
  if (
      !cedar::aux::math::isZero(C.at<float>(0, 0) - cos(cedar::aux::math::pi/6))
      || !cedar::aux::math::isZero(C.at<float>(0, 1) - 0)
      || !cedar::aux::math::isZero(C.at<float>(0, 2) - -0.5)
      || !cedar::aux::math::isZero(C.at<float>(1, 0) - 0)
      || !cedar::aux::math::isZero(C.at<float>(1, 1) - 1)
      || !cedar::aux::math::isZero(C.at<float>(1, 2) - 0)
      || !cedar::aux::math::isZero(C.at<float>(2, 0) - 0.5)
      || !cedar::aux::math::isZero(C.at<float>(2, 1) - 0)
      || !cedar::aux::math::isZero(C.at<float>(2, 2) - cos(cedar::aux::math::pi/6))
      || !cedar::aux::math::isZero(C.at<float>(0, 3) - 2.0)
      || !cedar::aux::math::isZero(C.at<float>(1, 3) - 2.2)
      || !cedar::aux::math::isZero(C.at<float>(2, 3) - 2.5)
      || !cedar::aux::math::isZero(C.at<float>(3, 0) - 0)
      || !cedar::aux::math::isZero(C.at<float>(3, 1) - 0)
      || !cedar::aux::math::isZero(C.at<float>(3, 2) - 0)
      || !cedar::aux::math::isZero(C.at<float>(3, 3) - 1)
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
