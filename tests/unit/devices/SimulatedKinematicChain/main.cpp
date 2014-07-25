/*======================================================================================================================

    Copyright 2011, 2012, 2013, 2014 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany

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

// LOCAL INCLUDES

// PROJECT INCLUDES
#include "cedar/devices/SimulatedKinematicChain.h"
#include "cedar/auxiliaries/math/tools.h"
#include "cedar/auxiliaries/math/constants.h"
#include "cedar/auxiliaries/sleepFunctions.h"
#include "cedar/auxiliaries/CallFunctionInThread.h"
#include "cedar/auxiliaries/systemFunctions.h"

// SYSTEM INCLUDES
#include <vector>
#include <opencv2/opencv.hpp>
#include <QApplication>

unsigned int errors;

void test()
{
  // create instance of test class
  std::cout << "testing position control ..." << std::endl;
  cedar::dev::SimulatedKinematicChainPtr test_arm_position(new cedar::dev::SimulatedKinematicChain());
  test_arm_position->readJson("test_arm.json");
  test_arm_position->startDevice();
  
  //--------------------------------------------------------------------------------------------------------------------
  // simple service functions
  //--------------------------------------------------------------------------------------------------------------------
  std::cout << "test: getNumberOfJoints()" << std::endl;
  if (test_arm_position->getNumberOfJoints() != 4)
  {
    errors++;
    std::cout << "ERROR with getNumberOfJoints(), read: " << test_arm_position->getNumberOfJoints() << std::endl;
  }
  else
  {
    std::cout << "passed" << std::endl;
  }

  std::cout << "checking the first joint ..." << std::endl;
  cedar::dev::KinematicChain::JointPtr joint = test_arm_position->getJoint(0);
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
    !cedar::aux::math::isZero(joint->_mpAngleLimits->getLowerLimit() - (-5))
    || !cedar::aux::math::isZero(joint->_mpAngleLimits->getUpperLimit() - (5))
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
  // single angle
  //--------------------------------------------------------------------------------------------------------------------
  std::cout << "test: single angle functions" << std::endl;
  test_arm_position->setJointAngle(0, 1);
  test_arm_position->setJointAngle(1, 2.0);
  test_arm_position->setJointAngle(2, cedar::aux::math::pi/2);
  test_arm_position->setJointAngle(3, sqrt(2.0));
  cedar::aux::sleep(cedar::unit::Time(20.0 * cedar::unit::milli * cedar::unit::seconds));
  if (
      !cedar::aux::math::isZero(test_arm_position->getJointAngle(0) - 1.0)
      || !cedar::aux::math::isZero(test_arm_position->getJointAngle(1) - 2.0)
      || !cedar::aux::math::isZero(test_arm_position->getJointAngle(2) - cedar::aux::math::pi/2.0)
      || !cedar::aux::math::isZero(test_arm_position->getJointAngle(3) - sqrt(2.0))
      )
  {
    errors++;
    std::cout << test_arm_position->getJointAngles() << std::endl;
    std::cout << "ERROR with setJointAngle() or getJointAngle()" << std::endl;
  }
  
  //--------------------------------------------------------------------------------------------------------------------
  // std::vector of angle values
  //--------------------------------------------------------------------------------------------------------------------
  std::cout << "test: std::vector angle functions" << std::endl;
  std::vector<double> angle_vector;
  angle_vector.push_back(0.5);
  angle_vector.push_back(1.5);
  angle_vector.push_back(2.5);
  angle_vector.push_back(3.5);
  test_arm_position->setJointAngles(angle_vector);
  cedar::aux::sleep(cedar::unit::Time(20.0 * cedar::unit::milli * cedar::unit::seconds));
  if (
      !cedar::aux::math::isZero<double>(test_arm_position->getJointAngle(0) - 0.5)
      || !cedar::aux::math::isZero<double>(test_arm_position->getJointAngle(1) - 1.5)
      || !cedar::aux::math::isZero<double>(test_arm_position->getJointAngle(2) - 2.5)
      || !cedar::aux::math::isZero<double>(test_arm_position->getJointAngle(3) - 3.5)
      )
  {
    errors++;
    std::cout << test_arm_position->getJointAngles() << std::endl;
    std::cout << "ERROR with setJointAngles() or getCachedJointAngles()" << std::endl;
  }

  //--------------------------------------------------------------------------------------------------------------------
  // cv::Mat of angle values
  //--------------------------------------------------------------------------------------------------------------------
  std::cout << "test: cv::Mat angle functions" << std::endl;
  cv::Mat angle_matrix = cv::Mat::zeros(4, 1, CV_64FC1);
  angle_matrix.at<double>(0, 0) = 0.1;
  angle_matrix.at<double>(1, 0) = 0.2;
  angle_matrix.at<double>(2, 0) = 0.3;
  angle_matrix.at<double>(3, 0) = 0.4;
  test_arm_position->setJointAngles(angle_matrix);
  cedar::aux::sleep(cedar::unit::Time(20.0 * cedar::unit::milli * cedar::unit::seconds));
  if (
      !cedar::aux::math::isZero(test_arm_position->getJointAngles().at<double>(0, 0) - 0.1)
      || !cedar::aux::math::isZero(test_arm_position->getJointAngles().at<double>(1, 0) - 0.2)
      || !cedar::aux::math::isZero(test_arm_position->getJointAngles().at<double>(2, 0) - 0.3)
      || !cedar::aux::math::isZero(test_arm_position->getJointAngles().at<double>(3, 0) - 0.4)
      )
  {
    errors++;
    std::cout << test_arm_position->getJointAngles() << std::endl;
    std::cout << "ERROR with setJointAngles() or getCachedJointAngles()" << std::endl;
  }
  test_arm_position->stopDevice();
  
  // create instance of test class
  std::cout << "testing velocity control ..." << std::endl;
  cedar::dev::SimulatedKinematicChainPtr test_arm_velocity(new cedar::dev::SimulatedKinematicChain());
  test_arm_velocity->readJson("test_arm.json");
  test_arm_velocity->startDevice();

  //--------------------------------------------------------------------------------------------------------------------
  // single angle velocity
  //--------------------------------------------------------------------------------------------------------------------
  std::cout << "test: single velocity functions" << std::endl;
  test_arm_velocity->setJointVelocity(0, 0.1);
  test_arm_velocity->setJointVelocity(1, 0.2);
  test_arm_velocity->setJointVelocity(2, 0.15);
  test_arm_velocity->setJointVelocity(3, 0.25);
  cedar::aux::sleep(cedar::unit::Time(50.0 * cedar::unit::milli * cedar::unit::seconds));
  if (
      !cedar::aux::math::isZero(test_arm_velocity->getJointVelocity(0) - 0.1)
      || !cedar::aux::math::isZero(test_arm_velocity->getJointVelocity(1) - 0.2)
      || !cedar::aux::math::isZero(test_arm_velocity->getJointVelocity(2) - 0.15)
      || !cedar::aux::math::isZero(test_arm_velocity->getJointVelocity(3) - 0.25)
      )
  {
    errors++;
    std::cout << test_arm_velocity->getJointAngles() << std::endl;
    std::cout << test_arm_velocity->getJointVelocities() << std::endl;
    std::cout << "ERROR with setJointVelocity() or getJointVelocity()" << std::endl;
  }

  //--------------------------------------------------------------------------------------------------------------------
  // std::vector of angle values
  //--------------------------------------------------------------------------------------------------------------------
  std::cout << "test: std::vector velocity functions" << std::endl;
  std::vector<double> velocity_vector;
  velocity_vector.push_back(0.5);
  velocity_vector.push_back(1.5);
  velocity_vector.push_back(2.5);
  velocity_vector.push_back(3.5);
  test_arm_velocity->setJointVelocities(velocity_vector);
  cedar::aux::sleep(cedar::unit::Time(50.0 * cedar::unit::milli * cedar::unit::seconds));
  if (
      !cedar::aux::math::isZero<double>(test_arm_velocity->getJointVelocity(0) - 0.5)
      || !cedar::aux::math::isZero<double>(test_arm_velocity->getJointVelocity(1) - 1.5)
      || !cedar::aux::math::isZero<double>(test_arm_velocity->getJointVelocity(2) - 2.5)
      || !cedar::aux::math::isZero<double>(test_arm_velocity->getJointVelocity(3) - 3.5)
      )
  {
    errors++;
    std::cout << test_arm_velocity->getJointAngles() << std::endl;
    std::cout << test_arm_velocity->getJointVelocities() << std::endl;
    std::cout << "ERROR with setJointVelocities(vector) or getJointVelocities()" << std::endl;
  }

  //--------------------------------------------------------------------------------------------------------------------
  // cv::Mat of angle values
  //--------------------------------------------------------------------------------------------------------------------
  std::cout << "test: cv::Mat velocity functions" << std::endl;
  cv::Mat velocity_matrix = cv::Mat::zeros(4, 1, CV_64FC1);
  velocity_matrix.at<double>(0, 0) = 0.1;
  velocity_matrix.at<double>(1, 0) = 0.2;
  velocity_matrix.at<double>(2, 0) = 0.3;
  velocity_matrix.at<double>(3, 0) = 0.4;
  test_arm_velocity->setJointVelocities(velocity_matrix);
  cedar::aux::sleep(cedar::unit::Time(50.0 * cedar::unit::milli * cedar::unit::seconds));
  if (
      !cedar::aux::math::isZero(test_arm_velocity->getJointVelocities().at<double>(0, 0) - 0.1)
      || !cedar::aux::math::isZero(test_arm_velocity->getJointVelocities().at<double>(1, 0) - 0.2)
      || !cedar::aux::math::isZero(test_arm_velocity->getJointVelocities().at<double>(2, 0) - 0.3)
      || !cedar::aux::math::isZero(test_arm_velocity->getJointVelocities().at<double>(3, 0) - 0.4)
      )
  {
    errors++;
    std::cout << test_arm_velocity->getJointAngles() << std::endl;
    std::cout << test_arm_velocity->getJointVelocities() << std::endl;
    std::cout << "ERROR with setJointVelocities(matrix) or getJointVelocities()" << std::endl;
  }
  test_arm_velocity->stopDevice();

  // create instance of test class
  std::cout << "testing acceleration control ..." << std::endl;
  cedar::dev::SimulatedKinematicChainPtr test_arm_acceleration(new cedar::dev::SimulatedKinematicChain());
  test_arm_acceleration->readJson("test_arm.json");
  test_arm_acceleration->startDevice();

  //--------------------------------------------------------------------------------------------------------------------
  // single angle acceleration
  //--------------------------------------------------------------------------------------------------------------------
  std::cout << "test: single acceleration functions" << std::endl;
  test_arm_acceleration->setJointAcceleration(0, 0.001);
  test_arm_acceleration->setJointAcceleration(1, 0.002);
  test_arm_acceleration->setJointAcceleration(2, 0.0015);
  test_arm_acceleration->setJointAcceleration(3, 0.0025);
  cedar::aux::sleep(cedar::unit::Time(50.0 * cedar::unit::milli * cedar::unit::seconds));
  if (
      !cedar::aux::math::isZero(test_arm_acceleration->getJointAcceleration(0) - 0.001)
      || !cedar::aux::math::isZero(test_arm_acceleration->getJointAcceleration(1) - 0.002)
      || !cedar::aux::math::isZero(test_arm_acceleration->getJointAcceleration(2) - 0.0015)
      || !cedar::aux::math::isZero(test_arm_acceleration->getJointAcceleration(3) - 0.0025)
      )
  {
    errors++;
    std::cout << test_arm_acceleration->getJointAngles() << std::endl;
    std::cout << test_arm_acceleration->getJointVelocities() << std::endl;
    std::cout << test_arm_acceleration->getJointAccelerations() << std::endl;
    std::cout << "ERROR with setJointAcceleration() or getJointAcceleration()" << std::endl;
  }

  //--------------------------------------------------------------------------------------------------------------------
  // std::vector of acceleration values
  //--------------------------------------------------------------------------------------------------------------------
  std::cout << "test: std::vector Acceleration functions" << std::endl;
  std::vector<double> acceleration_vector;
  acceleration_vector.push_back(0.005);
  acceleration_vector.push_back(0.004);
  acceleration_vector.push_back(0.003);
  acceleration_vector.push_back(0.002);
  test_arm_acceleration->setJointAccelerations(acceleration_vector);
  cedar::aux::sleep(cedar::unit::Time(50.0 * cedar::unit::milli * cedar::unit::seconds));
  if (
      !cedar::aux::math::isZero<double>(test_arm_acceleration->getJointAcceleration(0) - acceleration_vector.at(0))
      || !cedar::aux::math::isZero<double>(test_arm_acceleration->getJointAcceleration(1) - acceleration_vector.at(1))
      || !cedar::aux::math::isZero<double>(test_arm_acceleration->getJointAcceleration(2) - acceleration_vector.at(2))
      || !cedar::aux::math::isZero<double>(test_arm_acceleration->getJointAcceleration(3) - acceleration_vector.at(3))
      )
  {
    errors++;
    std::cout << test_arm_acceleration->getJointAngles() << std::endl;
    std::cout << test_arm_acceleration->getJointVelocities() << std::endl;
    std::cout << test_arm_acceleration->getJointAccelerations() << std::endl;
    std::cout << "ERROR with setJointAccelerations(vector) or getJointAccelerations()" << std::endl;
  }

  //--------------------------------------------------------------------------------------------------------------------
  // cv::Mat of acceleration values
  //--------------------------------------------------------------------------------------------------------------------
  std::cout << "test: cv::Mat Acceleration functions" << std::endl;
  cv::Mat acceleration_matrix = cv::Mat::zeros(4, 1, CV_64FC1);
  acceleration_matrix.at<double>(0, 0) = 0.001;
  acceleration_matrix.at<double>(1, 0) = 0.002;
  acceleration_matrix.at<double>(2, 0) = 0.003;
  acceleration_matrix.at<double>(3, 0) = 0.004;
  test_arm_acceleration->setJointAccelerations(acceleration_matrix);
  cedar::aux::sleep(cedar::unit::Time(50.0 * cedar::unit::milli * cedar::unit::seconds));
  if (
      !cedar::aux::math::isZero(test_arm_acceleration->getJointAccelerations().at<double>(0, 0) - acceleration_matrix.at<double>(0, 0))
      || !cedar::aux::math::isZero(test_arm_acceleration->getJointAccelerations().at<double>(1, 0) - acceleration_matrix.at<double>(1, 0))
      || !cedar::aux::math::isZero(test_arm_acceleration->getJointAccelerations().at<double>(2, 0) - acceleration_matrix.at<double>(2, 0))
      || !cedar::aux::math::isZero(test_arm_acceleration->getJointAccelerations().at<double>(3, 0) - acceleration_matrix.at<double>(3, 0))
      )
  {
    errors++;
    std::cout << test_arm_acceleration->getJointAngles() << std::endl;
    std::cout << test_arm_acceleration->getJointVelocities() << std::endl;
    std::cout << test_arm_acceleration->getJointAccelerations() << std::endl;
    std::cout << "ERROR with setJointAccelerations(matrix) or getJointAccelerations()" << std::endl;
  }
  test_arm_acceleration->stopDevice();

  // create instance of test class
  std::cout << "testing exclusiveness of commands ..." << std::endl;
  cedar::dev::SimulatedKinematicChainPtr test_arm_exclusive(new cedar::dev::SimulatedKinematicChain());
  test_arm_exclusive->readJson("test_arm.json");
  test_arm_exclusive->startDevice();
  try
  {
    test_arm_exclusive->setJointAngle(0, 1.0);
    test_arm_exclusive->setJointVelocity(0, 0.1);
    test_arm_exclusive->setJointAcceleration(0, 0.01);
    errors++;
    std::cout << "ERROR with exclusiveness of commands" << std::endl;
  }
  catch (const cedar::dev::Component::CouldNotGuessCommandTypeException& exc)
  {
    // all is well!
  }

  //--------------------------------------------------------------------------------------------------------------------
  // check configuration
  //--------------------------------------------------------------------------------------------------------------------

//  test_arm_position->applyInitialConfiguration("default");
  test_arm_position->setJointAngles( cv::Mat::zeros( test_arm_position->getNumberOfJoints(), 1, CV_64F) );
  test_arm_position->startDevice();
  cedar::aux::sleep( test_arm_position->getDeviceStepSize() * 1.5 );
  test_arm_position->stopDevice();

  std::cout << "checking the root coordinate frame translation" << std::endl;
  cedar::aux::LocalCoordinateFramePtr rootCoordinateFrame = test_arm_position->getRootCoordinateFrame();

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
    std::cout << "ERROR with root coordinate frame translation, read:" << std::endl;
    cedar::aux::write(rootCoordinateFrame->getTranslation().matrix);
  }
  else
    std::cout << "passed" << std::endl;

  std::cout << "checking the root coordinate frame rotation" << std::endl;
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
  else
  {
    std::cout << "passed" << std::endl;
  }

  std::cout << "checking the end-effector coordinate frame..." << std::endl;
  cedar::aux::LocalCoordinateFramePtr endEffectorCoordinateFrame = test_arm_position->getEndEffectorCoordinateFrame();
  if
  (
       !cedar::aux::math::isZero
        (
          endEffectorCoordinateFrame->getTranslationX() / cedar::unit::DEFAULT_LENGTH_UNIT - 0.0
        )
    || !cedar::aux::math::isZero
        (
          endEffectorCoordinateFrame->getTranslationY() / cedar::unit::DEFAULT_LENGTH_UNIT - 0.0
        )
    || !cedar::aux::math::isZero
        (
          endEffectorCoordinateFrame->getTranslationZ() / cedar::unit::DEFAULT_LENGTH_UNIT - 8.0
        )
  )
  {
    errors++;
    std::cout << "ERROR with end-effector coordinate frame translation, read:" << std::endl;
    std::cout << endEffectorCoordinateFrame->getTranslation().matrix << std::endl;
  }
  else
  {
    std::cout << "passed" << std::endl;
  }

  cv::Mat eefRotation = endEffectorCoordinateFrame->getRotation();
  if
  (
    !cedar::aux::math::isZero(eefRotation.at<double>(0, 0) - (0.0))
    || !cedar::aux::math::isZero(eefRotation.at<double>(0, 1) - (-1.0))
    || !cedar::aux::math::isZero(eefRotation.at<double>(0, 2) - (0.0))
    || !cedar::aux::math::isZero(eefRotation.at<double>(1, 0) - (1.0))
    || !cedar::aux::math::isZero(eefRotation.at<double>(1, 1) - (0.0))
    || !cedar::aux::math::isZero(eefRotation.at<double>(1, 2) - (0.0))
    || !cedar::aux::math::isZero(eefRotation.at<double>(2, 0) - (0.0))
    || !cedar::aux::math::isZero(eefRotation.at<double>(2, 1) - (0.0))
    || !cedar::aux::math::isZero(eefRotation.at<double>(2, 2) - (1.0))
  )
  {
    errors++;
    cedar::aux::write(eefRotation);
  }
  else
  {
    std::cout << "passed" << std::endl;
  }

  //--------------------------------------------------------------------------------------------------------------------
  // transformations
  //--------------------------------------------------------------------------------------------------------------------
  std::cout << "test: transformations" << std::endl;
  cv::Mat joint_transformation_1 = test_arm_position->getJointTransformation(1);
  cv::Mat joint_transformation_3 = test_arm_position->getJointTransformation(3);
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
      || !cedar::aux::math::isZero(joint_transformation_1.at<double>(3, 3) - 1) )
  {
    errors++;
    std::cout << "ERROR with calculateTransformations() or getJointTransformation() 1" << std::endl;
  }
  else
  {
    std::cout << "passed" << std::endl;
  }

  if (0
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
      || !cedar::aux::math::isZero(joint_transformation_3.at<double>(1, 3) - 0)
      || !cedar::aux::math::isZero(joint_transformation_3.at<double>(2, 3) - 6)

      || !cedar::aux::math::isZero(joint_transformation_3.at<double>(3, 0) - 0)
      || !cedar::aux::math::isZero(joint_transformation_3.at<double>(3, 1) - 0)
      || !cedar::aux::math::isZero(joint_transformation_3.at<double>(3, 2) - 0)
      || !cedar::aux::math::isZero(joint_transformation_3.at<double>(3, 3) - 1)
      )
  {
    errors++;
    std::cout << "ERROR with calculateTransformations() or getJointTransformation() 3" << std::endl;
  }
  else
  {
    std::cout << "passed" << std::endl;
  }


  //--------------------------------------------------------------------------------------------------------------------
  // Jacobians
  //--------------------------------------------------------------------------------------------------------------------
  std::cout << "test: Jacobians" << std::endl;
  cv::Mat origin = cv::Mat::zeros( 4, 1, CV_64FC1 );
  origin.at<double>( 3, 0 ) = 1;
  cv::Mat jacobian_1 = cv::Mat::zeros(3, 4, CV_64FC1);
  cv::Mat jacobian_3 = cv::Mat::zeros(3, 4, CV_64FC1);
  jacobian_1 = test_arm_position->calculateCartesianJacobian(origin, 1, cedar::dev::KinematicChain::LOCAL_COORDINATES);
  jacobian_3 = test_arm_position->calculateCartesianJacobian(origin, 3, cedar::dev::KinematicChain::LOCAL_COORDINATES);
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
          - test_arm_position->calculateCartesianJacobian(origin, 1, cedar::dev::KinematicChain::LOCAL_COORDINATES)
        )
      )
      // Jacobian of joint 3
      || !cedar::aux::math::isZero(jacobian_3.at<double>(0, 0) - 0)
      || !cedar::aux::math::isZero(jacobian_3.at<double>(1, 0) - 6)
      || !cedar::aux::math::isZero(jacobian_3.at<double>(2, 0) - 0)

      || !cedar::aux::math::isZero(jacobian_3.at<double>(0, 1) - 0)
      || !cedar::aux::math::isZero(jacobian_3.at<double>(1, 1) - 4)
      || !cedar::aux::math::isZero(jacobian_3.at<double>(2, 1) - 0)

      || !cedar::aux::math::isZero(jacobian_3.at<double>(0, 2) - 0)
      || !cedar::aux::math::isZero(jacobian_3.at<double>(1, 2) - 2)
      || !cedar::aux::math::isZero(jacobian_3.at<double>(2, 2) - 0)

      || !cedar::aux::math::isZero(jacobian_3.at<double>(0, 3) - 0)
      || !cedar::aux::math::isZero(jacobian_3.at<double>(1, 3) - 0)
      || !cedar::aux::math::isZero(jacobian_3.at<double>(2, 3) - 0)
      || !cedar::aux::math::isZero
      (
        norm
        (
          jacobian_3
          - test_arm_position->calculateCartesianJacobian(origin, 3, cedar::dev::KinematicChain::LOCAL_COORDINATES)
        )
      )
    )
  {
std::cout << "test j1 " << jacobian_1 << std::endl;
std::cout << "test j3 " << jacobian_3 << std::endl;

    errors++;
    std::cout << "ERROR with calculateCartesianJacobian()" << std::endl;
  }
  else
  {
    std::cout << "passed" << std::endl;
  }

  std::cout << "test: calculateSpatialJacobian" << std::endl;
  cv::Mat spatial_jacobian = test_arm_position->calculateSpatialJacobian(test_arm_position->getNumberOfJoints()-1);
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
      || !cedar::aux::math::isZero(spatial_jacobian.at<double>(1, 3) - -6)
      || !cedar::aux::math::isZero(spatial_jacobian.at<double>(2, 3) - 0)
      || !cedar::aux::math::isZero(spatial_jacobian.at<double>(3, 3) - -1)
      || !cedar::aux::math::isZero(spatial_jacobian.at<double>(4, 3) - 0)
      || !cedar::aux::math::isZero(spatial_jacobian.at<double>(5, 3) - 0)
     )
  {
std::cout << " spatial " << spatial_jacobian << std::endl;    
    errors++;
    std::cout << "ERROR with spatialJacobian()" << std::endl;
  }
  else
  {
    std::cout << "passed" << std::endl;
  }



  //--------------------------------------------------------------------------------------------------------------------
  // end-effector jacobian
  //--------------------------------------------------------------------------------------------------------------------
  std::cout << "test: calculateEndEffectorJacobian" << std::endl;
  cv::Mat end_effector_jacobian = test_arm_position->calculateEndEffectorJacobian();
  if (
      !cedar::aux::math::isZero(end_effector_jacobian.at<double>(0, 0) - 0)
      || !cedar::aux::math::isZero(end_effector_jacobian.at<double>(1, 0) - 8)
      || !cedar::aux::math::isZero(end_effector_jacobian.at<double>(2, 0) - 0)

      || !cedar::aux::math::isZero(end_effector_jacobian.at<double>(0, 1) - 0)
      || !cedar::aux::math::isZero(end_effector_jacobian.at<double>(1, 1) - 6)
      || !cedar::aux::math::isZero(end_effector_jacobian.at<double>(2, 1) - 0)

      || !cedar::aux::math::isZero(end_effector_jacobian.at<double>(0, 2) - 0)
      || !cedar::aux::math::isZero(end_effector_jacobian.at<double>(1, 2) - 4)
      || !cedar::aux::math::isZero(end_effector_jacobian.at<double>(2, 2) - 0)

      || !cedar::aux::math::isZero(end_effector_jacobian.at<double>(0, 3) - 0)
      || !cedar::aux::math::isZero(end_effector_jacobian.at<double>(1, 3) - 2)
      || !cedar::aux::math::isZero(end_effector_jacobian.at<double>(2, 3) - 0)
     )
  {
std::cout << "end eff jacobian " << end_effector_jacobian << std::endl;    
    errors++;
    std::cout << "ERROR with calculateEndEffectorJacobian()" << std::endl;
  }
  else
  {
    std::cout << "passed" << std::endl;
  }

  //--------------------------------------------------------------------------------------------------------------------
  // calculate finite differences for time derivatives
  //--------------------------------------------------------------------------------------------------------------------
  cv::Mat theta;
  cv::Mat thetaDot;
  cv::Mat thetaTwoDot;

  cv::Mat p_local = cv::Mat::zeros(4, 1, CV_64FC1);
  p_local.at<double>(2, 0) = 1.0;
  p_local.at<double>(3, 0) = 1.0;

  cedar::dev::SimulatedKinematicChainPtr complex_test_arm(new cedar::dev::SimulatedKinematicChain());
  std::string complex_test_arm_configuration_file = cedar::aux::locateResource("configs/complex_test_arm.json", false);
  complex_test_arm->readJson(complex_test_arm_configuration_file);
  complex_test_arm->startDevice();

  theta = cv::Mat(4, 1, CV_64FC1);
  thetaDot = cv::Mat(4, 1, CV_64FC1);
  thetaTwoDot = cv::Mat(4, 1, CV_64FC1);
  cv::randn(theta, cv::Scalar(0), cv::Scalar(1));
  cv::randn(thetaDot, cv::Scalar(0), cv::Scalar(1));
  cv::randn(thetaTwoDot, cv::Scalar(0), cv::Scalar(1));

  complex_test_arm->applyInitialConfiguration("default");
  cedar::aux::sleep( complex_test_arm->getDeviceStepSize() * 1.5 );

  complex_test_arm->clearUserCommand();
  complex_test_arm->setJointAngles(theta);
  cedar::aux::sleep( complex_test_arm->getDeviceStepSize() * 1.5 );

  complex_test_arm->clearUserCommand();
  complex_test_arm->setJointVelocities(thetaDot);
  cedar::aux::sleep( complex_test_arm->getDeviceStepSize() * 1.5 );

  complex_test_arm->clearUserCommand();
  complex_test_arm->setJointAccelerations(thetaTwoDot);
  cedar::aux::sleep( complex_test_arm->getDeviceStepSize() * 1.5 );

  complex_test_arm->stopDevice();

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

  complex_test_arm->startDevice();
  complex_test_arm->clearUserCommand();
  complex_test_arm->setJointVelocities
  (
    complex_test_arm->getJointVelocities()
    + delta_t*complex_test_arm->getJointAccelerations()
  );
  cedar::aux::sleep( complex_test_arm->getDeviceStepSize() * 1.5 );
  complex_test_arm->clearUserCommand();
  complex_test_arm->setJointAngles
  (
    complex_test_arm->getJointAngles()
    + delta_t*complex_test_arm->getJointVelocities()
    + delta_t*delta_t*complex_test_arm->getJointAccelerations()
  );
  cedar::aux::sleep( complex_test_arm->getDeviceStepSize() * 1.5 );
  complex_test_arm->stopDevice();

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
std::cout << "real    v : " << v_eef << std::endl;
std::cout << "numeric v : " << v_eef_numeric << std::endl;
std::cout << "real    a : " << a_eef << std::endl;
std::cout << "numeric a : " << a_eef_numeric << std::endl;


  //--------------------------------------------------------------------------------------------------------------------
  // spatial Jacobian temporal derivative
  //--------------------------------------------------------------------------------------------------------------------
  std::cout << "test: calculateSpatialJacobianTemporalDerivative" << std::endl;
  if (
      !cedar::aux::math::isZero(pow(norm(spatial_jacobian_dot - spatial_jacobian_dot_numeric)/(spatial_jacobian_dot.cols*6), 2))
     )
  {
std::cout << spatial_jacobian_dot << std::endl;
std::cout << spatial_jacobian_dot_numeric << std::endl;
    errors++;
    std::cout << "ERROR with calculateSpatialJacobianTemporalDerivative(...)" << std::endl;
  }
  else
  {
    std::cout << "passed" << std::endl;
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
  else
  {
    std::cout << "passed" << std::endl;
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
  else
  {
    std::cout << "passed" << std::endl;
  }

  //--------------------------------------------------------------------------------------------------------------------
  // end-effector acceleration
  //--------------------------------------------------------------------------------------------------------------------
  std::cout << "test: calculateEndEffectorAcceleration" << std::endl;
  if (!cedar::aux::math::isZero(pow(norm(a_eef - a_eef_numeric)/3.0, 2)))
  {
std::cout << a_eef_numeric << std::endl;    
std::cout << a_eef << std::endl;    
    errors++;
    std::cout << "ERROR with calculateEndEffectorAcceleration()" << std::endl;
  }
  else
  {
    std::cout << "passed" << std::endl;
  }

  QApplication::exit(errors);
}


int main(int argc, char** argv)
{
  QApplication app(argc, argv);
  // the number of errors encountered in this test
  errors = 0;

  cedar::aux::CallFunctionInThread caller(boost::bind(&test));

  caller.start();

  app.exec();

  std::cout << "test finished, there were " << errors << " errors" << std::endl;
  if (errors > 255)
  {
    errors = 255;
  }
  return errors;
}
