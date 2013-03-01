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
#include "cedar/devices/Locomotion.h"
#include "cedar/units/Velocity.h"
#include "cedar/units/AngularVelocity.h"

// SYSTEM INCLUDES

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

// constructor
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
{}

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
{}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

cedar::unit::Velocity cedar::dev::Locomotion::getForwardVelocity() const
{
  return mForwardVelocity;
}

cedar::unit::AngularVelocity cedar::dev::Locomotion::getTurningRate() const
{
  return mTurningRate;
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
