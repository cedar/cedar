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

    File:        CameraIsoSpeed.cpp

    Maintainer:  Georg Hartinger
    Email:       georg.hartinger@ini.rub.de
    Date:        2011 08 01

    Description: Implementation of the CameraIsoSpeed enum-type class

    Credits:

======================================================================================================================*/


#include "cedar/configuration.h"   // MAKE FIREWIRE OPTIONAL
#ifdef CEDAR_USE_LIB_DC1394

// CEDAR INCLUDES
#include "cedar/devices/sensors/visual/camera/CameraIsoSpeed.h"

// SYSTEM INCLUDES


cedar::aux::EnumType<cedar::dev::sensors::visual::CameraIsoSpeed> 
            cedar::dev::sensors::visual::CameraIsoSpeed::mType("cedar::dev::sensors::visual::CameraIsoSpeed::");
            
//!@cond SKIPPED_DOCUMENTATION
#ifndef CEDAR_COMPILER_MSVC
  const cedar::dev::sensors::visual::CameraIsoSpeed::Id cedar::dev::sensors::visual::CameraIsoSpeed::ISO_100;
  const cedar::dev::sensors::visual::CameraIsoSpeed::Id cedar::dev::sensors::visual::CameraIsoSpeed::ISO_200;
  const cedar::dev::sensors::visual::CameraIsoSpeed::Id cedar::dev::sensors::visual::CameraIsoSpeed::ISO_400;
  const cedar::dev::sensors::visual::CameraIsoSpeed::Id cedar::dev::sensors::visual::CameraIsoSpeed::ISO_800;
  const cedar::dev::sensors::visual::CameraIsoSpeed::Id cedar::dev::sensors::visual::CameraIsoSpeed::ISO_1600;
  const cedar::dev::sensors::visual::CameraIsoSpeed::Id cedar::dev::sensors::visual::CameraIsoSpeed::ISO_3200;
  const cedar::dev::sensors::visual::CameraIsoSpeed::Id cedar::dev::sensors::visual::CameraIsoSpeed::ISO_NOT_SET;
#endif // CEDAR_COMPILER_MSVC
//!@endcond

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

void cedar::dev::sensors::visual::CameraIsoSpeed::construct()
{
  mType.type()->def(cedar::aux::Enum(
                                      cedar::dev::sensors::visual::CameraIsoSpeed::ISO_NOT_SET,
                                      "ISO_NOT_SET",
                                      "Bus speed set to automatic"
                                    ));

  mType.type()->def(cedar::aux::Enum(
                                      cedar::dev::sensors::visual::CameraIsoSpeed::ISO_100,
                                      "ISO_100",
                                      "Bus speed 100 Mbit/s"
                                    ));
                                    
  mType.type()->def(cedar::aux::Enum(
                                      cedar::dev::sensors::visual::CameraIsoSpeed::ISO_200,
                                      "ISO_200",
                                      "Bus speed 200 Mbit/s"
                                    ));
  mType.type()->def(cedar::aux::Enum(
                                      cedar::dev::sensors::visual::CameraIsoSpeed::ISO_400,
                                      "ISO_400",
                                      "Bus speed 400 Mbit/s"
                                    ));
  mType.type()->def(cedar::aux::Enum(
                                      cedar::dev::sensors::visual::CameraIsoSpeed::ISO_800,
                                      "ISO_800",
                                      "Bus speed 800 Mbit/s"
                                    ));
  mType.type()->def(cedar::aux::Enum(
                                      cedar::dev::sensors::visual::CameraIsoSpeed::ISO_1600,
                                      "ISO_1600",
                                      "Bus speed 1600 Mbit/s"
                                    ));
  mType.type()->def(cedar::aux::Enum(
                                      cedar::dev::sensors::visual::CameraIsoSpeed::ISO_3200,
                                      "ISO_3200",
                                      "Bus speed 3200 Mbit/s"
                                    ));

}
//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------


const cedar::aux::EnumBase& cedar::dev::sensors::visual::CameraIsoSpeed::type()
{
  return *cedar::dev::sensors::visual::CameraIsoSpeed::mType.type();
}

const cedar::dev::sensors::visual::CameraIsoSpeed::TypePtr& cedar::dev::sensors::visual::CameraIsoSpeed::typePtr()
{
  return cedar::dev::sensors::visual::CameraIsoSpeed::mType.type();
}
#endif // CEDAR_USE_LIB_DC1394
