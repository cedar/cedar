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

    Maintainer:  Guido Knips
    Email:       guido.knips@ini.rub.de
    Date:        2010 12 9

    Description: Namespace file for cedar::dev::robot::kuka.

    Credits:

======================================================================================================================*/


#ifndef CEDAR_DEV_ROBOT_KUKA_NAMESPACE_H
#define CEDAR_DEV_ROBOT_KUKA_NAMESPACE_H

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
      namespace kuka
      {
        class KukaInterface;
        typedef boost::shared_ptr<KukaInterface> KukaInterfacePtr;
      }
    }
  }
}

#endif // CEDAR_DEV_ROBOT_KUKA_NAMESPACE_H
