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

    File:        Gripper.cpp

    Maintainer:  Mathis Richter
    Email:       mathis.richter@ini.rub.de
    Date:        2012 11 26

    Description: Gripper that can be attached to the Khepera robot.

    Credits:

======================================================================================================================*/

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/auxiliaries/casts.h"
#include "cedar/auxiliaries/StringParameter.h"
#include "cedar/auxiliaries/math/UIntLimitsParameter.h"
#include "cedar/devices/kteam/khepera/Gripper.h"
#include "cedar/devices/kteam/serialChannelHelperFunctions.h"
#include "cedar/devices/kteam/SerialChannel.h"

// SYSTEM INCLUDES

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::dev::kteam::khepera::Gripper::Gripper()
:
_mArmPositionLimits(new cedar::aux::math::UIntLimitsParameter(this, "arm position limits", 0, 0, 255, 255, 0, 255))
{
  initialize();
}

// constructor taking an externally created channel
cedar::dev::kteam::khepera::Gripper::Gripper(cedar::dev::ChannelPtr channel)
:
cedar::dev::Component(channel),
_mArmPositionLimits(new cedar::aux::math::UIntLimitsParameter(this, "arm position limits", 0, 0, 255, 255, 0, 255))
{
  initialize();
}

cedar::dev::kteam::khepera::Gripper::~Gripper()
{
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::dev::kteam::khepera::Gripper::initialize()
{
  _mArmPositionLimits->setDefaults(190, 249);
  _mArmPositionLimits->makeDefault();
}

void cedar::dev::kteam::khepera::Gripper::openGripper()
{
  this->setGripperPosition(true);
}

void cedar::dev::kteam::khepera::Gripper::closeGripper()
{
  this->setGripperPosition(false);
}
