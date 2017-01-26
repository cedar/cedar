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

    File:        RecordingFormat.cpp

    Maintainer:  Georg Hartinger
    Email:       georg.hartinger@ini.rub.de
    Date:        2012 07 04

    Description: Implementation of the RecordingFormat enum-type class

    Credits:

======================================================================================================================*/

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/devices/sensors/visual/RecordingFormat.h"

// SYSTEM INCLUDES


cedar::aux::EnumType<cedar::dev::sensors::visual::RecordingFormat>
  cedar::dev::sensors::visual::RecordingFormat::mType("cedar::dev::sensors::visual::RecordingFormat::");

//!@cond SKIPPED_DOCUMENTATION
#ifndef CEDAR_COMPILER_MSVC
const cedar::dev::sensors::visual::RecordingFormat::Id cedar::dev::sensors::visual::RecordingFormat::RECORD_RAW;
const cedar::dev::sensors::visual::RecordingFormat::Id cedar::dev::sensors::visual::RecordingFormat::RECORD_MP42;
const cedar::dev::sensors::visual::RecordingFormat::Id cedar::dev::sensors::visual::RecordingFormat::RECORD_MJPG;
const cedar::dev::sensors::visual::RecordingFormat::Id cedar::dev::sensors::visual::RecordingFormat::RECORD_MPEG2;
const cedar::dev::sensors::visual::RecordingFormat::Id cedar::dev::sensors::visual::RecordingFormat::RECORD_H264;
const cedar::dev::sensors::visual::RecordingFormat::Id cedar::dev::sensors::visual::RecordingFormat::RECORD_H263;
const cedar::dev::sensors::visual::RecordingFormat::Id cedar::dev::sensors::visual::RecordingFormat::RECORD_VP3;
#endif // CEDAR_COMPILER_MSVC
//!@endcond


//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

void cedar::dev::sensors::visual::RecordingFormat::construct()
{
  mType.type()->def(cedar::aux::Enum(
                                      cedar::dev::sensors::visual::RecordingFormat::RECORD_RAW,
                                      "RECORD_RAW",
                                      "raw"
                                    ));

  mType.type()->def(cedar::aux::Enum(
                                      cedar::dev::sensors::visual::RecordingFormat::RECORD_MP42,
                                      "RECORD_MP42",
                                      "mp42"
                                    ));
  mType.type()->def(cedar::aux::Enum(
                                      cedar::dev::sensors::visual::RecordingFormat::RECORD_MJPG,
                                      "RECORD_MJPG",
                                      "mjpg"
                                    ));
  mType.type()->def(cedar::aux::Enum(
                                      cedar::dev::sensors::visual::RecordingFormat::RECORD_MPEG2,
                                      "RECORD_MPEG2",
                                      "mpeg2"
                                    ));
  mType.type()->def(cedar::aux::Enum(
                                      cedar::dev::sensors::visual::RecordingFormat::RECORD_H264,
                                      "RECORD_H264",
                                      "h264"
                                    ));
  mType.type()->def(cedar::aux::Enum(
                                      cedar::dev::sensors::visual::RecordingFormat::RECORD_H263,
                                      "RECORD_H263",
                                      "h263"
                                    ));
  mType.type()->def(cedar::aux::Enum(
                                      cedar::dev::sensors::visual::RecordingFormat::RECORD_VP3,
                                      "RECORD_VP3",
                                      "VP3"
                                    ));
}
//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------


const cedar::aux::EnumBase& cedar::dev::sensors::visual::RecordingFormat::type()
{
  return *cedar::dev::sensors::visual::RecordingFormat::mType.type();
}

const cedar::dev::sensors::visual::RecordingFormat::TypePtr& cedar::dev::sensors::visual::RecordingFormat::typePtr()
{
  return cedar::dev::sensors::visual::RecordingFormat::mType.type();
}

