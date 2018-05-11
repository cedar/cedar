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

    File:        LoopMode.h

    Maintainer:  Stephan Zibner
    Email:       stephan.zibner@ini.ruhr-uni-bochum.de
    Date:        2012 06 22

    Description:

    Credits:

======================================================================================================================*/

// CEDAR INCLUDES
#include "cedar/auxiliaries/LoopMode.h"
#include "cedar/auxiliaries/EnumBase.h"
#include "cedar/auxiliaries/EnumType.h"

// SYSTEM INCLUDES


//----------------------------------------------------------------------------------------------------------------------
// Static members
//----------------------------------------------------------------------------------------------------------------------

cedar::aux::EnumType<cedar::aux::LoopMode> cedar::aux::LoopMode::mType("cedar::aux::LoopMode::");

#ifndef CEDAR_COMPILER_MSVC
// old and deprecated:
const cedar::aux::LoopMode::Id cedar::aux::LoopMode::RealTime;
const cedar::aux::LoopMode::Id cedar::aux::LoopMode::Fixed;
const cedar::aux::LoopMode::Id cedar::aux::LoopMode::FixedAdaptive;
const cedar::aux::LoopMode::Id cedar::aux::LoopMode::Simulated;

// new and shiny:
const cedar::aux::LoopMode::Id cedar::aux::LoopMode::RealDT;
const cedar::aux::LoopMode::Id cedar::aux::LoopMode::FakeDT;
#endif


//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::aux::LoopMode::construct()
{
  mType.type()->def(cedar::aux::Enum(cedar::aux::LoopMode::RealTime, "RealTime", "real-time"));
  mType.type()->def(cedar::aux::Enum(cedar::aux::LoopMode::Fixed, "Fixed", "fixed time steps"));
  mType.type()->def
  (
    cedar::aux::Enum(cedar::aux::LoopMode::FixedAdaptive, "FixedAdaptive", "fixed adaptive time steps")
  );
  mType.type()->def(cedar::aux::Enum(cedar::aux::LoopMode::Simulated, "Simulated", "simulated time"));
  mType.type()->def(cedar::aux::Enum(cedar::aux::LoopMode::RealDT, "real deltaT", "real deltaT"));
  mType.type()->def(cedar::aux::Enum(cedar::aux::LoopMode::FakeDT, "fake deltaT", "fake deltaT"));
}

const cedar::aux::EnumBase& cedar::aux::LoopMode::type()
{
  return *cedar::aux::LoopMode::typePtr();
}

const cedar::aux::LoopMode::TypePtr& cedar::aux::LoopMode::typePtr()
{
  return cedar::aux::LoopMode::mType.type();
}
