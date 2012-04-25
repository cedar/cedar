/*======================================================================================================================

    Copyright 2011 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany
 
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

    File:        LoopMode.cpp

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2011 09 20

    Description:

    Credits:

======================================================================================================================*/

// CEDAR INCLUDES
#include "cedar/processing/LoopMode.h"

// SYSTEM INCLUDES

cedar::aux::EnumType<cedar::proc::LoopMode> cedar::proc::LoopMode::mType("cedar::proc::LoopMode::");

#ifndef _MSC_VER
const cedar::proc::LoopMode::Id cedar::proc::LoopMode::FIXED_ADAPTIVE;
const cedar::proc::LoopMode::Id cedar::proc::LoopMode::FIXED;
const cedar::proc::LoopMode::Id cedar::proc::LoopMode::REALTIME;
#endif // _MSC_VER

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

void cedar::proc::LoopMode::construct()
{
  mType.type()->def(cedar::aux::Enum(cedar::proc::LoopMode::FIXED_ADAPTIVE, "FIXED_ADAPTIVE", "Fixed/Adaptive"));
  mType.type()->def(cedar::aux::Enum(cedar::proc::LoopMode::FIXED, "FIXED", "Fixed"));
  mType.type()->def(cedar::aux::Enum(cedar::proc::LoopMode::REALTIME, "REALTIME", "Real Time"));
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

const cedar::aux::EnumBase& cedar::proc::LoopMode::type()
{
  return *cedar::proc::LoopMode::mType.type();
}

const cedar::proc::LoopMode::TypePtr& cedar::proc::LoopMode::typePtr()
{
  return cedar::proc::LoopMode::mType.type();
}
