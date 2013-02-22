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

    File:        TimeParameterTemplate.h

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2013 02 14

    Description:

    Credits:

======================================================================================================================*/

#ifndef CEDAR_AUX_ACCELERATION_PARAMETER_H
#define CEDAR_AUX_ACCELERATION_PARAMETER_H

// CEDAR INCLUDES
#include "cedar/auxiliaries/UnitParameterTemplate.h"

// SYSTEM INCLUDES
#include <boost/units/systems/si/acceleration.hpp>

namespace cedar
{
  namespace aux
  {
    template <>
    boost::units::quantity<boost::units::si::acceleration>
      parseUnitString<boost::units::si::acceleration>(const std::string& unitStr)
    {
      return
        cedar::aux::parseCompoundUnit
                    <
                      boost::units::si::acceleration,
                      boost::units::si::length,
                      boost::units::si::time,
                      1, // power of length is 1 (m^1 / s^2)
                      2 // power of time is 2 (m / s^2)
                    >(unitStr);
    }

    // Generate types for the length parameter.
    typedef cedar::aux::UnitParameterTemplate<boost::units::si::acceleration> AccelerationParameter;
    CEDAR_GENERATE_POINTER_TYPES_INTRUSIVE(AccelerationParameter);
  }
}

#endif // CEDAR_AUX_ACCELERATION_PARAMETER_H
