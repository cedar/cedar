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

// LOCAL INCLUDES

// PROJECT INCLUDES
#include "cedar/devices/Robot.h"
#include "cedar/devices/SimulatedKinematicChain.h"
#include "cedar/auxiliaries/math/tools.h"
#include "cedar/auxiliaries/math/constants.h"
#include "cedar/auxiliaries/CallFunctionInThread.h"
#include "cedar/auxiliaries/systemFunctions.h"
#include "cedar/testingUtilities/helpers.h"

// SYSTEM INCLUDES
#include <vector>
#include <opencv2/opencv.hpp>
#include <QApplication>

unsigned int errors;

void test()
{
  // create instance of test class
  std::cout << "testing position control ..." << std::endl;

  auto posrobot = boost::make_shared< cedar::dev::Robot >();
  posrobot->readJson("test_configuration.json");

  cedar::dev::SimulatedKinematicChainPtr test_arm_position = posrobot->getComponent< cedar::dev::SimulatedKinematicChain >("arm");

  test_arm_position->startCommunication();
  
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
  float firstAngle = 1;
  float secondAngle =2;
  float thirdAngle =  cedar::aux::math::pi/2;
  float fourthAngle = sqrt(2.0);
  test_arm_position->setJointAngle(0, firstAngle);
  test_arm_position->setJointAngle(1, secondAngle);
  test_arm_position->setJointAngle(2, thirdAngle);
  test_arm_position->setJointAngle(3, fourthAngle);
  test_arm_position->waitUntilCommunicated();
  //cedar::aux::sleep(cedar::unit::Time(20.0 * cedar::unit::milli * cedar::unit::seconds));
  if (
      !cedar::aux::math::isZero(test_arm_position->getJointAngle(0) - firstAngle)
      || !cedar::aux::math::isZero(test_arm_position->getJointAngle(1) - secondAngle)
      || !cedar::aux::math::isZero(test_arm_position->getJointAngle(2) - thirdAngle)
      || !cedar::aux::math::isZero(test_arm_position->getJointAngle(3) - fourthAngle)
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
  std::vector<float> angle_vector;
  angle_vector.push_back(0.5);
  angle_vector.push_back(1.5);
  angle_vector.push_back(2.5);
  angle_vector.push_back(3.5);
  test_arm_position->setJointAngles(angle_vector);
  test_arm_position->waitUntilCommunicated();
  //cedar::aux::sleep(cedar::unit::Time(20.0 * cedar::unit::milli * cedar::unit::seconds));
  if (
      !cedar::aux::math::isZero<float>(test_arm_position->getJointAngle(0) - 0.5)
      || !cedar::aux::math::isZero<float>(test_arm_position->getJointAngle(1) - 1.5)
      || !cedar::aux::math::isZero<float>(test_arm_position->getJointAngle(2) - 2.5)
      || !cedar::aux::math::isZero<float>(test_arm_position->getJointAngle(3) - 3.5)
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
  cv::Mat angle_matrix = cv::Mat::zeros(4, 1, CV_32FC1);
  angle_matrix.at<float>(0, 0) = 0.1f;
  angle_matrix.at<float>(1, 0) = 0.2f;
  angle_matrix.at<float>(2, 0) = 0.3f;
  angle_matrix.at<float>(3, 0) = 0.4f;
  test_arm_position->setJointAngles(angle_matrix);
  test_arm_position->waitUntilCommunicated();
  //cedar::aux::sleep(cedar::unit::Time(20.0 * cedar::unit::milli * cedar::unit::seconds));
  if (
      !cedar::aux::math::isZero(test_arm_position->getJointAngles().at<float>(0, 0) - 0.1f)
      || !cedar::aux::math::isZero(test_arm_position->getJointAngles().at<float>(1, 0) - 0.2f)
      || !cedar::aux::math::isZero(test_arm_position->getJointAngles().at<float>(2, 0) - 0.3f)
      || !cedar::aux::math::isZero(test_arm_position->getJointAngles().at<float>(3, 0) - 0.4f)
      )
  {
    errors++;
    std::cout << test_arm_position->getJointAngles() << std::endl;
    std::cout << "ERROR with setJointAngles() or getCachedJointAngles()" << std::endl;
  }
  test_arm_position->stopCommunication();
  
  // create instance of test class
  std::cout << "testing velocity control ..." << std::endl;
  auto velrobot = boost::make_shared< cedar::dev::Robot >();
  velrobot->readJson("test_configuration.json");

  cedar::dev::SimulatedKinematicChainPtr test_arm_velocity = velrobot->getComponent< cedar::dev::SimulatedKinematicChain >("arm");

  test_arm_velocity->startCommunication();

  //--------------------------------------------------------------------------------------------------------------------
  // single angle velocity
  //--------------------------------------------------------------------------------------------------------------------
  std::cout << "test: single velocity functions" << std::endl;
  float firstVelocity = 0.1;
  float secondVelocity =0.2;
  float thirdVelocity = 0.15;
  float fourthVelocity = 0.25;
  test_arm_velocity->setJointVelocity(0,firstVelocity);
  test_arm_velocity->setJointVelocity(1, secondVelocity);
  test_arm_velocity->setJointVelocity(2, thirdVelocity);
  test_arm_velocity->setJointVelocity(3, fourthVelocity);
  test_arm_velocity->waitUntilCommunicated();
  //cedar::aux::sleep(cedar::unit::Time(50.0 * cedar::unit::milli * cedar::unit::seconds));
  if (
      !cedar::aux::math::isZero<float>(test_arm_velocity->getJointVelocity(0) - firstVelocity,1e-6)
      || !cedar::aux::math::isZero<float>(test_arm_velocity->getJointVelocity(1) - secondVelocity,1e-6)
      || !cedar::aux::math::isZero<float>(test_arm_velocity->getJointVelocity(2) -thirdVelocity,1e-6)
      || !cedar::aux::math::isZero<float>(test_arm_velocity->getJointVelocity(3) - fourthVelocity,1e-6)
      )
  {
    errors++;
//    std::cout << test_arm_velocity->getJointAngles() << std::endl;
    std::cout << "["<< firstVelocity << ";" << secondVelocity << ";" << thirdVelocity << ";" << fourthVelocity <<" ] << desired Velocities" <<std::endl;
    std::cout << test_arm_velocity->getJointVelocities() << std::endl;
    std::cout << "ERROR with setJointVelocity() or getJointVelocity()" << std::endl;
  }

  //--------------------------------------------------------------------------------------------------------------------
  // std::vector of angle values
  //--------------------------------------------------------------------------------------------------------------------
  std::cout << "test: std::vector velocity functions" << std::endl;
  std::vector<float> velocity_vector;
  velocity_vector.push_back(0.5);
  velocity_vector.push_back(1.5);
  velocity_vector.push_back(2.5);
  velocity_vector.push_back(3.5);
  test_arm_velocity->setJointVelocities(velocity_vector);
  test_arm_velocity->waitUntilCommunicated();
  //cedar::aux::sleep(cedar::unit::Time(50.0 * cedar::unit::milli * cedar::unit::seconds));
  if (
      !cedar::aux::math::isZero<float>(test_arm_velocity->getJointVelocity(0) - 0.5,1e-6)
      || !cedar::aux::math::isZero<float>(test_arm_velocity->getJointVelocity(1) - 1.5,1e-6)
      || !cedar::aux::math::isZero<float>(test_arm_velocity->getJointVelocity(2) - 2.5,1e-6)
      || !cedar::aux::math::isZero<float>(test_arm_velocity->getJointVelocity(3) - 3.5,1e-6)
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
  cv::Mat velocity_matrix = cv::Mat::zeros(4, 1, CV_32FC1);
  velocity_matrix.at<float>(0, 0) = 0.1;
  velocity_matrix.at<float>(1, 0) = 0.2;
  velocity_matrix.at<float>(2, 0) = 0.3;
  velocity_matrix.at<float>(3, 0) = 0.4;
  test_arm_velocity->setJointVelocities(velocity_matrix);
  test_arm_velocity->waitUntilCommunicated();
  //cedar::aux::sleep(cedar::unit::Time(50.0 * cedar::unit::milli * cedar::unit::seconds));
  if (
      !cedar::aux::math::isZero<float>(test_arm_velocity->getJointVelocities().at<float>(0, 0) - 0.1,1e-6)
      || !cedar::aux::math::isZero<float>(test_arm_velocity->getJointVelocities().at<float>(1, 0) - 0.2,1e-6)
      || !cedar::aux::math::isZero<float>(test_arm_velocity->getJointVelocities().at<float>(2, 0) - 0.3,1e-6)
      || !cedar::aux::math::isZero<float>(test_arm_velocity->getJointVelocities().at<float>(3, 0) - 0.4,1e-6)
      )
  {
    errors++;
    std::cout << test_arm_velocity->getJointAngles() << std::endl;
    std::cout << test_arm_velocity->getJointVelocities() << std::endl;
    std::cout << "ERROR with setJointVelocities(matrix) or getJointVelocities()" << std::endl;
  }
  test_arm_velocity->stopCommunication();

  // create instance of test class
  std::cout << "testing acceleration control ..." << std::endl;
  auto accelrobot = boost::make_shared< cedar::dev::Robot >();
  accelrobot->readJson("test_configuration.json");

  cedar::dev::SimulatedKinematicChainPtr test_arm_acceleration = accelrobot->getComponent< cedar::dev::SimulatedKinematicChain >("arm");
  test_arm_acceleration->startCommunication();

  //--------------------------------------------------------------------------------------------------------------------
  // single angle acceleration
  //--------------------------------------------------------------------------------------------------------------------
  std::cout << "test: single acceleration functions" << std::endl;
  test_arm_acceleration->setJointAcceleration(0, 0.001);
  test_arm_acceleration->setJointAcceleration(1, 0.002);
  test_arm_acceleration->setJointAcceleration(2, 0.0015);
  test_arm_acceleration->setJointAcceleration(3, 0.0025);
  test_arm_acceleration->waitUntilCommunicated();
  //cedar::aux::sleep(cedar::unit::Time(50.0 * cedar::unit::milli * cedar::unit::seconds));
  if (
      !cedar::aux::math::isZero<float>(test_arm_acceleration->getJointAcceleration(0) - 0.001,1e-6)
      || !cedar::aux::math::isZero<float>(test_arm_acceleration->getJointAcceleration(1) - 0.002,1e-6)
      || !cedar::aux::math::isZero<float>(test_arm_acceleration->getJointAcceleration(2) - 0.0015,1e-6)
      || !cedar::aux::math::isZero<float>(test_arm_acceleration->getJointAcceleration(3) - 0.0025,1e-6)
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
  std::vector<float> acceleration_vector;
//  acceleration_vector.push_back(0.005);
//  acceleration_vector.push_back(0.004);
//  acceleration_vector.push_back(0.003);
//  acceleration_vector.push_back(0.002);

  acceleration_vector.push_back(0.05);
  acceleration_vector.push_back(0.04);
  acceleration_vector.push_back(0.03);
  acceleration_vector.push_back(0.02);
  test_arm_acceleration->setJointAccelerations(acceleration_vector);
  test_arm_acceleration->waitUntilCommunicated();
  //cedar::aux::sleep(cedar::unit::Time(50.0 * cedar::unit::milli * cedar::unit::seconds));
  if (
      !cedar::aux::math::isZero<float>(test_arm_acceleration->getJointAcceleration(0) - acceleration_vector.at(0),1e-6)
      || !cedar::aux::math::isZero<float>(test_arm_acceleration->getJointAcceleration(1) - acceleration_vector.at(1),1e-6)
      || !cedar::aux::math::isZero<float>(test_arm_acceleration->getJointAcceleration(2) - acceleration_vector.at(2),1e-6)
      || !cedar::aux::math::isZero<float>(test_arm_acceleration->getJointAcceleration(3) - acceleration_vector.at(3),1e-6)
      )
  {
    errors++;
    std::cout << test_arm_acceleration->getJointAngles() << std::endl;
    std::cout << test_arm_acceleration->getJointVelocities() << std::endl;
    std::cout << test_arm_acceleration->getJointAccelerations() << std::endl;
    std::cout << "Desired Accelerations: [0.05;0.04;0.03;0.02]"<<std::endl;
    std::cout << "ERROR with setJointAccelerations(vector) or getJointAccelerations()" << std::endl;
  }

  //--------------------------------------------------------------------------------------------------------------------
  // cv::Mat of acceleration values
  //--------------------------------------------------------------------------------------------------------------------
  std::cout << "test: cv::Mat Acceleration functions" << std::endl;
  cv::Mat acceleration_matrix = cv::Mat::zeros(4, 1, CV_32FC1);
  acceleration_matrix.at<float>(0, 0) = 0.001;
  acceleration_matrix.at<float>(1, 0) = 0.002;
  acceleration_matrix.at<float>(2, 0) = 0.003;
  acceleration_matrix.at<float>(3, 0) = 0.004;
  test_arm_acceleration->setJointAccelerations(acceleration_matrix);
  test_arm_acceleration->waitUntilCommunicated();
  //cedar::aux::sleep(cedar::unit::Time(50.0 * cedar::unit::milli * cedar::unit::seconds));
  if (
      !cedar::aux::math::isZero<float>(test_arm_acceleration->getJointAccelerations().at<float>(0, 0) - acceleration_matrix.at<float>(0, 0),1e-6)
      || !cedar::aux::math::isZero<float>(test_arm_acceleration->getJointAccelerations().at<float>(1, 0) - acceleration_matrix.at<float>(1, 0),1e-6)
      || !cedar::aux::math::isZero<float>(test_arm_acceleration->getJointAccelerations().at<float>(2, 0) - acceleration_matrix.at<float>(2, 0),1e-6)
      || !cedar::aux::math::isZero<float>(test_arm_acceleration->getJointAccelerations().at<float>(3, 0) - acceleration_matrix.at<float>(3, 0),1e-6)
      )
  {
    errors++;
    std::cout << test_arm_acceleration->getJointAngles() << std::endl;
    std::cout << test_arm_acceleration->getJointVelocities() << std::endl;
    std::cout << test_arm_acceleration->getJointAccelerations() << std::endl;
    std::cout << "ERROR with setJointAccelerations(matrix) or getJointAccelerations()" << std::endl;
  }
  test_arm_acceleration->stopCommunication();

  // create instance of test class
  std::cout << "testing exclusiveness of commands ..." << std::endl;

  auto exclurobot = boost::make_shared< cedar::dev::Robot >();
  exclurobot->readJson("test_configuration.json");

  cedar::dev::SimulatedKinematicChainPtr test_arm_exclusive = exclurobot->getComponent< cedar::dev::SimulatedKinematicChain >("arm");
  test_arm_exclusive->startCommunication();
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

  // create instance of test class
  std::cout << "testing setting of initial configurations ..." << std::endl;

  auto inirobot = boost::make_shared< cedar::dev::Robot >();
  inirobot->readJson("test_configuration.json");

  cedar::dev::SimulatedKinematicChainPtr test_arm_initial = inirobot->getComponent< cedar::dev::SimulatedKinematicChain >("arm");

  CEDAR_UNIT_TEST_BEGIN_EXCEPTION_FREE_CODE();
  test_arm_initial->applyInitialConfiguration("default");
  CEDAR_UNIT_TEST_END_EXCEPTION_FREE_CODE
  (
    errors,
    "applying existing initial configuration."
  );

  CEDAR_UNIT_TEST_BEGIN_EXPECTING_EXCEPTION();
  test_arm_initial->applyInitialConfiguration("peter");
  CEDAR_UNIT_TEST_END_EXPECTING_SPECIFIC_EXCEPTION
  (
    errors,
    "applying non-existing initial configuration.",
    cedar::dev::KinematicChain::InitialConfigurationNotFoundException
  );

  //--------------------------------------------------------------------------------------------------------------------
  // check configuration
  //--------------------------------------------------------------------------------------------------------------------

//  test_arm_position->applyInitialConfiguration("default");
  std::cout << "testing coordinate frames ..." << std::endl;

  auto frobot = boost::make_shared< cedar::dev::Robot >();
  frobot->readJson("test_configuration.json");

  cedar::dev::SimulatedKinematicChainPtr test_coordinate_frames = inirobot->getComponent< cedar::dev::SimulatedKinematicChain >("arm");
  test_coordinate_frames->startCommunication();
  test_coordinate_frames->setJointAngles( cv::Mat::zeros( test_arm_position->getNumberOfJoints(), 1, CV_32F) );
  test_coordinate_frames->waitUntilCommunicated();
  //cedar::aux::sleep(test_coordinate_frames->getDeviceStepSize() * 1.5);
  test_coordinate_frames->stopCommunication();




  std::cout << "checking the root coordinate frame translation" << std::endl;
  cedar::aux::LocalCoordinateFramePtr rootCoordinateFrame = test_coordinate_frames->getRootCoordinateFrame();

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
    !cedar::aux::math::isZero(rootRotation.at<float>(0, 0) - (0.0))
    || !cedar::aux::math::isZero(rootRotation.at<float>(0, 1) - (-1.0))
    || !cedar::aux::math::isZero(rootRotation.at<float>(0, 2) - (0.0))
    || !cedar::aux::math::isZero(rootRotation.at<float>(1, 0) - (1.0))
    || !cedar::aux::math::isZero(rootRotation.at<float>(1, 1) - (0.0))
    || !cedar::aux::math::isZero(rootRotation.at<float>(1, 2) - (0.0))
    || !cedar::aux::math::isZero(rootRotation.at<float>(2, 0) - (0.0))
    || !cedar::aux::math::isZero(rootRotation.at<float>(2, 1) - (0.0))
    || !cedar::aux::math::isZero(rootRotation.at<float>(2, 2) - (1.0))
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
  cedar::aux::LocalCoordinateFramePtr endEffectorCoordinateFrame = test_coordinate_frames->getEndEffectorCoordinateFrame();
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
    !cedar::aux::math::isZero(eefRotation.at<float>(0, 0) - (0.0))
    || !cedar::aux::math::isZero(eefRotation.at<float>(0, 1) - (-1.0))
    || !cedar::aux::math::isZero(eefRotation.at<float>(0, 2) - (0.0))
    || !cedar::aux::math::isZero(eefRotation.at<float>(1, 0) - (1.0))
    || !cedar::aux::math::isZero(eefRotation.at<float>(1, 1) - (0.0))
    || !cedar::aux::math::isZero(eefRotation.at<float>(1, 2) - (0.0))
    || !cedar::aux::math::isZero(eefRotation.at<float>(2, 0) - (0.0))
    || !cedar::aux::math::isZero(eefRotation.at<float>(2, 1) - (0.0))
    || !cedar::aux::math::isZero(eefRotation.at<float>(2, 2) - (1.0))
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
  cv::Mat joint_transformation_1 = test_coordinate_frames->getJointTransformation(1);
  cv::Mat joint_transformation_3 = test_coordinate_frames->getJointTransformation(3);
  if (
      // transformation to joint 1 frame
      !cedar::aux::math::isZero(joint_transformation_1.at<float>(0, 0) - 0)
      || !cedar::aux::math::isZero(joint_transformation_1.at<float>(0, 1) - -1)
      || !cedar::aux::math::isZero(joint_transformation_1.at<float>(0, 2) - 0)

      || !cedar::aux::math::isZero(joint_transformation_1.at<float>(1, 0) - 1)
      || !cedar::aux::math::isZero(joint_transformation_1.at<float>(1, 1) - 0)
      || !cedar::aux::math::isZero(joint_transformation_1.at<float>(1, 2) - 0)

      || !cedar::aux::math::isZero(joint_transformation_1.at<float>(2, 0) - 0)
      || !cedar::aux::math::isZero(joint_transformation_1.at<float>(2, 1) - 0)
      || !cedar::aux::math::isZero(joint_transformation_1.at<float>(2, 2) - 1)

      || !cedar::aux::math::isZero(joint_transformation_1.at<float>(0, 3) - 0)
      || !cedar::aux::math::isZero(joint_transformation_1.at<float>(1, 3) - 0)
      || !cedar::aux::math::isZero(joint_transformation_1.at<float>(2, 3) - 2)

      || !cedar::aux::math::isZero(joint_transformation_1.at<float>(3, 0) - 0)
      || !cedar::aux::math::isZero(joint_transformation_1.at<float>(3, 1) - 0)
      || !cedar::aux::math::isZero(joint_transformation_1.at<float>(3, 2) - 0)
      || !cedar::aux::math::isZero(joint_transformation_1.at<float>(3, 3) - 1) )
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
      || !cedar::aux::math::isZero(joint_transformation_3.at<float>(0, 0) - 0)
      || !cedar::aux::math::isZero(joint_transformation_3.at<float>(0, 1) - -1)
      || !cedar::aux::math::isZero(joint_transformation_3.at<float>(0, 2) - 0)

      || !cedar::aux::math::isZero(joint_transformation_3.at<float>(1, 0) - 1)
      || !cedar::aux::math::isZero(joint_transformation_3.at<float>(1, 1) - 0)
      || !cedar::aux::math::isZero(joint_transformation_3.at<float>(1, 2) - 0)

      || !cedar::aux::math::isZero(joint_transformation_3.at<float>(2, 0) - 0)
      || !cedar::aux::math::isZero(joint_transformation_3.at<float>(2, 1) - 0)
      || !cedar::aux::math::isZero(joint_transformation_3.at<float>(2, 2) - 1)

      || !cedar::aux::math::isZero(joint_transformation_3.at<float>(0, 3) - 0)
      || !cedar::aux::math::isZero(joint_transformation_3.at<float>(1, 3) - 0)
      || !cedar::aux::math::isZero(joint_transformation_3.at<float>(2, 3) - 6)

      || !cedar::aux::math::isZero(joint_transformation_3.at<float>(3, 0) - 0)
      || !cedar::aux::math::isZero(joint_transformation_3.at<float>(3, 1) - 0)
      || !cedar::aux::math::isZero(joint_transformation_3.at<float>(3, 2) - 0)
      || !cedar::aux::math::isZero(joint_transformation_3.at<float>(3, 3) - 1)
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
  cv::Mat origin = cv::Mat::zeros( 4, 1, CV_32FC1 );
  origin.at<float>( 3, 0 ) = 1;
  cv::Mat jacobian_1 = cv::Mat::zeros(3, 4, CV_32FC1);
  cv::Mat jacobian_3 = cv::Mat::zeros(3, 4, CV_32FC1);
  jacobian_1 = test_coordinate_frames->calculateCartesianJacobian(origin, 1, cedar::dev::KinematicChain::LOCAL_COORDINATES);
  jacobian_3 = test_coordinate_frames->calculateCartesianJacobian(origin, 3, cedar::dev::KinematicChain::LOCAL_COORDINATES);
  if (
      // Jacobian of joint 1
      !cedar::aux::math::isZero(jacobian_1.at<float>(0, 0) - 0)
      || !cedar::aux::math::isZero(jacobian_1.at<float>(1, 0) - 2)
      || !cedar::aux::math::isZero(jacobian_1.at<float>(2, 0) - 0)

      || !cedar::aux::math::isZero(jacobian_1.at<float>(0, 1) - 0)
      || !cedar::aux::math::isZero(jacobian_1.at<float>(1, 1) - 0)
      || !cedar::aux::math::isZero(jacobian_1.at<float>(2, 1) - 0)

      || !cedar::aux::math::isZero(jacobian_1.at<float>(0, 2) - 0)
      || !cedar::aux::math::isZero(jacobian_1.at<float>(1, 2) - 0)
      || !cedar::aux::math::isZero(jacobian_1.at<float>(2, 2) - 0)

      || !cedar::aux::math::isZero(jacobian_1.at<float>(0, 3) - 0)
      || !cedar::aux::math::isZero(jacobian_1.at<float>(1, 3) - 0)
      || !cedar::aux::math::isZero(jacobian_1.at<float>(2, 3) - 0)
      || !cedar::aux::math::isZero
      (
        norm
        (
          jacobian_1
          - test_coordinate_frames->calculateCartesianJacobian(origin, 1, cedar::dev::KinematicChain::LOCAL_COORDINATES)
        )
      )
      // Jacobian of joint 3
      || !cedar::aux::math::isZero(jacobian_3.at<float>(0, 0) - 0)
      || !cedar::aux::math::isZero(jacobian_3.at<float>(1, 0) - 6)
      || !cedar::aux::math::isZero(jacobian_3.at<float>(2, 0) - 0)

      || !cedar::aux::math::isZero(jacobian_3.at<float>(0, 1) - 0)
      || !cedar::aux::math::isZero(jacobian_3.at<float>(1, 1) - 4)
      || !cedar::aux::math::isZero(jacobian_3.at<float>(2, 1) - 0)

      || !cedar::aux::math::isZero(jacobian_3.at<float>(0, 2) - 0)
      || !cedar::aux::math::isZero(jacobian_3.at<float>(1, 2) - 2)
      || !cedar::aux::math::isZero(jacobian_3.at<float>(2, 2) - 0)

      || !cedar::aux::math::isZero(jacobian_3.at<float>(0, 3) - 0)
      || !cedar::aux::math::isZero(jacobian_3.at<float>(1, 3) - 0)
      || !cedar::aux::math::isZero(jacobian_3.at<float>(2, 3) - 0)
      || !cedar::aux::math::isZero
      (
        norm
        (
          jacobian_3
          - test_coordinate_frames->calculateCartesianJacobian(origin, 3, cedar::dev::KinematicChain::LOCAL_COORDINATES)
        )
      )
    )
  {
//std::cout << "test j1 " << jacobian_1 << std::endl;
//std::cout << "test j3 " << jacobian_3 << std::endl;

    errors++;
    std::cout << "ERROR with calculateCartesianJacobian()" << std::endl;
  }
  else
  {
    std::cout << "passed" << std::endl;
  }

  std::cout << "test: calculateSpatialJacobian" << std::endl;
  cv::Mat spatial_jacobian = test_coordinate_frames->calculateSpatialJacobian(test_arm_position->getNumberOfJoints()-1);
  if (
      !cedar::aux::math::isZero(spatial_jacobian.at<float>(0, 0) - 0)
      || !cedar::aux::math::isZero(spatial_jacobian.at<float>(1, 0) - 0)
      || !cedar::aux::math::isZero(spatial_jacobian.at<float>(2, 0) - 0)
      || !cedar::aux::math::isZero(spatial_jacobian.at<float>(3, 0) - -1)
      || !cedar::aux::math::isZero(spatial_jacobian.at<float>(4, 0) - 0)
      || !cedar::aux::math::isZero(spatial_jacobian.at<float>(5, 0) - 0)

      || !cedar::aux::math::isZero(spatial_jacobian.at<float>(0, 1) - 0)
      || !cedar::aux::math::isZero(spatial_jacobian.at<float>(1, 1) - -2)
      || !cedar::aux::math::isZero(spatial_jacobian.at<float>(2, 1) - 0)
      || !cedar::aux::math::isZero(spatial_jacobian.at<float>(3, 1) - -1)
      || !cedar::aux::math::isZero(spatial_jacobian.at<float>(4, 1) - 0)
      || !cedar::aux::math::isZero(spatial_jacobian.at<float>(5, 1) - 0)

      || !cedar::aux::math::isZero(spatial_jacobian.at<float>(0, 2) - 0)
      || !cedar::aux::math::isZero(spatial_jacobian.at<float>(1, 2) - -4)
      || !cedar::aux::math::isZero(spatial_jacobian.at<float>(2, 2) - 0)
      || !cedar::aux::math::isZero(spatial_jacobian.at<float>(3, 2) - -1)
      || !cedar::aux::math::isZero(spatial_jacobian.at<float>(4, 2) - 0)
      || !cedar::aux::math::isZero(spatial_jacobian.at<float>(5, 2) - 0)

      || !cedar::aux::math::isZero(spatial_jacobian.at<float>(0, 3) - 0)
      || !cedar::aux::math::isZero(spatial_jacobian.at<float>(1, 3) - -6)
      || !cedar::aux::math::isZero(spatial_jacobian.at<float>(2, 3) - 0)
      || !cedar::aux::math::isZero(spatial_jacobian.at<float>(3, 3) - -1)
      || !cedar::aux::math::isZero(spatial_jacobian.at<float>(4, 3) - 0)
      || !cedar::aux::math::isZero(spatial_jacobian.at<float>(5, 3) - 0)
     )
  {
//std::cout << " spatial " << spatial_jacobian << std::endl;    
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
  cv::Mat end_effector_jacobian = test_coordinate_frames->calculateEndEffectorJacobian();
  if (
      !cedar::aux::math::isZero(end_effector_jacobian.at<float>(0, 0) - 0)
      || !cedar::aux::math::isZero(end_effector_jacobian.at<float>(1, 0) - 8)
      || !cedar::aux::math::isZero(end_effector_jacobian.at<float>(2, 0) - 0)

      || !cedar::aux::math::isZero(end_effector_jacobian.at<float>(0, 1) - 0)
      || !cedar::aux::math::isZero(end_effector_jacobian.at<float>(1, 1) - 6)
      || !cedar::aux::math::isZero(end_effector_jacobian.at<float>(2, 1) - 0)

      || !cedar::aux::math::isZero(end_effector_jacobian.at<float>(0, 2) - 0)
      || !cedar::aux::math::isZero(end_effector_jacobian.at<float>(1, 2) - 4)
      || !cedar::aux::math::isZero(end_effector_jacobian.at<float>(2, 2) - 0)

      || !cedar::aux::math::isZero(end_effector_jacobian.at<float>(0, 3) - 0)
      || !cedar::aux::math::isZero(end_effector_jacobian.at<float>(1, 3) - 2)
      || !cedar::aux::math::isZero(end_effector_jacobian.at<float>(2, 3) - 0)
     )
  {
//std::cout << "end eff jacobian " << end_effector_jacobian << std::endl;    
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

  cv::Mat p_local = cv::Mat::zeros(4, 1, CV_32FC1);
  p_local.at<float>(2, 0) = 1.0;
  p_local.at<float>(3, 0) = 1.0;


  auto robot = boost::make_shared< cedar::dev::Robot >();
  robot->readJson("resource://robots/unittests/complex_test_arm_configuration.json");

  cedar::dev::SimulatedKinematicChainPtr complex_test_arm = robot->getComponent< cedar::dev::SimulatedKinematicChain >("arm");

  complex_test_arm->startCommunication();

  theta = cv::Mat(4, 1, CV_32FC1);
  thetaDot = cv::Mat(4, 1, CV_32FC1);
  thetaTwoDot = cv::Mat(4, 1, CV_32FC1);
  cv::randn(theta, cv::Scalar(0), cv::Scalar(1));
  cv::randn(thetaDot, cv::Scalar(0), cv::Scalar(1));
  cv::randn(thetaTwoDot, cv::Scalar(0), cv::Scalar(1));

  complex_test_arm->setJointAngles(theta);
  complex_test_arm->waitUntilCommunicated();

  complex_test_arm->clearUserSideCommand();
  complex_test_arm->setJointVelocities(thetaDot);
  complex_test_arm->waitUntilCommunicated();

  complex_test_arm->clearUserSideCommand();
  complex_test_arm->setJointAccelerations(thetaTwoDot);
  complex_test_arm->waitUntilCommunicated();

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
  float delta_t = complex_test_arm->getCommunicationStepSize() / cedar::unit::DEFAULT_TIME_UNIT; //1e-07;

//  complex_test_arm->startCommunication();
  complex_test_arm->clearUserSideCommand();
  complex_test_arm->setJointVelocities
  (
    complex_test_arm->getJointVelocities()
    + delta_t*complex_test_arm->getJointAccelerations()
  );
  complex_test_arm->waitUntilCommunicated();
  complex_test_arm->clearUserSideCommand();
  complex_test_arm->setJointAngles
  (
    complex_test_arm->getJointAngles()
    + delta_t*complex_test_arm->getJointVelocities()
    + delta_t*delta_t*complex_test_arm->getJointAccelerations()
  );

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
  // spatial Jacobian temporal derivative
  //--------------------------------------------------------------------------------------------------------------------
  std::cout << "test: calculateSpatialJacobianTemporalDerivative" << std::endl;
  if (
      !cedar::test::checkZero("calculateSpatialJacobianTemporalDerivative", pow(norm(spatial_jacobian_dot - spatial_jacobian_dot_numeric)/(spatial_jacobian_dot.cols*6), 2), 1e-5)
     )
  {
    errors++;
  }

  //--------------------------------------------------------------------------------------------------------------------
  // Cartesian Jacobian temporal derivative
  //--------------------------------------------------------------------------------------------------------------------
  std::cout << "test: calculateCartesianJacobianTemporalDerivative" << std::endl;
  if (
      !cedar::test::checkZero("calculateCartesianJacobianTemporalDerivative", pow(norm(cartesian_jacobian_dot - cartesian_jacobian_dot_numeric)/(cartesian_jacobian_dot.cols*3), 2), 1e-5)
     )
  {
    errors++;
  }

  //--------------------------------------------------------------------------------------------------------------------
  // end-effector velocity
  //--------------------------------------------------------------------------------------------------------------------
  std::cout << "test: calculateEndEffectorVelocity" << std::endl;
  if (!cedar::test::checkZero("calculateEndEffectorVelocity", pow(norm(v_eef - v_eef_numeric)/3.0, 2), 1e-4))
  {
    errors++;
  }

  //--------------------------------------------------------------------------------------------------------------------
  // end-effector acceleration
  //--------------------------------------------------------------------------------------------------------------------
  std::cout << "test: calculateEndEffectorAcceleration" << std::endl;
  if (!cedar::test::checkZero("calculateEndEffectorAcceleration", pow(norm(a_eef - a_eef_numeric)/3.0, 2), 1e-3))
  {
    errors++;
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
