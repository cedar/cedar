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

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2012 06 28

    Description: Namespace file for cedar::aux::annotations.

    Credits:

======================================================================================================================*/

#ifndef CEDAR_AUX_ANNOTATIONS_NAMESPACE_H
#define CEDAR_AUX_ANNOTATIONS_NAMESPACE_H

// CEDAR INCLUDES
#include "cedar/auxiliaries/lib.h"
#include "cedar/defines.h"

// SYSTEM INCLUDES
#ifndef Q_MOC_RUN
  #include <boost/smart_ptr.hpp>
  #include <boost/property_tree/ptree.hpp>
#endif
#include <opencv2/opencv.hpp>

#warning Do not include this header any more. Use the new forward declaration headers instead.

#include "cedar/auxiliaries/annotation/Annotation.fwd.h"
#include "cedar/auxiliaries/annotation/Annotatable.fwd.h"
#include "cedar/auxiliaries/annotation/ColorSpace.fwd.h"
#include "cedar/auxiliaries/annotation/Depth.fwd.h"
#include "cedar/auxiliaries/annotation/Dimensions.fwd.h"
#include "cedar/auxiliaries/annotation/DiscreteMetric.fwd.h"
#include "cedar/auxiliaries/annotation/Disparity.fwd.h"

#endif // CEDAR_AUX_ANNOTATIONS_NAMESPACE_H
