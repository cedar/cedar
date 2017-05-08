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

    File:        OnTrialNumber.cpp

    Maintainer:  Christian Bodenstein
    Email:       christian.bodenstein@ini.rub.de
    Date:        2014 03 28

    Description: Source file for the class cedar::proc::experiment::condition::OnTrialNumber.

    Credits:

======================================================================================================================*/

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/processing/experiment/condition/OnTrialNumber.h"
#include "cedar/processing/experiment/Supervisor.h"
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
      registerType<cedar::proc::experiment::condition::OnTrialNumberPtr>();
    cedar::proc::experiment::condition::ConditionManagerSingleton::getInstance()->
      addDeprecatedName<cedar::proc::experiment::condition::OnTrialNumberPtr>
      (
        "cedar.proc.experiment.condition.OnTrial"
      );
    return true;
  }
  bool declared = declare();
}

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::proc::experiment::condition::OnTrialNumber::OnTrialNumber()
:
_mTrial(new cedar::aux::UIntParameter(this, "on trial", 1))
{
}

cedar::proc::experiment::condition::OnTrialNumber::~OnTrialNumber()
{
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

bool cedar::proc::experiment::condition::OnTrialNumber::checkValidity(std::vector<std::string>& /* errors */, std::vector<std::string>& warnings) const
{
  auto chosen_trial = this->_mTrial->getValue();
  Experiment* p_experiment = cedar::proc::experiment::SupervisorSingleton::getInstance()->getExperiment();
  auto max_trial = p_experiment->getTrialCount();
  if (chosen_trial >= max_trial)
  {
    warnings.push_back("OnTrialNumber condition will never fire: selected trial exceeds maximum number of trials.");
  }

  return true;
}

bool cedar::proc::experiment::condition::OnTrialNumber::check() const
{
  return false;
}

bool cedar::proc::experiment::condition::OnTrialNumber::initialCheck() const
{
  Experiment* p_experiment = cedar::proc::experiment::SupervisorSingleton::getInstance()->getExperiment();
  auto current_trial = p_experiment->getCurrentTrial();
  return current_trial == this->_mTrial->getValue();
}
