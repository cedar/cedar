/*======================================================================================================================

    Copyright 2011, 2012 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany

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

    Maintainer:  Mathis Richter
    Email:       mathis.richter@ini.rub.de
    Date:        2012 04 12

    Description: An object of this class represents the differential drive of a robot.

    Credits:     Original design by Andre Bartel (2011)

======================================================================================================================*/

// CEDAR INCLUDES
#include "cedar/auxiliaries/DoubleParameter.h"
#include "cedar/auxiliaries/math/DoubleLimitsParameter.h"
#include "cedar/auxiliaries/assert.h"
#include "cedar/devices/robot/DifferentialDrive.h"

// SYSTEM INCLUDES

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------
cedar::dev::robot::DifferentialDrive::DifferentialDrive()
:
mWheelSpeed(2, 0),
_mWheelDistance(new cedar::aux::DoubleParameter(this, "wheel distance", 0.1, 0.0, 1.0)),
_mWheelRadius(new cedar::aux::DoubleParameter(this, "wheel radius", 0.01, 0.0, 1.0)),
_mHardwareSpeedLimits(new cedar::aux::math::DoubleLimitsParameter(this, "hardware speed limits", 0.0, 0.2, 0.2, 2.0))
{}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

double cedar::dev::robot::DifferentialDrive::getWheelDistance() const
{
  return _mWheelDistance->getValue();
}

double cedar::dev::robot::DifferentialDrive::getWheelRadius() const
{
  return _mWheelRadius->getValue();
}

cedar::aux::math::DoubleLimitsParameterPtr cedar::dev::robot::DifferentialDrive::getHardwareSpeedLimits() const
{
  return _mHardwareSpeedLimits;
}

const std::vector<double>& cedar::dev::robot::DifferentialDrive::getWheelSpeed() const
{
  return mWheelSpeed;
}

void cedar::dev::robot::DifferentialDrive::setWheelSpeed(std::vector<double>& wheelSpeed)
{
  double forward_velocity = 0;
  double turning_rate = 0;

  convertToForwardVelocityAndTurningRate(wheelSpeed[0], wheelSpeed[1], forward_velocity, turning_rate);
  setForwardVelocityAndTurningRate(forward_velocity, turning_rate);
}

void cedar::dev::robot::DifferentialDrive::setForwardVelocity(double forwardVelocity)
{
  setForwardVelocityAndTurningRate(forwardVelocity, getTurningRate());
}

void cedar::dev::robot::DifferentialDrive::setTurningRate(double turningRate)
{
  setForwardVelocityAndTurningRate(getForwardVelocity(), turningRate);
}

void cedar::dev::robot::DifferentialDrive::setForwardVelocityAndTurningRate
     (
       double forwardVelocity,
       double turningRate
     )
{
  thresholdForwardVelocity(forwardVelocity);
  thresholdTurningRate(turningRate);

  double left_wheel_speed = 0;
  double right_wheel_speed = 0;
  convertToWheelSpeed(forwardVelocity, turningRate, left_wheel_speed, right_wheel_speed);
  thresholdToHardwareLimits(left_wheel_speed, right_wheel_speed);
  mWheelSpeed[0] = left_wheel_speed;
  mWheelSpeed[1] = right_wheel_speed;

  convertToForwardVelocityAndTurningRate(left_wheel_speed, right_wheel_speed, mForwardVelocity, mTurningRate);
  sendMovementCommand();
}

void cedar::dev::robot::DifferentialDrive::thresholdToHardwareLimits
     (
       double& leftWheelSpeed,
       double& rightWheelSpeed
     )
{
  leftWheelSpeed = _mHardwareSpeedLimits->getValue().limit(leftWheelSpeed);
  rightWheelSpeed = _mHardwareSpeedLimits->getValue().limit(rightWheelSpeed);
}

void cedar::dev::robot::DifferentialDrive::convertToWheelSpeed
     (
       double forwardVelocity,
       double turningRate,
       double& leftWheelSpeed,
       double& rightWheelSpeed
     ) const
{
  leftWheelSpeed = forwardVelocity - turningRate * _mWheelDistance->getValue() / 2.0;
  rightWheelSpeed = forwardVelocity + turningRate * _mWheelDistance->getValue() / 2.0;
}

void cedar::dev::robot::DifferentialDrive::convertToForwardVelocityAndTurningRate
     (
       double leftWheelSpeed,
       double rightWheelSpeed,
       double& forwardVelocity,
       double& turningRate
     ) const
{
  forwardVelocity = (rightWheelSpeed + leftWheelSpeed) / 2.0;
  turningRate = (rightWheelSpeed - leftWheelSpeed) / _mWheelDistance->getValue();
}
