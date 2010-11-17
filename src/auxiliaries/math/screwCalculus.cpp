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

// /TODO: shorten code
// /TODO: avoid allocation of memory wherever unnecessary


//----------------------------------------------------------------------------------------------------------------------
// wedge functions for axis vectors
//----------------------------------------------------------------------------------------------------------------------
template<typename T>
void cedar::aux::math::wedgeAxis(const cv::Mat& rAxis, cv::Mat& rResult)
{
	rResult.at<T>(0, 0) = 0.0;
	rResult.at<T>(0, 1) = -rAxis.at<T>(2, 0);
	rResult.at<T>(0, 2) = rAxis.at<T>(1, 0);
	rResult.at<T>(1, 0) = rAxis.at<T>(2, 0);
	rResult.at<T>(1, 1) = 0.0;
	rResult.at<T>(1, 2) = -rAxis.at<T>(0, 0);
	rResult.at<T>(2, 0) = -rAxis.at<T>(1, 0);
	rResult.at<T>(2, 1) = rAxis.at<T>(0, 0);
	rResult.at<T>(2, 2) = 0.0;
}

template void cedar::aux::math::wedgeAxis<double>(const cv::Mat&, cv::Mat&);
template void cedar::aux::math::wedgeAxis<float>(const cv::Mat&, cv::Mat&);

template<typename T>
cv::Mat cedar::aux::math::wedgeAxis(const cv::Mat& rAxis)
{
  cv::Mat result(3, 3, rAxis.type());
  wedgeAxis<T>(rAxis, result);
  return result;
}

template cv::Mat cedar::aux::math::wedgeAxis<double>(const cv::Mat&);
template cv::Mat cedar::aux::math::wedgeAxis<float>(const cv::Mat&);

//----------------------------------------------------------------------------------------------------------------------
// vee functions for axis vectors
//----------------------------------------------------------------------------------------------------------------------
template<typename T>
void cedar::aux::math::veeAxis(const cv::Mat& rMatrix, cv::Mat& rResult)
{
	rResult.at<T>(0, 0) = rMatrix.at<T>(2, 1);
	rResult.at<T>(1, 0) = rMatrix.at<T>(0, 2);
	rResult.at<T>(2, 0) = rMatrix.at<T>(1, 0);
}

template void cedar::aux::math::veeAxis<double>(const cv::Mat&, cv::Mat&);
template void cedar::aux::math::veeAxis<float>(const cv::Mat&, cv::Mat&);

template<typename T>
cv::Mat cedar::aux::math::veeAxis(const cv::Mat& rMatrix)
{
  cv::Mat result = cv::Mat::zeros(3, 1, rMatrix.type());
  veeAxis<T>(rMatrix, result);
  return result;
}

template cv::Mat cedar::aux::math::veeAxis<double>(const cv::Mat&);
template cv::Mat cedar::aux::math::veeAxis<float>(const cv::Mat&);

//----------------------------------------------------------------------------------------------------------------------
// wedge functions for twists
//----------------------------------------------------------------------------------------------------------------------
template<typename T>
void cedar::aux::math::wedgeTwist(const cv::Mat& rTwist, cv::Mat& rResult)
{
	rResult.at<T>(0, 0) = 0.0;
	rResult.at<T>(0, 1) = -rTwist.at<T>(5, 0);
	rResult.at<T>(0, 2) = rTwist.at<T>(4, 0);
	rResult.at<T>(1, 0) = rTwist.at<T>(5, 0);
	rResult.at<T>(1, 1) = 0.0;
	rResult.at<T>(1, 2) = -rTwist.at<T>(3, 0);
	rResult.at<T>(2, 0) = -rTwist.at<T>(4, 0);
	rResult.at<T>(2, 1) = rTwist.at<T>(3, 0);
	rResult.at<T>(2, 2) = 0.0;

	rResult.at<T>(0, 3) = rTwist.at<T>(0, 0);
	rResult.at<T>(1, 3) = rTwist.at<T>(1, 0);
	rResult.at<T>(2, 3) = rTwist.at<T>(2, 0);

	rResult.at<T>(3, 0) = 0.0;
	rResult.at<T>(3, 1) = 0.0;
	rResult.at<T>(3, 2) = 0.0;
	rResult.at<T>(3, 3) = 0.0;
}

