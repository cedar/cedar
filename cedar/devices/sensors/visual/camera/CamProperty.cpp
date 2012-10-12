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
  // bool onOffCapable
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
                   "value",
                   defaultValue,
                   minValue,
                   maxValue
                 )
               ),
mpPropertyMode(new cedar::aux::EnumParameter
                (
                  this,
                  "mode",
                  cedar::dev::sensors::visual::CameraPropertyMode::typePtr(),
                  cedar::dev::sensors::visual::CameraPropertyMode::BACKEND_DEFAULT
                )
              )
{

  QObject::connect(mpPropertyValue.get(),SIGNAL(valueChanged()),this,SLOT(propertyValueSlot()));
  QObject::connect(mpPropertyMode.get(),SIGNAL(valueChanged()),this,SLOT(propertyModeSlot()));

  // by default, all properties are in BACKEND_DEFAULT mode. It isn't allowed to change the value manually
  mpPropertyValue->setConstant(true);

  if (!mReadable)
  {
    // disable value (or set to const)
    mpPropertyValue->setConstant(true);
  }

  if (!mSupported)
  {
    // disable value and mode
    mpPropertyValue->setConstant(true);
    mpPropertyMode->setConstant(true);
  }

  if (!mAutoCapable)
  {
    // disable mode::auto
    mpPropertyMode->disable(cedar::dev::sensors::visual::CameraPropertyMode::AUTO);
  }

  if (!mManualCapable)
  {
    // disable mode::manual
    mpPropertyMode->disable(cedar::dev::sensors::visual::CameraPropertyMode::MANUAL);
  }

}

cedar::dev::sensors::visual::CamProperty::~CamProperty()
{
}

//----------------------------------------------------------------------------------------------------------------------
// slots
//----------------------------------------------------------------------------------------------------------------------
void cedar::dev::sensors::visual::CamProperty::propertyValueSlot()
{
  emit propertyValueChanged(mId, mpPropertyValue->getValue());
}

void cedar::dev::sensors::visual::CamProperty::propertyModeSlot()
{
  cedar::dev::sensors::visual::CameraPropertyMode::Id new_mode_id = mpPropertyValue->getValue();

  // if it is set to manual, it is not possible to get back to backend default, because this value is never stored.
  // the grabber have to be reinitialized (i.e. destroyed and new created to do this)
  if (new_mode_id == cedar::dev::sensors::visual::CameraPropertyMode::MANUAL)
  {
    std::cout << "propertyModeSlot MANUAL" << std::endl;
    mpPropertyMode->disable(cedar::dev::sensors::visual::CameraPropertyMode::BACKEND_DEFAULT);
    mpPropertyValue->setConstant(false);
  }
  else
  {
    // AUTO or BACKEND
    std::cout << "propertyModeSlot MODE AUTO" << std::endl;
    mpPropertyValue->setConstant(true);
  }

  emit propertyModeChanged(mId, mpPropertyMode->getValue());
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------


cedar::dev::sensors::visual::CameraProperty::Id cedar::dev::sensors::visual::CamProperty::getId()
{
  return mId;
}

double cedar::dev::sensors::visual::CamProperty::getValue()
{
  return mpPropertyValue->getValue();
}

void cedar::dev::sensors::visual::CamProperty::setValue(double value)
{
    mpPropertyValue->setValue(value);
}


cedar::dev::sensors::visual::CameraPropertyMode::Id cedar::dev::sensors::visual::CamProperty::getMode()
{
  return static_cast<cedar::dev::sensors::visual::CameraPropertyMode::Id>(mpPropertyMode->getValue());
}

void cedar::dev::sensors::visual::CamProperty::setMode(cedar::dev::sensors::visual::CameraPropertyMode::Id modeId)
{
  // ParameterPtr->setValue() refuses unavailable modes

  // this method is called from commandline programs as well as from the gui (i.e. the mpPropertyMode control)
  // check this to avoid an infinite loop on indirect invocation from the gui
  if (mpPropertyMode->getValue() != modeId)
  {
    mpPropertyMode->setValue(modeId);
  }
}


double cedar::dev::sensors::visual::CamProperty::getMinValue()
{
  return mpPropertyValue->getMinimum();
}


double cedar::dev::sensors::visual::CamProperty::getMaxValue()
{
  return mpPropertyValue->getMaximum();
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
