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
#include "cedar/units/Length.h"

// SYSTEM INCLUDES
#include <map>

namespace cedar
{
  namespace aux
  {
    template <>
    inline cedar::unit::Length getUnitFromPostFix(const std::string& postFix)
    {
      static std::map<std::string, cedar::unit::Length> map;

      if (map.empty())
      {
        addBaseUnitToMaps<boost::units::si::meter_base_unit>(map, cedar::unit::meter);
        addSubUnitToMaps
        (
          map,
          cedar::unit::meter,
          "centimeter", "cm",
          0.01
        );
        addAliasToMaps(map, "centimeter", "centimeters");

        addSubUnitToMaps
        (
          map,
          cedar::unit::meter,
          "kilometer", "km",
          1000.0
        );
        addAliasToMaps(map, "kilometer", "kilometers");
      }

      return cedar::aux::findUnit(postFix, map);
    }

    // Generate types for the length parameter.
    //! a unit parameter for length
    typedef cedar::aux::UnitParameterTemplate<cedar::unit::Length::unit_type> LengthParameter;
    CEDAR_GENERATE_POINTER_TYPES_INTRUSIVE(LengthParameter);
  }
}

#endif // CEDAR_AUX_LENGTH_PARAMETER_TEMPLATE_H
