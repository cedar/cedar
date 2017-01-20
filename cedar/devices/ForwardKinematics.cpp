/*======================================================================================================================

    Copyright 2011, 2012, 2013 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany
 
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

    File:        ForwardKinematics.cpp

    Maintainer:  jokeit
    Email:       jean-stephane.jokeit@ini.ruhr-uni-bochum.de
    Date:        2014 02 28

    Description: Source file for the class cedar::dev::ForwardKinematics.

    Credits:

======================================================================================================================*/

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/devices/ForwardKinematics.h"
#include "cedar/auxiliaries/LocalCoordinateFrame.h"
#include "cedar/auxiliaries/math/screwCalculus.h"
#include "cedar/auxiliaries/math/tools.h"

// SYSTEM INCLUDES

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::dev::ForwardKinematics::ForwardKinematics(KinematicChain *chain, cedar::aux::LocalCoordinateFramePtr pEndEffector)
  : mpKinematicChain(chain),
    mpRootCoordinateFrame(new cedar::aux::LocalCoordinateFrame()),
    mpEndEffectorCoordinateFrame(pEndEffector)
{
}

cedar::dev::ForwardKinematics::~ForwardKinematics()
{
  // do not delete mpKinematicChain (!) ownership belongs to different object
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::dev::ForwardKinematics::initializeFromJointList()
{
  // clear variables
  mReferenceJointTwists.clear();
  mReferenceJointTransformations.clear();
  mTwistExponentials.clear();
  mProductsOfExponentials.clear();
  mJointTransformations.clear();
  mJointTwists.clear();

  // transform joint geometry into twist coordinates
  cv::Mat xi;
  cv::Mat T;
  cv::Mat p;
  cv::Mat omega = cv::Mat::zeros(3, 1, CV_32FC1);
  for (unsigned int j=0; j < mpKinematicChain->getNumberOfJoints(); j++)
  {
    // create and store twist
    cedar::dev::KinematicChain::JointPtr joint = mpKinematicChain->getJoint(j);
    p = cv::Mat::zeros(3, 1, CV_32FC1);
    p.at<float>(0, 0) = joint->_mpPosition->at(0);
    p.at<float>(1, 0) = joint->_mpPosition->at(1);
    p.at<float>(2, 0) = joint->_mpPosition->at(2);
    omega = cv::Mat::zeros(3, 1, CV_32FC1);
    omega.at<float>(0, 0) = joint->_mpAxis->at(0);
    omega.at<float>(1, 0) = joint->_mpAxis->at(1);
    omega.at<float>(2, 0) = joint->_mpAxis->at(2);
    xi = cedar::aux::math::twistCoordinates<float>(p, omega);
    mReferenceJointTwists.push_back(xi.clone());

    // create and store transformation matrix to joint coordinate frame
    T = cv::Mat::eye(4, 4, CV_32FC1);
    T.at<float>(0, 3) = joint->_mpPosition->at(0);
    T.at<float>(1, 3) = joint->_mpPosition->at(1);
    T.at<float>(2, 3) = joint->_mpPosition->at(2);
    mReferenceJointTransformations.push_back(T.clone());

    // create storage variables for intermediate results
    mTwistExponentials.push_back(cv::Mat::eye(4, 4, CV_32FC1));
    mProductsOfExponentials.push_back(cv::Mat::eye(4, 4, CV_32FC1));
    mJointTransformations.push_back(cv::Mat::eye(4, 4, CV_32FC1));
    mJointTwists.push_back(cv::Mat::eye(6, 1, CV_32FC1));
  }

  // end-effector
  mReferenceEndEffectorTransformation = getEndEffectorCoordinateFrame()->getTransformation();
}

cv::Mat cedar::dev::ForwardKinematics::getJointTransformation(unsigned int index)
{
  cv::Mat T;
  mTransformationsLock.lockForRead();
  T = mpRootCoordinateFrame->getTransformation() * mJointTransformations[index];
  mTransformationsLock.unlock();
  return T;
}

void cedar::dev::ForwardKinematics::calculateCartesianJacobian
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
    case cedar::dev::KinematicChain::WORLD_COORDINATES :
    {
      point_local = (mpRootCoordinateFrame->getTransformation() * mJointTransformations[jointIndex]).inv() * point;
      break;
    }
    case cedar::dev::KinematicChain::BASE_COORDINATES :
    {
      point_local = mJointTransformations[jointIndex].inv() * point;
      break;
    }
    case cedar::dev::KinematicChain::LOCAL_COORDINATES :
    {
      point_local = point;
      break;
    }
  }

  // calculate Jacobian column by column
  cv::Mat column;
  for (unsigned int j = 0; j <=  jointIndex; j++)
  {
    column = cedar::aux::math::wedgeTwist<float>
             (
               cedar::aux::math::rigidToAdjointTransformation<float>(mpRootCoordinateFrame->getTransformation())
               * mJointTwists[j]
             )
             * mpRootCoordinateFrame->getTransformation() // change point to world coordinates
             * mJointTransformations[jointIndex] // change point to root coordinates
             * point_local;
    // export
    result.at<float>(0, j) = column.at<float>(0, 0);
    result.at<float>(1, j) = column.at<float>(1, 0);
    result.at<float>(2, j) = column.at<float>(2, 0);
  }
  mTransformationsLock.unlock();
}

