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

    File:        MobileRobot.cpp

    Maintainer:  Andre Bartel
    Email:       andre.bartel@ini.ruhr-uni-bochum.de
    Date:        2011 03 19

    Description: An object of this class represents the drive of a mobile robot.

    Credits:

======================================================================================================================*/

// LOCAL INCLUDES

#include "MobileRobot.h"

// PROJECT INCLUDES

// SYSTEM INCLUDES
#include <iostream>

using namespace cedar::dev::robot::mobile;

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

  MobileRobot::MobileRobot()
  {

  }

  MobileRobot::~MobileRobot()
  {

  }

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

  const std::vector<double>& MobileRobot::getVelocity() const
  {
    return mVelocity;
  }

  double MobileRobot::getForwardVelocity() const
  {
    return mVelocity[0];
  }

  double MobileRobot::getTurningRate() const
  {
    return mVelocity[1];
  }

  int MobileRobot::stop()
  {
    int s = setVelocity(0,0); //stop by setting both forward velocity and turning rate to 0
    if (s == 0 && _mDebug) //setting velocity failed
    {
      std::cout << "MobileRobot: Error Stopping Robot\n";
    }
    else if (_mDebug)
    {
      std::cout << "MobileRobot: Stopping Robot Successful\n";
    }

    return s;
  }
