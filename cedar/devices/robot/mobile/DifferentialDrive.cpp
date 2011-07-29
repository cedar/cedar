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

    File:        DifferentialDrive.cpp

    Maintainer:  Andre Bartel
    Email:       andre.bartel@ini.ruhr-uni-bochum.de
    Date:        2011 03 19

    Description: An object of this class represents the differential drive of a robot.

    Credits:

======================================================================================================================*/

// LOCAL INCLUDES

#include "DifferentialDrive.h"

// PROJECT INCLUDES

// SYSTEM INCLUDES

using namespace cedar::dev::robot::mobile;

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

DifferentialDrive::DifferentialDrive()
{

}

DifferentialDrive::~DifferentialDrive()
{

}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

double DifferentialDrive::getWheelDistance() const
{
  return _mWheelDistance;
}

double DifferentialDrive::getWheelRadius() const
{
  return _mWheelRadius;
}

const std::vector<double>& DifferentialDrive::getWheelSpeed() const
{
  return mWheelSpeed;
}

std::vector<double> DifferentialDrive::calculateVelocity(double leftWheelSpeed, double rightWheelSpeed)
{
    //construct the vector to return
    std::vector<double> velocity;
    velocity.resize(2);

    //the calculation
    velocity[0] = (rightWheelSpeed + leftWheelSpeed) / 2;
    velocity[1] = (rightWheelSpeed - leftWheelSpeed) / _mWheelDistance;

    return velocity;
}

std::vector<double> DifferentialDrive::calculateWheelSpeed(double forwardVelocity, double turningRate)
{
  //construct the vector to return
  std::vector<double> wheel_speed;
  wheel_speed.resize(2);

  //the calculation
  wheel_speed[0] = forwardVelocity - turningRate * _mWheelDistance / 2;
  wheel_speed[1] = forwardVelocity + turningRate * _mWheelDistance / 2;

  return wheel_speed;
}

int DifferentialDrive::setVelocity(double forwardVelocity, double turningRate)
{
  std::vector<double> wheel_speed;
  wheel_speed.resize(2);

  //calculate and set the wheel speed
  wheel_speed = calculateWheelSpeed(forwardVelocity, turningRate);
  int s = setWheelSpeed(wheel_speed[0], wheel_speed[1]);
  if (s == 0 && _mDebug) // setting wheel speed failed
  {
    std::cout << "DifferentialDrive: Error Setting Velocity\n";
  }

  return s;
}

int DifferentialDrive::setForwardVelocity(double forwardVelocity)
{
  std::vector<double> wheel_speed;
  wheel_speed.resize(2);

  //calculate and set the wheel speed
  wheel_speed = calculateWheelSpeed(forwardVelocity, mVelocity[1]);
  int s = setWheelSpeed(wheel_speed[0], wheel_speed[1]);
  if (s == 0 && _mDebug) //setting wheel speed failed
  {
    std::cout << "DifferentialDrive: Error Setting Forward Velocity\n";
  }

  return s;
}

int DifferentialDrive::setTurningRate(double turningRate)
{
  std::vector<double> wheel_speed;
  wheel_speed.resize(2);

  //calculate and set the wheel speed
  wheel_speed = calculateWheelSpeed(mVelocity[0], turningRate);
  int s = setWheelSpeed(wheel_speed[0], wheel_speed[1]);
  if (s == 0 && _mDebug)  //setting wheel speed failed
  {
    std::cout << "DifferentialDrive: Error Setting Turning Rate\n";
  }

  return s;
}

double DifferentialDrive::getMaximalWheelSpeed() const
{
  return mMaximalWheelSpeed;
}
