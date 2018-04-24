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

    File:        Mode.cpp

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2012 03 19

    Description:

    Credits:

======================================================================================================================*/

// CEDAR INCLUDES
#include "cedar/auxiliaries/convolution/Mode.h"
#include "cedar/auxiliaries/EnumBase.h"
#include "cedar/auxiliaries/EnumType.h"

// SYSTEM INCLUDES


//----------------------------------------------------------------------------------------------------------------------
// Static members
//----------------------------------------------------------------------------------------------------------------------

cedar::aux::EnumType<cedar::aux::conv::Mode> cedar::aux::conv::Mode::mType("cedar::aux::conv::Mode::");

#ifndef CEDAR_COMPILER_MSVC
const cedar::aux::conv::Mode::Id cedar::aux::conv::Mode::Same;
const cedar::aux::conv::Mode::Id cedar::aux::conv::Mode::Full;
const cedar::aux::conv::Mode::Id cedar::aux::conv::Mode::Valid;
#endif


//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::aux::conv::Mode::construct()
{
  mType.type()->def(cedar::aux::Enum(cedar::aux::conv::Mode::Same, "Same", "same"));
  mType.type()->def(cedar::aux::Enum(cedar::aux::conv::Mode::Full, "Full", "full"));
  mType.type()->def(cedar::aux::Enum(cedar::aux::conv::Mode::Valid, "Valid", "valid"));
}

const cedar::aux::EnumBase& cedar::aux::conv::Mode::type()
{
  return *cedar::aux::conv::Mode::typePtr();
}

const cedar::aux::conv::Mode::TypePtr& cedar::aux::conv::Mode::typePtr()
{
  return cedar::aux::conv::Mode::mType.type();
}
