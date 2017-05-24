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
#include "cedar/devices/sensors/camera/CamProperty.h"

// SYSTEM INCLUDES

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------
cedar::dev::sensors::camera::CamProperty::CamProperty
(
  cedar::dev::sensors::camera::Property::Id id,
  std::string name,
  double minValue,
  double maxValue,
  double defaultValue,
  bool supported,
  bool autoCapable,
  bool manualCapable
)
:
cedar::aux::Configurable(),
mId(id),
mName(name),
mSupported(supported),
mAutoCapable(autoCapable),
mManualCapable(manualCapable),
mDefaultValue(128.f),
doNotHandleEvents(true),
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
                  cedar::dev::sensors::camera::PropertyMode::typePtr(),
                  cedar::dev::sensors::camera::PropertyMode::BACKEND_DEFAULT
                )
              )
{

  QObject::connect(mpPropertyValue.get(),SIGNAL(valueChanged()),this,SLOT(propertyValueSlot()));
  QObject::connect(mpPropertyMode.get(),SIGNAL(valueChanged()),this,SLOT(propertyModeSlot()));

  // by default, all properties are in BACKEND_DEFAULT mode. It isn't allowed to change the value manually
  mpPropertyValue->setConstant(true);

  this->update();
  doNotHandleEvents = false;
}

cedar::dev::sensors::camera::CamProperty::~CamProperty()
{
}

//----------------------------------------------------------------------------------------------------------------------
// slots
//----------------------------------------------------------------------------------------------------------------------
void cedar::dev::sensors::camera::CamProperty::propertyValueSlot()
{
  if (doNotHandleEvents)
  {
    return;
  }
  emit propertyValueChanged(mId, mpPropertyValue->getValue());
}

void cedar::dev::sensors::camera::CamProperty::propertyModeSlot()
{
  if (doNotHandleEvents)
  {
    return;
  }

  cedar::dev::sensors::camera::PropertyMode::Id new_mode_id = mpPropertyMode->getValue();

  // the grabber have to be reinitialized (i.e. destroyed and new created to do this)
  switch (new_mode_id)
  {
    case cedar::dev::sensors::camera::PropertyMode::MANUAL:
      mpPropertyValue->setConstant(false);
      break;

    case cedar::dev::sensors::camera::PropertyMode::BACKEND_DEFAULT:
      this->setValue(mDefaultValue);

    // AUTO and BACKEND_DEFAULT
    default:
      mpPropertyValue->setConstant(true);
  }
  emit propertyModeChanged(mId, mpPropertyMode->getValue());

}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::dev::sensors::camera::CamProperty::update()
{
  if (!mSupported)
  {
    this->disable();
  }

  if (mAutoCapable)
  {
    mpPropertyMode->enable(cedar::dev::sensors::camera::PropertyMode::AUTO);
  }
  else
  {
    mpPropertyMode->disable(cedar::dev::sensors::camera::PropertyMode::AUTO);
  }

  if (mManualCapable)
  {
    mpPropertyMode->enable(cedar::dev::sensors::camera::PropertyMode::MANUAL);
    mpPropertyMode->enable(cedar::dev::sensors::camera::PropertyMode::BACKEND_DEFAULT);
  }
  else
  {
    mpPropertyMode->disable(cedar::dev::sensors::camera::PropertyMode::MANUAL);
    mpPropertyMode->disable(cedar::dev::sensors::camera::PropertyMode::BACKEND_DEFAULT);
  }
}

void cedar::dev::sensors::camera::CamProperty::disable()
{
  mpPropertyValue->setConstant(true);
  mpPropertyMode->setConstant(true);
  //  mpPropertyValue->setHidden(true);
  //  mpPropertyMode->setHidden(true);
}

void cedar::dev::sensors::camera::CamProperty::enable()
{
  mpPropertyMode->setConstant(false);

  cedar::dev::sensors::camera::PropertyMode::Id new_mode_id = mpPropertyMode->getValue();
  if (new_mode_id == cedar::dev::sensors::camera::PropertyMode::MANUAL)
  {
    mpPropertyValue->setConstant(false);
  }
  else
  {
    mpPropertyValue->setConstant(true);
  }
}

void cedar::dev::sensors::camera::CamProperty::setDefaultValue(double value)
{
  mDefaultValue = value;
}

cedar::dev::sensors::camera::Property::Id cedar::dev::sensors::camera::CamProperty::getId() const
{
  return mId;
}

double cedar::dev::sensors::camera::CamProperty::getValue() const
{
  return mpPropertyValue->getValue();
}

void cedar::dev::sensors::camera::CamProperty::setValue(double value)
{
    mpPropertyValue->setValue(value);
}

cedar::dev::sensors::camera::PropertyMode::Id cedar::dev::sensors::camera::CamProperty::getMode() const
{
  return static_cast<cedar::dev::sensors::camera::PropertyMode::Id>(mpPropertyMode->getValue());
}

void cedar::dev::sensors::camera::CamProperty::setMode(cedar::dev::sensors::camera::PropertyMode::Id modeId)
{
  // ParameterPtr->setValue() refuses unavailable (i.e. disabled) modes

  // this method is called from commandline programs as well as from the gui (i.e. the mpPropertyMode control)
  // check this to avoid an infinite loop on indirect invocation from the gui
  if (mpPropertyMode->getValue() != modeId)
  {
    mpPropertyMode->setValue(modeId);
  }
}

double cedar::dev::sensors::camera::CamProperty::getMinValue() const
{
  return mpPropertyValue->getMinimum();
}


double cedar::dev::sensors::camera::CamProperty::getMaxValue() const
{
  return mpPropertyValue->getMaximum();
}

bool cedar::dev::sensors::camera::CamProperty::isSupported() const
{
  return mSupported;
}

bool cedar::dev::sensors::camera::CamProperty::isAutoCapable() const
{
  return mAutoCapable;
}

bool cedar::dev::sensors::camera::CamProperty::isManualCapable() const
{
  return mManualCapable;
}
