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

    File:        KTeamDrive.cpp

    Maintainer:  Stephan Zibner
    Email:       stephan.zibner@ini.ruhr-uni-bochum.de
    Date:        2011 03 19

    Description: An object of this class represents the differential drive of a PWM-driven robot.

    Credits:

======================================================================================================================*/

// CEDAR INCLUDES
#include "cedar/devices/kteam/Drive.h"

// SYSTEM INCLUDES
#include <iostream>

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------
cedar::dev::kteam::Drive::Drive()
:
_mPulsesPerRevolution(new cedar::aux::DoubleParameter(this, "pulses per revolution", 0)),
_mMaximalEncoderValue(new cedar::aux::IntParameter(this, "maximal encoder value", 0)),
_mMinimalEncoderValue(new cedar::aux::IntParameter(this, "minimal encoder value", 0)),
_mMaximalNumberPulsesPerSecond(new cedar::aux::IntParameter(this, "maximum number of pulses per second", 0))
{
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------
double cedar::dev::kteam::Drive::getPulsesPerRevolution() const
{
  return _mPulsesPerRevolution->getValue();
}

int cedar::dev::kteam::Drive::getMaximalEncoderValue() const
{
  return _mMaximalEncoderValue->getValue();
}

int cedar::dev::kteam::Drive::getMinimalEncoderValue() const
{
  return _mMinimalEncoderValue->getValue();
}

double cedar::dev::kteam::Drive::getDistancePerPulse() const
{
  return mDistancePerPulse;
}

int cedar::dev::kteam::Drive::getMaximalNumberPulsesPerSecond() const
{
  return _mMaximalNumberPulsesPerSecond->getValue();
}

int cedar::dev::kteam::Drive::resetEncoder()
{
  int s = setEncoder(0,0);
  if (s == 0 && this->debug()) // setting encoder failed
  {
    std::cout << "KTeamDrive: Error Resetting Encoder\n";
  }
  return s;
}

int cedar::dev::kteam::Drive::reset()
{
  int s = setWheelSpeed(0,0); // = 1 if setting wheel speed successful, else 0
  s = s * resetEncoder(); // = 1 if setting both wheel speed and resetting encoder successful, else 0
  if (s == 0 && this->debug()) // setting wheel speed or resetting encoder failed
  {
    std::cout << "KTeamDrive: Error Resetting Robot\n";
  }
  else if (this->debug())
  {
    std::cout << "KTeamDrive: Resetting Robot Successful\n";
  }

  return s;
}
