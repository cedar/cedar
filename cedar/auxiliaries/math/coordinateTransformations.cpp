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

    File:        coordinateTransformations.cpp

    Maintainer:  Hendrik Reimann
    Email:       hendrik.reimann@ini.rub.de
    Date:        2010 11 11

    Description: This file provides transformations between different coordinate systems of the same space.

    Credits:

======================================================================================================================*/


// LOCAL INCLUDES
#include "cedar/auxiliaries/math/coordinateTransformations.h"
#include "cedar/auxiliaries/math/constants.h"

// PROJECT INCLUDES

// SYSTEM INCLUDES
#include <iostream>

using namespace std;

void cartesianToPolar(const double x, const double y, double& rho, double& theta)
{
  theta = atan2(y,x);
  if (theta < 0)
  {
    theta = 2*cedar::aux::math::pi + theta;
  }
  rho = sqrt(x*x+y*y);
}

template<typename T>
void cedar::aux::math::sphericalToCartesian(const cv::Mat& spherical, cv::Mat& cartesian)
{
  // spherical = radius, theta, phi
  cartesian.at<T>(0, 0) = spherical.at<T>(0, 0) * sin(spherical.at<T>(1, 0)) * cos(spherical.at<T>(2, 0));
  cartesian.at<T>(1, 0) = spherical.at<T>(0, 0) * sin(spherical.at<T>(1, 0)) * sin(spherical.at<T>(2, 0));
  cartesian.at<T>(2, 0) = spherical.at<T>(0, 0) * cos(spherical.at<T>(1, 0));
}

template CEDAR_AUX_LIB_EXPORT void cedar::aux::math::sphericalToCartesian<double>(const cv::Mat&, cv::Mat&);
template CEDAR_AUX_LIB_EXPORT void cedar::aux::math::sphericalToCartesian<float>(const cv::Mat&, cv::Mat&);

template<typename T>
cv::Mat cedar::aux::math::sphericalToCartesian(const cv::Mat& spherical)
{
  cv::Mat result = cv::Mat::zeros(3, 1, spherical.type());
  sphericalToCartesian<T>(spherical, result);
  return result;
}

template CEDAR_AUX_LIB_EXPORT cv::Mat cedar::aux::math::sphericalToCartesian<double>(const cv::Mat&);
template CEDAR_AUX_LIB_EXPORT cv::Mat cedar::aux::math::sphericalToCartesian<float>(const cv::Mat&);

template<typename T>
void cedar::aux::math::cartesianToSpherical(const cv::Mat& cartesian, cv::Mat& spherical)
{
  // spherical = radius, theta, phi
  spherical.at<T>(0, 0) = sqrt(
                                cartesian.at<T>(0, 0) * cartesian.at<T>(0, 0)
                                + cartesian.at<T>(1, 0) * cartesian.at<T>(1, 0)
                                + cartesian.at<T>(2, 0) * cartesian.at<T>(2, 0)
                              );
  spherical.at<T>(1, 0) = atan2(
                                 sqrt(
                                       cartesian.at<T>(0, 0) * cartesian.at<T>(0, 0)
                                       + cartesian.at<T>(1, 0) * cartesian.at<T>(1, 0)
                                     ),
                                 cartesian.at<T>(2, 0)
                               );
  spherical.at<T>(2, 0) = atan2(cartesian.at<T>(1, 0), cartesian.at<T>(0, 0));
}

template CEDAR_AUX_LIB_EXPORT void cedar::aux::math::cartesianToSpherical<double>(const cv::Mat&, cv::Mat&);
template CEDAR_AUX_LIB_EXPORT void cedar::aux::math::cartesianToSpherical<float>(const cv::Mat&, cv::Mat&);
//
//void timeDerivativeSphericalToCartesian(CvMAT& timeDerivativeSpherical,
//                                             CvMAT& pointSpherical,
//                                             CvMAT& timeDerivativeCartesian
//                                          )
//{
//  // calculate the operator Jacobian
//  CvMAT operatorJacobian = CvMAT(3, 3, CV_64FC1);
//  operatorJacobian = 0;
//  double rho = pointSpherical(0, 0, 0);
//  double phi = pointSpherical(1, 0, 0);
//  double theta = pointSpherical(2, 0, 0);
//  operatorJacobian(0, 0) = sin(theta) * cos(phi);
//  operatorJacobian(1, 0) = sin(theta) * sin(phi);
//  operatorJacobian(2, 0) = cos(theta);
//
//  operatorJacobian(0, 1) = - rho * sin(theta) * sin(phi);
//  operatorJacobian(1, 1) = rho * sin(theta) * cos(phi);
//  operatorJacobian(2, 1) = 0;
//
//  operatorJacobian(0, 2) = rho * cos(theta) * cos(phi);
//  operatorJacobian(1, 2) = rho * cos(theta) * sin(phi);
//  operatorJacobian(2, 2) = - rho * sin(theta);
//
//  // apply chain rule
//  timeDerivativeCartesian = operatorJacobian * timeDerivativeSpherical;
//}
//
//void timeDerivativeCartesianToSpherical(CvMAT& timeDerivativeCartesian,
//                                             CvMAT& pointCartesian,
//                                             CvMAT& timeDerivativeSpherical
//                                          )
//{
//  // calculate the operator Jacobian
//  CvMAT operatorJacobian = CvMAT(3, 3, CV_64FC1);
//  operatorJacobian = 0;
//  double x = pointCartesian(0, 0, 0);
//  double y = pointCartesian(1, 0, 0);
//  double z = pointCartesian(2, 0, 0);
//  double s = sqrt(x*x + y*y + z*z);
//
//  operatorJacobian(0, 0) = x / s;
//  operatorJacobian(0, 1) = y / s;
//  operatorJacobian(0, 2) = z / s;
//
//  operatorJacobian(1, 0) = -y / (x*x + y*y);
//  operatorJacobian(1, 1) = x / (x*x + y*y);
//  operatorJacobian(1, 2) = 0;
//
//  operatorJacobian(2, 0) = x * z / (s * s * sqrt(x*x + y*y));
//  operatorJacobian(2, 1) = y * z / (s * s * sqrt(x*x + y*y));
//  operatorJacobian(2, 2) = - (x*x + y*y) / (s * s * sqrt(x*x + y*y));
//
//  // apply chain rule
//  timeDerivativeSpherical = operatorJacobian * timeDerivativeCartesian;
//}







