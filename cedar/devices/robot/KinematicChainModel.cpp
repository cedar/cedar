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

 ----- Institute:   Ruhr-Universitaet Bochum
                    Institut fuer Neuroinformatik
 
 ----- File:        KinematicChainModel.cpp
 
 ----- Maintainer:  Hendrik Reimann
 ----- Email:       hendrik.reimann@ini.rub.de
 ----- Date:        2010 11 03
 
 ----- Description: implementation of the @em cedar::dev::robot::KinematicChainModel class.
                    Calculates the geometric transformations occurring in a serial chain depending upon joint angles
 
 ----- Credits:     Mathematics from Murray, Lee, Sastry: A mathematical introduction to robotic manipulation
======================================================================================================================*/

// CEDAR INCLUDES
#include "cedar/devices/robot/KinematicChainModel.h"
#include "cedar/devices/robot/KinematicChain.h"
#include "cedar/devices/robot/ReferenceGeometry.h"
#include "cedar/auxiliaries/math/tools.h"
#include "cedar/auxiliaries/math/screwCalculus.h"
#include "cedar/auxiliaries/LocalCoordinateFrame.h"


// SYSTEM INCLUDES


//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::dev::robot::KinematicChainModel::KinematicChainModel(cedar::dev::robot::KinematicChainPtr pKinematicChain)
:
mpKinematicChain(pKinematicChain),
mpRootCoordinateFrame(new cedar::aux::LocalCoordinateFrame()),
mpEndEffectorCoordinateFrame(new cedar::aux::LocalCoordinateFrame())
{
  init();
}


cedar::dev::robot::KinematicChainModel::KinematicChainModel
(
  cedar::dev::robot::KinematicChainPtr pKinematicChain,
  cedar::aux::LocalCoordinateFramePtr pEndEffector
)
:
mpKinematicChain(pKinematicChain),
mpRootCoordinateFrame(new cedar::aux::LocalCoordinateFrame()),
mpEndEffectorCoordinateFrame(pEndEffector)
{
  init();
}

cedar::dev::robot::KinematicChainModel::~KinematicChainModel()
{

}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::dev::robot::KinematicChainModel::timerEvent(QTimerEvent*)
{
  update();
}

void cedar::dev::robot::KinematicChainModel::update()
{
  calculateTransformations();
  mpEndEffectorCoordinateFrame->update();
}

unsigned int cedar::dev::robot::KinematicChainModel::getNumberOfJoints()
{
  return mpKinematicChain->getNumberOfJoints();
}

cedar::aux::LocalCoordinateFramePtr cedar::dev::robot::KinematicChainModel::getEndEffectorCoordinateFrame()
{
  return mpEndEffectorCoordinateFrame;
}

void cedar::dev::robot::KinematicChainModel::setEndEffector(cedar::aux::LocalCoordinateFramePtr pEndEffector)
{
  mpEndEffectorCoordinateFrame = pEndEffector;
}

cv::Mat cedar::dev::robot::KinematicChainModel::getJointTransformation(unsigned int index)
{
  cv::Mat T;
  mTransformationsLock.lockForRead();
  T = mpRootCoordinateFrame->getTransformation() * mJointTransformations[index];
  mTransformationsLock.unlock();
  return T;
}

void cedar::dev::robot::KinematicChainModel::calculateCartesianJacobian
     (
       const cv::Mat& point,
       unsigned int jointIndex,
       cv::Mat& result,
       unsigned int coordinateFrame
     )
{
  // transform to local coordinates if necessary
  cv::Mat point_local;
  mTransformationsLock.lockForRead();
  switch (coordinateFrame)
  {
    case WORLD_COORDINATES :
    {
      point_local = (mpRootCoordinateFrame->getTransformation() * mJointTransformations[jointIndex]).inv() * point;
      break;
    }
    case BASE_COORDINATES :
    {
      point_local = mJointTransformations[jointIndex].inv() * point;
      break;
    }
    case LOCAL_COORDINATES :
    {
      point_local = point;
      break;
    }
  }
  
  // calculate Jacobian column by column
  cv::Mat column;
  for (unsigned int j = 0; j <=  jointIndex; j++)
  {
    column = cedar::aux::math::wedgeTwist<double>
             (
               cedar::aux::math::rigidToAdjointTransformation<double>(mpRootCoordinateFrame->getTransformation())
               * mJointTwists[j]
             )
             * mpRootCoordinateFrame->getTransformation() // change point to world coordinates
             * mJointTransformations[jointIndex] // change point to root coordinates
             * point_local;
    // export
    result.at<double>(0, j) = column.at<double>(0, 0);
    result.at<double>(1, j) = column.at<double>(1, 0);
    result.at<double>(2, j) = column.at<double>(2, 0);
  }
  mTransformationsLock.unlock();
}

