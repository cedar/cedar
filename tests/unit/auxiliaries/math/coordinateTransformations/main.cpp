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

// SYSTEM INCLUDES

int main(int, char**)
{
  // the number of errors encountered in this test
  int errors = 0;

  std::cout << "test: sphericalToCartesian<double>" << std::endl;
  cv::Mat horst = cv::Mat::zeros(3, 1, CV_64FC1);
  horst.at<double>(0, 0) = sqrt(2.0);
  horst.at<double>(1, 0) = cedar::aux::math::pi/2.0;
  horst.at<double>(2, 0) = cedar::aux::math::pi/4.0;
  cv::Mat kevin(3, 1, CV_64FC1);
  cedar::aux::math::sphericalToCartesian<double>(horst, kevin);
  if (
       cedar::aux::math::isZero(kevin.at<double>(0, 0) -1.0)
       && cedar::aux::math::isZero(kevin.at<double>(1, 0) -1.0)
       && cedar::aux::math::isZero(kevin.at<double>(2, 0) + 0.0)
     )
  {
    ;
  }
  else
  {
    errors++;
    std::cout << kevin.at<double>(0, 0)<< " " << kevin.at<double>(1, 0)<< " " << kevin.at<double>(2, 0) << std::endl;
    std::cout << "test of sphericalToCartesian<double> failed!" << std::endl;
  }


  std::cout << "test: sphericalToCartesian<float>" << std::endl;
  cv::Mat chantalle = cv::Mat::zeros(3, 1, CV_32FC1);
  chantalle.at<float>(0, 0) = 1.0;
  chantalle.at<float>(1, 0) = cedar::aux::math::pi/4.0;
  chantalle.at<float>(2, 0) = cedar::aux::math::pi/4.0;
  cv::Mat jaqueline(3, 1, CV_32FC1);
  cedar::aux::math::sphericalToCartesian<float>(chantalle, jaqueline);
  if (
       cedar::aux::math::isZero(jaqueline.at<float>(0, 0) - 0.5, 1e-5)
       && cedar::aux::math::isZero(jaqueline.at<float>(1, 0) - 0.5, 1e-5)
       && cedar::aux::math::isZero(jaqueline.at<float>(2, 0) - sqrt(2.0)/2.0, 1e-5)
     )
  {
    ;
  }
  else
  {
    errors++;
    std::cout << jaqueline.at<float>(0, 0)<< " " << jaqueline.at<float>(1, 0)<< " " << jaqueline.at<float>(2, 0) << std::endl;
    std::cout << "test of sphericalToCartesian<float> failed!" << std::endl;
  }


  std::cout << "test: cartesianToSpherical<double>" << std::endl;
  cedar::aux::math::cartesianToSpherical<double>(kevin, horst);
  if (
       cedar::aux::math::isZero(horst.at<double>(0, 0) -sqrt(2.0))
       && cedar::aux::math::isZero(horst.at<double>(1, 0) - cedar::aux::math::pi/2.0)
       && cedar::aux::math::isZero(horst.at<double>(2, 0) - cedar::aux::math::pi/4.0)
     )
  {
    ;
  }
  else
  {
    errors++;
    std::cout << horst.at<double>(0, 0)<< " " << horst.at<double>(1, 0)<< " " << horst.at<double>(2, 0) << std::endl;
    std::cout << "test of cartesianToSpherical<double> failed!" << std::endl;
  }

  std::cout << "test finished, there were " << errors << " errors" << std::endl;
  if (errors > 255)
  {
    errors = 255;
  }
  return errors;
}
