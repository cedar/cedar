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
#include "cedar/auxiliaries/math/tools.h"

// SYSTEM INCLUDES

using namespace cedar::dev::robot;
using namespace cedar::aux::math;
using namespace std;
using namespace cv;

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

//KinematicChainModel::KinematicChainModel()
//{
//  std::cout << "running KinematicChainModel()" << endl;
//  initExample();
//}

KinematicChainModel::KinematicChainModel(cedar::dev::robot::ReferenceGeometry* pReferenceGeometry)
{
  std::cout << "running KinematicChainModel(cedar::dev::robot::ReferenceGeometry* pReferenceGeometry)" << endl;
  mpReferenceGeometry = pReferenceGeometry;
  init();
}

KinematicChainModel::~KinematicChainModel()
{

}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

unsigned int KinematicChainModel::numberOfJoints()
{
  return mNumberOfJoints;
}

void KinematicChainModel::calculateTransformations(const cv::Mat& jointAngles)
{
  mTransformationsLock.lockForWrite();
	// first joint
	expTwist<double>(mReferenceJointTwists[0], jointAngles.at<double>(0, 0), mTwistExponentials[0]);
	mProductsOfExponentials[0] = mTwistExponentials[0].clone();
	mJointTransformations[0] = mProductsOfExponentials[0] * mReferenceJointTransformations[0];
	mJointTwists[0] = mReferenceJointTwists[0];
  // other joints
  for (int i = 1; i < mNumberOfJoints; i++)
	{
    expTwist<double>(mReferenceJointTwists[i], jointAngles.at<double>(i, 0), mTwistExponentials[i]);
		mProductsOfExponentials[i] = mProductsOfExponentials[i - 1] * mTwistExponentials[i];
		mJointTransformations[i] = mProductsOfExponentials[i] * mReferenceJointTransformations[i];
    mJointTwists[i] = rigidToAdjointTransformation<double>(mProductsOfExponentials[i]) * mReferenceJointTwists[i];
	}
  // end-effector
  mEndEffectorTransformation = mProductsOfExponentials[mNumberOfJoints-1] * mReferenceEndEffectorTransformation;
	mTransformationsLock.unlock();
  
  cedar::aux::math::write(mReferenceJointTwists[2]);
  cedar::aux::math::write(jointTransformation(2));
  cedar::aux::math::write(endEffectorTransformation());
  
}

cv::Mat KinematicChainModel::jointTransformation(const unsigned int index)
{
  Mat T;
  mTransformationsLock.lockForRead();
  T = mJointTransformations[index].clone();
  mTransformationsLock.unlock();
  return T;
}

void KinematicChainModel::jacobian(
                                   const cv::Mat& point,
                                   const unsigned int jointIndex,
                                   cv::Mat& result,
                                   const unsigned int coordinateFrame
                                  )
{
  // transform to local coordinates if necessary
  Mat point_local;
  if (coordinateFrame == BASE_COORDINATES)
  {
    point_local = mJointTransformations[jointIndex].inv() * point;
  }
  else
  {
    point_local = point;
  }
  
  // calculate Jacobian column by column
  Mat column;
  mTransformationsLock.lockForRead();
	for (unsigned int j = 0; j <=  jointIndex; j++)
	{
		column = wedgeTwist<double>(mJointTwists[j]) * mJointTransformations[jointIndex] * point_local;
		// export
		result.at<double>(0, j) = column.at<double>(0, 0);
		result.at<double>(1, j) = column.at<double>(1, 0);
		result.at<double>(2, j) = column.at<double>(2, 0);
	}
	mTransformationsLock.unlock();
}

cv::Mat KinematicChainModel::jacobian(
                                      const cv::Mat& point,
                                      const unsigned int jointIndex,
                                      const unsigned int coordinateFrame
                                     )
{
  cv::Mat J = Mat::zeros(3, mNumberOfJoints, CV_64FC1);
  jacobian(point, jointIndex, J, coordinateFrame);
  return J;
}

cv::Mat KinematicChainModel::spatialJacobian()
{
  Mat jacobian = Mat::zeros(6, mNumberOfJoints, CV_64FC1);
	mTransformationsLock.lockForRead();
	for (int j = 0; j < mNumberOfJoints; j++)
	{
    for (int i = 0; i < 6; i++)
    {
      jacobian.at<double>(i, j) = mJointTwists[j].at<double>(i, 0);
    }
  }
	mTransformationsLock.unlock();
  return jacobian;
}

cv::Mat KinematicChainModel::endEffectorPosition()
{
  Mat position;
	mTransformationsLock.lockForRead();
  position = mEndEffectorTransformation(Rect(3, 0, 1, 4)).clone();
	mTransformationsLock.unlock();
  return position;
}