template void cedar::aux::math::wedgeTwist<double>(const cv::Mat&, cv::Mat&);
template void cedar::aux::math::wedgeTwist<float>(const cv::Mat&, cv::Mat&);

template<typename T>
cv::Mat cedar::aux::math::wedgeTwist(const cv::Mat& rTwist)
{
  Mat result(4, 4, rTwist.type());
  wedgeTwist<T>(rTwist, result);
  return result;
}

template cv::Mat cedar::aux::math::wedgeTwist<double>(const cv::Mat&);
template cv::Mat cedar::aux::math::wedgeTwist<float>(const cv::Mat&);

//----------------------------------------------------------------------------------------------------------------------
// vee functions for twists
//----------------------------------------------------------------------------------------------------------------------
template<typename T>
void cedar::aux::math::veeTwist(const cv::Mat& rMatrix, cv::Mat& rResult)
{
	// position
	rResult.at<T>(0, 0) = rMatrix.at<T>(0, 3);
	rResult.at<T>(1, 0) = rMatrix.at<T>(1, 3);
	rResult.at<T>(2, 0) = rMatrix.at<T>(2, 3);
	// axis
	rResult.at<T>(3, 0) = rMatrix.at<T>(2, 1);
	rResult.at<T>(4, 0) = rMatrix.at<T>(0, 2);
	rResult.at<T>(5, 0) = rMatrix.at<T>(1, 0);
}

template void cedar::aux::math::veeTwist<double>(const cv::Mat&, cv::Mat&);
template void cedar::aux::math::veeTwist<float>(const cv::Mat&, cv::Mat&);

template<typename T>
cv::Mat cedar::aux::math::veeTwist(const cv::Mat& rMatrix)
{
  Mat result = cv::Mat::zeros(6, 1, rMatrix.type());
  veeTwist<T>(rMatrix, result);
  return result;
}

template cv::Mat cedar::aux::math::veeTwist<double>(const cv::Mat&);
template cv::Mat cedar::aux::math::veeTwist<float>(const cv::Mat&);

//----------------------------------------------------------------------------------------------------------------------
// exp functions for axis vectors
//----------------------------------------------------------------------------------------------------------------------
template<typename T>
void cedar::aux::math::expAxis(const cv::Mat& rAxis, double theta, cv::Mat& rResult)
{
  setIdentity(rResult);
  cv::Mat omega_wedge = cv::Mat::zeros(3, 3, rAxis.type());
  wedgeAxis<T>(rAxis, omega_wedge);
  rResult += (omega_wedge * sin(theta)) + (omega_wedge * omega_wedge * (1 - cos(theta)));
}

template void cedar::aux::math::expAxis<double>(const cv::Mat&, double, cv::Mat&);
template void cedar::aux::math::expAxis<float>(const cv::Mat&, double, cv::Mat&);

template<typename T>
cv::Mat cedar::aux::math::expAxis(const cv::Mat& rAxis, double theta)
{
  Mat result(3, 3, rAxis.type());
  expAxis<T>(rAxis, theta, result);
  return result;
}

template cv::Mat cedar::aux::math::expAxis<double>(const cv::Mat&, double);
template cv::Mat cedar::aux::math::expAxis<float>(const cv::Mat&, double);

