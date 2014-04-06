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

    File:        ActionStop.cpp

    Maintainer:  Christian Bodenstein
    Email:       christian.bodenstein@ini.rub.de
    Date:        2014 03 09

    Description: Source file for the class cedar::proc::experiment::ActionStop.

    Credits:

======================================================================================================================*/

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/processing/experiment/ActionStop.h"
#include "cedar/processing/experiment/Experiment.h"
#include "cedar/processing/experiment/ExperimentSuperviser.h"


// SYSTEM INCLUDES

//----------------------------------------------------------------------------------------------------------------------
// register class
//----------------------------------------------------------------------------------------------------------------------

namespace
{
  bool declared = cedar::proc::experiment::ActionManagerSingleton::getInstance()->
      registerType<cedar::proc::experiment::ActionStopPtr>();
}
//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::proc::experiment::ActionStop::ActionStop()
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
_mSuccess( new cedar::aux::BoolParameter(this,"Success",true) ),
_mMessage(new cedar::aux::StringParameter(this,"Message",""))
{
}

cedar::proc::experiment::ActionStop::~ActionStop()
{
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::proc::experiment::ActionStop::run()
{
    ExperimentControllerSingleton::getInstance()->getExperiment()->stopTrial(_mResetType->getValue());
}
