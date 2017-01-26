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

    File:        screwCalculus.cpp

    Maintainer:  Hendrik Reimann
    Email:       hendrik.reimann@ini.rub.de
    Date:        2010 11 11

    Description: This file provides screw calculus functions for rigid body geometry.

    Credits:     These functions are implementations of concepts taken from the book "A mathematical introduction to
                   robotic manipulation", by R. Murray, S. Sastry and Z. Lee

======================================================================================================================*/

// CEDAR INCLUDES
#include "cedar/auxiliaries/lib.h"
#include "cedar/auxiliaries/math/screwCalculus.h"
#include "cedar/auxiliaries/math/tools.h"
#include "cedar/auxiliaries/math/constants.h"

// SYSTEM INCLUDES


//----------------------------------------------------------------------------------------------------------------------
// wedge functions for axis vectors
//----------------------------------------------------------------------------------------------------------------------
template<typename T>
void cedar::aux::math::wedgeAxis(const cv::Mat& rAxis, cv::Mat& rResult)
{
  CEDAR_ASSERT(cedar::aux::math::matrixTypeCheck<T>(rAxis));
  CEDAR_ASSERT(cedar::aux::math::matrixTypeCheck<T>(rResult));

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

template CEDAR_AUX_LIB_EXPORT void cedar::aux::math::wedgeAxis<double>(const cv::Mat&, cv::Mat&);
template CEDAR_AUX_LIB_EXPORT void cedar::aux::math::wedgeAxis<float>(const cv::Mat&, cv::Mat&);

template<typename T>
cv::Mat cedar::aux::math::wedgeAxis(const cv::Mat& rAxis)
{
  CEDAR_ASSERT(cedar::aux::math::matrixTypeCheck<T>(rAxis));

  cv::Mat result = cv::Mat::zeros(3, 3, rAxis.type());
  wedgeAxis<T>(rAxis, result);
  return result;
}

template CEDAR_AUX_LIB_EXPORT cv::Mat cedar::aux::math::wedgeAxis<double>(const cv::Mat&);
template CEDAR_AUX_LIB_EXPORT cv::Mat cedar::aux::math::wedgeAxis<float>(const cv::Mat&);

//----------------------------------------------------------------------------------------------------------------------
// vee functions for axis vectors
//----------------------------------------------------------------------------------------------------------------------
template<typename T>
void cedar::aux::math::veeAxis(const cv::Mat& rMatrix, cv::Mat& rResult)
{
  CEDAR_ASSERT(cedar::aux::math::matrixTypeCheck<T>(rMatrix));
  CEDAR_ASSERT(cedar::aux::math::matrixTypeCheck<T>(rResult));

  rResult.at<T>(0, 0) = rMatrix.at<T>(2, 1);
  rResult.at<T>(1, 0) = rMatrix.at<T>(0, 2);
  rResult.at<T>(2, 0) = rMatrix.at<T>(1, 0);
}

template CEDAR_AUX_LIB_EXPORT void cedar::aux::math::veeAxis<double>(const cv::Mat&, cv::Mat&);
template CEDAR_AUX_LIB_EXPORT void cedar::aux::math::veeAxis<float>(const cv::Mat&, cv::Mat&);

template<typename T>
cv::Mat cedar::aux::math::veeAxis(const cv::Mat& rMatrix)
{
  CEDAR_ASSERT(cedar::aux::math::matrixTypeCheck<T>(rMatrix));

  cv::Mat result = cv::Mat::zeros(3, 1, rMatrix.type());
  veeAxis<T>(rMatrix, result);
  return result;
}

template CEDAR_AUX_LIB_EXPORT cv::Mat cedar::aux::math::veeAxis<double>(const cv::Mat&);
template CEDAR_AUX_LIB_EXPORT cv::Mat cedar::aux::math::veeAxis<float>(const cv::Mat&);

//----------------------------------------------------------------------------------------------------------------------
// wedge functions for twists
//----------------------------------------------------------------------------------------------------------------------
template<typename T>
void cedar::aux::math::wedgeTwist(const cv::Mat& rTwist, cv::Mat& rResult)
{
  CEDAR_ASSERT(cedar::aux::math::matrixTypeCheck<T>(rTwist));
  CEDAR_ASSERT(cedar::aux::math::matrixTypeCheck<T>(rResult));

  rResult = cv::Mat::zeros(4, 4, rTwist.type());
  rResult.at<T>(0, 1) = -rTwist.at<T>(5, 0);
  rResult.at<T>(0, 2) = rTwist.at<T>(4, 0);
  rResult.at<T>(1, 0) = rTwist.at<T>(5, 0);
  rResult.at<T>(1, 2) = -rTwist.at<T>(3, 0);
  rResult.at<T>(2, 0) = -rTwist.at<T>(4, 0);
  rResult.at<T>(2, 1) = rTwist.at<T>(3, 0);

  rResult.at<T>(0, 3) = rTwist.at<T>(0, 0);
  rResult.at<T>(1, 3) = rTwist.at<T>(1, 0);
  rResult.at<T>(2, 3) = rTwist.at<T>(2, 0);
}

template CEDAR_AUX_LIB_EXPORT void cedar::aux::math::wedgeTwist<double>(const cv::Mat&, cv::Mat&);
template CEDAR_AUX_LIB_EXPORT void cedar::aux::math::wedgeTwist<float>(const cv::Mat&, cv::Mat&);

template<typename T>
cv::Mat cedar::aux::math::wedgeTwist(const cv::Mat& rTwist)
{
  CEDAR_ASSERT(cedar::aux::math::matrixTypeCheck<T>(rTwist));

  cv::Mat result = cv::Mat::zeros(4, 4, rTwist.type());
  wedgeTwist<T>(rTwist, result);
  return result;
}

template CEDAR_AUX_LIB_EXPORT cv::Mat cedar::aux::math::wedgeTwist<double>(const cv::Mat&);
template CEDAR_AUX_LIB_EXPORT cv::Mat cedar::aux::math::wedgeTwist<float>(const cv::Mat&);

//----------------------------------------------------------------------------------------------------------------------
// vee functions for twists
//----------------------------------------------------------------------------------------------------------------------
template<typename T>
void cedar::aux::math::veeTwist(const cv::Mat& rMatrix, cv::Mat& rResult)
{
  CEDAR_ASSERT(cedar::aux::math::matrixTypeCheck<T>(rMatrix));
  CEDAR_ASSERT(cedar::aux::math::matrixTypeCheck<T>(rResult));

  // position
  rResult.at<T>(0, 0) = rMatrix.at<T>(0, 3);
  rResult.at<T>(1, 0) = rMatrix.at<T>(1, 3);
  rResult.at<T>(2, 0) = rMatrix.at<T>(2, 3);
  // axis
  rResult.at<T>(3, 0) = rMatrix.at<T>(2, 1);
  rResult.at<T>(4, 0) = rMatrix.at<T>(0, 2);
  rResult.at<T>(5, 0) = rMatrix.at<T>(1, 0);
}

template CEDAR_AUX_LIB_EXPORT void cedar::aux::math::veeTwist<double>(const cv::Mat&, cv::Mat&);
template CEDAR_AUX_LIB_EXPORT void cedar::aux::math::veeTwist<float>(const cv::Mat&, cv::Mat&);

template<typename T>
cv::Mat cedar::aux::math::veeTwist(const cv::Mat& rMatrix)
{
  CEDAR_ASSERT(cedar::aux::math::matrixTypeCheck<T>(rMatrix));

  cv::Mat result = cv::Mat::zeros(6, 1, rMatrix.type());
  veeTwist<T>(rMatrix, result);
  return result;
}

template CEDAR_AUX_LIB_EXPORT cv::Mat cedar::aux::math::veeTwist<double>(const cv::Mat&);
template CEDAR_AUX_LIB_EXPORT cv::Mat cedar::aux::math::veeTwist<float>(const cv::Mat&);

//----------------------------------------------------------------------------------------------------------------------
// exp functions for axis vectors
//----------------------------------------------------------------------------------------------------------------------
template<typename T>
void cedar::aux::math::expAxis(const cv::Mat& rAxis, double theta, cv::Mat& rResult)
{
  CEDAR_ASSERT(cedar::aux::math::matrixTypeCheck<T>(rAxis));
  CEDAR_ASSERT(cedar::aux::math::matrixTypeCheck<T>(rResult));

  rResult = cv::Mat::eye(3, 3, rAxis.type());
  cv::Mat omega_wedge = wedgeAxis<T>(rAxis);
  rResult += (omega_wedge * sin(theta)) + (omega_wedge * omega_wedge * (1 - cos(theta)));
}

template CEDAR_AUX_LIB_EXPORT void cedar::aux::math::expAxis<double>(const cv::Mat&, double, cv::Mat&);
template CEDAR_AUX_LIB_EXPORT void cedar::aux::math::expAxis<float>(const cv::Mat&, double, cv::Mat&);

template<typename T>
cv::Mat cedar::aux::math::expAxis(const cv::Mat& rAxis, double theta)
{
  CEDAR_ASSERT(cedar::aux::math::matrixTypeCheck<T>(rAxis));

  cv::Mat result = cv::Mat::zeros(3, 3, rAxis.type());
  expAxis<T>(rAxis, theta, result);
  return result;
}

template CEDAR_AUX_LIB_EXPORT cv::Mat cedar::aux::math::expAxis<double>(const cv::Mat&, double);
template CEDAR_AUX_LIB_EXPORT cv::Mat cedar::aux::math::expAxis<float>(const cv::Mat&, double);

//----------------------------------------------------------------------------------------------------------------------
// log functions for axis vectors
//----------------------------------------------------------------------------------------------------------------------
template<typename T>
void cedar::aux::math::logAxis(const cv::Mat& rRotation, cv::Mat& rOmega, double& rTheta, bool negativeTheta)
{
  CEDAR_ASSERT(cedar::aux::math::matrixTypeCheck<T>(rRotation));
  CEDAR_ASSERT(cedar::aux::math::matrixTypeCheck<T>(rOmega));

  T trace = rRotation.at<T>(0, 0) + rRotation.at<T>(1, 1) + rRotation.at<T>(2, 2);
  // calculate rotation angle
  rTheta = acos((trace - 1.0) / 2.0);
  if (trace <= -1.0)
  {
    // capture numeric failures for trace very close to one
    rTheta = cedar::aux::math::pi;
  }
  if (negativeTheta)
  {
    rTheta = 2.0 * cedar::aux::math::pi - rTheta;
  }
  // calculate axis of rotation
  T sin_theta = sin(rTheta);
  T cos_theta = cos(rTheta);
  rOmega.at<T>(0, 0) = 1 / (2 * sin_theta) * (rRotation.at<T>(2, 1) - rRotation.at<T>(1, 2));
  rOmega.at<T>(1, 0) = 1 / (2 * sin_theta) * (rRotation.at<T>(0, 2) - rRotation.at<T>(2, 0));
  rOmega.at<T>(2, 0) = 1 / (2 * sin_theta) * (rRotation.at<T>(1, 0) - rRotation.at<T>(0, 1));
  rOmega = rOmega * (1 / cv::norm(rOmega));

  if (cedar::aux::math::isZero(rTheta - cedar::aux::math::pi))
  {
    // easy way of calculating axis fails for numerical reasons, choose different formula, depending on signs
    if ((rRotation.at<T>(1, 0) > 0) && (rRotation.at<T>(2, 0) > 0) && (rRotation.at<T>(2, 1) > 0))
    {
      rOmega.at<T>(0, 0) = + sqrt((rRotation.at<T>(0, 0) - cos_theta) / (1-cos_theta));
      rOmega.at<T>(1, 0) = + sqrt((rRotation.at<T>(1, 1) - cos_theta) / (1-cos_theta));
      rOmega.at<T>(2, 0) = + sqrt((rRotation.at<T>(2, 2) - cos_theta) / (1-cos_theta));
    }
    if ((rRotation.at<T>(1, 0) > 0) && (rRotation.at<T>(2, 0) > 0) && (rRotation.at<T>(2, 1) < 0))
    {
      rOmega.at<T>(0, 0) = + sqrt((rRotation.at<T>(0, 0) - cos_theta) / (1-cos_theta));
      rOmega.at<T>(1, 0) = + sqrt((rRotation.at<T>(1, 1) - cos_theta) / (1-cos_theta));
      rOmega.at<T>(2, 0) = - sqrt((rRotation.at<T>(2, 2) - cos_theta) / (1-cos_theta));
    }
    if ((rRotation.at<T>(1, 0) < 0) && (rRotation.at<T>(2, 0) > 0) && (rRotation.at<T>(2, 1) < 0))
    {
      rOmega.at<T>(0, 0) = + sqrt((rRotation.at<T>(0, 0) - cos_theta) / (1-cos_theta));
      rOmega.at<T>(1, 0) = - sqrt((rRotation.at<T>(1, 1) - cos_theta) / (1-cos_theta));
      rOmega.at<T>(2, 0) = + sqrt((rRotation.at<T>(2, 2) - cos_theta) / (1-cos_theta));
    }
    if ((rRotation.at<T>(1, 0) < 0) && (rRotation.at<T>(2, 0) < 0) && (rRotation.at<T>(2, 1) > 0))
    {
      rOmega.at<T>(0, 0) = + sqrt((rRotation.at<T>(0, 0) - cos_theta) / (1-cos_theta));
      rOmega.at<T>(1, 0) = - sqrt((rRotation.at<T>(1, 1) - cos_theta) / (1-cos_theta));
      rOmega.at<T>(2, 0) = - sqrt((rRotation.at<T>(2, 2) - cos_theta) / (1-cos_theta));
    }
  }
}

template CEDAR_AUX_LIB_EXPORT void cedar::aux::math::logAxis<double>(const cv::Mat&, cv::Mat&, double&, bool);
template CEDAR_AUX_LIB_EXPORT void cedar::aux::math::logAxis<float>(const cv::Mat&, cv::Mat&, double&, bool);

//----------------------------------------------------------------------------------------------------------------------
// exp functions for twists
//----------------------------------------------------------------------------------------------------------------------
template<typename T>
void cedar::aux::math::expTwist(const cv::Mat& rXi, double theta, cv::Mat& rResult)
{
  CEDAR_ASSERT(cedar::aux::math::matrixTypeCheck<T>(rXi));
  CEDAR_ASSERT(cedar::aux::math::matrixTypeCheck<T>(rResult));

  cv::Mat v = rXi(cv::Rect(0, 0, 1, 3));
  cv::Mat omega = rXi(cv::Rect(0, 3, 1, 3));

  // rotation
  cv::Mat R = expAxis<T>(omega, theta);
  cv::Mat p;

  // translation
  if (cv::norm(omega) == 0) // pure translation
  {
    p = v * theta;
  }
  else // translation and rotation
  {
    // p = (eye(3,3) - r)*(cross(w,v)) + w*w'*v*theta (Matlab notation)
    cv::Mat left_term = (cv::Mat::eye(3, 3, rXi.type()) - R) * omega.cross(v);
    p = left_term + (omega * omega.t() * v * theta);
  }
  // join to form rigid transformation matrix
  // a = [ R p
  //       0 1 ]
  rResult = cv::Mat::eye(4, 4, rXi.type());
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
}

template CEDAR_AUX_LIB_EXPORT void cedar::aux::math::expTwist<double>(const cv::Mat&, double, cv::Mat&);
template CEDAR_AUX_LIB_EXPORT void cedar::aux::math::expTwist<float>(const cv::Mat&, double, cv::Mat&);

template<typename T>
cv::Mat cedar::aux::math::expTwist(const cv::Mat& rXi, double theta)
{
  CEDAR_ASSERT(cedar::aux::math::matrixTypeCheck<T>(rXi));

  cv::Mat result = cv::Mat::zeros(4, 4, rXi.type());
  expTwist<T>(rXi, theta, result);
  return result;
}

template CEDAR_AUX_LIB_EXPORT cv::Mat cedar::aux::math::expTwist<double>(const cv::Mat&, double);
template CEDAR_AUX_LIB_EXPORT cv::Mat cedar::aux::math::expTwist<float>(const cv::Mat&, double);

//----------------------------------------------------------------------------------------------------------------------
// log functions for twists
//----------------------------------------------------------------------------------------------------------------------
template<typename T>
void cedar::aux::math::logTwist(const cv::Mat& rTransformation, cv::Mat& rXi, double& rTheta, bool negativeTheta)
{
  CEDAR_ASSERT(cedar::aux::math::matrixTypeCheck<T>(rXi));
  CEDAR_ASSERT(cedar::aux::math::matrixTypeCheck<T>(rTransformation));

  // extract rotation and translation
  cv::Mat R = rTransformation(cv::Rect(0, 0, 3, 3));
  cv::Mat p = rTransformation(cv::Rect(3, 0, 1, 3));

  // calculate exponential coordinates for R = exp(\wedge \omega \theta)
  cv::Mat omega = cv::Mat::zeros(3, 1, rTransformation.type());
  logAxis<T>(R, omega, rTheta, negativeTheta);

  // calculate v
  cv::Mat omega_wedge = wedgeAxis<T>(omega);
  cv::Mat s1 = (cv::Mat::eye(3, 3, rTransformation.type()) - R) * omega_wedge;
  cv::Mat v = (s1 + omega * omega.t() * rTheta).inv() * p;

  // concatenate omega and v to get twist coordinates
  rXi.at<T>(0, 0) = v.at<T>(0, 0);
  rXi.at<T>(1, 0) = v.at<T>(1, 0);
  rXi.at<T>(2, 0) = v.at<T>(2, 0);
  rXi.at<T>(3, 0) = omega.at<T>(0, 0);
  rXi.at<T>(4, 0) = omega.at<T>(1, 0);
  rXi.at<T>(5, 0) = omega.at<T>(2, 0);
}

template CEDAR_AUX_LIB_EXPORT void cedar::aux::math::logTwist<double>(const cv::Mat&, cv::Mat&, double&, bool);
template CEDAR_AUX_LIB_EXPORT void cedar::aux::math::logTwist<float>(const cv::Mat&, cv::Mat&, double&, bool);

//----------------------------------------------------------------------------------------------------------------------
// rigid --> adjoint transformation
//----------------------------------------------------------------------------------------------------------------------
template<typename T>
void cedar::aux::math::rigidToAdjointTransformation(const cv::Mat& rRigidTransformation, cv::Mat& rAdjointTransformation)
{
  CEDAR_ASSERT(cedar::aux::math::matrixTypeCheck<T>(rRigidTransformation));
  CEDAR_ASSERT(cedar::aux::math::matrixTypeCheck<T>(rAdjointTransformation));

  // extract componenets
  cv::Mat rot = rRigidTransformation(cv::Rect(0, 0, 3, 3));
  cv::Mat pos = rRigidTransformation(cv::Rect(3, 0, 1, 3));
  cv::Mat pos_wedge_times_rot = wedgeAxis<T>(pos) * rot;

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

template CEDAR_AUX_LIB_EXPORT void cedar::aux::math::rigidToAdjointTransformation<double>(const cv::Mat&, cv::Mat&);
template CEDAR_AUX_LIB_EXPORT void cedar::aux::math::rigidToAdjointTransformation<float>(const cv::Mat&, cv::Mat&);

template<typename T>
cv::Mat cedar::aux::math::rigidToAdjointTransformation(const cv::Mat& rRigidTransformation)
{
  CEDAR_ASSERT(cedar::aux::math::matrixTypeCheck<T>(rRigidTransformation));

  cv::Mat result = cv::Mat::zeros(6, 6, rRigidTransformation.type());
  rigidToAdjointTransformation<T>(rRigidTransformation, result);
  return result;
}

template CEDAR_AUX_LIB_EXPORT cv::Mat cedar::aux::math::rigidToAdjointTransformation<double>(const cv::Mat&);
template CEDAR_AUX_LIB_EXPORT cv::Mat cedar::aux::math::rigidToAdjointTransformation<float>(const cv::Mat&);

//----------------------------------------------------------------------------------------------------------------------
// adjoint --> rigid transformation
//----------------------------------------------------------------------------------------------------------------------
template<typename T>
void cedar::aux::math::adjointToRigidTransformation(const cv::Mat& rAdjointTransformation, cv::Mat& rRigidTransformation)
{
  CEDAR_ASSERT(cedar::aux::math::matrixTypeCheck<T>(rRigidTransformation));
  CEDAR_ASSERT(cedar::aux::math::matrixTypeCheck<T>(rAdjointTransformation));

  cv::Mat rot = rAdjointTransformation(cv::Rect(0, 0, 3, 3));
  cv::Mat pos = veeAxis<T>(rAdjointTransformation(cv::Rect(3, 0, 3, 3)) * rot.inv());
  
  rRigidTransformation = cv::Mat::eye(4, 4, rAdjointTransformation.type());
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
}

template CEDAR_AUX_LIB_EXPORT void cedar::aux::math::adjointToRigidTransformation<double>(const cv::Mat&, cv::Mat&);
template CEDAR_AUX_LIB_EXPORT void cedar::aux::math::adjointToRigidTransformation<float>(const cv::Mat&, cv::Mat&);

template<typename T>
cv::Mat cedar::aux::math::adjointToRigidTransformation(const cv::Mat& rAdjointTransformation)
{
  CEDAR_ASSERT(cedar::aux::math::matrixTypeCheck<T>(rAdjointTransformation));

  cv::Mat result = cv::Mat::zeros(4, 4, rAdjointTransformation.type());
  adjointToRigidTransformation<T>(rAdjointTransformation, result);
  return result;
}

template CEDAR_AUX_LIB_EXPORT cv::Mat cedar::aux::math::adjointToRigidTransformation<double>(const cv::Mat&);
template CEDAR_AUX_LIB_EXPORT cv::Mat cedar::aux::math::adjointToRigidTransformation<float>(const cv::Mat&);

//----------------------------------------------------------------------------------------------------------------------
// inverse for adjoint transformations
//----------------------------------------------------------------------------------------------------------------------
template<typename T>
void cedar::aux::math::invertAdjointTransformation(const cv::Mat& rAdjointTransformation, cv::Mat& rInverse)
{
  CEDAR_ASSERT(cedar::aux::math::matrixTypeCheck<T>(rInverse));
  CEDAR_ASSERT(cedar::aux::math::matrixTypeCheck<T>(rAdjointTransformation));

  // calculate block matrices of the inverse
  cv::Mat rot_transpose = rAdjointTransformation(cv::Rect(0, 0, 3, 3)).t();
  cv::Mat upper_right = - rot_transpose * rAdjointTransformation(cv::Rect(3, 0, 3, 3)) * rot_transpose;

  // concatenate to inverse
  rInverse = cv::Mat::zeros(6, 6, rAdjointTransformation.type());

  rInverse.at<T> (0, 0) = rot_transpose.at<T> (0, 0);
  rInverse.at<T> (0, 1) = rot_transpose.at<T> (0, 1);
  rInverse.at<T> (0, 2) = rot_transpose.at<T> (0, 2);
  rInverse.at<T> (1, 0) = rot_transpose.at<T> (1, 0);
  rInverse.at<T> (1, 1) = rot_transpose.at<T> (1, 1);
  rInverse.at<T> (1, 2) = rot_transpose.at<T> (1, 2);
  rInverse.at<T> (2, 0) = rot_transpose.at<T> (2, 0);
  rInverse.at<T> (2, 1) = rot_transpose.at<T> (2, 1);
  rInverse.at<T> (2, 2) = rot_transpose.at<T> (2, 2);

  rInverse.at<T> (0, 3) = upper_right.at<T> (0, 0);
  rInverse.at<T> (0, 4) = upper_right.at<T> (0, 1);
  rInverse.at<T> (0, 5) = upper_right.at<T> (0, 2);
  rInverse.at<T> (1, 3) = upper_right.at<T> (1, 0);
  rInverse.at<T> (1, 4) = upper_right.at<T> (1, 1);
  rInverse.at<T> (1, 5) = upper_right.at<T> (1, 2);
  rInverse.at<T> (2, 3) = upper_right.at<T> (2, 0);
  rInverse.at<T> (2, 4) = upper_right.at<T> (2, 1);
  rInverse.at<T> (2, 5) = upper_right.at<T> (2, 2);

  rInverse.at<T> (3, 3) = rot_transpose.at<T> (0, 0);
  rInverse.at<T> (3, 4) = rot_transpose.at<T> (0, 1);
  rInverse.at<T> (3, 5) = rot_transpose.at<T> (0, 2);
  rInverse.at<T> (4, 3) = rot_transpose.at<T> (1, 0);
  rInverse.at<T> (4, 4) = rot_transpose.at<T> (1, 1);
  rInverse.at<T> (4, 5) = rot_transpose.at<T> (1, 2);
  rInverse.at<T> (5, 3) = rot_transpose.at<T> (2, 0);
  rInverse.at<T> (5, 4) = rot_transpose.at<T> (2, 1);
  rInverse.at<T> (5, 5) = rot_transpose.at<T> (2, 2);

}

template CEDAR_AUX_LIB_EXPORT void cedar::aux::math::invertAdjointTransformation<double>(const cv::Mat&, cv::Mat&);
template CEDAR_AUX_LIB_EXPORT void cedar::aux::math::invertAdjointTransformation<float>(const cv::Mat&, cv::Mat&);

template<typename T>
cv::Mat cedar::aux::math::invertAdjointTransformation(const cv::Mat& rAdjointTransformation)
{
  CEDAR_ASSERT(cedar::aux::math::matrixTypeCheck<T>(rAdjointTransformation));

  cv::Mat result = cv::Mat::zeros(6, 6, rAdjointTransformation.type());
  invertAdjointTransformation<T>(rAdjointTransformation, result);
  return result;
}

template CEDAR_AUX_LIB_EXPORT cv::Mat cedar::aux::math::invertAdjointTransformation<double>(const cv::Mat&);
template CEDAR_AUX_LIB_EXPORT cv::Mat cedar::aux::math::invertAdjointTransformation<float>(const cv::Mat&);

//----------------------------------------------------------------------------------------------------------------------
// generation of matrix representations from basic information
//----------------------------------------------------------------------------------------------------------------------
template<typename T>
cv::Mat cedar::aux::math::twistCoordinates(const cv::Mat& rSupportPoint, const cv::Mat& rAxis)
{
  CEDAR_ASSERT(cedar::aux::math::matrixTypeCheck<T>(rSupportPoint));
  CEDAR_ASSERT(cedar::aux::math::matrixTypeCheck<T>(rAxis));

  cv::Mat omega = rAxis(cv::Rect(0, 0, 1, 3)) * (1 / cv::norm(rAxis(cv::Rect(0, 0, 1, 3))));
  cv::Mat cross = rSupportPoint(cv::Rect(0, 0, 1, 3)).cross(omega);
  cv::Mat twist = cv::Mat::zeros(6, 1, rSupportPoint.type());
  twist.at<T>(0, 0) = cross.at<T>(0, 0);
  twist.at<T>(1, 0) = cross.at<T>(1, 0);
  twist.at<T>(2, 0) = cross.at<T>(2, 0);
  twist.at<T>(3, 0) = omega.at<T>(0, 0);
  twist.at<T>(4, 0) = omega.at<T>(1, 0);
  twist.at<T>(5, 0) = omega.at<T>(2, 0);
  return twist;
}

template CEDAR_AUX_LIB_EXPORT cv::Mat cedar::aux::math::twistCoordinates<double>(const cv::Mat&, const cv::Mat&);
template CEDAR_AUX_LIB_EXPORT cv::Mat cedar::aux::math::twistCoordinates<float>(const cv::Mat&, const cv::Mat&);
