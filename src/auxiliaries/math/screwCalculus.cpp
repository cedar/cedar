/*----------------------------------------------------------------------------------------------------------------------
 ----- Institute:   Ruhr-Universitaet-Bochum
                    Institut fuer Neuroinformatik

 ----- File:        coordinateTransformations.h

 ----- Author:      Hendrik Reimann
 ------Email:       hendrik.reimann@ini.rub.de
 ----- Date:        2010 11 01

 ----- Description: this file provides screw calculus functions for rigid body geometry

 ----- Credits:
 ---------------------------------------------------------------------------------------------------------------------*/

// LOCAL INCLUDES
#include "screwCalculus.h"

// PROJECT INCLUDES
#include "tools.h"

// SYSTEM INCLUDES

using namespace std;
using namespace cv;


// /TODO: change names of references to "r..."
// /TODO: shorten code
// /TODO: avoid allocation of memory wherever unnecessary


//----------------------------------------------------------------------------------------------------------------------
// wedge axis functions
//----------------------------------------------------------------------------------------------------------------------
template<typename T>
void cedar::aux::math::wedgeAxis(const cv::Mat& axis, cv::Mat& result)
{
	result.at<T>(0, 0) = 0.0;
	result.at<T>(0, 1) = -axis.at<T>(2, 0);
	result.at<T>(0, 2) = axis.at<T>(1, 0);
	result.at<T>(1, 0) = axis.at<T>(2, 0);
	result.at<T>(1, 1) = 0.0;
	result.at<T>(1, 2) = -axis.at<T>(0, 0);
	result.at<T>(2, 0) = -axis.at<T>(1, 0);
	result.at<T>(2, 1) = axis.at<T>(0, 0);
	result.at<T>(2, 2) = 0.0;
}

template void cedar::aux::math::wedgeAxis<double>(const cv::Mat&, cv::Mat&);
template void cedar::aux::math::wedgeAxis<float>(const cv::Mat&, cv::Mat&);

template<typename T>
cv::Mat cedar::aux::math::wedgeAxis(const cv::Mat& axis)
{
  cv::Mat result(3, 3, axis.type());
  wedgeAxis<T>(axis, result);
  return result;
}

template cv::Mat cedar::aux::math::wedgeAxis<double>(const cv::Mat&);
template cv::Mat cedar::aux::math::wedgeAxis<float>(const cv::Mat&);

//----------------------------------------------------------------------------------------------------------------------
// vee axis functions
//----------------------------------------------------------------------------------------------------------------------
template<typename T>
void cedar::aux::math::veeAxis(const cv::Mat& matrix, cv::Mat& result)
{
	result.at<T>(0, 0) = matrix.at<T>(2, 1);
	result.at<T>(1, 0) = matrix.at<T>(0, 2);
	result.at<T>(2, 0) = matrix.at<T>(1, 0);
}

template void cedar::aux::math::veeAxis<double>(const cv::Mat&, cv::Mat&);
template void cedar::aux::math::veeAxis<float>(const cv::Mat&, cv::Mat&);

template<typename T>
cv::Mat cedar::aux::math::veeAxis(const cv::Mat& matrix)
{
  cv::Mat result(3, 1, matrix.type());
  veeAxis<T>(matrix, result);
  return result;
}

template cv::Mat cedar::aux::math::veeAxis<double>(const cv::Mat&);
template cv::Mat cedar::aux::math::veeAxis<float>(const cv::Mat&);

/* ------------------------------------------------------------------------------------------------- */
template<typename T>
void cedar::aux::math::wedgeTwist(const cv::Mat& twist, cv::Mat& result)
{
	result.at<T>(0, 0) = 0.0;
	result.at<T>(0, 1) = -twist.at<T>(5, 0);
	result.at<T>(0, 2) = twist.at<T>(4, 0);
	result.at<T>(1, 0) = twist.at<T>(5, 0);
	result.at<T>(1, 1) = 0.0;
	result.at<T>(1, 2) = -twist.at<T>(3, 0);
	result.at<T>(2, 0) = -twist.at<T>(4, 0);
	result.at<T>(2, 1) = twist.at<T>(3, 0);
	result.at<T>(2, 2) = 0.0;

	result.at<T>(0, 3) = twist.at<T>(0, 0);
	result.at<T>(1, 3) = twist.at<T>(1, 0);
	result.at<T>(2, 3) = twist.at<T>(2, 0);

	result.at<T>(3, 0) = 0.0;
	result.at<T>(3, 1) = 0.0;
	result.at<T>(3, 2) = 0.0;
	result.at<T>(3, 3) = 0.0;
}

