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

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2011 10 28

    Description: Namespace file for cedar::proc::steps.

    Credits:

======================================================================================================================*/


#ifndef CEDAR_PROC_STEPS_NAMESPACE_H
#define CEDAR_PROC_STEPS_NAMESPACE_H

// LOCAL INCLUDES
#include "processing/lib.h"

// PROJECT INCLUDES

// SYSTEM INCLUDES
#include <boost/smart_ptr.hpp>


namespace cedar
{
  namespace proc
  {
    /*!@brief Namespace for processing steps. */
    namespace steps
    {
      CEDAR_DECLARE_PROC_CLASS(StaticGain);
    }
  }
}

#endif // CEDAR_PROC_STEPS_NAMESPACE_H
