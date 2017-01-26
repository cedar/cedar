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

    File:        CameraVideoMode.cpp

    Maintainer:  Georg Hartinger
    Email:       georg.hartinger@ini.rub.de
    Date:        2011 08 01

    Description: Implementation of the CameraVideoMode enum-type class

    Credits:

======================================================================================================================*/

#include "cedar/configuration.h"   // MAKE FIREWIRE OPTIONAL

// CEDAR INCLUDES
#include "cedar/devices/sensors/camera/enums/VideoMode.h"

// SYSTEM INCLUDES


cedar::aux::EnumType<cedar::dev::sensors::camera::VideoMode>
            cedar::dev::sensors::camera::VideoMode::mType("cedar::dev::sensors::camera::VideoMode::");

//!@cond SKIPPED_DOCUMENTATION
#ifndef CEDAR_COMPILER_MSVC
//supported video modes by number
const cedar::dev::sensors::camera::VideoMode::Id cedar::dev::sensors::camera::VideoMode::MODE_NOT_SET;
const cedar::dev::sensors::camera::VideoMode::Id cedar::dev::sensors::camera::VideoMode::NUM_0 ;
const cedar::dev::sensors::camera::VideoMode::Id cedar::dev::sensors::camera::VideoMode::NUM_1 ;
const cedar::dev::sensors::camera::VideoMode::Id cedar::dev::sensors::camera::VideoMode::NUM_2 ;
const cedar::dev::sensors::camera::VideoMode::Id cedar::dev::sensors::camera::VideoMode::NUM_3 ;
const cedar::dev::sensors::camera::VideoMode::Id cedar::dev::sensors::camera::VideoMode::NUM_4 ;
const cedar::dev::sensors::camera::VideoMode::Id cedar::dev::sensors::camera::VideoMode::NUM_5 ;
const cedar::dev::sensors::camera::VideoMode::Id cedar::dev::sensors::camera::VideoMode::NUM_6 ;
const cedar::dev::sensors::camera::VideoMode::Id cedar::dev::sensors::camera::VideoMode::NUM_7 ;
const cedar::dev::sensors::camera::VideoMode::Id cedar::dev::sensors::camera::VideoMode::NUM_8 ;
const cedar::dev::sensors::camera::VideoMode::Id cedar::dev::sensors::camera::VideoMode::NUM_9 ;