//----------------------------------------------------------------------------------------------------------------------
// log functions for axis vectors
//----------------------------------------------------------------------------------------------------------------------
template<typename T>
void cedar::aux::math::logAxis(const cv::Mat& rRotation, cv::Mat& rOmega, double& rTheta, bool optionalThetaChoice)
{
	T trace = rRotation.at<T>(0, 0) + rRotation.at<T>(1, 1) + rRotation.at<T>(2, 2);
  // calculate rotation angle
	rTheta = acos((trace - 1) / 2.0);
  if (trace <= -1)
  {
    // capture numeric failures for trace very close to one
    rTheta = M_PI;
  }
	if (optionalThetaChoice)
  {
    rTheta = 2*M_PI - rTheta;
  }
  // calculate axis of rotation
	rOmega.at<T>(0, 0) = 1 / (2 * sin(rTheta)) * (rRotation.at<T>(2, 1) - rRotation.at<T>(1, 2));
	rOmega.at<T>(1, 0) = 1 / (2 * sin(rTheta)) * (rRotation.at<T>(0, 2) - rRotation.at<T>(2, 0));
	rOmega.at<T>(2, 0) = 1 / (2 * sin(rTheta)) * (rRotation.at<T>(1, 0) - rRotation.at<T>(0, 1));
  rOmega = rOmega * (1 / norm(rOmega));

  if (IsZero(rTheta - M_PI))
  {
    // easy way of calculating axis fails for numerical reasons, choose different formula
    if ((rRotation.at<T>(1, 0) > 0) && (rRotation.at<T>(2, 0) > 0) && (rRotation.at<T>(2, 1) > 0))
    {
      rOmega.at<T>(0, 0) = + sqrt((rRotation.at<T>(0, 0) - cos(rTheta)) / (1-cos(rTheta)));
      rOmega.at<T>(1, 0) = + sqrt((rRotation.at<T>(1, 1) - cos(rTheta)) / (1-cos(rTheta)));
      rOmega.at<T>(2, 0) = + sqrt((rRotation.at<T>(2, 2) - cos(rTheta)) / (1-cos(rTheta)));
    }
    if ((rRotation.at<T>(1, 0) > 0) && (rRotation.at<T>(2, 0) > 0) && (rRotation.at<T>(2, 1) < 0))
    {
      rOmega.at<T>(0, 0) = + sqrt((rRotation.at<T>(0, 0) - cos(rTheta)) / (1-cos(rTheta)));
      rOmega.at<T>(1, 0) = + sqrt((rRotation.at<T>(1, 1) - cos(rTheta)) / (1-cos(rTheta)));
      rOmega.at<T>(2, 0) = - sqrt((rRotation.at<T>(2, 2) - cos(rTheta)) / (1-cos(rTheta)));
    }
    if ((rRotation.at<T>(1, 0) < 0) && (rRotation.at<T>(2, 0) > 0) && (rRotation.at<T>(2, 1) < 0))
    {
      rOmega.at<T>(0, 0) = + sqrt((rRotation.at<T>(0, 0) - cos(rTheta)) / (1-cos(rTheta)));
      rOmega.at<T>(1, 0) = - sqrt((rRotation.at<T>(1, 1) - cos(rTheta)) / (1-cos(rTheta)));
      rOmega.at<T>(2, 0) = + sqrt((rRotation.at<T>(2, 2) - cos(rTheta)) / (1-cos(rTheta)));
    }
    if ((rRotation.at<T>(1, 0) < 0) && (rRotation.at<T>(2, 0) < 0) && (rRotation.at<T>(2, 1) > 0))
    {
      rOmega.at<T>(0, 0) = + sqrt((rRotation.at<T>(0, 0) - cos(rTheta)) / (1-cos(rTheta)));
      rOmega.at<T>(1, 0) = - sqrt((rRotation.at<T>(1, 1) - cos(rTheta)) / (1-cos(rTheta)));
      rOmega.at<T>(2, 0) = - sqrt((rRotation.at<T>(2, 2) - cos(rTheta)) / (1-cos(rTheta)));
    }
  }
}

template void cedar::aux::math::logAxis<double>(const cv::Mat&, cv::Mat&, double&, bool);
template void cedar::aux::math::logAxis<float>(const cv::Mat&, cv::Mat&, double&, bool);

