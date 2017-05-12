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

#ifndef CEDAR_AUX_FREQUENCY_PARAMETER_TEMPLATE_H
#define CEDAR_AUX_FREQUENCY_PARAMETER_TEMPLATE_H

// CEDAR INCLUDES
#include "cedar/auxiliaries/UnitParameterTemplate.h"
#include "cedar/auxiliaries/TimeParameter.h"
#include "cedar/units/Frequency.h"
#include "cedar/units/Time.h"

// SYSTEM INCLUDES
#include <map>

namespace cedar
{
  namespace aux
  {
    //! a string parser for unit frequency
    template <>
    inline cedar::unit::Frequency getUnitFromPostFix(const std::string& postFix)
    {
      static std::map<std::string, cedar::unit::Frequency> map;

      if (map.empty())
      {
        addSubUnitToMaps
        (
          map,
          cedar::unit::hertz,
          "hertz", "Hz",
          1
        );
      }

      try
      {
        return cedar::aux::findUnit(postFix, map);
      }
      catch (const cedar::aux::UnknownUnitSuffixException&)
      {
        return
          cedar::aux::parseCompoundUnit
                      <
                        cedar::unit::Frequency::unit_type,
                        boost::units::si::dimensionless,
                        cedar::unit::Time::unit_type,
                        1,
                        1
                      >(postFix);
      }
    }

    template <>
    inline cedar::unit::Frequency parseUnitString(const std::string& unitStr)
    {
      return getUnitFromPostFix<cedar::unit::Frequency::unit_type>(unitStr);
    }

    // Generate types for the length parameter.
    //! a unit parameter for frequency
    typedef cedar::aux::UnitParameterTemplate<cedar::unit::Frequency::unit_type> FrequencyParameter;
    CEDAR_GENERATE_POINTER_TYPES_INTRUSIVE(FrequencyParameter);
  }
}

#endif // CEDAR_AUX_FREQUENCY_PARAMETER_TEMPLATE_H
