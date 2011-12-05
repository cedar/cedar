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

    Institute:   Ruhr-Universitaet Bochum
                 Institut fuer Neuroinformatik

    File:        main.cpp

    Maintainer:  Mathis Richter
    Email:       mathis.richter@ini.rub.de
    Date:        2010 11 12

    Description: Implements all unit tests for the @em cedar::dev::robot::ReferenceGeometry class.

    Credits:

======================================================================================================================*/


// LOCAL INCLUDES

// PROJECT INCLUDES
#include "cedar/devices/robot/ReferenceGeometry.h"
#include "cedar/auxiliaries/LogFile.h"
#include "cedar/auxiliaries/math/tools.h"

// SYSTEM INCLUDES
#include <string>

using namespace cedar::aux;
using namespace cedar::dev::robot;

int main()
{
  LogFile log_file("ReferenceGeometry.log");
  log_file.addTimeStamp();
  log_file << std::endl;

  // the number of errors encountered in this test
  int errors = 0;

  ReferenceGeometry reference_geometry("test.conf");
  log_file << "name: " << reference_geometry.getName() << "\n";

  //--------------------------------------------------------------------------------------------------------------------
  // number of joints
  //--------------------------------------------------------------------------------------------------------------------
  log_file << "test: getNumberOfJoints" << std::endl;
  if (reference_geometry.getNumberOfJoints() != 4)
  {
    errors++;
    log_file << "ERROR with getNumberOfJoints()" << std::endl;
  }

  //--------------------------------------------------------------------------------------------------------------------
  // getJoint
  //--------------------------------------------------------------------------------------------------------------------
  log_file << "test: getJoint" << std::endl;
  if (
       // transformation to joint 1 frame
       !IsZero(reference_geometry.getJoint(0)->position[0] - 0)
       || !IsZero(reference_geometry.getJoint(0)->position[1] - 2)
       || !IsZero(reference_geometry.getJoint(0)->position[2] - 0)
       || !IsZero(reference_geometry.getJoint(0)->axis[0] - 0)
       || !IsZero(reference_geometry.getJoint(0)->axis[1] - 1)
       || !IsZero(reference_geometry.getJoint(0)->axis[2] - 0)
       || !IsZero(reference_geometry.getJoint(0)->angleLimits.min - -7.0)
       || !IsZero(reference_geometry.getJoint(0)->angleLimits.max - 7.0)
       || !IsZero(reference_geometry.getJoint(0)->velocityLimits.min - -10.0)
       || !IsZero(reference_geometry.getJoint(0)->velocityLimits.max - 10.0)
       || !IsZero(reference_geometry.getJoint(1)->position[0] - 0)
       || !IsZero(reference_geometry.getJoint(1)->position[1] - 2)
       || !IsZero(reference_geometry.getJoint(1)->position[2] - 2)
       || !IsZero(reference_geometry.getJoint(2)->position[0] - 0)
       || !IsZero(reference_geometry.getJoint(2)->position[1] - 2)
       || !IsZero(reference_geometry.getJoint(2)->position[2] - 4)
       || !IsZero(reference_geometry.getJoint(3)->position[0] - 0)
       || !IsZero(reference_geometry.getJoint(3)->position[1] - 2)
       || !IsZero(reference_geometry.getJoint(3)->position[2] - 6)
     )
  {
    errors++;
    log_file << "ERROR with getJoint()" << std::endl;
  }

  //--------------------------------------------------------------------------------------------------------------------
  // getEndEffector
  //--------------------------------------------------------------------------------------------------------------------
  log_file << "test: getEndEffector" << std::endl;
  if (
       // transformation to joint 1 frame
       !IsZero(reference_geometry.getEndEffector()->position[0] - 0)
       || !IsZero(reference_geometry.getEndEffector()->position[1] - 2)
       || !IsZero(reference_geometry.getEndEffector()->position[2] - 8)
       || !IsZero(reference_geometry.getEndEffector()->orientation[0] - 1)
       || !IsZero(reference_geometry.getEndEffector()->orientation[1] - 0)
       || !IsZero(reference_geometry.getEndEffector()->orientation[2] - 0)
       || !IsZero(reference_geometry.getEndEffector()->orientation[3] - 0)
       || !IsZero(reference_geometry.getEndEffector()->orientation[4] - 1)
       || !IsZero(reference_geometry.getEndEffector()->orientation[5] - 0)
       || !IsZero(reference_geometry.getEndEffector()->orientation[6] - 0)
       || !IsZero(reference_geometry.getEndEffector()->orientation[7] - 0)
       || !IsZero(reference_geometry.getEndEffector()->orientation[8] - 1)
     )
  {
    errors++;
    log_file << "ERROR with getEndEffector()" << std::endl;
  }

  //--------------------------------------------------------------------------------------------------------------------
  // getLinkSegment
  //--------------------------------------------------------------------------------------------------------------------
  log_file << "test: getJoint" << std::endl;
  if (
       // transformation to joint 1 frame
       !IsZero(reference_geometry.getLinkSegment(0)->centerOfMassPosition[0] - 0)
       || !IsZero(reference_geometry.getLinkSegment(0)->centerOfMassPosition[1] - 2)
       || !IsZero(reference_geometry.getLinkSegment(0)->centerOfMassPosition[2] - 1)
       || !IsZero(reference_geometry.getLinkSegment(0)->orientation[0] - 1)
       || !IsZero(reference_geometry.getLinkSegment(0)->orientation[1] - 0)
       || !IsZero(reference_geometry.getLinkSegment(0)->orientation[2] - 0)
       || !IsZero(reference_geometry.getLinkSegment(0)->orientation[3] - 0)
       || !IsZero(reference_geometry.getLinkSegment(0)->orientation[4] - 1)
       || !IsZero(reference_geometry.getLinkSegment(0)->orientation[5] - 0)
       || !IsZero(reference_geometry.getLinkSegment(0)->orientation[6] - 0)
       || !IsZero(reference_geometry.getLinkSegment(0)->orientation[7] - 0)
       || !IsZero(reference_geometry.getLinkSegment(0)->orientation[8] - 1)
       || !IsZero(reference_geometry.getLinkSegment(0)->inertiaMoments[0] - 15.0)
       || !IsZero(reference_geometry.getLinkSegment(0)->inertiaMoments[1] - 15.0)
       || !IsZero(reference_geometry.getLinkSegment(0)->inertiaMoments[2] - 1.0)
       || !IsZero(reference_geometry.getLinkSegment(1)->centerOfMassPosition[0] - 0)
       || !IsZero(reference_geometry.getLinkSegment(1)->centerOfMassPosition[1] - 2)
       || !IsZero(reference_geometry.getLinkSegment(1)->centerOfMassPosition[2] - 3)
       || !IsZero(reference_geometry.getLinkSegment(2)->centerOfMassPosition[0] - 0)
       || !IsZero(reference_geometry.getLinkSegment(2)->centerOfMassPosition[1] - 2)
       || !IsZero(reference_geometry.getLinkSegment(2)->centerOfMassPosition[2] - 5)
       || !IsZero(reference_geometry.getLinkSegment(3)->centerOfMassPosition[0] - 0)
       || !IsZero(reference_geometry.getLinkSegment(3)->centerOfMassPosition[1] - 2)
       || !IsZero(reference_geometry.getLinkSegment(3)->centerOfMassPosition[2] - 7)
     )
  {
    errors++;
    log_file << "ERROR with getJoint()" << std::endl;
  }


  return errors;
}