//----------------------------------------------------------------------------------------------------------------------
// exp functions for twists
//----------------------------------------------------------------------------------------------------------------------
template<typename T>
void cedar::aux::math::expTwist(const cv::Mat& rXi, double theta, cv::Mat& rResult)
{
  Mat v = rXi(Rect(0, 0, 1, 3));
  Mat omega = rXi(Rect(0, 3, 1, 3));

	// rotation
  Mat R(3, 3, rXi.type());
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
    cv::Mat I = cv::Mat::eye(3, 3, rXi.type());
    cv::Mat s1 = (I - R) * omega.cross(v);
    p = s1 + (omega * omega.t() * v * theta);
	}
	// join to form rigid transformation matrix
	// a = [ R p
	//			 0 1 ]
	rResult.at<T>(0, 0) = R.at<T>(0, 0);
	rResult.at<T>(0, 1) = R.at<T>(0, 1);
	rResult.at<T>(0, 2) = R.at<T>(0, 2);
	rResult.at<T>(1, 0) = R.at<T>(1, 0);
	rResult.at<T>(1, 1) = R.at<T>(1, 1);
	rResult.at<T>(1, 2) = R.at<T>(1, 2);
	rResult.at<T>(2, 0) = R.at<T>(2, 0);
	rResult.at<T>(2, 1) = R.at<T>(2, 1);
	rResult.at<T>(2, 2) = R.at<T>(2, 2);

	rResult.at<T>(0, 3) = p.at<T>(0, 0);
	rResult.at<T>(1, 3) = p.at<T>(1, 0);
	rResult.at<T>(2, 3) = p.at<T>(2, 0);

	rResult.at<T>(3, 0) = 0.0;
	rResult.at<T>(3, 1) = 0.0;
	rResult.at<T>(3, 2) = 0.0;
	rResult.at<T>(3, 3) = 1.0;
}

template void cedar::aux::math::expTwist<double>(const cv::Mat&, double, cv::Mat&);
template void cedar::aux::math::expTwist<float>(const cv::Mat&, double, cv::Mat&);

template<typename T>
cv::Mat cedar::aux::math::expTwist(const cv::Mat& rXi, double theta)
{
  Mat result = cv::Mat::zeros(4, 4, rXi.type());
  expTwist<T>(rXi, theta, result);
  return result;
}

template cv::Mat cedar::aux::math::expTwist<double>(const cv::Mat&, double);
template cv::Mat cedar::aux::math::expTwist<float>(const cv::Mat&, double);

//----------------------------------------------------------------------------------------------------------------------
// log functions for twists
//----------------------------------------------------------------------------------------------------------------------
template<typename T>
void cedar::aux::math::logTwist(const cv::Mat& rTransformation, cv::Mat& rXi, double& rTheta, bool optionalThetaChoice)
{
	// extract rotation and translation
  Mat R = rTransformation(Rect(0, 0, 3, 3));
  Mat p = rTransformation(Rect(3, 0, 1, 3));

	// calculate exponential coordinates for R = exp(\wedge \omega \theta)
	Mat omega(3, 1, rTransformation.type());
	logAxis<T>(R, omega, rTheta, optionalThetaChoice);

	// calculate v
	Mat omega_wedge = cv::Mat::zeros(3, 3, rTransformation.type());
  wedgeAxis<T>(omega, omega_wedge);
  Mat I = cv::Mat::eye(3, 3, rTransformation.type());
  Mat s1 = (I - R) * omega_wedge;
	Mat v = (s1 + omega_wedge * omega_wedge.t() * rTheta).inv() * p;

	// concatenate omega and v to get twist coordinates
	rXi.at<T>(0, 0) = v.at<T>(0, 0);
	rXi.at<T>(1, 0) = v.at<T>(1, 0);
	rXi.at<T>(2, 0) = v.at<T>(2, 0);
	rXi.at<T>(3, 0) = omega.at<T>(0, 0);
	rXi.at<T>(4, 0) = omega.at<T>(1, 0);
	rXi.at<T>(5, 0) = omega.at<T>(2, 0);
}