cv::Mat cedar::dev::ForwardKinematics::calculateCartesianJacobian
        (
          const cv::Mat& point,
          unsigned int jointIndex,
          unsigned int coordinateFrame
        )
{
  cv::Mat J = cv::Mat::zeros(3, mpKinematicChain->getNumberOfJoints(), CV_32FC1);
  calculateCartesianJacobian(point, jointIndex, J, coordinateFrame);
  return J;
}

void cedar::dev::ForwardKinematics::calculateCartesianJacobianTemporalDerivative
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
    case cedar::dev::KinematicChain::WORLD_COORDINATES :
    {
      point_world = point;
      break;
    }
    case cedar::dev::KinematicChain::BASE_COORDINATES :
    {
      //!@todo add base coordinate treatment
//      point_local = mJointTransformations[jointIndex].inv() * point; ...
      break;
    }
    case cedar::dev::KinematicChain::LOCAL_COORDINATES :
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
    S1 = cedar::aux::math::wedgeTwist<float>(calculateTwistTemporalDerivative(j)) * point_world;
    S2 = cedar::aux::math::wedgeTwist<float>
         (
           cedar::aux::math::rigidToAdjointTransformation<float>(mpRootCoordinateFrame->getTransformation())
           *mJointTwists[j]
         )
         * calculateVelocity(point_world, jointIndex, cedar::dev::KinematicChain::WORLD_COORDINATES);

    column = S1 + S2;
    // export
    result.at<float>(0, j) = column.at<float>(0, 0);
    result.at<float>(1, j) = column.at<float>(1, 0);
    result.at<float>(2, j) = column.at<float>(2, 0);
  }
  mTransformationsLock.unlock();
}

cv::Mat cedar::dev::ForwardKinematics::calculateCartesianJacobianTemporalDerivative
        (
          const cv::Mat& point,
          unsigned int jointIndex,
          unsigned int coordinateFrame
        )
{
  cv::Mat J = cv::Mat::zeros(3, mpKinematicChain->getNumberOfJoints(), CV_32FC1);
  calculateCartesianJacobianTemporalDerivative(point, jointIndex, J, coordinateFrame);
  return J;
}

cv::Mat cedar::dev::ForwardKinematics::calculateVelocity
        (
          const cv::Mat& point,
          unsigned int jointIndex,
          unsigned int coordinateFrame
        )
{
  cv::Mat point_world;
  switch (coordinateFrame)
  {
    case cedar::dev::KinematicChain::WORLD_COORDINATES :
    {
      point_world = point;
      break;
    }
    case cedar::dev::KinematicChain::BASE_COORDINATES :
    {
      //!@todo add base coordinate treatment
//      point_local = mJointTransformations[jointIndex].inv() * point; ...
      break;
    }
    case cedar::dev::KinematicChain::LOCAL_COORDINATES :
    {
      point_world = mpRootCoordinateFrame->getTransformation() * mJointTransformations[jointIndex] * point; //... check this
      break;
    }
  }
  return cedar::aux::math::wedgeTwist<float>
         (
           calculateSpatialJacobian(jointIndex) * mpKinematicChain->getJointVelocities()
         )
         * point_world;
}

