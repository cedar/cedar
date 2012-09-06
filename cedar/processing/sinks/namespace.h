/*=============================================================================

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

===============================================================================

    Institute:   Ruhr-Universitaet Bochum
                 Institut fuer Neuroinformatik

    File:        namespace.h

    Maintainer:  Jean-Stephane Jokeit
    Email:       jean-stephane.jokeit@ini.ruhr-uni-bochum.de
    Date:        Fri 11 May 2012

    Description:

    Credits:

=============================================================================*/

#ifndef CEDAR_PROC_SINKS_NAMESPACE_H
#define CEDAR_PROC_SINKS_NAMESPACE_H

#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/processing/lib.h"

// SYSTEM INCLUDES
#include <boost/smart_ptr.hpp>


namespace cedar
{
  namespace proc
  {
    /*!@brief Namespace for processing sinks. */
    namespace sinks
    {
      //!@cond SKIPPED_DOCUMENTATION
#ifdef CEDAR_USE_YARP
      CEDAR_DECLARE_PROC_CLASS(NetWriter);
#endif
      //!@endcond
    }
  }
}

#endif // CEDAR_PROC_STEPS_NAMESPACE_H