template void cedar::aux::math::logTwist<double>(const cv::Mat&, cv::Mat&, double&, bool);
template void cedar::aux::math::logTwist<float>(const cv::Mat&, cv::Mat&, double&, bool);

//----------------------------------------------------------------------------------------------------------------------
// rigid --> adjoint transformation
//----------------------------------------------------------------------------------------------------------------------
template<typename T>
void cedar::aux::math::rigidToAdjointTransformation(const cv::Mat& rRigidTransformation, cv::Mat& rAdjointTransformation)
{
  // extract componenets
	Mat rot = rRigidTransformation(Rect(0, 0, 3, 3));
	Mat pos = rRigidTransformation(Rect(3, 0, 1, 3));
  Mat pos_wedge(3, 3, rRigidTransformation.type());
  wedgeAxis<T>(pos, pos_wedge);
  Mat pos_wedge_times_rot = pos_wedge * rot;

  // concatenate to form adjoint
  rAdjointTransformation = cv::Mat::zeros(6, 6, rRigidTransformation.type());
	rAdjointTransformation.at<T>(0, 0) = rot.at<T>(0, 0);
	rAdjointTransformation.at<T>(0, 1) = rot.at<T>(0, 1);
	rAdjointTransformation.at<T>(0, 2) = rot.at<T>(0, 2);
	rAdjointTransformation.at<T>(1, 0) = rot.at<T>(1, 0);
	rAdjointTransformation.at<T>(1, 1) = rot.at<T>(1, 1);
	rAdjointTransformation.at<T>(1, 2) = rot.at<T>(1, 2);
	rAdjointTransformation.at<T>(2, 0) = rot.at<T>(2, 0);
	rAdjointTransformation.at<T>(2, 1) = rot.at<T>(2, 1);
	rAdjointTransformation.at<T>(2, 2) = rot.at<T>(2, 2);

	rAdjointTransformation.at<T>(0, 3) = pos_wedge_times_rot.at<T>(0, 0);
	rAdjointTransformation.at<T>(0, 4) = pos_wedge_times_rot.at<T>(0, 1);
	rAdjointTransformation.at<T>(0, 5) = pos_wedge_times_rot.at<T>(0, 2);
	rAdjointTransformation.at<T>(1, 3) = pos_wedge_times_rot.at<T>(1, 0);
	rAdjointTransformation.at<T>(1, 4) = pos_wedge_times_rot.at<T>(1, 1);
	rAdjointTransformation.at<T>(1, 5) = pos_wedge_times_rot.at<T>(1, 2);
	rAdjointTransformation.at<T>(2, 3) = pos_wedge_times_rot.at<T>(2, 0);
	rAdjointTransformation.at<T>(2, 4) = pos_wedge_times_rot.at<T>(2, 1);
	rAdjointTransformation.at<T>(2, 5) = pos_wedge_times_rot.at<T>(2, 2);

	rAdjointTransformation.at<T>(3, 3) = rot.at<T>(0, 0);
	rAdjointTransformation.at<T>(3, 4) = rot.at<T>(0, 1);
	rAdjointTransformation.at<T>(3, 5) = rot.at<T>(0, 2);
	rAdjointTransformation.at<T>(4, 3) = rot.at<T>(1, 0);
	rAdjointTransformation.at<T>(4, 4) = rot.at<T>(1, 1);
	rAdjointTransformation.at<T>(4, 5) = rot.at<T>(1, 2);
	rAdjointTransformation.at<T>(5, 3) = rot.at<T>(2, 0);
	rAdjointTransformation.at<T>(5, 4) = rot.at<T>(2, 1);
	rAdjointTransformation.at<T>(5, 5) = rot.at<T>(2, 2);
}

