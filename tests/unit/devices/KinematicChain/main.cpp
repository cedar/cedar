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

 ----- Institute:   Ruhr-Universitaet Bochum
                    Institut fuer Neuroinformatik
 
 ----- File:        main.cpp
 
 ----- Author:      Hendrik Reimann
 ----- Email:       hendrik.reimann@ini.rub.de
 ----- Date:        2010 11 30
 
 ----- Description: unit test for the @em cedar::dev::SimulatedKinematicChain class.
 
 ----- Credits:
 ---------------------------------------------------------------------------------------------------------------------*/

// CEDAR INCLUDES
#include "cedar/auxiliaries/math/tools.h"
#include "cedar/auxiliaries/LocalCoordinateFrame.h"
#include "cedar/auxiliaries/DoubleVectorParameter.h"
#include "cedar/auxiliaries/math/LimitsParameter.h"
#include "cedar/devices/KinematicChain.h"
#include "tests/unit/devices/KinematicChain/TestKinematicChain.h"
#include "cedar/auxiliaries/math/constants.h"
#include "cedar/auxiliaries/systemFunctions.h"
#include "cedar/units/UnitMatrix.h"

// SYSTEM INCLUDES
CEDAR_GENERATE_POINTER_TYPES(TestKinematicChain);

int main()
{
  // the number of errors encountered in this test
  int errors = 0;
  
  // create instance of test class
  TestKinematicChainPtr test_arm(new TestKinematicChain());
  TestKinematicChainPtr acceleration_test_arm(new TestKinematicChain());
  TestKinematicChainPtr complex_test_arm(new TestKinematicChain());
  std::cout << "reading configuration from test_arm->json" << std::endl;
  test_arm->readJson("test://unit/devices/KinematicChain/test_arm.json");
  acceleration_test_arm->readJson("test://unit/devices/KinematicChain/acceleration_test_arm.json");
  complex_test_arm->readJson("resource://configs/complex_test_arm.json");

  cv::Mat theta;
  cv::Mat thetaDot;
  cv::Mat thetaTwoDot;
  
  //--------------------------------------------------------------------------------------------------------------------
  // check configuration
  //--------------------------------------------------------------------------------------------------------------------

  std::cout << "checking the root coordinate frame..." << std::endl;
  cedar::aux::LocalCoordinateFramePtr rootCoordinateFrame = test_arm->getRootCoordinateFrame();
  if
  (
      !cedar::aux::math::isZero
       (
         rootCoordinateFrame->getTranslationX() / cedar::unit::DEFAULT_LENGTH_UNIT - 2.0
       )
    || !cedar::aux::math::isZero
        (
          rootCoordinateFrame->getTranslationY() / cedar::unit::DEFAULT_LENGTH_UNIT - 0.0
        )
    || !cedar::aux::math::isZero
        (
          rootCoordinateFrame->getTranslationZ() / cedar::unit::DEFAULT_LENGTH_UNIT - 0.0
        )
  )
  {
    errors++;
    std::cout << "ERROR with root coordinate frame translation, read:" << std::endl;
    std::cout << rootCoordinateFrame->getTranslation().matrix;
  }
  cv::Mat rootRotation = rootCoordinateFrame->getRotation();
  if
  (
    !cedar::aux::math::isZero(rootRotation.at<double>(0, 0) - (0.0))
    || !cedar::aux::math::isZero(rootRotation.at<double>(0, 1) - (-1.0))
    || !cedar::aux::math::isZero(rootRotation.at<double>(0, 2) - (0.0))
    || !cedar::aux::math::isZero(rootRotation.at<double>(1, 0) - (1.0))
    || !cedar::aux::math::isZero(rootRotation.at<double>(1, 1) - (0.0))
    || !cedar::aux::math::isZero(rootRotation.at<double>(1, 2) - (0.0))
    || !cedar::aux::math::isZero(rootRotation.at<double>(2, 0) - (0.0))
    || !cedar::aux::math::isZero(rootRotation.at<double>(2, 1) - (0.0))
    || !cedar::aux::math::isZero(rootRotation.at<double>(2, 2) - (1.0))
  )
  {
    errors++;
    std::cout << "ERROR with root coordinate frame rotation, read:" << std::endl;
    cedar::aux::write(rootRotation);
  }

  std::cout << "checking the end-effector coordinate frame..." << std::endl;
  cedar::aux::LocalCoordinateFramePtr endEffectorCoordinateFrame = test_arm->getEndEffectorCoordinateFrame();
  if
  (
       !cedar::aux::math::isZero
        (
          endEffectorCoordinateFrame->getTranslationX() / cedar::unit::DEFAULT_LENGTH_UNIT - 0.0
        )
    || !cedar::aux::math::isZero
        (
          endEffectorCoordinateFrame->getTranslationY() / cedar::unit::DEFAULT_LENGTH_UNIT - 2.0
        )
    || !cedar::aux::math::isZero
        (
          endEffectorCoordinateFrame->getTranslationZ() / cedar::unit::DEFAULT_LENGTH_UNIT - 8.0
        )
  )
  {
    errors++;
    std::cout << "ERROR with end-effector coordinate frame translation, read:" << std::endl;
    std::cout << endEffectorCoordinateFrame->getTranslation();
  }
  cv::Mat eefRotation = endEffectorCoordinateFrame->getRotation();
  if
  (
    !cedar::aux::math::isZero(eefRotation.at<double>(0, 0) - (1.0))
    || !cedar::aux::math::isZero(eefRotation.at<double>(0, 1) - (0.0))
    || !cedar::aux::math::isZero(eefRotation.at<double>(0, 2) - (0.0))
    || !cedar::aux::math::isZero(eefRotation.at<double>(1, 0) - (0.0))
    || !cedar::aux::math::isZero(eefRotation.at<double>(1, 1) - (1.0))
    || !cedar::aux::math::isZero(eefRotation.at<double>(1, 2) - (0.0))
    || !cedar::aux::math::isZero(eefRotation.at<double>(2, 0) - (0.0))
    || !cedar::aux::math::isZero(eefRotation.at<double>(2, 1) - (0.0))
    || !cedar::aux::math::isZero(eefRotation.at<double>(2, 2) - (1.0))
  )
  {
    errors++;
    std::cout << "ERROR with end-effector coordinate frame rotation, read:" << std::endl;
    cedar::aux::write(eefRotation);
  }

  std::cout << "checking the first joint ..." << std::endl;
  cedar::dev::KinematicChain::JointPtr joint = test_arm->getJoint(0);
  if
  (
    !cedar::aux::math::isZero(joint->_mpPosition->at(0) - (0.0))
    || !cedar::aux::math::isZero(joint->_mpPosition->at(1) - (2.0))
    || !cedar::aux::math::isZero(joint->_mpPosition->at(2) - (0.0))
  )
  {
    errors++;
    std::cout << "ERROR with first joint position, read: "
              << joint->_mpPosition->at(0) << ", "
              << joint->_mpPosition->at(1) << ", "
              << joint->_mpPosition->at(2) << std::endl;
  }
  if
  (
    !cedar::aux::math::isZero(joint->_mpAxis->at(0) - (0.0))
    || !cedar::aux::math::isZero(joint->_mpAxis->at(1) - (1.0))
    || !cedar::aux::math::isZero(joint->_mpAxis->at(2) - (0.0))
  )
  {
    errors++;
    std::cout << "ERROR with first joint axis, read: "
              << joint->_mpAxis->at(0) << ", "
              << joint->_mpAxis->at(1) << ", "
              << joint->_mpAxis->at(2) << std::endl;
  }
  if
  (
    !cedar::aux::math::isZero(joint->_mpAngleLimits->getLowerLimit() - (-3.1415927))
    || !cedar::aux::math::isZero(joint->_mpAngleLimits->getUpperLimit() - (3.1415927))
  )
  {
    errors++;
    std::cout << "ERROR with first joint axis limits, read: "
              << joint->_mpAngleLimits->getLowerLimit() << ", "
              << joint->_mpAngleLimits->getUpperLimit() << std::endl;
  }
  if
  (
    !cedar::aux::math::isZero(joint->_mpVelocityLimits->getLowerLimit() - (-5.0))
    || !cedar::aux::math::isZero(joint->_mpVelocityLimits->getUpperLimit() - (5.0))
  )
  {
    errors++;
    std::cout << "ERROR with first joint velocity limits, read: "
              << joint->_mpVelocityLimits->getLowerLimit() << ", "
              << joint->_mpVelocityLimits->getUpperLimit() << std::endl;
  }

  //--------------------------------------------------------------------------------------------------------------------
  // simple service functions
  //--------------------------------------------------------------------------------------------------------------------
  std::cout << "test: getNumberOfJoints()" << std::endl;
  if (test_arm->getNumberOfJoints() != 4)
  {
    errors++;
    std::cout << "ERROR with getNumberOfJoints(), read: " << test_arm->getNumberOfJoints() << std::endl;
  }
  
  std::cout << "test: set/get joint angle/velocity functions" << std::endl;
  test_arm->setJointAngle(2, -cedar::aux::math::pi*0.5);
  test_arm->setJointAngle(3, cedar::aux::math::pi*0.5);
  test_arm->setJointVelocity(1, 1);
  test_arm->setJointVelocity(2, 1);
  test_arm->updateTransformations();
  if
  (
    !cedar::aux::math::isZero(test_arm->getJointAngle(0) - (0.0))
    || !cedar::aux::math::isZero(test_arm->getJointAngle(1) - (0.0))
    || !cedar::aux::math::isZero(test_arm->getJointAngle(2) - (-cedar::aux::math::pi*0.5))
    || !cedar::aux::math::isZero(test_arm->getJointAngle(3) - (cedar::aux::math::pi*0.5))
    || !cedar::aux::math::isZero(test_arm->getJointVelocity(1) - (1.0))
    || !cedar::aux::math::isZero(test_arm->getJointVelocity(2) - (1.0))
  )
  {
    errors++;
    std::cout << "ERROR with set/get joint acceleration" << std::endl;
  }
  acceleration_test_arm->setJointAngle(0, 0.1);
  acceleration_test_arm->setJointAngle(1, 0.2);
  acceleration_test_arm->setJointVelocity(0, 1.1);
  acceleration_test_arm->setJointVelocity(1, 1.2);
  acceleration_test_arm->setJointAcceleration(0, 2.1);
  acceleration_test_arm->setJointAcceleration(1, 2.2);
  acceleration_test_arm->updateTransformations();
  if
  (
    !cedar::aux::math::isZero(acceleration_test_arm->getJointAcceleration(0) - (2.1))
    || !cedar::aux::math::isZero(acceleration_test_arm->getJointAcceleration(1) - (2.2))
  )
  {
    errors++;
    std::cout << "ERROR with set/get joint acceleration" << std::endl;
  }

  //--------------------------------------------------------------------------------------------------------------------
  // transformations
  //--------------------------------------------------------------------------------------------------------------------
  std::cout << "test: transformations" << std::endl;
  cv::Mat joint_transformation_1 = test_arm->getJointTransformation(1);
  cv::Mat joint_transformation_3 = test_arm->getJointTransformation(3);
  if (
      // transformation to joint 1 frame
      !cedar::aux::math::isZero(joint_transformation_1.at<double>(0, 0) - 0)
      || !cedar::aux::math::isZero(joint_transformation_1.at<double>(0, 1) - -1)
      || !cedar::aux::math::isZero(joint_transformation_1.at<double>(0, 2) - 0)

      || !cedar::aux::math::isZero(joint_transformation_1.at<double>(1, 0) - 1)
      || !cedar::aux::math::isZero(joint_transformation_1.at<double>(1, 1) - 0)
      || !cedar::aux::math::isZero(joint_transformation_1.at<double>(1, 2) - 0)

      || !cedar::aux::math::isZero(joint_transformation_1.at<double>(2, 0) - 0)
      || !cedar::aux::math::isZero(joint_transformation_1.at<double>(2, 1) - 0)
      || !cedar::aux::math::isZero(joint_transformation_1.at<double>(2, 2) - 1)

      || !cedar::aux::math::isZero(joint_transformation_1.at<double>(0, 3) - 0)
      || !cedar::aux::math::isZero(joint_transformation_1.at<double>(1, 3) - 0)
      || !cedar::aux::math::isZero(joint_transformation_1.at<double>(2, 3) - 2)
      || !cedar::aux::math::isZero(joint_transformation_1.at<double>(3, 0) - 0)
      || !cedar::aux::math::isZero(joint_transformation_1.at<double>(3, 1) - 0)
      || !cedar::aux::math::isZero(joint_transformation_1.at<double>(3, 2) - 0)
      || !cedar::aux::math::isZero(joint_transformation_1.at<double>(3, 3) - 1)
      // transformation to joint 3 frame
      || !cedar::aux::math::isZero(joint_transformation_3.at<double>(0, 0) - 0)
      || !cedar::aux::math::isZero(joint_transformation_3.at<double>(0, 1) - -1)
      || !cedar::aux::math::isZero(joint_transformation_3.at<double>(0, 2) - 0)
      || !cedar::aux::math::isZero(joint_transformation_3.at<double>(1, 0) - 1)
      || !cedar::aux::math::isZero(joint_transformation_3.at<double>(1, 1) - 0)
      || !cedar::aux::math::isZero(joint_transformation_3.at<double>(1, 2) - 0)
      || !cedar::aux::math::isZero(joint_transformation_3.at<double>(2, 0) - 0)

      || !cedar::aux::math::isZero(joint_transformation_3.at<double>(2, 1) - 0)
      || !cedar::aux::math::isZero(joint_transformation_3.at<double>(2, 2) - 1)
      || !cedar::aux::math::isZero(joint_transformation_3.at<double>(0, 3) - 0)
      || !cedar::aux::math::isZero(joint_transformation_3.at<double>(1, 3) - -2)
      || !cedar::aux::math::isZero(joint_transformation_3.at<double>(2, 3) - 4)
      || !cedar::aux::math::isZero(joint_transformation_3.at<double>(3, 0) - 0)
      || !cedar::aux::math::isZero(joint_transformation_3.at<double>(3, 1) - 0)
      || !cedar::aux::math::isZero(joint_transformation_3.at<double>(3, 2) - 0)
      || !cedar::aux::math::isZero(joint_transformation_3.at<double>(3, 3) - 1)
      )
  {
    errors++;
    std::cout << "ERROR with calculateTransformations() or getJointTransformation()" << std::endl;
  }

  //--------------------------------------------------------------------------------------------------------------------
  // Jacobians
  //--------------------------------------------------------------------------------------------------------------------
  std::cout << "test: Jacobians" << std::endl;
  cv::Mat origin = cv::Mat::zeros( 4, 1, CV_64FC1 );
  origin.at<double>( 3, 0 ) = 1;
  cv::Mat jacobian_1 = cv::Mat::zeros(3, 4, CV_64FC1);
  cv::Mat jacobian_3 = cv::Mat::zeros(3, 4, CV_64FC1);
  test_arm->calculateCartesianJacobian(origin, 1, jacobian_1, cedar::dev::KinematicChain::LOCAL_COORDINATES);
  test_arm->calculateCartesianJacobian(origin, 3, jacobian_3, cedar::dev::KinematicChain::LOCAL_COORDINATES);
  if (
      // Jacobian of joint 1
      !cedar::aux::math::isZero(jacobian_1.at<double>(0, 0) - 0)
      || !cedar::aux::math::isZero(jacobian_1.at<double>(1, 0) - 2)
      || !cedar::aux::math::isZero(jacobian_1.at<double>(2, 0) - 0)
      || !cedar::aux::math::isZero(jacobian_1.at<double>(0, 1) - 0)
      || !cedar::aux::math::isZero(jacobian_1.at<double>(1, 1) - 0)
      || !cedar::aux::math::isZero(jacobian_1.at<double>(2, 1) - 0)
      || !cedar::aux::math::isZero(jacobian_1.at<double>(0, 2) - 0)
      || !cedar::aux::math::isZero(jacobian_1.at<double>(1, 2) - 0)
      || !cedar::aux::math::isZero(jacobian_1.at<double>(2, 2) - 0)
      || !cedar::aux::math::isZero(jacobian_1.at<double>(0, 3) - 0)
      || !cedar::aux::math::isZero(jacobian_1.at<double>(1, 3) - 0)
      || !cedar::aux::math::isZero(jacobian_1.at<double>(2, 3) - 0)
      || !cedar::aux::math::isZero
      (
        norm
        (
          jacobian_1
          - test_arm->calculateCartesianJacobian(origin, 1, cedar::dev::KinematicChain::LOCAL_COORDINATES)
        )
      )
      // Jacobian of joint 3
      || !cedar::aux::math::isZero(jacobian_3.at<double>(0, 0) - 0)
      || !cedar::aux::math::isZero(jacobian_3.at<double>(1, 0) - 4)
      || !cedar::aux::math::isZero(jacobian_3.at<double>(2, 0) - 2)
      || !cedar::aux::math::isZero(jacobian_3.at<double>(0, 1) - 0)
      || !cedar::aux::math::isZero(jacobian_3.at<double>(1, 1) - 2)
      || !cedar::aux::math::isZero(jacobian_3.at<double>(2, 1) - 2)
      || !cedar::aux::math::isZero(jacobian_3.at<double>(0, 2) - 0)
      || !cedar::aux::math::isZero(jacobian_3.at<double>(1, 2) - 0)
      || !cedar::aux::math::isZero(jacobian_3.at<double>(2, 2) - 2)
      || !cedar::aux::math::isZero(jacobian_3.at<double>(0, 3) - 0)
      || !cedar::aux::math::isZero(jacobian_3.at<double>(1, 3) - 0)
      || !cedar::aux::math::isZero(jacobian_3.at<double>(2, 3) - 0)
      || !cedar::aux::math::isZero
      (
        norm
        (
          jacobian_3
          - test_arm->calculateCartesianJacobian(origin, 3, cedar::dev::KinematicChain::LOCAL_COORDINATES)
        )
      )
    )
  {
    errors++;
    std::cout << "ERROR with calculateCartesianJacobian()" << std::endl;
  }

  std::cout << "test: calculateSpatialJacobian" << std::endl;
  cv::Mat spatial_jacobian = test_arm->calculateSpatialJacobian(test_arm->getNumberOfJoints()-1);
  if (
      !cedar::aux::math::isZero(spatial_jacobian.at<double>(0, 0) - 0)
      || !cedar::aux::math::isZero(spatial_jacobian.at<double>(1, 0) - 0)
      || !cedar::aux::math::isZero(spatial_jacobian.at<double>(2, 0) - 0)
      || !cedar::aux::math::isZero(spatial_jacobian.at<double>(3, 0) - -1)
      || !cedar::aux::math::isZero(spatial_jacobian.at<double>(4, 0) - 0)
      || !cedar::aux::math::isZero(spatial_jacobian.at<double>(5, 0) - 0)

      || !cedar::aux::math::isZero(spatial_jacobian.at<double>(0, 1) - 0)
      || !cedar::aux::math::isZero(spatial_jacobian.at<double>(1, 1) - -2)
      || !cedar::aux::math::isZero(spatial_jacobian.at<double>(2, 1) - 0)
      || !cedar::aux::math::isZero(spatial_jacobian.at<double>(3, 1) - -1)
      || !cedar::aux::math::isZero(spatial_jacobian.at<double>(4, 1) - 0)
      || !cedar::aux::math::isZero(spatial_jacobian.at<double>(5, 1) - 0)

      || !cedar::aux::math::isZero(spatial_jacobian.at<double>(0, 2) - 0)
      || !cedar::aux::math::isZero(spatial_jacobian.at<double>(1, 2) - -4)
      || !cedar::aux::math::isZero(spatial_jacobian.at<double>(2, 2) - 0)
      || !cedar::aux::math::isZero(spatial_jacobian.at<double>(3, 2) - -1)
      || !cedar::aux::math::isZero(spatial_jacobian.at<double>(4, 2) - 0)
      || !cedar::aux::math::isZero(spatial_jacobian.at<double>(5, 2) - 0)

      || !cedar::aux::math::isZero(spatial_jacobian.at<double>(0, 3) - 0)
      || !cedar::aux::math::isZero(spatial_jacobian.at<double>(1, 3) - -4)
      || !cedar::aux::math::isZero(spatial_jacobian.at<double>(2, 3) - -2)
      || !cedar::aux::math::isZero(spatial_jacobian.at<double>(3, 3) - -1)
      || !cedar::aux::math::isZero(spatial_jacobian.at<double>(4, 3) - 0)
      || !cedar::aux::math::isZero(spatial_jacobian.at<double>(5, 3) - 0)
     )
  {
    errors++;
    std::cout << "ERROR with spatialJacobian()" << std::endl;
  }

  //--------------------------------------------------------------------------------------------------------------------
  // end-effector position
  //--------------------------------------------------------------------------------------------------------------------
  std::cout << "test: calculateEndEffectorPosition" << std::endl;
  cv::Mat end_effector_position = test_arm->calculateEndEffectorPosition();
  if (
      !cedar::aux::math::isZero(end_effector_position.at<double>(0, 0) - 0)
      || !cedar::aux::math::isZero(end_effector_position.at<double>(1, 0) - -2)
      || !cedar::aux::math::isZero(end_effector_position.at<double>(2, 0) - 6)
      || !cedar::aux::math::isZero(end_effector_position.at<double>(3, 0) - 1)
     )
  {
    errors++;
    std::cout << "ERROR with calculateEndEffectorPosition()" << std::endl;
  }

  //--------------------------------------------------------------------------------------------------------------------
  // end-effector transformation
  //--------------------------------------------------------------------------------------------------------------------
  std::cout << "test: calculateEndEffectorTransformation" << std::endl;
  cv::Mat end_effector_transformation = test_arm->getEndEffectorTransformation();
  if (
      !cedar::aux::math::isZero(end_effector_transformation.at<double>(0, 0) - 0)
      || !cedar::aux::math::isZero(end_effector_transformation.at<double>(0, 1) - -1)
      || !cedar::aux::math::isZero(end_effector_transformation.at<double>(0, 2) - 0)
      || !cedar::aux::math::isZero(end_effector_transformation.at<double>(1, 0) - 1)
      || !cedar::aux::math::isZero(end_effector_transformation.at<double>(1, 1) - 0)
      || !cedar::aux::math::isZero(end_effector_transformation.at<double>(1, 2) - 0)
      || !cedar::aux::math::isZero(end_effector_transformation.at<double>(2, 0) - 0)
      || !cedar::aux::math::isZero(end_effector_transformation.at<double>(2, 1) - 0)
      || !cedar::aux::math::isZero(end_effector_transformation.at<double>(2, 2) - 1)
      || !cedar::aux::math::isZero(end_effector_transformation.at<double>(0, 3) - 0)
      || !cedar::aux::math::isZero(end_effector_transformation.at<double>(1, 3) - -2)
      || !cedar::aux::math::isZero(end_effector_transformation.at<double>(2, 3) - 6)
      || !cedar::aux::math::isZero(end_effector_transformation.at<double>(3, 0) - 0)
      || !cedar::aux::math::isZero(end_effector_transformation.at<double>(3, 1) - 0)
      || !cedar::aux::math::isZero(end_effector_transformation.at<double>(3, 2) - 0)
      || !cedar::aux::math::isZero(end_effector_transformation.at<double>(3, 3) - 1)
     )
  {
    errors++;
    std::cout << "ERROR with calculateEndEffectorTransformation()" << std::endl;
  }

  //--------------------------------------------------------------------------------------------------------------------
  // end-effector jacobian
  //--------------------------------------------------------------------------------------------------------------------
  std::cout << "test: calculateEndEffectorJacobian" << std::endl;
  cv::Mat end_effector_jacobian = test_arm->calculateEndEffectorJacobian();
  if (
      !cedar::aux::math::isZero(end_effector_jacobian.at<double>(0, 0) - 0)
      || !cedar::aux::math::isZero(end_effector_jacobian.at<double>(1, 0) - 6)
      || !cedar::aux::math::isZero(end_effector_jacobian.at<double>(2, 0) - 2)
      || !cedar::aux::math::isZero(end_effector_jacobian.at<double>(0, 1) - 0)
      || !cedar::aux::math::isZero(end_effector_jacobian.at<double>(1, 1) - 4)
      || !cedar::aux::math::isZero(end_effector_jacobian.at<double>(2, 1) - 2)
      || !cedar::aux::math::isZero(end_effector_jacobian.at<double>(0, 2) - 0)
      || !cedar::aux::math::isZero(end_effector_jacobian.at<double>(1, 2) - 2)
      || !cedar::aux::math::isZero(end_effector_jacobian.at<double>(2, 2) - 2)
      || !cedar::aux::math::isZero(end_effector_jacobian.at<double>(0, 3) - 0)
      || !cedar::aux::math::isZero(end_effector_jacobian.at<double>(1, 3) - 2)
      || !cedar::aux::math::isZero(end_effector_jacobian.at<double>(2, 3) - 0)
     )
  {
    errors++;
    std::cout << "ERROR with calculateEndEffectorJacobian()" << std::endl;
  }

  //--------------------------------------------------------------------------------------------------------------------
  // calculate finite differences for time derivatives
  //--------------------------------------------------------------------------------------------------------------------

  cv::Mat p_local = cv::Mat::zeros(4, 1, CV_64FC1);
  p_local.at<double>(2, 0) = 1.0;
  p_local.at<double>(3, 0) = 1.0;
  theta = cv::Mat(4, 1, CV_64FC1);
  thetaDot = cv::Mat(4, 1, CV_64FC1);
  thetaTwoDot = cv::Mat(4, 1, CV_64FC1);
  cv::randn(theta, cv::Scalar(0), cv::Scalar(1));
  cv::randn(thetaDot, cv::Scalar(0), cv::Scalar(1));
  cv::randn(thetaTwoDot, cv::Scalar(0), cv::Scalar(1));
  complex_test_arm->setJointAngles(theta);
  complex_test_arm->setJointVelocities(thetaDot);
  complex_test_arm->setJointAccelerations(thetaTwoDot);
  complex_test_arm->updateTransformations();

  // compute variable values
  cv::Mat root_transformation = complex_test_arm->getRootCoordinateFrame()->getTransformation();
  cv::Mat p0 = root_transformation * complex_test_arm->getJointTransformation(0) * p_local;
  cv::Mat p1 = root_transformation * complex_test_arm->getJointTransformation(1) * p_local;
  cv::Mat p2 = root_transformation * complex_test_arm->getJointTransformation(2) * p_local;
  cv::Mat p3 = root_transformation * complex_test_arm->getJointTransformation(3) * p_local;
  cv::Mat v0 = complex_test_arm->calculateVelocity(p_local, 0, cedar::dev::KinematicChain::LOCAL_COORDINATES);
  cv::Mat v1 = complex_test_arm->calculateVelocity(p_local, 1, cedar::dev::KinematicChain::LOCAL_COORDINATES);
  cv::Mat v2 = complex_test_arm->calculateVelocity(p_local, 2, cedar::dev::KinematicChain::LOCAL_COORDINATES);
  cv::Mat v3 = complex_test_arm->calculateVelocity(p_local, 3, cedar::dev::KinematicChain::LOCAL_COORDINATES);
  cv::Mat a0 = complex_test_arm->calculateAcceleration(p_local, 0, cedar::dev::KinematicChain::LOCAL_COORDINATES);
  cv::Mat a1 = complex_test_arm->calculateAcceleration(p_local, 1, cedar::dev::KinematicChain::LOCAL_COORDINATES);
  cv::Mat a2 = complex_test_arm->calculateAcceleration(p_local, 2, cedar::dev::KinematicChain::LOCAL_COORDINATES);
  cv::Mat a3 = complex_test_arm->calculateAcceleration(p_local, 3, cedar::dev::KinematicChain::LOCAL_COORDINATES);
  spatial_jacobian
    = complex_test_arm->calculateSpatialJacobian(complex_test_arm->getNumberOfJoints() - 1);
  cv::Mat cartesian_jacobian = complex_test_arm->calculateCartesianJacobian
  (
    p_local,
    complex_test_arm->getNumberOfJoints()-1,
    cedar::dev::KinematicChain::LOCAL_COORDINATES
  );
  cv::Mat p_eef = complex_test_arm->calculateEndEffectorPosition();
  cv::Mat v_eef = complex_test_arm->calculateEndEffectorVelocity();
  cv::Mat a_eef = complex_test_arm->calculateEndEffectorAcceleration();

  // compute temporal derivatives in old configuration
  cv::Mat spatial_jacobian_dot
    = complex_test_arm->calculateSpatialJacobianTemporalDerivative(complex_test_arm->getNumberOfJoints() - 1);
  cv::Mat cartesian_jacobian_dot = complex_test_arm->calculateCartesianJacobianTemporalDerivative
  (
    p_local,
    complex_test_arm->getNumberOfJoints()-1,
    cedar::dev::KinematicChain::LOCAL_COORDINATES
  );

  // make a small step for finite difference method
  double delta_t = 1e-07;
  complex_test_arm->setJointAngles
  (
    complex_test_arm->getCachedJointAngles()
    + delta_t*complex_test_arm->getCachedJointVelocities()
    + delta_t*delta_t*complex_test_arm->getCachedJointAccelerations()
  );
  complex_test_arm->setJointVelocities
  (
    complex_test_arm->getCachedJointVelocities()
    + delta_t*complex_test_arm->getCachedJointAccelerations()
  );
  complex_test_arm->updateTransformations();

  // compute new values
  cv::Mat p0_new = root_transformation * complex_test_arm->getJointTransformation(0) * p_local;
  cv::Mat p1_new = root_transformation * complex_test_arm->getJointTransformation(1) * p_local;
  cv::Mat p2_new = root_transformation * complex_test_arm->getJointTransformation(2) * p_local;
  cv::Mat p3_new = root_transformation * complex_test_arm->getJointTransformation(3) * p_local;
  cv::Mat v0_new = complex_test_arm->calculateVelocity(p_local, 0, cedar::dev::KinematicChain::LOCAL_COORDINATES);
  cv::Mat v1_new = complex_test_arm->calculateVelocity(p_local, 1, cedar::dev::KinematicChain::LOCAL_COORDINATES);
  cv::Mat v2_new = complex_test_arm->calculateVelocity(p_local, 2, cedar::dev::KinematicChain::LOCAL_COORDINATES);
  cv::Mat v3_new = complex_test_arm->calculateVelocity(p_local, 3, cedar::dev::KinematicChain::LOCAL_COORDINATES);
  cv::Mat a0_new = complex_test_arm->calculateAcceleration(p_local, 0, cedar::dev::KinematicChain::LOCAL_COORDINATES);
  cv::Mat a1_new = complex_test_arm->calculateAcceleration(p_local, 1, cedar::dev::KinematicChain::LOCAL_COORDINATES);
  cv::Mat a2_new = complex_test_arm->calculateAcceleration(p_local, 2, cedar::dev::KinematicChain::LOCAL_COORDINATES);
  cv::Mat a3_new = complex_test_arm->calculateAcceleration(p_local, 3, cedar::dev::KinematicChain::LOCAL_COORDINATES);
  cv::Mat spatial_jacobian_new
    = complex_test_arm->calculateSpatialJacobian(complex_test_arm->getNumberOfJoints() - 1);
  cv::Mat cartesian_jacobian_new = complex_test_arm->calculateCartesianJacobian
  (
    p_local,
    complex_test_arm->getNumberOfJoints()-1,
    cedar::dev::KinematicChain::LOCAL_COORDINATES
  );
  cv::Mat p_eef_new = complex_test_arm->calculateEndEffectorPosition();
  cv::Mat v_eef_new = complex_test_arm->calculateEndEffectorVelocity();
  cv::Mat a_eef_new = complex_test_arm->calculateEndEffectorAcceleration();

  // approximate time derivatives with finite difference method
  cv::Mat v0_numeric = (p0_new - p0) * 1 / delta_t;
  cv::Mat v1_numeric = (p1_new - p1) * 1 / delta_t;
  cv::Mat v2_numeric = (p2_new - p2) * 1 / delta_t;
  cv::Mat v3_numeric = (p3_new - p3) * 1 / delta_t;
  cv::Mat a0_numeric = (v0_new - v0) * 1 / delta_t;
  cv::Mat a1_numeric = (v1_new - v1) * 1 / delta_t;
  cv::Mat a2_numeric = (v2_new - v2) * 1 / delta_t;
  cv::Mat a3_numeric = (v3_new - v3) * 1 / delta_t;
  cv::Mat spatial_jacobian_dot_numeric = (spatial_jacobian_new - spatial_jacobian) * 1 / delta_t;
  cv::Mat cartesian_jacobian_dot_numeric = (cartesian_jacobian_new - cartesian_jacobian) * 1 / delta_t;
  cv::Mat v_eef_numeric = (p_eef_new - p_eef) * 1 / delta_t;
  cv::Mat a_eef_numeric = (v_eef_new - v_eef) * 1 / delta_t;

  //--------------------------------------------------------------------------------------------------------------------
  // velocity
  //--------------------------------------------------------------------------------------------------------------------
  std::cout << "test: calculateVelocity" << std::endl;
  if (
      !cedar::aux::math::isZero(pow(norm(v0 - v0_numeric)/3.0, 2))
      || !cedar::aux::math::isZero(pow(norm(v1 - v1_numeric)/3.0, 2))
      || !cedar::aux::math::isZero(pow(norm(v2 - v2_numeric)/3.0, 2))
      || !cedar::aux::math::isZero(pow(norm(v3 - v3_numeric)/3.0, 2))
     )
  {
    errors++;
    std::cout << "ERROR with calculateVelocity()" << std::endl;
  }

  //--------------------------------------------------------------------------------------------------------------------
  // acceleration
  //--------------------------------------------------------------------------------------------------------------------
  std::cout << "test: calculateAcceleration" << std::endl;
  if (
      !cedar::aux::math::isZero(pow(norm(a0 - a0_numeric)/3.0, 2))
      || !cedar::aux::math::isZero(pow(norm(a1 - a1_numeric)/3.0, 2))
      || !cedar::aux::math::isZero(pow(norm(a2 - a2_numeric)/3.0, 2))
      || !cedar::aux::math::isZero(pow(norm(a3 - a3_numeric)/3.0, 2))
     )
  {
    errors++;
    std::cout << "ERROR with calculateAcceleration()" << std::endl;
  }

  //--------------------------------------------------------------------------------------------------------------------
  // spatial Jacobian temporal derivative
  //--------------------------------------------------------------------------------------------------------------------
  std::cout << "test: calculateSpatialJacobianTemporalDerivative" << std::endl;
  if (
      !cedar::aux::math::isZero(pow(norm(spatial_jacobian_dot - spatial_jacobian_dot_numeric)/(spatial_jacobian_dot.cols*6), 2))
     )
  {
    errors++;
    std::cout << "ERROR with calculateSpatialJacobianTemporalDerivative(...)" << std::endl;
  }

  //--------------------------------------------------------------------------------------------------------------------
  // Cartesian Jacobian temporal derivative
  //--------------------------------------------------------------------------------------------------------------------
  std::cout << "test: calculateCartesianJacobianTemporalDerivative" << std::endl;
  if (
      !cedar::aux::math::isZero(pow(norm(cartesian_jacobian_dot - cartesian_jacobian_dot_numeric)/(cartesian_jacobian_dot.cols*3), 2))
     )
  {
    errors++;
    std::cout << "ERROR with calculateCartesianJacobianTemporalDerivative(...)" << std::endl;
  }

  //--------------------------------------------------------------------------------------------------------------------
  // end-effector velocity
  //--------------------------------------------------------------------------------------------------------------------
  std::cout << "test: calculateEndEffectorVelocity" << std::endl;
  if (!cedar::aux::math::isZero(pow(norm(v_eef - v_eef_numeric)/3.0, 2)))
  {
    errors++;
    std::cout << "ERROR with calculateEndEffectorVelocity()" << std::endl;
  }

  //--------------------------------------------------------------------------------------------------------------------
  // end-effector acceleration
  //--------------------------------------------------------------------------------------------------------------------
  std::cout << "test: calculateEndEffectorAcceleration" << std::endl;
  if (!cedar::aux::math::isZero(pow(norm(a_eef - a_eef_numeric)/3.0, 2)))
  {
    errors++;
    std::cout << "ERROR with calculateEndEffectorAcceleration()" << std::endl;
  }

  //--------------------------------------------------------------------------------------------------------------------
  // end of test function
  //--------------------------------------------------------------------------------------------------------------------

  std::cout << "test finished, there were " << errors << " errors" << std::endl;
  if (errors > 255)
  {
    errors = 255;
  }
  return errors;
}