template void cedar::aux::math::wedgeTwist<double>(const cv::Mat&, cv::Mat&);
template void cedar::aux::math::wedgeTwist<float>(const cv::Mat&, cv::Mat&);

template<typename T>
cv::Mat cedar::aux::math::wedgeTwist(const cv::Mat& twist)
{
  Mat result(4, 4, twist.type());
  wedgeTwist<T>(twist, result);
  return result;
}

template cv::Mat cedar::aux::math::wedgeTwist<double>(const cv::Mat&);
template cv::Mat cedar::aux::math::wedgeTwist<float>(const cv::Mat&);

/* ------------------------------------------------------------------------------------------------- */
template<typename T>
void cedar::aux::math::veeTwist(const cv::Mat& matrix, cv::Mat& result)
{
	// position
	result.at<T>(0, 0) = matrix.at<T>(0, 3);
	result.at<T>(1, 0) = matrix.at<T>(1, 3);
	result.at<T>(2, 0) = matrix.at<T>(2, 3);
	// axis
	result.at<T>(3, 0) = matrix.at<T>(2, 1);
	result.at<T>(4, 0) = matrix.at<T>(0, 2);
	result.at<T>(5, 0) = matrix.at<T>(1, 0);
}

template void cedar::aux::math::veeTwist<double>(const cv::Mat&, cv::Mat&);
template void cedar::aux::math::veeTwist<float>(const cv::Mat&, cv::Mat&);

template<typename T>
cv::Mat cedar::aux::math::veeTwist(const cv::Mat& matrix)
{
  Mat result(6, 1, matrix.type());
  veeTwist<T>(matrix, result);
  return result;
}

template cv::Mat cedar::aux::math::veeTwist<double>(const cv::Mat&);
template cv::Mat cedar::aux::math::veeTwist<float>(const cv::Mat&);

/* ------------------------------------------------------------------------------------------------- */
template<typename T>
void cedar::aux::math::expAxis(const cv::Mat& axis, double theta, cv::Mat& result)
{
  setIdentity(result);
  cv::Mat omegaWedge(3, 3, axis.type());
  wedgeAxis<T>(axis, omegaWedge);
  result += (omegaWedge * sin(theta)) + (omegaWedge * omegaWedge * (1 - cos(theta)));
}

template void cedar::aux::math::expAxis<double>(const cv::Mat&, double, cv::Mat&);
template void cedar::aux::math::expAxis<float>(const cv::Mat&, double, cv::Mat&);

template<typename T>
cv::Mat cedar::aux::math::expAxis(const cv::Mat& axis, double theta)
{
  Mat result(3, 3, axis.type());
  expAxis<T>(axis, theta, result);
  return result;
}

template cv::Mat cedar::aux::math::expAxis<double>(const cv::Mat&, double);
template cv::Mat cedar::aux::math::expAxis<float>(const cv::Mat&, double);

