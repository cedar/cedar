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

#ifndef CEDAR_AUX_TIME_PARAMETER_TEMPLATE_H
#define CEDAR_AUX_TIME_PARAMETER_TEMPLATE_H

// CEDAR INCLUDES
#include "cedar/auxiliaries/UnitParameterTemplate.h"
#include "cedar/units/Time.h"

// SYSTEM INCLUDES
#include <string>
#include <map>

namespace cedar
{
  namespace aux
  {
    //! a string parser for unit time
    template <>
    inline cedar::unit::Time getUnitFromPostFix(const std::string& postFix)
    {
      static std::map<std::string, cedar::unit::Time> map;

      if (map.empty())
      {
        addBaseUnitToMaps<boost::units::si::second_base_unit>(map, cedar::unit::seconds);
        addAliasToMaps(map, "second", "seconds");

        addSubUnitToMaps
        (
          map,
          cedar::unit::seconds,
          "millisecond", "ms",
          0.001
        );
        addAliasToMaps(map, "millisecond", "milliseconds");

        addSubUnitToMaps
        (
          map,
          cedar::unit::seconds,
          "minute", "min",
          60.0
        );
        addAliasToMaps(map, "minute", "minutes");

        addSubUnitToMaps
        (
          map,
          cedar::unit::seconds,
          "hour", "h",
          60.0 * 60.0
        );
        addAliasToMaps(map, "hour", "hours");
      }

      return cedar::aux::findUnit(postFix, map);
    }

    template <>
    inline std::string getDefaultUnit<cedar::unit::Time::unit_type>()
    {
      return "ms";
    }

    // Generate types for the length parameter.
    //! a unit parameter for time
    typedef cedar::aux::UnitParameterTemplate<cedar::unit::Time::unit_type> TimeParameter;
    CEDAR_GENERATE_POINTER_TYPES_INTRUSIVE(TimeParameter);
  }
}

#endif // CEDAR_AUX_TIME_PARAMETER_TEMPLATE_H
