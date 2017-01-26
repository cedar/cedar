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

    File:        FFTWPlanningStrategy.cpp

    Maintainer:  Stephan Zibner
    Email:       stephan.zibner@ini.ruhr-uni-bochum.de
    Date:        2014 01 14

    Description:

    Credits:

======================================================================================================================*/

// CEDAR INCLUDES
#include "cedar/auxiliaries/convolution/FFTWPlanningStrategy.h"
#include "cedar/auxiliaries/EnumBase.h"
#include "cedar/auxiliaries/EnumType.h"

// SYSTEM INCLUDES


//----------------------------------------------------------------------------------------------------------------------
// Static members
//----------------------------------------------------------------------------------------------------------------------

cedar::aux::EnumType<cedar::aux::conv::FFTWPlanningStrategy> cedar::aux::conv::FFTWPlanningStrategy::mType("cedar::aux::conv::FFTWPlanningStrategy::");

#ifndef CEDAR_COMPILER_MSVC
const cedar::aux::conv::FFTWPlanningStrategy::Id cedar::aux::conv::FFTWPlanningStrategy::Estimate;
const cedar::aux::conv::FFTWPlanningStrategy::Id cedar::aux::conv::FFTWPlanningStrategy::Measure;
const cedar::aux::conv::FFTWPlanningStrategy::Id cedar::aux::conv::FFTWPlanningStrategy::Patient;
const cedar::aux::conv::FFTWPlanningStrategy::Id cedar::aux::conv::FFTWPlanningStrategy::Exhaustive;
#endif


//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::aux::conv::FFTWPlanningStrategy::construct()
{
  mType.type()->def(cedar::aux::Enum(cedar::aux::conv::FFTWPlanningStrategy::Estimate, "Estimate"));
  mType.type()->def(cedar::aux::Enum(cedar::aux::conv::FFTWPlanningStrategy::Measure, "Measure"));
  mType.type()->def(cedar::aux::Enum(cedar::aux::conv::FFTWPlanningStrategy::Patient, "Patient"));
  mType.type()->def(cedar::aux::Enum(cedar::aux::conv::FFTWPlanningStrategy::Exhaustive, "Exhaustive"));
}

const cedar::aux::EnumBase& cedar::aux::conv::FFTWPlanningStrategy::type()
{
  return *cedar::aux::conv::FFTWPlanningStrategy::typePtr();
}

const cedar::aux::conv::FFTWPlanningStrategy::TypePtr& cedar::aux::conv::FFTWPlanningStrategy::typePtr()
{
  return cedar::aux::conv::FFTWPlanningStrategy::mType.type();
}
