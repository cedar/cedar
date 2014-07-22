/*======================================================================================================================

    Copyright 2011, 2012, 2013, 2014 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany

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

    Maintainer:  Mathis Richter
    Email:       mathis.richter@ini.rub.de
    Date:        2012 04 13

    Description: Namespace file for cedar::dev.

    Credits:

======================================================================================================================*/

#ifndef CEDAR_DEV_NAMESPACE_H
#define CEDAR_DEV_NAMESPACE_H

// CEDAR INCLUDES
#include "cedar/devices/lib.h"
#include "cedar/defines.h"

// SYSTEM INCLUDES
#include <boost/intrusive_ptr.hpp>


namespace cedar
{
  /*!@brief Namespace for all devices classes. */
  namespace dev
  {
    //!@cond SKIPPED_DOCUMENTATION
    CEDAR_DECLARE_DEV_CLASS(Robot);
    CEDAR_DECLARE_DEV_CLASS(ComponentSlot);
    CEDAR_DECLARE_DEV_CLASS(Component);
    CEDAR_DECLARE_DEV_CLASS_INTRUSIVE(ComponentParameter);
    CEDAR_DECLARE_DEV_CLASS(Channel);
    CEDAR_DECLARE_DEV_CLASS(SerialChannel);
    CEDAR_DECLARE_DEV_CLASS(Locomotion);
    CEDAR_DECLARE_DEV_CLASS(DifferentialDrive);
    CEDAR_DECLARE_DEV_CLASS(KinematicChain);
    CEDAR_DECLARE_DEV_CLASS(NetworkChannel);
    CEDAR_DECLARE_DEV_CLASS(Odometry);
    CEDAR_DECLARE_DEV_CLASS(RobotManager);
    CEDAR_DECLARE_DEV_CLASS(Sensor);
    CEDAR_DECLARE_DEV_CLASS(SimulatedKinematicChain);

#ifdef CEDAR_USE_YARP
    template <typename T> class YarpChannel;
#endif

    // exceptions
    CEDAR_DECLARE_DEV_CLASS(ChannelConfigurationNotFoundException);
    CEDAR_DECLARE_DEV_CLASS(ChannelNotInstantiatedException);
    CEDAR_DECLARE_DEV_CLASS(CommunicationException);
    CEDAR_DECLARE_DEV_CLASS(InvalidComponentPathException);
    CEDAR_DECLARE_DEV_CLASS(NoComponentSelectedException);
    CEDAR_DECLARE_DEV_CLASS(NoTemplateConfigurationLoadedException);
    CEDAR_DECLARE_DEV_CLASS(NoTemplateLoadedException);
    CEDAR_DECLARE_DEV_CLASS(ResourceNotAvailableException);
    CEDAR_DECLARE_DEV_CLASS(SerialCommunicationException);
    CEDAR_DECLARE_DEV_CLASS(TemplateNotFoundException);
    CEDAR_DECLARE_DEV_CLASS(TimeoutException);
    CEDAR_DECLARE_DEV_CLASS(UnknownOperatingSystemException);
    CEDAR_DECLARE_DEV_CLASS(UnresponsiveRobotException);
    //!@endcond
  }
}

#endif // CEDAR_DEV_NAMESPACE_H
