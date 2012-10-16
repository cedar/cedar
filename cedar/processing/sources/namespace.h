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

    File:        namespace.h

    Maintainer:  Oliver Lomp,
                 Mathis Richter,
                 Stephan Zibner
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de,
                 mathis.richter@ini.ruhr-uni-bochum.de,
                 stephan.zibner@ini.ruhr-uni-bochum.de
    Date:        2011 07 19

    Description: Namespace file for cedar::proc::source.

    Credits:

======================================================================================================================*/

#ifndef CEDAR_PROC_SOURCES_NAMESPACE_H
#define CEDAR_PROC_SOURCES_NAMESPACE_H

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/processing/lib.h"

// SYSTEM INCLUDES
#include <boost/smart_ptr.hpp>


namespace cedar
{
  namespace proc
  {
    /*!@brief Namespace for all sources in the processing framework. */
    namespace sources
    {
      //!@cond SKIPPED_DOCUMENTATION
      CEDAR_DECLARE_PROC_CLASS(Boost);
      CEDAR_DECLARE_PROC_CLASS(GrabberBase);
      CEDAR_DECLARE_PROC_CLASS(GaussInput);
      CEDAR_DECLARE_PROC_CLASS(Picture);
      CEDAR_DECLARE_PROC_CLASS(Video);

      CEDAR_DECLARE_PROC_CLASS(Camera);

#ifdef CEDAR_USE_LIB_DC1394
#endif

#ifdef CEDAR_USE_YARP
      CEDAR_DECLARE_PROC_CLASS(NetReader);
#endif      
      //!@endcond
    }
  }
}

#endif // CEDAR_PROC_SOURCES_NAMESPACE_H
