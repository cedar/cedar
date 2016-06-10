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

    File:        Arm.cpp

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2014 07 30

    Description: Source file for the class cedar::dev::kteam::khepera::Arm.

    Credits:

======================================================================================================================*/

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/devices/kteam/khepera/Arm.h"

// SYSTEM INCLUDES

//----------------------------------------------------------------------------------------------------------------------
// static members
//----------------------------------------------------------------------------------------------------------------------

unsigned int cedar::dev::kteam::khepera::Arm::POSITION = 0;

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::dev::kteam::khepera::Arm::Arm()
:
_mArmPositionLimits(new cedar::aux::math::UIntLimitsParameter(this, "arm position limits", 0, 0, 255, 255, 0, 255))
{
  this->_mArmPositionLimits->setDefaults(190, 249);
  this->_mArmPositionLimits->makeDefault();

  this->installCommandAndMeasurementType(POSITION, "arm position");
  this->setCommandAndMeasurementDimensionality(POSITION, 1);
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------


bool cedar::dev::kteam::khepera::Arm::applyBrakeNowController()
{
  // TODO
  return false;
}

bool cedar::dev::kteam::khepera::Arm::applyBrakeSlowlyController()
{
  // TODO
  return false;
}
