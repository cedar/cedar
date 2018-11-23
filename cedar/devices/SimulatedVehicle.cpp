/*======================================================================================================================

    Copyright 2011, 2012, 2013, 2014, 2015 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany
 
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

    File:        SimulatedVehicle.cpp

    Maintainer:  Jan Tekülve
    Email:       jan.tekuelve@ini.rub.de
    Date:        2016 07 21

    Description: Source file for the class cedar::dev::SimulatedVehicle.

    Credits:

======================================================================================================================*/

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CLASS HEADER
#include "cedar/devices/SimulatedVehicle.h"

// CEDAR INCLUDES
#include "cedar/devices/Vehicle.h"
#include "cedar/auxiliaries/math/tools.h"

// SYSTEM INCLUDES
#include <QWriteLocker>

//----------------------------------------------------------------------------------------------------------------------
// type registration
//----------------------------------------------------------------------------------------------------------------------
namespace
{
  bool registered()
  {
    cedar::dev::ComponentManagerSingleton::getInstance()->registerType<cedar::dev::SimulatedVehiclePtr>();
    return true;
  }

  bool registerFnCall = registered();
}

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::dev::SimulatedVehicle::SimulatedVehicle()
{
  registerCommandHook(cedar::dev::Vehicle::WHEEL_VELOCITIES, boost::bind(&cedar::dev::SimulatedVehicle::sendSimulatedVelocities, this, _1));
  registerMeasurementHook(cedar::dev::Vehicle::WHEEL_VELOCITIES, boost::bind(&cedar::dev::SimulatedVehicle::retrieveSimulatedVelocities, this));

  this->applyDeviceSideCommandsAs(cedar::dev::Vehicle::WHEEL_VELOCITIES);

//  connect(this->mWheels.get(), SIGNAL(valueChanged()), this, SLOT(updateInitialConfiguration()));
}

cedar::dev::SimulatedVehicle::~SimulatedVehicle()
{
  prepareComponentDestructAbsolutelyRequired();
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------
void cedar::dev::SimulatedVehicle::sendSimulatedVelocities(cv::Mat mat)
{
  QWriteLocker lock(&mSimulationLock);

//  std::cout << "  write pos " << mat << std::endl;
  mSimulation[cedar::dev::Vehicle::WHEEL_VELOCITIES] = mat.clone();
}

cv::Mat cedar::dev::SimulatedVehicle::retrieveSimulatedVelocities()
{
  QReadLocker lock(&mSimulationLock);

//  std::cout << "  in retrieveSimulatedAngles: " << mSimulation[ cedar::dev::KinematicChain::JOINT_ANGLES ] << std::endl;
  return mSimulation[ cedar::dev::Vehicle::WHEEL_VELOCITIES ].clone();
}

//void cedar::dev::SimulatedVehicle::updateInitialConfiguration()
//{
//  auto number_of_wheels = this->getNumberOfWheels();
//
//  if (number_of_wheels > 0)
//  {
//    if (this->hasInitialConfiguration("default"))
//    {
//      this->deleteInitialConfiguration("default");
//    }
//    this->addInitialConfiguration("default", cv::Mat::zeros(number_of_wheels, 1, CV_32F));
//    this->applyInitialConfiguration("default");
//    mSimulation[cedar::dev::Vehicle::WHEEL_VELOCITIES] = cv::Mat::zeros(number_of_wheels, 1, CV_32F);
//    mSimulation[cedar::dev::Vehicle::WHEEL_ACCELERATIONS] = cv::Mat::zeros(number_of_wheels, 1, CV_32F);
//  }
//}

bool cedar::dev::SimulatedVehicle::applyCrashbrake()
{
  QWriteLocker lock(&mSimulationLock);

  mSimulation[ cedar::dev::Vehicle::WHEEL_VELOCITIES ] =
    cv::Mat::zeros( this->getNumberOfWheels(), 1, CV_32F );
  mSimulation[ cedar::dev::Vehicle::WHEEL_ACCELERATIONS] =
    cv::Mat::zeros( this->getNumberOfWheels(), 1, CV_32F );

  return true;
}