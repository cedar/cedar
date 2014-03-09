/*======================================================================================================================

    Copyright 2011, 2012, 2013 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany
 
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

    File:        ConditionAnd.cpp

    Maintainer:  Christian Bodenstein
    Email:       christian.bodenstein@ini.ruhr-uni-bochum.de
    Date:        2014 01 22

    Description:

    Credits:

======================================================================================================================*/

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/processing/experiment/ConditionAnd.h"
#include "cedar/processing/experiment/ConditionOnInit.h"
#include "cedar/auxiliaries/FactoryManager.h"
#include "cedar/processing/experiment/Experiment.h"
#include "cedar/processing/experiment/ExperimentController.h"

// SYSTEM INCLUDES

//----------------------------------------------------------------------------------------------------------------------
// register the class
//----------------------------------------------------------------------------------------------------------------------
namespace
{
	bool declared = cedar::proc::experiment::ConditionManagerSingleton::getInstance()->
		registerType<cedar::proc::experiment::ConditionAndPtr>();
}

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------
cedar::proc::experiment::ConditionAnd::ConditionAnd()
:
_mCondition1
 (
   new cedar::proc::experiment::Condition::ConditionParameter
   (
     this,
     "Condition1",
     cedar::proc::experiment::ConditionPtr(new cedar::proc::experiment::ConditionOnInit())
   )
 )
,
_mCondition2
 (
   new cedar::proc::experiment::Condition::ConditionParameter
   (
     this,
     "Condition2",
     cedar::proc::experiment::ConditionPtr(new cedar::proc::experiment::ConditionOnInit())
   )
 )
{
}
cedar::proc::experiment::ConditionAnd::~ConditionAnd()
{

}

bool cedar::proc::experiment::ConditionAnd::check()
{
  return _mCondition1->getValue()->check() && _mCondition2->getValue()->check();
}


//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------


