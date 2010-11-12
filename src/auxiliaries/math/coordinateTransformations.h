/*----------------------------------------------------------------------------------------------------------------------
 ----- Institute:   Ruhr-Universitaet-Bochum
                    Institut fuer Neuroinformatik

 ----- File:        coordinateTransformations.h

 ----- Author:      INI robotics group, maintained by HR
 ------Email:       ar@ini.rub.de
 ----- Date:        2010 11 11

 ----- Description: this file provides transformations between different coordinate systems of the same space

 ----- Credits:
 ---------------------------------------------------------------------------------------------------------------------*/

#ifndef CEDAR_AUX_MATH_COORDNIATE_TRANSFORMATIONS_H
#define CEDAR_AUX_MATH_COORDNIATE_TRANSFORMATIONS_H

// LOCAL INCLUDES

// PROJECT INCLUDES

// SYSTEM INCLUDES
#include <cv.h>

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
      void cartesianToPolar(const double x, const double y, double& rho, double& theta);

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

//      //! cartesian --> spherical coordinates of 3-dimensional real space
//      //! spherical = r, phi, theta -- radius, azimuth, zenith
//      void cartesianToSpherical(CvMAT& cartesian, CvMAT& spherical);
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
