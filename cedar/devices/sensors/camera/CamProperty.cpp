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

  this->updateGuiElements();
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

  // if it is set to manual, it is not possible to get back to backend default, because this value is never stored.
  // the grabber have to be reinitialized (i.e. destroyed and new created to do this)
  switch (new_mode_id)
  {
    case cedar::dev::sensors::camera::PropertyMode::MANUAL:
      mpPropertyValue->setConstant(false);
      break;

    case cedar::dev::sensors::camera::PropertyMode::BACKEND_DEFAULT:
      this->setValue(mDefaultValue);

    default:
      // AUTO and BACKEND_DEFAULT
      mpPropertyValue->setConstant(true);
  }
  emit propertyModeChanged(mId, mpPropertyMode->getValue());
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::dev::sensors::camera::CamProperty::updateGuiElements()
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
  //std::cout << "disable property " << mId << std::endl;
  mpPropertyValue->setConstant(true);
  mpPropertyMode->setConstant(true);
  //  mpPropertyValue->setHidden(true);
  //  mpPropertyMode->setHidden(true);
}

void cedar::dev::sensors::camera::CamProperty::enable()
{
  //std::cout << "enable property " << mId << std::endl;
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

cedar::dev::sensors::camera::Property::Id cedar::dev::sensors::camera::CamProperty::getId()
{
  return mId;
}

double cedar::dev::sensors::camera::CamProperty::getValue()
{
  return mpPropertyValue->getValue();
}

void cedar::dev::sensors::camera::CamProperty::setValue(double value)
{
    mpPropertyValue->setValue(value);
}


cedar::dev::sensors::camera::PropertyMode::Id cedar::dev::sensors::camera::CamProperty::getMode()
{
  return static_cast<cedar::dev::sensors::camera::PropertyMode::Id>(mpPropertyMode->getValue());
}

void cedar::dev::sensors::camera::CamProperty::setMode(cedar::dev::sensors::camera::PropertyMode::Id modeId)
{
  // ParameterPtr->setValue() refuses unavailable modes

  // this method is called from commandline programs as well as from the gui (i.e. the mpPropertyMode control)
  // check this to avoid an infinite loop on indirect invocation from the gui
  if (mpPropertyMode->getValue() != modeId)
  {
    mpPropertyMode->setValue(modeId);
  }
}


double cedar::dev::sensors::camera::CamProperty::getMinValue()
{
  return mpPropertyValue->getMinimum();
}


double cedar::dev::sensors::camera::CamProperty::getMaxValue()
{
  return mpPropertyValue->getMaximum();
}

bool cedar::dev::sensors::camera::CamProperty::isSupported()
{
  return mSupported;
}


bool cedar::dev::sensors::camera::CamProperty::isAutoCapable()
{
  return mAutoCapable;
}

bool cedar::dev::sensors::camera::CamProperty::isManualCapable()
{
  return mManualCapable;
}
