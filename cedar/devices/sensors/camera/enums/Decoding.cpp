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

    File:        DeBayerFilter.cpp

    Maintainer:  Georg Hartinger
    Email:       georg.hartinger@ini.rub.de
    Date:        2012 07 04

    Description: Implementation of the DeBayerFilter enum-type class

    Credits:

======================================================================================================================*/

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/devices/sensors/camera/enums/Decoding.h"

// SYSTEM INCLUDES


cedar::aux::EnumType<cedar::dev::sensors::camera::Decoding>
  cedar::dev::sensors::camera::Decoding::mType("cedar::dev::sensors::camera::Decoding::");

//!@cond SKIPPED_DOCUMENTATION
#ifndef CEDAR_COMPILER_MSVC
const cedar::dev::sensors::camera::Decoding::Id cedar::dev::sensors::camera::Decoding::NONE;
const cedar::dev::sensors::camera::Decoding::Id cedar::dev::sensors::camera::Decoding::FROM_BG;
const cedar::dev::sensors::camera::Decoding::Id cedar::dev::sensors::camera::Decoding::FROM_GB;
const cedar::dev::sensors::camera::Decoding::Id cedar::dev::sensors::camera::Decoding::FROM_RG;
const cedar::dev::sensors::camera::Decoding::Id cedar::dev::sensors::camera::Decoding::FROM_GR;
#endif // CEDAR_COMPILER_MSVC
//!@endcond


//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

void cedar::dev::sensors::camera::Decoding::construct()
{
  mType.type()->def(cedar::aux::Enum(
                                      cedar::dev::sensors::camera::Decoding::NONE,
                                      "NONE",
                                      "NONE"
                                    ));
  mType.type()->def(cedar::aux::Enum(
                                      cedar::dev::sensors::camera::Decoding::FROM_BG,
                                      "BG_TO_BGR",
                                      "from blue/green"
                                    ));
  mType.type()->def(cedar::aux::Enum(
                                      cedar::dev::sensors::camera::Decoding::FROM_GB,
                                      "GB_TO_BGR",
                                      "from green/blue"
                                    ));
  mType.type()->def(cedar::aux::Enum(
                                      cedar::dev::sensors::camera::Decoding::FROM_RG,
                                      "RG_TO_BGR",
                                      "from red/green"
                                    ));
  mType.type()->def(cedar::aux::Enum(
                                      cedar::dev::sensors::camera::Decoding::FROM_GR,
                                      "GR_TO_BGR",
                                      "from green/red"
                                    ));
}
//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------


const cedar::aux::EnumBase& cedar::dev::sensors::camera::Decoding::type()
{
  return *cedar::dev::sensors::camera::Decoding::mType.type();
}

const cedar::dev::sensors::camera::Decoding::TypePtr& cedar::dev::sensors::camera::Decoding::typePtr()
{
  return cedar::dev::sensors::camera::Decoding::mType.type();
}