/* ------------------------------------------------------------------------------------------------- */
template<typename T>
void cedar::aux::math::logAxis(const cv::Mat& rotation, cv::Mat& omega, double& theta, bool optionalThetaChoice)
{
	T trace = rotation.at<T>(0, 0) + rotation.at<T>(1, 1) + rotation.at<T>(2, 2);
  // calculate rotation angle
	theta = acos((trace - 1) / 2.0);
  if (trace <= -1)
  {
    // capture numeric failures for trace very close to one
    theta = M_PI;
  }
	if (optionalThetaChoice)
  {
    theta = 2*M_PI - theta;
  }
  // calculate axis of rotation
	omega.at<T>(0, 0) = 1 / (2 * sin(theta)) * (rotation.at<T>(2, 1) - rotation.at<T>(1, 2));
	omega.at<T>(1, 0) = 1 / (2 * sin(theta)) * (rotation.at<T>(0, 2) - rotation.at<T>(2, 0));
	omega.at<T>(2, 0) = 1 / (2 * sin(theta)) * (rotation.at<T>(1, 0) - rotation.at<T>(0, 1));
  omega = omega * (1 / norm(omega));

  if (IsZero(theta - M_PI))
  {
    // easy way of calculating axis fails for numerical reasons, choose different formula
    if ((rotation.at<T>(1, 0) > 0) && (rotation.at<T>(2, 0) > 0) && (rotation.at<T>(2, 1) > 0))
    {
      omega.at<T>(0, 0) = + sqrt((rotation.at<T>(0, 0) - cos(theta)) / (1-cos(theta)));
      omega.at<T>(1, 0) = + sqrt((rotation.at<T>(1, 1) - cos(theta)) / (1-cos(theta)));
      omega.at<T>(2, 0) = + sqrt((rotation.at<T>(2, 2) - cos(theta)) / (1-cos(theta)));
    }
    if ((rotation.at<T>(1, 0) > 0) && (rotation.at<T>(2, 0) > 0) && (rotation.at<T>(2, 1) < 0))
    {
      omega.at<T>(0, 0) = + sqrt((rotation.at<T>(0, 0) - cos(theta)) / (1-cos(theta)));
      omega.at<T>(1, 0) = + sqrt((rotation.at<T>(1, 1) - cos(theta)) / (1-cos(theta)));
      omega.at<T>(2, 0) = - sqrt((rotation.at<T>(2, 2) - cos(theta)) / (1-cos(theta)));
    }
    if ((rotation.at<T>(1, 0) < 0) && (rotation.at<T>(2, 0) > 0) && (rotation.at<T>(2, 1) < 0))
    {
      omega.at<T>(0, 0) = + sqrt((rotation.at<T>(0, 0) - cos(theta)) / (1-cos(theta)));
      omega.at<T>(1, 0) = - sqrt((rotation.at<T>(1, 1) - cos(theta)) / (1-cos(theta)));
      omega.at<T>(2, 0) = + sqrt((rotation.at<T>(2, 2) - cos(theta)) / (1-cos(theta)));
    }
    if ((rotation.at<T>(1, 0) < 0) && (rotation.at<T>(2, 0) < 0) && (rotation.at<T>(2, 1) > 0))
    {
      omega.at<T>(0, 0) = + sqrt((rotation.at<T>(0, 0) - cos(theta)) / (1-cos(theta)));
      omega.at<T>(1, 0) = - sqrt((rotation.at<T>(1, 1) - cos(theta)) / (1-cos(theta)));
      omega.at<T>(2, 0) = - sqrt((rotation.at<T>(2, 2) - cos(theta)) / (1-cos(theta)));
    }
  }
}

template void cedar::aux::math::logAxis<double>(const cv::Mat&, cv::Mat&, double&, bool);
template void cedar::aux::math::logAxis<float>(const cv::Mat&, cv::Mat&, double&, bool);

/* ------------------------------------------------------------------------------------------------- */
template<typename T>
void cedar::aux::math::expTwist(const cv::Mat& xi, double theta, cv::Mat& result)
{
  Mat v = xi(Rect(0, 0, 1, 3));
  Mat omega = xi(Rect(0, 3, 1, 3));

	// rotation
  Mat R(3, 3, xi.type());
	expAxis<T>(omega, theta, R);
	Mat p;

  // translation
	if (norm(omega) == 0) // pure translation
	{
		p = v * theta;
	}
	else // translation and rotation
	{
		// p = (eye(3,3) - r)*(cross(w,v)) + w*w'*v*theta (Matlab notation)
//		cv::Mat I(3, 3, xi.type());
//		setIdentity(I);
    cv::Mat I = cv::Mat::eye(3, 3, xi.type());
    cv::Mat s1 = (I - R) * omega.cross(v);
    p = s1 + (omega * omega.t() * v * theta);
	}
	// join to form rigid transformation matrix
	// a = [ R p
	//			 0 1 ]
	result.at<T>(0, 0) = R.at<T>(0, 0);
	result.at<T>(0, 1) = R.at<T>(0, 1);
	result.at<T>(0, 2) = R.at<T>(0, 2);
	result.at<T>(1, 0) = R.at<T>(1, 0);
	result.at<T>(1, 1) = R.at<T>(1, 1);
	result.at<T>(1, 2) = R.at<T>(1, 2);
	result.at<T>(2, 0) = R.at<T>(2, 0);
	result.at<T>(2, 1) = R.at<T>(2, 1);
	result.at<T>(2, 2) = R.at<T>(2, 2);

	result.at<T>(0, 3) = p.at<T>(0, 0);
	result.at<T>(1, 3) = p.at<T>(1, 0);
	result.at<T>(2, 3) = p.at<T>(2, 0);

	result.at<T>(3, 0) = 0.0;
	result.at<T>(3, 1) = 0.0;
	result.at<T>(3, 2) = 0.0;
	result.at<T>(3, 3) = 1.0;
}

