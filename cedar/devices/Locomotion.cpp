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
#include "cedar/auxiliaries/math/DoubleLimitsParameter.h"
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
  new cedar::aux::math::DoubleLimitsParameter
      (
        this,
        "forward velocity limits",
        -2.0,
        2.0,
        -4.0,
        4.0,
        -4.0,
        4.0
      )
),
_mTurningRateLimits
(
  new cedar::aux::math::DoubleLimitsParameter
      (
        this,
        "turning rate limits",
        -2.0,
        2.0,
        -20.0,
        20.0,
        -20.0,
        20.0
      )
)
{}

// constructor taking an externally created channel
cedar::dev::Locomotion::Locomotion(cedar::dev::ChannelPtr channel)
:
cedar::dev::Component(channel),
_mForwardVelocityLimits
(
  new cedar::aux::math::DoubleLimitsParameter
      (
        this,
        "forward velocity limits",
        -2.0,
        2.0,
        -4.0,
        4.0,
        -4.0,
        4.0
      )
),
_mTurningRateLimits
(
  new cedar::aux::math::DoubleLimitsParameter
      (
        this,
        "turning rate limits",
        -2.0,
        2.0,
        -20.0,
        20.0,
        -20.0,
        20.0
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
  //!todo fix this by writing a VelocityLimitsPatameter
  double forward_velocity = forwardVelocity / cedar::unit::DEFAULT_VELOCITY_UNIT;
  forwardVelocity = _mForwardVelocityLimits->getValue().limit(forward_velocity) * cedar::unit::DEFAULT_VELOCITY_UNIT;
}

void cedar::dev::Locomotion::thresholdTurningRate(cedar::unit::AngularVelocity& turningRate) const
{
  //!todo fix this by writing a AngularVelocityLimitsPatameter
  double turning_rate = turningRate / cedar::unit::DEFAULT_ANGULAR_VELOCITY_UNIT;
  turningRate = _mTurningRateLimits->getValue().limit(turning_rate) * cedar::unit::DEFAULT_ANGULAR_VELOCITY_UNIT;
}

void cedar::dev::Locomotion::stop()
{
  setForwardVelocityAndTurningRate
  (
    0.0 * cedar::unit::DEFAULT_VELOCITY_UNIT,
    0.0 * cedar::unit::DEFAULT_ANGULAR_VELOCITY_UNIT
  );
}
