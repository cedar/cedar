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

    Maintainer:  Andre Bartel
    Email:       andre.bartel@ini.ruhr-uni-bochum.de
    Date:        2011 03 19

    Description: An object of this class represents the differential drive of a PWM-driven robot.

    Credits:

======================================================================================================================*/

// LOCAL INCLUDES

#include "KTeamDrive.h"

// PROJECT INCLUDES

// SYSTEM INCLUDES

using namespace cedar::dev::robot::mobile;

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

KTeamDrive::KTeamDrive()
{

}

KTeamDrive::~KTeamDrive()
{

}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

double KTeamDrive::getPulsesPerRevolution() const
{
  return _mPulsesPerRevolution;
}

int KTeamDrive::getMaximalEncoderValue() const
{
  return _mMaximalEncoderValue;
}

int KTeamDrive::getMinimalEncoderValue() const
{
  return _mMinimalEncoderValue;
}

double KTeamDrive::getDistancePerPulse() const
{
  return mDistancePerPulse;
}

int KTeamDrive::getMaximalNumberPulsesPerSecond() const
{
  return _mMaximalNumberPulsesPerSecond;
}

int KTeamDrive::resetEncoder()
{
  int s = setEncoder(0,0);
  if (s == 0 && _mDebug) //setting encoder failed
  {
    std::cout << "KTeamDrive: Error Resetting Encoder\n";
  }
  return s;
}

int KTeamDrive::reset()
{
  int s = setWheelSpeed(0,0); // = 1 if setting wheel speed successful, else 0
  s = s * resetEncoder(); // = 1 if setting both wheel speed and resetting encoder successful, else 0
  if (s == 0 && _mDebug) //setting wheel speed or resetting encoder failed
  {
    std::cout << "KTeamDrive: Error Resetting Robot\n";
    }
  else if (_mDebug)
  {
    std::cout << "KTeamDrive: Resetting Robot Successful\n";
  }

  return s;
}
