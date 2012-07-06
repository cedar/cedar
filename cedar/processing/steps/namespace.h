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

#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/processing/lib.h"

// SYSTEM INCLUDES
#include <boost/smart_ptr.hpp>


namespace cedar
{
  namespace proc
  {
    /*!@brief Namespace for processing steps. */
    namespace steps
    {
      //!@cond SKIPPED_DOCUMENTATION
      CEDAR_DECLARE_PROC_CLASS(TransformationDirection);
      CEDAR_DECLARE_PROC_CLASS(TransformationType);
      CEDAR_DECLARE_PROC_CLASS(CoordinateTransformation);
      CEDAR_DECLARE_PROC_CLASS(ChannelSplit);
      CEDAR_DECLARE_PROC_CLASS(Convolution);
      CEDAR_DECLARE_PROC_CLASS(ColorConversion);
      CEDAR_DECLARE_PROC_CLASS(Projection);
      CEDAR_DECLARE_PROC_CLASS(Resize);
      CEDAR_DECLARE_PROC_CLASS(StaticGain);
      CEDAR_DECLARE_PROC_CLASS(Sum);
      CEDAR_DECLARE_PROC_CLASS(Switch);

#ifdef CEDAR_USE_YARP
      CEDAR_DECLARE_PROC_CLASS(NetWriterSink);
#endif
      //!@endcond
    }
  }
}

#endif // CEDAR_PROC_STEPS_NAMESPACE_H
