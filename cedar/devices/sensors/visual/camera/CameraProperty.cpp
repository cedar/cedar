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

    File:        CameraProperty.cpp

    Maintainer:  Georg Hartinger
    Email:       georg.hartinger@ini.rub.de
    Date:        2011 08 01

    Description: Implementation of the CameraProperty enum-type class

    Credits:

======================================================================================================================*/

// LOCAL INCLUDES
#include "cedar/devices/sensors/visual/camera/CameraProperty.h"

// PROJECT INCLUDES

// SYSTEM INCLUDES


cedar::aux::EnumType<cedar::dev::sensors::visual::CameraProperty>
            cedar::dev::sensors::visual::CameraProperty::mType("cedar::dev::sensors::visual::CameraProperty::");

//!@cond SKIPPED_DOCUMENTATION
#ifndef MSVC
const cedar::dev::sensors::visual::CameraProperty::Id cedar::dev::sensors::visual::CameraProperty::PROP_BRIGHTNESS;
const cedar::dev::sensors::visual::CameraProperty::Id cedar::dev::sensors::visual::CameraProperty::PROP_SATURATION;
const cedar::dev::sensors::visual::CameraProperty::Id cedar::dev::sensors::visual::CameraProperty::PROP_HUE;
const cedar::dev::sensors::visual::CameraProperty::Id cedar::dev::sensors::visual::CameraProperty::PROP_GAIN;
const cedar::dev::sensors::visual::CameraProperty::Id cedar::dev::sensors::visual::CameraProperty::PROP_EXPOSURE;
const cedar::dev::sensors::visual::CameraProperty::Id cedar::dev::sensors::visual::CameraProperty::PROP_WHITE_BALANCE_BLUE_U;
const cedar::dev::sensors::visual::CameraProperty::Id cedar::dev::sensors::visual::CameraProperty::PROP_SHARPNESS;
const cedar::dev::sensors::visual::CameraProperty::Id cedar::dev::sensors::visual::CameraProperty::PROP_AUTO_EXPOSURE;
const cedar::dev::sensors::visual::CameraProperty::Id cedar::dev::sensors::visual::CameraProperty::PROP_GAMMA;
const cedar::dev::sensors::visual::CameraProperty::Id cedar::dev::sensors::visual::CameraProperty::PROP_TEMPERATURE;
const cedar::dev::sensors::visual::CameraProperty::Id cedar::dev::sensors::visual::CameraProperty::PROP_TRIGGER;
const cedar::dev::sensors::visual::CameraProperty::Id cedar::dev::sensors::visual::CameraProperty::PROP_TRIGGER_DELAY;
const cedar::dev::sensors::visual::CameraProperty::Id cedar::dev::sensors::visual::CameraProperty::PROP_WHITE_BALANCE_RED_V;
const cedar::dev::sensors::visual::CameraProperty::Id cedar::dev::sensors::visual::CameraProperty::PROP_ZOOM;
const cedar::dev::sensors::visual::CameraProperty::Id cedar::dev::sensors::visual::CameraProperty::PROP_FOCUS;
#endif // MSVC
//!@endcond

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

void cedar::dev::sensors::visual::CameraProperty::construct()
{
  mType.type()->def(cedar::aux::Enum(
                                      cedar::dev::sensors::visual::CameraProperty::PROP_BRIGHTNESS,
                                      "PROP_BRIGHTNESS",
                                      "Brightness"
                                    ));
  mType.type()->def(cedar::aux::Enum(
                                      cedar::dev::sensors::visual::CameraProperty::PROP_SATURATION,
                                      "PROP_SATURATION",
                                      "Saturation"
                                    ));
  mType.type()->def(cedar::aux::Enum(
                                      cedar::dev::sensors::visual::CameraProperty::PROP_HUE,
                                      "PROP_HUE",
                                      "Hue"
                                    ));
  mType.type()->def(cedar::aux::Enum(
                                      cedar::dev::sensors::visual::CameraProperty::PROP_GAIN,
                                      "PROP_GAIN",
                                      "Gain"
                                    ));
  mType.type()->def(cedar::aux::Enum(
                                      cedar::dev::sensors::visual::CameraProperty::PROP_EXPOSURE,
                                      "PROP_EXPOSURE",
                                      "Exposure (In case of a dc1394-camera, shutter is meant)"
                                    ));
  mType.type()->def(cedar::aux::Enum(
                                      cedar::dev::sensors::visual::CameraProperty::PROP_WHITE_BALANCE_BLUE_U,
                                      "PROP_WHITE_BALANCE_BLUE_U",
                                      "White balance blue"
                                    ));
  mType.type()->def(cedar::aux::Enum(
                                      cedar::dev::sensors::visual::CameraProperty::PROP_SHARPNESS,
                                      "PROP_SHARPNESS",
                                      "Sharpness"
                                    ));
  mType.type()->def(cedar::aux::Enum(
                                      cedar::dev::sensors::visual::CameraProperty::PROP_AUTO_EXPOSURE,
                                      "PROP_AUTO_EXPOSURE",
                                      "Exposure control done by camera, adjust reference level using this feature"
                                    ));
  mType.type()->def(cedar::aux::Enum(
                                      cedar::dev::sensors::visual::CameraProperty::PROP_GAMMA,
                                      "PROP_GAMMA",
                                      "Gamma"
                                    ));
  mType.type()->def(cedar::aux::Enum(
                                      cedar::dev::sensors::visual::CameraProperty::PROP_TEMPERATURE,
                                      "PROP_TEMPERATURE",
                                      "Temperature"
                                    ));
 mType.type()->def(cedar::aux::Enum(
                                      cedar::dev::sensors::visual::CameraProperty::PROP_TRIGGER,
                                      "PROP_TRIGGER",
                                      "Trigger"
                                    ));
  mType.type()->def(cedar::aux::Enum(
                                      cedar::dev::sensors::visual::CameraProperty::PROP_TRIGGER_DELAY,
                                      "PROP_TRIGGER_DELAY",
                                      "Trigger delay"
                                    ));

  mType.type()->def(cedar::aux::Enum(
                                      cedar::dev::sensors::visual::CameraProperty::PROP_WHITE_BALANCE_RED_V,
                                      "PROP_WHITE_BALANCE_RED_V",
                                      "White balance red"
                                    ));
  mType.type()->def(cedar::aux::Enum(
                                      cedar::dev::sensors::visual::CameraProperty::PROP_ZOOM,
                                      "PROP_ZOOM",
                                      "Zoom"
                                    ));
  mType.type()->def(cedar::aux::Enum(
                                      cedar::dev::sensors::visual::CameraProperty::PROP_FOCUS,
                                      "PROP_FOCUS",
                                      "Focus"
                                    ));
}
//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------


const cedar::aux::EnumBase& cedar::dev::sensors::visual::CameraProperty::type()
{
  return *cedar::dev::sensors::visual::CameraProperty::mType.type();
}

const cedar::dev::sensors::visual::CameraProperty::TypePtr& cedar::dev::sensors::visual::CameraProperty::typePtr()
{
  return cedar::dev::sensors::visual::CameraProperty::mType.type();
}
