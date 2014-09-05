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

    File:        ConditionOnTrial.cpp

    Maintainer:  Christian Bodenstein
    Email:       christian.bodenstein@ini.rub.de
    Date:        2014 03 28

    Description: Source file for the class cedar::proc::experiment::ConditionOnTrial.

    Credits:

======================================================================================================================*/

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/processing/experiment/condition/OnTrial.h"
#include "cedar/processing/experiment/ExperimentSuperviser.h"
#include "cedar/processing/experiment/Experiment.h"


// SYSTEM INCLUDES

//----------------------------------------------------------------------------------------------------------------------
// register the class
//----------------------------------------------------------------------------------------------------------------------
namespace
{
  bool declared = cedar::proc::experiment::condition::ConditionManagerSingleton::getInstance()->
    registerType<cedar::proc::experiment::condition::OnTrialPtr>();
}

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::proc::experiment::condition::OnTrial::OnTrial()
:
mHasFired(false),
_mTrial(new cedar::aux::UIntParameter(this,"on trial",1))
{
}

cedar::proc::experiment::condition::OnTrial::~OnTrial()
{
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------
bool cedar::proc::experiment::condition::OnTrial::check()
{
  Experiment* p_experiment = cedar::proc::experiment::ExperimentSuperviserSingleton::getInstance()->getExperiment();
  auto current_trial = p_experiment->getActualTrial();
  //!@todo This (and the same thing in AtTime) should be solved by a sort of reset function
  //! Also, this may be undesired behavior (i.e., users should possibly be able to change this)
  //! -> should this be part of the superclass? I.e., a fire-on-change parameter?
  if (current_trial < this->_mTrial->getValue())
  {
    this->mHasFired = false;
  }
  else if (!this->mHasFired && current_trial == this->_mTrial->getValue())
  {
    this->mHasFired = true;
    return true;
  }
  return false;
}
