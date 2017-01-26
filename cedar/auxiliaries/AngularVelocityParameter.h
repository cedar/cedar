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

    File:        TimeParameterTemplate.h

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2013 02 14

    Description:

    Credits:

======================================================================================================================*/

#ifndef CEDAR_AUX_ANGULAR_VELOCITY_PARAMETER_TEMPLATE_H
#define CEDAR_AUX_ANGULAR_VELOCITY_PARAMETER_TEMPLATE_H

// CEDAR INCLUDES
#include "cedar/auxiliaries/UnitParameterTemplate.h"
#include "cedar/units/AngularVelocity.h"
#include "cedar/units/PlaneAngle.h"
#include "cedar/units/Time.h"

// SYSTEM INCLUDES
#ifndef Q_MOC_RUN
  #include <boost/units/systems/si/angular_velocity.hpp>
#endif // Q_MOC_RUN

namespace cedar
{
  namespace aux
  {
    //! a string parser for unit angular velocity
    template <>
    inline cedar::unit::AngularVelocity
      parseUnitString<cedar::unit::AngularVelocity::unit_type>(const std::string& unitStr)
    {
      return
        cedar::aux::parseCompoundUnit
                    <
                      cedar::unit::AngularVelocity::unit_type,
                      cedar::unit::PlaneAngle::unit_type,
                      cedar::unit::Time::unit_type,
                      1,
                      1
                    >(unitStr);
    }

    // Generate types for the length parameter.
    //! a unit parameter for angular velocity
    typedef cedar::aux::UnitParameterTemplate<cedar::unit::AngularVelocity::unit_type> AngularVelocityParameter;
    CEDAR_GENERATE_POINTER_TYPES_INTRUSIVE(AngularVelocityParameter);
  }
}

#endif // CEDAR_AUX_ANGULAR_VELOCITY_PARAMETER_TEMPLATE_H
