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

    File:        default_units.cpp

    Maintainer:  Mathis Richter

    Email:       mathis.richter@ini.rub.de

    Date:        2013 02 18

    Description: Initializations for the default units in cedar.

    Credits:

======================================================================================================================*/

// CEDAR INCLUDES
#include "cedar/units/Length.h"
#include "cedar/units/Time.h"
#include "cedar/units/Velocity.h"
#include "cedar/units/Acceleration.h"
#include "cedar/units/PlaneAngle.h"
#include "cedar/units/Frequency.h"
#include "cedar/units/AngularVelocity.h"

// SYSTEM INCLUDES

const cedar::unit::Length cedar::unit::DEFAULT_LENGTH_UNIT = 1 * boost::units::si::meter;
const cedar::unit::Time cedar::unit::DEFAULT_TIME_UNIT = 1 * boost::units::si::second;
const cedar::unit::Velocity cedar::unit::DEFAULT_VELOCITY_UNIT = 1 * boost::units::si::meter_per_second;
const cedar::unit::Acceleration cedar::unit::DEFAULT_ACCELERATION_UNIT = 1 * boost::units::si::meter_per_second_squared;
const cedar::unit::PlaneAngle cedar::unit::DEFAULT_PLANE_ANGLE_UNIT = 1 * boost::units::si::radian;
const cedar::unit::Frequency cedar::unit::DEFAULT_FREQUENCY_UNIT = 1 * boost::units::si::hertz;
const cedar::unit::AngularVelocity cedar::unit::DEFAULT_ANGULAR_VELOCITY_UNIT = 1 * boost::units::si::radian_per_second;
