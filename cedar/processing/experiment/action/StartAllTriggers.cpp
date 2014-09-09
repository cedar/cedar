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

    File:        ActionStart.cpp

    Maintainer:  Christian Bodenstein
    Email:       christian.bodenstein@ini.rub.de
    Date:        2014 02 06

    Description: Source file for the class cedar::proc::experiment::ActionStart.

    Credits:

======================================================================================================================*/

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/processing/experiment/action/StartAllTriggers.h"
#include "cedar/processing/experiment/Experiment.h"
#include "cedar/processing/experiment/Supervisor.h"

// SYSTEM INCLUDES

//----------------------------------------------------------------------------------------------------------------------
// register class
//----------------------------------------------------------------------------------------------------------------------

namespace
{
  bool declared = cedar::proc::experiment::action::ActionManagerSingleton::getInstance()->
      registerType<cedar::proc::experiment::action::StartAllTriggersPtr>();
}

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::proc::experiment::action::StartAllTriggers::StartAllTriggers()
{
}

cedar::proc::experiment::action::StartAllTriggers::~StartAllTriggers()
{
}



//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------


void cedar::proc::experiment::action::StartAllTriggers::run()
{
  cedar::proc::experiment::SupervisorSingleton::getInstance()->getExperiment()->startAllTriggers();
}