template void cedar::aux::math::rigidToAdjointTransformation<double>(const cv::Mat&, cv::Mat&);
template void cedar::aux::math::rigidToAdjointTransformation<float>(const cv::Mat&, cv::Mat&);

template<typename T>
cv::Mat cedar::aux::math::rigidToAdjointTransformation(const cv::Mat& rRigidTransformation)
{
  Mat result = Mat::zeros(6, 6, rRigidTransformation.type());
  rigidToAdjointTransformation<T>(rRigidTransformation, result);
  return result;
}

template cv::Mat cedar::aux::math::rigidToAdjointTransformation<double>(const cv::Mat&);
template cv::Mat cedar::aux::math::rigidToAdjointTransformation<float>(const cv::Mat&);

//----------------------------------------------------------------------------------------------------------------------
// adjoint --> rigid transformation
//----------------------------------------------------------------------------------------------------------------------
template<typename T>
void cedar::aux::math::adjointToRigidTransformation(const cv::Mat& rAdjointTransformation, cv::Mat& rRigidTransformation)
{
  Mat rot = rAdjointTransformation(Rect(0, 0, 3, 3));
  Mat pos_wedge_times_rot = rAdjointTransformation(Rect(3, 0, 3, 3));
  Mat pos_wedge = pos_wedge_times_rot * rot.inv();
  Mat pos = cv::Mat::zeros(3, 1, rAdjointTransformation.type());
  veeAxis<T>(pos_wedge, pos);

	rRigidTransformation.at<T>(0, 0) = rot.at<T>(0, 0);
	rRigidTransformation.at<T>(0, 1) = rot.at<T>(0, 1);
	rRigidTransformation.at<T>(0, 2) = rot.at<T>(0, 2);
	rRigidTransformation.at<T>(1, 0) = rot.at<T>(1, 0);
	rRigidTransformation.at<T>(1, 1) = rot.at<T>(1, 1);
	rRigidTransformation.at<T>(1, 2) = rot.at<T>(1, 2);
	rRigidTransformation.at<T>(2, 0) = rot.at<T>(2, 0);
	rRigidTransformation.at<T>(2, 1) = rot.at<T>(2, 1);
	rRigidTransformation.at<T>(2, 2) = rot.at<T>(2, 2);

	rRigidTransformation.at<T>(0, 3) = pos.at<T>(0, 0);
	rRigidTransformation.at<T>(1, 3) = pos.at<T>(1, 0);
	rRigidTransformation.at<T>(2, 3) = pos.at<T>(2, 0);

	rRigidTransformation.at<T>(3, 0) = 0;
	rRigidTransformation.at<T>(3, 1) = 0;
	rRigidTransformation.at<T>(3, 2) = 0;
	rRigidTransformation.at<T>(3, 3) = 1;
}

template void cedar::aux::math::adjointToRigidTransformation<double>(const cv::Mat&, cv::Mat&);
template void cedar::aux::math::adjointToRigidTransformation<float>(const cv::Mat&, cv::Mat&);

template<typename T>
cv::Mat cedar::aux::math::adjointToRigidTransformation(const cv::Mat& rAdjointTransformation)
{
  Mat result = cv::Mat::zeros(4, 4, rAdjointTransformation.type());
  adjointToRigidTransformation<T>(rAdjointTransformation, result);
  return result;
}

template cv::Mat cedar::aux::math::adjointToRigidTransformation<double>(const cv::Mat&);
template cv::Mat cedar::aux::math::adjointToRigidTransformation<float>(const cv::Mat&);