cv::Mat cedar::dev::ForwardKinematics::calculateAcceleration
        (
          const cv::Mat& point,
          unsigned int jointIndex,
          unsigned int coordinateFrame
        )
{
  cv::Mat point_world;
  switch (coordinateFrame)
  {
    case cedar::dev::KinematicChain::WORLD_COORDINATES :
    {
      point_world = point;
      break;
    }
    case cedar::dev::KinematicChain::BASE_COORDINATES :
    {
      //!@todo: add base coordinate treatment
//      point_local = mJointTransformations[jointIndex].inv() * point; ...
      break;
    }
    case cedar::dev::KinematicChain::LOCAL_COORDINATES :
    {
      point_world = mpRootCoordinateFrame->getTransformation() * mJointTransformations[jointIndex] * point; //... check this
      break;
    }
  }
  cv::Mat J = calculateSpatialJacobian(jointIndex);
  cv::Mat J_dot = calculateSpatialJacobianTemporalDerivative(jointIndex);
  cv::Mat T1 = J_dot * mpKinematicChain->getJointVelocities();
  cv::Mat T2 = J * mpKinematicChain->getJointAccelerations();
  cv::Mat S1 = cedar::aux::math::wedgeTwist<float>(T1 + T2) * point_world;
  cv::Mat S2 = cedar::aux::math::wedgeTwist<float>(calculateSpatialJacobian(jointIndex)
               * mpKinematicChain->getJointVelocities())
           * calculateVelocity(point_world, jointIndex, cedar::dev::KinematicChain::WORLD_COORDINATES);
  return S1 + S2;
}

cv::Mat cedar::dev::ForwardKinematics::calculateSpatialJacobian(unsigned int index)
{
  cv::Mat jacobian = cv::Mat::zeros(6, mpKinematicChain->getNumberOfJoints(), CV_32FC1);
  mTransformationsLock.lockForRead();
  for (unsigned int j = 0; j <= index; j++)
  {
    for (int i = 0; i < 6; i++)
    {
      jacobian.at<float>(i, j) = mJointTwists[j].at<float>(i, 0);
    }
  }
  mTransformationsLock.unlock();
  return cedar::aux::math::rigidToAdjointTransformation<float>(mpRootCoordinateFrame->getTransformation())*jacobian;
}

cv::Mat cedar::dev::ForwardKinematics::calculateSpatialJacobianTemporalDerivative(unsigned int index)
{
  // create k-th column
  cv::Mat J = cv::Mat::zeros(6, mpKinematicChain->getNumberOfJoints(), CV_32FC1);
  for (unsigned int i=0; i<=index; i++)
  {
    mTransformationsLock.lockForRead();
    // create i-th column
    cv::Mat column = cv::Mat::zeros(6, 1, CV_32FC1);
    column = calculateTwistTemporalDerivative(i);
    // export to matrix
    for (unsigned int j=0; j<6; j++)
    {
      J.at<float>(j, i) = column.at<float>(j, 0);
    }
    mTransformationsLock.unlock();
  }
  return J;
}

