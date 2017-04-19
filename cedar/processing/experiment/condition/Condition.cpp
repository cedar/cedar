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

    File:        Condition.cpp

    Maintainer:  Christian Bodenstein
    Email:       christian.bodenstein@ini.ruhr-uni-bochum.de
    Date:        2014 01 22

    Description:

    Credits:

======================================================================================================================*/

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/processing/experiment/condition/Condition.h"
#include "cedar/auxiliaries/FactoryManager.h"
#include "cedar/processing/experiment/Experiment.h"
#include "cedar/processing/experiment/Experiment.h"

// SYSTEM INCLUDES

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::proc::experiment::condition::Condition::Condition()
{
  this->reset();
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

bool cedar::proc::experiment::condition::Condition::checkValidity
(
  std::vector<std::string>& /* errors */,
  std::vector<std::string>& /* warnings */
)
const
{
  // by default, conditions are just valid
  return true;
}

bool cedar::proc::experiment::condition::Condition::runCheck(bool skipIfFired) const
{
  if (this->mHasFired && skipIfFired)
  {
    return false;
  }
  bool v = this->check();
  if (v)
  {
    this->mHasFired = true;
  }
  return v;
}

bool cedar::proc::experiment::condition::Condition::initialCheck() const
{
  return false;
}

void cedar::proc::experiment::condition::Condition::reset()
{
  this->mHasFired = false;
}
