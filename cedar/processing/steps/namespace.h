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
#ifndef Q_MOC_RUN
  #include <boost/smart_ptr.hpp>
#endif


#warning Do not include this header anymore. Use the new forward declaration headers instead.

#include "cedar/processing/steps/AbsoluteValue.fwd.h"
#include "cedar/processing/steps/ChannelSplit.fwd.h"
#include "cedar/processing/steps/ColorConversion.fwd.h"
#include "cedar/processing/steps/Component.fwd.h"
#include "cedar/processing/steps/ComponentMultiply.fwd.h"
#include "cedar/processing/steps/Convolution.fwd.h"
#include "cedar/processing/steps/CoordinateTransformation.fwd.h"
#include "cedar/processing/steps/Flip.fwd.h"
#include "cedar/processing/steps/Histogram.fwd.h"
#include "cedar/processing/steps/Mask.fwd.h"
#include "cedar/processing/steps/MatrixMultiply.fwd.h"
#include "cedar/processing/steps/MatrixSlice.fwd.h"
#include "cedar/processing/steps/MatrixTypeConverter.fwd.h"
#include "cedar/processing/steps/Normalization.fwd.h"
#include "cedar/processing/steps/NormalizationType.fwd.h"
#include "cedar/processing/steps/Projection.fwd.h"
#include "cedar/processing/steps/RemoveMean.fwd.h"
#include "cedar/processing/steps/Resize.fwd.h"
#include "cedar/processing/steps/StaticGain.fwd.h"
#include "cedar/processing/steps/ShiftedMultiplication.fwd.h"
#include "cedar/processing/steps/Sum.fwd.h"
#include "cedar/processing/steps/Switch.fwd.h"
#include "cedar/processing/steps/Threshold.fwd.h"
#include "cedar/processing/steps/TransferFunction.fwd.h"
#include "cedar/processing/steps/UnitConverter.fwd.h"

#endif // CEDAR_PROC_STEPS_NAMESPACE_H
