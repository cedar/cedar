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

    File:        CameraPropertyMode.cpp

    Maintainer:  Georg Hartinger
    Email:       georg.hartinger@ini.rub.de
    Date:        2011 08 01

    Description: Implementation of the CameraPropertyMode enum-type class

    Credits:

======================================================================================================================*/

// CEDAR INCLUDES
#include "cedar/devices/sensors/visual/camera/enums/CameraPropertyMode.h"

// SYSTEM INCLUDES


cedar::aux::EnumType<cedar::dev::sensors::visual::CameraPropertyMode>
  cedar::dev::sensors::visual::CameraPropertyMode::mType("cedar::dev::sensors::visual::CameraPropertyMode::");

//!@cond SKIPPED_DOCUMENTATION
#ifndef CEDAR_COMPILER_MSVC
  const cedar::dev::sensors::visual::CameraPropertyMode::Id cedar::dev::sensors::visual::CameraPropertyMode::BACKEND_DEFAULT;
  const cedar::dev::sensors::visual::CameraPropertyMode::Id cedar::dev::sensors::visual::CameraPropertyMode::AUTO;
  const cedar::dev::sensors::visual::CameraPropertyMode::Id cedar::dev::sensors::visual::CameraPropertyMode::MANUAL;
#ifdef CEDAR_USE_LIB_DC1394
  //const cedar::dev::sensors::visual::CameraPropertyMode::Id cedar::dev::sensors::visual::CameraPropertyMode::ON_OFF;
#endif
#endif // CEDAR_COMPILER_MSVC
//!@endcond

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

void cedar::dev::sensors::visual::CameraPropertyMode::construct()
{
  mType.type()->def(cedar::aux::Enum(
                                      cedar::dev::sensors::visual::CameraPropertyMode::BACKEND_DEFAULT,
                                      "BACKEND_DEFAULT",
                                      "Backend"
                                    ));
  mType.type()->def(cedar::aux::Enum(
                                      cedar::dev::sensors::visual::CameraPropertyMode::AUTO,
                                      "AUTO",
                                      "Automatic"
                                    ));
  mType.type()->def(cedar::aux::Enum(
                                      cedar::dev::sensors::visual::CameraPropertyMode::MANUAL,
                                      "MANUAL",
                                      "Manual"
                                    ));
#ifdef CEDAR_USE_LIB_DC1394
//  mType.type()->def(cedar::aux::Enum(
//                                      cedar::dev::sensors::visual::CameraPropertyMode::ON_OFF,
//                                      "ON_OFF",
//                                      "On/Off"
//                                    ));
#endif
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

const cedar::aux::EnumBase& cedar::dev::sensors::visual::CameraPropertyMode::type()
{
  return *cedar::dev::sensors::visual::CameraPropertyMode::mType.type();
}

const cedar::dev::sensors::visual::CameraPropertyMode::TypePtr& cedar::dev::sensors::visual::CameraPropertyMode::typePtr()
{
  return cedar::dev::sensors::visual::CameraPropertyMode::mType.type();
}
