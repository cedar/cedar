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

    Maintainer:  Hendrik Reimann
    Email:       hendrik.reimann@ini.rub.de
    Date:        2010 11 11

    Description: Tests all screw calculus functions.

    Credits:

======================================================================================================================*/


// LOCAL INCLUDES

// PROJECT INCLUDES
#include "auxiliaries/math/screwCalculus.h"
#include "auxiliaries/math/tools.h"
#include "auxiliaries/LogFile.h"
#include "auxiliaries/math/constants.h"

// SYSTEM INCLUDES
#include <opencv2/opencv.hpp>

using namespace cedar::aux;
using namespace cedar::aux::math;
using namespace std;
// floats need smaller zero threshold
#ifdef EQN_EPS
#undef EQN_EPS
#endif
#define EQN_EPS 1e-5


int main()
{
  LogFile log_file("UnitTestScrewCalculus.log");
  log_file.addTimeStamp();
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
  log_file << "test: wedgeAxis" << std::endl;
  vec3_double.at<double>(0, 0) = 3;
  vec3_double.at<double>(1, 0) = 5;
  vec3_double.at<double>(2, 0) = 7;
  wedgeAxis<double>(vec3_double, mat33_double);
  if (!
       (
        IsZero(mat33_double.at<double>(0, 0) - 0.0)
        && IsZero(mat33_double.at<double>(0, 1) - -7.0)
        && IsZero(mat33_double.at<double>(0, 2) - 5.0)
        && IsZero(mat33_double.at<double>(1, 0) - 7.0)
        && IsZero(mat33_double.at<double>(1, 1) - 0.0)
        && IsZero(mat33_double.at<double>(1, 2) - -3.0)
        && IsZero(mat33_double.at<double>(2, 0) - -5.0)
        && IsZero(mat33_double.at<double>(2, 1) - 3.0)
        && IsZero(mat33_double.at<double>(2, 2) - 0.0)
       )
     )
  {
    errors++;
    log_file << "ERROR in function wedgeAxis<double>(cv::Mat& axis, cv::Mat& result)" << std::endl;
  }
  if (!IsZero(norm(mat33_double - wedgeAxis<double>(vec3_double))))
  {
    errors++;
    log_file << "ERROR in function wedgeAxis<double>(cv::Mat& axis)" << endl;
  }

  log_file << "test: wedgeAxis" << std::endl;
  vec3_float.at<float>(0, 0) = 5;
  vec3_float.at<float>(1, 0) = 55;
  vec3_float.at<float>(2, 0) = 555;
  wedgeAxis<float>(vec3_float, mat33_float);
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
    log_file << "ERROR in function wedgeAxis<float>(cv::Mat& vector, cv::Mat& result)" << std::endl;
  }
  if (!IsZero(norm(mat33_float - wedgeAxis<float>(vec3_float))))
  {
    errors++;
    log_file << "ERROR in function wedgeAxis<float>(cv::Mat& vector)" << endl;
  }

  //--------------------------------------------------------------------------------------------------------------------
  // veeAxis
  //--------------------------------------------------------------------------------------------------------------------
  log_file << "test: veeAxis" << std::endl;
  vec3_double *= 0;
  veeAxis<double>(mat33_double, vec3_double);
  if (!
       (
        vec3_double.at<double>(0, 0) == 3.0
        && vec3_double.at<double>(1, 0) == 5.0
        && vec3_double.at<double>(2, 0) == 7.0
       )
     )
  {
    errors++;
    log_file << "ERROR in function veeAxis<double>(cv::Mat& vector, cv::Mat& result)" << endl;
  }
  if (!IsZero(norm(vec3_double - veeAxis<double>(mat33_double))))
  {
    errors++;
    log_file << "ERROR in function veeAxis<double>(cv::Mat& matrix)" << endl;
  }

  log_file << "test: veeAxis" << std::endl;
  vec3_float *= 0;
  veeAxis<float>(mat33_float, vec3_float);
  if (!
       (
        vec3_float.at<float>(0, 0) == 5.0
        && vec3_float.at<float>(1, 0) == 55.0
        && vec3_float.at<float>(2, 0) == 555.0
       )
     )
  {
    errors++;
    log_file << "ERROR in function veeAxis<float>(cv::Mat& matrix, cv::Mat& result)" << endl;
  }
  if (!IsZero(norm(vec3_float - veeAxis<float>(mat33_float))))
  {
    errors++;
    log_file << "ERROR in function veeAxis<float>(cv::Mat& matrix)" << endl;
  }

  //--------------------------------------------------------------------------------------------------------------------
  // wedgeTwist
  //--------------------------------------------------------------------------------------------------------------------
  log_file << "test: wedgeTwist" << std::endl;
  vec6_double.at<double>(0, 0) = 2;
  vec6_double.at<double>(1, 0) = 3;
  vec6_double.at<double>(2, 0) = 5;
  vec6_double.at<double>(3, 0) = 7;
  vec6_double.at<double>(4, 0) = 11;
  vec6_double.at<double>(5, 0) = 13;
  wedgeTwist<double>(vec6_double, mat44_double);
  if (!
       (
        IsZero(mat44_double.at<double>(0, 0) - 0.0)
        && IsZero(mat44_double.at<double>(0, 1) - -13.0)
        && IsZero(mat44_double.at<double>(0, 2) - 11.0)
        && IsZero(mat44_double.at<double>(0, 3) - 2.0)
        && IsZero(mat44_double.at<double>(1, 0) - 13.0)
        && IsZero(mat44_double.at<double>(1, 1) - 0.0)
        && IsZero(mat44_double.at<double>(1, 2) - -7.0)
        && IsZero(mat44_double.at<double>(1, 3) - 3.0)
        && IsZero(mat44_double.at<double>(2, 0) - -11.0)
        && IsZero(mat44_double.at<double>(2, 1) - 7.0)
        && IsZero(mat44_double.at<double>(2, 2) - 0.0)
        && IsZero(mat44_double.at<double>(2, 3) - 5.0)
        && IsZero(mat44_double.at<double>(3, 0) - 0.0)
        && IsZero(mat44_double.at<double>(3, 1) - 0.0)
        && IsZero(mat44_double.at<double>(3, 2) - 0.0)
        && IsZero(mat44_double.at<double>(3, 3) - 0.0)
       )
     )
  {
    errors++;
    log_file << "ERROR in function wedgeTwist<double>(cv::Mat& twist, cv::Mat& result)" << std::endl;
  }
  if (!IsZero(norm(mat44_double - wedgeTwist<double>(vec6_double))))
  {
    errors++;
    log_file << "ERROR in function wedgeTwist<double>(cv::Mat& twist)" << endl;
  }

  log_file << "test: wedgeTwist" << std::endl;
  vec6_float.at<float>(0, 0) = 2;
  vec6_float.at<float>(1, 0) = 3;
  vec6_float.at<float>(2, 0) = 5;
  vec6_float.at<float>(3, 0) = 7;
  vec6_float.at<float>(4, 0) = 11;
  vec6_float.at<float>(5, 0) = 13;
  wedgeTwist<float>(vec6_float, mat44_float);
  if (!
       (
        IsZero(mat44_float.at<float>(0, 0) - 0.0)
        && IsZero(mat44_float.at<float>(0, 1) - -13.0)
        && IsZero(mat44_float.at<float>(0, 2) - 11.0)
        && IsZero(mat44_float.at<float>(0, 3) - 2.0)
        && IsZero(mat44_float.at<float>(1, 0) - 13.0)
        && IsZero(mat44_float.at<float>(1, 1) - 0.0)
        && IsZero(mat44_float.at<float>(1, 2) - -7.0)
        && IsZero(mat44_float.at<float>(1, 3) - 3.0)
        && IsZero(mat44_float.at<float>(2, 0) - -11.0)
        && IsZero(mat44_float.at<float>(2, 1) - 7.0)
        && IsZero(mat44_float.at<float>(2, 2) - 0.0)
        && IsZero(mat44_float.at<float>(2, 3) - 5.0)
        && IsZero(mat44_float.at<float>(3, 0) - 0.0)
        && IsZero(mat44_float.at<float>(3, 1) - 0.0)
        && IsZero(mat44_float.at<float>(3, 2) - 0.0)
        && IsZero(mat44_float.at<float>(3, 3) - 0.0)
       )
     )
  {
    errors++;
    log_file << "ERROR in function wedgeTwist<float>(cv::Mat& twist, cv::Mat& result)" << std::endl;
  }
  if (!IsZero(norm(mat44_float - wedgeTwist<float>(vec6_float))))
  {
    errors++;
    log_file << "ERROR in function wedgeTwist<float>(cv::Mat& twist)" << endl;
  }

  //--------------------------------------------------------------------------------------------------------------------
  // veeTwist
  //--------------------------------------------------------------------------------------------------------------------
  log_file << "test: veeTwist" << std::endl;
  vec6_double *= 0;
  veeTwist<double>(mat44_double, vec6_double);
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
    log_file << "ERROR in function veeTwist<double>(cv::Mat& vector, cv::Mat& result)" << endl;
  }
  if (!IsZero(norm(vec6_double - veeTwist<double>(mat44_double))))
  {
    errors++;
    log_file << "ERROR in function veeTwist<double>(cv::Mat& matrix)" << endl;
  }

  log_file << "test: veeTwist" << std::endl;
  vec6_float *= 0;
  veeTwist<float>(mat44_float, vec6_float);
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
    log_file << "ERROR in function veeTwist<float>(cv::Mat& vector, cv::Mat& result)" << endl;
  }
  if (!IsZero(norm(vec6_float - veeTwist<float>(mat44_float))))
  {
    errors++;
    log_file << "ERROR in function veeTwist<float>(cv::Mat& matrix)" << endl;
  }

  //--------------------------------------------------------------------------------------------------------------------
  // expAxis
  //--------------------------------------------------------------------------------------------------------------------
  log_file << "test: expAxis" << std::endl;
  vec3_double.at<double>(0, 0) = 1;
  vec3_double.at<double>(1, 0) = 2;
  vec3_double.at<double>(2, 0) = 3;
  vec3_double = vec3_double * (1 / norm(vec3_double));
  expAxis<double>(vec3_double, 1, mat33_double);
  if (!
       (
        IsZero(mat33_double.at<double>(0, 0) - 0.57313785544898699)
        && IsZero(mat33_double.at<double>(0, 1) - -0.60900664213739331)
        && IsZero(mat33_double.at<double>(0, 2) - 0.54829180960859991)
        && IsZero(mat33_double.at<double>(1, 0) - 0.74034884046078198)
        && IsZero(mat33_double.at<double>(1, 1) - 0.67164450419152844)
        && IsZero(mat33_double.at<double>(1, 2) - -0.027879282947946255)
        && IsZero(mat33_double.at<double>(2, 0) - -0.35127851212351696)
        && IsZero(mat33_double.at<double>(2, 1) - 0.42190587791811218)
        && IsZero(mat33_double.at<double>(2, 2) - 0.83582225209576411)
       )
     )
  {
    errors++;
    log_file << "ERROR in function expAxis<double>(const cv::Mat& matrix, double theta, cv::Mat& result)" << endl;
  }
  if (!IsZero(norm(mat33_double - expAxis<double>(vec3_double, 1))))
  {
    errors++;
    log_file << "ERROR in function expAxis<double>(const cv::Mat& matrix, double theta)" << endl;
  }

  log_file << "test: expAxis" << std::endl;
  vec3_float.at<float>(0, 0) = 1;
  vec3_float.at<float>(1, 0) = 2;
  vec3_float.at<float>(2, 0) = 3;
  vec3_float = vec3_float * (1 / norm(vec3_float));
  expAxis<float>(vec3_float, 1, mat33_float);
  if (!
       (
        IsZero(mat33_float.at<float>(0, 0) - 0.57313785544898699)
        && IsZero(mat33_float.at<float>(0, 1) - -0.60900664213739331)
        && IsZero(mat33_float.at<float>(0, 2) - 0.54829180960859991)
        && IsZero(mat33_float.at<float>(1, 0) - 0.74034884046078198)
        && IsZero(mat33_float.at<float>(1, 1) - 0.67164450419152844)
        && IsZero(mat33_float.at<float>(1, 2) - -0.027879282947946255)
        && IsZero(mat33_float.at<float>(2, 0) - -0.35127851212351696)
        && IsZero(mat33_float.at<float>(2, 1) - 0.42190587791811218)
        && IsZero(mat33_float.at<float>(2, 2) - 0.83582225209576411)
       )
     )
  {
    errors++;
    log_file << "ERROR in function expAxis<float>(const cv::Mat& matrix, float theta, cv::Mat& result)" << endl;
  }
  if (!IsZero(norm(mat33_float - expAxis<float>(vec3_float, 1))))
  {
    errors++;
    log_file << "ERROR in function expAxis<float>(const cv::Mat& matrix, double theta)" << endl;
  }

  //--------------------------------------------------------------------------------------------------------------------
  // logAxis
  //--------------------------------------------------------------------------------------------------------------------
  log_file << "test: logAxis" << std::endl;
  vec3_double *= 0;
  logAxis<double>(mat33_double, vec3_double, theta);
  if (!
       (
        IsZero(vec3_double.at<double>(0, 0) - 0.2672612419124244)
        && IsZero(vec3_double.at<double>(1, 0) - 0.53452248382484879)
        && IsZero(vec3_double.at<double>(2, 0) - 0.80178372573727308)
       )
     )
  {
    errors++;
    log_file << "ERROR in function logAxis<double>(const cv::Mat& rotation, cv::Mat& omega, "
             << "double& theta, bool optionalThetaChoice = false)" << endl;
  }

  log_file << "test: logAxis" << std::endl;
  vec3_float *= 0;
  logAxis<float>(mat33_float, vec3_float, theta);
  if (!
       (
        IsZero(vec3_float.at<float>(0, 0) - 0.2672612419124244)
        && IsZero(vec3_float.at<float>(1, 0) - 0.53452248382484879)
        && IsZero(vec3_float.at<float>(2, 0) - 0.80178372573727308)
       )
     ) 
  {
    errors++;
    log_file << "ERROR in function logAxis<float>(const cv::Mat& rotation, cv::Mat& omega, "
             << "double& theta, bool optionalThetaChoice = false)" << endl;
  }

  //--------------------------------------------------------------------------------------------------------------------
  // expTwist
  //--------------------------------------------------------------------------------------------------------------------
  log_file << "test: expTwist" << std::endl;
  vec6_double.at<double>(0, 0) = - vec3_double.at<double>(1, 0) + vec3_double.at<double>(2, 0);
  vec6_double.at<double>(1, 0) = - vec3_double.at<double>(2, 0) + vec3_double.at<double>(0, 0);
  vec6_double.at<double>(2, 0) = - vec3_double.at<double>(0, 0) + vec3_double.at<double>(1, 0);
  vec6_double.at<double>(3, 0) = vec3_double.at<double>(0, 0);
  vec6_double.at<double>(4, 0) = vec3_double.at<double>(1, 0);
  vec6_double.at<double>(5, 0) = vec3_double.at<double>(2, 0);
  expTwist<double>(vec6_double, cedar::aux::math::pi, mat44_double);
  if (!
       (
        IsZero(mat44_double.at<double>(0, 0) - -0.85714285714285676)
        && IsZero(mat44_double.at<double>(0, 1) - 0.28571428571428564)
        && IsZero(mat44_double.at<double>(0, 2) - 0.4285714285714286)
        && IsZero(mat44_double.at<double>(0, 3) - 1.1428571428571423)
        && IsZero(mat44_double.at<double>(1, 0) - 0.28571428571428586)
        && IsZero(mat44_double.at<double>(1, 1) - -0.42857142857142816)
        && IsZero(mat44_double.at<double>(1, 2) - 0.8571428571428571)
        && IsZero(mat44_double.at<double>(1, 3) - 0.28571428571428537)
        && IsZero(mat44_double.at<double>(2, 0) - 0.42857142857142849)
        && IsZero(mat44_double.at<double>(2, 1) - 0.8571428571428571)
        && IsZero(mat44_double.at<double>(2, 2) - 0.28571428571428559)
        && IsZero(mat44_double.at<double>(2, 3) - -0.57142857142857117)
        && IsZero(mat44_double.at<double>(3, 0) - 0.0)
        && IsZero(mat44_double.at<double>(3, 1) - 0.0)
        && IsZero(mat44_double.at<double>(3, 2) - 0.0)
        && IsZero(mat44_double.at<double>(3, 3) - 1.0)
       )
     )
  {
    errors++;
    log_file << "ERROR in function expTwist<double>(const cv::Mat& xi, double theta, cv::Mat& result)" << endl;
  }
  if (!IsZero(norm(mat44_double - expTwist<double>(vec6_double, cedar::aux::math::pi))))
  {
    errors++;
    log_file << "ERROR in function expTwist<double>(const cv::Mat& xi, double theta, cv::Mat& result)" << endl;
  }

  log_file << "test: expTwist" << std::endl;
  vec6_float.at<float>(0, 0) = - vec3_float.at<float>(1, 0) + vec3_float.at<float>(2, 0);
  vec6_float.at<float>(1, 0) = - vec3_float.at<float>(2, 0) + vec3_float.at<float>(0, 0);
  vec6_float.at<float>(2, 0) = - vec3_float.at<float>(0, 0) + vec3_float.at<float>(1, 0);
  vec6_float.at<float>(3, 0) = vec3_float.at<float>(0, 0);
  vec6_float.at<float>(4, 0) = vec3_float.at<float>(1, 0);
  vec6_float.at<float>(5, 0) = vec3_float.at<float>(2, 0);
  expTwist<float>(vec6_float, cedar::aux::math::pi, mat44_float);
  if (!
       (
        IsZero(mat44_float.at<float>(0, 0) - -0.85714285714285676)
        && IsZero(mat44_float.at<float>(0, 1) - 0.28571428571428564)
        && IsZero(mat44_float.at<float>(0, 2) - 0.4285714285714286)
        && IsZero(mat44_float.at<float>(0, 3) - 1.1428571428571423)
        && IsZero(mat44_float.at<float>(1, 0) - 0.28571428571428586)
        && IsZero(mat44_float.at<float>(1, 1) - -0.42857142857142816)
        && IsZero(mat44_float.at<float>(1, 2) - 0.8571428571428571)
        && IsZero(mat44_float.at<float>(1, 3) - 0.28571428571428537)
        && IsZero(mat44_float.at<float>(2, 0) - 0.42857142857142849)
        && IsZero(mat44_float.at<float>(2, 1) - 0.8571428571428571)
        && IsZero(mat44_float.at<float>(2, 2) - 0.28571428571428559)
        && IsZero(mat44_float.at<float>(2, 3) - -0.57142857142857117)
        && IsZero(mat44_float.at<float>(3, 0) - 0.0)
        && IsZero(mat44_float.at<float>(3, 1) - 0.0)
        && IsZero(mat44_float.at<float>(3, 2) - 0.0)
        && IsZero(mat44_float.at<float>(3, 3) - 1.0)
       )
     )
  {
    errors++;
    log_file << "ERROR in function expTwist<float>(const cv::Mat& xi, float theta, cv::Mat& result)" << endl;
  }
  if (!IsZero(norm(mat44_float - expTwist<float>(vec6_float, cedar::aux::math::pi))))
  {
    errors++;
    log_file << "ERROR in function expTwist<float>(const cv::Mat& xi, float theta, cv::Mat& result)" << endl;
  }

  //--------------------------------------------------------------------------------------------------------------------
  // logTwist
  //--------------------------------------------------------------------------------------------------------------------
  log_file << "test: logTwist" << std::endl;
  vec6_double *= 0;
  logTwist<double>(mat44_double, vec6_double, theta);
  if (!
       (
        IsZero(vec3_double.at<double>(0, 0) - 0.2672612419124244)
        && IsZero(vec3_double.at<double>(1, 0) - 0.53452248382484879)
        && IsZero(vec3_double.at<double>(2, 0) - 0.80178372573727308)
       )
     )
  {
    errors++;
    log_file << "ERROR in function logTwist<double>(const cv::Mat& transformation, "
             << "cv::Mat& xi, double& theta, bool optionalThetaChoice = false)" << endl;
  }

  log_file << "test: logTwist" << std::endl;
  vec6_float *= 0;
  logTwist<float>(mat44_float, vec6_float, theta);
  if (!
       (
        IsZero(vec3_float.at<float>(0, 0) - 0.2672612419124244)
        && IsZero(vec3_float.at<float>(1, 0) - 0.53452248382484879)
        && IsZero(vec3_float.at<float>(2, 0) - 0.80178372573727308)
       )
     )
  {
    errors++;
    log_file << "ERROR in function logTwist<float>(const cv::Mat& transformation, "
             << "cv::Mat& xi, float& theta, bool optionalThetaChoice = false)" << endl;
  }

  //--------------------------------------------------------------------------------------------------------------------
  // rigidToAdjointTransformation
  //--------------------------------------------------------------------------------------------------------------------
  log_file << "test: rigidToAdjointTransformation" << std::endl;
  rigidToAdjointTransformation<double>(mat44_double, mat66_double);
  if (!
       (
        IsZero(mat66_double.at<double>(0, 0) - -0.85714285714285676)
        && IsZero(mat66_double.at<double>(0, 1) - 0.28571428571428564)
        && IsZero(mat66_double.at<double>(0, 2) - 0.4285714285714286)
        && IsZero(mat66_double.at<double>(1, 0) - 0.28571428571428586)
        && IsZero(mat66_double.at<double>(1, 1) - -0.42857142857142816)
        && IsZero(mat66_double.at<double>(1, 2) - 0.8571428571428571)
        && IsZero(mat66_double.at<double>(2, 0) - 0.42857142857142849)
        && IsZero(mat66_double.at<double>(2, 1) - 0.8571428571428571)
        && IsZero(mat66_double.at<double>(2, 2) - 0.28571428571428559)

        && IsZero(mat66_double.at<double>(3, 0) - 0.0)
        && IsZero(mat66_double.at<double>(3, 1) - 0.0)
        && IsZero(mat66_double.at<double>(3, 2) - 0.0)
        && IsZero(mat66_double.at<double>(4, 0) - 0.0)
        && IsZero(mat66_double.at<double>(4, 1) - 0.0)
        && IsZero(mat66_double.at<double>(4, 2) - 0.0)
        && IsZero(mat66_double.at<double>(5, 0) - 0.0)
        && IsZero(mat66_double.at<double>(5, 1) - 0.0)
        && IsZero(mat66_double.at<double>(5, 2) - 0.0)

        && IsZero(mat66_double.at<double>(0, 3) - 0.28571428571428559)
        && IsZero(mat66_double.at<double>(0, 4) - 0.0)
        && IsZero(mat66_double.at<double>(0, 5) - 0.57142857142857106)
        && IsZero(mat66_double.at<double>(1, 3) - 0.0)
        && IsZero(mat66_double.at<double>(1, 4) - -1.1428571428571423)
        && IsZero(mat66_double.at<double>(1, 5) - -0.57142857142857106)
        && IsZero(mat66_double.at<double>(2, 3) - 0.57142857142857106)
        && IsZero(mat66_double.at<double>(2, 4) - -0.57142857142857062)
        && IsZero(mat66_double.at<double>(2, 5) - 0.85714285714285676)

        && IsZero(mat66_double.at<double>(3, 3) - -0.85714285714285676)
        && IsZero(mat66_double.at<double>(3, 4) - 0.28571428571428564)
        && IsZero(mat66_double.at<double>(3, 5) - 0.4285714285714286)
        && IsZero(mat66_double.at<double>(4, 3) - 0.28571428571428586)
        && IsZero(mat66_double.at<double>(4, 4) - -0.42857142857142816)
        && IsZero(mat66_double.at<double>(4, 5) - 0.8571428571428571)
        && IsZero(mat66_double.at<double>(5, 3) - 0.42857142857142849)
        && IsZero(mat66_double.at<double>(5, 4) - 0.8571428571428571)
        && IsZero(mat66_double.at<double>(5, 5) - 0.28571428571428559)
       )
     )
  {
    errors++;
    log_file << "ERROR in function rigidToAdjointTransformation<double>(const cv::Mat& rigidTransformation, "
             << "cv::Mat& adjointTransformation)" << endl;
  }
  if (!IsZero(norm(mat66_double - rigidToAdjointTransformation<double>(mat44_double))))
  {
    errors++;
    log_file << "ERROR in function rigidToAdjointTransformation<double>(const cv::Mat& rigidTransformation)" << endl;
  }

  log_file << "test: rigidToAdjointTransformation" << std::endl;
  rigidToAdjointTransformation<float>(mat44_float, mat66_float);
  if (!
       (
        IsZero(mat66_float.at<float>(0, 0) - -0.85714285714285676)
        && IsZero(mat66_float.at<float>(0, 1) - 0.28571428571428564)
        && IsZero(mat66_float.at<float>(0, 2) - 0.4285714285714286)
        && IsZero(mat66_float.at<float>(1, 0) - 0.28571428571428586)
        && IsZero(mat66_float.at<float>(1, 1) - -0.42857142857142816)
        && IsZero(mat66_float.at<float>(1, 2) - 0.8571428571428571)
        && IsZero(mat66_float.at<float>(2, 0) - 0.42857142857142849)
        && IsZero(mat66_float.at<float>(2, 1) - 0.8571428571428571)
        && IsZero(mat66_float.at<float>(2, 2) - 0.28571428571428559)

        && IsZero(mat66_float.at<float>(3, 0) - 0.0)
        && IsZero(mat66_float.at<float>(3, 1) - 0.0)
        && IsZero(mat66_float.at<float>(3, 2) - 0.0)
        && IsZero(mat66_float.at<float>(4, 0) - 0.0)
        && IsZero(mat66_float.at<float>(4, 1) - 0.0)
        && IsZero(mat66_float.at<float>(4, 2) - 0.0)
        && IsZero(mat66_float.at<float>(5, 0) - 0.0)
        && IsZero(mat66_float.at<float>(5, 1) - 0.0)
        && IsZero(mat66_float.at<float>(5, 2) - 0.0)

        && IsZero(mat66_float.at<float>(0, 3) - 0.28571428571428559)
        && IsZero(mat66_float.at<float>(0, 4) - 0.0)
        && IsZero(mat66_float.at<float>(0, 5) - 0.57142857142857106)
        && IsZero(mat66_float.at<float>(1, 3) - 0.0)
        && IsZero(mat66_float.at<float>(1, 4) - -1.1428571428571423)
        && IsZero(mat66_float.at<float>(1, 5) - -0.57142857142857106)
        && IsZero(mat66_float.at<float>(2, 3) - 0.57142857142857106)
        && IsZero(mat66_float.at<float>(2, 4) - -0.57142857142857062)
        && IsZero(mat66_float.at<float>(2, 5) - 0.85714285714285676)

        && IsZero(mat66_float.at<float>(3, 3) - -0.85714285714285676)
        && IsZero(mat66_float.at<float>(3, 4) - 0.28571428571428564)
        && IsZero(mat66_float.at<float>(3, 5) - 0.4285714285714286)
        && IsZero(mat66_float.at<float>(4, 3) - 0.28571428571428586)
        && IsZero(mat66_float.at<float>(4, 4) - -0.42857142857142816)
        && IsZero(mat66_float.at<float>(4, 5) - 0.8571428571428571)
        && IsZero(mat66_float.at<float>(5, 3) - 0.42857142857142849)
        && IsZero(mat66_float.at<float>(5, 4) - 0.8571428571428571)
        && IsZero(mat66_float.at<float>(5, 5) - 0.28571428571428559)
       )
     )
  {
    errors++;
    log_file << "ERROR in function rigidToAdjointTransformation<float>(const cv::Mat& rigidTransformation, "
             << "cv::Mat& adjointTransformation)" << endl;
  }
  if (!IsZero(norm(mat66_float - rigidToAdjointTransformation<float>(mat44_float))))
  {
    errors++;
    log_file << "ERROR in function rigidToAdjointTransformation<float>(const cv::Mat& rigidTransformation)" << endl;
  }

  //--------------------------------------------------------------------------------------------------------------------
  // adjointToRigidTransformation
  //--------------------------------------------------------------------------------------------------------------------
  log_file << "test: adjointToRigidTransformation" << std::endl;
  adjointToRigidTransformation<double>(mat66_double, mat44_double);
  if (!
       (
        IsZero(mat44_double.at<double>(0, 0) - -0.85714285714285676)
        && IsZero(mat44_double.at<double>(0, 1) - 0.28571428571428564)
        && IsZero(mat44_double.at<double>(0, 2) - 0.4285714285714286)
        && IsZero(mat44_double.at<double>(0, 3) - 1.1428571428571423)
        && IsZero(mat44_double.at<double>(1, 0) - 0.28571428571428586)
        && IsZero(mat44_double.at<double>(1, 1) - -0.42857142857142816)
        && IsZero(mat44_double.at<double>(1, 2) - 0.8571428571428571)
        && IsZero(mat44_double.at<double>(1, 3) - 0.28571428571428537)
        && IsZero(mat44_double.at<double>(2, 0) - 0.42857142857142849)
        && IsZero(mat44_double.at<double>(2, 1) - 0.8571428571428571)
        && IsZero(mat44_double.at<double>(2, 2) - 0.28571428571428559)
        && IsZero(mat44_double.at<double>(2, 3) - -0.57142857142857117)
        && IsZero(mat44_double.at<double>(3, 0) - 0.0)
        && IsZero(mat44_double.at<double>(3, 1) - 0.0)
        && IsZero(mat44_double.at<double>(3, 2) - 0.0)
        && IsZero(mat44_double.at<double>(3, 3) - 1.0)
       )
     )
  {
    errors++;
    log_file << "ERROR in function adjointToRigidTransformation<double>(const cv::Mat& adjointTransformation, "
             << "cv::Mat& rigidTransformation)" << endl;
  }
  if (!IsZero(norm(mat44_double - adjointToRigidTransformation<double>(mat66_double))))
  {
    errors++;
    log_file << "ERROR in function adjointToRigidTransformation<double>(const cv::Mat& adjointTransformation)" << endl;
  }

  log_file << "test: adjointToRigidTransformation" << std::endl;
  adjointToRigidTransformation<float>(mat66_float, mat44_float);
  if (!
       (
        IsZero(mat44_float.at<float>(0, 0) - -0.85714285714285676)
        && IsZero(mat44_float.at<float>(0, 1) - 0.28571428571428564)
        && IsZero(mat44_float.at<float>(0, 2) - 0.4285714285714286)
        && IsZero(mat44_float.at<float>(0, 3) - 1.1428571428571423)
        && IsZero(mat44_float.at<float>(1, 0) - 0.28571428571428586)
        && IsZero(mat44_float.at<float>(1, 1) - -0.42857142857142816)
        && IsZero(mat44_float.at<float>(1, 2) - 0.8571428571428571)
        && IsZero(mat44_float.at<float>(1, 3) - 0.28571428571428537)
        && IsZero(mat44_float.at<float>(2, 0) - 0.42857142857142849)
        && IsZero(mat44_float.at<float>(2, 1) - 0.8571428571428571)
        && IsZero(mat44_float.at<float>(2, 2) - 0.28571428571428559)
        && IsZero(mat44_float.at<float>(2, 3) - -0.57142857142857117)
        && IsZero(mat44_float.at<float>(3, 0) - 0.0)
        && IsZero(mat44_float.at<float>(3, 1) - 0.0)
        && IsZero(mat44_float.at<float>(3, 2) - 0.0)
        && IsZero(mat44_float.at<float>(3, 3) - 1.0)
       )
     )
  {
    errors++;
    log_file << "ERROR in function adjointToRigidTransformation<float>(const cv::Mat& adjointTransformation, "
             << "cv::Mat& rigidTransformation)" << endl;
  }
  if (!IsZero(norm(mat44_float - adjointToRigidTransformation<float>(mat66_float))))
  {
    errors++;
    log_file << "ERROR in function adjointToRigidTransformation<float>(const cv::Mat& adjointTransformation)" << endl;
  }

  //--------------------------------------------------------------------------------------------------------------------
  // invertAdjointTransformation
  //--------------------------------------------------------------------------------------------------------------------
  log_file << "test: invertAdjointTransformation" << std::endl;
  vec6_double.at<double>(0, 0) = 1;
  vec6_double.at<double>(1, 0) = 0;
  vec6_double.at<double>(2, 0) = 0;
  vec6_double.at<double>(3, 0) = 0;
  vec6_double.at<double>(4, 0) = 0;
  vec6_double.at<double>(5, 0) = 1;
  expTwist<double>(vec6_double, 1, mat44_double);
  rigidToAdjointTransformation<double>(mat44_double, mat66_double);
  invertAdjointTransformation<double>(mat66_double, inverse66_double);
  if (!IsZero(norm(inverse66_double - mat66_double.inv())))
  {
    errors++;
    log_file << "ERROR in function invertAdjointTransformation<double>(const cv::Mat& adjointTransformation, "
             << "cv::Mat& inverse)" << endl;
  }
  if (!IsZero(norm(invertAdjointTransformation<double>(mat66_double) - mat66_double.inv())))
  {
    errors++;
    log_file << "ERROR in function invertAdjointTransformation<double>(const cv::Mat& adjointTransformation)" << endl;
  }

  log_file << "test: invertAdjointTransformation" << std::endl;
  vec6_float.at<float>(0, 0) = 1;
  vec6_float.at<float>(1, 0) = 0;
  vec6_float.at<float>(2, 0) = 0;
  vec6_float.at<float>(3, 0) = 0;
  vec6_float.at<float>(4, 0) = 0;
  vec6_float.at<float>(5, 0) = 1;
  expTwist<float>(vec6_float, 1, mat44_float);
  rigidToAdjointTransformation<float>(mat44_float, mat66_float);
  invertAdjointTransformation<float>(mat66_float, inverse66_float);
  if (!IsZero(norm(inverse66_float - mat66_float.inv())))
  {
    errors++;
    log_file << "ERROR in function invertAdjointTransformation<float>(const cv::Mat& adjointTransformation, "
             << "cv::Mat& inverse)" << endl;
  }
  if (!IsZero(norm(invertAdjointTransformation<float>(mat66_float) - mat66_float.inv())))
  {
    errors++;
    log_file << "ERROR in function invertAdjointTransformation<float>(const cv::Mat& adjointTransformation)" << endl;
  }

  //--------------------------------------------------------------------------------------------------------------------
  // twistCoordinates
  //--------------------------------------------------------------------------------------------------------------------
  log_file << "test: twistCoordinates" << std::endl;
  cv::Mat p_double = cv::Mat::zeros(3, 1, CV_64FC1);
  p_double.at<double>(2, 0) = 1;
  cv::Mat omega_double = cv::Mat::zeros(3, 1, CV_64FC1);
  omega_double.at<double>(0, 0) = 1;
  vec6_double = twistCoordinates<double>(p_double, omega_double);
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
    log_file << "ERROR in function twistCoordinates<double>(const cv::Mat& supportPoint, const cv::Mat& axis)" << endl;
  }

  log_file << "test: twistCoordinates" << std::endl;
  cv::Mat p_float = cv::Mat::zeros(3, 1, CV_32FC1);
  p_float.at<float>(2, 0) = 1;
  cv::Mat omega_float = cv::Mat::zeros(3, 1, CV_32FC1);
  omega_float.at<float>(0, 0) = 1;
  vec6_float = twistCoordinates<float>(p_float, omega_float);
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
    log_file << "ERROR in function twistCoordinates<float>(const cv::Mat& supportPoint, const cv::Mat& axis)" << endl;
  }





  log_file << "test finished, there were " << errors << " errors" << std::endl;
  log_file << std::endl;
  if (errors > 255)
  {
    errors = 255;
  }
  return errors;
}