cv::Mat cedar::dev::robot::KinematicChainModel::calculateCartesianJacobian
        (
          const cv::Mat& point,
          unsigned int jointIndex,
          unsigned int coordinateFrame
        )
{
  cv::Mat J = cv::Mat::zeros(3, getNumberOfJoints(), CV_64FC1);
  calculateCartesianJacobian(point, jointIndex, J, coordinateFrame);
  return J;
}

void cedar::dev::robot::KinematicChainModel::calculateCartesianJacobianTemporalDerivative
     (
       const cv::Mat& point,
       unsigned int jointIndex,
       cv::Mat& result,
       unsigned int coordinateFrame
     )
{
  cv::Mat point_world;
  switch (coordinateFrame)
  {
    case WORLD_COORDINATES :
    {
      point_world = point;
      break;
    }
    case BASE_COORDINATES :
    {
      //!\todo add base coordinate treatment
//      point_local = mJointTransformations[jointIndex].inv() * point; ...
      break;
    }
    case LOCAL_COORDINATES :
    {
      point_world = mpRootCoordinateFrame->getTransformation() * mJointTransformations[jointIndex] * point; //... check this
      break;
    }
  }

  // calculate Jacobian temporal derivative column by column
  cv::Mat column;
  cv::Mat S1;
  cv::Mat S2;
  mTransformationsLock.lockForRead();
  for (unsigned int j = 0; j <= jointIndex; j++)
  {
    S1 = cedar::aux::math::wedgeTwist<double>(calculateTwistTemporalDerivative(j)) * point_world;
    S2 = cedar::aux::math::wedgeTwist<double>
         (
           cedar::aux::math::rigidToAdjointTransformation<double>(mpRootCoordinateFrame->getTransformation())
           *mJointTwists[j]
         )
         * calculateVelocity(point_world, jointIndex, WORLD_COORDINATES);

    column = S1 + S2;
    // export
    result.at<double>(0, j) = column.at<double>(0, 0);
    result.at<double>(1, j) = column.at<double>(1, 0);
    result.at<double>(2, j) = column.at<double>(2, 0);
  }
  mTransformationsLock.unlock();
}

cv::Mat cedar::dev::robot::KinematicChainModel::calculateCartesianJacobianTemporalDerivative
        (
          const cv::Mat& point,
          unsigned int jointIndex,
          unsigned int coordinateFrame
        )
{
  cv::Mat J = cv::Mat::zeros(3, getNumberOfJoints(), CV_64FC1);
  calculateCartesianJacobianTemporalDerivative(point, jointIndex, J, coordinateFrame);
  return J;
}

cv::Mat cedar::dev::robot::KinematicChainModel::calculateVelocity
        (
          const cv::Mat& point,
          unsigned int jointIndex,
          unsigned int coordinateFrame
        )
{
  cv::Mat point_world;
  switch (coordinateFrame)
  {
    case WORLD_COORDINATES :
    {
      point_world = point;
      break;
    }
    case BASE_COORDINATES :
    {
      //!\todo add base coordinate treatment
//      point_local = mJointTransformations[jointIndex].inv() * point; ...
      break;
    }
    case LOCAL_COORDINATES :
    {
      point_world = mpRootCoordinateFrame->getTransformation() * mJointTransformations[jointIndex] * point; //... check this
      break;
    }
  }
  return cedar::aux::math::wedgeTwist<double>
         (
           calculateSpatialJacobian(jointIndex) * mpKinematicChain->getJointVelocitiesMatrix()
         )
         * point_world;
}

cv::Mat cedar::dev::robot::KinematicChainModel::calculateAcceleration
        (
          const cv::Mat& point,
          unsigned int jointIndex,
          unsigned int coordinateFrame
        )
{
  cv::Mat point_world;
  switch (coordinateFrame)
  {
    case WORLD_COORDINATES :
    {
      point_world = point;
      break;
    }
    case BASE_COORDINATES :
    {
      //! \todo: add base coordinate treatment
//      point_local = mJointTransformations[jointIndex].inv() * point; ...
      break;
    }
    case LOCAL_COORDINATES :
    {
      point_world = mpRootCoordinateFrame->getTransformation() * mJointTransformations[jointIndex] * point; //... check this
      break;
    }
  }
  cv::Mat J = calculateSpatialJacobian(jointIndex);
  cv::Mat J_dot = calculateSpatialJacobianTemporalDerivative(jointIndex);
  cv::Mat T1 = J_dot * mpKinematicChain->getJointVelocitiesMatrix();
  cv::Mat T2 = J * mpKinematicChain->getJointAccelerationsMatrix();
  cv::Mat S1 = cedar::aux::math::wedgeTwist<double>(T1 + T2) * point_world;
  cv::Mat S2 = cedar::aux::math::wedgeTwist<double>(calculateSpatialJacobian(jointIndex)
               * mpKinematicChain->getJointVelocitiesMatrix())
           * calculateVelocity(point_world, jointIndex, WORLD_COORDINATES);
  return S1 + S2;
}

