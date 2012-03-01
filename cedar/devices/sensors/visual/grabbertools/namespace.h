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

    File:        namespace.h

    Maintainer:  Georg.Hartinger
    Email:       georg.hartinger@ini.rub.de
    Date:        2012 02 17

    Description: namespace for grabbertools

    Credits:

======================================================================================================================*/

#ifndef CEDAR_DEV_SENSORS_VISUAL_GRABBERTOOLS_H
#define CEDAR_DEV_SENSORS_VISUAL_GRABBERTOOLS_H

// MAKE FIREWIRE OPTIONAL
#include "cedar/devices/robot/CMakeDefines.h"
#ifdef CEDAR_USE_LIB_DC1394

// LOCAL INCLUDES
//#include "cedar/devices/sensors/visual/namespace.h"
// PROJECT INCLUDES

// SYSTEM INCLUDES

namespace cedar
{
  namespace dev
  {
    namespace sensors
    {
      namespace visual
      {
        namespace grabbertools
        {
          //! @brief The common interface for direct interface to the firewire bus
          class FirewireInterface;

        }
      }
    }
  }
}
#endif // CEDAR_USE_LIB_DC1394

#endif // CEDAR_DEV_SENSORS_VISUAL_GRABBERTOOLS_H

