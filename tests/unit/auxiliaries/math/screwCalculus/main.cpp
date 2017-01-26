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

    Maintainer:  Hendrik Reimann
    Email:       hendrik.reimann@ini.rub.de
    Date:        2010 11 11

    Description: Tests all screw calculus functions.

    Credits:

======================================================================================================================*/


// LOCAL INCLUDES

// PROJECT INCLUDES
#include "cedar/auxiliaries/math/screwCalculus.h"
#include "cedar/auxiliaries/math/tools.h"
#include "cedar/auxiliaries/math/constants.h"

// SYSTEM INCLUDES
#include <opencv2/opencv.hpp>

int main()
{
  // the number of errors encountered in this test
  int errors = 0;
  
  // matrices of varying size being used throughout the testing
  double theta;
  cv::Mat vec3_double(3, 1, CV_64FC1);
  cv::Mat mat33_double(3, 3, CV_64FC1);
  cv::Mat vec6_double(6, 1, CV_64FC1);
  cv::Mat mat44_double(4, 4, CV_64FC1);
  cv::Mat mat66_double(6, 6, CV_64FC1);
  cv::Mat inverse66_double(6, 6, CV_64FC1);
  cv::Mat vec3_float(3, 1, CV_32FC1);
  cv::Mat mat33_float(3, 3, CV_32FC1);
  cv::Mat vec6_float(6, 1, CV_32FC1);
  cv::Mat mat44_float(4, 4, CV_32FC1);
  cv::Mat mat66_float(6, 6, CV_32FC1);
  cv::Mat inverse66_float(6, 6, CV_32FC1);

  //--------------------------------------------------------------------------------------------------------------------
  // wedgeAxis
  //--------------------------------------------------------------------------------------------------------------------
  std::cout << "test: wedgeAxis" << std::endl;
  vec3_double.at<double>(0, 0) = 3;
  vec3_double.at<double>(1, 0) = 5;
  vec3_double.at<double>(2, 0) = 7;
  cedar::aux::math::wedgeAxis(vec3_double, mat33_double);
  if (!
       (
        cedar::aux::math::isZero(mat33_double.at<double>(0, 0) - 0.0)
        && cedar::aux::math::isZero(mat33_double.at<double>(0, 1) - -7.0)
        && cedar::aux::math::isZero(mat33_double.at<double>(0, 2) - 5.0)
        && cedar::aux::math::isZero(mat33_double.at<double>(1, 0) - 7.0)
        && cedar::aux::math::isZero(mat33_double.at<double>(1, 1) - 0.0)
        && cedar::aux::math::isZero(mat33_double.at<double>(1, 2) - -3.0)
        && cedar::aux::math::isZero(mat33_double.at<double>(2, 0) - -5.0)
        && cedar::aux::math::isZero(mat33_double.at<double>(2, 1) - 3.0)
        && cedar::aux::math::isZero(mat33_double.at<double>(2, 2) - 0.0)
       )
     )
  {
    errors++;
    std::cout << "ERROR in function wedgeAxis[double](cv::Mat& axis, cv::Mat& result)" << std::endl;
  }
  if (!cedar::aux::math::isZero(cv::norm(mat33_double - cedar::aux::math::wedgeAxis(vec3_double))))
  {
    errors++;
    std::cout << "ERROR in function wedgeAxis[double](cv::Mat& axis)" << std::endl;
  }

  std::cout << "test: wedgeAxis" << std::endl;
  vec3_float.at<float>(0, 0) = 5;
  vec3_float.at<float>(1, 0) = 55;
  vec3_float.at<float>(2, 0) = 555;
  cedar::aux::math::wedgeAxis(vec3_float, mat33_float);
  if (!
       (
        mat33_float.at<float>(0, 0) == 0.0
        && mat33_float.at<float>(0, 1) == -555.0
        && mat33_float.at<float>(0, 2) == 55.0
        && mat33_float.at<float>(1, 0) == 555.0
        && mat33_float.at<float>(1, 1) == 0.0
        && mat33_float.at<float>(1, 2) == -5.0
        && mat33_float.at<float>(2, 0) == -55.0
        && mat33_float.at<float>(2, 1) == 5.0
        && mat33_float.at<float>(2, 2) == 0.0
       )
     )
  {
    errors++;
    std::cout << "ERROR in function wedgeAxis[float](cv::Mat& vector, cv::Mat& result)" << std::endl;
  }
  if (!cedar::aux::math::isZero(cv::norm(mat33_float - cedar::aux::math::wedgeAxis(vec3_float)), 1e-5))
  {
    errors++;
    std::cout << "ERROR in function wedgeAxis[float](cv::Mat& vector)" << std::endl;
  }

  //--------------------------------------------------------------------------------------------------------------------
  // veeAxis
  //--------------------------------------------------------------------------------------------------------------------
  std::cout << "test: veeAxis" << std::endl;
  vec3_double *= 0;
  cedar::aux::math::veeAxis(mat33_double, vec3_double);
  if (!
       (
        vec3_double.at<double>(0, 0) == 3.0
        && vec3_double.at<double>(1, 0) == 5.0
        && vec3_double.at<double>(2, 0) == 7.0
       )
     )
  {
    errors++;
    std::cout << "ERROR in function veeAxis<double>(cv::Mat& vector, cv::Mat& result)" << std::endl;
  }
  if (!cedar::aux::math::isZero(cv::norm(vec3_double - cedar::aux::math::veeAxis(mat33_double))))
  {
    errors++;
    std::cout << "ERROR in function veeAxis<double>(cv::Mat& matrix)" << std::endl;
  }

  std::cout << "test: veeAxis" << std::endl;
  vec3_float *= 0;
  cedar::aux::math::veeAxis(mat33_float, vec3_float);
  if (!
       (
        vec3_float.at<float>(0, 0) == 5.0
        && vec3_float.at<float>(1, 0) == 55.0
        && vec3_float.at<float>(2, 0) == 555.0
       )
     )
  {
    errors++;
    std::cout << "ERROR in function veeAxis<float>(cv::Mat& matrix, cv::Mat& result)" << std::endl;
  }
  if (!cedar::aux::math::isZero(cv::norm(vec3_float - cedar::aux::math::veeAxis(mat33_float)), 1e-5))
  {
    errors++;
    std::cout << "ERROR in function veeAxis<float>(cv::Mat& matrix)" << std::endl;
  }

  //--------------------------------------------------------------------------------------------------------------------
  // wedgeTwist
  //--------------------------------------------------------------------------------------------------------------------
  std::cout << "test: wedgeTwist" << std::endl;
  vec6_double.at<double>(0, 0) = 2;
  vec6_double.at<double>(1, 0) = 3;
  vec6_double.at<double>(2, 0) = 5;
  vec6_double.at<double>(3, 0) = 7;
  vec6_double.at<double>(4, 0) = 11;
  vec6_double.at<double>(5, 0) = 13;
  cedar::aux::math::wedgeTwist(vec6_double, mat44_double);
  if (!
       (
        cedar::aux::math::isZero(mat44_double.at<double>(0, 0) - 0.0)
        && cedar::aux::math::isZero(mat44_double.at<double>(0, 1) - -13.0)
        && cedar::aux::math::isZero(mat44_double.at<double>(0, 2) - 11.0)
        && cedar::aux::math::isZero(mat44_double.at<double>(0, 3) - 2.0)
        && cedar::aux::math::isZero(mat44_double.at<double>(1, 0) - 13.0)
        && cedar::aux::math::isZero(mat44_double.at<double>(1, 1) - 0.0)
        && cedar::aux::math::isZero(mat44_double.at<double>(1, 2) - -7.0)
        && cedar::aux::math::isZero(mat44_double.at<double>(1, 3) - 3.0)
        && cedar::aux::math::isZero(mat44_double.at<double>(2, 0) - -11.0)
        && cedar::aux::math::isZero(mat44_double.at<double>(2, 1) - 7.0)
        && cedar::aux::math::isZero(mat44_double.at<double>(2, 2) - 0.0)
        && cedar::aux::math::isZero(mat44_double.at<double>(2, 3) - 5.0)
        && cedar::aux::math::isZero(mat44_double.at<double>(3, 0) - 0.0)
        && cedar::aux::math::isZero(mat44_double.at<double>(3, 1) - 0.0)
        && cedar::aux::math::isZero(mat44_double.at<double>(3, 2) - 0.0)
        && cedar::aux::math::isZero(mat44_double.at<double>(3, 3) - 0.0)
       )
     )
  {
    errors++;
    std::cout << "ERROR in function wedgeTwist<double>(cv::Mat& twist, cv::Mat& result)" << std::endl;
  }
  if (!cedar::aux::math::isZero(cv::norm(mat44_double - cedar::aux::math::wedgeTwist(vec6_double))))
  {
    errors++;
    std::cout << "ERROR in function wedgeTwist<double>(cv::Mat& twist)" << std::endl;
  }

  std::cout << "test: wedgeTwist" << std::endl;
  vec6_float.at<float>(0, 0) = 2;
  vec6_float.at<float>(1, 0) = 3;
  vec6_float.at<float>(2, 0) = 5;
  vec6_float.at<float>(3, 0) = 7;
  vec6_float.at<float>(4, 0) = 11;
  vec6_float.at<float>(5, 0) = 13;
  cedar::aux::math::wedgeTwist(vec6_float, mat44_float);
  if (!
       (
        cedar::aux::math::isZero(mat44_float.at<float>(0, 0) - 0.0, 1e-5)
        && cedar::aux::math::isZero(mat44_float.at<float>(0, 1) - -13.0, 1e-5)
        && cedar::aux::math::isZero(mat44_float.at<float>(0, 2) - 11.0, 1e-5)
        && cedar::aux::math::isZero(mat44_float.at<float>(0, 3) - 2.0, 1e-5)
        && cedar::aux::math::isZero(mat44_float.at<float>(1, 0) - 13.0, 1e-5)
        && cedar::aux::math::isZero(mat44_float.at<float>(1, 1) - 0.0, 1e-5)
        && cedar::aux::math::isZero(mat44_float.at<float>(1, 2) - -7.0, 1e-5)
        && cedar::aux::math::isZero(mat44_float.at<float>(1, 3) - 3.0, 1e-5)
        && cedar::aux::math::isZero(mat44_float.at<float>(2, 0) - -11.0, 1e-5)
        && cedar::aux::math::isZero(mat44_float.at<float>(2, 1) - 7.0, 1e-5)
        && cedar::aux::math::isZero(mat44_float.at<float>(2, 2) - 0.0, 1e-5)
        && cedar::aux::math::isZero(mat44_float.at<float>(2, 3) - 5.0, 1e-5)
        && cedar::aux::math::isZero(mat44_float.at<float>(3, 0) - 0.0, 1e-5)
        && cedar::aux::math::isZero(mat44_float.at<float>(3, 1) - 0.0, 1e-5)
        && cedar::aux::math::isZero(mat44_float.at<float>(3, 2) - 0.0, 1e-5)
        && cedar::aux::math::isZero(mat44_float.at<float>(3, 3) - 0.0, 1e-5)
       )
     )
  {
    errors++;
    std::cout << "ERROR in function wedgeTwist<float>(cv::Mat& twist, cv::Mat& result)" << std::endl;
  }
  if (!cedar::aux::math::isZero(cv::norm(mat44_float - cedar::aux::math::wedgeTwist(vec6_float)), 1e-5))
  {
    errors++;
    std::cout << "ERROR in function wedgeTwist<float>(cv::Mat& twist)" << std::endl;
  }

  //--------------------------------------------------------------------------------------------------------------------
  // veeTwist
  //--------------------------------------------------------------------------------------------------------------------
  std::cout << "test: veeTwist" << std::endl;
  vec6_double *= 0;
  cedar::aux::math::veeTwist(mat44_double, vec6_double);
  if (!
       (
        vec6_double.at<double>(0, 0) == 2.0
        && vec6_double.at<double>(1, 0) == 3.0
        && vec6_double.at<double>(2, 0) == 5.0
        && vec6_double.at<double>(3, 0) == 7.0
        && vec6_double.at<double>(4, 0) == 11.0
        && vec6_double.at<double>(5, 0) == 13.0
       )
     )
  {
    errors++;
    std::cout << "ERROR in function veeTwist<double>(cv::Mat& vector, cv::Mat& result)" << std::endl;
  }
  if (!cedar::aux::math::isZero(cv::norm(vec6_double - cedar::aux::math::veeTwist(mat44_double))))
  {
    errors++;
    std::cout << "ERROR in function veeTwist<double>(cv::Mat& matrix)" << std::endl;
  }

  std::cout << "test: veeTwist" << std::endl;
  vec6_float *= 0;
  cedar::aux::math::veeTwist(mat44_float, vec6_float);
  if (!
       (
        vec6_float.at<float>(0, 0) == 2.0
        && vec6_float.at<float>(1, 0) == 3.0
        && vec6_float.at<float>(2, 0) == 5.0
        && vec6_float.at<float>(3, 0) == 7.0
        && vec6_float.at<float>(4, 0) == 11.0
        && vec6_float.at<float>(5, 0) == 13.0
       )
     )
  {
    errors++;
    std::cout << "ERROR in function veeTwist<float>(cv::Mat& vector, cv::Mat& result)" << std::endl;
  }
  if (!cedar::aux::math::isZero(cv::norm(vec6_float - cedar::aux::math::veeTwist(mat44_float)), 1e-5))
  {
    errors++;
    std::cout << "ERROR in function veeTwist<float>(cv::Mat& matrix)" << std::endl;
  }

  //--------------------------------------------------------------------------------------------------------------------
  // expAxis
  //--------------------------------------------------------------------------------------------------------------------
  std::cout << "test: expAxis" << std::endl;
  vec3_double.at<double>(0, 0) = 1;
  vec3_double.at<double>(1, 0) = 2;
  vec3_double.at<double>(2, 0) = 3;
  vec3_double = vec3_double * (1 / cv::norm(vec3_double));
  cedar::aux::math::expAxis(vec3_double, 1, mat33_double);
  if (!
       (
        cedar::aux::math::isZero(mat33_double.at<double>(0, 0) - 0.57313785544898699)
        && cedar::aux::math::isZero(mat33_double.at<double>(0, 1) - -0.60900664213739331)
        && cedar::aux::math::isZero(mat33_double.at<double>(0, 2) - 0.54829180960859991)
        && cedar::aux::math::isZero(mat33_double.at<double>(1, 0) - 0.74034884046078198)
        && cedar::aux::math::isZero(mat33_double.at<double>(1, 1) - 0.67164450419152844)
        && cedar::aux::math::isZero(mat33_double.at<double>(1, 2) - -0.027879282947946255)
        && cedar::aux::math::isZero(mat33_double.at<double>(2, 0) - -0.35127851212351696)
        && cedar::aux::math::isZero(mat33_double.at<double>(2, 1) - 0.42190587791811218)
        && cedar::aux::math::isZero(mat33_double.at<double>(2, 2) - 0.83582225209576411)
       )
     )
  {
    errors++;
    std::cout << "ERROR in function expAxis<double>(const cv::Mat& matrix, double theta, cv::Mat& result)" << std::endl;
  }
  if (!cedar::aux::math::isZero(cv::norm(mat33_double - cedar::aux::math::expAxis(vec3_double, 1))))
  {
    errors++;
    std::cout << "ERROR in function expAxis<double>(const cv::Mat& matrix, double theta)" << std::endl;
  }

  std::cout << "test: expAxis" << std::endl;
  vec3_float.at<float>(0, 0) = 1;
  vec3_float.at<float>(1, 0) = 2;
  vec3_float.at<float>(2, 0) = 3;
  vec3_float = vec3_float * (1 / cv::norm(vec3_float));
  cedar::aux::math::expAxis(vec3_float, 1, mat33_float);
  if (!
       (
        cedar::aux::math::isZero(mat33_float.at<float>(0, 0) - 0.57313785544898699, 1e-5)
        && cedar::aux::math::isZero(mat33_float.at<float>(0, 1) - -0.60900664213739331, 1e-5)
        && cedar::aux::math::isZero(mat33_float.at<float>(0, 2) - 0.54829180960859991, 1e-5)
        && cedar::aux::math::isZero(mat33_float.at<float>(1, 0) - 0.74034884046078198, 1e-5)
        && cedar::aux::math::isZero(mat33_float.at<float>(1, 1) - 0.67164450419152844, 1e-5)
        && cedar::aux::math::isZero(mat33_float.at<float>(1, 2) - -0.027879282947946255, 1e-5)
        && cedar::aux::math::isZero(mat33_float.at<float>(2, 0) - -0.35127851212351696, 1e-5)
        && cedar::aux::math::isZero(mat33_float.at<float>(2, 1) - 0.42190587791811218, 1e-5)
        && cedar::aux::math::isZero(mat33_float.at<float>(2, 2) - 0.83582225209576411, 1e-5)
       )
     )
  {
    errors++;
    std::cout << "ERROR in function expAxis<float>(const cv::Mat& matrix, float theta, cv::Mat& result)" << std::endl;
  }
  if (!cedar::aux::math::isZero(cv::norm(mat33_float - cedar::aux::math::expAxis(vec3_float, 1)), 1e-5))
  {
    errors++;
    std::cout << "ERROR in function expAxis<float>(const cv::Mat& matrix, double theta)" << std::endl;
  }

  //--------------------------------------------------------------------------------------------------------------------
  // logAxis
  //--------------------------------------------------------------------------------------------------------------------
  std::cout << "test: logAxis" << std::endl;
  vec3_double *= 0;
  cedar::aux::math::logAxis(mat33_double, vec3_double, theta);
  if (!
       (
        cedar::aux::math::isZero(vec3_double.at<double>(0, 0) - 0.2672612419124244)
        && cedar::aux::math::isZero(vec3_double.at<double>(1, 0) - 0.53452248382484879)
        && cedar::aux::math::isZero(vec3_double.at<double>(2, 0) - 0.80178372573727308)
       )
     )
  {
    errors++;
    std::cout << "ERROR in function logAxis<double>(const cv::Mat& rotation, cv::Mat& omega, "
              << "double& theta, bool optionalThetaChoice = false)" << std::endl;
  }

  std::cout << "test: logAxis" << std::endl;
  vec3_float *= 0;
  cedar::aux::math::logAxis(mat33_float, vec3_float, theta);
  if (!
       (
        cedar::aux::math::isZero(vec3_float.at<float>(0, 0) - 0.2672612419124244, 1e-5)
        && cedar::aux::math::isZero(vec3_float.at<float>(1, 0) - 0.53452248382484879, 1e-5)
        && cedar::aux::math::isZero(vec3_float.at<float>(2, 0) - 0.80178372573727308, 1e-5)
       )
     ) 
  {
    errors++;
    std::cout << "ERROR in function logAxis<float>(const cv::Mat& rotation, cv::Mat& omega, "
              << "double& theta, bool optionalThetaChoice = false)" << std::endl;
  }

  //--------------------------------------------------------------------------------------------------------------------
  // expTwist
  //--------------------------------------------------------------------------------------------------------------------
  std::cout << "test: expTwist" << std::endl;
  vec6_double.at<double>(0, 0) = - vec3_double.at<double>(1, 0) + vec3_double.at<double>(2, 0);
  vec6_double.at<double>(1, 0) = - vec3_double.at<double>(2, 0) + vec3_double.at<double>(0, 0);
  vec6_double.at<double>(2, 0) = - vec3_double.at<double>(0, 0) + vec3_double.at<double>(1, 0);
  vec6_double.at<double>(3, 0) = vec3_double.at<double>(0, 0);
  vec6_double.at<double>(4, 0) = vec3_double.at<double>(1, 0);
  vec6_double.at<double>(5, 0) = vec3_double.at<double>(2, 0);
  cedar::aux::math::expTwist(vec6_double, cedar::aux::math::pi, mat44_double);
  if (!
       (
        cedar::aux::math::isZero(mat44_double.at<double>(0, 0) - -0.85714285714285676)
        && cedar::aux::math::isZero(mat44_double.at<double>(0, 1) - 0.28571428571428564)
        && cedar::aux::math::isZero(mat44_double.at<double>(0, 2) - 0.4285714285714286)
        && cedar::aux::math::isZero(mat44_double.at<double>(0, 3) - 1.1428571428571423)
        && cedar::aux::math::isZero(mat44_double.at<double>(1, 0) - 0.28571428571428586)
        && cedar::aux::math::isZero(mat44_double.at<double>(1, 1) - -0.42857142857142816)
        && cedar::aux::math::isZero(mat44_double.at<double>(1, 2) - 0.8571428571428571)
        && cedar::aux::math::isZero(mat44_double.at<double>(1, 3) - 0.28571428571428537)
        && cedar::aux::math::isZero(mat44_double.at<double>(2, 0) - 0.42857142857142849)
        && cedar::aux::math::isZero(mat44_double.at<double>(2, 1) - 0.8571428571428571)
        && cedar::aux::math::isZero(mat44_double.at<double>(2, 2) - 0.28571428571428559)
        && cedar::aux::math::isZero(mat44_double.at<double>(2, 3) - -0.57142857142857117)
        && cedar::aux::math::isZero(mat44_double.at<double>(3, 0) - 0.0)
        && cedar::aux::math::isZero(mat44_double.at<double>(3, 1) - 0.0)
        && cedar::aux::math::isZero(mat44_double.at<double>(3, 2) - 0.0)
        && cedar::aux::math::isZero(mat44_double.at<double>(3, 3) - 1.0)
       )
     )
  {
    errors++;
    std::cout << "ERROR in function expTwist<double>(const cv::Mat& xi, double theta, cv::Mat& result)" << std::endl;
  }
  if (!cedar::aux::math::isZero(cv::norm(mat44_double - cedar::aux::math::expTwist(vec6_double, cedar::aux::math::pi))))
  {
    errors++;
    std::cout << "ERROR in function expTwist<double>(const cv::Mat& xi, double theta, cv::Mat& result)" << std::endl;
  }

  std::cout << "test: expTwist" << std::endl;
  vec6_float.at<float>(0, 0) = - vec3_float.at<float>(1, 0) + vec3_float.at<float>(2, 0);
  vec6_float.at<float>(1, 0) = - vec3_float.at<float>(2, 0) + vec3_float.at<float>(0, 0);
  vec6_float.at<float>(2, 0) = - vec3_float.at<float>(0, 0) + vec3_float.at<float>(1, 0);
  vec6_float.at<float>(3, 0) = vec3_float.at<float>(0, 0);
  vec6_float.at<float>(4, 0) = vec3_float.at<float>(1, 0);
  vec6_float.at<float>(5, 0) = vec3_float.at<float>(2, 0);
  cedar::aux::math::expTwist(vec6_float, cedar::aux::math::pi, mat44_float);
  if (!
       (
        cedar::aux::math::isZero(mat44_float.at<float>(0, 0) - -0.85714285714285676, 1e-5)
        && cedar::aux::math::isZero(mat44_float.at<float>(0, 1) - 0.28571428571428564, 1e-5)
        && cedar::aux::math::isZero(mat44_float.at<float>(0, 2) - 0.4285714285714286, 1e-5)
        && cedar::aux::math::isZero(mat44_float.at<float>(0, 3) - 1.1428571428571423, 1e-5)
        && cedar::aux::math::isZero(mat44_float.at<float>(1, 0) - 0.28571428571428586, 1e-5)
        && cedar::aux::math::isZero(mat44_float.at<float>(1, 1) - -0.42857142857142816, 1e-5)
        && cedar::aux::math::isZero(mat44_float.at<float>(1, 2) - 0.8571428571428571, 1e-5)
        && cedar::aux::math::isZero(mat44_float.at<float>(1, 3) - 0.28571428571428537, 1e-5)
        && cedar::aux::math::isZero(mat44_float.at<float>(2, 0) - 0.42857142857142849, 1e-5)
        && cedar::aux::math::isZero(mat44_float.at<float>(2, 1) - 0.8571428571428571, 1e-5)
        && cedar::aux::math::isZero(mat44_float.at<float>(2, 2) - 0.28571428571428559, 1e-5)
        && cedar::aux::math::isZero(mat44_float.at<float>(2, 3) - -0.57142857142857117, 1e-5)
        && cedar::aux::math::isZero(mat44_float.at<float>(3, 0) - 0.0, 1e-5)
        && cedar::aux::math::isZero(mat44_float.at<float>(3, 1) - 0.0, 1e-5)
        && cedar::aux::math::isZero(mat44_float.at<float>(3, 2) - 0.0, 1e-5)
        && cedar::aux::math::isZero(mat44_float.at<float>(3, 3) - 1.0, 1e-5)
       )
     )
  {
    errors++;
    std::cout << "ERROR in function expTwist<float>(const cv::Mat& xi, float theta, cv::Mat& result)" << std::endl;
  }
  if
  (
    !cedar::aux::math::isZero
     (
       cv::norm(mat44_float - cedar::aux::math::expTwist(vec6_float, cedar::aux::math::pi)),
       1e-5
     )
  )
  {
    errors++;
    std::cout << "ERROR in function expTwist<float>(const cv::Mat& xi, float theta, cv::Mat& result)" << std::endl;
  }

  //--------------------------------------------------------------------------------------------------------------------
  // logTwist
  //--------------------------------------------------------------------------------------------------------------------
  std::cout << "test: logTwist" << std::endl;
  vec6_double *= 0;
  cedar::aux::math::logTwist(mat44_double, vec6_double, theta);
  if (!
       (
        cedar::aux::math::isZero(vec3_double.at<double>(0, 0) - 0.2672612419124244)
        && cedar::aux::math::isZero(vec3_double.at<double>(1, 0) - 0.53452248382484879)
        && cedar::aux::math::isZero(vec3_double.at<double>(2, 0) - 0.80178372573727308)
       )
     )
  {
    errors++;
    std::cout << "ERROR in function logTwist<double>(const cv::Mat& transformation, "
              << "cv::Mat& xi, double& theta, bool optionalThetaChoice = false)" << std::endl;
  }

  std::cout << "test: logTwist" << std::endl;
  vec6_float *= 0;
  cedar::aux::math::logTwist(mat44_float, vec6_float, theta);
  if (!
       (
        cedar::aux::math::isZero(vec3_float.at<float>(0, 0) - 0.2672612419124244, 1e-5)
        && cedar::aux::math::isZero(vec3_float.at<float>(1, 0) - 0.53452248382484879, 1e-5)
        && cedar::aux::math::isZero(vec3_float.at<float>(2, 0) - 0.80178372573727308, 1e-5)
       )
     )
  {
    errors++;
    std::cout << "ERROR in function logTwist<float>(const cv::Mat& transformation, "
              << "cv::Mat& xi, float& theta, bool optionalThetaChoice = false)" << std::endl;
  }

  //--------------------------------------------------------------------------------------------------------------------
  // rigidToAdjointTransformation
  //--------------------------------------------------------------------------------------------------------------------
  std::cout << "test: rigidToAdjointTransformation" << std::endl;
  cedar::aux::math::rigidToAdjointTransformation(mat44_double, mat66_double);
  if (!
       (
        cedar::aux::math::isZero(mat66_double.at<double>(0, 0) - -0.85714285714285676)
        && cedar::aux::math::isZero(mat66_double.at<double>(0, 1) - 0.28571428571428564)
        && cedar::aux::math::isZero(mat66_double.at<double>(0, 2) - 0.4285714285714286)
        && cedar::aux::math::isZero(mat66_double.at<double>(1, 0) - 0.28571428571428586)
        && cedar::aux::math::isZero(mat66_double.at<double>(1, 1) - -0.42857142857142816)
        && cedar::aux::math::isZero(mat66_double.at<double>(1, 2) - 0.8571428571428571)
        && cedar::aux::math::isZero(mat66_double.at<double>(2, 0) - 0.42857142857142849)
        && cedar::aux::math::isZero(mat66_double.at<double>(2, 1) - 0.8571428571428571)
        && cedar::aux::math::isZero(mat66_double.at<double>(2, 2) - 0.28571428571428559)

        && cedar::aux::math::isZero(mat66_double.at<double>(3, 0) - 0.0)
        && cedar::aux::math::isZero(mat66_double.at<double>(3, 1) - 0.0)
        && cedar::aux::math::isZero(mat66_double.at<double>(3, 2) - 0.0)
        && cedar::aux::math::isZero(mat66_double.at<double>(4, 0) - 0.0)
        && cedar::aux::math::isZero(mat66_double.at<double>(4, 1) - 0.0)
        && cedar::aux::math::isZero(mat66_double.at<double>(4, 2) - 0.0)
        && cedar::aux::math::isZero(mat66_double.at<double>(5, 0) - 0.0)
        && cedar::aux::math::isZero(mat66_double.at<double>(5, 1) - 0.0)
        && cedar::aux::math::isZero(mat66_double.at<double>(5, 2) - 0.0)

        && cedar::aux::math::isZero(mat66_double.at<double>(0, 3) - 0.28571428571428559)
        && cedar::aux::math::isZero(mat66_double.at<double>(0, 4) - 0.0)
        && cedar::aux::math::isZero(mat66_double.at<double>(0, 5) - 0.57142857142857106)
        && cedar::aux::math::isZero(mat66_double.at<double>(1, 3) - 0.0)
        && cedar::aux::math::isZero(mat66_double.at<double>(1, 4) - -1.1428571428571423)
        && cedar::aux::math::isZero(mat66_double.at<double>(1, 5) - -0.57142857142857106)
        && cedar::aux::math::isZero(mat66_double.at<double>(2, 3) - 0.57142857142857106)
        && cedar::aux::math::isZero(mat66_double.at<double>(2, 4) - -0.57142857142857062)
        && cedar::aux::math::isZero(mat66_double.at<double>(2, 5) - 0.85714285714285676)

        && cedar::aux::math::isZero(mat66_double.at<double>(3, 3) - -0.85714285714285676)
        && cedar::aux::math::isZero(mat66_double.at<double>(3, 4) - 0.28571428571428564)
        && cedar::aux::math::isZero(mat66_double.at<double>(3, 5) - 0.4285714285714286)
        && cedar::aux::math::isZero(mat66_double.at<double>(4, 3) - 0.28571428571428586)
        && cedar::aux::math::isZero(mat66_double.at<double>(4, 4) - -0.42857142857142816)
        && cedar::aux::math::isZero(mat66_double.at<double>(4, 5) - 0.8571428571428571)
        && cedar::aux::math::isZero(mat66_double.at<double>(5, 3) - 0.42857142857142849)
        && cedar::aux::math::isZero(mat66_double.at<double>(5, 4) - 0.8571428571428571)
        && cedar::aux::math::isZero(mat66_double.at<double>(5, 5) - 0.28571428571428559)
       )
     )
  {
    errors++;
    std::cout << "ERROR in function rigidToAdjointTransformation<double>(const cv::Mat& rigidTransformation, "
              << "cv::Mat& adjointTransformation)" << std::endl;
  }
  if (!cedar::aux::math::isZero(cv::norm(mat66_double - cedar::aux::math::rigidToAdjointTransformation(mat44_double))))
  {
    errors++;
    std::cout << "ERROR in function rigidToAdjointTransformation<double>(const cv::Mat& rigidTransformation)" << std::endl;
  }

  std::cout << "test: rigidToAdjointTransformation" << std::endl;
  cedar::aux::math::rigidToAdjointTransformation(mat44_float, mat66_float);
  if (!
       (
        cedar::aux::math::isZero(mat66_float.at<float>(0, 0) - -0.85714285714285676, 1e-5)
        && cedar::aux::math::isZero(mat66_float.at<float>(0, 1) - 0.28571428571428564, 1e-5)
        && cedar::aux::math::isZero(mat66_float.at<float>(0, 2) - 0.4285714285714286, 1e-5)
        && cedar::aux::math::isZero(mat66_float.at<float>(1, 0) - 0.28571428571428586, 1e-5)
        && cedar::aux::math::isZero(mat66_float.at<float>(1, 1) - -0.42857142857142816, 1e-5)
        && cedar::aux::math::isZero(mat66_float.at<float>(1, 2) - 0.8571428571428571, 1e-5)
        && cedar::aux::math::isZero(mat66_float.at<float>(2, 0) - 0.42857142857142849, 1e-5)
        && cedar::aux::math::isZero(mat66_float.at<float>(2, 1) - 0.8571428571428571, 1e-5)
        && cedar::aux::math::isZero(mat66_float.at<float>(2, 2) - 0.28571428571428559, 1e-5)

        && cedar::aux::math::isZero(mat66_float.at<float>(3, 0) - 0.0, 1e-5)
        && cedar::aux::math::isZero(mat66_float.at<float>(3, 1) - 0.0, 1e-5)
        && cedar::aux::math::isZero(mat66_float.at<float>(3, 2) - 0.0, 1e-5)
        && cedar::aux::math::isZero(mat66_float.at<float>(4, 0) - 0.0, 1e-5)
        && cedar::aux::math::isZero(mat66_float.at<float>(4, 1) - 0.0, 1e-5)
        && cedar::aux::math::isZero(mat66_float.at<float>(4, 2) - 0.0, 1e-5)
        && cedar::aux::math::isZero(mat66_float.at<float>(5, 0) - 0.0, 1e-5)
        && cedar::aux::math::isZero(mat66_float.at<float>(5, 1) - 0.0, 1e-5)
        && cedar::aux::math::isZero(mat66_float.at<float>(5, 2) - 0.0, 1e-5)

        && cedar::aux::math::isZero(mat66_float.at<float>(0, 3) - 0.28571428571428559, 1e-5)
        && cedar::aux::math::isZero(mat66_float.at<float>(0, 4) - 0.0, 1e-5)
        && cedar::aux::math::isZero(mat66_float.at<float>(0, 5) - 0.57142857142857106, 1e-5)
        && cedar::aux::math::isZero(mat66_float.at<float>(1, 3) - 0.0, 1e-5)
        && cedar::aux::math::isZero(mat66_float.at<float>(1, 4) - -1.1428571428571423, 1e-5)
        && cedar::aux::math::isZero(mat66_float.at<float>(1, 5) - -0.57142857142857106, 1e-5)
        && cedar::aux::math::isZero(mat66_float.at<float>(2, 3) - 0.57142857142857106, 1e-5)
        && cedar::aux::math::isZero(mat66_float.at<float>(2, 4) - -0.57142857142857062, 1e-5)
        && cedar::aux::math::isZero(mat66_float.at<float>(2, 5) - 0.85714285714285676, 1e-5)

        && cedar::aux::math::isZero(mat66_float.at<float>(3, 3) - -0.85714285714285676, 1e-5)
        && cedar::aux::math::isZero(mat66_float.at<float>(3, 4) - 0.28571428571428564, 1e-5)
        && cedar::aux::math::isZero(mat66_float.at<float>(3, 5) - 0.4285714285714286, 1e-5)
        && cedar::aux::math::isZero(mat66_float.at<float>(4, 3) - 0.28571428571428586, 1e-5)
        && cedar::aux::math::isZero(mat66_float.at<float>(4, 4) - -0.42857142857142816, 1e-5)
        && cedar::aux::math::isZero(mat66_float.at<float>(4, 5) - 0.8571428571428571, 1e-5)
        && cedar::aux::math::isZero(mat66_float.at<float>(5, 3) - 0.42857142857142849, 1e-5)
        && cedar::aux::math::isZero(mat66_float.at<float>(5, 4) - 0.8571428571428571, 1e-5)
        && cedar::aux::math::isZero(mat66_float.at<float>(5, 5) - 0.28571428571428559, 1e-5)
       )
     )
  {
    errors++;
    std::cout << "ERROR in function rigidToAdjointTransformation<float>(const cv::Mat& rigidTransformation, "
              << "cv::Mat& adjointTransformation)" << std::endl;
  }
  if
  (
    !cedar::aux::math::isZero(cv::norm(mat66_float - cedar::aux::math::rigidToAdjointTransformation(mat44_float)), 1e-5)
  )
  {
    errors++;
    std::cout << "ERROR in function rigidToAdjointTransformation<float>(const cv::Mat& rigidTransformation)" << std::endl;
  }

  //--------------------------------------------------------------------------------------------------------------------
  // adjointToRigidTransformation
  //--------------------------------------------------------------------------------------------------------------------
  std::cout << "test: adjointToRigidTransformation" << std::endl;
  cedar::aux::math::adjointToRigidTransformation(mat66_double, mat44_double);
  if (!
       (
        cedar::aux::math::isZero(mat44_double.at<double>(0, 0) - -0.85714285714285676)
        && cedar::aux::math::isZero(mat44_double.at<double>(0, 1) - 0.28571428571428564)
        && cedar::aux::math::isZero(mat44_double.at<double>(0, 2) - 0.4285714285714286)
        && cedar::aux::math::isZero(mat44_double.at<double>(0, 3) - 1.1428571428571423)
        && cedar::aux::math::isZero(mat44_double.at<double>(1, 0) - 0.28571428571428586)
        && cedar::aux::math::isZero(mat44_double.at<double>(1, 1) - -0.42857142857142816)
        && cedar::aux::math::isZero(mat44_double.at<double>(1, 2) - 0.8571428571428571)
        && cedar::aux::math::isZero(mat44_double.at<double>(1, 3) - 0.28571428571428537)
        && cedar::aux::math::isZero(mat44_double.at<double>(2, 0) - 0.42857142857142849)
        && cedar::aux::math::isZero(mat44_double.at<double>(2, 1) - 0.8571428571428571)
        && cedar::aux::math::isZero(mat44_double.at<double>(2, 2) - 0.28571428571428559)
        && cedar::aux::math::isZero(mat44_double.at<double>(2, 3) - -0.57142857142857117)
        && cedar::aux::math::isZero(mat44_double.at<double>(3, 0) - 0.0)
        && cedar::aux::math::isZero(mat44_double.at<double>(3, 1) - 0.0)
        && cedar::aux::math::isZero(mat44_double.at<double>(3, 2) - 0.0)
        && cedar::aux::math::isZero(mat44_double.at<double>(3, 3) - 1.0)
       )
     )
  {
    errors++;
    std::cout << "ERROR in function adjointToRigidTransformation<double>(const cv::Mat& adjointTransformation, "
             << "cv::Mat& rigidTransformation)" << std::endl;
  }
  if (!cedar::aux::math::isZero(cv::norm(mat44_double - cedar::aux::math::adjointToRigidTransformation(mat66_double))))
  {
    errors++;
    std::cout << "ERROR in function adjointToRigidTransformation<double>(const cv::Mat& adjointTransformation)" << std::endl;
  }

  std::cout << "test: adjointToRigidTransformation" << std::endl;
  cedar::aux::math::adjointToRigidTransformation(mat66_float, mat44_float);
  if (!
       (
        cedar::aux::math::isZero(mat44_float.at<float>(0, 0) - -0.85714285714285676, 1e-5)
        && cedar::aux::math::isZero(mat44_float.at<float>(0, 1) - 0.28571428571428564, 1e-5)
        && cedar::aux::math::isZero(mat44_float.at<float>(0, 2) - 0.4285714285714286, 1e-5)
        && cedar::aux::math::isZero(mat44_float.at<float>(0, 3) - 1.1428571428571423, 1e-5)
        && cedar::aux::math::isZero(mat44_float.at<float>(1, 0) - 0.28571428571428586, 1e-5)
        && cedar::aux::math::isZero(mat44_float.at<float>(1, 1) - -0.42857142857142816, 1e-5)
        && cedar::aux::math::isZero(mat44_float.at<float>(1, 2) - 0.8571428571428571, 1e-5)
        && cedar::aux::math::isZero(mat44_float.at<float>(1, 3) - 0.28571428571428537, 1e-5)
        && cedar::aux::math::isZero(mat44_float.at<float>(2, 0) - 0.42857142857142849, 1e-5)
        && cedar::aux::math::isZero(mat44_float.at<float>(2, 1) - 0.8571428571428571, 1e-5)
        && cedar::aux::math::isZero(mat44_float.at<float>(2, 2) - 0.28571428571428559, 1e-5)
        && cedar::aux::math::isZero(mat44_float.at<float>(2, 3) - -0.57142857142857117, 1e-5)
        && cedar::aux::math::isZero(mat44_float.at<float>(3, 0) - 0.0, 1e-5)
        && cedar::aux::math::isZero(mat44_float.at<float>(3, 1) - 0.0, 1e-5)
        && cedar::aux::math::isZero(mat44_float.at<float>(3, 2) - 0.0, 1e-5)
        && cedar::aux::math::isZero(mat44_float.at<float>(3, 3) - 1.0, 1e-5)
       )
     )
  {
    errors++;
    std::cout << "ERROR in function adjointToRigidTransformation<float>(const cv::Mat& adjointTransformation, "
              << "cv::Mat& rigidTransformation)" << std::endl;
  }
  if
  (
    !cedar::aux::math::isZero(cv::norm(mat44_float - cedar::aux::math::adjointToRigidTransformation(mat66_float)), 1e-5)
  )
  {
    errors++;
    std::cout << "ERROR in function adjointToRigidTransformation<float>(const cv::Mat& adjointTransformation)"
      << std::endl;
  }

  //--------------------------------------------------------------------------------------------------------------------
  // invertAdjointTransformation
  //--------------------------------------------------------------------------------------------------------------------
  std::cout << "test: invertAdjointTransformation" << std::endl;
  vec6_double.at<double>(0, 0) = 1;
  vec6_double.at<double>(1, 0) = 0;
  vec6_double.at<double>(2, 0) = 0;
  vec6_double.at<double>(3, 0) = 0;
  vec6_double.at<double>(4, 0) = 0;
  vec6_double.at<double>(5, 0) = 1;
  cedar::aux::math::expTwist(vec6_double, 1, mat44_double);
  cedar::aux::math::rigidToAdjointTransformation(mat44_double, mat66_double);
  cedar::aux::math::invertAdjointTransformation(mat66_double, inverse66_double);
  if (!cedar::aux::math::isZero(cv::norm(inverse66_double - mat66_double.inv())))
  {
    errors++;
    std::cout << "ERROR in function invertAdjointTransformation<double>(const cv::Mat& adjointTransformation, "
             << "cv::Mat& inverse)" << std::endl;
  }
  if (!cedar::aux::math::isZero(cv::norm(cedar::aux::math::invertAdjointTransformation(mat66_double) - mat66_double.inv())))
  {
    errors++;
    std::cout << "ERROR in function invertAdjointTransformation<double>(const cv::Mat& adjointTransformation)" << std::endl;
  }

  std::cout << "test: invertAdjointTransformation" << std::endl;
  vec6_float.at<float>(0, 0) = 1;
  vec6_float.at<float>(1, 0) = 0;
  vec6_float.at<float>(2, 0) = 0;
  vec6_float.at<float>(3, 0) = 0;
  vec6_float.at<float>(4, 0) = 0;
  vec6_float.at<float>(5, 0) = 1;
  cedar::aux::math::expTwist(vec6_float, 1, mat44_float);
  cedar::aux::math::rigidToAdjointTransformation(mat44_float, mat66_float);
  cedar::aux::math::invertAdjointTransformation(mat66_float, inverse66_float);
  if (!cedar::aux::math::isZero(cv::norm(inverse66_float - mat66_float.inv()), 1e-5))
  {
    errors++;
    std::cout << "ERROR in function invertAdjointTransformation<float>(const cv::Mat& adjointTransformation, "
              << "cv::Mat& inverse)" << std::endl;
  }
  if
  (
    !cedar::aux::math::isZero
     (
       cv::norm(cedar::aux::math::invertAdjointTransformation(mat66_float) - mat66_float.inv()),
       1e-5
     )
  )
  {
    errors++;
    std::cout << "ERROR in function invertAdjointTransformation<float>(const cv::Mat& adjointTransformation)"
      << std::endl;
  }

  //--------------------------------------------------------------------------------------------------------------------
  // twistCoordinates
  //--------------------------------------------------------------------------------------------------------------------
  std::cout << "test: twistCoordinates" << std::endl;
  cv::Mat p_double = cv::Mat::zeros(3, 1, CV_64FC1);
  p_double.at<double>(2, 0) = 1;
  cv::Mat omega_double = cv::Mat::zeros(3, 1, CV_64FC1);
  omega_double.at<double>(0, 0) = 1;
  vec6_double = cedar::aux::math::twistCoordinates(p_double, omega_double);
  if (!
       (
        vec6_double.at<double>(0, 0) == 0.0
        && vec6_double.at<double>(1, 0) == 1.0
        && vec6_double.at<double>(2, 0) == 0.0
        && vec6_double.at<double>(3, 0) == 1.0
        && vec6_double.at<double>(4, 0) == 0.0
        && vec6_double.at<double>(5, 0) == 0.0
       )
     )
  {
    errors++;
    std::cout << "ERROR in function twistCoordinates<double>(const cv::Mat& supportPoint, const cv::Mat& axis)" << std::endl;
  }

  std::cout << "test: twistCoordinates" << std::endl;
  cv::Mat p_float = cv::Mat::zeros(3, 1, CV_32FC1);
  p_float.at<float>(2, 0) = 1;
  cv::Mat omega_float = cv::Mat::zeros(3, 1, CV_32FC1);
  omega_float.at<float>(0, 0) = 1;
  vec6_float = cedar::aux::math::twistCoordinates(p_float, omega_float);
  if (!
       (
        vec6_float.at<float>(0, 0) == 0.0
        && vec6_float.at<float>(1, 0) == 1.0
        && vec6_float.at<float>(2, 0) == 0.0
        && vec6_float.at<float>(3, 0) == 1.0
        && vec6_float.at<float>(4, 0) == 0.0
        && vec6_float.at<float>(5, 0) == 0.0
       )
     )
  {
    errors++;
    std::cout << "ERROR in function twistCoordinates<float>(const cv::Mat& supportPoint, const cv::Mat& axis)" << std::endl;
  }

  std::cout << "test finished, there were " << errors << " errors" << std::endl;
  std::cout << std::endl;
  if (errors > 255)
  {
    errors = 255;
  }
  return errors;
}

