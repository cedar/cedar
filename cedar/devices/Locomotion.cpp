/*======================================================================================================================

    Copyright 2011, 2012, 2013, 2014, 2015, 2016, 2017 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany

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

    File:        Locomotion.cpp

    Maintainer:  Stephan Zibner
    Email:       stephan.zibner@ini.ruhr-uni-bochum.de
    Date:        2011 03 19

    Description: An object of this class represents the locomotion of a mobile robot.

    Credits:

======================================================================================================================*/

// CEDAR INCLUDES
#include "cedar/auxiliaries/math/VelocityLimitsParameter.h"
#include "cedar/auxiliaries/math/AngularVelocityLimitsParameter.h"
#include "cedar/auxiliaries/math/constants.h"
#include "cedar/auxiliaries/UnitDataTemplate.h"
#include "cedar/devices/Locomotion.h"
#include "cedar/units/Velocity.h"
#include "cedar/units/AngularVelocity.h"

// SYSTEM INCLUDES


//------------------------------------------------------------------------------
// static variables
//------------------------------------------------------------------------------

const cedar::dev::Component::ComponentDataType cedar::dev::Locomotion::FORWARD_VELOCITY = 1;
const cedar::dev::Component::ComponentDataType cedar::dev::Locomotion::TURNING_RATE = 2;
//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

void cedar::dev::Locomotion::init()
{
  installCommandAndMeasurementType( cedar::dev::Locomotion::FORWARD_VELOCITY, "Forward Velocity" );
  installCommandAndMeasurementType( cedar::dev::Locomotion::TURNING_RATE, "Turning Rate" );

  setCommandAndMeasurementDimensionality( cedar::dev::Locomotion::FORWARD_VELOCITY, 1 );
  setCommandAndMeasurementDimensionality( cedar::dev::Locomotion::TURNING_RATE, 1 );

}

cedar::dev::Locomotion::Locomotion()
:
_mForwardVelocityLimits
(
  new cedar::aux::math::VelocityLimitsParameter
      (
        this,
        "forward velocity limits",
        -4.0 * cedar::unit::meters_per_second,
        -5.0 * cedar::unit::meters_per_second,
        0.0 * cedar::unit::meters_per_second,
        4.0 * cedar::unit::meters_per_second,
        0.0 * cedar::unit::meters_per_second,
        5.0 * cedar::unit::meters_per_second
      )
),
_mTurningRateLimits
(
  new cedar::aux::math::AngularVelocityLimitsParameter
      (
        this,
        "turning rate limits",
        -5.0 * cedar::unit::radian_per_second,
        -20.0 * cedar::unit::radian_per_second,
        0.0 * cedar::unit::radian_per_second,
        5.0 * cedar::unit::radian_per_second,
        0.0 * cedar::unit::radian_per_second,
        20.0 * cedar::unit::radian_per_second
      )
)
{
  init();
}

