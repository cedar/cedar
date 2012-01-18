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

    File:        CameraFrameRate.cpp

    Maintainer:  Georg Hartinger
    Email:       georg.hartinger@ini.rub.de
    Date:        2011 08 01

    Description: Implementation of the CameraFrameRate enum-type class

    Credits:

======================================================================================================================*/

// LOCAL INCLUDES
#include "CameraFrameRate.h"

// PROJECT INCLUDES

// SYSTEM INCLUDES


cedar::aux::EnumType<cedar::dev::sensors::visual::CameraFrameRate>
            cedar::dev::sensors::visual::CameraFrameRate::mType("cedar::dev::sensors::visual::CameraFramerate::");

#ifndef MSVC
const cedar::dev::sensors::visual::CameraFrameRate::Id cedar::dev::sensors::visual::CameraFrameRate::FRAMERATE_1_875;
const cedar::dev::sensors::visual::CameraFrameRate::Id cedar::dev::sensors::visual::CameraFrameRate::FRAMERATE_3_75;
const cedar::dev::sensors::visual::CameraFrameRate::Id cedar::dev::sensors::visual::CameraFrameRate::FRAMERATE_7_5;
const cedar::dev::sensors::visual::CameraFrameRate::Id cedar::dev::sensors::visual::CameraFrameRate::FRAMERATE_15;
const cedar::dev::sensors::visual::CameraFrameRate::Id cedar::dev::sensors::visual::CameraFrameRate::FRAMERATE_30;
const cedar::dev::sensors::visual::CameraFrameRate::Id cedar::dev::sensors::visual::CameraFrameRate::FRAMERATE_60;
const cedar::dev::sensors::visual::CameraFrameRate::Id cedar::dev::sensors::visual::CameraFrameRate::FRAMERATE_120;
const cedar::dev::sensors::visual::CameraFrameRate::Id cedar::dev::sensors::visual::CameraFrameRate::FRAMERATE_240;
#endif // MSVC


//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

void cedar::dev::sensors::visual::CameraFrameRate::construct()
{
  mType.type()->def(cedar::aux::Enum(
                                      cedar::dev::sensors::visual::CameraFrameRate::FRAMERATE_1_875,
                                      "FRAMERATE_1_875",
                                      "Grab with 1.875 frames per second"
                                    ));

  mType.type()->def(cedar::aux::Enum(
                                      cedar::dev::sensors::visual::CameraFrameRate::FRAMERATE_3_75,
                                      "FRAMERATE_3_75",
                                      "Grab with 3.75 frames per second"
                                    ));
  mType.type()->def(cedar::aux::Enum(
                                      cedar::dev::sensors::visual::CameraFrameRate::FRAMERATE_7_5,
                                      "FRAMERATE_7_5",
                                      "Grab with 7.5 frames per second"
                                    ));
  mType.type()->def(cedar::aux::Enum(
                                      cedar::dev::sensors::visual::CameraFrameRate::FRAMERATE_15,
                                      "FRAMERATE_15",
                                      "Grab with 15 frames per second"
                                    ));
  mType.type()->def(cedar::aux::Enum(
                                      cedar::dev::sensors::visual::CameraFrameRate::FRAMERATE_30,
                                      "FRAMERATE_30",
                                      "Grab with 30 frames per second"
                                    ));
  mType.type()->def(cedar::aux::Enum(
                                      cedar::dev::sensors::visual::CameraFrameRate::FRAMERATE_60,
                                      "FRAMERATE_60",
                                      "Grab with 60 frames per second"
                                    ));
  mType.type()->def(cedar::aux::Enum(
                                      cedar::dev::sensors::visual::CameraFrameRate::FRAMERATE_120,
                                      "FRAMERATE_120",
                                      "Grab with 120 frames per second"
                                    ));
  mType.type()->def(cedar::aux::Enum(
                                      cedar::dev::sensors::visual::CameraFrameRate::FRAMERATE_240,
                                      "FRAMERATE_240",
                                      "Grab with 240 frames per second"
                                    ));
}
//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------


const cedar::aux::EnumBase& cedar::dev::sensors::visual::CameraFrameRate::type()
{
  return *cedar::dev::sensors::visual::CameraFrameRate::mType.type();
}

const cedar::dev::sensors::visual::CameraFrameRate::TypePtr& cedar::dev::sensors::visual::CameraFrameRate::typePtr()
{
  return cedar::dev::sensors::visual::CameraFrameRate::mType.type();
}
