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

    File:        ForwardKinematics.h

    Maintainer:  jokeit
    Email:       jean-stephane.jokeit@ini.ruhr-uni-bochum.de
    Date:        2014 02 28

    Description: Header file for the class cedar::dev::ForwardKinematics.

    Credits:

======================================================================================================================*/

#ifndef CEDAR_DEV_FORWARD_KINEMATICS_H
#define CEDAR_DEV_FORWARD_KINEMATICS_H

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/devices/namespace.h"

// FORWARD DECLARATIONS
#include "cedar/devices/ForwardKinematics.fwd.h"
#include "cedar/devices/KinematicChain.h"

// SYSTEM INCLUDES
#include <opencv2/opencv.hpp>


/*!@todo describe.
 *
 * @todo describe more.
 */
class cedar::dev::ForwardKinematics
{

  //--------------------------------------------------------------------------------------------------------------------
  // nested types
  //--------------------------------------------------------------------------------------------------------------------

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  ForwardKinematics(KinematicChain *chain, cedar::aux::LocalCoordinateFramePtr pEndEffector);

  //!@brief Destructor
  virtual ~ForwardKinematics();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  // none yet
  // todo: some of these can be private

  void initializeFromJointList();

  // @todo: deprecate
  void calculateCartesianJacobian( const cv::Mat& point, unsigned int jointIndex, cv::Mat& result, unsigned int coordinateFrame);
  cv::Mat calculateCartesianJacobian( const cv::Mat& point, unsigned int jointIndex, unsigned int coordinateFrame);
  // @todo: deprecate
  void calculateCartesianJacobianTemporalDerivative( const cv::Mat& point, unsigned int jointIndex, cv::Mat& result, unsigned int coordinateFrame);
  cv::Mat calculateCartesianJacobianTemporalDerivative( const cv::Mat& point, unsigned int jointIndex, unsigned int coordinateFrame);
  cv::Mat calculateVelocity( const cv::Mat& point, unsigned int jointIndex, unsigned int coordinateFrame); 
  cv::Mat calculateAcceleration( const cv::Mat& point, unsigned int jointIndex, unsigned int coordinateFrame);

  cv::Mat calculateSpatialJacobian(unsigned int index);
  cv::Mat calculateSpatialJacobianTemporalDerivative(unsigned int index);

  cv::Mat calculateTwistTemporalDerivative(unsigned int jointIndex);
  cv::Mat calculateEndEffectorPosition();
  cv::Mat getRootTransformation();
  cv::Mat getProductOfExponentials(unsigned int jointIndex);
  cv::Mat getEndEffectorTransformation();
  cv::Mat calculateEndEffectorJacobian();
  cv::Mat calculateEndEffectorVelocity();
  cv::Mat calculateEndEffectorAcceleration();
  void calculateTransformations();

  cedar::aux::LocalCoordinateFramePtr getRootCoordinateFrame();
  cedar::aux::LocalCoordinateFramePtr getEndEffectorCoordinateFrame();
  void setEndEffector(cedar::aux::LocalCoordinateFramePtr pEndEffector);

  cv::Mat getJointTransformation(unsigned int index);

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet
private:
  cedar::dev::KinematicChain* mpKinematicChain;

  //!@brief pointer to the root coordinate frame of the kinematic chain
  cedar::aux::LocalCoordinateFramePtr mpRootCoordinateFrame;
  //!@brief pointer to the local coordinate frame of the end-effector
  cedar::aux::LocalCoordinateFramePtr mpEndEffectorCoordinateFrame;
  // locking for thread safety
  QReadWriteLock mTransformationsLock;

  //! twist coordinates for the transformations induced by rotating the joints (assuming reference configurations)
  std::vector<cv::Mat> mReferenceJointTwists;
  //! transformations to the joint frames (assuming reference configurations)
  std::vector<cv::Mat> mReferenceJointTransformations;
  //! transformations to the end-effector frame (assuming reference configurations)
  cv::Mat mReferenceEndEffectorTransformation;
    //! exponentials of joint twists with specified joint angle
  std::vector<cv::Mat> mTwistExponentials;
  //! transformation matrices between joints, generated by exponential map of joint twists
  std::vector<cv::Mat> mProductsOfExponentials;
  //! transformation matrices to the joint frames in the current configuration
  std::vector<cv::Mat> mJointTransformations;
  //! twist coordinates for the transformations induced by rotating the joints in the curent configuration
  std::vector<cv::Mat> mJointTwists;

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

private:
  // none yet

}; // class cedar::dev::ForwardKinematics

#endif // CEDAR_DEV_FORWARD_KINEMATICS_H

