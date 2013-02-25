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

    File:        LengthParameterTemplate.h

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2013 02 14

    Description:

    Credits:

======================================================================================================================*/

#ifndef CEDAR_AUX_LENGTH_PARAMETER_TEMPLATE_H
#define CEDAR_AUX_LENGTH_PARAMETER_TEMPLATE_H

// CEDAR INCLUDES
#include "cedar/auxiliaries/UnitParameterTemplate.h"

// SYSTEM INCLUDES
#include <boost/units/systems/si/length.hpp>

namespace cedar
{
  namespace aux
  {
    template <>
    boost::units::quantity<boost::units::si::length> getUnitFromPostFix(const std::string& postFix)
    {
      static std::map<std::string, boost::units::quantity<boost::units::si::length> > map;

      if (map.empty())
      {
        addBaseUnitToMaps<boost::units::si::meter_base_unit>(map, boost::units::si::meters);
        addSubUnitToMaps
        (
          map,
          boost::units::si::meters,
          "centimeter", "cm",
          0.01
        );
        addAliasToMaps(map, "centimeter", "centimeters");

        addSubUnitToMaps
        (
          map,
          boost::units::si::meters,
          "kilometer", "km",
          1000.0
        );
        addAliasToMaps(map, "kilometer", "kilometers");
      }

      return cedar::aux::findUnit(postFix, map);
    }

    // Generate types for the length parameter.
    typedef cedar::aux::UnitParameterTemplate<boost::units::si::length> LengthParameter;
    CEDAR_GENERATE_POINTER_TYPES_INTRUSIVE(LengthParameter);
  }
}

#endif // CEDAR_AUX_LENGTH_PARAMETER_TEMPLATE_H