cv::Mat cedar::dev::robot::KinematicChainModel::calculateSpatialJacobian(unsigned int index)
{
  cv::Mat jacobian = cv::Mat::zeros(6, getNumberOfJoints(), CV_64FC1);
  mTransformationsLock.lockForRead();
  for (unsigned int j = 0; j <= index; j++)
  {
    for (int i = 0; i < 6; i++)
    {
      jacobian.at<double>(i, j) = mJointTwists[j].at<double>(i, 0);
    }
  }
  mTransformationsLock.unlock();
  return cedar::aux::math::rigidToAdjointTransformation<double>(mpRootCoordinateFrame->getTransformation())*jacobian;
}

cv::Mat cedar::dev::robot::KinematicChainModel::calculateSpatialJacobianTemporalDerivative(unsigned int index)
{
  // create k-th column
  cv::Mat J = cv::Mat::zeros(6, getNumberOfJoints(), CV_64FC1);
  for (unsigned int i=0; i<=index; i++)
  {
    // create i-th column
    cv::Mat column = cv::Mat::zeros(6, 1, CV_64FC1);
    column = calculateTwistTemporalDerivative(i);
    // export to matrix
    for (unsigned int j=0; j<6; j++)
    {
      J.at<double>(j, i) = column.at<double>(j, 0);
    }
  }
  return J;
}

cv::Mat cedar::dev::robot::KinematicChainModel::calculateTwistTemporalDerivative(unsigned int j)
{
  // calculate transformation to (j-1)-th joint frame
  cv::Mat g = cv::Mat::zeros(4, 4, CV_64FC1);
  // g is a product of j-1 exponentials, so the temporal derivative is a sum with j-1 summands
  for (unsigned int k = 0; k < j; k++)
  {
    // for the k-th summand, we derive the k-th factor and leave the other ones
    cv::Mat s_k = cv::Mat::eye(4, 4, CV_64FC1); // k-th summand
    // factors before the k-th
    for (unsigned int i = 0; i < k; i++)
    {
      // i-th factor stays the same for i < k
      s_k = s_k * mTwistExponentials[i];
    }
    // the k-th factor of the k-th summand is derived by time
    s_k = s_k * cedar::aux::math::wedgeTwist<double>(mReferenceJointTwists[k])
              * mTwistExponentials[k]
              * mpKinematicChain->getJointVelocity(k);
    // factors after the k-th
    for (unsigned int i = k+1; i < j-1; i++)
    {
      // i-th factor stays the same for i > k
      s_k = s_k * mTwistExponentials[i];
    }
    // add this summand to the sum
    g = g + s_k;
  }
  // adjoint of the calculated sum times the j-th twist is the derivative
  return cedar::aux::math::rigidToAdjointTransformation<double>(g) * mReferenceJointTwists[j];
 }

cv::Mat cedar::dev::robot::KinematicChainModel::calculateEndEffectorPosition()
{
  return mpEndEffectorCoordinateFrame->getTranslation();
}

cv::Mat cedar::dev::robot::KinematicChainModel::getRootTransformation()
{
  return mpRootCoordinateFrame->getTransformation();
}

cv::Mat cedar::dev::robot::KinematicChainModel::getEndEffectorTransformation()
{
  return mpEndEffectorCoordinateFrame->getTransformation();
}

cv::Mat cedar::dev::robot::KinematicChainModel::calculateEndEffectorJacobian()
{
  cv::Mat p = calculateEndEffectorPosition();
  return calculateCartesianJacobian(p, getNumberOfJoints()-1, WORLD_COORDINATES);
}

cv::Mat cedar::dev::robot::KinematicChainModel::calculateEndEffectorVelocity()
{
  cv::Mat p = calculateEndEffectorPosition();
  return calculateVelocity(p, getNumberOfJoints()-1, WORLD_COORDINATES);
}

cv::Mat cedar::dev::robot::KinematicChainModel::calculateEndEffectorAcceleration()
{
  cv::Mat p = calculateEndEffectorPosition();
  return calculateAcceleration(p, getNumberOfJoints()-1, WORLD_COORDINATES);
}

