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

    Maintainer:  Mathis Richter
    Email:       mathis.richter@ini.rub.de
    Date:        2010 11 11

    Description: Namespace file for cedar::aux::math.

    Credits:

======================================================================================================================*/

#ifndef CEDAR_AUX_MATH_NAMESPACE_H
#define CEDAR_AUX_MATH_NAMESPACE_H

// CEDAR INCLUDES
#include "cedar/defines.h"
#include "cedar/auxiliaries/Factory.h"
#include "cedar/units/Length.h"
#include "cedar/units/Time.h"
#include "cedar/units/Velocity.h"
#include "cedar/units/Acceleration.h"
#include "cedar/units/PlaneAngle.h"
#include "cedar/units/AngularVelocity.h"

// SYSTEM INCLUDES
#ifndef Q_MOC_RUN
  #include <boost/smart_ptr.hpp>
#endif

#warning Do not include this header any more. Use the new forward declaration headers instead.

#include "cedar/auxiliaries/math/Limits.fwd.h"
#include "cedar/auxiliaries/math/LimitsParameter.fwd.h"
#include "cedar/auxiliaries/math/DoubleLimitsParameter.fwd.h"
#include "cedar/auxiliaries/math/IntLimitsParameter.fwd.h"
#include "cedar/auxiliaries/math/UIntLimitsParameter.fwd.h"
#include "cedar/auxiliaries/math/AccelerationLimitsParameter.fwd.h"
#include "cedar/auxiliaries/math/AngularVelocityLimitsParameter.fwd.h"
#include "cedar/auxiliaries/math/LengthLimitsParameter.fwd.h"
#include "cedar/auxiliaries/math/PlaneAngleLimitsParameter.fwd.h"
#include "cedar/auxiliaries/math/TimeLimitsParameter.fwd.h"
#include "cedar/auxiliaries/math/VelocityLimitsParameter.fwd.h"
#include "cedar/auxiliaries/math/Sigmoid.fwd.h"
#include "cedar/auxiliaries/math/TransferFunction.fwd.h"
#include "cedar/auxiliaries/math/TransferFunctionDeclaration.fwd.h"

#endif // CEDAR_AUX_MATH_NAMESPACE_H
