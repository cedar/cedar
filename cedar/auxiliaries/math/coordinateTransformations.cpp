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

    File:        coordinateTransformations.cpp

    Maintainer:  Hendrik Reimann
    Email:       hendrik.reimann@ini.rub.de
    Date:        2010 11 11

    Description: This file provides transformations between different coordinate systems of the same space.

    Credits:

======================================================================================================================*/

// CEDAR INCLUDES
#include "cedar/auxiliaries/math/coordinateTransformations.h"
#include "cedar/auxiliaries/math/constants.h"

// SYSTEM INCLUDES
#include <iostream>


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
