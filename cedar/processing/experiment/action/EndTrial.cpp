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

    File:        EndTrial.cpp

    Maintainer:  Christian Bodenstein
    Email:       christian.bodenstein@ini.rub.de
    Date:        2014 03 09

    Description: Source file for the class cedar::proc::experiment::EndTrial.

    Credits:

======================================================================================================================*/

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/processing/experiment/action/EndTrial.h"
#include "cedar/processing/experiment/Experiment.h"
#include "cedar/processing/experiment/Supervisor.h"


// SYSTEM INCLUDES

//----------------------------------------------------------------------------------------------------------------------
// register class
//----------------------------------------------------------------------------------------------------------------------

namespace
{
  bool register_it()
  {
    cedar::proc::experiment::action::ActionManagerSingleton::getInstance()->
          registerType<cedar::proc::experiment::action::EndTrialPtr>();

    cedar::proc::experiment::action::ActionManagerSingleton::getInstance()->
        addDeprecatedName<cedar::proc::experiment::action::EndTrialPtr>("cedar.proc.experiment.action.StopAllTriggers");

    return true;
  }
  bool declared = register_it();
}
//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::proc::experiment::action::EndTrial::EndTrial()
:
_mResetType
(
  new cedar::aux::EnumParameter
  (
    this,
    "Reset type",
    cedar::proc::experiment::Experiment::ResetType::typePtr(),
    cedar::proc::experiment::Experiment::ResetType::Reset
  )
),
_mSuccess( new cedar::aux::BoolParameter(this, "Success", true)),
_mMessage(new cedar::aux::StringParameter(this, "Message", "")),
_mStopTriggers( new cedar::aux::BoolParameter(this, "stop triggers", true))
{
}

cedar::proc::experiment::action::EndTrial::~EndTrial()
{
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::proc::experiment::action::EndTrial::run()
{
  auto super = cedar::proc::experiment::SupervisorSingleton::getInstance();
  super->log(_mSuccess->getValue() ? "Trial success" : "Trial failed", _mMessage->getValue());
  super->getExperiment()->stopTrial(_mResetType->getValue(), _mStopTriggers->getValue());
}
