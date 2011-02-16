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
 
 ----- Description: Header for the @em cedar::dev::robot::KinematicChainModel class.
                    Calculates the geometric transformations occurring in a serial chain depending upon joint angles
 
 ----- Credits:     Mathematics from Murray, Lee, Sastry: A mathematical introduction to robotic manipulation
 ---------------------------------------------------------------------------------------------------------------------*/

// LOCAL INCLUDES
#include "KinematicChainModel.h"

// PROJECT INCLUDES
#include "auxiliaries/math/tools.h"
#include "auxiliaries/math/screwCalculus.h"

// SYSTEM INCLUDES

using namespace cedar::dev::robot;
using namespace cedar::aux::math;
using namespace std;
using namespace cv;

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

KinematicChainModel::KinematicChainModel(cedar::dev::robot::KinematicChainPtr& rpKinematicChain)
:
cedar::aux::Object(rpKinematicChain->getReferenceGeometry()->getConfigFileName()),
mpKinematicChain(rpKinematicChain)
{
  init();
}

KinematicChainModel::~KinematicChainModel()
{

}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void KinematicChainModel::timerEvent(QTimerEvent*)
{
  update();
}

void KinematicChainModel::update()
{
  calculateTransformations();
}

unsigned int KinematicChainModel::getNumberOfJoints()
{
  return mpKinematicChain->getNumberOfJoints();
}

cv::Mat KinematicChainModel::getJointTransformation(const unsigned int index)
{
  Mat T;
  mTransformationsLock.lockForRead();
  T = mTransformation * mJointTransformations[index];
  mTransformationsLock.unlock();
  return T;
}

void KinematicChainModel::calculateCartesianJacobian(
                                                      const cv::Mat& point,
                                                      const unsigned int jointIndex,
                                                      cv::Mat& result,
                                                      const unsigned int coordinateFrame
                                                    )
{
  // transform to local coordinates if necessary
  Mat point_local;
  switch (coordinateFrame)
  {
    case WORLD_COORDINATES :
    {
      point_local = (mTransformation * mJointTransformations[jointIndex]).inv() * point;
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
  Mat column;
  mTransformationsLock.lockForRead();
	for (unsigned int j = 0; j <=  jointIndex; j++)
	{
    column = wedgeTwist<double>(rigidToAdjointTransformation<double>(mTransformation)*mJointTwists[j])
             * mTransformation
             * mJointTransformations[jointIndex]
             * point_local;
		// export
		result.at<double>(0, j) = column.at<double>(0, 0);
		result.at<double>(1, j) = column.at<double>(1, 0);
		result.at<double>(2, j) = column.at<double>(2, 0);
	}
	mTransformationsLock.unlock();
}

cv::Mat KinematicChainModel::calculateCartesianJacobian(
                                                         const cv::Mat& point,
                                                         const unsigned int jointIndex,
                                                         const unsigned int coordinateFrame
                                                       )
{
  cv::Mat J = Mat::zeros(3, getNumberOfJoints(), CV_64FC1);
  calculateCartesianJacobian(point, jointIndex, J, coordinateFrame);
  return J;
}

void KinematicChainModel::calculateCartesianJacobianTemporalDerivative(
                                                                        const cv::Mat& point,
                                                                        const unsigned int jointIndex,
                                                                        cv::Mat& result,
                                                                        const unsigned int coordinateFrame
                                                                      )
{
  Mat point_world;
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
      point_world = mTransformation * mJointTransformations[jointIndex] * point; //... check this
      break;
    }
  }

  // calculate Jacobian temporal derivative column by column
  Mat column;
  Mat S1;
  Mat S2;
  mTransformationsLock.lockForRead();
  for (unsigned int j = 0; j <= jointIndex; j++)
  {
    S1 = wedgeTwist<double>(calculateTwistTemporalDerivative(j)) * point_world;
    S2 = wedgeTwist<double>(rigidToAdjointTransformation<double>(mTransformation)*mJointTwists[j])
         * calculateVelocity(point_world, jointIndex, WORLD_COORDINATES);

    column = S1 + S2;
    // export
    result.at<double>(0, j) = column.at<double>(0, 0);
    result.at<double>(1, j) = column.at<double>(1, 0);
    result.at<double>(2, j) = column.at<double>(2, 0);
  }
  mTransformationsLock.unlock();
}

cv::Mat KinematicChainModel::calculateCartesianJacobianTemporalDerivative(
                                                                           const cv::Mat& point,
                                                                           const unsigned int jointIndex,
                                                                           const unsigned int coordinateFrame
                                                                         )
{
  cv::Mat J = Mat::zeros(3, getNumberOfJoints(), CV_64FC1);
  calculateCartesianJacobianTemporalDerivative(point, jointIndex, J, coordinateFrame);
  return J;
}