template void cedar::aux::math::expTwist<double>(const cv::Mat&, double, cv::Mat&);
template void cedar::aux::math::expTwist<float>(const cv::Mat&, double, cv::Mat&);

template<typename T>
cv::Mat cedar::aux::math::expTwist(const cv::Mat& xi, double theta)
{
  Mat result(4, 4, xi.type());
  expTwist<T>(xi, theta, result);
  return result;
}

template cv::Mat cedar::aux::math::expTwist<double>(const cv::Mat&, double);
template cv::Mat cedar::aux::math::expTwist<float>(const cv::Mat&, double);

/* ------------------------------------------------------------------------------------------------- */
template<typename T>
void cedar::aux::math::logTwist(const cv::Mat& transformation, cv::Mat& xi, double& theta, bool optionalThetaChoice)
{
	// extract rotation and translation
  Mat R = transformation(Rect(0, 0, 3, 3));
  Mat p = transformation(Rect(3, 0, 1, 3));

	// calculate exponential coordinates for R = exp(\wedge \omega \theta)
	Mat omega(3, 1, transformation.type());
	logAxis<T>(R, omega, theta, optionalThetaChoice);

	// calculate v
	Mat omegaWedge(3, 3, transformation.type());
  wedgeAxis<T>(omega, omegaWedge);
  Mat I(3, 3, transformation.type());
  setIdentity(I);
  Mat s1 = (I - R) * omegaWedge;
	Mat v = (s1 + omega * omega.t() * theta).inv() * p;

	// concatenate omega and v to get twist coordinates
	xi.at<T>(0, 0) = v.at<T>(0, 0);
	xi.at<T>(1, 0) = v.at<T>(1, 0);
	xi.at<T>(2, 0) = v.at<T>(2, 0);
	xi.at<T>(3, 0) = omega.at<T>(0, 0);
	xi.at<T>(4, 0) = omega.at<T>(1, 0);
	xi.at<T>(5, 0) = omega.at<T>(2, 0);
}

template void cedar::aux::math::logTwist<double>(const cv::Mat&, cv::Mat&, double&, bool);
template void cedar::aux::math::logTwist<float>(const cv::Mat&, cv::Mat&, double&, bool);

/* ------------------------------------------------------------------------------------------------- */
template<typename T>
void cedar::aux::math::rigidToAdjointTransformation(const cv::Mat& rigidTransformation, cv::Mat& adjointTransformation)
{
  // extract componenets
	Mat rot = rigidTransformation(Rect(0, 0, 3, 3));
	Mat pos = rigidTransformation(Rect(3, 0, 1, 3));
  Mat posWedge(3, 3, rigidTransformation.type());
  wedgeAxis<T>(pos, posWedge);
  Mat posWedgeTimesRot = posWedge * rot;

  // concatenate to form adjoint
  adjointTransformation = cv::Mat::zeros(6, 6, rigidTransformation.type());
	adjointTransformation.at<T>(0, 0) = rot.at<T>(0, 0);
	adjointTransformation.at<T>(0, 1) = rot.at<T>(0, 1);
	adjointTransformation.at<T>(0, 2) = rot.at<T>(0, 2);
	adjointTransformation.at<T>(1, 0) = rot.at<T>(1, 0);
	adjointTransformation.at<T>(1, 1) = rot.at<T>(1, 1);
	adjointTransformation.at<T>(1, 2) = rot.at<T>(1, 2);
	adjointTransformation.at<T>(2, 0) = rot.at<T>(2, 0);
	adjointTransformation.at<T>(2, 1) = rot.at<T>(2, 1);
	adjointTransformation.at<T>(2, 2) = rot.at<T>(2, 2);

	adjointTransformation.at<T>(0, 3) = posWedgeTimesRot.at<T>(0, 0);
	adjointTransformation.at<T>(0, 4) = posWedgeTimesRot.at<T>(0, 1);
	adjointTransformation.at<T>(0, 5) = posWedgeTimesRot.at<T>(0, 2);
	adjointTransformation.at<T>(1, 3) = posWedgeTimesRot.at<T>(1, 0);
	adjointTransformation.at<T>(1, 4) = posWedgeTimesRot.at<T>(1, 1);
	adjointTransformation.at<T>(1, 5) = posWedgeTimesRot.at<T>(1, 2);
	adjointTransformation.at<T>(2, 3) = posWedgeTimesRot.at<T>(2, 0);
	adjointTransformation.at<T>(2, 4) = posWedgeTimesRot.at<T>(2, 1);
	adjointTransformation.at<T>(2, 5) = posWedgeTimesRot.at<T>(2, 2);

	adjointTransformation.at<T>(3, 3) = rot.at<T>(0, 0);
	adjointTransformation.at<T>(3, 4) = rot.at<T>(0, 1);
	adjointTransformation.at<T>(3, 5) = rot.at<T>(0, 2);
	adjointTransformation.at<T>(4, 3) = rot.at<T>(1, 0);
	adjointTransformation.at<T>(4, 4) = rot.at<T>(1, 1);
	adjointTransformation.at<T>(4, 5) = rot.at<T>(1, 2);
	adjointTransformation.at<T>(5, 3) = rot.at<T>(2, 0);
	adjointTransformation.at<T>(5, 4) = rot.at<T>(2, 1);
	adjointTransformation.at<T>(5, 5) = rot.at<T>(2, 2);
}

