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
  installCommandAndMeasurementType( cedar::dev::Locomotion::FORWARD_VELOCITY );
  installCommandAndMeasurementType( cedar::dev::Locomotion::TURNING_RATE );

  setCommandAndMeasurementDimensionality( cedar::dev::Locomotion::FORWARD_VELOCITY, 1 );
  setCommandAndMeasurementDimensionality( cedar::dev::Locomotion::TURNING_RATE, 1 );

}

cedar::dev::Locomotion::Locomotion()
:
mForwardVelocity(new cedar::aux::VelocityData(0.0 * cedar::unit::meters_per_second)),
mTurningRate(new cedar::aux::AngularVelocityData(0.0 * cedar::unit::radians_per_second)),
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
mForwardVelocity(new cedar::aux::VelocityData(0.0 * cedar::unit::meters_per_second)),
mTurningRate(new cedar::aux::AngularVelocityData(0.0 * cedar::unit::radians_per_second)),
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

void cedar::dev::Locomotion::applyTurningRate()
{
  this->setTurningRate(this->mTurningRate->getData());
}

void cedar::dev::Locomotion::applyForwardVelocity()
{
  this->setForwardVelocity(this->mForwardVelocity->getData());
}

cedar::unit::Velocity cedar::dev::Locomotion::getForwardVelocity() const
{
  return mForwardVelocity->getData();
}

cedar::unit::AngularVelocity cedar::dev::Locomotion::getTurningRate() const
{
  return mTurningRate->getData();
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