cv::Mat cedar::dev::ForwardKinematics::calculateTwistTemporalDerivative(unsigned int jointIndex)
{
  // calculate transformation to (j-1)-th joint frame
  cv::Mat g = cv::Mat::zeros(4, 4, CV_32FC1);
  // g is a product of j-1 exponentials, so the temporal derivative is a sum with j-1 summands
  for (unsigned int k = 0; k < jointIndex; k++)
  {
    /*******************************************************************************************************************
     * the k-th summand, deriving the factor with positive sign theta_k
     ******************************************************************************************************************/
    cv::Mat s_k = cv::Mat::eye(4, 4, CV_32FC1); // summand where the factor with the positive sign theta_k is derived
    // factors before the k-th
    for (unsigned int j = 0; j < k; j++)
    {
      // j-th factor stays the same for j < k
      s_k = s_k * mTwistExponentials[j];
//      if((jointIndex == 2) && (k == 1))
//      {
//        std::cout << "joint index = " << jointIndex << ", k = " << k << std::endl;
//        std::cout << "j = " << j << std::endl;
//        std::cout << "s_k:" << std::endl;
//        cedar::aux::math::write(s_k);
//      }

    }
    // k-th factor is derived by time
    s_k = s_k * cedar::aux::math::wedgeTwist<float>(mReferenceJointTwists[k]) * mTwistExponentials[k];

//    if((jointIndex == 2) && (k == 1))
//    {
//      std::cout << "j = " << k << std::endl;
//
//      std::cout << "s_k:" << std::endl;
//      cedar::aux::math::write(s_k);
//      std::cout << "cedar::aux::math::wedgeTwist<float>(mReferenceJointTwists[k]):" << std::endl;
//      cedar::aux::math::write(cedar::aux::math::wedgeTwist<float>(mReferenceJointTwists[k]));
//      std::cout << "mTwistExponentials[k]:" << std::endl;
//      cedar::aux::math::write(mTwistExponentials[k]);
//    }
    // factors after the k-th
    for (unsigned int j = k+1; j < jointIndex; j++)
    {
      // j-th factor stays the same for j > k
      s_k = s_k * mTwistExponentials[j];
//      if((jointIndex == 2) && (k == 1))
//      {
//        std::cout << "j = " << j << std::endl;
//        std::cout << "s_k:" << std::endl;
//        cedar::aux::math::write(s_k);
//      }
    }
    s_k = s_k * cedar::aux::math::wedgeTwist<float>(mReferenceJointTwists[jointIndex])
              * mProductsOfExponentials[jointIndex-1].inv();
//    if((jointIndex == 2) && (k == 1))
//    {
//      std::cout << "final s_k:" << std::endl;
//      cedar::aux::math::write(s_k);
//    }
    /*******************************************************************************************************************
     * the (2*(j-1)-k)-th summand, deriving the factor with negative sign theta_k
     ******************************************************************************************************************/
    cv::Mat t_k; // summand where the factor with the negative sign theta_k is derived
    t_k = mProductsOfExponentials[jointIndex-1]
            * cedar::aux::math::wedgeTwist<float>(mReferenceJointTwists[jointIndex]);
    // factors before the k-th
    for (unsigned int j = jointIndex-1; j > k; j--)
    {
      // j-th factor stays the same for j > k
      t_k = t_k * mTwistExponentials[j].inv();
    }
    // k-th factor is derived by time
    t_k = t_k * cedar::aux::math::wedgeTwist<float>(mReferenceJointTwists[k]) * mTwistExponentials[k].inv();
    // factors after the k-th
    for (int j = k-1; j >= 0; j--)
    {
      // j-th factor stays the same for j < k
      t_k = t_k * mTwistExponentials[j].inv();
    }

//    if((jointIndex == 2) && (k == 1))
//    {
//      std::cout << "final s_k:" << std::endl;
//      cedar::aux::math::write(s_k);
//      std::cout << "final t_k:" << std::endl;
//      cedar::aux::math::write(t_k);
//      std::cout << "g:" << std::endl;
//      cedar::aux::math::write(g);
//    }
    // add this summand to the sum
    g = g + mpKinematicChain->getJointVelocity(k)*(s_k - t_k);

//    std::cout << "joint index = " << jointIndex << ", summand " << k << std::endl;
//    std::cout << "final s_k:" << std::endl;
//    cedar::aux::math::write(s_k);
//    std::cout << "final t_k:" << std::endl;
//    cedar::aux::math::write(t_k);
//    std::cout << "g:" << std::endl;
//    cedar::aux::math::write(g);
  }

//  std::cout << "twist exponentials:" << std::endl;
//  for (unsigned int l = 0; l <= jointIndex; l++)
//  {
//    cedar::aux::math::write(mTwistExponentials[l]);
//  }
//  std::cout << "cedar::aux::math::veeTwist<float>(g):" << std::endl;
//  cedar::aux::math::write(cedar::aux::math::veeTwist<float>(g));
//  std::cout << "------------------------------------------------------" << std::endl;

  // adjoint of the calculated sum times the j-th twist is the derivative
  return cedar::aux::math::veeTwist<float>(g);
 }

