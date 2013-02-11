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
#include "cedar/devices/namespace.h"
#include "cedar/devices/DifferentialDrive.h"

// SYSTEM INCLUDES
#include <boost/units/quantity.hpp>
#include <boost/units/systems/si/velocity.hpp>
#include <boost/units/systems/si/length.hpp>
#include <boost/units/systems/si/plane_angle.hpp>
#include <boost/units/systems/si/angular_velocity.hpp>

// units namespaces
using namespace boost::units;
using namespace boost::units::si;

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------
cedar::dev::DifferentialDrive::DifferentialDrive()
:
mWheelSpeed(2, 0 * meters_per_second),
_mWheelDistance(new cedar::aux::DoubleParameter(this, "wheel distance", 0.1, 0.0, 1.0)),
_mWheelRadius(new cedar::aux::DoubleParameter(this, "wheel radius", 0.01, 0.0, 1.0)),
_mHardwareSpeedLimits(new cedar::aux::math::DoubleLimitsParameter(this, "hardware speed limits", 0.0, 0.2, 0.2, 2.0))
{}

cedar::dev::DifferentialDrive::DifferentialDrive(cedar::dev::ChannelPtr channel)
:
cedar::dev::Locomotion(channel),
mWheelSpeed(2, 0 * meters_per_second),
_mWheelDistance(new cedar::aux::DoubleParameter(this, "wheel distance", 0.1, 0.0, 1.0)),
_mWheelRadius(new cedar::aux::DoubleParameter(this, "wheel radius", 0.01, 0.0, 1.0)),
_mHardwareSpeedLimits(new cedar::aux::math::DoubleLimitsParameter(this, "hardware speed limits", 0.0, 0.2, 0.2, 2.0))
{}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

quantity<length> cedar::dev::DifferentialDrive::getWheelDistance() const
{
  //!todo fix this by introducing a LengthParameter class
  quantity<length> wheel_distance = _mWheelDistance->getValue() * meters;
  return wheel_distance;
}

quantity<length> cedar::dev::DifferentialDrive::getWheelRadius() const
{
  //!todo fix this by introducing a LengthParameter class
  quantity<length> wheel_radius = _mWheelRadius->getValue() * meters;
  return wheel_radius;
}

cedar::aux::math::DoubleLimitsParameterPtr cedar::dev::DifferentialDrive::getHardwareSpeedLimits() const
{
  return _mHardwareSpeedLimits;
}

const std::vector<quantity<velocity> >& cedar::dev::DifferentialDrive::getWheelSpeed() const
{
  return mWheelSpeed;
}

void cedar::dev::DifferentialDrive::setWheelSpeed(std::vector<quantity<velocity> >& wheelSpeed)
{
  quantity<velocity> forward_velocity = 0 * meters_per_second;
  quantity<angular_velocity> turning_rate = 0 * radians_per_second;

  convertToForwardVelocityAndTurningRate(wheelSpeed[0], wheelSpeed[1], forward_velocity, turning_rate);
  setForwardVelocityAndTurningRate(forward_velocity, turning_rate);
}

void cedar::dev::DifferentialDrive::setForwardVelocity(quantity<velocity> forwardVelocity)
{
  setForwardVelocityAndTurningRate(forwardVelocity, getTurningRate());
}

void cedar::dev::DifferentialDrive::setTurningRate(quantity<angular_velocity> turningRate)
{
  setForwardVelocityAndTurningRate(getForwardVelocity(), turningRate);
}

void cedar::dev::DifferentialDrive::setForwardVelocityAndTurningRate
     (
       quantity<velocity> forwardVelocity,
       quantity<angular_velocity> turningRate
     )
{
  thresholdForwardVelocity(forwardVelocity);
  thresholdTurningRate(turningRate);

  quantity<velocity> left_wheel_speed = 0 * meters_per_second;
  quantity<velocity> right_wheel_speed = 0 * meters_per_second;
  convertToWheelSpeed(forwardVelocity, turningRate, left_wheel_speed, right_wheel_speed);
  thresholdToHardwareLimits(left_wheel_speed, right_wheel_speed);
  mWheelSpeed[0] = left_wheel_speed;
  mWheelSpeed[1] = right_wheel_speed;

  convertToForwardVelocityAndTurningRate(left_wheel_speed, right_wheel_speed, mForwardVelocity, mTurningRate);
  sendMovementCommand();
}

void cedar::dev::DifferentialDrive::thresholdToHardwareLimits
     (
       quantity<velocity>& leftWheelSpeed,
       quantity<velocity>& rightWheelSpeed
     )
{
  //!todo fix this by introducing a VelocityLimitsParameter class
  double left_wheel_speed = leftWheelSpeed / meters_per_second;
  double right_wheel_speed = rightWheelSpeed / meters_per_second;
  leftWheelSpeed = _mHardwareSpeedLimits->getValue().limit(left_wheel_speed) * meters_per_second;
  rightWheelSpeed = _mHardwareSpeedLimits->getValue().limit(right_wheel_speed) * meters_per_second;
}

void cedar::dev::DifferentialDrive::convertToWheelSpeed
     (
       quantity<velocity> forwardVelocity,
       quantity<angular_velocity> turningRate,
       quantity<velocity>& leftWheelSpeed,
       quantity<velocity>& rightWheelSpeed
     ) const
{
  quantity<length> wheel_distance = _mWheelDistance->getValue() * meters;

  leftWheelSpeed = forwardVelocity - turningRate * wheel_distance / (2.0 * radians);
  rightWheelSpeed = forwardVelocity + turningRate * wheel_distance / (2.0 * radians);
}

void cedar::dev::DifferentialDrive::convertToForwardVelocityAndTurningRate
     (
       quantity<velocity> leftWheelSpeed,
       quantity<velocity> rightWheelSpeed,
       quantity<velocity>& forwardVelocity,
       quantity<angular_velocity>& turningRate
     ) const
{
  quantity<length> wheel_distance = _mWheelDistance->getValue() * meters;

  forwardVelocity = (rightWheelSpeed + leftWheelSpeed) / 2.0;
  turningRate = (rightWheelSpeed - leftWheelSpeed) / (wheel_distance / radians);
}
