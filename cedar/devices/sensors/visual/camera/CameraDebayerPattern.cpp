/*======================================================================================================================

    Copyright 2011, 2012 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany

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

    File:        CameraDebayerPattern.cpp

    Maintainer:  Georg Hartinger
    Email:       georg.hartinger@ini.rub.de
    Date:        2012 07 04

    Description: Implementation of the CameraDebayerPattern enum-type class

    Credits:

======================================================================================================================*/

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/devices/sensors/visual/camera/CameraDebayerPattern.h"

// SYSTEM INCLUDES


cedar::aux::EnumType<cedar::dev::sensors::visual::CameraDebayerPattern>
  cedar::dev::sensors::visual::CameraDebayerPattern::mType("cedar::dev::sensors::visual::CameraDebayerPattern::");

//!@cond SKIPPED_DOCUMENTATION
#ifndef CEDAR_COMPILER_MSVC
const cedar::dev::sensors::visual::CameraDebayerPattern::Id cedar::dev::sensors::visual::CameraDebayerPattern::NONE;
const cedar::dev::sensors::visual::CameraDebayerPattern::Id cedar::dev::sensors::visual::CameraDebayerPattern::BG_TO_BGR;
const cedar::dev::sensors::visual::CameraDebayerPattern::Id cedar::dev::sensors::visual::CameraDebayerPattern::GB_TO_BGR;
const cedar::dev::sensors::visual::CameraDebayerPattern::Id cedar::dev::sensors::visual::CameraDebayerPattern::RG_TO_BGR;
const cedar::dev::sensors::visual::CameraDebayerPattern::Id cedar::dev::sensors::visual::CameraDebayerPattern::GR_TO_BGR;
#endif // CEDAR_COMPILER_MSVC
//!@endcond


//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

void cedar::dev::sensors::visual::CameraDebayerPattern::construct()
{
  mType.type()->def(cedar::aux::Enum(
                                      cedar::dev::sensors::visual::CameraDebayerPattern::NONE,
                                      "NONE",
                                      "auto"
                                    ));

  mType.type()->def(cedar::aux::Enum(
                                      cedar::dev::sensors::visual::CameraDebayerPattern::BG_TO_BGR,
                                      "BG_TO_BGR",
                                      "BG to BGR"
                                    ));
  mType.type()->def(cedar::aux::Enum(
                                      cedar::dev::sensors::visual::CameraDebayerPattern::GB_TO_BGR,
                                      "GB_TO_BGR",
                                      "GB to BGR"
                                    ));
  mType.type()->def(cedar::aux::Enum(
                                      cedar::dev::sensors::visual::CameraDebayerPattern::RG_TO_BGR,
                                      "RG_TO_BGR",
                                      "RG to BGR"
                                    ));
  mType.type()->def(cedar::aux::Enum(
                                      cedar::dev::sensors::visual::CameraDebayerPattern::GR_TO_BGR,
                                      "GR_TO_BGR",
                                      "GR to BGR"
                                    ));
}
//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------


const cedar::aux::EnumBase& cedar::dev::sensors::visual::CameraDebayerPattern::type()
{
  return *cedar::dev::sensors::visual::CameraDebayerPattern::mType.type();
}

const cedar::dev::sensors::visual::CameraDebayerPattern::TypePtr& cedar::dev::sensors::visual::CameraDebayerPattern::typePtr()
{
  return cedar::dev::sensors::visual::CameraDebayerPattern::mType.type();
}

