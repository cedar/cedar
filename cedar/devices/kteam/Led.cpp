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

    File:        Led.cpp

    Maintainer:  Mathis Richter
    Email:       mathis.richter@ini.rub.de
    Date:        2013 02 05

    Description: A component that allows control of LEDs on KTeam robots.

    Credits:

======================================================================================================================*/

// CEDAR INCLUDES
#include "cedar/devices/kteam/Led.h"
#include "cedar/devices/Component.h"
#include "cedar/devices/Channel.h"

// SYSTEM INCLUDES

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------
cedar::dev::kteam::Led::Led()
{
}

cedar::dev::kteam::Led::Led(cedar::dev::ChannelPtr channel)
:
cedar::dev::Component(channel)
{
}

cedar::dev::kteam::Led::~Led()
{
  prepareComponentDestructAbsolutelyRequired();
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

bool cedar::dev::kteam::Led::applyBrakeSlowlyController()
{
  return true;
}

bool cedar::dev::kteam::Led::applyBrakeNowController()
{
  return true;
}

bool cedar::dev::kteam::Led::applyCrashbrake()
{
  return true;
}


