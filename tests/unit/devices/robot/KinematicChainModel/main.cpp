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
 
 ----- File:        main.cpp
 
 ----- Author:      Hendrik Reimann
 ----- Email:       hendrik.reimann@ini.rub.de
 ----- Date:        2010 11 28
 
 ----- Description: unit test for the @em cedar::dev::robot::KinematicChainModel class.
 
 ----- Credits:
 ---------------------------------------------------------------------------------------------------------------------*/

// LOCAL INCLUDES

// PROJECT INCLUDES
#include "cedar/devices/robot/ReferenceGeometry.h"
#include "cedar/devices/robot/SimulatedKinematicChain.h"
#include "cedar/devices/robot/KinematicChainModel.h"
#include "cedar/auxiliaries/math/tools.h"
#include "cedar/auxiliaries/math/constants.h"

// SYSTEM INCLUDES


int main()
{
  // the number of errors encountered in this test
  int errors = 0;
  
  // create instances of test class
  cedar::dev::robot::ReferenceGeometryPtr p_reference_geometry(new cedar::dev::robot::ReferenceGeometry("test.conf"));
  cedar::dev::robot::KinematicChainPtr p_test_arm(new cedar::dev::robot::SimulatedKinematicChain(p_reference_geometry));
  cedar::dev::robot::KinematicChainModel test_arm_model(p_test_arm);

  cedar::dev::robot::ReferenceGeometryPtr p_acceleration_reference_geometry(new cedar::dev::robot::ReferenceGeometry("acceleration_test_arm.conf"));
  cedar::dev::robot::KinematicChainPtr p_acceleration_test_arm (new cedar::dev::robot::SimulatedKinematicChain(p_acceleration_reference_geometry));
  cedar::dev::robot::KinematicChainModel acceleration_test_arm_model(p_acceleration_test_arm);

  // set test configurations
  p_test_arm->setJointAngle(2, -cedar::aux::math::pi*0.5);
  p_test_arm->setJointAngle(3, cedar::aux::math::pi*0.5);
  p_test_arm->setJointVelocity(1, 1);
  p_test_arm->setJointVelocity(2, 1);
  test_arm_model.update();

  p_acceleration_test_arm->setJointAngle(0, 0.1);
  p_acceleration_test_arm->setJointAngle(1, 0.2);
  p_acceleration_test_arm->setJointVelocity(0, 1.1);
  p_acceleration_test_arm->setJointVelocity(1, 1.2);
  acceleration_test_arm_model.update();

  //--------------------------------------------------------------------------------------------------------------------
  // number of joints
  //--------------------------------------------------------------------------------------------------------------------
  std::cout << "test: getNumberOfJoints" << std::endl;
  if (test_arm_model.getNumberOfJoints() != 4)
  {
    errors++;
    std::cout << "ERROR with getNumberOfJoints()" << std::endl;
  }

  //--------------------------------------------------------------------------------------------------------------------
  // transformation
  //--------------------------------------------------------------------------------------------------------------------
  std::cout << "test: transformations" << std::endl;
  cv::Mat joint_transformation_1 = test_arm_model.getJointTransformation(1);
  cv::Mat joint_transformation_3 = test_arm_model.getJointTransformation(3);
  if (
      // transformation to joint 1 frame
      !IsZero(joint_transformation_1.at<double>(0, 0) - 0)
      || !IsZero(joint_transformation_1.at<double>(0, 1) - -1)
      || !IsZero(joint_transformation_1.at<double>(0, 2) - 0)

      || !IsZero(joint_transformation_1.at<double>(1, 0) - 1)
      || !IsZero(joint_transformation_1.at<double>(1, 1) - 0)
      || !IsZero(joint_transformation_1.at<double>(1, 2) - 0)

      || !IsZero(joint_transformation_1.at<double>(2, 0) - 0)
      || !IsZero(joint_transformation_1.at<double>(2, 1) - 0)
      || !IsZero(joint_transformation_1.at<double>(2, 2) - 1)

      || !IsZero(joint_transformation_1.at<double>(0, 3) - 0)
      || !IsZero(joint_transformation_1.at<double>(1, 3) - 0)
      || !IsZero(joint_transformation_1.at<double>(2, 3) - 2)
      || !IsZero(joint_transformation_1.at<double>(3, 0) - 0)
      || !IsZero(joint_transformation_1.at<double>(3, 1) - 0)
      || !IsZero(joint_transformation_1.at<double>(3, 2) - 0)
      || !IsZero(joint_transformation_1.at<double>(3, 3) - 1)
      // transformation to joint 3 frame
      || !IsZero(joint_transformation_3.at<double>(0, 0) - 0)
      || !IsZero(joint_transformation_3.at<double>(0, 1) - -1)
      || !IsZero(joint_transformation_3.at<double>(0, 2) - 0)
      || !IsZero(joint_transformation_3.at<double>(1, 0) - 1)
      || !IsZero(joint_transformation_3.at<double>(1, 1) - 0)
      || !IsZero(joint_transformation_3.at<double>(1, 2) - 0)
      || !IsZero(joint_transformation_3.at<double>(2, 0) - 0)

      || !IsZero(joint_transformation_3.at<double>(2, 1) - 0)
      || !IsZero(joint_transformation_3.at<double>(2, 2) - 1)
      || !IsZero(joint_transformation_3.at<double>(0, 3) - 0)
      || !IsZero(joint_transformation_3.at<double>(1, 3) - -2)
      || !IsZero(joint_transformation_3.at<double>(2, 3) - 4)
      || !IsZero(joint_transformation_3.at<double>(3, 0) - 0)
      || !IsZero(joint_transformation_3.at<double>(3, 1) - 0)
      || !IsZero(joint_transformation_3.at<double>(3, 2) - 0)
      || !IsZero(joint_transformation_3.at<double>(3, 3) - 1)
      )
  {
    errors++;
    std::cout << "ERROR with calculateTransformations() or transformation()" << std::endl;
  }
  
  //--------------------------------------------------------------------------------------------------------------------
  // Jacobians
  //--------------------------------------------------------------------------------------------------------------------
  std::cout << "test: Jacobians" << std::endl;
  cv::Mat origin = cv::Mat::zeros( 4, 1, CV_64FC1 );
  origin.at<double>( 3, 0 ) = 1;
  cv::Mat jacobian_1 = cv::Mat::zeros(3, 4, CV_64FC1);
  cv::Mat jacobian_3 = cv::Mat::zeros(3, 4, CV_64FC1);
  test_arm_model.calculateCartesianJacobian(origin, 1, jacobian_1, cedar::dev::robot::KinematicChainModel::LOCAL_COORDINATES);
  test_arm_model.calculateCartesianJacobian(origin, 3, jacobian_3, cedar::dev::robot::KinematicChainModel::LOCAL_COORDINATES);
  if (
      // Jacobian of joint 1
      !IsZero(jacobian_1.at<double>(0, 0) - 0)
      || !IsZero(jacobian_1.at<double>(1, 0) - 2)
      || !IsZero(jacobian_1.at<double>(2, 0) - 0)
      || !IsZero(jacobian_1.at<double>(0, 1) - 0)
      || !IsZero(jacobian_1.at<double>(1, 1) - 0)
      || !IsZero(jacobian_1.at<double>(2, 1) - 0)
      || !IsZero(jacobian_1.at<double>(0, 2) - 0)
      || !IsZero(jacobian_1.at<double>(1, 2) - 0)
      || !IsZero(jacobian_1.at<double>(2, 2) - 0)
      || !IsZero(jacobian_1.at<double>(0, 3) - 0)
      || !IsZero(jacobian_1.at<double>(1, 3) - 0)
      || !IsZero(jacobian_1.at<double>(2, 3) - 0)
      || !IsZero(norm(jacobian_1 - test_arm_model.calculateCartesianJacobian(origin, 1, cedar::dev::robot::KinematicChainModel::LOCAL_COORDINATES)))
      // Jacobian of joint 3
      || !IsZero(jacobian_3.at<double>(0, 0) - 0)
      || !IsZero(jacobian_3.at<double>(1, 0) - 4)
      || !IsZero(jacobian_3.at<double>(2, 0) - 2)
      || !IsZero(jacobian_3.at<double>(0, 1) - 0)
      || !IsZero(jacobian_3.at<double>(1, 1) - 2)
      || !IsZero(jacobian_3.at<double>(2, 1) - 2)
      || !IsZero(jacobian_3.at<double>(0, 2) - 0)
      || !IsZero(jacobian_3.at<double>(1, 2) - 0)
      || !IsZero(jacobian_3.at<double>(2, 2) - 2)
      || !IsZero(jacobian_3.at<double>(0, 3) - 0)
      || !IsZero(jacobian_3.at<double>(1, 3) - 0)
      || !IsZero(jacobian_3.at<double>(2, 3) - 0)
      || !IsZero(norm(jacobian_3 - test_arm_model.calculateCartesianJacobian(origin, 3, cedar::dev::robot::KinematicChainModel::LOCAL_COORDINATES)))
     )
  {
    errors++;
    std::cout << "ERROR with calculateCartesianJacobian()" << std::endl;
  }
  
  std::cout << "test: calculateSpatialJacobian" << std::endl;
  cv::Mat spatial_jacobian = test_arm_model.calculateSpatialJacobian(test_arm_model.getNumberOfJoints()-1);
  if (
      !IsZero(spatial_jacobian.at<double>(0, 0) - 0)
      || !IsZero(spatial_jacobian.at<double>(1, 0) - 0)
      || !IsZero(spatial_jacobian.at<double>(2, 0) - 0)
      || !IsZero(spatial_jacobian.at<double>(3, 0) - -1)
      || !IsZero(spatial_jacobian.at<double>(4, 0) - 0)
      || !IsZero(spatial_jacobian.at<double>(5, 0) - 0)

      || !IsZero(spatial_jacobian.at<double>(0, 1) - 0)
      || !IsZero(spatial_jacobian.at<double>(1, 1) - -2)
      || !IsZero(spatial_jacobian.at<double>(2, 1) - 0)
      || !IsZero(spatial_jacobian.at<double>(3, 1) - -1)
      || !IsZero(spatial_jacobian.at<double>(4, 1) - 0)
      || !IsZero(spatial_jacobian.at<double>(5, 1) - 0)

      || !IsZero(spatial_jacobian.at<double>(0, 2) - 0)
      || !IsZero(spatial_jacobian.at<double>(1, 2) - -4)
      || !IsZero(spatial_jacobian.at<double>(2, 2) - 0)
      || !IsZero(spatial_jacobian.at<double>(3, 2) - -1)
      || !IsZero(spatial_jacobian.at<double>(4, 2) - 0)
      || !IsZero(spatial_jacobian.at<double>(5, 2) - 0)

      || !IsZero(spatial_jacobian.at<double>(0, 3) - 0)
      || !IsZero(spatial_jacobian.at<double>(1, 3) - -4)
      || !IsZero(spatial_jacobian.at<double>(2, 3) - -2)
      || !IsZero(spatial_jacobian.at<double>(3, 3) - -1)
      || !IsZero(spatial_jacobian.at<double>(4, 3) - 0)
      || !IsZero(spatial_jacobian.at<double>(5, 3) - 0)
     )
  {
    errors++;
    std::cout << "ERROR with spatialJacobian()" << std::endl;
  }
  

  //--------------------------------------------------------------------------------------------------------------------
  // velocity
  //--------------------------------------------------------------------------------------------------------------------
  std::cout << "test: calculateVelocity" << std::endl;
  cv::Mat p = cv::Mat::zeros(4, 1, CV_64FC1);
  p.at<double>(2, 0) = 1.0;
  p.at<double>(3, 0) = 1.0;
  cv::Mat v0 = test_arm_model.calculateVelocity(p, 0, cedar::dev::robot::KinematicChainModel::LOCAL_COORDINATES);
  cv::Mat v1 = test_arm_model.calculateVelocity(p, 1, cedar::dev::robot::KinematicChainModel::LOCAL_COORDINATES);
  cv::Mat v2 = test_arm_model.calculateVelocity(p, 2, cedar::dev::robot::KinematicChainModel::LOCAL_COORDINATES);
  cv::Mat v3 = test_arm_model.calculateVelocity(p, 3, cedar::dev::robot::KinematicChainModel::LOCAL_COORDINATES);
  if (
      !IsZero(v0.at<double>(0, 0) - 0)
      || !IsZero(v0.at<double>(1, 0) - 0)
      || !IsZero(v0.at<double>(2, 0) - 0)
      || !IsZero(v1.at<double>(0, 0) - 0)
      || !IsZero(v1.at<double>(1, 0) - 1)
      || !IsZero(v1.at<double>(2, 0) - 0)
      || !IsZero(v2.at<double>(0, 0) - 0)
      || !IsZero(v2.at<double>(1, 0) - 2)
      || !IsZero(v2.at<double>(2, 0) - 2)
      || !IsZero(v3.at<double>(0, 0) - 0)
      || !IsZero(v3.at<double>(1, 0) - 4)
      || !IsZero(v3.at<double>(2, 0) - 4)
     )
  {
    errors++;
    std::cout << "ERROR with calculateVelocity()" << std::endl;
  }

  //--------------------------------------------------------------------------------------------------------------------
  // temporal derivative of spatial Jacobian
  //--------------------------------------------------------------------------------------------------------------------
  std::cout << "test: calculateSpatialJacobianTemporalDerivative" << std::endl;
  cv::Mat spatial_jacobian_dot = acceleration_test_arm_model.calculateSpatialJacobianTemporalDerivative(acceleration_test_arm_model.getNumberOfJoints()-1);
  if (
      !IsZero(spatial_jacobian_dot.at<double>(0, 0) - 0)
      || !IsZero(spatial_jacobian_dot.at<double>(1, 0) - 0)
      || !IsZero(spatial_jacobian_dot.at<double>(2, 0) - 0)
      || !IsZero(spatial_jacobian_dot.at<double>(3, 0) - 0)
      || !IsZero(spatial_jacobian_dot.at<double>(4, 0) - 0)
      || !IsZero(spatial_jacobian_dot.at<double>(5, 0) - 0)
      || !IsZero(spatial_jacobian_dot.at<double>(0, 1) - 0)
      || !IsZero(spatial_jacobian_dot.at<double>(1, 1) - -sin(0.1)*1.1)
      || !IsZero(spatial_jacobian_dot.at<double>(2, 1) - cos(0.1)*1.1)
      || !IsZero(spatial_jacobian_dot.at<double>(3, 1) - 0)
      || !IsZero(spatial_jacobian_dot.at<double>(4, 1) - 0)
      || !IsZero(spatial_jacobian_dot.at<double>(5, 1) - 0)
     )
  {
    errors++;
    std::cout << "ERROR with calculateSpatialJacobianTemporalDerivative()" << std::endl;
  }

  //--------------------------------------------------------------------------------------------------------------------
  // acceleration
  //--------------------------------------------------------------------------------------------------------------------
  p_acceleration_test_arm->setJointAcceleration(0, 2.1);
  p_acceleration_test_arm->setJointAcceleration(1, 2.2);
  acceleration_test_arm_model.update();
  std::cout << "test: calculateAcceleration" << std::endl;
  cv::Mat a0 = acceleration_test_arm_model.calculateAcceleration(p, 0, cedar::dev::robot::KinematicChainModel::LOCAL_COORDINATES);
  cv::Mat a1 = acceleration_test_arm_model.calculateAcceleration(p, 1, cedar::dev::robot::KinematicChainModel::LOCAL_COORDINATES);
  double s0 = sin(p_acceleration_test_arm->getJointAngle(0));
  double c0 = cos(p_acceleration_test_arm->getJointAngle(0));
  double s01 = sin(p_acceleration_test_arm->getJointAngle(0) + p_acceleration_test_arm->getJointAngle(1));
  double c01 = cos(p_acceleration_test_arm->getJointAngle(0) + p_acceleration_test_arm->getJointAngle(1));
  double dot_theta_0 = p_acceleration_test_arm->getJointVelocity(0);
  double dot_theta_1 = p_acceleration_test_arm->getJointVelocity(1);
  double dot_theta_01 = dot_theta_0 + dot_theta_1;
  double ddot_theta_0 = p_acceleration_test_arm->getJointAcceleration(0);
  double ddot_theta_1 = p_acceleration_test_arm->getJointAcceleration(1);
  double ddot_theta_01 = ddot_theta_0 + ddot_theta_1;
  double a0_1 = s0*dot_theta_0*dot_theta_0
                - c0*ddot_theta_0;
  double a0_2 = - c0*dot_theta_0*dot_theta_0
                - s0*ddot_theta_0;
  double a1_1 = s0*dot_theta_0*dot_theta_0
                + s01*dot_theta_01*dot_theta_01
                - c0*ddot_theta_0
                - c01*ddot_theta_01;
  double a1_2 = - c0*dot_theta_0*dot_theta_0
                - c01*dot_theta_01*dot_theta_01
                - s0*ddot_theta_0
                - s01*ddot_theta_01;
  if (
      !IsZero(a0.at<double>(0, 0) - 0)
      || !IsZero(a0.at<double>(1, 0) - a0_1)
      || !IsZero(a0.at<double>(2, 0) - a0_2)
      || !IsZero(a1.at<double>(0, 0) - 0)
      || !IsZero(a1.at<double>(1, 0) - a1_1)
      || !IsZero(a1.at<double>(2, 0) - a1_2)
     )
  {
    errors++;
    std::cout << "ERROR with calculateAcceleration()" << std::endl;
  }

  //--------------------------------------------------------------------------------------------------------------------
  // temporal derivative of Cartesian Jacobian
  //--------------------------------------------------------------------------------------------------------------------
  std::cout << "test: calculateCartesianJacobianTemporalDerivative" << std::endl;
  cv::Mat t = acceleration_test_arm_model.calculateEndEffectorPosition();
  cv::Mat cartesian_jacobian_dot = acceleration_test_arm_model.calculateCartesianJacobianTemporalDerivative(t, acceleration_test_arm_model.getNumberOfJoints()-1, cedar::dev::robot::KinematicChainModel::WORLD_COORDINATES);
  if (
      !IsZero(cartesian_jacobian_dot.at<double>(0, 0) - 0)
      || !IsZero(cartesian_jacobian_dot.at<double>(1, 0) - (s0*dot_theta_0 + s01*dot_theta_01))
      || !IsZero(cartesian_jacobian_dot.at<double>(2, 0) - (-c0*dot_theta_0 - c01*dot_theta_01))
      || !IsZero(cartesian_jacobian_dot.at<double>(0, 1) - 0)
      || !IsZero(cartesian_jacobian_dot.at<double>(1, 1) - s01*dot_theta_01)
      || !IsZero(cartesian_jacobian_dot.at<double>(2, 1) - -c01*dot_theta_01)
     )
  {
    errors++;
    std::cout << "ERROR with calculateCartesianJacobianTemporalDerivative()" << std::endl;
  }

  //--------------------------------------------------------------------------------------------------------------------
  // end-effector position
  //--------------------------------------------------------------------------------------------------------------------
  std::cout << "test: calculateEndEffectorPosition" << std::endl;
  cv::Mat end_effector_position = test_arm_model.calculateEndEffectorPosition();
  if (
      !IsZero(end_effector_position.at<double>(0, 0) - 0)
      || !IsZero(end_effector_position.at<double>(1, 0) - -2)
      || !IsZero(end_effector_position.at<double>(2, 0) - 6)
      || !IsZero(end_effector_position.at<double>(3, 0) - 1)
     )
  {
    errors++;
    std::cout << "ERROR with calculateEndEffectorPosition()" << std::endl;
  }
  
  //--------------------------------------------------------------------------------------------------------------------
  // end-effector transformation
  //--------------------------------------------------------------------------------------------------------------------
  std::cout << "test: calculateEndEffectorTransformation" << std::endl;
  cv::Mat end_effector_transformation = test_arm_model.getEndEffectorTransformation();
  if (
      !IsZero(end_effector_transformation.at<double>(0, 0) - 0)
      || !IsZero(end_effector_transformation.at<double>(0, 1) - -1)
      || !IsZero(end_effector_transformation.at<double>(0, 2) - 0)
      || !IsZero(end_effector_transformation.at<double>(1, 0) - 1)
      || !IsZero(end_effector_transformation.at<double>(1, 1) - 0)
      || !IsZero(end_effector_transformation.at<double>(1, 2) - 0)
      || !IsZero(end_effector_transformation.at<double>(2, 0) - 0)
      || !IsZero(end_effector_transformation.at<double>(2, 1) - 0)
      || !IsZero(end_effector_transformation.at<double>(2, 2) - 1)
      || !IsZero(end_effector_transformation.at<double>(0, 3) - 0)
      || !IsZero(end_effector_transformation.at<double>(1, 3) - -2)
      || !IsZero(end_effector_transformation.at<double>(2, 3) - 6)
      || !IsZero(end_effector_transformation.at<double>(3, 0) - 0)
      || !IsZero(end_effector_transformation.at<double>(3, 1) - 0)
      || !IsZero(end_effector_transformation.at<double>(3, 2) - 0)
      || !IsZero(end_effector_transformation.at<double>(3, 3) - 1)
     )
  {
    errors++;
    std::cout << "ERROR with calculateEndEffectorTransformation()" << std::endl;
  }
  
  //--------------------------------------------------------------------------------------------------------------------
  // end-effector jacobian
  //--------------------------------------------------------------------------------------------------------------------
  std::cout << "test: calculateEndEffectorJacobian" << std::endl;
  cv::Mat end_effector_jacobian = test_arm_model.calculateEndEffectorJacobian();
  if (
      !IsZero(end_effector_jacobian.at<double>(0, 0) - 0)
      || !IsZero(end_effector_jacobian.at<double>(1, 0) - 6)
      || !IsZero(end_effector_jacobian.at<double>(2, 0) - 2)
      || !IsZero(end_effector_jacobian.at<double>(0, 1) - 0)
      || !IsZero(end_effector_jacobian.at<double>(1, 1) - 4)
      || !IsZero(end_effector_jacobian.at<double>(2, 1) - 2)
      || !IsZero(end_effector_jacobian.at<double>(0, 2) - 0)
      || !IsZero(end_effector_jacobian.at<double>(1, 2) - 2)
      || !IsZero(end_effector_jacobian.at<double>(2, 2) - 2)
      || !IsZero(end_effector_jacobian.at<double>(0, 3) - 0)
      || !IsZero(end_effector_jacobian.at<double>(1, 3) - 2)
      || !IsZero(end_effector_jacobian.at<double>(2, 3) - 0)
     )
  {
    errors++;
    std::cout << "ERROR with calculateEndEffectorJacobian()" << std::endl;
  }
  
  //--------------------------------------------------------------------------------------------------------------------
  // end-effector velocity
  //--------------------------------------------------------------------------------------------------------------------
  std::cout << "test: calculateEndEffectorVelocity" << std::endl;
  cv::Mat v4 = test_arm_model.calculateEndEffectorVelocity();
  if (
      !IsZero(v4.at<double>(0, 0) - 0)
      || !IsZero(v4.at<double>(1, 0) - 6)
      || !IsZero(v4.at<double>(2, 0) - 4)
     )
  {
    errors++;
    std::cout << "ERROR with calculateEndEffectorVelocity()" << std::endl;
  }

  //--------------------------------------------------------------------------------------------------------------------
  // end-effector acceleration
  //--------------------------------------------------------------------------------------------------------------------
  std::cout << "test: calculateEndEffectorAcceleration" << std::endl;
  cv::Mat a2 = acceleration_test_arm_model.calculateEndEffectorAcceleration();
  if (
      !IsZero(a2.at<double>(0, 0) - 0)
      || !IsZero(a2.at<double>(1, 0) - a1_1)
      || !IsZero(a2.at<double>(2, 0) - a1_2)
     )
  {
    errors++;
    std::cout << "ERROR with calculateEndEffectorAcceleration()" << std::endl;
  }


  std::cout << "test finished, there were " << errors << " errors" << std::endl;
  if (errors > 255)
  {
    errors = 255;
  }
  return errors;
}











