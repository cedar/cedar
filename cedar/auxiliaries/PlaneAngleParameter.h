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

#ifndef CEDAR_AUX_PLANE_ANGLE_PARAMETER_TEMPLATE_H
#define CEDAR_AUX_PLANE_ANGLE_PARAMETER_TEMPLATE_H

// CEDAR INCLUDES
#include "cedar/auxiliaries/UnitParameterTemplate.h"
#include "cedar/units/PlaneAngle.h"

// SYSTEM INCLUDES
#include <map>


namespace cedar
{
  namespace aux
  {
    template <>
    inline cedar::unit::PlaneAngle getUnitFromPostFix(const std::string& postFix)
    {
      static std::map<std::string, cedar::unit::PlaneAngle> map;

      if (map.empty())
      {
        addSubUnitToMaps
        (
          map,
          cedar::unit::radian,
          "radian", "rad",
          1.0
        );
        addAliasToMaps(map, "radian", "radians");
      }

      return cedar::aux::findUnit(postFix, map);
    }

    // Generate types for the length parameter.
    //! a unit parameter for plane angles
    typedef cedar::aux::UnitParameterTemplate<cedar::unit::PlaneAngle::unit_type> PlaneAngleParameter;
    CEDAR_GENERATE_POINTER_TYPES_INTRUSIVE(PlaneAngleParameter);
  }
}

#endif // CEDAR_AUX_PLANE_ANGLE_PARAMETER_TEMPLATE_H
