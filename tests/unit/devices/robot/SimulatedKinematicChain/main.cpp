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
#include "cedar/auxiliaries/LogFile.h"

// SYSTEM INCLUDES

using namespace cedar::dev::robot;
using namespace cedar::aux;
using namespace cv;

int main()
{
  LogFile log_file("KinematicChainModel.log");
  log_file.addTimeStamp();
  log_file << std::endl;
  // the number of errors encountered in this test
  int errors = 0;
  
  // create instance of test class
  log_file << "constructing reference geometry" << std::endl;
  ReferenceGeometryPtr p_reference_geometry(new ReferenceGeometry("test.conf"));

  log_file << p_reference_geometry->numberOfJoints() << std::endl;

//  ReferenceGeometry::EndEffectorPtr p_end_effector(new ReferenceGeometry::EndEffector());

  log_file << "constructing arm" << std::endl;
  SimulatedKinematicChain test_arm(p_reference_geometry);
  
  //--------------------------------------------------------------------------------------------------------------------
  // setting a single angle
  //--------------------------------------------------------------------------------------------------------------------
  log_file << "test: single angle functions" << std::endl;
  test_arm.setJointAngle(0, 1);
  test_arm.setJointAngle(1, 2.0);
  test_arm.setJointAngle(2, M_PI/2);
  test_arm.setJointAngle(3, sqrt(2));
  if (
      !IsZero(test_arm.getJointAngle(0) - 1.0)
      || !IsZero(test_arm.getJointAngle(0) - 2.0)
      || !IsZero(test_arm.getJointAngle(0) - M_PI/2.0)
      || !IsZero(test_arm.getJointAngle(0) - sqrt(2.0))
      )
  {
    errors++;
    log_file << "ERROR with setJointAngle() or getJointAngle()" << std::endl;
  }
  
  // ...
  
  
  log_file << "test finished, there were " << errors << " errors" << std::endl;
  if (errors > 255)
  {
    errors = 255;
  }
  return errors;
}











