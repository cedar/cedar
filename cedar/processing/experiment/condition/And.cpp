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

    File:        And.cpp

    Maintainer:  Christian Bodenstein
    Email:       christian.bodenstein@ini.ruhr-uni-bochum.de
    Date:        2014 01 22

    Description:

    Credits:

======================================================================================================================*/

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/processing/experiment/condition/And.h"
#include "cedar/processing/experiment/condition/OnMatrixValue.h"
#include "cedar/processing/experiment/condition/AtTime.h"
#include "cedar/auxiliaries/FactoryManager.h"
#include "cedar/processing/experiment/Experiment.h"

// SYSTEM INCLUDES

//----------------------------------------------------------------------------------------------------------------------
// register the class
//----------------------------------------------------------------------------------------------------------------------
namespace
{
	bool declared = cedar::proc::experiment::condition::ConditionManagerSingleton::getInstance()->
		registerType<cedar::proc::experiment::condition::AndPtr>();
}

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------
cedar::proc::experiment::condition::And::And()
:
_mCondition1
 (
   new cedar::proc::experiment::condition::Condition::ConditionParameter
   (
     this,
     "Condition 1",
     cedar::proc::experiment::condition::ConditionPtr(new cedar::proc::experiment::condition::OnMatrixValue())
   )
 )
,
_mCondition2
 (
   new cedar::proc::experiment::condition::Condition::ConditionParameter
   (
     this,
     "Condition 2",
     cedar::proc::experiment::condition::ConditionPtr(new cedar::proc::experiment::condition::AtTime())
   )
 )
{
}

bool cedar::proc::experiment::condition::And::check() const
{
  return _mCondition1->getValue()->runCheck(false) && _mCondition2->getValue()->runCheck(false);
}


//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

bool cedar::proc::experiment::condition::And::checkValidity(std::vector<std::string>& errors, std::vector<std::string>& warnings) const
{
  bool valid_1 = this->_mCondition1->getValue()->checkValidity(errors, warnings);
  bool valid_2 = this->_mCondition2->getValue()->checkValidity(errors, warnings);

  return valid_1 && valid_2;
}
