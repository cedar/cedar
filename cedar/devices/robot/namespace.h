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

    Maintainer:  Mathis Richter
    Email:       mathis.richter@ini.rub.de
    Date:        2010 08 30

    Description: Namespace file for cedar::dev::robot.

    Credits:

======================================================================================================================*/

#ifndef CEDAR_DEV_ROBOT_NAMESPACE_H
#define CEDAR_DEV_ROBOT_NAMESPACE_H

// LOCAL INCLUDES

// PROJECT INCLUDES

// SYSTEM INCLUDES
#include <boost/smart_ptr.hpp>

namespace cedar
{
  namespace dev
  {
    /*! @brief Namespace for all robot interfaces. */
    namespace robot
    {
      class Component;
      //!@brief smart pointer for Component
      typedef boost::shared_ptr<Component> ComponentPtr;

      class Robot;
      //!@brief smart pointer for Robot
      typedef boost::shared_ptr<Robot> RobotPtr;

      class KinematicChain;
      //!@brief smart pointer for KinematicChain
      typedef boost::shared_ptr<KinematicChain> KinematicChainPtr;

      class KinematicChainModel;
      //!@brief smart pointer for KinematicChainModel
      typedef boost::shared_ptr<KinematicChainModel> KinematicChainModelPtr;

#ifdef CEDAR_USE_AMTEC
      class AmtecKinematicChain;
      //!@brief smart pointer for AmtecKinematicChainModel
      typedef boost::shared_ptr<AmtecKinematicChain> AmtecKinematicChainPtr;
#endif // CEDAR_USE_AMTEC

      class SimulatedKinematicChain;
      //!@brief smart pointer for SimulatedKinematicChainModel
      typedef boost::shared_ptr<SimulatedKinematicChain> SimulatedKinematicChainPtr;
      
      class ReferenceGeometry;
      //!@brief smart pointer for ReferenceGeometry
      typedef boost::shared_ptr<ReferenceGeometry> ReferenceGeometryPtr;

      class ComponentNotAvailableException;
      //!@brief smart pointer for ComponentNotAvailableException
      typedef boost::shared_ptr<ComponentNotAvailableException> ComponentNotAvailableExceptionPtr;
    }
  }
}

#endif // CEDAR_DEV_ROBOT_NAMESPACE_H