cv::Mat KinematicChainModel::calculateVelocity(
                                                const cv::Mat& point,
                                                const unsigned int jointIndex,
                                                const unsigned int coordinateFrame
                                              )
{
  Mat point_world;
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
      point_world = mTransformation * mJointTransformations[jointIndex] * point; //... check this
      break;
    }
  }
  return wedgeTwist<double>(calculateSpatialJacobian(jointIndex) * mpKinematicChain->getJointVelocitiesMatrix()) * point_world;
}

cv::Mat KinematicChainModel::calculateAcceleration(
                                                    const cv::Mat& point,
                                                    const unsigned int jointIndex,
                                                    const unsigned int coordinateFrame
                                                  )
{
  Mat point_world;
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
      point_world = mTransformation * mJointTransformations[jointIndex] * point; //... check this
      break;
    }
  }
  Mat J = calculateSpatialJacobian(jointIndex);
  Mat J_dot = calculateSpatialJacobianTemporalDerivative(jointIndex);
  Mat T1 = J_dot * mpKinematicChain->getJointVelocitiesMatrix();
  Mat T2 = J * mpKinematicChain->getJointAccelerationsMatrix();
  Mat S1 = wedgeTwist<double>(T1 + T2) * point_world;
  Mat S2 = wedgeTwist<double>(calculateSpatialJacobian(jointIndex) * mpKinematicChain->getJointVelocitiesMatrix())
           * calculateVelocity(point_world, jointIndex, WORLD_COORDINATES);
  return S1 + S2;
}

cv::Mat KinematicChainModel::calculateSpatialJacobian(unsigned int index)
{
  Mat jacobian = Mat::zeros(6, getNumberOfJoints(), CV_64FC1);
	mTransformationsLock.lockForRead();
	for (unsigned int j = 0; j <= index; j++)
	{
    for (int i = 0; i < 6; i++)
    {
      jacobian.at<double>(i, j) = mJointTwists[j].at<double>(i, 0);
    }
  }
	mTransformationsLock.unlock();
  return rigidToAdjointTransformation<double>(mTransformation)*jacobian;
}

cv::Mat KinematicChainModel::calculateSpatialJacobianTemporalDerivative(unsigned int index)
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

cv::Mat KinematicChainModel::calculateTwistTemporalDerivative(unsigned int j)
{
  // calculate transformation to (j-1)-th joint frame
  cv::Mat g = cv::Mat::zeros(4, 4, CV_64FC1);
  // g is a product of j-1 exponentials, so the temporal derivative is a sum with j-1 summands
  for (unsigned int k=0; k<j; k++)
  {
    // for the k-th summand, we derive the k-th factor and leave the other ones
    cv::Mat s_k = cv::Mat::eye(4, 4, CV_64FC1); // k-th summand
    // factors before the k-th
    for (unsigned int i=0; i<k; i++)
    {
      // i-th factor stays the same for i < k
      s_k = s_k * mTwistExponentials[i];
    }
    // the k-th factor of the k-th summand is derived by time
    s_k = s_k * wedgeTwist<double>(mReferenceJointTwists[k])
              * mTwistExponentials[k]
              * mpKinematicChain->getJointVelocity(k);
    // factors after the k-th
    for (unsigned int i=k+1; i<j-1; i++)
    {
      // i-th factor stays the same for i > k
      s_k = s_k * mTwistExponentials[i];
    }
    // add this summand to the sum
    g = g + s_k;
  }
  // adjoint of the calculated sum times the j-th twist is the derivative
  return rigidToAdjointTransformation<double>(g) * mReferenceJointTwists[j];
 }

cv::Mat KinematicChainModel::calculateEndEffectorPosition()
{
  Mat position;
	mTransformationsLock.lockForRead();
  position = (mTransformation*mEndEffectorTransformation)(Rect(3, 0, 1, 4));
	mTransformationsLock.unlock();
  return position;
}

cv::Mat KinematicChainModel::calculateEndEffectorTransformation()
{
  Mat T;
  mTransformationsLock.lockForRead();
  T = mTransformation * mEndEffectorTransformation;
  mTransformationsLock.unlock();
  return T;
}

cv::Mat KinematicChainModel::calculateEndEffectorJacobian()
{
  Mat p = calculateEndEffectorPosition();
  return calculateCartesianJacobian(p, getNumberOfJoints()-1, WORLD_COORDINATES);
}

cv::Mat KinematicChainModel::calculateEndEffectorVelocity()
{
  Mat p = calculateEndEffectorPosition();
  return calculateVelocity(p, getNumberOfJoints()-1, WORLD_COORDINATES);
}

