/*======================================================================================================================

    Copyright 2011, 2012, 2013, 2014 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany
 
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

    File:        StartTrigger.cpp

    Maintainer:  Christian Bodenstein
    Email:       christian.bodenstein@ini.rub.de
    Date:        2014 05 10

    Description: Source file for the class cedar::proc::experiment::condition::StartTrigger.

    Credits:

======================================================================================================================*/

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/processing/experiment/action/StartTrigger.h"
#include "cedar/processing/experiment/ExperimentSuperviser.h"
#include "cedar/auxiliaries/CallFunctionInThread.h"
// SYSTEM INCLUDES

//----------------------------------------------------------------------------------------------------------------------
// register the class
//----------------------------------------------------------------------------------------------------------------------
namespace
{
  bool declared = cedar::proc::experiment::action::ActionManagerSingleton::getInstance()->
    registerType<cedar::proc::experiment::action::StartTriggerPtr>();
}

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::proc::experiment::action::StartTrigger::StartTrigger()
:
mTrigger(new cedar::proc::experiment::TriggerParameter(this,"Start Trigger"))
{
}

cedar::proc::experiment::action::StartTrigger::~StartTrigger()
{
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::proc::experiment::action::StartTrigger::run()
{
  ExperimentSuperviserSingleton::getInstance()->getExperiment()->startTrigger(mTrigger->getTrigger());
}
