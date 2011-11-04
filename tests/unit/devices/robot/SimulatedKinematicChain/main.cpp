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

// LOCAL INCLUDES

// PROJECT INCLUDES
#include "cedar/devices/robot/SimulatedKinematicChain.h"
#include "cedar/auxiliaries/math/tools.h"
#include "cedar/auxiliaries/math/constants.h"
#include "cedar/auxiliaries/LogFile.h"

// SYSTEM INCLUDES
#include <vector>
#include <opencv2/opencv.hpp>

using namespace cedar::dev::robot;
using namespace cedar::aux;
using namespace cv;

int main()
{
  LogFile log_file("SimulatedKinematicChain.log");
  log_file.addTimeStamp();
  log_file << std::endl;
  // the number of errors encountered in this test
  int errors = 0;
  
  // create instance of test class
  log_file << "reading kinematic chain from conig file ..." << std::endl;
  ReferenceGeometryPtr p_reference_geometry(new ReferenceGeometry("test.conf"));
  SimulatedKinematicChain test_arm(p_reference_geometry);
  
  //--------------------------------------------------------------------------------------------------------------------
  // single angle
  //--------------------------------------------------------------------------------------------------------------------
  log_file << "test: single angle functions" << std::endl;
  test_arm.setJointAngle(0, 1);
  test_arm.setJointAngle(1, 2.0);
  test_arm.setJointAngle(2, cedar::aux::math::pi/2);
  test_arm.setJointAngle(3, sqrt(2.0));
  if (
      !IsZero(test_arm.getJointAngle(0) - 1.0)
      || !IsZero(test_arm.getJointAngle(1) - 2.0)
      || !IsZero(test_arm.getJointAngle(2) - cedar::aux::math::pi/2.0)
      || !IsZero(test_arm.getJointAngle(3) - sqrt(2.0))
      )
  {
    errors++;
    log_file << "ERROR with setJointAngle() or getJointAngle()" << std::endl;
  }
  
  //--------------------------------------------------------------------------------------------------------------------
  // std::vector of angle values
  //--------------------------------------------------------------------------------------------------------------------
  log_file << "test: std::vector angle functions" << std::endl;
  std::vector<double> angle_vector;
  angle_vector.push_back(0.5);
  angle_vector.push_back(1.5);
  angle_vector.push_back(2.5);
  angle_vector.push_back(3.5);
  test_arm.setJointAngles(angle_vector);
  if (
      !IsZero(test_arm.getJointAngles()[0] - 0.5)
      || !IsZero(test_arm.getJointAngles()[1] - 1.5)
      || !IsZero(test_arm.getJointAngles()[2] - 2.5)
      || !IsZero(test_arm.getJointAngles()[3] - 3.5)
      )
  {
    errors++;
    log_file << "ERROR with setJointAngles() or getJointAngles()" << std::endl;
  }

  //--------------------------------------------------------------------------------------------------------------------
  // cv::Mat of angle values
  //--------------------------------------------------------------------------------------------------------------------
  log_file << "test: cv::Mat angle functions" << std::endl;
  cv::Mat angle_matrix = cv::Mat::zeros(4, 1, CV_64FC1);
  angle_matrix.at<double>(0, 0) = 0.1;
  angle_matrix.at<double>(1, 0) = 0.2;
  angle_matrix.at<double>(2, 0) = 0.3;
  angle_matrix.at<double>(3, 0) = 0.4;
  test_arm.setJointAngles(angle_matrix);
  if (
      !IsZero(test_arm.getJointAnglesMatrix().at<double>(0, 0) - 0.1)
      || !IsZero(test_arm.getJointAnglesMatrix().at<double>(1, 0) - 0.2)
      || !IsZero(test_arm.getJointAnglesMatrix().at<double>(2, 0) - 0.3)
      || !IsZero(test_arm.getJointAnglesMatrix().at<double>(3, 0) - 0.4)
      )
  {
    errors++;
    log_file << "ERROR with setJointAnglesMatrix() or getJointAnglesMatrix()" << std::endl;
  }
  
  log_file << "test finished, there were " << errors << " errors" << std::endl;
  if (errors > 255)
  {
    errors = 255;
  }
  return errors;
}
