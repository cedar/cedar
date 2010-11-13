/*--------------------------------------------------------------------------
 ----- Institute:   Ruhr-Universitaet-Bochum, Institut fuer Neuroinformatik

 ----- File:        rgScrewCalculus.cpp
 
 ----- Author:      Hendrik Reimann
 
 ----- Date:        2010 11 01
 
 ----- Description:

 ----- Credits:
 --------------------------------------------------------------------------*/

// LOCAL INCLUDES (includes from this project)
#include "screwCalculus.h"

// PROJECT INCLUDES (includes from other projects)

// SYSTEM INCLUDES (everything else)

using namespace std;
using namespace cv;

/* epsilon surrounding for near zero values */
#ifndef EQN_EPS_LOC
#define EQN_EPS_LOC 1e-7
#endif

#ifndef IsZeroLoc(x)
#define	IsZeroLoc(x)	((x) > -EQN_EPS_LOC && (x) < EQN_EPS_LOC)
#endif

/* ------------------------------------------------------------------------------------------------- */
template<typename T>
void cedar::aux::math::wedgeRotation(const cv::Mat& vector, cv::Mat& result)
{
	result.at<T>(0, 0) = 0;
	result.at<T>(0, 1) = -vector.at<T>(2, 0);
	result.at<T>(0, 2) = vector.at<T>(1, 0);
	result.at<T>(1, 0) = vector.at<T>(2, 0);
	result.at<T>(1, 1) = 0;
	result.at<T>(1, 2) = -vector.at<T>(0, 0);
	result.at<T>(2, 0) = -vector.at<T>(1, 0);
	result.at<T>(2, 1) = vector.at<T>(0, 0);
	result.at<T>(2, 2) = 0;
}

template void cedar::aux::math::wedgeRotation<double>(const cv::Mat&, cv::Mat&);
template void cedar::aux::math::wedgeRotation<float>(const cv::Mat&, cv::Mat&);

template<typename T>
cv::Mat cedar::aux::math::wedgeRotation(const cv::Mat& vector)
{
  cv::Mat result(3, 3, vector.type());
  wedgeRotation<T>(vector, result);
  return result;
}

template cv::Mat cedar::aux::math::wedgeRotation<double>(const cv::Mat&);
template cv::Mat cedar::aux::math::wedgeRotation<float>(const cv::Mat&);

/* ------------------------------------------------------------------------------------------------- */
template<typename T>
void cedar::aux::math::veeRotation(const cv::Mat& matrix, cv::Mat& result)
{
	result.at<T>(0, 0) = matrix.at<T>(2, 1);
	result.at<T>(1, 0) = matrix.at<T>(0, 2);
	result.at<T>(2, 0) = matrix.at<T>(1, 0);
}

template void cedar::aux::math::veeRotation<double>(const cv::Mat&, cv::Mat&);
template void cedar::aux::math::veeRotation<float>(const cv::Mat&, cv::Mat&);

template<typename T>
cv::Mat cedar::aux::math::veeRotation(const cv::Mat& matrix)
{
  cv::Mat result(3, 1, matrix.type());
  veeRotation<T>(matrix, result);
  return result;
}

template cv::Mat cedar::aux::math::veeRotation<double>(const cv::Mat&);
template cv::Mat cedar::aux::math::veeRotation<float>(const cv::Mat&);