//----------------------------------------------------------------------------------------------------------------------
// inverse for adjoint transformations
//----------------------------------------------------------------------------------------------------------------------
template<typename T>
void cedar::aux::math::invertAdjointTransformation(const cv::Mat& rAdjointTransformation, cv::Mat& rInverse)
{
  Mat rot = rAdjointTransformation(Rect(0, 0, 3, 3));
  Mat pos_wedge_times_rot = rAdjointTransformation(Rect(3, 0, 3, 3));
  Mat rot_transpose = rot.t();
	Mat upper_right = - rot_transpose * pos_wedge_times_rot * rot_transpose;

	rInverse = cv::Mat::zeros(6, 6, rAdjointTransformation.type());

	rInverse.at<T>(0, 0) = rot_transpose.at<T>(0, 0);
	rInverse.at<T>(0, 1) = rot_transpose.at<T>(0, 1);
	rInverse.at<T>(0, 2) = rot_transpose.at<T>(0, 2);
	rInverse.at<T>(1, 0) = rot_transpose.at<T>(1, 0);
	rInverse.at<T>(1, 1) = rot_transpose.at<T>(1, 1);
	rInverse.at<T>(1, 2) = rot_transpose.at<T>(1, 2);
	rInverse.at<T>(2, 0) = rot_transpose.at<T>(2, 0);
	rInverse.at<T>(2, 1) = rot_transpose.at<T>(2, 1);
	rInverse.at<T>(2, 2) = rot_transpose.at<T>(2, 2);

	rInverse.at<T>(0, 3) = upper_right.at<T>(0, 0);
	rInverse.at<T>(0, 4) = upper_right.at<T>(0, 1);
	rInverse.at<T>(0, 5) = upper_right.at<T>(0, 2);
	rInverse.at<T>(1, 3) = upper_right.at<T>(1, 0);
	rInverse.at<T>(1, 4) = upper_right.at<T>(1, 1);
	rInverse.at<T>(1, 5) = upper_right.at<T>(1, 2);
	rInverse.at<T>(2, 3) = upper_right.at<T>(2, 0);
	rInverse.at<T>(2, 4) = upper_right.at<T>(2, 1);
	rInverse.at<T>(2, 5) = upper_right.at<T>(2, 2);

	rInverse.at<T>(3, 3) = rot_transpose.at<T>(0, 0);
	rInverse.at<T>(3, 4) = rot_transpose.at<T>(0, 1);
	rInverse.at<T>(3, 5) = rot_transpose.at<T>(0, 2);
	rInverse.at<T>(4, 3) = rot_transpose.at<T>(1, 0);
	rInverse.at<T>(4, 4) = rot_transpose.at<T>(1, 1);
	rInverse.at<T>(4, 5) = rot_transpose.at<T>(1, 2);
	rInverse.at<T>(5, 3) = rot_transpose.at<T>(2, 0);
	rInverse.at<T>(5, 4) = rot_transpose.at<T>(2, 1);
	rInverse.at<T>(5, 5) = rot_transpose.at<T>(2, 2);

}

template void cedar::aux::math::invertAdjointTransformation<double>(const cv::Mat&, cv::Mat&);
template void cedar::aux::math::invertAdjointTransformation<float>(const cv::Mat&, cv::Mat&);

template<typename T>
cv::Mat cedar::aux::math::invertAdjointTransformation(const cv::Mat& rAdjointTransformation)
{
  Mat result = cv::Mat::zeros(6, 6, rAdjointTransformation.type());
  invertAdjointTransformation<T>(rAdjointTransformation, result);
  return result;
}

template cv::Mat cedar::aux::math::invertAdjointTransformation<double>(const cv::Mat&);
template cv::Mat cedar::aux::math::invertAdjointTransformation<float>(const cv::Mat&);

//----------------------------------------------------------------------------------------------------------------------
// generation of matrix representations from basic information
//----------------------------------------------------------------------------------------------------------------------
template<typename T>
cv::Mat cedar::aux::math::twistCoordinates(const cv::Mat& rSupportPoint, const cv::Mat& rAxis)
{
	Mat twist = cv::Mat::zeros(6, 1, rSupportPoint.type());
	Mat omega = rAxis(Rect(0, 0, 1, 3)) * (1 / norm(rAxis(Rect(0, 0, 1, 3))));
  Mat cross = rSupportPoint(Rect(0, 0, 1, 3)).cross(omega);
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