// constructor taking an externally created channel
cedar::dev::Locomotion::Locomotion(cedar::dev::ChannelPtr channel)
:
cedar::dev::Component(channel),
_mForwardVelocityLimits
(
  new cedar::aux::math::VelocityLimitsParameter
      (
        this,
        "forward velocity limits",
        -4.0 * cedar::unit::meters_per_second,
        -5.0 * cedar::unit::meters_per_second,
        0.0 * cedar::unit::meters_per_second,
        4.0 * cedar::unit::meters_per_second,
        0.0 * cedar::unit::meters_per_second,
        5.0 * cedar::unit::meters_per_second
      )
),
_mTurningRateLimits
(
  new cedar::aux::math::AngularVelocityLimitsParameter
      (
        this,
        "turning rate limits",
        -5.0 * cedar::unit::radian_per_second,
        -20.0 * cedar::unit::radian_per_second,
        0.0 * cedar::unit::radian_per_second,
        5.0 * cedar::unit::radian_per_second,
        0.0 * cedar::unit::radian_per_second,
        20.0 * cedar::unit::radian_per_second
      )
)
{
  init();
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------
void cedar::dev::Locomotion::setForwardVelocity(cedar::unit::Velocity forwardVelocity)
{
  setForwardVelocityAndTurningRate(forwardVelocity, getTurningRate());
}

void cedar::dev::Locomotion::setTurningRate(cedar::unit::AngularVelocity turningRate)
{
  setForwardVelocityAndTurningRate(getForwardVelocity(), turningRate);
}

void cedar::dev::Locomotion::setForwardVelocityAndTurningRate
     (
       cedar::unit::Velocity forwardVelocity,
       cedar::unit::AngularVelocity turningRate
     )
{
  cv::Mat forward_matrix = cv::Mat(1,1,CV_32F);
  cv::Mat turning_matrix = cv::Mat(1,1,CV_32F);

  //@todo: use a generic limit mechanism
  thresholdForwardVelocity(forwardVelocity);
  thresholdTurningRate(turningRate);

  forward_matrix.at<float>(0,0) = forwardVelocity / cedar::unit::DEFAULT_VELOCITY_UNIT;
  turning_matrix.at<float>(0,0) = turningRate / cedar::unit::DEFAULT_ANGULAR_VELOCITY_UNIT;

  setUserSideCommandBuffer( cedar::dev::Locomotion::FORWARD_VELOCITY, forward_matrix);
  setUserSideCommandBuffer( cedar::dev::Locomotion::TURNING_RATE, turning_matrix);

/*
  cedar::unit::Velocity left_wheel_speed = 0.0 * cedar::unit::DEFAULT_VELOCITY_UNIT;
  cedar::unit::Velocity right_wheel_speed = 0.0 * cedar::unit::DEFAULT_VELOCITY_UNIT;
  convertToWheelSpeed(forwardVelocity, turningRate, left_wheel_speed, right_wheel_speed);

  thresholdToHardwareLimits(left_wheel_speed, right_wheel_speed);

  mWheelSpeed[0] = left_wheel_speed;
  mWheelSpeed[1] = right_wheel_speed;

  cedar::unit::AngularVelocity turning_rate;
  cedar::unit::Velocity forward_velocity;
  convertToForwardVelocityAndTurningRate(left_wheel_speed, right_wheel_speed, forward_velocity, turning_rate);
  this->mTurningRate->setData(turning_rate);
  this->mForwardVelocity->setData(forward_velocity);

  sendMovementCommand();
*/
}

cedar::unit::Velocity cedar::dev::Locomotion::getForwardVelocity() const
{
  cv::Mat ret = getUserSideMeasurementBuffer( cedar::dev::Locomotion::FORWARD_VELOCITY );

  return double(ret.at<float>(0,0)) * cedar::unit::DEFAULT_VELOCITY_UNIT;
}

cedar::unit::AngularVelocity cedar::dev::Locomotion::getTurningRate() const
{
  cv::Mat ret = getUserSideMeasurementBuffer( cedar::dev::Locomotion::TURNING_RATE );

  return double(ret.at<float>(0,0)) * cedar::unit::DEFAULT_ANGULAR_VELOCITY_UNIT;
}

void cedar::dev::Locomotion::thresholdForwardVelocity(cedar::unit::Velocity& forwardVelocity) const
{
  forwardVelocity = _mForwardVelocityLimits->getValue().limit(forwardVelocity);
}

void cedar::dev::Locomotion::thresholdTurningRate(cedar::unit::AngularVelocity& turningRate) const
{
  turningRate = _mTurningRateLimits->getValue().limit(turningRate);
}

void cedar::dev::Locomotion::stop()
{
  setForwardVelocityAndTurningRate
  (
    0.0 * cedar::unit::DEFAULT_VELOCITY_UNIT,
    0.0 * cedar::unit::DEFAULT_ANGULAR_VELOCITY_UNIT
  );
}