void cedar::dev::robot::KinematicChainModel::init()
{

  cv::Mat xi;
  cv::Mat T;
  cv::Mat p;
  cv::Mat omega = cv::Mat::zeros(3, 1, CV_64FC1);
  omega.at<double>(0, 0) = 1;
  for (unsigned int j=0; j<getNumberOfJoints(); j++)
  {
//    cedar::dev::robot::ReferenceGeometry::JointPtr joint = mpKinematicChain->getReferenceGeometry()->getJoint(j);
//    p = cv::Mat::zeros(3, 1, CV_64FC1);
//    p.at<double>(0, 0) = joint->position[0];
//    p.at<double>(1, 0) = joint->position[1];
//    p.at<double>(2, 0) = joint->position[2];
//    omega = cv::Mat::zeros(3, 1, CV_64FC1);
//    omega.at<double>(0, 0) = joint->axis[0];
//    omega.at<double>(1, 0) = joint->axis[1];
//    omega.at<double>(2, 0) = joint->axis[2];
//    xi = cedar::aux::math::twistCoordinates<double>(p, omega);
//
//    mReferenceJointTwists.push_back(xi.clone());
//    T = cv::Mat::eye(4, 4, CV_64FC1);
//    T.at<double>(0, 3) = joint->position[0];
//    T.at<double>(1, 3) = joint->position[1];
//    T.at<double>(2, 3) = joint->position[2];
//    mReferenceJointTransformations.push_back(T.clone());
//    mTwistExponentials.push_back(cv::Mat::zeros(4, 4, CV_64FC1));
//    mProductsOfExponentials.push_back(cv::Mat::zeros(4, 4, CV_64FC1));
//    mJointTransformations.push_back(cv::Mat::zeros(4, 4, CV_64FC1));
//    mJointTwists.push_back(cv::Mat::zeros(6, 1, CV_64FC1));
  }

  // end effector
//  mReferenceEndEffectorTransformation = cv::Mat::zeros(4, 4, CV_64FC1);
//  cedar::dev::robot::ReferenceGeometry::EndEffectorPtr endEffector
//    = mpKinematicChain->getReferenceGeometry()->getEndEffector();
//
//  mReferenceEndEffectorTransformation.at<double>(0, 3) = endEffector->position[0];
//  mReferenceEndEffectorTransformation.at<double>(1, 3) = endEffector->position[1];
//  mReferenceEndEffectorTransformation.at<double>(2, 3) = endEffector->position[2];
//
//  mReferenceEndEffectorTransformation.at<double>(0, 0) = endEffector->orientation[0];
//  mReferenceEndEffectorTransformation.at<double>(0, 1) = endEffector->orientation[1];
//  mReferenceEndEffectorTransformation.at<double>(0, 2) = endEffector->orientation[2];
//  mReferenceEndEffectorTransformation.at<double>(1, 0) = endEffector->orientation[3];
//  mReferenceEndEffectorTransformation.at<double>(1, 1) = endEffector->orientation[4];
//  mReferenceEndEffectorTransformation.at<double>(1, 2) = endEffector->orientation[5];
//  mReferenceEndEffectorTransformation.at<double>(2, 0) = endEffector->orientation[6];
//  mReferenceEndEffectorTransformation.at<double>(2, 1) = endEffector->orientation[7];
//  mReferenceEndEffectorTransformation.at<double>(2, 2) = endEffector->orientation[8];
//
//  mReferenceEndEffectorTransformation.at<double>(3, 3) = 1.0;

  update();
}

void cedar::dev::robot::KinematicChainModel::calculateTransformations()
{
  mTransformationsLock.lockForWrite();
  // first joint
  cedar::aux::math::expTwist<double>
  (
    mReferenceJointTwists[0],
    mpKinematicChain->getJointAngle(0),
    mTwistExponentials[0]
  );
  mProductsOfExponentials[0] = mTwistExponentials[0].clone();
  mJointTransformations[0] = mProductsOfExponentials[0] * mReferenceJointTransformations[0];
  mJointTwists[0] = mReferenceJointTwists[0];
  // other joints
  for (unsigned int i = 1; i < getNumberOfJoints(); i++)
  {
    cedar::aux::math::expTwist<double>
    (
      mReferenceJointTwists[i],
      mpKinematicChain->getJointAngle(i),
      mTwistExponentials[i]
    );
    mProductsOfExponentials[i] = mProductsOfExponentials[i - 1] * mTwistExponentials[i];
    mJointTransformations[i] = mProductsOfExponentials[i] * mReferenceJointTransformations[i];
    mJointTwists[i] = cedar::aux::math::rigidToAdjointTransformation<double>
                      (
                        mProductsOfExponentials[i]
                      )
                      * mReferenceJointTwists[i];
  }
// end-effector
  mpEndEffectorCoordinateFrame->setTransformation
  (
    mpRootCoordinateFrame->getTransformation()
    * mProductsOfExponentials[getNumberOfJoints()-1]
    * mReferenceEndEffectorTransformation
  );
  mTransformationsLock.unlock();
}

cedar::aux::LocalCoordinateFramePtr cedar::dev::robot::KinematicChainModel::getRootCoordinateFrame()
{
  return mpRootCoordinateFrame;
}
