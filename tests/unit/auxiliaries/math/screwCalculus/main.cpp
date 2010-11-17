/*----------------------------------------------------------------------------------------------------------------------
 ----- Institute:   Ruhr-Universitaet Bochum
                    Institut fuer Neuroinformatik

 ----- File:        main.cpp

 ----- Author:      Hendrik Reimann
 ----- Email:       hendrik.reimann@ini.rub.de
 ----- Date:        2010 11 12

 ----- Description: Tests all screw calculus functions

 ----- Credits:
 ---------------------------------------------------------------------------------------------------------------------*/

// LOCAL INCLUDES

// PROJECT INCLUDES
#include "src/auxiliaries/math/screwCalculus.h"
#include "src/auxiliaries/math/tools.h"
#include "src/auxiliaries/LogFile.h"

// SYSTEM INCLUDES
#include <cv.h>

using namespace cedar::aux;
using namespace cedar::aux::math;
using namespace std;
#ifdef EQN_EPS
#undef EQN_EPS
#endif
#define EQN_EPS 1e-5


int main()
{
  LogFile log_file("UnitTestScrewCalculus.log", ".");
  log_file << std::endl;
  log_file.addTimeStamp();
  log_file << std::endl;
  // the number of errors encountered in this test
  int errors = 0;
  double theta;
  cv::Mat vec3double(3, 1, CV_64FC1);
  cv::Mat mat33double(3, 3, CV_64FC1);
  cv::Mat vec6double(6, 1, CV_64FC1);
  cv::Mat mat44double(4, 4, CV_64FC1);
  cv::Mat mat66double(6, 6, CV_64FC1);
  cv::Mat inverse66double(6, 6, CV_64FC1);
  cv::Mat vec3float(3, 1, CV_32FC1);
  cv::Mat mat33float(3, 3, CV_32FC1);
  cv::Mat vec6float(6, 1, CV_32FC1);
  cv::Mat mat44float(4, 4, CV_32FC1);
  cv::Mat mat66float(6, 6, CV_32FC1);
  cv::Mat inverse66float(6, 6, CV_32FC1);

  /* ------------------------------------------------------------------------------------------------- */
  log_file << "test: wedgeAxis<double>" << std::endl;
  vec3double.at<double>(0, 0) = 3;
  vec3double.at<double>(1, 0) = 5;
  vec3double.at<double>(2, 0) = 7;
  wedgeAxis<double>(vec3double, mat33double);
//  log_file << mat33double.at<double>(0, 0) << " " << mat33double.at<double>(0, 1) << " " << mat33double.at<double>(0, 2) << endl;
//  log_file << mat33double.at<double>(1, 0) << " " << mat33double.at<double>(1, 1) << " " << mat33double.at<double>(1, 2) << endl;
//  log_file << mat33double.at<double>(2, 0) << " " << mat33double.at<double>(2, 1) << " " << mat33double.at<double>(2, 2) << endl;
  if (
       IsZero(mat33double.at<double>(0, 0) - 0.0)
       && IsZero(mat33double.at<double>(0, 1) - -7.0)
       && IsZero(mat33double.at<double>(0, 2) - 5.0)
       && IsZero(mat33double.at<double>(1, 0) - 7.0)
       && IsZero(mat33double.at<double>(1, 1) - 0.0)
       && IsZero(mat33double.at<double>(1, 2) - -3.0)
       && IsZero(mat33double.at<double>(2, 0) - -5.0)
       && IsZero(mat33double.at<double>(2, 1) - 3.0)
       && IsZero(mat33double.at<double>(2, 2) - 0.0)
     )
  {
    ;
  }
  else
  {
    errors++;
    log_file << "wedgeAxis<double>(cv::Mat& axis, cv::Mat& result) is NOT working correctly!" << std::endl;
  }
  if (IsZero(norm(mat33double - wedgeAxis<double>(vec3double))))
  {
    ;
  }
  else
  {
    errors++;
    log_file << "wedgeAxis<double>(cv::Mat& axis) is NOT working correctly!" << endl;
  }

  log_file << "test: wedgeAxis<float>" << std::endl;
  vec3float.at<float>(0, 0) = 5;
  vec3float.at<float>(1, 0) = 55;
  vec3float.at<float>(2, 0) = 555;
  wedgeAxis<float>(vec3float, mat33float);
  if (
       mat33float.at<float>(0, 0) == 0.0
       && mat33float.at<float>(0, 1) == -555.0
       && mat33float.at<float>(0, 2) == 55.0
       && mat33float.at<float>(1, 0) == 555.0
       && mat33float.at<float>(1, 1) == 0.0
       && mat33float.at<float>(1, 2) == -5.0
       && mat33float.at<float>(2, 0) == -55.0
       && mat33float.at<float>(2, 1) == 5.0
       && mat33float.at<float>(2, 2) == 0.0
     )
  {
    ;
  }
  else
  {
    errors++;
    log_file << "wedgeAxis<float>(cv::Mat& vector, cv::Mat& result) is NOT working correctly!" << std::endl;
  }
  if (IsZero(norm(mat33float - wedgeAxis<float>(vec3float))))
  {
    ;
  }
  else
  {
    errors++;
    log_file << "wedgeAxis<float>(cv::Mat& vector) is NOT working correctly!" << endl;
  }

  /* ------------------------------------------------------------------------------------------------- */
  log_file << "test: veeAxis<double>" << std::endl;
  vec3double *= 0;
  veeAxis<double>(mat33double, vec3double);
  if (
       vec3double.at<double>(0, 0) == 3.0
       && vec3double.at<double>(1, 0) == 5.0
       && vec3double.at<double>(2, 0) == 7.0
     )
  {
    ;
  }
  else
  {
    errors++;
    log_file << "veeAxis<double>(cv::Mat& vector, cv::Mat& result) is NOT working correctly!" << endl;
  }
  if (IsZero(norm(vec3double - veeAxis<double>(mat33double))))
  {
    ;
  }
  else
  {
    errors++;
    log_file << "veeAxis<double>(cv::Mat& matrix) is NOT working correctly!" << endl;
  }

  log_file << "test: veeAxis<float>" << std::endl;
  vec3float *= 0;
  veeAxis<float>(mat33float, vec3float);
  if (
       vec3float.at<float>(0, 0) == 5.0
       && vec3float.at<float>(1, 0) == 55.0
       && vec3float.at<float>(2, 0) == 555.0
     )
  {
    ;
  }
  else
  {
    errors++;
    log_file << "veeAxis<float>(cv::Mat& matrix, cv::Mat& result) is NOT working correctly!" << endl;
  }
  if (IsZero(norm(vec3float - veeAxis<float>(mat33float))))
  {
    ;
  }
  else
  {
    errors++;
    log_file << "veeAxis<float>(cv::Mat& matrix) is NOT working correctly!" << endl;
  }

  /* ------------------------------------------------------------------------------------------------- */
  log_file << "test: wedgeTwist<double>" << std::endl;
  vec6double.at<double>(0, 0) = 2;
  vec6double.at<double>(1, 0) = 3;
  vec6double.at<double>(2, 0) = 5;
  vec6double.at<double>(3, 0) = 7;
  vec6double.at<double>(4, 0) = 11;
  vec6double.at<double>(5, 0) = 13;
  wedgeTwist<double>(vec6double, mat44double);
//  log_file << mat44double.at<double>(0, 0) << " " << mat44double.at<double>(0, 1) << " " << mat44double.at<double>(0, 2) << " " << mat44double.at<double>(0, 3) << endl;
//  log_file << mat44double.at<double>(1, 0) << " " << mat44double.at<double>(1, 1) << " " << mat44double.at<double>(1, 2) << " " << mat44double.at<double>(1, 3) << endl;
//  log_file << mat44double.at<double>(2, 0) << " " << mat44double.at<double>(2, 1) << " " << mat44double.at<double>(2, 2) << " " << mat44double.at<double>(2, 3) << endl;
//  log_file << mat44double.at<double>(3, 0) << " " << mat44double.at<double>(3, 1) << " " << mat44double.at<double>(3, 2) << " " << mat44double.at<double>(3, 3) << endl;
  if (
       IsZero(mat44double.at<double>(0, 0) - 0.0)
       && IsZero(mat44double.at<double>(0, 1) - -13.0)
       && IsZero(mat44double.at<double>(0, 2) - 11.0)
       && IsZero(mat44double.at<double>(0, 3) - 2.0)
       && IsZero(mat44double.at<double>(1, 0) - 13.0)
       && IsZero(mat44double.at<double>(1, 1) - 0.0)
       && IsZero(mat44double.at<double>(1, 2) - -7.0)
       && IsZero(mat44double.at<double>(1, 3) - 3.0)
       && IsZero(mat44double.at<double>(2, 0) - -11.0)
       && IsZero(mat44double.at<double>(2, 1) - 7.0)
       && IsZero(mat44double.at<double>(2, 2) - 0.0)
       && IsZero(mat44double.at<double>(2, 3) - 5.0)
       && IsZero(mat44double.at<double>(3, 0) - 0.0)
       && IsZero(mat44double.at<double>(3, 1) - 0.0)
       && IsZero(mat44double.at<double>(3, 2) - 0.0)
       && IsZero(mat44double.at<double>(3, 3) - 0.0)
     )
  {
    ;
  }
  else
  {
    errors++;
    log_file << "wedgeTwist<double>(cv::Mat& twist, cv::Mat& result) is NOT working correctly!" << std::endl;
  }
  if (IsZero(norm(mat44double - wedgeTwist<double>(vec6double))))
  {
    ;
  }
  else
  {
    errors++;
    log_file << "wedgeTwist<double>(cv::Mat& twist) is NOT working correctly!" << endl;
  }

  log_file << "test: wedgeTwist<float>" << std::endl;
  vec6float.at<float>(0, 0) = 2;
  vec6float.at<float>(1, 0) = 3;
  vec6float.at<float>(2, 0) = 5;
  vec6float.at<float>(3, 0) = 7;
  vec6float.at<float>(4, 0) = 11;
  vec6float.at<float>(5, 0) = 13;
  wedgeTwist<float>(vec6float, mat44float);
  if (
        IsZero(mat44float.at<float>(0, 0) - 0.0)
        && IsZero(mat44float.at<float>(0, 1) - -13.0)
        && IsZero(mat44float.at<float>(0, 2) - 11.0)
        && IsZero(mat44float.at<float>(0, 3) - 2.0)
        && IsZero(mat44float.at<float>(1, 0) - 13.0)
        && IsZero(mat44float.at<float>(1, 1) - 0.0)
        && IsZero(mat44float.at<float>(1, 2) - -7.0)
        && IsZero(mat44float.at<float>(1, 3) - 3.0)
        && IsZero(mat44float.at<float>(2, 0) - -11.0)
        && IsZero(mat44float.at<float>(2, 1) - 7.0)
        && IsZero(mat44float.at<float>(2, 2) - 0.0)
        && IsZero(mat44float.at<float>(2, 3) - 5.0)
        && IsZero(mat44float.at<float>(3, 0) - 0.0)
        && IsZero(mat44float.at<float>(3, 1) - 0.0)
        && IsZero(mat44float.at<float>(3, 2) - 0.0)
        && IsZero(mat44float.at<float>(3, 3) - 0.0)
      )
  {
    ;
  }
  else
  {
    errors++;
    log_file << "wedgeTwist<float>(cv::Mat& twist, cv::Mat& result) is NOT working correctly!" << std::endl;
  }
  if (IsZero(norm(mat44float - wedgeTwist<float>(vec6float))))
  {
    ;
  }
  else
  {
    errors++;
    log_file << "wedgeTwist<float>(cv::Mat& twist) is NOT working correctly!" << endl;
  }

  /* ------------------------------------------------------------------------------------------------- */
  log_file << "test: veeTwist<double>" << std::endl;
  vec6double *= 0;
  veeTwist<double>(mat44double, vec6double);
  if (
       vec6double.at<double>(0, 0) == 2.0
       && vec6double.at<double>(1, 0) == 3.0
       && vec6double.at<double>(2, 0) == 5.0
       && vec6double.at<double>(3, 0) == 7.0
       && vec6double.at<double>(4, 0) == 11.0
       && vec6double.at<double>(5, 0) == 13.0
     )
  {
    ;
  }
  else
  {
    errors++;
    log_file << "veeTwist<double>(cv::Mat& vector, cv::Mat& result) is NOT working correctly!" << endl;
  }
  if (IsZero(norm(vec6double - veeTwist<double>(mat44double))))
  {
    ;
  }
  else
  {
    errors++;
    log_file << "veeTwist<double>(cv::Mat& matrix) is NOT working correctly!" << endl;
  }

  log_file << "test: veeTwist<float>" << std::endl;
  vec6float *= 0;
  veeTwist<float>(mat44float, vec6float);
  if (
       vec6float.at<float>(0, 0) == 2.0
       && vec6float.at<float>(1, 0) == 3.0
       && vec6float.at<float>(2, 0) == 5.0
       && vec6float.at<float>(3, 0) == 7.0
       && vec6float.at<float>(4, 0) == 11.0
       && vec6float.at<float>(5, 0) == 13.0
     )
  {
    ;
  }
  else
  {
    errors++;
    log_file << "veeTwist<float>(cv::Mat& vector, cv::Mat& result) is NOT working correctly!" << endl;
  }
  if (IsZero(norm(vec6float - veeTwist<float>(mat44float))))
  {
    ;
  }
  else
  {
    errors++;
    log_file << "veeTwist<float>(cv::Mat& matrix) is NOT working correctly!" << endl;
  }

  /* ------------------------------------------------------------------------------------------------- */
  log_file << "test: expAxis<double>" << std::endl;
  vec3double.at<double>(0, 0) = 1;
  vec3double.at<double>(1, 0) = 2;
  vec3double.at<double>(2, 0) = 3;
  vec3double = vec3double * (1 / norm(vec3double));
  expAxis<double>(vec3double, 1, mat33double);
  if ( IsZero(mat33double.at<double>(0, 0) - 0.57313785544898699)
       && IsZero(mat33double.at<double>(0, 1) - -0.60900664213739331)
       && IsZero(mat33double.at<double>(0, 2) - 0.54829180960859991)
       && IsZero(mat33double.at<double>(1, 0) - 0.74034884046078198)
       && IsZero(mat33double.at<double>(1, 1) - 0.67164450419152844)
       && IsZero(mat33double.at<double>(1, 2) - -0.027879282947946255)
       && IsZero(mat33double.at<double>(2, 0) - -0.35127851212351696)
       && IsZero(mat33double.at<double>(2, 1) - 0.42190587791811218)
       && IsZero(mat33double.at<double>(2, 2) - 0.83582225209576411)
     )
  {
    ;
  }
  else
  {
    errors++;
    log_file << "expAxis<double>(const cv::Mat& matrix, double theta, cv::Mat& result) is NOT working correctly!" << endl;
  }
  if (IsZero(norm(mat33double - expAxis<double>(vec3double, 1))))
  {
    ;
  }
  else
  {
    errors++;
    log_file << "expAxis<double>(const cv::Mat& matrix, double theta) is NOT working correctly!" << endl;
  }

  log_file << "test: expAxis<float>" << std::endl;
  vec3float.at<float>(0, 0) = 1;
  vec3float.at<float>(1, 0) = 2;
  vec3float.at<float>(2, 0) = 3;
  vec3float = vec3float * (1 / norm(vec3float));
  expAxis<float>(vec3float, 1, mat33float);
  if ( IsZero(mat33float.at<float>(0, 0) - 0.57313785544898699)
       && IsZero(mat33float.at<float>(0, 1) - -0.60900664213739331)
       && IsZero(mat33float.at<float>(0, 2) - 0.54829180960859991)
       && IsZero(mat33float.at<float>(1, 0) - 0.74034884046078198)
       && IsZero(mat33float.at<float>(1, 1) - 0.67164450419152844)
       && IsZero(mat33float.at<float>(1, 2) - -0.027879282947946255)
       && IsZero(mat33float.at<float>(2, 0) - -0.35127851212351696)
       && IsZero(mat33float.at<float>(2, 1) - 0.42190587791811218)
       && IsZero(mat33float.at<float>(2, 2) - 0.83582225209576411)
     )
  {
    ;
  }
  else
  {
    errors++;
    log_file << "expAxis<float>(const cv::Mat& matrix, float theta, cv::Mat& result) is NOT working correctly!" << endl;
  }
  if (IsZero(norm(mat33float - expAxis<float>(vec3float, 1))))
  {
    ;
  }
  else
  {
    errors++;
    log_file << "expAxis<float>(const cv::Mat& matrix, double theta) is NOT working correctly!" << endl;
  }

  /* ------------------------------------------------------------------------------------------------- */
  log_file << "test: logAxis<double>" << std::endl;
  vec3double *= 0;
  logAxis<double>(mat33double, vec3double, theta);
  if (
       IsZero(vec3double.at<double>(0, 0) - 0.2672612419124244)
       && IsZero(vec3double.at<double>(1, 0) - 0.53452248382484879)
       && IsZero(vec3double.at<double>(2, 0) - 0.80178372573727308)
     )
  {
    ;
  }
  else
  {
    errors++;
    log_file << "logAxis<double>(const cv::Mat& rotation, cv::Mat& omega, double& theta, bool optionalThetaChoice = false) is NOT working correctly!" << endl;
  }

  log_file << "test: logAxis<float>" << std::endl;
  vec3float *= 0;
  logAxis<float>(mat33float, vec3float, theta);
  if (
       IsZero(vec3float.at<float>(0, 0) - 0.2672612419124244)
       && IsZero(vec3float.at<float>(1, 0) - 0.53452248382484879)
       && IsZero(vec3float.at<float>(2, 0) - 0.80178372573727308)
     )
  {
    ;
  }
  else
  {
    errors++;
    log_file << "logAxis<float>(const cv::Mat& rotation, cv::Mat& omega, double& theta, bool optionalThetaChoice = false) is NOT working correctly!" << endl;
  }

  /* ------------------------------------------------------------------------------------------------- */
  log_file << "test: expTwist<double>" << std::endl;
  vec6double.at<double>(0, 0) = - vec3double.at<double>(1, 0) + vec3double.at<double>(2, 0);
  vec6double.at<double>(1, 0) = - vec3double.at<double>(2, 0) + vec3double.at<double>(0, 0);
  vec6double.at<double>(2, 0) = - vec3double.at<double>(0, 0) + vec3double.at<double>(1, 0);
  vec6double.at<double>(3, 0) = vec3double.at<double>(0, 0);
  vec6double.at<double>(4, 0) = vec3double.at<double>(1, 0);
  vec6double.at<double>(5, 0) = vec3double.at<double>(2, 0);
  expTwist<double>(vec6double, M_PI, mat44double);
  if (
       IsZero(mat44double.at<double>(0, 0) - -0.85714285714285676)
       && IsZero(mat44double.at<double>(0, 1) - 0.28571428571428564)
       && IsZero(mat44double.at<double>(0, 2) - 0.4285714285714286)
       && IsZero(mat44double.at<double>(0, 3) - 1.1428571428571423)
       && IsZero(mat44double.at<double>(1, 0) - 0.28571428571428586)
       && IsZero(mat44double.at<double>(1, 1) - -0.42857142857142816)
       && IsZero(mat44double.at<double>(1, 2) - 0.8571428571428571)
       && IsZero(mat44double.at<double>(1, 3) - 0.28571428571428537)
       && IsZero(mat44double.at<double>(2, 0) - 0.42857142857142849)
       && IsZero(mat44double.at<double>(2, 1) - 0.8571428571428571)
       && IsZero(mat44double.at<double>(2, 2) - 0.28571428571428559)
       && IsZero(mat44double.at<double>(2, 3) - -0.57142857142857117)
       && IsZero(mat44double.at<double>(3, 0) - 0.0)
       && IsZero(mat44double.at<double>(3, 1) - 0.0)
       && IsZero(mat44double.at<double>(3, 2) - 0.0)
       && IsZero(mat44double.at<double>(3, 3) - 1.0)
     )
  {
    ;
  }
  else
  {
    errors++;
    log_file << "expTwist<double>(const cv::Mat& xi, double theta, cv::Mat& result) is NOT working correctly!" << endl;
  }
  if (IsZero(norm(mat44double - expTwist<double>(vec6double, M_PI))))
  {
    ;
  }
  else
  {
    errors++;
    log_file << "expTwist<double>(const cv::Mat& xi, double theta, cv::Mat& result) is NOT working correctly!" << endl;
  }

  log_file << "test: expTwist<float>" << std::endl;
  vec6float.at<float>(0, 0) = - vec3float.at<float>(1, 0) + vec3float.at<float>(2, 0);
  vec6float.at<float>(1, 0) = - vec3float.at<float>(2, 0) + vec3float.at<float>(0, 0);
  vec6float.at<float>(2, 0) = - vec3float.at<float>(0, 0) + vec3float.at<float>(1, 0);
  vec6float.at<float>(3, 0) = vec3float.at<float>(0, 0);
  vec6float.at<float>(4, 0) = vec3float.at<float>(1, 0);
  vec6float.at<float>(5, 0) = vec3float.at<float>(2, 0);
  expTwist<float>(vec6float, M_PI, mat44float);
  if (
       IsZero(mat44float.at<float>(0, 0) - -0.85714285714285676)
       && IsZero(mat44float.at<float>(0, 1) - 0.28571428571428564)
       && IsZero(mat44float.at<float>(0, 2) - 0.4285714285714286)
       && IsZero(mat44float.at<float>(0, 3) - 1.1428571428571423)
       && IsZero(mat44float.at<float>(1, 0) - 0.28571428571428586)
       && IsZero(mat44float.at<float>(1, 1) - -0.42857142857142816)
       && IsZero(mat44float.at<float>(1, 2) - 0.8571428571428571)
       && IsZero(mat44float.at<float>(1, 3) - 0.28571428571428537)
       && IsZero(mat44float.at<float>(2, 0) - 0.42857142857142849)
       && IsZero(mat44float.at<float>(2, 1) - 0.8571428571428571)
       && IsZero(mat44float.at<float>(2, 2) - 0.28571428571428559)
       && IsZero(mat44float.at<float>(2, 3) - -0.57142857142857117)
       && IsZero(mat44float.at<float>(3, 0) - 0.0)
       && IsZero(mat44float.at<float>(3, 1) - 0.0)
       && IsZero(mat44float.at<float>(3, 2) - 0.0)
       && IsZero(mat44float.at<float>(3, 3) - 1.0)
     )
  {
    ;
  }
  else
  {
    errors++;
    log_file << "expTwist<float>(const cv::Mat& xi, float theta, cv::Mat& result) is NOT working correctly!" << endl;
  }
  if (IsZero(norm(mat44float - expTwist<float>(vec6float, M_PI))))
  {
    ;
  }
  else
  {
    errors++;
    log_file << "expTwist<float>(const cv::Mat& xi, float theta, cv::Mat& result) is NOT working correctly!" << endl;
  }

  /* ------------------------------------------------------------------------------------------------- */
  log_file << "test: logTwist<double>" << std::endl;
  vec6double *= 0;
  logTwist<double>(mat44double, vec6double, theta);
  if (
      IsZero(vec3double.at<double>(0, 0) - 0.2672612419124244)
       && IsZero(vec3double.at<double>(1, 0) - 0.53452248382484879)
       && IsZero(vec3double.at<double>(2, 0) - 0.80178372573727308)
     )
  {
    ;
  }
  else
  {
    errors++;
    log_file << "logTwist<double>(const cv::Mat& transformation, cv::Mat& xi, double& theta, bool optionalThetaChoice = false) is NOT working correctly!" << endl;
  }

  log_file << "test: logTwist<float>" << std::endl;
  vec6float *= 0;
  logTwist<float>(mat44float, vec6float, theta);
  if (
      IsZero(vec3float.at<float>(0, 0) - 0.2672612419124244)
       && IsZero(vec3float.at<float>(1, 0) - 0.53452248382484879)
       && IsZero(vec3float.at<float>(2, 0) - 0.80178372573727308)
     )
  {
    ;
  }
  else
  {
    errors++;
    log_file << "logTwist<float>(const cv::Mat& transformation, cv::Mat& xi, float& theta, bool optionalThetaChoice = false) is NOT working correctly!" << endl;
  }

  /* ------------------------------------------------------------------------------------------------- */
  log_file << "test: rigidToAdjointTransformation<double>" << std::endl;
  rigidToAdjointTransformation<double>(mat44double, mat66double);
  if (
       IsZero(mat66double.at<double>(0, 0) - -0.85714285714285676)
       && IsZero(mat66double.at<double>(0, 1) - 0.28571428571428564)
       && IsZero(mat66double.at<double>(0, 2) - 0.4285714285714286)
       && IsZero(mat66double.at<double>(1, 0) - 0.28571428571428586)
       && IsZero(mat66double.at<double>(1, 1) - -0.42857142857142816)
       && IsZero(mat66double.at<double>(1, 2) - 0.8571428571428571)
       && IsZero(mat66double.at<double>(2, 0) - 0.42857142857142849)
       && IsZero(mat66double.at<double>(2, 1) - 0.8571428571428571)
       && IsZero(mat66double.at<double>(2, 2) - 0.28571428571428559)

       && IsZero(mat66double.at<double>(3, 0) - 0.0)
       && IsZero(mat66double.at<double>(3, 1) - 0.0)
       && IsZero(mat66double.at<double>(3, 2) - 0.0)
       && IsZero(mat66double.at<double>(4, 0) - 0.0)
       && IsZero(mat66double.at<double>(4, 1) - 0.0)
       && IsZero(mat66double.at<double>(4, 2) - 0.0)
       && IsZero(mat66double.at<double>(5, 0) - 0.0)
       && IsZero(mat66double.at<double>(5, 1) - 0.0)
       && IsZero(mat66double.at<double>(5, 2) - 0.0)

       && IsZero(mat66double.at<double>(0, 3) - 0.28571428571428559)
       && IsZero(mat66double.at<double>(0, 4) - 0.0)
       && IsZero(mat66double.at<double>(0, 5) - 0.57142857142857106)
       && IsZero(mat66double.at<double>(1, 3) - 0.0)
       && IsZero(mat66double.at<double>(1, 4) - -1.1428571428571423)
       && IsZero(mat66double.at<double>(1, 5) - -0.57142857142857106)
       && IsZero(mat66double.at<double>(2, 3) - 0.57142857142857106)
       && IsZero(mat66double.at<double>(2, 4) - -0.57142857142857062)
       && IsZero(mat66double.at<double>(2, 5) - 0.85714285714285676)

       && IsZero(mat66double.at<double>(3, 3) - -0.85714285714285676)
       && IsZero(mat66double.at<double>(3, 4) - 0.28571428571428564)
       && IsZero(mat66double.at<double>(3, 5) - 0.4285714285714286)
       && IsZero(mat66double.at<double>(4, 3) - 0.28571428571428586)
       && IsZero(mat66double.at<double>(4, 4) - -0.42857142857142816)
       && IsZero(mat66double.at<double>(4, 5) - 0.8571428571428571)
       && IsZero(mat66double.at<double>(5, 3) - 0.42857142857142849)
       && IsZero(mat66double.at<double>(5, 4) - 0.8571428571428571)
       && IsZero(mat66double.at<double>(5, 5) - 0.28571428571428559)
     )
  {
    ;
  }
  else
  {
    errors++;
    log_file << "rigidToAdjointTransformation<double>(const cv::Mat& rigidTransformation, cv::Mat& adjointTransformation) is NOT working correctly!" << endl;
  }
//  for (int i=0; i<6; i++)
//  {
//    for (int j=0; j<6; j++)
//    {
//      log_file << mat66double.at<double>(i, j) << " ";
//      log_file << rigidToAdjointTransformation<double>(mat44double).at<double>(i, j) << endl;
//    }
//  }
  if (IsZero(norm(mat66double - rigidToAdjointTransformation<double>(mat44double))))
  {
    ;
  }
  else
  {
    errors++;
    log_file << "rigidToAdjointTransformation<double>(const cv::Mat& rigidTransformation) is NOT working correctly!" << endl;
  }

  log_file << "test: rigidToAdjointTransformation<float>" << std::endl;
  rigidToAdjointTransformation<float>(mat44float, mat66float);
  if (
       IsZero(mat66float.at<float>(0, 0) - -0.85714285714285676)
       && IsZero(mat66float.at<float>(0, 1) - 0.28571428571428564)
       && IsZero(mat66float.at<float>(0, 2) - 0.4285714285714286)
       && IsZero(mat66float.at<float>(1, 0) - 0.28571428571428586)
       && IsZero(mat66float.at<float>(1, 1) - -0.42857142857142816)
       && IsZero(mat66float.at<float>(1, 2) - 0.8571428571428571)
       && IsZero(mat66float.at<float>(2, 0) - 0.42857142857142849)
       && IsZero(mat66float.at<float>(2, 1) - 0.8571428571428571)
       && IsZero(mat66float.at<float>(2, 2) - 0.28571428571428559)

       && IsZero(mat66float.at<float>(3, 0) - 0.0)
       && IsZero(mat66float.at<float>(3, 1) - 0.0)
       && IsZero(mat66float.at<float>(3, 2) - 0.0)
       && IsZero(mat66float.at<float>(4, 0) - 0.0)
       && IsZero(mat66float.at<float>(4, 1) - 0.0)
       && IsZero(mat66float.at<float>(4, 2) - 0.0)
       && IsZero(mat66float.at<float>(5, 0) - 0.0)
       && IsZero(mat66float.at<float>(5, 1) - 0.0)
       && IsZero(mat66float.at<float>(5, 2) - 0.0)

       && IsZero(mat66float.at<float>(0, 3) - 0.28571428571428559)
       && IsZero(mat66float.at<float>(0, 4) - 0.0)
       && IsZero(mat66float.at<float>(0, 5) - 0.57142857142857106)
       && IsZero(mat66float.at<float>(1, 3) - 0.0)
       && IsZero(mat66float.at<float>(1, 4) - -1.1428571428571423)
       && IsZero(mat66float.at<float>(1, 5) - -0.57142857142857106)
       && IsZero(mat66float.at<float>(2, 3) - 0.57142857142857106)
       && IsZero(mat66float.at<float>(2, 4) - -0.57142857142857062)
       && IsZero(mat66float.at<float>(2, 5) - 0.85714285714285676)

       && IsZero(mat66float.at<float>(3, 3) - -0.85714285714285676)
       && IsZero(mat66float.at<float>(3, 4) - 0.28571428571428564)
       && IsZero(mat66float.at<float>(3, 5) - 0.4285714285714286)
       && IsZero(mat66float.at<float>(4, 3) - 0.28571428571428586)
       && IsZero(mat66float.at<float>(4, 4) - -0.42857142857142816)
       && IsZero(mat66float.at<float>(4, 5) - 0.8571428571428571)
       && IsZero(mat66float.at<float>(5, 3) - 0.42857142857142849)
       && IsZero(mat66float.at<float>(5, 4) - 0.8571428571428571)
       && IsZero(mat66float.at<float>(5, 5) - 0.28571428571428559)
     )
  {
    log_file << "tirili" << endl;
    ;
  }
  else
  {
    errors++;
    log_file << norm(mat66float) << endl;
    log_file << norm(mat44float) << endl;
    log_file << "rigidToAdjointTransformation<float>(const cv::Mat& rigidTransformation, cv::Mat& adjointTransformation) is NOT working correctly!" << endl;
  }
  if (IsZero(norm(mat66float - rigidToAdjointTransformation<float>(mat44float))))
  {
    ;
  }
  else
  {
    errors++;
    log_file << norm(mat66float) << endl;
    log_file << norm(mat44float) << endl;
    log_file << norm(rigidToAdjointTransformation<float>(mat44float)) << endl;
    log_file << "rigidToAdjointTransformation<float>(const cv::Mat& rigidTransformation) is NOT working correctly!" << endl;
  }

  /* ------------------------------------------------------------------------------------------------- */
  log_file << "test: adjointToRigidTransformation<double>" << std::endl;
  adjointToRigidTransformation<double>(mat66double, mat44double);
  if (
       IsZero(mat44double.at<double>(0, 0) - -0.85714285714285676)
       && IsZero(mat44double.at<double>(0, 1) - 0.28571428571428564)
       && IsZero(mat44double.at<double>(0, 2) - 0.4285714285714286)
       && IsZero(mat44double.at<double>(0, 3) - 1.1428571428571423)
       && IsZero(mat44double.at<double>(1, 0) - 0.28571428571428586)
       && IsZero(mat44double.at<double>(1, 1) - -0.42857142857142816)
       && IsZero(mat44double.at<double>(1, 2) - 0.8571428571428571)
       && IsZero(mat44double.at<double>(1, 3) - 0.28571428571428537)
       && IsZero(mat44double.at<double>(2, 0) - 0.42857142857142849)
       && IsZero(mat44double.at<double>(2, 1) - 0.8571428571428571)
       && IsZero(mat44double.at<double>(2, 2) - 0.28571428571428559)
       && IsZero(mat44double.at<double>(2, 3) - -0.57142857142857117)
       && IsZero(mat44double.at<double>(3, 0) - 0.0)
       && IsZero(mat44double.at<double>(3, 1) - 0.0)
       && IsZero(mat44double.at<double>(3, 2) - 0.0)
       && IsZero(mat44double.at<double>(3, 3) - 1.0)
     )
  {
    ;
  }
  else
  {
    errors++;
    log_file << "adjointToRigidTransformation<double>(const cv::Mat& adjointTransformation, cv::Mat& rigidTransformation) is NOT working correctly!" << endl;
  }
  if (IsZero(norm(mat44double - adjointToRigidTransformation<double>(mat66double))))
  {
    ;
  }
  else
  {
    errors++;
    log_file << "adjointToRigidTransformation<double>(const cv::Mat& adjointTransformation, cv::Mat& rigidTransformation) is NOT working correctly!" << endl;
  }

  log_file << "test: adjointToRigidTransformation<float>" << std::endl;
  adjointToRigidTransformation<float>(mat66float, mat44float);
  if (
       IsZero(mat44float.at<float>(0, 0) - -0.85714285714285676)
       && IsZero(mat44float.at<float>(0, 1) - 0.28571428571428564)
       && IsZero(mat44float.at<float>(0, 2) - 0.4285714285714286)
       && IsZero(mat44float.at<float>(0, 3) - 1.1428571428571423)
       && IsZero(mat44float.at<float>(1, 0) - 0.28571428571428586)
       && IsZero(mat44float.at<float>(1, 1) - -0.42857142857142816)
       && IsZero(mat44float.at<float>(1, 2) - 0.8571428571428571)
       && IsZero(mat44float.at<float>(1, 3) - 0.28571428571428537)
       && IsZero(mat44float.at<float>(2, 0) - 0.42857142857142849)
       && IsZero(mat44float.at<float>(2, 1) - 0.8571428571428571)
       && IsZero(mat44float.at<float>(2, 2) - 0.28571428571428559)
       && IsZero(mat44float.at<float>(2, 3) - -0.57142857142857117)
       && IsZero(mat44float.at<float>(3, 0) - 0.0)
       && IsZero(mat44float.at<float>(3, 1) - 0.0)
       && IsZero(mat44float.at<float>(3, 2) - 0.0)
       && IsZero(mat44float.at<float>(3, 3) - 1.0)
     )
  {
    ;
  }
  else
  {
    errors++;
    log_file << "adjointToRigidTransformation<float>(const cv::Mat& adjointTransformation, cv::Mat& rigidTransformation) is NOT working correctly!" << endl;
  }
  if (IsZero(norm(mat44float - adjointToRigidTransformation<float>(mat66float))))
  {
    ;
  }
  else
  {
    errors++;
    log_file << "adjointToRigidTransformation<float>(const cv::Mat& adjointTransformation, cv::Mat& rigidTransformation) is NOT working correctly!" << endl;
  }

  /* ------------------------------------------------------------------------------------------------- */
  log_file << "test: invertAdjointTransformation<double>" << std::endl;
  vec6double.at<double>(0, 0) = 1;
  vec6double.at<double>(1, 0) = 0;
  vec6double.at<double>(2, 0) = 0;
  vec6double.at<double>(3, 0) = 0;
  vec6double.at<double>(4, 0) = 0;
  vec6double.at<double>(5, 0) = 1;
  expTwist<double>(vec6double, 1, mat44double);
  rigidToAdjointTransformation<double>(mat44double, mat66double);
  invertAdjointTransformation<double>(mat66double, inverse66double);
  if (IsZero(norm(inverse66double - mat66double.inv())))
  {
    ;
  }
  else
  {
    errors++;
    log_file << "invertAdjointTransformation<double>(const cv::Mat& adjointTransformation, cv::Mat& inverse) is NOT working correctly!" << endl;
  }
  if (IsZero(norm(invertAdjointTransformation<double>(mat66double) - mat66double.inv())))
  {
    ;
  }
  else
  {
    errors++;
    log_file << "invertAdjointTransformation<double>(const cv::Mat& adjointTransformation) is NOT working correctly!" << endl;
  }

  log_file << "test: invertAdjointTransformation<float>" << std::endl;
  vec6float.at<float>(0, 0) = 1;
  vec6float.at<float>(1, 0) = 0;
  vec6float.at<float>(2, 0) = 0;
  vec6float.at<float>(3, 0) = 0;
  vec6float.at<float>(4, 0) = 0;
  vec6float.at<float>(5, 0) = 1;
  expTwist<float>(vec6float, 1, mat44float);
  rigidToAdjointTransformation<float>(mat44float, mat66float);
  invertAdjointTransformation<float>(mat66float, inverse66float);
  if (IsZero(norm(inverse66float - mat66float.inv())))
  {
    ;
  }
  else
  {
    errors++;
    log_file << "invertAdjointTransformation<float>(const cv::Mat& adjointTransformation, cv::Mat& inverse) is NOT working correctly!" << endl;
  }
  if (IsZero(norm(invertAdjointTransformation<float>(mat66float) - mat66float.inv())))
  {
    ;
  }
  else
  {
    errors++;
    log_file << "invertAdjointTransformation<float>(const cv::Mat& adjointTransformation) is NOT working correctly!" << endl;
  }

  /* ------------------------------------------------------------------------------------------------- */
  log_file << "test: twistCoordinates<double>" << std::endl;
  cv::Mat p_double = cv::Mat::zeros(3, 1, CV_64FC1);
  p_double.at<double>(2, 0) = 1;
  cv::Mat omega_double = cv::Mat::zeros(3, 1, CV_64FC1);
  omega_double.at<double>(0, 0) = 1;
  vec6double = twistCoordinates<double>(p_double, omega_double);
  if (
       vec6double.at<double>(0, 0) == 0.0
       && vec6double.at<double>(1, 0) == 1.0
       && vec6double.at<double>(2, 0) == 0.0
       && vec6double.at<double>(3, 0) == 1.0
       && vec6double.at<double>(4, 0) == 0.0
       && vec6double.at<double>(5, 0) == 0.0
     )
  {
    ;
  }
  else
  {
    errors++;
    log_file << "twistCoordinates<double>(const cv::Mat& supportPoint, const cv::Mat& axis) is NOT working correctly!" << endl;
  }

  log_file << "test: twistCoordinates<float>" << std::endl;
  cv::Mat p_float = cv::Mat::zeros(3, 1, CV_32FC1);
  p_float.at<float>(2, 0) = 1;
  cv::Mat omega_float = cv::Mat::zeros(3, 1, CV_32FC1);
  omega_float.at<float>(0, 0) = 1;
  vec6float = twistCoordinates<float>(p_float, omega_float);
  if (
       vec6float.at<float>(0, 0) == 0.0
       && vec6float.at<float>(1, 0) == 1.0
       && vec6float.at<float>(2, 0) == 0.0
       && vec6float.at<float>(3, 0) == 1.0
       && vec6float.at<float>(4, 0) == 0.0
       && vec6float.at<float>(5, 0) == 0.0
     )
  {
    ;
  }
  else
  {
    errors++;
    log_file << "twistCoordinates<float>(const cv::Mat& supportPoint, const cv::Mat& axis) is NOT working correctly!" << endl;
  }





  log_file << "test finished, there were " << errors << " errors" << std::endl;
  if (errors > 255)
  {
    errors = 255;
  }
  return errors;
}

