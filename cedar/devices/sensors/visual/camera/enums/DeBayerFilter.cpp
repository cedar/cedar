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
#include "cedar/devices/sensors/visual/camera/enums/DeBayerFilter.h"

// SYSTEM INCLUDES


cedar::aux::EnumType<cedar::dev::sensors::visual::DeBayerFilter>
  cedar::dev::sensors::visual::DeBayerFilter::mType("cedar::dev::sensors::visual::DeBayerFilter::");

//!@cond SKIPPED_DOCUMENTATION
#ifndef CEDAR_COMPILER_MSVC
const cedar::dev::sensors::visual::DeBayerFilter::Id cedar::dev::sensors::visual::DeBayerFilter::NONE;
const cedar::dev::sensors::visual::DeBayerFilter::Id cedar::dev::sensors::visual::DeBayerFilter::BG_TO_BGR;
const cedar::dev::sensors::visual::DeBayerFilter::Id cedar::dev::sensors::visual::DeBayerFilter::GB_TO_BGR;
const cedar::dev::sensors::visual::DeBayerFilter::Id cedar::dev::sensors::visual::DeBayerFilter::RG_TO_BGR;
const cedar::dev::sensors::visual::DeBayerFilter::Id cedar::dev::sensors::visual::DeBayerFilter::GR_TO_BGR;
#endif // CEDAR_COMPILER_MSVC
//!@endcond


//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

void cedar::dev::sensors::visual::DeBayerFilter::construct()
{
  mType.type()->def(cedar::aux::Enum(
                                      cedar::dev::sensors::visual::DeBayerFilter::NONE,
                                      "NONE",
                                      "NONE"
                                    ));
  mType.type()->def(cedar::aux::Enum(
                                      cedar::dev::sensors::visual::DeBayerFilter::BG_TO_BGR,
                                      "BG_TO_BGR",
                                      "Blue/Green to BGR"
                                    ));
  mType.type()->def(cedar::aux::Enum(
                                      cedar::dev::sensors::visual::DeBayerFilter::GB_TO_BGR,
                                      "GB_TO_BGR",
                                      "Green/Blue to BGR"
                                    ));
  mType.type()->def(cedar::aux::Enum(
                                      cedar::dev::sensors::visual::DeBayerFilter::RG_TO_BGR,
                                      "RG_TO_BGR",
                                      "Red/Green to BGR"
                                    ));
  mType.type()->def(cedar::aux::Enum(
                                      cedar::dev::sensors::visual::DeBayerFilter::GR_TO_BGR,
                                      "GR_TO_BGR",
                                      "Green/Red to BGR"
                                    ));
}
//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------


const cedar::aux::EnumBase& cedar::dev::sensors::visual::DeBayerFilter::type()
{
  return *cedar::dev::sensors::visual::DeBayerFilter::mType.type();
}

const cedar::dev::sensors::visual::DeBayerFilter::TypePtr& cedar::dev::sensors::visual::DeBayerFilter::typePtr()
{
  return cedar::dev::sensors::visual::DeBayerFilter::mType.type();
}