#ifdef CEDAR_USE_LIB_DC1394
//firewire modes by name
const cedar::dev::sensors::camera::VideoMode::Id cedar::dev::sensors::camera::VideoMode::FW_160x120_YUV444  ;
const cedar::dev::sensors::camera::VideoMode::Id cedar::dev::sensors::camera::VideoMode::FW_320x240_YUV422  ;
const cedar::dev::sensors::camera::VideoMode::Id cedar::dev::sensors::camera::VideoMode::FW_640x480_YUV411  ;
const cedar::dev::sensors::camera::VideoMode::Id cedar::dev::sensors::camera::VideoMode::FW_640x480_YUV422  ;
const cedar::dev::sensors::camera::VideoMode::Id cedar::dev::sensors::camera::VideoMode::FW_640x480_RGB8    ;
const cedar::dev::sensors::camera::VideoMode::Id cedar::dev::sensors::camera::VideoMode::FW_640x480_MONO8   ;
const cedar::dev::sensors::camera::VideoMode::Id cedar::dev::sensors::camera::VideoMode::FW_640x480_MONO16  ;
const cedar::dev::sensors::camera::VideoMode::Id cedar::dev::sensors::camera::VideoMode::FW_800x600_YUV422  ;
const cedar::dev::sensors::camera::VideoMode::Id cedar::dev::sensors::camera::VideoMode::FW_800x600_RGB8    ;
const cedar::dev::sensors::camera::VideoMode::Id cedar::dev::sensors::camera::VideoMode::FW_800x600_MONO8   ;
const cedar::dev::sensors::camera::VideoMode::Id cedar::dev::sensors::camera::VideoMode::FW_1024x768_YUV422 ;
const cedar::dev::sensors::camera::VideoMode::Id cedar::dev::sensors::camera::VideoMode::FW_1024x768_RGB8   ;
const cedar::dev::sensors::camera::VideoMode::Id cedar::dev::sensors::camera::VideoMode::FW_1024x768_MONO8  ;
const cedar::dev::sensors::camera::VideoMode::Id cedar::dev::sensors::camera::VideoMode::FW_800x600_MONO16  ;
const cedar::dev::sensors::camera::VideoMode::Id cedar::dev::sensors::camera::VideoMode::FW_1024x768_MONO16 ;
const cedar::dev::sensors::camera::VideoMode::Id cedar::dev::sensors::camera::VideoMode::FW_1280x960_YUV422 ;
const cedar::dev::sensors::camera::VideoMode::Id cedar::dev::sensors::camera::VideoMode::FW_1280x960_RGB8   ;
const cedar::dev::sensors::camera::VideoMode::Id cedar::dev::sensors::camera::VideoMode::FW_1280x960_MONO8  ;
const cedar::dev::sensors::camera::VideoMode::Id cedar::dev::sensors::camera::VideoMode::FW_1600x1200_YUV422;
const cedar::dev::sensors::camera::VideoMode::Id cedar::dev::sensors::camera::VideoMode::FW_1600x1200_RGB8  ;
const cedar::dev::sensors::camera::VideoMode::Id cedar::dev::sensors::camera::VideoMode::FW_1600x1200_MONO8 ;
const cedar::dev::sensors::camera::VideoMode::Id cedar::dev::sensors::camera::VideoMode::FW_1280x960_MONO16 ;
const cedar::dev::sensors::camera::VideoMode::Id cedar::dev::sensors::camera::VideoMode::FW_1600x1200_MONO16;
const cedar::dev::sensors::camera::VideoMode::Id cedar::dev::sensors::camera::VideoMode::FW_EXIF ;
/*
const cedar::dev::sensors::camera::VideoMode::Id cedar::dev::sensors::camera::VideoMode::FW_FORMAT7_0 ;
const cedar::dev::sensors::camera::VideoMode::Id cedar::dev::sensors::camera::VideoMode::FW_FORMAT7_1 ;
const cedar::dev::sensors::camera::VideoMode::Id cedar::dev::sensors::camera::VideoMode::FW_FORMAT7_2 ;
const cedar::dev::sensors::camera::VideoMode::Id cedar::dev::sensors::camera::VideoMode::FW_FORMAT7_3 ;
const cedar::dev::sensors::camera::VideoMode::Id cedar::dev::sensors::camera::VideoMode::FW_FORMAT7_4 ;
const cedar::dev::sensors::camera::VideoMode::Id cedar::dev::sensors::camera::VideoMode::FW_FORMAT7_5 ;
const cedar::dev::sensors::camera::VideoMode::Id cedar::dev::sensors::camera::VideoMode::FW_FORMAT7_6 ;
const cedar::dev::sensors::camera::VideoMode::Id cedar::dev::sensors::camera::VideoMode::FW_FORMAT7_7 ;
*/
#endif //CEDAR_USE_LIB_DC1394
#endif // CEDAR_COMPILER_MSVC

//!@endcond

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

