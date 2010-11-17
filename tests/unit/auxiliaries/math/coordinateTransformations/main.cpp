/*----------------------------------------------------------------------------------------------------------------------
 ----- Institute:   Ruhr-Universitaet Bochum
                    Institut fuer Neuroinformatik

 ----- File:        main.cpp

 ----- Author:      Stephan Zibner
 ----- Email:       stephan.zibner@ini.rub.de
 ----- Date:        2010 11 12

 ----- Description: Tests all coordinate transformations.

 ----- Credits:
 ---------------------------------------------------------------------------------------------------------------------*/

// LOCAL INCLUDES

// PROJECT INCLUDES
#include "src/auxiliaries/math/coordinateTransformations.h"
#include "src/auxiliaries/math/tools.h"
#include "src/auxiliaries/LogFile.h"

// SYSTEM INCLUDES

using namespace cedar::aux;
using namespace cedar::aux::math;
using namespace std;
#ifdef EQN_EPS
#undef EQN_EPS
#endif
#define EQN_EPS 1e-5


int main()
{
  LogFile log_file("UnitTestCoordinateTransformations.log");
  log_file.addTimeStamp();
  log_file << std::endl;
  // the number of errors encountered in this test
  int errors = 0;

  log_file << "test: sphericalToCartesian<double>" << std::endl;
  cv::Mat horst = cv::Mat::zeros(3, 1, CV_64FC1);
  horst.at<double>(0, 0) = sqrt(2.0);
  horst.at<double>(1, 0) = M_PI/2.0;
  horst.at<double>(2, 0) = M_PI/4.0;
  cv::Mat kevin(3, 1, CV_64FC1);
  sphericalToCartesian<double>(horst, kevin);
  if (
       IsZero(kevin.at<double>(0, 0) -1.0)
       && IsZero(kevin.at<double>(1, 0) -1.0)
       && IsZero(kevin.at<double>(2, 0) + 0.0)
     )
  {
    ;
  }
  else
  {
    errors++;
    log_file << kevin.at<double>(0, 0)<< " " << kevin.at<double>(1, 0)<< " " << kevin.at<double>(2, 0) << std::endl;
    log_file << "test of sphericalToCartesian<double> failed!" << std::endl;
  }


  log_file << "test: sphericalToCartesian<float>" << std::endl;
  cv::Mat chantalle = cv::Mat::zeros(3, 1, CV_32FC1);
  chantalle.at<float>(0, 0) = 1.0;
  chantalle.at<float>(1, 0) = M_PI/4.0;
  chantalle.at<float>(2, 0) = M_PI/4.0;
  cv::Mat jaqueline(3, 1, CV_32FC1);
  sphericalToCartesian<float>(chantalle, jaqueline);
  if (
       IsZero(jaqueline.at<float>(0, 0) - 0.5)
       && IsZero(jaqueline.at<float>(1, 0) - 0.5)
       && IsZero(jaqueline.at<float>(2, 0) - sqrt(2.0)/2.0)
     )
  {
    ;
  }
  else
  {
    errors++;
    log_file << jaqueline.at<float>(0, 0)<< " " << jaqueline.at<float>(1, 0)<< " " << jaqueline.at<float>(2, 0) << std::endl;
    log_file << "test of sphericalToCartesian<float> failed!" << std::endl;
  }


  log_file << "test: cartesianToSpherical<double>" << std::endl;
  cartesianToSpherical<double>(kevin, horst);
  if (
       IsZero(horst.at<double>(0, 0) -sqrt(2.0))
       && IsZero(horst.at<double>(1, 0) - M_PI/2.0)
       && IsZero(horst.at<double>(2, 0) - M_PI/4.0)
     )
  {
    ;
  }
  else
  {
    errors++;
    log_file << horst.at<double>(0, 0)<< " " << horst.at<double>(1, 0)<< " " << horst.at<double>(2, 0) << std::endl;
    log_file << "test of cartesianToSpherical<double> failed!" << std::endl;
  }

  log_file << "test finished, there were " << errors << " errors" << std::endl;
  if (errors > 255)
  {
    errors = 255;
  }
  return errors;
}
