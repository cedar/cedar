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

    File:        CameraSetting.cpp

    Maintainer:  Georg Hartinger
    Email:       georg.hartinger@ini.rub.de
    Date:        2011 08 01

    Description: Implementation of the CameraSetting enum-type class

    Credits:

======================================================================================================================*/

#include "cedar/configuration.h"   // MAKE FIREWIRE OPTIONAL

// CEDAR INCLUDES
#include "cedar/devices/sensors/visual/camera/enums/CameraSetting.h"

// SYSTEM INCLUDES


cedar::aux::EnumType<cedar::dev::sensors::visual::CameraSetting>
                     cedar::dev::sensors::visual::CameraSetting::mType("cedar::dev::sensors::visual::CameraSetting::");

//!@cond SKIPPED_DOCUMENTATION
#ifndef CEDAR_COMPILER_MSVC
  const cedar::dev::sensors::visual::CameraSetting::Id cedar::dev::sensors::visual::CameraSetting::FRAME_WIDTH;
  const cedar::dev::sensors::visual::CameraSetting::Id cedar::dev::sensors::visual::CameraSetting::FRAME_HEIGHT;
  const cedar::dev::sensors::visual::CameraSetting::Id cedar::dev::sensors::visual::CameraSetting::FPS;
  const cedar::dev::sensors::visual::CameraSetting::Id cedar::dev::sensors::visual::CameraSetting::MODE;
#ifdef CEDAR_USE_LIB_DC1394
  const cedar::dev::sensors::visual::CameraSetting::Id cedar::dev::sensors::visual::CameraSetting::ISO_SPEED;
#endif
#endif // CEDAR_COMPILER_MSVC
//!@endcond

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

void cedar::dev::sensors::visual::CameraSetting::construct()
{
  mType.type()->def(cedar::aux::Enum(
                                      cedar::dev::sensors::visual::CameraSetting::FRAME_WIDTH,
                                      "SETTING_FRAME_WIDTH",
                                      "framewidth"
                                    ));
  mType.type()->def(cedar::aux::Enum(
                                      cedar::dev::sensors::visual::CameraSetting::FRAME_HEIGHT,
                                      "SETTING_FRAME_HEIGHT",
                                      "frameheight"
                                    ));
  mType.type()->def(cedar::aux::Enum(
                                      cedar::dev::sensors::visual::CameraSetting::FPS,
                                      "SETTING_FPS",
                                      "fps"
                                    ));
  mType.type()->def(cedar::aux::Enum(
                                      cedar::dev::sensors::visual::CameraSetting::MODE,
                                      "SETTING_MODE",
                                      "mode"
                                    ));
#ifdef CEDAR_USE_LIB_DC1394
  mType.type()->def(cedar::aux::Enum(
                                      cedar::dev::sensors::visual::CameraSetting::ISO_SPEED,
                                      "SETTING_ISO_SPEED",
                                      "ISO speed"
                                    ));
#endif
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

const cedar::aux::EnumBase& cedar::dev::sensors::visual::CameraSetting::type()
{
  return *cedar::dev::sensors::visual::CameraSetting::mType.type();
}

const cedar::dev::sensors::visual::CameraSetting::TypePtr& cedar::dev::sensors::visual::CameraSetting::typePtr()
{
  return cedar::dev::sensors::visual::CameraSetting::mType.type();
}
