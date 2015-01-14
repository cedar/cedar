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

    File:        Drive.cpp

    Maintainer:  Mathis Richter
    Email:       mathis.richter@ini.rub.de
    Date:        2012 04 12

    Description: The differential drive of a PWM-driven robot.

    Credits:     Original design by Andre Bartel (2011)

======================================================================================================================*/

// CEDAR INCLUDES
#include "cedar/auxiliaries/casts.h"
#include "cedar/auxiliaries/math/IntLimitsParameter.h"
#include "cedar/auxiliaries/DoubleParameter.h"
#include "cedar/auxiliaries/math/tools.h"
#include "cedar/auxiliaries/math/constants.h"
#include "cedar/devices/kteam/Drive.h"
#include "cedar/devices/exceptions.h"
#include "cedar/units/Length.h"
#include "cedar/units/Frequency.h"

// SYSTEM INCLUDES
#include <vector>
#include <boost/units/cmath.hpp>

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------
cedar::dev::kteam::Drive::Drive()
:
_mNumberOfPulsesPerRevolution(new cedar::aux::DoubleParameter(this, "number of pulses per revolution", 0.1)),
_mEncoderLimits(new cedar::aux::math::IntLimitsParameter(this, "encoder limits", 0, -32768, 0, 32767, 0, 32767))
{
}

cedar::dev::kteam::Drive::Drive(cedar::dev::ChannelPtr channel)
:
cedar::dev::DifferentialDrive(channel),
_mNumberOfPulsesPerRevolution(new cedar::aux::DoubleParameter(this, "number of pulses per revolution", 0.1)),
_mEncoderLimits(new cedar::aux::math::IntLimitsParameter(this, "encoder limits", 0, -32768, 0, 32767, 0, 32767))
{
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

cedar::unit::Length cedar::dev::kteam::Drive::getDistancePerPulse() const
{
  return 2.0 * cedar::aux::math::pi * getWheelRadius() / getNumberOfPulsesPerRevolution();
}

double cedar::dev::kteam::Drive::getNumberOfPulsesPerRevolution() const
{
  return _mNumberOfPulsesPerRevolution->getValue();
}

cedar::aux::math::IntLimitsParameterPtr cedar::dev::kteam::Drive::getEncoderLimits() const
{
  return _mEncoderLimits;
}

void cedar::dev::kteam::Drive::reset()
{
  // stop the robot
  stop();

  // reset the encoder values of the robot to zero
  std::vector<int> encoders(2);
  encoders[0] = 0;
  encoders[1] = 0;
  setEncoders(encoders);
}

std::vector<cedar::unit::Frequency> cedar::dev::kteam::Drive::convertWheelSpeedToPulses
                                    (
                                      const std::vector<cedar::unit::Velocity>& wheelSpeed
                                    ) const
{
  CEDAR_ASSERT(wheelSpeed.size() == 2);

  // the speed has be thresholded based on the maximum possible number
  // of pulses per second (this is hardware-specific).
  // first: convert speed from m/s into Pulses/s ...
  std::vector<cedar::unit::Frequency> wheel_speed_pulses
                                      (
                                        wheelSpeed.size(),
                                        0.0 * cedar::unit::DEFAULT_FREQUENCY_UNIT
                                      );

  for (unsigned int i = 0; i < wheelSpeed.size(); ++i)
  {
    // compute the number of pulses per second and round the value to a natural number
    wheel_speed_pulses[i] = boost::units::round(wheelSpeed[i] / this->getDistancePerPulse());
  }

  return wheel_speed_pulses;
}

std::vector<cedar::unit::Velocity> cedar::dev::kteam::Drive::convertPulsesToWheelSpeed
                                    (
                                      const std::vector<cedar::unit::Frequency>& wheelSpeedPulses
                                    ) const
{
  CEDAR_ASSERT(wheelSpeedPulses.size() == 2);

  // first: convert speed from Pulses/s into m/s ...
  std::vector<cedar::unit::Velocity> wheel_speed
                                      (
                                        wheelSpeedPulses.size(),
                                        0.0 * cedar::unit::DEFAULT_VELOCITY_UNIT
                                      );

  for (unsigned int i = 0; i < wheelSpeedPulses.size(); ++i)
  {
    // compute the number of pulses per second and round the value to a natural number
    wheel_speed[i] = wheelSpeedPulses[i] * this->getDistancePerPulse();
  }

  return wheel_speed;
}

void cedar::dev::kteam::Drive::setWheelSpeedPulses(const std::vector<cedar::unit::Frequency>& wheelSpeedPulses)
{
  std::vector<cedar::unit::Velocity> wheel_speeds = convertPulsesToWheelSpeed(wheelSpeedPulses);
  this->setWheelSpeed(wheel_speeds);
}
