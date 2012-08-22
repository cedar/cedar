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
#include "cedar/devices/robot/Locomotion.h"

// SYSTEM INCLUDES

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------
cedar::dev::robot::Locomotion::Locomotion()
:
_mForwardVelocityLimits
(
  new cedar::aux::math::DoubleLimitsParameter(this, "forward velocity limits", -2.0, 2.0, 2.0, 4.0)
),
_mTurningRateLimits
(
  new cedar::aux::math::DoubleLimitsParameter
      (
        this,
        "turning rate limits",
        -2.0 * cedar::aux::math::pi,
        0.0,
        0.0,
        2.0 * cedar::aux::math::pi
      )
)
{}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

double cedar::dev::robot::Locomotion::getForwardVelocity() const
{
  return mForwardVelocity;
}

double cedar::dev::robot::Locomotion::getTurningRate() const
{
  return mTurningRate;
}

void cedar::dev::robot::Locomotion::thresholdForwardVelocity(double& forwardVelocity) const
{
  forwardVelocity = _mForwardVelocityLimits->getValue().limit(forwardVelocity);
}

void cedar::dev::robot::Locomotion::thresholdTurningRate(double& turningRate) const
{
  turningRate = _mTurningRateLimits->getValue().limit(turningRate);
}

void cedar::dev::robot::Locomotion::stop()
{
  setForwardVelocityAndTurningRate(0.0, 0.0);
}
