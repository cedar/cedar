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

// LOCAL INCLUDES

#include "devices/robot/Locomotion.h"

// PROJECT INCLUDES

// SYSTEM INCLUDES
#include <iostream>

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

const std::vector<double>& cedar::dev::robot::Locomotion::getVelocity() const
{
  return mVelocity;
}

double cedar::dev::robot::Locomotion::getForwardVelocity() const
{
  return mVelocity[0];
}

double cedar::dev::robot::Locomotion::getTurningRate() const
{
  return mVelocity[1];
}

int cedar::dev::robot::Locomotion::stop()
{
  int s = setVelocity(0,0); //stop by setting both forward velocity and turning rate to 0
  if (s == 0 && _mDebug) //setting velocity failed
  {
    std::cout << "Locomotion: Error Stopping Robot\n";
  }
  else if (_mDebug)
  {
    std::cout << "Locomotion: Stopping Robot Successful\n";
  }
  return s;
}
