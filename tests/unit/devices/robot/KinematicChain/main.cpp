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
 ----- Date:        2010 11 30
 
 ----- Description: unit test for the @em cedar::dev::robot::SimulatedKinematicChain class.
 
 ----- Credits:
 ---------------------------------------------------------------------------------------------------------------------*/

// CEDAR INCLUDES
#include "cedar/auxiliaries/math/tools.h"
#include "cedar/auxiliaries/LocalCoordinateFrame.h"
#include "cedar/auxiliaries/DoubleVectorParameter.h"
#include "cedar/auxiliaries/math/LimitsParameter.h"
#include "cedar/devices/robot/KinematicChain.h"
#include "tests/unit/devices/robot/KinematicChain/TestKinematicChain.h"

// SYSTEM INCLUDES


int main()
{
  // the number of errors encountered in this test
  int errors = 0;
  
  // create instance of test class
  TestKinematicChain test_arm("test.conf");
  // do I actually need the config file constructor?
  // - yes, for the LoopedThread stuff
  std::cout << "reading configuration from test_arm.json" << std::endl;
  test_arm.readJson("test_arm.json");
  
  //--------------------------------------------------------------------------------------------------------------------
  // check configuration
  //--------------------------------------------------------------------------------------------------------------------

  std::cout << "checking the root coordinate frame..." << std::endl;
  cedar::aux::LocalCoordinateFramePtr rootCoordinateFrame = test_arm.getRootCoordinateFrame();
  if
  (
    !IsZero(rootCoordinateFrame->getTranslationX() - (2.0))
    || !IsZero(rootCoordinateFrame->getTranslationY() - (0.0))
    || !IsZero(rootCoordinateFrame->getTranslationZ() - (0.0))
  )
  {
    errors++;
    std::cout << "ERROR with root coordinate frame translation, read:" << std::endl;
    cedar::aux::math::write(rootCoordinateFrame->getTranslation());
  }
  cv::Mat rootRotation = rootCoordinateFrame->getRotation();
  if
  (
    !IsZero(rootRotation.at<double>(0, 0) - (0.0))
    || !IsZero(rootRotation.at<double>(0, 1) - (-1.0))
    || !IsZero(rootRotation.at<double>(0, 2) - (0.0))
    || !IsZero(rootRotation.at<double>(1, 0) - (1.0))
    || !IsZero(rootRotation.at<double>(1, 1) - (0.0))
    || !IsZero(rootRotation.at<double>(1, 2) - (0.0))
    || !IsZero(rootRotation.at<double>(2, 0) - (0.0))
    || !IsZero(rootRotation.at<double>(2, 1) - (0.0))
    || !IsZero(rootRotation.at<double>(2, 2) - (1.0))
  )
  {
    errors++;
    std::cout << "ERROR with root coordinate frame rotation, read:" << std::endl;
    cedar::aux::math::write(rootRotation);
  }

  std::cout << "checking the end-effector coordinate frame..." << std::endl;
  cedar::aux::LocalCoordinateFramePtr endEffectorCoordinateFrame = test_arm.getEndEffectorCoordinateFrame();
  if
  (
    !IsZero(endEffectorCoordinateFrame->getTranslationX() - (0.0))
    || !IsZero(endEffectorCoordinateFrame->getTranslationY() - (2.0))
    || !IsZero(endEffectorCoordinateFrame->getTranslationZ() - (8.0))
  )
  {
    errors++;
    std::cout << "ERROR with end-effector coordinate frame translation, read:" << std::endl;
    cedar::aux::math::write(endEffectorCoordinateFrame->getTranslation());
  }
  cv::Mat eefRotation = endEffectorCoordinateFrame->getRotation();
  if
  (
    !IsZero(eefRotation.at<double>(0, 0) - (1.0))
    || !IsZero(eefRotation.at<double>(0, 1) - (0.0))
    || !IsZero(eefRotation.at<double>(0, 2) - (0.0))
    || !IsZero(eefRotation.at<double>(1, 0) - (0.0))
    || !IsZero(eefRotation.at<double>(1, 1) - (1.0))
    || !IsZero(eefRotation.at<double>(1, 2) - (0.0))
    || !IsZero(eefRotation.at<double>(2, 0) - (0.0))
    || !IsZero(eefRotation.at<double>(2, 1) - (0.0))
    || !IsZero(eefRotation.at<double>(2, 2) - (1.0))
  )
  {
    errors++;
    std::cout << "ERROR with end-effector coordinate frame rotation, read:" << std::endl;
    cedar::aux::math::write(eefRotation);
  }

  std::cout << "checking the first joint ..." << std::endl;
  cedar::dev::robot::KinematicChain::JointPtr joint = test_arm.getJoint(0);
  if
  (
    !IsZero(joint->_mpPosition->at(0) - (0.0))
    || !IsZero(joint->_mpPosition->at(1) - (2.0))
    || !IsZero(joint->_mpPosition->at(2) - (0.0))
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
    !IsZero(joint->_mpAxis->at(0) - (0.0))
    || !IsZero(joint->_mpAxis->at(1) - (1.0))
    || !IsZero(joint->_mpAxis->at(2) - (0.0))
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
    !IsZero(joint->_mpAngleLimits->getLowerLimit() - (-3.1415927))
    || !IsZero(joint->_mpAngleLimits->getUpperLimit() - (3.1415927))
  )
  {
    errors++;
    std::cout << "ERROR with first joint axis limits, read: "
              << joint->_mpAngleLimits->getLowerLimit() << ", "
              << joint->_mpAngleLimits->getUpperLimit() << std::endl;
  }
  if
  (
    !IsZero(joint->_mpVelocityLimits->getLowerLimit() - (-5.0))
    || !IsZero(joint->_mpVelocityLimits->getUpperLimit() - (5.0))
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
  std::cout << "test: getNumberOfJoints()" << std::endl;
  if (!IsZero(test_arm.getNumberOfJoints() - 3))
  {
    errors++;
    std::cout << "ERROR with getNumberOfJoints()" << std::endl;
  }
  
  std::cout << "test finished, there were " << errors << " errors" << std::endl;
  if (errors > 255)
  {
    errors = 255;
  }
  return errors;
}
