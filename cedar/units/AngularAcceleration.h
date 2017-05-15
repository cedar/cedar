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

    File:        AngularAcceleration.h

    Maintainer:  Mathis Richter
    Email:       mathis.richter@ini.rub.de
    Date:        2013 02 22

    Description: This is a header for all angular-acceleration-related units.

    Credits:

======================================================================================================================*/

#ifndef CEDAR_UNITS_ANGULAR_ACCELERATION_H
#define CEDAR_UNITS_ANGULAR_ACCELERATION_H

// CEDAR INCLUDES
#include "cedar/units/UnitMatrix.h"
#include "cedar/units/version.h"

// SYSTEM INCLUDES
#ifndef Q_MOC_RUN
  #include <boost/units/quantity.hpp>
  #include <boost/units/systems/si/angular_acceleration.hpp>
#endif // Q_MOC_RUN

namespace cedar
{
  namespace unit
  {
    typedef boost::units::quantity<boost::units::si::angular_acceleration> AngularAcceleration;

    //!@brief template concretion for angular_acceleration
    typedef UnitMatrix<boost::units::si::angular_acceleration> AngularAccelerationMatrix;
  }
}

#endif // CEDAR_UNITS_ANGULAR_ACCELERATION_H
