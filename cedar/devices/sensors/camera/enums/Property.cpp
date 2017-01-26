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

    File:        CameraProperty.cpp

    Maintainer:  Georg Hartinger
    Email:       georg.hartinger@ini.rub.de
    Date:        2011 08 01

    Description: Implementation of the CameraProperty enum-type class

    Credits:

======================================================================================================================*/

#include "cedar/configuration.h"   // MAKE FIREWIRE OPTIONAL

// CEDAR INCLUDES
#include "cedar/devices/sensors/camera/enums/Property.h"

// SYSTEM INCLUDES


cedar::aux::EnumType<cedar::dev::sensors::camera::Property>
            cedar::dev::sensors::camera::Property::mType("cedar::dev::sensors::camera::Property::");

//!@cond SKIPPED_DOCUMENTATION
#ifndef CEDAR_COMPILER_MSVC
const cedar::dev::sensors::camera::Property::Id cedar::dev::sensors::camera::Property::PROP_BRIGHTNESS;
const cedar::dev::sensors::camera::Property::Id cedar::dev::sensors::camera::Property::PROP_SATURATION;
const cedar::dev::sensors::camera::Property::Id cedar::dev::sensors::camera::Property::PROP_HUE;
const cedar::dev::sensors::camera::Property::Id cedar::dev::sensors::camera::Property::PROP_GAIN;
const cedar::dev::sensors::camera::Property::Id cedar::dev::sensors::camera::Property::PROP_EXPOSURE;
const cedar::dev::sensors::camera::Property::Id cedar::dev::sensors::camera::Property::PROP_WHITE_BALANCE_BLUE_U;
const cedar::dev::sensors::camera::Property::Id cedar::dev::sensors::camera::Property::PROP_SHARPNESS;
const cedar::dev::sensors::camera::Property::Id cedar::dev::sensors::camera::Property::PROP_AUTO_EXPOSURE;
const cedar::dev::sensors::camera::Property::Id cedar::dev::sensors::camera::Property::PROP_GAMMA;
const cedar::dev::sensors::camera::Property::Id cedar::dev::sensors::camera::Property::PROP_TEMPERATURE;
/* not supported from cedar
const cedar::dev::sensors::camera::Property::Id cedar::dev::sensors::camera::Property::PROP_TRIGGER;
const cedar::dev::sensors::camera::Property::Id cedar::dev::sensors::camera::Property::PROP_TRIGGER_DELAY;
*/
const cedar::dev::sensors::camera::Property::Id cedar::dev::sensors::camera::Property::PROP_WHITE_BALANCE_RED_V;
const cedar::dev::sensors::camera::Property::Id cedar::dev::sensors::camera::Property::PROP_ZOOM;
const cedar::dev::sensors::camera::Property::Id cedar::dev::sensors::camera::Property::PROP_FOCUS;
#endif // CEDAR_COMPILER_MSVC
//!@endcond

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

void cedar::dev::sensors::camera::Property::construct()
{
  mType.type()->def(cedar::aux::Enum(
                                      cedar::dev::sensors::camera::Property::PROP_BRIGHTNESS,
                                      "PROP_BRIGHTNESS",
                                      "brightness"
                                    ));
  mType.type()->def(cedar::aux::Enum(
                                      cedar::dev::sensors::camera::Property::PROP_SATURATION,
                                      "PROP_SATURATION",
                                      "saturation"
                                    ));
  mType.type()->def(cedar::aux::Enum(
                                      cedar::dev::sensors::camera::Property::PROP_HUE,
                                      "PROP_HUE",
                                      "hue"
                                    ));
  mType.type()->def(cedar::aux::Enum(
                                      cedar::dev::sensors::camera::Property::PROP_GAIN,
                                      "PROP_GAIN",
                                      "gain"
                                    ));
  mType.type()->def(cedar::aux::Enum(
                                      cedar::dev::sensors::camera::Property::PROP_EXPOSURE,
                                      "PROP_EXPOSURE",
                                      "exposure shutter" // in case of firewire: shutter is meant
                                    ));
  mType.type()->def(cedar::aux::Enum(
                                      cedar::dev::sensors::camera::Property::PROP_WHITE_BALANCE_BLUE_U,
                                      "PROP_WHITE_BALANCE_BLUE_U",
                                      "white balance blue"
                                    ));
  mType.type()->def(cedar::aux::Enum(
                                      cedar::dev::sensors::camera::Property::PROP_SHARPNESS,
                                      "PROP_SHARPNESS",
                                      "sharpness"
                                    ));
  mType.type()->def(cedar::aux::Enum(
                                      cedar::dev::sensors::camera::Property::PROP_AUTO_EXPOSURE,
                                      "PROP_AUTO_EXPOSURE",
                                      "exposure reference level"
                                      //"exposure control done by camera, adjust reference level using this feature"
                                    ));
  mType.type()->def(cedar::aux::Enum(
                                      cedar::dev::sensors::camera::Property::PROP_GAMMA,
                                      "PROP_GAMMA",
                                      "gamma"
                                    ));
  mType.type()->def(cedar::aux::Enum(
                                      cedar::dev::sensors::camera::Property::PROP_TEMPERATURE,
                                      "PROP_TEMPERATURE",
                                      "temperature"
                                    ));
/* not supported from cedar
  mType.type()->def(cedar::aux::Enum(
                                      cedar::dev::sensors::camera::Property::PROP_TRIGGER,
                                      "PROP_TRIGGER",
                                      "trigger"
                                    ));
  mType.type()->def(cedar::aux::Enum(
                                      cedar::dev::sensors::camera::Property::PROP_TRIGGER_DELAY,
                                      "PROP_TRIGGER_DELAY",
                                      "trigger delay"
                                    ));
*/
  mType.type()->def(cedar::aux::Enum(
                                      cedar::dev::sensors::camera::Property::PROP_WHITE_BALANCE_RED_V,
                                      "PROP_WHITE_BALANCE_RED_V",
                                      "white balance red"
                                    ));
  mType.type()->def(cedar::aux::Enum(
                                      cedar::dev::sensors::camera::Property::PROP_ZOOM,
                                      "PROP_ZOOM",
                                      "zoom"
                                    ));
  mType.type()->def(cedar::aux::Enum(
                                      cedar::dev::sensors::camera::Property::PROP_FOCUS,
                                      "PROP_FOCUS",
                                      "focus"
                                    ));
}
//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------


const cedar::aux::EnumBase& cedar::dev::sensors::camera::Property::type()
{
  return *cedar::dev::sensors::camera::Property::mType.type();
}

const cedar::dev::sensors::camera::Property::TypePtr& cedar::dev::sensors::camera::Property::typePtr()
{
  return cedar::dev::sensors::camera::Property::mType.type();
}