cv::Mat KinematicChainModel::endEffectorTransformation()
{
  Mat T;
  mTransformationsLock.lockForRead();
  T = mEndEffectorTransformation.clone();
  mTransformationsLock.unlock();
  return T;
}

cv::Mat KinematicChainModel::endEffectorJacobian()
{
  Mat p = endEffectorPosition();
  return jacobian(p, mNumberOfJoints-1, BASE_COORDINATES);
}

//void KinematicChainModel::initExample()
//{
//  // this hard-codes a planar manipulator with 4 joints, just to test stuff
//  
//  mNumberOfJoints = 4;
//  Mat xi;
//  Mat T;
//  Mat p;
//  Mat omega = Mat::zeros(3, 1, CV_64FC1);
//  omega.at<double>(0, 0) = 1;
//  for (int j=0; j<mNumberOfJoints; j++)
//  {
//    // reference twists
//    p = Mat::zeros(3, 1, CV_64FC1);
//    p.at<double>(2, 0) = j*2.0;
//    xi = twistCoordinates<double>(p, omega);
//    //    cout << "xi = twistCoordinates(p, omega):" << endl;
//    //    write(xi);
//    //    xi = Mat::zeros(6, 1, CV_64FC1);
//    //    xi.at<double>(1, 0) = j*2.0;
//    //    xi.at<double>(3, 0) = 1;
//    //    cout << "xi by hand:" << endl;
//    //    write(xi);
//    mReferenceJointTwists.push_back(xi.clone());
//    T = Mat::eye(4, 4, CV_64FC1);
//    T.at<double>(2, 3) = j*2.0;
//    mReferenceJointTransformations.push_back(T.clone());
//    mTwistExponentials.push_back(Mat::zeros(4, 4, CV_64FC1));
//    mProductsOfExponentials.push_back(Mat::zeros(4, 4, CV_64FC1));
//    mJointTransformations.push_back(Mat::zeros(4, 4, CV_64FC1));
//    mJointTwists.push_back(Mat::zeros(6, 1, CV_64FC1));
//  }
//  mReferenceEndEffectorTransformation = Mat::eye(4, 4, CV_64FC1);
//  mReferenceEndEffectorTransformation.at<double>(2, 3) = 8;
//  mEndEffectorTransformation = Mat::zeros(4, 4, CV_64FC1);
//  
//  //  for (int j=0; j<mNumberOfJoints; j++)
//  //  {
//  //    write(mReferenceJointTwists[j]);
//  //    write(mReferenceJointTransformations[j]);
//  //    cout << "--------------------------" << endl;
//  //    
//  //  }  
//  
//}

void KinematicChainModel::init()
{
  mNumberOfJoints = mpReferenceGeometry->numberOfJoints();

  Mat xi;
  Mat T;
  Mat p;
  Mat omega = Mat::zeros(3, 1, CV_64FC1);
  omega.at<double>(0, 0) = 1;
  for (int j=0; j<mNumberOfJoints; j++)
  {
    cedar::dev::robot::ReferenceGeometry::JointPtr joint = mpReferenceGeometry->getJoint(j);
    p = Mat::zeros(3, 1, CV_64FC1);
    p.at<double>(0, 0) = joint->position[0];
    p.at<double>(1, 0) = joint->position[1];
    p.at<double>(2, 0) = joint->position[2];
    omega = Mat::zeros(3, 1, CV_64FC1);
    omega.at<double>(0, 0) = joint->axis[0];
    omega.at<double>(1, 0) = joint->axis[1];
    omega.at<double>(2, 0) = joint->axis[2];
    xi = twistCoordinates<double>(p, omega);
    
    cout << "xi = twistCoordinates(p, omega):" << endl;
    write(xi);
    //    xi = Mat::zeros(6, 1, CV_64FC1);
    //    xi.at<double>(1, 0) = j*2.0;
    //    xi.at<double>(3, 0) = 1;
    //    cout << "xi by hand:" << endl;
    //    write(xi);
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
  cedar::dev::robot::ReferenceGeometry::EndEffectorPtr endEffector = mpReferenceGeometry->getEndEffector();
  
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

  write(mReferenceEndEffectorTransformation);
  
  mEndEffectorTransformation = Mat::zeros(4, 4, CV_64FC1);
  
  //  for (int j=0; j<mNumberOfJoints; j++)
  //  {
  //    write(mReferenceJointTwists[j]);
  //    write(mReferenceJointTransformations[j]);
  //    cout << "--------------------------" << endl;
  //    
  //  }  
  
}






















