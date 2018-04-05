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
#ifndef Q_MOC_RUN
  #include <boost/smart_ptr.hpp>
#endif

#warning Do not include this header any more. Use the new forward declaration headers instead.

#include "cedar/processing/sources/Boost.fwd.h"
#include "cedar/processing/sources/BoxInput.fwd.h"
#include "cedar/processing/sources/Camera.fwd.h"
#include "cedar/processing/sources/GaussInput.fwd.h"
#include "cedar/processing/sources/GrabberBase.fwd.h"
#include "cedar/processing/sources/NetReader.fwd.h"
#include "cedar/processing/sources/Noise.fwd.h"
#include "cedar/processing/sources/Picture.fwd.h"
#include "cedar/processing/sources/SpatialTemplate.fwd.h"
#include "cedar/processing/sources/Video.fwd.h"

#endif // CEDAR_PROC_SOURCES_NAMESPACE_H
