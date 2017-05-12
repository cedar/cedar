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
// static members
//----------------------------------------------------------------------------------------------------------------------

unsigned int cedar::dev::kteam::khepera::Gripper::GRIPPER_POSITION = 0;
unsigned int cedar::dev::kteam::khepera::Gripper::OPTICAL_SENSOR = 1;
unsigned int cedar::dev::kteam::khepera::Gripper::RESISTIVITY = 2;


//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::dev::kteam::khepera::Gripper::Gripper()
{
  initialize();
}

// constructor taking an externally created channel
cedar::dev::kteam::khepera::Gripper::Gripper(cedar::dev::ChannelPtr channel)
:
cedar::dev::Component(channel)
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
  this->installCommandType(GRIPPER_POSITION, "gripper position");
  this->setCommandDimensionality(GRIPPER_POSITION, 1);

  this->installMeasurementType(OPTICAL_SENSOR, "optical sensor");
  this->setMeasurementDimensionality(OPTICAL_SENSOR, 1);

  this->installMeasurementType(RESISTIVITY, "resistivity");
  this->setMeasurementDimensionality(RESISTIVITY, 1);
}

void cedar::dev::kteam::khepera::Gripper::openGripper()
{
  this->setGripperPosition(true);
}

void cedar::dev::kteam::khepera::Gripper::closeGripper()
{
  this->setGripperPosition(false);
}

bool cedar::dev::kteam::khepera::Gripper::applyBrakeNowController()
{
  // TODO
  return false;
}

bool cedar::dev::kteam::khepera::Gripper::applyBrakeSlowlyController()
{
  // TODO
  return false;
}

bool cedar::dev::kteam::khepera::Gripper::applyCrashbrake()
{
  // TODO
  return false;
}

