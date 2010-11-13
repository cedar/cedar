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
  cv::Mat vec3float(3, 1, CV_32FC1);
  cv::Mat mat33float(3, 3, CV_32FC1);
  cv::Mat vec6float(6, 1, CV_32FC1);
  cv::Mat mat44float(4, 4, CV_32FC1);
  cv::Mat mat66float(6, 6, CV_32FC1);

  /* ------------------------------------------------------------------------------------------------- */
  log_file << "test: wedgeRotation<double>" << std::endl;
  vec3double.at<double>(0, 0) = 3;
  vec3double.at<double>(1, 0) = 5;
  vec3double.at<double>(2, 0) = 7;
  wedgeRotation<double>(vec3double, mat33double);
  if (
       mat33double.at<double>(0, 0) == 0.0
       && mat33double.at<double>(0, 1) == -7.0
       && mat33double.at<double>(0, 2) == 5.0
       && mat33double.at<double>(1, 0) == 7.0
       && mat33double.at<double>(1, 1) == 0.0
       && mat33double.at<double>(1, 2) == -3.0
       && mat33double.at<double>(2, 0) == -5.0
       && mat33double.at<double>(2, 1) == 3.0
       && mat33double.at<double>(2, 2) == 0.0
     )
  {
    ;
  }
  else
  {
    errors++;
    log_file << "wedgeRotation<double>(cv::Mat& vector, cv::Mat& result) is NOT working correctly!" << std::endl;
  }
  if (IsZero(norm(mat33double - wedgeRotation<double>(vec3double))))
  {
    ;
  }
  else
  {
    log_file << "wedgeRotationMatrix(cv::Mat& vector) is NOT working correctly!" << endl;
  }

  log_file << "test: wedgeRotation<float>" << std::endl;
  vec3float.at<float>(0, 0) = 5;
  vec3float.at<float>(1, 0) = 55;
  vec3float.at<float>(2, 0) = 555;
  wedgeRotation<float>(vec3float, mat33float);
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
    log_file << "wedgeRotation<float>(cv::Mat& vector, cv::Mat& result) is NOT working correctly!" << std::endl;
  }
  if (IsZero(norm(mat33float - wedgeRotation<float>(vec3float))))
  {
    ;
  }
  else
  {
    log_file << "wedgeRotationMatrix(cv::Mat& vector) is NOT working correctly!" << endl;
  }

  /* ------------------------------------------------------------------------------------------------- */
  log_file << "test: veeRotation<double>" << std::endl;
  vec3double *= 0;
  veeRotation<double>(mat33double, vec3double);
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
    log_file << "veeRotation<double>(cv::Mat& vector, cv::Mat& result) is NOT working correctly!" << endl;
  }
  if (IsZero(norm(vec3double - veeRotation<double>(mat33double))))
  {
    ;
  }
  else
  {
    log_file << "veeRotation<double>(cv::Mat& matrix) is NOT working correctly!" << endl;
  }

  log_file << "test: veeRotation<float>" << std::endl;
  vec3float *= 0;
  veeRotation<float>(mat33float, vec3float);
  if (
       vec3float.at<float>(0, 0) == 5.0
       && vec3float.at<float>(1, 0) == 55.0
       && vec3float.at<float>(2, 0) == 5555.0
     )
  {
    ;
  }
  else
  {
    log_file << "veeRotation<float>(cv::Mat& vector, cv::Mat& result) is NOT working correctly!" << endl;
  }
  if (IsZero(norm(vec3float - veeRotation<float>(mat33float))))
  {
    ;
  }
  else
  {
    log_file << "veeRotation<float>(cv::Mat& matrix) is NOT working correctly!" << endl;
  }


  log_file << "test finished, there were " << errors << " errors" << std::endl;
  if (errors > 255)
  {
    errors = 255;
  }
  return errors;
}
