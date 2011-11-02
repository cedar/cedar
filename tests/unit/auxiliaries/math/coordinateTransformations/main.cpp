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

    Maintainer:  Stephan Zibner
    Email:       stephan.zibner@rub.de
    Date:        2010 11 12

    Description: Tests all coordinate transformations.

    Credits:

======================================================================================================================*/

// LOCAL INCLUDES

// PROJECT INCLUDES
#include "cedar/auxiliaries/math/coordinateTransformations.h"
#include "cedar/auxiliaries/math/tools.h"
#include "cedar/auxiliaries/math/constants.h"
#include "cedar/auxiliaries/LogFile.h"

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
  horst.at<double>(1, 0) = cedar::aux::math::pi/2.0;
  horst.at<double>(2, 0) = cedar::aux::math::pi/4.0;
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
  chantalle.at<float>(1, 0) = cedar::aux::math::pi/4.0;
  chantalle.at<float>(2, 0) = cedar::aux::math::pi/4.0;
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
       && IsZero(horst.at<double>(1, 0) - cedar::aux::math::pi/2.0)
       && IsZero(horst.at<double>(2, 0) - cedar::aux::math::pi/4.0)
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
