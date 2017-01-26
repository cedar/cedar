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

    File:        CameraFrameRate.cpp

    Maintainer:  Georg Hartinger
    Email:       georg.hartinger@ini.rub.de
    Date:        2011 08 01

    Description: Implementation of the CameraFrameRate enum-type class

    Credits:

======================================================================================================================*/

#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/devices/sensors/camera/enums/FrameRate.h"

// SYSTEM INCLUDES

cedar::aux::EnumType<cedar::dev::sensors::camera::FrameRate>
            cedar::dev::sensors::camera::FrameRate::mType("cedar::dev::sensors::camera::Framerate::");

//!@cond SKIPPED_DOCUMENTATION
#ifndef CEDAR_COMPILER_MSVC
const cedar::dev::sensors::camera::FrameRate::Id cedar::dev::sensors::camera::FrameRate::FPS_NOT_SET;
const cedar::dev::sensors::camera::FrameRate::Id cedar::dev::sensors::camera::FrameRate::FPS_2;
const cedar::dev::sensors::camera::FrameRate::Id cedar::dev::sensors::camera::FrameRate::FPS_4;
const cedar::dev::sensors::camera::FrameRate::Id cedar::dev::sensors::camera::FrameRate::FPS_8;
const cedar::dev::sensors::camera::FrameRate::Id cedar::dev::sensors::camera::FrameRate::FPS_15;
const cedar::dev::sensors::camera::FrameRate::Id cedar::dev::sensors::camera::FrameRate::FPS_30;
const cedar::dev::sensors::camera::FrameRate::Id cedar::dev::sensors::camera::FrameRate::FPS_60;
const cedar::dev::sensors::camera::FrameRate::Id cedar::dev::sensors::camera::FrameRate::FPS_120;
const cedar::dev::sensors::camera::FrameRate::Id cedar::dev::sensors::camera::FrameRate::FPS_240;
#endif // CEDAR_COMPILER_MSVC
//!@endcond

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

void cedar::dev::sensors::camera::FrameRate::construct()
{
  mType.type()->def(cedar::aux::Enum(
                                      cedar::dev::sensors::camera::FrameRate::FPS_NOT_SET,
                                      "FRAMERATE_NOT_SET",
                                      "auto"
                                    ));

  mType.type()->def(cedar::aux::Enum(
                                      cedar::dev::sensors::camera::FrameRate::FPS_2,
                                      "FRAMERATE_1_875",
                                      "1.875 fps"
                                    ));

  mType.type()->def(cedar::aux::Enum(
                                      cedar::dev::sensors::camera::FrameRate::FPS_4,
                                      "FRAMERATE_3_75",
                                      "3.75 fps"
                                    ));
  mType.type()->def(cedar::aux::Enum(
                                      cedar::dev::sensors::camera::FrameRate::FPS_8,
                                      "FRAMERATE_7_5",
                                      "7.5 fps"
                                     ));
  mType.type()->def(cedar::aux::Enum(
                                      cedar::dev::sensors::camera::FrameRate::FPS_15,
                                      "FRAMERATE_15",
                                      "15 fps"
                                    ));
  mType.type()->def(cedar::aux::Enum(
                                      cedar::dev::sensors::camera::FrameRate::FPS_30,
                                      "FRAMERATE_30",
                                      "30 fps"
                                    ));
  mType.type()->def(cedar::aux::Enum(
                                      cedar::dev::sensors::camera::FrameRate::FPS_60,
                                      "FRAMERATE_60",
                                      "60 fps"
                                    ));
  mType.type()->def(cedar::aux::Enum(
                                      cedar::dev::sensors::camera::FrameRate::FPS_120,
                                      "FRAMERATE_120",
                                      "120 fps"
                                    ));
  mType.type()->def(cedar::aux::Enum(
                                      cedar::dev::sensors::camera::FrameRate::FPS_240,
                                      "FRAMERATE_240",
                                      "240 fps"
                                    ));
}
//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------


const cedar::aux::EnumBase& cedar::dev::sensors::camera::FrameRate::type()
{
  return *cedar::dev::sensors::camera::FrameRate::mType.type();
}

const cedar::dev::sensors::camera::FrameRate::TypePtr& cedar::dev::sensors::camera::FrameRate::typePtr()
{
  return cedar::dev::sensors::camera::FrameRate::mType.type();
}
