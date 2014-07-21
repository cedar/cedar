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
  //!@todo add these functions
//  registerIOCommandHook( cedar::dev::KinematicChain::JOINT_ANGLES, boost::bind(&cedar::dev::SimulatedKinematicChain::sendSimulatedAngles, this, _1 ) );
//  registerIOCommandHook( cedar::dev::KinematicChain::JOINT_VELOCITIES, boost::bind(&cedar::dev::SimulatedKinematicChain::sendSimulatedVelocities, this, _1 ) );
//  registerIOCommandHook( cedar::dev::KinematicChain::JOINT_ACCELERATIONS, boost::bind(&cedar::dev::SimulatedKinematicChain::sendSimulatedAccelerations, this, _1 ) );
  //!@todo add these functions
//  registerIOMeasurementHook( cedar::dev::KinematicChain::JOINT_ANGLES, boost::bind(&cedar::dev::SimulatedKinematicChain::retrieveSimulatedAngles, this ) );
//  registerIOMeasurementHook( cedar::dev::KinematicChain::JOINT_VELOCITIES, boost::bind(&cedar::dev::SimulatedKinematicChain::retrieveSimulatedVelocities, this ) );
//  registerIOMeasurementHook( cedar::dev::KinematicChain::JOINT_ACCELERATIONS, boost::bind(&cedar::dev::SimulatedKinematicChain::retrieveSimulatedAccelerations, this ) );
}

cedar::dev::SimulatedKinematicChain::~SimulatedKinematicChain()
{
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------
void cedar::dev::SimulatedKinematicChain::sendSimulatedAngles(cv::Mat mat)
{
  QWriteLocker lock(&mSimulationLock);

std::cout << "\n\n\n\n  write angles \n\n\n" << mat << std::endl;
  mSimulation[ cedar::dev::KinematicChain::JOINT_ANGLES ] = mat.clone();
}

void cedar::dev::SimulatedKinematicChain::sendSimulatedVelocities(cv::Mat mat)
{
  QWriteLocker lock(&mSimulationLock);

std::cout << "  write vels " << mat << std::endl;
  mSimulation[ cedar::dev::KinematicChain::JOINT_VELOCITIES ] = mat.clone();
}

void cedar::dev::SimulatedKinematicChain::sendSimulatedAccelerations(cv::Mat mat)
{
  QWriteLocker lock(&mSimulationLock);

std::cout << "  write accels " << mat << std::endl;
  mSimulation[ cedar::dev::KinematicChain::JOINT_ACCELERATIONS ] = mat.clone();
}

cv::Mat cedar::dev::SimulatedKinematicChain::retrieveSimulatedAngles()
{

     // HIER HIER HIER HIER
     // mSimulation ist leer !!!!
     // HIER HIER HIER HIER

std::cout << "  in retrieveSimulatedAngles: " << mSimulation[ cedar::dev::KinematicChain::JOINT_ANGLES ] << std::endl;
  return mSimulation[ cedar::dev::KinematicChain::JOINT_ANGLES ].clone();
}

cv::Mat cedar::dev::SimulatedKinematicChain::retrieveSimulatedVelocities()
{
  // todo: lock
  return mSimulation[ cedar::dev::KinematicChain::JOINT_VELOCITIES ].clone();
}

cv::Mat cedar::dev::SimulatedKinematicChain::retrieveSimulatedAccelerations()
{
  return mSimulation[ cedar::dev::KinematicChain::JOINT_ACCELERATIONS].clone();
}


bool cedar::dev::SimulatedKinematicChain::isMovable() const
{
  return true;
}