cv::Mat KinematicChainModel::calculateEndEffectorAcceleration()
{
  Mat p = calculateEndEffectorPosition();
  return calculateAcceleration(p, getNumberOfJoints()-1, WORLD_COORDINATES);
}

void KinematicChainModel::init()
{

  Mat xi;
  Mat T;
  Mat p;
  Mat omega = Mat::zeros(3, 1, CV_64FC1);
  omega.at<double>(0, 0) = 1;
  for (unsigned int j=0; j<getNumberOfJoints(); j++)
  {
    cedar::dev::robot::ReferenceGeometry::JointPtr joint = mpKinematicChain->getReferenceGeometry()->getJoint(j);
    p = Mat::zeros(3, 1, CV_64FC1);
    p.at<double>(0, 0) = joint->position[0];
    p.at<double>(1, 0) = joint->position[1];
    p.at<double>(2, 0) = joint->position[2];
    omega = Mat::zeros(3, 1, CV_64FC1);
    omega.at<double>(0, 0) = joint->axis[0];
    omega.at<double>(1, 0) = joint->axis[1];
    omega.at<double>(2, 0) = joint->axis[2];
    xi = twistCoordinates<double>(p, omega);
    
    mReferenceJointTwists.push_back(xi.clone());
    T = Mat::eye(4, 4, CV_64FC1);
    T.at<double>(0, 3) = joint->position[0];
    T.at<double>(1, 3) = joint->position[1];
    T.at<double>(2, 3) = joint->position[2];
    mReferenceJointTransformations.push_back(T.clone());
    mTwistExponentials.push_back(Mat::zeros(4, 4, CV_64FC1));
    mProductsOfExponentials.push_back(Mat::zeros(4, 4, CV_64FC1));
    mJointTransformations.push_back(Mat::zeros(4, 4, CV_64FC1));
    mJointTwists.push_back(Mat::zeros(6, 1, CV_64FC1));
  }

  // end effector
  mReferenceEndEffectorTransformation = Mat::zeros(4, 4, CV_64FC1);
  cedar::dev::robot::ReferenceGeometry::EndEffectorPtr endEffector
    = mpKinematicChain->getReferenceGeometry()->getEndEffector();
  
  mReferenceEndEffectorTransformation.at<double>(0, 3) = endEffector->position[0];
  mReferenceEndEffectorTransformation.at<double>(1, 3) = endEffector->position[1];
  mReferenceEndEffectorTransformation.at<double>(2, 3) = endEffector->position[2];

  mReferenceEndEffectorTransformation.at<double>(0, 0) = endEffector->orientation[0];
  mReferenceEndEffectorTransformation.at<double>(0, 1) = endEffector->orientation[1];
  mReferenceEndEffectorTransformation.at<double>(0, 2) = endEffector->orientation[2];
  mReferenceEndEffectorTransformation.at<double>(1, 0) = endEffector->orientation[3];
  mReferenceEndEffectorTransformation.at<double>(1, 1) = endEffector->orientation[4];
  mReferenceEndEffectorTransformation.at<double>(1, 2) = endEffector->orientation[5];
  mReferenceEndEffectorTransformation.at<double>(2, 0) = endEffector->orientation[6];
  mReferenceEndEffectorTransformation.at<double>(2, 1) = endEffector->orientation[7];
  mReferenceEndEffectorTransformation.at<double>(2, 2) = endEffector->orientation[8];

  mReferenceEndEffectorTransformation.at<double>(3, 3) = 1.0;

  mEndEffectorTransformation = Mat::zeros(4, 4, CV_64FC1);
  update();
}

void KinematicChainModel::calculateTransformations()
{
  mTransformationsLock.lockForWrite();
	// first joint
	expTwist<double>(mReferenceJointTwists[0], mpKinematicChain->getJointAngle(0), mTwistExponentials[0]);
	mProductsOfExponentials[0] = mTwistExponentials[0].clone();
	mJointTransformations[0] = mProductsOfExponentials[0] * mReferenceJointTransformations[0];
	mJointTwists[0] = mReferenceJointTwists[0];
  // other joints
  for (unsigned int i = 1; i < getNumberOfJoints(); i++)
	{
    expTwist<double>(mReferenceJointTwists[i], mpKinematicChain->getJointAngle(i), mTwistExponentials[i]);
		mProductsOfExponentials[i] = mProductsOfExponentials[i - 1] * mTwistExponentials[i];
		mJointTransformations[i] = mProductsOfExponentials[i] * mReferenceJointTransformations[i];
    mJointTwists[i] = rigidToAdjointTransformation<double>(mProductsOfExponentials[i]) * mReferenceJointTwists[i];
	}
  // end-effector
  mEndEffectorTransformation = mProductsOfExponentials[getNumberOfJoints()-1] * mReferenceEndEffectorTransformation;
	mTransformationsLock.unlock();
}
