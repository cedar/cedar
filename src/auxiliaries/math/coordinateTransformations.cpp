/*----------------------------------------------------------------------------------------------------------------------
 ----- Institute:   Ruhr-Universitaet-Bochum
                    Institut fuer Neuroinformatik

 ----- File:        coordinateTransformations.cpp

 ----- Author:      INI robotics group, maintained by HR
 ------Email:       ar@ini.rub.de
 ----- Date:        2010 11 11

 ----- Description: this file provides transformations between different coordinate systems of the same space

 ----- Credits:
 ---------------------------------------------------------------------------------------------------------------------*/
// LOCAL INCLUDES
#include "coordinateTransformations.h"

// PROJECT INCLUDES

// SYSTEM INCLUDES
#include <iostream>

using namespace std;

void cartesianToPolar(const double x, const double y, double& rho, double& theta)
{
  theta = atan2(y,x);
  if (theta < 0)
  {
    theta = 2*M_PI + theta;
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

template void cedar::aux::math::sphericalToCartesian<double>(const cv::Mat&, cv::Mat&);
template void cedar::aux::math::sphericalToCartesian<float>(const cv::Mat&, cv::Mat&);

//void cartesianToSpherical( CvMAT& cartesian, CvMAT& spherical )
//{
//  // spherical = r, phi, theta -- radius, azimuth, zenith
//  spherical( 0, 0 ) = sqrt( cartesian( 0, 0, 0 ) * cartesian( 0, 0, 0 )
//                          + cartesian( 1, 0, 0 ) * cartesian( 1, 0, 0 )
//                          + cartesian( 2, 0, 0 ) * cartesian( 2, 0, 0 )
//                          );
//	spherical( 1, 0 ) = atan2( cartesian( 1, 0, 0 ), cartesian( 0, 0, 0 ) );
//	spherical( 2, 0 ) = atan2( sqrt( cartesian( 0, 0, 0 ) * cartesian( 0, 0, 0 )
//                                 + cartesian( 1, 0, 0 ) * cartesian( 1, 0, 0 )
//                                 ),
//                             cartesian( 2, 0, 0 )
//                           );
//}
//
//void timeDerivativeSphericalToCartesian( CvMAT& timeDerivativeSpherical,
//                                             CvMAT& pointSpherical,
//                                             CvMAT& timeDerivativeCartesian
//                                           )
//{
//  // calculate the operator Jacobian
//  CvMAT operatorJacobian = CvMAT( 3, 3, CV_64FC1 );
//  operatorJacobian = 0;
//  double rho = pointSpherical( 0, 0, 0 );
//  double phi = pointSpherical( 1, 0, 0 );
//  double theta = pointSpherical( 2, 0, 0 );
//  operatorJacobian( 0, 0 ) = sin( theta ) * cos( phi );
//  operatorJacobian( 1, 0 ) = sin( theta ) * sin( phi );
//  operatorJacobian( 2, 0 ) = cos( theta );
//
//  operatorJacobian( 0, 1 ) = - rho * sin( theta ) * sin( phi );
//  operatorJacobian( 1, 1 ) = rho * sin( theta ) * cos( phi );
//  operatorJacobian( 2, 1 ) = 0;
//
//  operatorJacobian( 0, 2 ) = rho * cos( theta ) * cos( phi );
//  operatorJacobian( 1, 2 ) = rho * cos( theta ) * sin( phi );
//  operatorJacobian( 2, 2 ) = - rho * sin( theta );
//
//  // apply chain rule
//  timeDerivativeCartesian = operatorJacobian * timeDerivativeSpherical;
//}
//
//void timeDerivativeCartesianToSpherical( CvMAT& timeDerivativeCartesian,
//                                             CvMAT& pointCartesian,
//                                             CvMAT& timeDerivativeSpherical
//                                           )
//{
//  // calculate the operator Jacobian
//  CvMAT operatorJacobian = CvMAT( 3, 3, CV_64FC1 );
//  operatorJacobian = 0;
//  double x = pointCartesian( 0, 0, 0 );
//  double y = pointCartesian( 1, 0, 0 );
//  double z = pointCartesian( 2, 0, 0 );
//  double s = sqrt( x*x + y*y + z*z );
//
//  operatorJacobian( 0, 0 ) = x / s;
//  operatorJacobian( 0, 1 ) = y / s;
//  operatorJacobian( 0, 2 ) = z / s;
//
//  operatorJacobian( 1, 0 ) = -y / ( x*x + y*y );
//  operatorJacobian( 1, 1 ) = x / ( x*x + y*y );
//  operatorJacobian( 1, 2 ) = 0;
//
//  operatorJacobian( 2, 0 ) = x * z / ( s * s * sqrt( x*x + y*y ) );
//  operatorJacobian( 2, 1 ) = y * z / ( s * s * sqrt( x*x + y*y ) );
//  operatorJacobian( 2, 2 ) = - ( x*x + y*y ) / ( s * s * sqrt( x*x + y*y ) );
//
//  // apply chain rule
//  timeDerivativeSpherical = operatorJacobian * timeDerivativeCartesian;
//}







