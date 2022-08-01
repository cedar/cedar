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

    File:        SizesRangeHint.cpp

    Maintainer:  Lars Janssen
    Email:       lars.janssen@ini.rub.de
    Date:        2022 07 24

    Description: Source file for the class cedar::aux::annotation::SizesRangeHint.

    Credits:

======================================================================================================================*/

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CLASS HEADER
#include "cedar/auxiliaries/annotation/SizesRangeHint.h"

// CEDAR INCLUDES

// SYSTEM INCLUDES

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::aux::annotation::SizesRangeHint::SizesRangeHint(const std::vector<cedar::aux::math::Limits<double>>& limits)
{
  this->mRange = limits;
}

cedar::aux::annotation::SizesRangeHint::~SizesRangeHint()
{
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

bool cedar::aux::annotation::SizesRangeHint::excludeFromCopying() const
{
  return true;
}

const std::vector<cedar::aux::math::Limits<double>>& cedar::aux::annotation::SizesRangeHint::getRange() const
{
  return this->mRange;
}

std::string cedar::aux::annotation::SizesRangeHint::getDescription() const
{
  std::string description = "dimensions should be in the range ";
  for(auto limit : this->mRange)
  {
    description += "[";
    description += cedar::aux::toString(limit.getLower());
    description += ", ";
    description += cedar::aux::toString(limit.getUpper());
    description += "] ";
  }
  return description;
}