cv::Mat cedar::dev::ForwardKinematics::calculateEndEffectorPosition()
{
  return mpEndEffectorCoordinateFrame->getTranslation().matrix;
}

cv::Mat cedar::dev::ForwardKinematics::getRootTransformation()
{
  return mpRootCoordinateFrame->getTransformation();
}
  
cedar::aux::LocalCoordinateFramePtr cedar::dev::ForwardKinematics::getRootCoordinateFrame()
{
  return mpRootCoordinateFrame;
}

cedar::aux::LocalCoordinateFramePtr cedar::dev::ForwardKinematics::getEndEffectorCoordinateFrame()
{
  return mpEndEffectorCoordinateFrame;
}

void cedar::dev::ForwardKinematics::setEndEffector(cedar::aux::LocalCoordinateFramePtr pEndEffector)
{
  mpEndEffectorCoordinateFrame = pEndEffector;
}

cv::Mat cedar::dev::ForwardKinematics::getProductOfExponentials(unsigned int jointIndex)
{
  return mProductsOfExponentials[jointIndex];
}

cv::Mat cedar::dev::ForwardKinematics::getEndEffectorTransformation()
{
  return mpEndEffectorCoordinateFrame->getTransformation();
}

cv::Mat cedar::dev::ForwardKinematics::calculateEndEffectorJacobian()
{
  cv::Mat p = calculateEndEffectorPosition();
  return calculateCartesianJacobian(p, mpKinematicChain->getNumberOfJoints()-1, cedar::dev::KinematicChain::WORLD_COORDINATES);
}

cv::Mat cedar::dev::ForwardKinematics::calculateEndEffectorVelocity()
{
  cv::Mat p = calculateEndEffectorPosition();
  return calculateVelocity(p, mpKinematicChain->getNumberOfJoints()-1, cedar::dev::KinematicChain::WORLD_COORDINATES);
}

cv::Mat cedar::dev::ForwardKinematics::calculateEndEffectorAcceleration()
{
  cv::Mat p = calculateEndEffectorPosition();
  return calculateAcceleration(p, mpKinematicChain->getNumberOfJoints()-1, cedar::dev::KinematicChain::WORLD_COORDINATES);
}

void cedar::dev::ForwardKinematics::calculateTransformations()
{
  mTransformationsLock.lockForWrite();
  // first joint
  cedar::aux::math::expTwist<float>
  (
    mReferenceJointTwists[0],
    mpKinematicChain->getJointAngle(0),
    mTwistExponentials[0]
  );
  mProductsOfExponentials[0] = mTwistExponentials[0].clone();
  mJointTransformations[0] = mProductsOfExponentials[0] * mReferenceJointTransformations[0];
  mJointTwists[0] = mReferenceJointTwists[0];
  // other joints
  for (unsigned int i = 1; i < mpKinematicChain->getNumberOfJoints(); i++)
  {
    cedar::aux::math::expTwist<float>
    (
      mReferenceJointTwists[i],
      mpKinematicChain->getJointAngle(i),
      mTwistExponentials[i]
    );
    mProductsOfExponentials[i] = mProductsOfExponentials[i - 1] * mTwistExponentials[i];
    mJointTransformations[i] = mProductsOfExponentials[i] * mReferenceJointTransformations[i];
    mJointTwists[i] = cedar::aux::math::rigidToAdjointTransformation<float>
                      (
                        mProductsOfExponentials[i]
                      )
                      * mReferenceJointTwists[i];
  }
// end-effector
  mpEndEffectorCoordinateFrame->setTransformation
  (
    mpRootCoordinateFrame->getTransformation()
    * mProductsOfExponentials[mpKinematicChain->getNumberOfJoints()-1]
    * mReferenceEndEffectorTransformation
  );
  mTransformationsLock.unlock();
}


