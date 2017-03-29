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

    File:        NormalizationType.cpp

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2011 10 17

    Description:

    Credits:

======================================================================================================================*/

// CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/processing/steps/NormalizationType.h"

// SYSTEM INCLUDES

cedar::aux::EnumType<cedar::proc::steps::NormalizationType>
  cedar::proc::steps::NormalizationType::mType("cedar::proc::steps::NormalizationType::");

#ifndef CEDAR_COMPILER_MSVC
const cedar::proc::steps::NormalizationType::Id cedar::proc::steps::NormalizationType::None;
const cedar::proc::steps::NormalizationType::Id cedar::proc::steps::NormalizationType::InfinityNorm;
const cedar::proc::steps::NormalizationType::Id cedar::proc::steps::NormalizationType::L1Norm;
const cedar::proc::steps::NormalizationType::Id cedar::proc::steps::NormalizationType::L2Norm;
#endif // CEDAR_COMPILER_MSVC

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

void cedar::proc::steps::NormalizationType::construct()
{
  mType.type()->def(cedar::aux::Enum(cedar::proc::steps::NormalizationType::None, "None", "None"));
  mType.type()->def(cedar::aux::Enum(cedar::proc::steps::NormalizationType::InfinityNorm, "InfinityNorm", "Infinity Norm"));
  mType.type()->def(cedar::aux::Enum(cedar::proc::steps::NormalizationType::L1Norm, "L1Norm", "L1 Norm"));
  mType.type()->def(cedar::aux::Enum(cedar::proc::steps::NormalizationType::L2Norm, "L2Norm", "L2 Norm"));
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

const cedar::aux::EnumBase& cedar::proc::steps::NormalizationType::type()
{
  return *cedar::proc::steps::NormalizationType::mType.type();
}

const cedar::proc::steps::NormalizationType::TypePtr& cedar::proc::steps::NormalizationType::typePtr()
{
  return cedar::proc::steps::NormalizationType::mType.type();
}