void cedar::dev::sensors::camera::VideoMode::construct()
{
  mType.type()->def(cedar::aux::Enum(
                                      cedar::dev::sensors::camera::VideoMode::MODE_NOT_SET,
                                      "MODE_NOT_SET",
                                      "auto"
                                    ));
  mType.type()->def(cedar::aux::Enum(
                                      cedar::dev::sensors::camera::VideoMode::NUM_0,
                                      "MODE_BY_NUM_0",
                                      "mode #0"
                                    ));
  mType.type()->def(cedar::aux::Enum(
                                      cedar::dev::sensors::camera::VideoMode::NUM_1,
                                      "MODE_BY_NUM_1",
                                      "mode #1"
                                    ));
  mType.type()->def(cedar::aux::Enum(
                                      cedar::dev::sensors::camera::VideoMode::NUM_2,
                                      "MODE_BY_NUM_2",
                                      "mode #2"
                                    ));
  mType.type()->def(cedar::aux::Enum(
                                      cedar::dev::sensors::camera::VideoMode::NUM_3,
                                      "MODE_BY_NUM_3",
                                      "mode #3"
                                    ));
  mType.type()->def(cedar::aux::Enum(
                                      cedar::dev::sensors::camera::VideoMode::NUM_4,
                                      "MODE_BY_NUM_4",
                                      "mode #4"
                                    ));
  mType.type()->def(cedar::aux::Enum(
                                      cedar::dev::sensors::camera::VideoMode::NUM_5,
                                      "MODE_BY_NUM_5",
                                      "mode #5"
                                    ));
  mType.type()->def(cedar::aux::Enum(
                                      cedar::dev::sensors::camera::VideoMode::NUM_6,
                                      "MODE_BY_NUM_6",
                                      "mode #6"
                                    ));
  mType.type()->def(cedar::aux::Enum(
                                      cedar::dev::sensors::camera::VideoMode::NUM_7,
                                      "MODE_BY_NUM_7",
                                      "mode #7"
                                    ));
  mType.type()->def(cedar::aux::Enum(
                                      cedar::dev::sensors::camera::VideoMode::NUM_8,
                                      "MODE_BY_NUM_8",
                                      "mode #8"
                                    ));
  mType.type()->def(cedar::aux::Enum(
                                      cedar::dev::sensors::camera::VideoMode::NUM_9,
                                      "MODE_BY_NUM_9",
                                      "mode #9"
                                    ));

#ifdef CEDAR_USE_LIB_DC1394
  mType.type()->def(cedar::aux::Enum(
                                      cedar::dev::sensors::camera::VideoMode::FW_160x120_YUV444,
                                      "MODE_FW_160x120_YUV444",
                                      "160x120_YUV444"
                                    ));
  mType.type()->def(cedar::aux::Enum(
                                      cedar::dev::sensors::camera::VideoMode::FW_320x240_YUV422,
                                      "MODE_FW_320x240_YUV422",
                                      "320x240_YUV422"
                                    ));
  mType.type()->def(cedar::aux::Enum(
                                      cedar::dev::sensors::camera::VideoMode::FW_640x480_YUV411,
                                      "MODE_FW_640x480_YUV411",
                                      "640x480_YUV411"
                                    ));
  mType.type()->def(cedar::aux::Enum(
                                      cedar::dev::sensors::camera::VideoMode::FW_640x480_YUV422,
                                      "MODE_FW_640x480_YUV422",
                                      "640x480_YUV422"
                                    ));
  mType.type()->def(cedar::aux::Enum(
                                      cedar::dev::sensors::camera::VideoMode::FW_640x480_RGB8,
                                      "MODE_FW_640x480_RGB8",
                                      "640x480_RGB8"
                                    ));
  mType.type()->def(cedar::aux::Enum(
                                      cedar::dev::sensors::camera::VideoMode::FW_640x480_MONO8,
                                      "MODE_FW_640x480_MONO8",
                                      "640x480_MONO8"
                                    ));
  mType.type()->def(cedar::aux::Enum(
                                      cedar::dev::sensors::camera::VideoMode::FW_640x480_MONO16,
                                      "MODE_FW_640x480_MONO16",
                                      "640x480_MONO16"
                                    ));
  mType.type()->def(cedar::aux::Enum(
                                      cedar::dev::sensors::camera::VideoMode::FW_800x600_YUV422,
                                      "MODE_FW_800x600_YUV422",
                                      "800x600_YUV422"
                                    ));
  mType.type()->def(cedar::aux::Enum(
                                      cedar::dev::sensors::camera::VideoMode::FW_800x600_RGB8,
                                      "MODE_FW_800x600_RGB8",
                                      "800x600_RGB8"
                                    ));
  mType.type()->def(cedar::aux::Enum(
                                      cedar::dev::sensors::camera::VideoMode::FW_800x600_MONO8,
                                      "MODE_FW_800x600_MONO8",
                                      "800x600_MONO8"
                                    ));
  mType.type()->def(cedar::aux::Enum(
                                      cedar::dev::sensors::camera::VideoMode::FW_1024x768_YUV422,
                                      "MODE_FW_1024x768_YUV422",
                                      "1024x768_YUV422"
                                    ));
  mType.type()->def(cedar::aux::Enum(
                                      cedar::dev::sensors::camera::VideoMode::FW_1024x768_RGB8,
                                      "MODE_FW_1024x768_RGB8",
                                      "1024x768_RGB8"
                                    ));

  mType.type()->def(cedar::aux::Enum(
                                      cedar::dev::sensors::camera::VideoMode::FW_1024x768_MONO8,
                                      "MODE_FW_1024x768_MONO8",
                                      "1024x768_MONO8"
                                    ));
  mType.type()->def(cedar::aux::Enum(
                                      cedar::dev::sensors::camera::VideoMode::FW_800x600_MONO16,
                                      "MODE_FW_800x600_MONO16",
                                      "800x600_MONO16"
                                    ));
  mType.type()->def(cedar::aux::Enum(
                                      cedar::dev::sensors::camera::VideoMode::FW_1024x768_MONO16,
                                      "MODE_FW_1024x768_MONO16",
                                      "1024x768_MONO16"
                                    ));
  mType.type()->def(cedar::aux::Enum(
                                      cedar::dev::sensors::camera::VideoMode::FW_1280x960_YUV422,
                                      "MODE_FW_1280x960_YUV422",
                                      "1280x960_YUV422"
                                    ));
  mType.type()->def(cedar::aux::Enum(
                                      cedar::dev::sensors::camera::VideoMode::FW_1280x960_RGB8,
                                      "MODE_FW_1280x960_RGB8",
                                      "1280x960_RGB8"
                                    ));
  mType.type()->def(cedar::aux::Enum(
                                      cedar::dev::sensors::camera::VideoMode::FW_1280x960_MONO8,
                                      "MODE_FW_1280x960_MONO8",
                                      "1280x960_MONO8"
                                    ));
  mType.type()->def(cedar::aux::Enum(
                                      cedar::dev::sensors::camera::VideoMode::FW_1600x1200_YUV422,
                                      "MODE_FW_1600x1200_YUV422",
                                      "1600x1200_YUV422"
                                    ));
  mType.type()->def(cedar::aux::Enum(
                                      cedar::dev::sensors::camera::VideoMode::FW_1600x1200_RGB8,
                                      "MODE_FW_1600x1200_RGB8",
                                      "1600x1200_RGB8"
                                    ));

  mType.type()->def(cedar::aux::Enum(
                                      cedar::dev::sensors::camera::VideoMode::FW_1600x1200_MONO8,
                                      "MODE_FW_1600x1200_MONO8",
                                      "1600x1200_MONO8"
                                    ));
  mType.type()->def(cedar::aux::Enum(
                                      cedar::dev::sensors::camera::VideoMode::FW_1280x960_MONO16,
                                      "MODE_FW_1280x960_MONO16",
                                      "1280x960_MONO16"
                                    ));
  mType.type()->def(cedar::aux::Enum(
                                      cedar::dev::sensors::camera::VideoMode::FW_1600x1200_MONO16,
                                      "MODE_FW_1600x1200_MONO16",
                                      "1600x1200_MONO16"
                                    ));
  mType.type()->def(cedar::aux::Enum(
                                      cedar::dev::sensors::camera::VideoMode::FW_EXIF,
                                      "MODE_FW_EXIF",
                                      "EXIF"
                                    ));
  /*
  mType.type()->def(cedar::aux::Enum(
                                      cedar::dev::sensors::camera::VideoMode::FW_FORMAT7_0,
                                      "MODE_FW_FORMAT7_0",
                                      "MODE_FW_FORMAT7_0"
                                    ));
  mType.type()->def(cedar::aux::Enum(
                                      cedar::dev::sensors::camera::VideoMode::FW_FORMAT7_1,
                                      "MODE_FW_FORMAT7_1",
                                      "MODE_FW_FORMAT7_1"
                                    ));
  mType.type()->def(cedar::aux::Enum(
                                      cedar::dev::sensors::camera::VideoMode::FW_FORMAT7_2,
                                      "MODE_FW_FORMAT7_2",
                                      "MODE_FW_FORMAT7_2"
                                    ));
  mType.type()->def(cedar::aux::Enum(
                                      cedar::dev::sensors::camera::VideoMode::FW_FORMAT7_3,
                                      "MODE_FW_FORMAT7_3",
                                      "MODE_FW_FORMAT7_3"
                                    ));
  mType.type()->def(cedar::aux::Enum(
                                      cedar::dev::sensors::camera::VideoMode::FW_FORMAT7_4,
                                      "MODE_FW_FORMAT7_4",
                                      "MODE_FW_FORMAT7_4"
                                    ));
  mType.type()->def(cedar::aux::Enum(
                                      cedar::dev::sensors::camera::VideoMode::FW_FORMAT7_5,
                                      "MODE_FW_FORMAT7_5",
                                      "MODE_FW_FORMAT7_5"
                                    ));
  mType.type()->def(cedar::aux::Enum(
                                      cedar::dev::sensors::camera::VideoMode::FW_FORMAT7_6,
                                      "MODE_FW_FORMAT7_6",
                                      "MODE_FW_FORMAT7_6"
                                    ));
  mType.type()->def(cedar::aux::Enum(
                                      cedar::dev::sensors::camera::VideoMode::FW_FORMAT7_7,
                                      "MODE_FW_FORMAT7_7",
                                      "MODE_FW_FORMAT7_7"
                                    ));
*/
#endif // CEDAR_USE_LIB_DC1394
}
//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------


const cedar::aux::EnumBase& cedar::dev::sensors::camera::VideoMode::type()
{
  return *cedar::dev::sensors::camera::VideoMode::mType.type();
}

const cedar::dev::sensors::camera::VideoMode::TypePtr& cedar::dev::sensors::camera::VideoMode::typePtr()
{
  return cedar::dev::sensors::camera::VideoMode::mType.type();
}
