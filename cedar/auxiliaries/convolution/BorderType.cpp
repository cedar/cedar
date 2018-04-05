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

    File:        BorderType.cpp

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2012 03 14

    Description:

    Credits:

======================================================================================================================*/

// CEDAR INCLUDES
#include "cedar/auxiliaries/convolution/BorderType.h"
#include "cedar/auxiliaries/EnumBase.h"
#include "cedar/auxiliaries/EnumType.h"

// SYSTEM INCLUDES


//----------------------------------------------------------------------------------------------------------------------
// Static members
//----------------------------------------------------------------------------------------------------------------------

cedar::aux::EnumType<cedar::aux::conv::BorderType> cedar::aux::conv::BorderType::mType("cedar::aux::conv::BorderType::");

#ifndef CEDAR_COMPILER_MSVC
const cedar::aux::conv::BorderType::Id cedar::aux::conv::BorderType::Cyclic;
const cedar::aux::conv::BorderType::Id cedar::aux::conv::BorderType::Zero;
const cedar::aux::conv::BorderType::Id cedar::aux::conv::BorderType::Replicate;
const cedar::aux::conv::BorderType::Id cedar::aux::conv::BorderType::Reflect;
#endif


//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::aux::conv::BorderType::construct()
{
  mType.type()->def(cedar::aux::Enum(cedar::aux::conv::BorderType::Cyclic, "Cyclic", "cyclic borders"));
  mType.type()->def(cedar::aux::Enum(cedar::aux::conv::BorderType::Zero, "Zero", "zero-filled borders"));
  mType.type()->def(cedar::aux::Enum(cedar::aux::conv::BorderType::Replicate, "Replicate", "replicate borders"));
  mType.type()->def(cedar::aux::Enum(cedar::aux::conv::BorderType::Reflect, "Reflect", "mirror borders"));
}

const cedar::aux::EnumBase& cedar::aux::conv::BorderType::type()
{
  return *cedar::aux::conv::BorderType::typePtr();
}

const cedar::aux::conv::BorderType::TypePtr& cedar::aux::conv::BorderType::typePtr()
{
  return cedar::aux::conv::BorderType::mType.type();
}
