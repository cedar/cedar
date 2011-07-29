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

    Maintainer:  Andre Bartel
    Email:       andre.bartel@ini.ruhr-uni-bochum.de
    Date:        2011 03 19

    Description: Namespace file for cedar::dev::robot::mobile.

    Credits:

======================================================================================================================*/


#ifndef CEDAR_DEV_ROBOT_MOBILE_NAMESPACE_H
#define CEDAR_DEV_ROBOT_MOBILE_NAMESPACE_H

// LOCAL INCLUDES
#include "auxiliaries/macros.h"

// PROJECT INCLUDES

// SYSTEM INCLUDES
#include <boost/smart_ptr.hpp>

namespace cedar
{
  namespace dev
  {
    namespace robot
    {
      namespace mobile
      {
        CEDAR_DECLARE_CLASS(MobileRobot);

        CEDAR_DECLARE_CLASS(DifferentialDrive);

        CEDAR_DECLARE_CLASS(KTeamDrive);

        CEDAR_DECLARE_CLASS(EPuckDrive);

        CEDAR_DECLARE_CLASS(MobileRobotModel);

        CEDAR_DECLARE_CLASS(KTeamDriveModel);

        CEDAR_DECLARE_CLASS(KTeamPositionController);
      }
    }
  }
}

#endif // CEDAR_DEV_ROBOT_MOBILE_NAMESPACE_H