template void cedar::aux::math::rigidToAdjointTransformation<double>(const cv::Mat&, cv::Mat&);
template void cedar::aux::math::rigidToAdjointTransformation<float>(const cv::Mat&, cv::Mat&);

template<typename T>
cv::Mat cedar::aux::math::rigidToAdjointTransformation(const cv::Mat& rigidTransformation)
{
  Mat result = Mat::zeros(6, 6, rigidTransformation.type());
  rigidToAdjointTransformation<T>(rigidTransformation, result);
  return result;
}

template cv::Mat cedar::aux::math::rigidToAdjointTransformation<double>(const cv::Mat&);
template cv::Mat cedar::aux::math::rigidToAdjointTransformation<float>(const cv::Mat&);

/* ------------------------------------------------------------------------------------------------- */
template<typename T>
void cedar::aux::math::adjointToRigidTransformation(const cv::Mat& adjointTransformation, cv::Mat& rigidTransformation)
{
  Mat R = adjointTransformation(Rect(0, 0, 3, 3));
  Mat pWedgeTimesR = adjointTransformation(Rect(3, 0, 3, 3));
  Mat pWedge = pWedgeTimesR * R.inv();
  Mat p(3, 1, adjointTransformation.type());
  veeAxis<T>(pWedge, p);

	rigidTransformation.at<T>(0, 0) = R.at<T>(0, 0);
	rigidTransformation.at<T>(0, 1) = R.at<T>(0, 1);
	rigidTransformation.at<T>(0, 2) = R.at<T>(0, 2);
	rigidTransformation.at<T>(1, 0) = R.at<T>(1, 0);
	rigidTransformation.at<T>(1, 1) = R.at<T>(1, 1);
	rigidTransformation.at<T>(1, 2) = R.at<T>(1, 2);
	rigidTransformation.at<T>(2, 0) = R.at<T>(2, 0);
	rigidTransformation.at<T>(2, 1) = R.at<T>(2, 1);
	rigidTransformation.at<T>(2, 2) = R.at<T>(2, 2);

	rigidTransformation.at<T>(0, 3) = p.at<T>(0, 0);
	rigidTransformation.at<T>(1, 3) = p.at<T>(1, 0);
	rigidTransformation.at<T>(2, 3) = p.at<T>(2, 0);

	rigidTransformation.at<T>(3, 0) = 0;
	rigidTransformation.at<T>(3, 1) = 0;
	rigidTransformation.at<T>(3, 2) = 0;
	rigidTransformation.at<T>(3, 3) = 1;
}

template void cedar::aux::math::adjointToRigidTransformation<double>(const cv::Mat&, cv::Mat&);
template void cedar::aux::math::adjointToRigidTransformation<float>(const cv::Mat&, cv::Mat&);

template<typename T>
cv::Mat cedar::aux::math::adjointToRigidTransformation(const cv::Mat& adjointTransformation)
{
  Mat result(4, 4, adjointTransformation.type());
  adjointToRigidTransformation<T>(adjointTransformation, result);
  return result;
}

template cv::Mat cedar::aux::math::adjointToRigidTransformation<double>(const cv::Mat&);
template cv::Mat cedar::aux::math::adjointToRigidTransformation<float>(const cv::Mat&);

