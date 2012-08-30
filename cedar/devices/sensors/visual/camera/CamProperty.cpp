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

    File:        Property.h

    Maintainer:  Georg Hartinger
    Email:       georg.hartinger@ini.rub.de
    Date:        2012 07 04

    Description:  Implementation for the cedar::dev::sensors::visual::Property class

    Credits:

======================================================================================================================*/

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/devices/sensors/visual/camera/CamProperty.h"

// SYSTEM INCLUDES

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------
cedar::dev::sensors::visual::CamProperty::CamProperty
(
  cedar::dev::sensors::visual::CameraProperty::Id id,
  std::string name,
  double minValue,
  double maxValue,
  double defaultValue,
  bool supported,
  bool readable,
  bool autoCapable,
  bool manualCapable
  //bool onOffCapable
)
:
cedar::aux::Configurable(),
mId(id),
mName(name),
mSupported(supported),
mReadable(readable),
mAutoCapable(autoCapable),
mManualCapable(manualCapable),
mpPropertyValue(new cedar::aux::DoubleParameter
                 (
                   this,
                   "Value",
                   defaultValue,
                   minValue,
                   maxValue
                 )
               ),
mpPropertyMode(new cedar::aux::EnumParameter
                (
                  this,
                  "Mode",
                  cedar::dev::sensors::visual::CameraPropertyMode::typePtr(),
                  cedar::dev::sensors::visual::CameraPropertyMode::BACKEND_DEFAULT
                )
              )
{

  //todo add to configurable as subsection with name mName

  // todo connect mpPropertyValue with signal for changing the value

  // todo allow only specific modes of operations for mpPropertyMode
  // which depends on autoCapable, onOffCapable, readable, manualCapable
}

//todo write signal handlers for parameter value changed
// and mModeId changed


cedar::dev::sensors::visual::CamProperty::~CamProperty()
{
}


//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------


cedar::dev::sensors::visual::CameraProperty::Id cedar::dev::sensors::visual::CamProperty::getId()
{
  return mId;
}


cedar::dev::sensors::visual::CameraPropertyMode::Id cedar::dev::sensors::visual::CamProperty::getMode()
{
  //@todo: cast to Id ??
  return mpPropertyMode->getValue();
}


void cedar::dev::sensors::visual::CamProperty::setMode(cedar::dev::sensors::visual::CameraPropertyMode::Id mode)
{
  //@!todo setmode
}


double cedar::dev::sensors::visual::CamProperty::getMinValue()
{
  //return mpPropertyValue->getLower();
  return 0.0f;
}


double cedar::dev::sensors::visual::CamProperty::getMaxValue()
{
  //return mpPropertyValue->getMaximum();
  return 1024.0f;
}

bool cedar::dev::sensors::visual::CamProperty::isSupported()
{
  return mSupported;
}

bool cedar::dev::sensors::visual::CamProperty::isReadable()
{
  return mReadable;
}

bool cedar::dev::sensors::visual::CamProperty::isAutoCapable()
{
  return mAutoCapable;
}

bool cedar::dev::sensors::visual::CamProperty::isManualCapable()
{
  return mManualCapable;
}
