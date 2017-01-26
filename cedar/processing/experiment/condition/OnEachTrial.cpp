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

    File:        ConditionOnEachTrial.cpp

    Maintainer:  Christian Bodenstein
    Email:       christian.bodenstein@ini.rub.de
    Date:        2014 02 06

    Description: Source file for the class cedar::proc::experiment::ConditionOnEachTrial.

    Credits:

======================================================================================================================*/

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/processing/experiment/condition/OnEachTrial.h"
#include "cedar/processing/experiment/Experiment.h"

// SYSTEM INCLUDES

//----------------------------------------------------------------------------------------------------------------------
// register the class
//----------------------------------------------------------------------------------------------------------------------
namespace
{
  bool declare()
  {
    cedar::proc::experiment::condition::ConditionManagerSingleton::getInstance()->
      registerType<cedar::proc::experiment::condition::OnEachTrialPtr>();
    cedar::proc::experiment::condition::ConditionManagerSingleton::getInstance()->
      addDeprecatedName<cedar::proc::experiment::condition::OnEachTrialPtr>
      (
        "cedar.proc.experiment.condition.OnInit"
      );
    return true;
  }
  bool declared = declare();
}

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::proc::experiment::condition::OnEachTrial::OnEachTrial()
{
}


//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------


bool cedar::proc::experiment::condition::OnEachTrial::check() const
{
  return false;
}

bool cedar::proc::experiment::condition::OnEachTrial::initialCheck() const
{
  return true;
}
