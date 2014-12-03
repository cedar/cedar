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
 
 ----- File:        KinematicChain.cpp
 
 ----- Author:      Hendrik Reimann
 ----- Email:       hendrik.reimann@ini.rub.de
 ----- Date:        2010 11 06
 
 ----- Description: implementation for the \em cedar::dev::SimulatedKinematicChain class
 
 ----- Credits:
 ---------------------------------------------------------------------------------------------------------------------*/

// CEDAR INCLUDES
#include "cedar/devices/SimulatedKinematicChain.h"
#include "cedar/devices/KinematicChain.h"
#include "cedar/auxiliaries/math/tools.h"

// SYSTEM INCLUDES
#include <QWriteLocker>

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::dev::SimulatedKinematicChain::SimulatedKinematicChain()
{
  registerCommandHook(cedar::dev::KinematicChain::JOINT_ANGLES, boost::bind(&cedar::dev::SimulatedKinematicChain::sendSimulatedAngles, this, _1));
  registerMeasurementHook(cedar::dev::KinematicChain::JOINT_ANGLES, boost::bind(&cedar::dev::SimulatedKinematicChain::retrieveSimulatedAngles, this));

  this->applyDeviceCommandsAs(cedar::dev::KinematicChain::JOINT_ANGLES);

  connect(this->mpJoints.get(), SIGNAL(valueChanged()), this, SLOT(updateInitialConfiguration()));
}

cedar::dev::SimulatedKinematicChain::~SimulatedKinematicChain()
{
  prepareComponentDestructAbsolutelyRequired();
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------
void cedar::dev::SimulatedKinematicChain::sendSimulatedAngles(cv::Mat mat)
{
  QWriteLocker lock(&mSimulationLock);

//  std::cout << "  write pos " << mat << std::endl;
  mSimulation[cedar::dev::KinematicChain::JOINT_ANGLES] = mat.clone();
}

cv::Mat cedar::dev::SimulatedKinematicChain::retrieveSimulatedAngles()
{
  QReadLocker lock(&mSimulationLock);

//  std::cout << "  in retrieveSimulatedAngles: " << mSimulation[ cedar::dev::KinematicChain::JOINT_ANGLES ] << std::endl;
  return mSimulation[ cedar::dev::KinematicChain::JOINT_ANGLES ].clone();
}

bool cedar::dev::SimulatedKinematicChain::isMovable() const
{
  return true;
}

void cedar::dev::SimulatedKinematicChain::updateInitialConfiguration()
{
  auto number_of_joints = this->getNumberOfJoints();

  if (number_of_joints > 0)
  {
    if (this->hasInitialConfiguration("default"))
    {
      this->deleteInitialConfiguration("default");
    }
    this->addInitialConfiguration("default", cv::Mat::zeros(number_of_joints, 1, CV_64F) + 0.001);
    this->applyInitialConfiguration("default");
    mSimulation[cedar::dev::KinematicChain::JOINT_ANGLES] = cv::Mat::zeros(number_of_joints, 1, CV_64F);
    mSimulation[cedar::dev::KinematicChain::JOINT_VELOCITIES] = cv::Mat::zeros(number_of_joints, 1, CV_64F);
    mSimulation[cedar::dev::KinematicChain::JOINT_ACCELERATIONS] = cv::Mat::zeros(number_of_joints, 1, CV_64F);
  }
}

bool cedar::dev::SimulatedKinematicChain::applyCrashbrake()
{
  QWriteLocker lock(&mSimulationLock);

  mSimulation[ cedar::dev::KinematicChain::JOINT_VELOCITIES ] = 
    cv::Mat::zeros( getNumberOfJoints(), 1, CV_64F );
  mSimulation[ cedar::dev::KinematicChain::JOINT_ACCELERATIONS ] = 
    cv::Mat::zeros( getNumberOfJoints(), 1, CV_64F );

  return true;
}

