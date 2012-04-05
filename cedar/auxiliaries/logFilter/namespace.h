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
    Date:        2012 02 15

    Description: Namespace file for cedar::aux::logFilter.

    Credits:

======================================================================================================================*/

#ifndef CEDAR_AUX_LOG_FILTER_NAMESPACE_H
#define CEDAR_AUX_LOG_FILTER_NAMESPACE_H

// CEDAR INCLUDES
#include "cedar/auxiliaries/lib.h"
#include "cedar/defines.h"

// SYSTEM INCLUDES
#include <boost/smart_ptr.hpp>
#include <boost/property_tree/ptree.hpp>
#include <opencv2/opencv.hpp>


namespace cedar
{
  namespace aux
  {
    /*!@brief Namespace for all aux::logFilter classes. */
    namespace logFilter
    {
      CEDAR_DECLARE_AUX_CLASS(All);
      CEDAR_DECLARE_AUX_CLASS(Type);
    }
  }
}

#endif // CEDAR_AUX_LOG_FILTER_NAMESPACE_H
