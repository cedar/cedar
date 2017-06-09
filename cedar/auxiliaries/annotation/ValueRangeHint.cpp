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

    File:        ValueRangeHint.cpp

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2015 02 04

    Description: Source file for the class cedar::aux::annotation::ValueRangeHint.

    Credits:

======================================================================================================================*/

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CLASS HEADER
#include "cedar/auxiliaries/annotation/ValueRangeHint.h"

// CEDAR INCLUDES
#include "cedar/auxiliaries/stringFunctions.h"

// SYSTEM INCLUDES

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::aux::annotation::ValueRangeHint::ValueRangeHint(const cedar::aux::math::Limits<double>& limits)
{
  this->init(limits);
}

cedar::aux::annotation::ValueRangeHint::ValueRangeHint(double lower, double upper)
{
  this->init(cedar::aux::math::Limits<double>(lower, upper));
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::aux::annotation::ValueRangeHint::init(const cedar::aux::math::Limits<double>& limits)
{
  this->mRange = limits;
}

bool cedar::aux::annotation::ValueRangeHint::excludeFromCopying() const
{
  return true;
}

const cedar::aux::math::Limits<double>& cedar::aux::annotation::ValueRangeHint::getRange() const
{
  return this->mRange;
}

std::string cedar::aux::annotation::ValueRangeHint::getDescription() const
{
  std::string description = "values should be in the range [";
  description += cedar::aux::toString(this->mRange.getLower());
  description += ", ";
  description += cedar::aux::toString(this->mRange.getUpper());
  return description;
}
