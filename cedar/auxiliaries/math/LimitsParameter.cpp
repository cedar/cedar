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

    File:        LimitsParameter.cpp

    Maintainer:  Mathis Richter
    Email:       mathis.richter@ini.rub.de
    Date:        2010 12 09

    Description: Specialization for the @em cedar::aux::math::LimitsParameter struct.

    Credits:

======================================================================================================================*/

// CEDAR INCLUDES
#include "cedar/auxiliaries/math/LimitsParameter.h"
#include "cedar/auxiliaries/LengthParameter.h"
#include "cedar/auxiliaries/TimeParameter.h"
#include "cedar/auxiliaries/VelocityParameter.h"
#include "cedar/auxiliaries/AccelerationParameter.h"
#include "cedar/auxiliaries/PlaneAngleParameter.h"
#include "cedar/auxiliaries/AngularVelocityParameter.h"
#include "cedar/auxiliaries/FrequencyParameter.h"

// SYSTEM INCLUDES

template class cedar::aux::math::LimitsParameter<double>;
template class cedar::aux::math::LimitsParameter<int>;
template class cedar::aux::math::LimitsParameter<unsigned int>;
template class cedar::aux::math::LimitsParameter<cedar::unit::Length>;
template class cedar::aux::math::LimitsParameter<cedar::unit::Time>;
template class cedar::aux::math::LimitsParameter<cedar::unit::Velocity>;
template class cedar::aux::math::LimitsParameter<cedar::unit::Acceleration>;
template class cedar::aux::math::LimitsParameter<cedar::unit::PlaneAngle>;
template class cedar::aux::math::LimitsParameter<cedar::unit::AngularVelocity>;
template class cedar::aux::math::LimitsParameter<cedar::unit::Frequency>;
