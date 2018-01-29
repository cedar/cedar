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

    File:        prefixes.h

    Maintainer:  Mathis Richter
    Email:       mathis.richter@ini.rub.de
    Date:        2013 02 14

    Description: Contains prefixes for units.

    Credits:

======================================================================================================================*/

#ifndef CEDAR_UNITS_PREFIXES_H
#define CEDAR_UNITS_PREFIXES_H

// CEDAR INCLUDES

// SYSTEM INCLUDES
#ifndef Q_MOC_RUN
  #include <boost/units/systems/si/prefixes.hpp>
#endif // Q_MOC_RUN

namespace cedar
{
  namespace unit
  {
    // unit prefixes
    using boost::units::si::yocto; // 10^(-24)
    using boost::units::si::zepto; // 10^(-21)
    using boost::units::si::atto;  // 10^(-18)
    using boost::units::si::femto; // 10^(-15)
    using boost::units::si::pico;  // 10^(-12)
    using boost::units::si::nano;  // 10^(-9)
    using boost::units::si::micro; // 10^(-6)
    using boost::units::si::milli; // 10^(-3)
    using boost::units::si::centi; // 10^(-2)
    using boost::units::si::deci;  // 10^(-1)
    using boost::units::si::deka;  // 10^(1)
    using boost::units::si::hecto; // 10^(2)
    using boost::units::si::kilo;  // 10^(3)
    using boost::units::si::mega;  // 10^(6)
    using boost::units::si::giga;  // 10^(9)
    using boost::units::si::tera;  // 10^(12)
    using boost::units::si::peta;  // 10^(15)
    using boost::units::si::exa;   // 10^(18)
    using boost::units::si::zetta; // 10^(21)
    using boost::units::si::yotta; // 10^(24)
  }
}

#endif // CEDAR_UNITS_PREFIXES_H