/* ------------------------------------------------------------------------------------------------- */
//void wedgeRigidTransformation(cv::Mat& twist, cv::Mat& result)
//{
//	result.at<double>(0, 0) = 0;
//	result.at<double>(0, 1) = -twist.at<double>(5, 0);
//	result.at<double>(0, 2) = twist.at<double>(4, 0);
//	result.at<double>(1, 0) = twist.at<double>(5, 0);
//	result.at<double>(1, 1) = 0;
//	result.at<double>(1, 2) = -twist.at<double>(3, 0);
//	result.at<double>(2, 0) = -twist.at<double>(4, 0);
//	result.at<double>(2, 1) = twist.at<double>(3, 0);
//	result.at<double>(2, 2) = 0;
//
//	result.at<double>(0, 3) = twist.at<double>(0, 0);
//	result.at<double>(1, 3) = twist.at<double>(1, 0);
//	result.at<double>(2, 3) = twist.at<double>(2, 0);
//
//	result.at<double>(3, 0) = 0;
//	result.at<double>(3, 1) = 0;
//	result.at<double>(3, 2) = 0;
//	result.at<double>(3, 3) = 0;
//}
//
//cv::Mat wedgeRigidTransformation(cv::Mat& twist)
//{
//  Mat result(4, 4, CV_64FC1);
//  wedgeRigidTransformation(twist, result);
//  return result;
//}
//
//void veeRigidTransformation(cv::Mat& matrix, cv::Mat& result)
//{
//	// position
//	result.at<double>(0, 0) = matrix.at<double>(0, 3);
//	result.at<double>(1, 0) = matrix.at<double>(1, 3);
//	result.at<double>(2, 0) = matrix.at<double>(2, 3);
//	// axis
//	result.at<double>(3, 0) = matrix.at<double>(2, 1);
//	result.at<double>(4, 0) = matrix.at<double>(0, 2);
//	result.at<double>(5, 0) = matrix.at<double>(1, 0);
//}
//
//cv::Mat veeRigidTransformation(cv::Mat& matrix)
//{
//  Mat result(6, 1, CV_64FC1);
//  veeRigidTransformation(matrix, result);
//  return result;
//}
//
//void expRotation(cv::Mat& omega, double theta, cv::Mat& result)
//{
//	setIdentity(result);
//  cv::Mat omegaWedge(3, 3, CV_64FC1);
//	wedgeRotation(omega, omegaWedge);
//	result += (omegaWedge * sin(theta)) + (omegaWedge * omegaWedge * (1 - cos(theta)));
//}
//
//cv::Mat expRotation(cv::Mat& omega, double theta)
//{
//  Mat result(3, 3, CV_64FC1);
//  expRotation(omega, theta, result);
//  return result;
//}
//
//void logRotation(cv::Mat& rotation, cv::Mat& omega, double& theta, bool optionalThetaChoice)
//{
//	double trace = rotation.at<double>(0, 0) + rotation.at<double>(1, 1) + rotation.at<double>(2, 2);
//  // calculate rotation angle
//	theta = acos((trace - 1) / 2.0);
//  if (trace <= -1)
//  {
//    // capture numeric failures for trace very close to one
//    theta = M_PI;
//  }
//	if (optionalThetaChoice)
//  {
//    theta = 2*M_PI - theta;
//  }
//  // calculate axis of rotation
//	omega.at<double>(0, 0) = 1 / (2 * sin(theta)) * (rotation.at<double>(2, 1) - rotation.at<double>(1, 2));
//	omega.at<double>(1, 0) = 1 / (2 * sin(theta)) * (rotation.at<double>(0, 2) - rotation.at<double>(2, 0));
//	omega.at<double>(2, 0) = 1 / (2 * sin(theta)) * (rotation.at<double>(1, 0) - rotation.at<double>(0, 1));
//  omega = omega * (1 / norm(omega));
//
//  // TODO: IsZero() should be global precompiler macro in rgMathTools
//  if (IsZeroLoc(theta - M_PI))
//  {
//    // easy way of calculating axis fails for numerical reasons, choose different formula
//#ifdef DEBUG
//    cout << "    in function logRotation()" << endl;
//    cout << "    theta very close to pi, using different formula for omega." << endl;
//    cout << "    this case was not tested thoroughly, watch out for errors." << endl;
//#endif
//    if ((rotation.at<double>(1, 0) > 0) && (rotation.at<double>(2, 0) > 0) && (rotation.at<double>(2, 1) > 0))
//    {
//      omega.at<double>(0, 0) = + sqrt((rotation.at<double>(0, 0) - cos(theta)) / (1-cos(theta)));
//      omega.at<double>(1, 0) = + sqrt((rotation.at<double>(1, 1) - cos(theta)) / (1-cos(theta)));
//      omega.at<double>(2, 0) = + sqrt((rotation.at<double>(2, 2) - cos(theta)) / (1-cos(theta)));
//    }
//    if ((rotation.at<double>(1, 0) > 0) && (rotation.at<double>(2, 0) > 0) && (rotation.at<double>(2, 1) < 0))
//    {
//      omega.at<double>(0, 0) = + sqrt((rotation.at<double>(0, 0) - cos(theta)) / (1-cos(theta)));
//      omega.at<double>(1, 0) = + sqrt((rotation.at<double>(1, 1) - cos(theta)) / (1-cos(theta)));
//      omega.at<double>(2, 0) = - sqrt((rotation.at<double>(2, 2) - cos(theta)) / (1-cos(theta)));
//    }
//    if ((rotation.at<double>(1, 0) < 0) && (rotation.at<double>(2, 0) > 0) && (rotation.at<double>(2, 1) < 0))
//    {
//      omega.at<double>(0, 0) = + sqrt((rotation.at<double>(0, 0) - cos(theta)) / (1-cos(theta)));
//      omega.at<double>(1, 0) = - sqrt((rotation.at<double>(1, 1) - cos(theta)) / (1-cos(theta)));
//      omega.at<double>(2, 0) = + sqrt((rotation.at<double>(2, 2) - cos(theta)) / (1-cos(theta)));
//    }
//    if ((rotation.at<double>(1, 0) < 0) && (rotation.at<double>(2, 0) < 0) && (rotation.at<double>(2, 1) > 0))
//    {
//      omega.at<double>(0, 0) = + sqrt((rotation.at<double>(0, 0) - cos(theta)) / (1-cos(theta)));
//      omega.at<double>(1, 0) = - sqrt((rotation.at<double>(1, 1) - cos(theta)) / (1-cos(theta)));
//      omega.at<double>(2, 0) = - sqrt((rotation.at<double>(2, 2) - cos(theta)) / (1-cos(theta)));
//    }
//  }
//}
//
//void expRigidTransformation(cv::Mat& xi, double theta, cv::Mat& result)
//{
//  Mat v = xi(Rect(0, 0, 1, 3));
//  Mat omega = xi(Rect(0, 3, 1, 3));
//
//	// rotation
//  cv::Mat R(3, 3, CV_64FC1);
//	expRotation(omega, theta, R);
//	cv::Mat p;
//
//  // translation
//	if (norm(omega) == 0) // pure translation
//	{
//		p = v * theta;
//	}
//	else                      // translation and rotation
//	{
//		// p = (eye(3,3) - r)*(cross(w,v)) + w*w'*v*theta <-- matlab code
//		cv::Mat I(3, 3, CV_64FC1);
//		setIdentity(I);
//    cv::Mat s1 = (I - R) * omega.cross(v);
//    p = s1 + (omega * omega.t() * v * theta);
//	}
//	// join to form rigid transformation matrix
//	// a = [ R p
//	//			 0 1 ]
//	result.at<double>(0, 0) = R.at<double>(0, 0);
//	result.at<double>(0, 1) = R.at<double>(0, 1);
//	result.at<double>(0, 2) = R.at<double>(0, 2);
//	result.at<double>(1, 0) = R.at<double>(1, 0);
//	result.at<double>(1, 1) = R.at<double>(1, 1);
//	result.at<double>(1, 2) = R.at<double>(1, 2);
//	result.at<double>(2, 0) = R.at<double>(2, 0);
//	result.at<double>(2, 1) = R.at<double>(2, 1);
//	result.at<double>(2, 2) = R.at<double>(2, 2);
//
//	result.at<double>(0, 3) = p.at<double>(0, 0);
//	result.at<double>(1, 3) = p.at<double>(1, 0);
//	result.at<double>(2, 3) = p.at<double>(2, 0);
//
//	result.at<double>(3, 0) = 0;
//	result.at<double>(3, 1) = 0;
//	result.at<double>(3, 2) = 0;
//	result.at<double>(3, 3) = 1;
//}
//
//cv::Mat expRigidTransformation(cv::Mat& xi, double theta)
//{
//  Mat result(4, 4, CV_64FC1);
//  expRigidTransformation(xi, theta, result);
//  return result;
//}
//
//void logRigidTransformation(cv::Mat& transformation, cv::Mat& xi, double& theta, bool optionalThetaChoice)
//{
//	// extract rotation and translation
//  Mat R = transformation(Rect(0, 0, 3, 3));
//  Mat p = transformation(Rect(3, 0, 1, 3));
//
//	// calculate exponential coordinates for R = exp(\wedge \omega \theta)
//	Mat omega(3, 1, CV_64FC1);
//	logRotation(R, omega, theta, optionalThetaChoice);
//
//	// calculate v
//	Mat omegaWedge(3, 3, CV_64FC1);
//  wedgeRotation(omega, omegaWedge);
//  Mat I(3, 3, CV_64FC1);
//  setIdentity(I);
//  Mat s1 = (I - R) * omegaWedge;
//	Mat v = (s1 + omega * omega.t() * theta).inv() * p;
//
//	// concatenate omega and v to get twist coordinates
//	xi.at<double>(0, 0) = v.at<double>(0, 0);
//	xi.at<double>(1, 0) = v.at<double>(1, 0);
//	xi.at<double>(2, 0) = v.at<double>(2, 0);
//	xi.at<double>(3, 0) = omega.at<double>(0, 0);
//	xi.at<double>(4, 0) = omega.at<double>(1, 0);
//	xi.at<double>(5, 0) = omega.at<double>(2, 0);
//}
//
//void rigidToAdjointTransformation(cv::Mat& rigidTransformation, cv::Mat& adjointTransformation)
//{
//  // extract componenets
//	Mat rot = rigidTransformation(Rect(0, 0, 3, 3));
//	Mat pos = rigidTransformation(Rect(3, 0, 1, 3));
//  Mat posWedge(3, 3, CV_64FC1);
//  wedgeRotation(pos, posWedge);
//  Mat posWedgeTimesRot = posWedge * rot;
//
//  // concatenate to form adjoint
//	adjointTransformation = Mat::zeros(6, 6, CV_64FC1);
//
//	adjointTransformation.at<double>(0, 0) = rot.at<double>(0, 0);
//	adjointTransformation.at<double>(0, 1) = rot.at<double>(0, 1);
//	adjointTransformation.at<double>(0, 2) = rot.at<double>(0, 2);
//	adjointTransformation.at<double>(1, 0) = rot.at<double>(1, 0);
//	adjointTransformation.at<double>(1, 1) = rot.at<double>(1, 1);
//	adjointTransformation.at<double>(1, 2) = rot.at<double>(1, 2);
//	adjointTransformation.at<double>(2, 0) = rot.at<double>(2, 0);
//	adjointTransformation.at<double>(2, 1) = rot.at<double>(2, 1);
//	adjointTransformation.at<double>(2, 2) = rot.at<double>(2, 2);
//
//	adjointTransformation.at<double>(0, 3) = posWedgeTimesRot.at<double>(0, 0);
//	adjointTransformation.at<double>(0, 4) = posWedgeTimesRot.at<double>(0, 1);
//	adjointTransformation.at<double>(0, 5) = posWedgeTimesRot.at<double>(0, 2);
//	adjointTransformation.at<double>(1, 3) = posWedgeTimesRot.at<double>(1, 0);
//	adjointTransformation.at<double>(1, 4) = posWedgeTimesRot.at<double>(1, 1);
//	adjointTransformation.at<double>(1, 5) = posWedgeTimesRot.at<double>(1, 2);
//	adjointTransformation.at<double>(2, 3) = posWedgeTimesRot.at<double>(2, 0);
//	adjointTransformation.at<double>(2, 4) = posWedgeTimesRot.at<double>(2, 1);
//	adjointTransformation.at<double>(2, 5) = posWedgeTimesRot.at<double>(2, 2);
//
//	adjointTransformation.at<double>(3, 3) = rot.at<double>(0, 0);
//	adjointTransformation.at<double>(3, 4) = rot.at<double>(0, 1);
//	adjointTransformation.at<double>(3, 5) = rot.at<double>(0, 2);
//	adjointTransformation.at<double>(4, 3) = rot.at<double>(1, 0);
//	adjointTransformation.at<double>(4, 4) = rot.at<double>(1, 1);
//	adjointTransformation.at<double>(4, 5) = rot.at<double>(1, 2);
//	adjointTransformation.at<double>(5, 3) = rot.at<double>(2, 0);
//	adjointTransformation.at<double>(5, 4) = rot.at<double>(2, 1);
//	adjointTransformation.at<double>(5, 5) = rot.at<double>(2, 2);
//}
//
//cv::Mat rigidToAdjointTransformation(cv::Mat& rigidTransformation)
//{
//  Mat result(6, 6, CV_64FC1);
//  rigidToAdjointTransformation(rigidTransformation, result);
//  return result;
//}
//
//void adjointToRigidTransformation(cv::Mat& adjointTransformation, cv::Mat& rigidTransformation)
//{
//  Mat R = adjointTransformation(Rect(0, 0, 3, 3));
//  Mat pWedgeTimesR = adjointTransformation(Rect(3, 0, 3, 3));
//  Mat pWedge = pWedgeTimesR * R.inv();
//  Mat p(3, 1, CV_64FC1);
//  veeRotation(pWedge, p);
//
//	rigidTransformation.at<double>(0, 0) = R.at<double>(0, 0);
//	rigidTransformation.at<double>(0, 1) = R.at<double>(0, 1);
//	rigidTransformation.at<double>(0, 2) = R.at<double>(0, 2);
//	rigidTransformation.at<double>(1, 0) = R.at<double>(1, 0);
//	rigidTransformation.at<double>(1, 1) = R.at<double>(1, 1);
//	rigidTransformation.at<double>(1, 2) = R.at<double>(1, 2);
//	rigidTransformation.at<double>(2, 0) = R.at<double>(2, 0);
//	rigidTransformation.at<double>(2, 1) = R.at<double>(2, 1);
//	rigidTransformation.at<double>(2, 2) = R.at<double>(2, 2);
//
//	rigidTransformation.at<double>(0, 3) = p.at<double>(0, 0);
//	rigidTransformation.at<double>(1, 3) = p.at<double>(1, 0);
//	rigidTransformation.at<double>(2, 3) = p.at<double>(2, 0);
//
//	rigidTransformation.at<double>(3, 0) = 0;
//	rigidTransformation.at<double>(3, 1) = 0;
//	rigidTransformation.at<double>(3, 2) = 0;
//	rigidTransformation.at<double>(3, 3) = 1;
//}
//
//cv::Mat adjointToRigidTransformation(cv::Mat& adjointTransformation)
//{
//  Mat result(4, 4, CV_64FC1);
//  adjointToRigidTransformation(adjointTransformation, result);
//  return result;
//}
//
//void invertAdjointTransformation(cv::Mat& adjointTransformation, cv::Mat& inverse)
//{
//  Mat rot = adjointTransformation(Rect(0, 0, 3, 3));
//  Mat posWedgeTimesRot = adjointTransformation(Rect(3, 0, 3, 3));
//  Mat rotTranspose = rot.t();
//	Mat upperRight = - rotTranspose * posWedgeTimesRot * rotTranspose;
//
//	inverse = Mat::zeros(6, 6, CV_64FC1);
//
//	inverse.at<double>(0, 0) = rotTranspose.at<double>(0, 0);
//	inverse.at<double>(0, 1) = rotTranspose.at<double>(0, 1);
//	inverse.at<double>(0, 2) = rotTranspose.at<double>(0, 2);
//	inverse.at<double>(1, 0) = rotTranspose.at<double>(1, 0);
//	inverse.at<double>(1, 1) = rotTranspose.at<double>(1, 1);
//	inverse.at<double>(1, 2) = rotTranspose.at<double>(1, 2);
//	inverse.at<double>(2, 0) = rotTranspose.at<double>(2, 0);
//	inverse.at<double>(2, 1) = rotTranspose.at<double>(2, 1);
//	inverse.at<double>(2, 2) = rotTranspose.at<double>(2, 2);
//
//	inverse.at<double>(0, 3) = upperRight.at<double>(0, 0);
//	inverse.at<double>(0, 4) = upperRight.at<double>(0, 1);
//	inverse.at<double>(0, 5) = upperRight.at<double>(0, 2);
//	inverse.at<double>(1, 3) = upperRight.at<double>(1, 0);
//	inverse.at<double>(1, 4) = upperRight.at<double>(1, 1);
//	inverse.at<double>(1, 5) = upperRight.at<double>(1, 2);
//	inverse.at<double>(2, 3) = upperRight.at<double>(2, 0);
//	inverse.at<double>(2, 4) = upperRight.at<double>(2, 1);
//	inverse.at<double>(2, 5) = upperRight.at<double>(2, 2);
//
//	inverse.at<double>(3, 3) = rotTranspose.at<double>(0, 0);
//	inverse.at<double>(3, 4) = rotTranspose.at<double>(0, 1);
//	inverse.at<double>(3, 5) = rotTranspose.at<double>(0, 2);
//	inverse.at<double>(4, 3) = rotTranspose.at<double>(1, 0);
//	inverse.at<double>(4, 4) = rotTranspose.at<double>(1, 1);
//	inverse.at<double>(4, 5) = rotTranspose.at<double>(1, 2);
//	inverse.at<double>(5, 3) = rotTranspose.at<double>(2, 0);
//	inverse.at<double>(5, 4) = rotTranspose.at<double>(2, 1);
//	inverse.at<double>(5, 5) = rotTranspose.at<double>(2, 2);
//
//}
//
//cv::Mat invertAdjointTransformation(cv::Mat& adjointTransformation)
//{
//  Mat result(6, 6, CV_64FC1);
//  invertAdjointTransformation(adjointTransformation, result);
//  return result;
//}
//
//cv::Mat createGeneralizedInertiaMatrix(double mass, double Ix, double Iy, double Iz)
//{
//	Mat inertiaMatrix = Mat::zeros(6, 6, CV_64FC1);
//	inertiaMatrix.at<double>(0, 0) = mass;
//	inertiaMatrix.at<double>(1, 1) = mass;
//	inertiaMatrix.at<double>(2, 2) = mass;
//	inertiaMatrix.at<double>(3, 3) = Ix;
//	inertiaMatrix.at<double>(4, 4) = Iy;
//	inertiaMatrix.at<double>(5, 5) = Iz;
//  return inertiaMatrix;
//}
//
//cv::Mat createGeneralizedInertiaMatrixBlockMass(double mass, double length, double width, double height)
//{
//	double Ix = mass/12 * (width*width + height*height);
//	double Iy = mass/12 * (length*length + height*height);
//	double Iz = mass/12 * (width*width + length*length);
//  return createGeneralizedInertiaMatrix(mass, Ix, Iy, Iz);
//}
//
//cv::Mat twistCoordinates(cv::Mat& supportPoint, cv::Mat& axis)
//{
//	Mat twist(6, 1, CV_64FC1);
//	Mat omega = axis(Rect(0, 0, 1, 3)) * (1 / norm(axis(Rect(0, 0, 1, 3))));
//  Mat cross = supportPoint(Rect(0, 0, 1, 3)).cross(omega);
//	twist.at<double>(0, 0) = cross.at<double>(0, 0);
//	twist.at<double>(1, 0) = cross.at<double>(1, 0);
//	twist.at<double>(2, 0) = cross.at<double>(2, 0);
//	twist.at<double>(3, 0) = omega.at<double>(0, 0);
//	twist.at<double>(4, 0) = omega.at<double>(1, 0);
//	twist.at<double>(5, 0) = omega.at<double>(2, 0);
//	return twist;
//
//}