/* ------------------------------------------------------------------------------------------------- */
template<typename T>
void cedar::aux::math::invertAdjointTransformation(const cv::Mat& adjointTransformation, cv::Mat& inverse)
{
  Mat rot = adjointTransformation(Rect(0, 0, 3, 3));
  Mat posWedgeTimesRot = adjointTransformation(Rect(3, 0, 3, 3));
  Mat rotTranspose = rot.t();
	Mat upperRight = - rotTranspose * posWedgeTimesRot * rotTranspose;

	inverse *= 0;

	inverse.at<T>(0, 0) = rotTranspose.at<T>(0, 0);
	inverse.at<T>(0, 1) = rotTranspose.at<T>(0, 1);
	inverse.at<T>(0, 2) = rotTranspose.at<T>(0, 2);
	inverse.at<T>(1, 0) = rotTranspose.at<T>(1, 0);
	inverse.at<T>(1, 1) = rotTranspose.at<T>(1, 1);
	inverse.at<T>(1, 2) = rotTranspose.at<T>(1, 2);
	inverse.at<T>(2, 0) = rotTranspose.at<T>(2, 0);
	inverse.at<T>(2, 1) = rotTranspose.at<T>(2, 1);
	inverse.at<T>(2, 2) = rotTranspose.at<T>(2, 2);

	inverse.at<T>(0, 3) = upperRight.at<T>(0, 0);
	inverse.at<T>(0, 4) = upperRight.at<T>(0, 1);
	inverse.at<T>(0, 5) = upperRight.at<T>(0, 2);
	inverse.at<T>(1, 3) = upperRight.at<T>(1, 0);
	inverse.at<T>(1, 4) = upperRight.at<T>(1, 1);
	inverse.at<T>(1, 5) = upperRight.at<T>(1, 2);
	inverse.at<T>(2, 3) = upperRight.at<T>(2, 0);
	inverse.at<T>(2, 4) = upperRight.at<T>(2, 1);
	inverse.at<T>(2, 5) = upperRight.at<T>(2, 2);

	inverse.at<T>(3, 3) = rotTranspose.at<T>(0, 0);
	inverse.at<T>(3, 4) = rotTranspose.at<T>(0, 1);
	inverse.at<T>(3, 5) = rotTranspose.at<T>(0, 2);
	inverse.at<T>(4, 3) = rotTranspose.at<T>(1, 0);
	inverse.at<T>(4, 4) = rotTranspose.at<T>(1, 1);
	inverse.at<T>(4, 5) = rotTranspose.at<T>(1, 2);
	inverse.at<T>(5, 3) = rotTranspose.at<T>(2, 0);
	inverse.at<T>(5, 4) = rotTranspose.at<T>(2, 1);
	inverse.at<T>(5, 5) = rotTranspose.at<T>(2, 2);

}

template void cedar::aux::math::invertAdjointTransformation<double>(const cv::Mat&, cv::Mat&);
template void cedar::aux::math::invertAdjointTransformation<float>(const cv::Mat&, cv::Mat&);

template<typename T>
cv::Mat cedar::aux::math::invertAdjointTransformation(const cv::Mat& adjointTransformation)
{
  Mat result(6, 6, adjointTransformation.type());
  invertAdjointTransformation<T>(adjointTransformation, result);
  return result;
}

template cv::Mat cedar::aux::math::invertAdjointTransformation<double>(const cv::Mat&);
template cv::Mat cedar::aux::math::invertAdjointTransformation<float>(const cv::Mat&);

/* ------------------------------------------------------------------------------------------------- */
template<typename T>
cv::Mat cedar::aux::math::twistCoordinates(const cv::Mat& supportPoint, const cv::Mat& axis)
{
	Mat twist(6, 1, supportPoint.type());
	Mat omega = axis(Rect(0, 0, 1, 3)) * (1 / norm(axis(Rect(0, 0, 1, 3))));
  Mat cross = supportPoint(Rect(0, 0, 1, 3)).cross(omega);
	twist.at<double>(0, 0) = cross.at<double>(0, 0);
	twist.at<double>(1, 0) = cross.at<double>(1, 0);
	twist.at<double>(2, 0) = cross.at<double>(2, 0);
	twist.at<double>(3, 0) = omega.at<double>(0, 0);
	twist.at<double>(4, 0) = omega.at<double>(1, 0);
	twist.at<double>(5, 0) = omega.at<double>(2, 0);
	return twist;
}

template cv::Mat cedar::aux::math::twistCoordinates<double>(const cv::Mat&, const cv::Mat&);
template cv::Mat cedar::aux::math::twistCoordinates<float>(const cv::Mat&, const cv::Mat&);
