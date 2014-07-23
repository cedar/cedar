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
  test_arm_position->addInitialConfiguration("peter", cv::Mat::zeros(test_arm_position->getNumberOfJoints(), 1, CV_64F));
  test_arm_position->applyInitialConfiguration("peter");
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
    std::cout << "ERROR with setJointAngles() or getCachedJointAngles()" << std::endl;
  }
  
  // create instance of test class
  std::cout << "testing velocity control ..." << std::endl;
  cedar::dev::SimulatedKinematicChainPtr test_arm_velocity(new cedar::dev::SimulatedKinematicChain());
  test_arm_velocity->readJson("test_arm.json");
  test_arm_velocity->addInitialConfiguration("peter", cv::Mat::zeros(test_arm_position->getNumberOfJoints(), 1, CV_64F));
  test_arm_velocity->applyInitialConfiguration("peter");
  test_arm_velocity->startDevice();

  //--------------------------------------------------------------------------------------------------------------------
  // single angle velocity
  //--------------------------------------------------------------------------------------------------------------------
  std::cout << "test: single angle velocity functions" << std::endl;
  test_arm_velocity->setJointVelocity(0, 0.1);
  test_arm_velocity->setJointVelocity(1, 0.2);
  test_arm_velocity->setJointVelocity(2, 0.15);
  test_arm_velocity->setJointVelocity(3, 0.25);
  cedar::aux::sleep(cedar::unit::Time(20.0 * cedar::unit::milli * cedar::unit::seconds));
  if (
      !cedar::aux::math::isZero(test_arm_velocity->getJointVelocity(0) - 0.1)
      || !cedar::aux::math::isZero(test_arm_velocity->getJointAngle(1) - 0.2)
      || !cedar::aux::math::isZero(test_arm_velocity->getJointAngle(2) - 0.15)
      || !cedar::aux::math::isZero(test_arm_velocity->getJointAngle(3) - 0.25)
      )
  {
    errors++;
    std::cout << "ERROR with setJointVelocity() or getJointVelocity()" << std::endl;
  }

  //--------------------------------------------------------------------------------------------------------------------
  // std::vector of angle values
  //--------------------------------------------------------------------------------------------------------------------
  std::cout << "test: std::vector angle functions" << std::endl;
  std::vector<double> velocity_vector;
  velocity_vector.push_back(0.5);
  velocity_vector.push_back(1.5);
  velocity_vector.push_back(2.5);
  velocity_vector.push_back(3.5);
  test_arm_velocity->setJointVelocities(velocity_vector);
  cedar::aux::sleep(cedar::unit::Time(20.0 * cedar::unit::milli * cedar::unit::seconds));
  if (
      !cedar::aux::math::isZero<double>(test_arm_velocity->getJointVelocity(0) - 0.5)
      || !cedar::aux::math::isZero<double>(test_arm_velocity->getJointVelocity(1) - 1.5)
      || !cedar::aux::math::isZero<double>(test_arm_velocity->getJointVelocity(2) - 2.5)
      || !cedar::aux::math::isZero<double>(test_arm_velocity->getJointVelocity(3) - 3.5)
      )
  {
    errors++;
    std::cout << "ERROR with setJointVelocities(vector) or getJointVelocities()" << std::endl;
  }

  //--------------------------------------------------------------------------------------------------------------------
  // cv::Mat of angle values
  //--------------------------------------------------------------------------------------------------------------------
  std::cout << "test: cv::Mat angle functions" << std::endl;
  cv::Mat velocity_matrix = cv::Mat::zeros(4, 1, CV_64FC1);
  velocity_matrix.at<double>(0, 0) = 0.1;
  velocity_matrix.at<double>(1, 0) = 0.2;
  velocity_matrix.at<double>(2, 0) = 0.3;
  velocity_matrix.at<double>(3, 0) = 0.4;
  test_arm_velocity->setJointVelocities(velocity_matrix);
  cedar::aux::sleep(cedar::unit::Time(20.0 * cedar::unit::milli * cedar::unit::seconds));
  if (
      !cedar::aux::math::isZero(test_arm_velocity->getJointVelocities().at<double>(0, 0) - 0.1)
      || !cedar::aux::math::isZero(test_arm_velocity->getJointVelocities().at<double>(1, 0) - 0.2)
      || !cedar::aux::math::isZero(test_arm_velocity->getJointVelocities().at<double>(2, 0) - 0.3)
      || !cedar::aux::math::isZero(test_arm_velocity->getJointVelocities().at<double>(3, 0) - 0.4)
      )
  {
    errors++;
    std::cout << "ERROR with setJointVelocities(matrix) or getJointVelocities()" << std::endl;
  }
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
