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

    Maintainer:  Stephan Zibner
    Email:       stephan.zibner@ini.rub.de
    Date:        2011 07 14

    Description: Namespace file for cedar::aux::kernel.

    Credits:

======================================================================================================================*/

#ifndef CEDAR_AUX_KERNEL_NAMESPACE_H
#define CEDAR_AUX_KERNEL_NAMESPACE_H

// CEDAR INCLUDES
#include "cedar/auxiliaries/namespace.h"
#include "cedar/auxiliaries/Singleton.h"
#include "cedar/defines.h"

// SYSTEM INCLUDES
#include <boost/smart_ptr.hpp>

namespace cedar
{
  /*!@brief Namespace for all aux classes. */
  namespace aux
  {
  /*!@brief Namespace for all kernel classes. */
    namespace kernel
    {
      //!@cond SKIPPED_DOCUMENTATION
      CEDAR_DECLARE_AUX_CLASS(Box);
      CEDAR_DECLARE_AUX_CLASS(Kernel);
      CEDAR_DECLARE_AUX_CLASS(Separable);
      CEDAR_DECLARE_AUX_CLASS(Gauss);
      //!@endcond

      //!@brief The factory manager for kernel classes.
      typedef cedar::aux::FactoryManager<cedar::aux::kernel::KernelPtr> FactoryManager;

      //!@brief The singleton instance of the kernel factory manager.
      typedef cedar::aux::Singleton<FactoryManager> FactoryManagerSingleton;
    }
  }
}

#endif // CEDAR_AUX_KERNEL_NAMESPACE_H
