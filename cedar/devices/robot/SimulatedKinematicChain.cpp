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
 
 ----- File:        KinematicChain.cpp
 
 ----- Author:      Hendrik Reimann
 ----- Email:       hendrik.reimann@ini.rub.de
 ----- Date:        2010 11 06
 
 ----- Description: implementation for the \em cedar::dev::robot::SimulatedKinematicChain class
 
 ----- Credits:
 ---------------------------------------------------------------------------------------------------------------------*/

// CEDAR INCLUDES
#include "cedar/devices/robot/SimulatedKinematicChain.h"
#include "cedar/auxiliaries/math/tools.h"

// SYSTEM INCLUDES

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::dev::robot::SimulatedKinematicChain::SimulatedKinematicChain
(
  const cedar::dev::robot::ReferenceGeometryPtr& rpReferenceGeometry
)
:
KinematicChain(rpReferenceGeometry)
{
  init();
}

cedar::dev::robot::SimulatedKinematicChain::SimulatedKinematicChain(const std::string& configFileName)
:
KinematicChain(configFileName)
{
  init();
}

cedar::dev::robot::SimulatedKinematicChain::~SimulatedKinematicChain()
{
  
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

double cedar::dev::robot::SimulatedKinematicChain::getJointAngle(unsigned int index)
{
  return mJointAngles.at<double>(index, 0);
}

void cedar::dev::robot::SimulatedKinematicChain::setJointAngle(unsigned int index, double angle)
{
  mJointAngles.at<double>(index, 0) = angle;
}

void cedar::dev::robot::SimulatedKinematicChain::init()
{
  mJointAngles = cv::Mat::zeros(getNumberOfJoints(), 1, CV_64FC1);
}
