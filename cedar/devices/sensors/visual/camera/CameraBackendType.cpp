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

    File:        CameraBackendType.cpp

    Maintainer:  Georg Hartinger
    Email:       georg.hartinger@ini.rub.de
    Date:        2012 07 04

    Description: Implementation of the CameraBackendType enum-type class

    Credits:

======================================================================================================================*/

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/devices/sensors/visual/camera/CameraBackendType.h"

// SYSTEM INCLUDES


cedar::aux::EnumType<cedar::dev::sensors::visual::CameraBackendType>
            cedar::dev::sensors::visual::CameraBackendType::mType("cedar::dev::sensors::visual::CameraBackendType::");

//!@cond SKIPPED_DOCUMENTATION
#ifndef CEDAR_COMPILER_MSVC
const cedar::dev::sensors::visual::CameraBackendType::Id cedar::dev::sensors::visual::CameraBackendType::AUTO;
const cedar::dev::sensors::visual::CameraBackendType::Id cedar::dev::sensors::visual::CameraBackendType::CVCAPTURE;

#ifdef CEDAR_USE_LIB_DC1394
const cedar::dev::sensors::visual::CameraBackendType::Id cedar::dev::sensors::visual::CameraBackendType::DC1394;
#endif

const cedar::dev::sensors::visual::CameraBackendType::Id cedar::dev::sensors::visual::CameraBackendType::VFL;
#endif // CEDAR_COMPILER_MSVC
//!@endcond

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

void cedar::dev::sensors::visual::CameraBackendType::construct()
{
  mType.type()->def(cedar::aux::Enum(
                                      cedar::dev::sensors::visual::CameraBackendType::AUTO,
                                      "BACKEND_AUTO",
                                      "auto"
                                    ));

  mType.type()->def(cedar::aux::Enum(
                                      cedar::dev::sensors::visual::CameraBackendType::CVCAPTURE,
                                      "CVCAPTURE",
                                      "cv::VideoCapture"
                                    ));
#ifdef CEDAR_USE_LIB_DC1394
  mType.type()->def(cedar::aux::Enum(
                                      cedar::dev::sensors::visual::CameraBackendType::DC1394,
                                      "BACKEND_DC1394",
                                      "DC1394 (firewire)"
                                    ));
#endif

  mType.type()->def(cedar::aux::Enum(
                                      cedar::dev::sensors::visual::CameraBackendType::VFL,
                                      "VFL",
                                      "VFL (video for linux)"
                                    ));
}
//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------


const cedar::aux::EnumBase& cedar::dev::sensors::visual::CameraBackendType::type()
{
  return *cedar::dev::sensors::visual::CameraBackendType::mType.type();
}

const cedar::dev::sensors::visual::CameraBackendType::TypePtr& cedar::dev::sensors::visual::CameraBackendType::typePtr()
{
  return cedar::dev::sensors::visual::CameraBackendType::mType.type();
}

