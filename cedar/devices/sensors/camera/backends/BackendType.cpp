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

    File:        ackendType.cpp

    Maintainer:  Georg Hartinger
    Email:       georg.hartinger@ini.rub.de
    Date:        2012 07 04

    Description: Implementation of the BackendType enum-type class

    Credits:

======================================================================================================================*/

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/devices/sensors/camera/backends/BackendType.h"

// SYSTEM INCLUDES


cedar::aux::EnumType<cedar::dev::sensors::camera::BackendType>
            cedar::dev::sensors::camera::BackendType::mType("cedar::dev::sensors::camera::BackendType::");

//!@cond SKIPPED_DOCUMENTATION
#ifndef CEDAR_COMPILER_MSVC
const cedar::dev::sensors::camera::BackendType::Id cedar::dev::sensors::camera::BackendType::AUTO;
const cedar::dev::sensors::camera::BackendType::Id cedar::dev::sensors::camera::BackendType::CVCAPTURE;

#ifdef CEDAR_USE_LIB_DC1394
const cedar::dev::sensors::camera::BackendType::Id cedar::dev::sensors::camera::BackendType::DC1394;
#endif //CEDAR_USE_LIB_DC1394

#ifdef CEDAR_USE_VIDEO_FOR_LINUX
const cedar::dev::sensors::camera::BackendType::Id cedar::dev::sensors::camera::BackendType::VFL;
#endif // CEDAR_USE_VIDEO_FOR_LINUX


#endif // CEDAR_COMPILER_MSVC
//!@endcond

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

void cedar::dev::sensors::camera::BackendType::construct()
{
  mType.type()->def(cedar::aux::Enum(
                                      cedar::dev::sensors::camera::BackendType::AUTO,
                                      "AUTO",
                                      "auto"
                                    ));

  mType.type()->def(cedar::aux::Enum(
                                      cedar::dev::sensors::camera::BackendType::CVCAPTURE,
                                      "CVCAPTURE",
                                      "OpenCV (cv::VideoCapture)"
                                    ));
#ifdef CEDAR_USE_LIB_DC1394
  mType.type()->def(cedar::aux::Enum(
                                      cedar::dev::sensors::camera::BackendType::DC1394,
                                      "DC1394",
                                      "DC1394 (libdc1394, Firewire)"
                                    ));
#endif //CEDAR_USE_LIB_DC1394

#ifdef CEDAR_USE_VIDEO_FOR_LINUX
  mType.type()->def(cedar::aux::Enum(
                                      cedar::dev::sensors::camera::BackendType::VFL,
                                      "V4L",
                                      "V4L (Video for Linux)"
                                    ));
#endif // CEDAR_USE_VIDEO_FOR_LINUX

}
//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------


const cedar::aux::EnumBase& cedar::dev::sensors::camera::BackendType::type()
{
  return *cedar::dev::sensors::camera::BackendType::mType.type();
}

const cedar::dev::sensors::camera::BackendType::TypePtr& cedar::dev::sensors::camera::BackendType::typePtr()
{
  return cedar::dev::sensors::camera::BackendType::mType.type();
}

