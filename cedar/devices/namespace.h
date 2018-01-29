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
#ifndef Q_MOC_RUN
  #include <boost/intrusive_ptr.hpp>
#endif // Q_MOC_RUN

#include "cedar/devices/Channel.fwd.h"
#include "cedar/devices/Component.fwd.h"
#include "cedar/devices/ComponentParameter.fwd.h"
#include "cedar/devices/ComponentSlot.fwd.h"
#include "cedar/devices/KinematicChain.fwd.h"
#include "cedar/devices/Robot.fwd.h"
#include "cedar/devices/RobotManager.fwd.h"
#include "cedar/devices/Sensor.fwd.h"
#include "cedar/devices/SerialChannel.fwd.h"
#include "cedar/devices/YarpChannel.fwd.h"

namespace cedar
{
  /*!@brief Namespace for all devices classes. */
  namespace dev
  {
    //!@cond SKIPPED_DOCUMENTATION
    CEDAR_DECLARE_DEV_CLASS(Locomotion);
    CEDAR_DECLARE_DEV_CLASS(DifferentialDrive);
    CEDAR_DECLARE_DEV_CLASS(NetworkChannel);
    CEDAR_DECLARE_DEV_CLASS(Odometry);
    CEDAR_DECLARE_DEV_CLASS(SimulatedKinematicChain);
    CEDAR_DECLARE_DEV_CLASS(SimulatedCamera);

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
    CEDAR_DECLARE_DEV_CLASS(JointIndexOutOfRangeException);
    CEDAR_DECLARE_DEV_CLASS(JointNumberMismatchException);
    CEDAR_DECLARE_DEV_CLASS(IgnoreCommunicationException);
    //!@endcond
  }
}

#endif // CEDAR_DEV_NAMESPACE_H
