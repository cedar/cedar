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

    File:        coordinateTransformations.h

    Maintainer:  Hendrik Reimann
    Email:       hendrik.reimann@ini.rub.de
    Date:        2010 11 11

    Description: This file provides transformations between different coordinate systems of the same space.

    Credits:

======================================================================================================================*/

#ifndef CEDAR_AUX_MATH_COORDNIATE_TRANSFORMATIONS_H
#define CEDAR_AUX_MATH_COORDNIATE_TRANSFORMATIONS_H

// LOCAL INCLUDES
#include "cedar/auxiliaries/lib.h"

// PROJECT INCLUDES

// SYSTEM INCLUDES
#include <opencv2/opencv.hpp>

namespace cedar
{
  namespace aux
  {
    namespace math
    {
      /*! Cartesian --> polar coordinates of 2-dimensional real space
       * @param x    Cartesian x coordinate
       * @param y    Cartesian y coordinate
       * @param rho    polar coordinate radius
       * @param theta    polar coordinate angle
       */
      CEDAR_AUX_LIB_EXPORT void cartesianToPolar(const double x, const double y, double& rho, double& theta);

      /*! spherical --> cartesian coordinates for points of 3-dimensional space
       * spherical coordinate explanation:
       * radius: distance from origin
       * theta: angular displacement from the z-axis in radians (inclination)
       * phi: angular displacement from positive x-axis in x-y-plane in radians (azimuth)
       * @param spherical    spherical coordinates (radius, theta, phi), use 3 rows, 1 column
       * @param cartesian    Cartesian coordinates (x, y, z), use 3 rows, 1 column
       */
      template<typename T>
      void sphericalToCartesian(const cv::Mat& spherical, cv::Mat& cartesian);

      /*! spherical --> cartesian coordinates for points of 3-dimensional space
       * spherical coordinate explanation:
       * radius: distance from origin
       * theta: angular displacement from the z-axis in radians (inclination)
       * phi: angular displacement from positive x-axis in x-y-plane in radians (azimuth)
       * @param spherical    spherical coordinates (radius, theta, phi), use 3 rows, 1 column
       * @return Cartesian coordinates (x, y, z), uses 3 rows, 1 column
       */
      template<typename T>
      cv::Mat sphericalToCartesian(const cv::Mat& spherical);

      /*! cartesian --> spherical coordinates of 3-dimensional real space
       * radius: distance from origin
       * theta: angular displacement from the z-axis in radians (inclination)
       * phi: angular displacement from positive x-axis in x-y-plane in radians (azimuth)
       * @param cartesian    Cartesian coordinates (x, y, z), use 3 rows, 1 column
       * @param spherical    spherical coordinates (radius, theta, phi), use 3 rows, 1 column
       */
      template<typename T>
      void cartesianToSpherical(const cv::Mat& cartesian, cv::Mat& spherical);
//
//      //! transforms a vector at a point, both given in spherical coords, to a vector in cartesian coords
//      //! spherical = r, phi, theta -- radius, azimuth, zenith
//      void timeDerivativeSphericalToCartesian(
//                                               CvMAT& timeDerivativeSpherical,
//                                               CvMAT& pointSpherical,
//                                               CvMAT& timeDerivativeCartesian
//                                             );
//
//      //! transforms a vector at a point, both given in cartesian coords, to a vector in spherical coords
//      //! spherical = r, phi, theta -- radius, azimuth, zenith
//      void timeDerivativeCartesianToSpherical(
//                                               CvMAT& timeDerivativeCartesian,
//                                               CvMAT& pointCartesian,
//                                               CvMAT& timeDerivativeSpherical
//                                             );
    };
  };
};
#endif  // CEDAR_AUX_MATH_COORDNIATE_TRANSFORMATIONS_H
