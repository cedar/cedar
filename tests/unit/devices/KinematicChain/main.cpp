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

// CEDAR INCLUDES
#include "cedar/auxiliaries/math/tools.h"
#include "cedar/auxiliaries/LocalCoordinateFrame.h"
#include "cedar/auxiliaries/DoubleVectorParameter.h"
#include "cedar/auxiliaries/math/LimitsParameter.h"
#include "cedar/devices/KinematicChain.h"
#include "tests/unit/devices/KinematicChain/TestKinematicChain.h"
#include "cedar/auxiliaries/math/constants.h"
#include "cedar/auxiliaries/systemFunctions.h"
#include "cedar/auxiliaries/CallFunctionInThread.h"
#include "cedar/units/UnitMatrix.h"
#include "cedar/auxiliaries/sleepFunctions.h"

// SYSTEM INCLUDES
#include <QApplication>

CEDAR_GENERATE_POINTER_TYPES(TestKinematicChain);
unsigned int errors;

void test()
{
#if 0  
  // the number of errors encountered in this test
  
  // create instance of test class
  TestKinematicChainPtr test_arm(new TestKinematicChain());
  TestKinematicChainPtr acceleration_test_arm(new TestKinematicChain());
  std::cout << "reading configuration from test_arm->json" << std::endl;
  test_arm->readJson("test_arm.json");
  test_arm->startDevice();
  acceleration_test_arm->readJson("acceleration_test_arm.json");
  acceleration_test_arm->startDevice();

  cv::Mat theta;
  cv::Mat thetaDot;
  cv::Mat thetaTwoDot;
  
  test_arm->addInitialConfiguration("default", cv::Mat::zeros( test_arm->getNumberOfJoints(), 1, CV_64F) + 0.001);
  test_arm->applyInitialConfiguration("default");
 
  acceleration_test_arm->addInitialConfiguration("default", cv::Mat::zeros( acceleration_test_arm->getNumberOfJoints(), 1, CV_64F) + 0.001);
  acceleration_test_arm->applyInitialConfiguration("default");
  complex_test_arm->addInitialConfiguration("default", cv::Mat::zeros( complex_test_arm->getNumberOfJoints(), 1, CV_64F) + 0.001);
  complex_test_arm->applyInitialConfiguration("default");



#if 0

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
  else
  {
    std::cout << "passed" << std::endl;
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
  else
  {
    std::cout << "passed" << std::endl;
  }


#endif
#endif

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
