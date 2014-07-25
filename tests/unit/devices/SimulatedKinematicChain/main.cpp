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
  cedar::dev::SimulatedKinematicChainPtr test_arm_position(new cedar::dev::SimulatedKinematicChain());
  test_arm_position->readJson("test_arm.json");
  test_arm_position->startDevice();
  
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

  // create instance of test class
  std::cout << "testing setting of initial configurations ..." << std::endl;
  cedar::dev::SimulatedKinematicChainPtr test_arm_initial(new cedar::dev::SimulatedKinematicChain());
  test_arm_initial->readJson("test_arm.json");

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
