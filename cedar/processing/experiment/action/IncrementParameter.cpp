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

    File:        IncrementParameter.cpp

    Maintainer:  Christian Bodenstein
    Email:       christian.bodenstein@ini.rub.de
    Date:        2014 03 19

    Description: Source file for the class cedar::proc::experiment::IncrementParameter.

    Credits:

======================================================================================================================*/

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/processing/experiment/action/IncrementParameter.h"
#include "cedar/auxiliaries/DoubleParameter.h"
#include "cedar/auxiliaries/UIntParameter.h"
#include "cedar/auxiliaries/IntParameter.h"

// SYSTEM INCLUDES

//----------------------------------------------------------------------------------------------------------------------
// register class
//----------------------------------------------------------------------------------------------------------------------

namespace
{
  bool declare()
  {
    cedar::proc::experiment::action::ActionManagerSingleton::getInstance()->
      registerType<cedar::proc::experiment::action::IncrementParameterPtr>();
    cedar::proc::experiment::action::ActionManagerSingleton::getInstance()->
      addDeprecatedName<cedar::proc::experiment::action::IncrementParameterPtr>
      (
        "cedar.proc.experiment.action.ParameterAdd"
      );
    return true;
  }

  bool declared = declare();
}
//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::proc::experiment::action::IncrementParameter::IncrementParameter()
:
    _mStepParameter
(
    new cedar::proc::experiment::StepPropertyParameter(this,"StepParameter")
)
{
  _mStepParameter->setType(cedar::proc::experiment::StepPropertyParameter::PARAMETER_VALUE);

  // allow only UIntParameter and DoubleParameter to increase
  _mStepParameter->allowType("cedar.aux.DoubleParameter");
  _mStepParameter->allowType("cedar.aux.UIntParameter");
}

cedar::proc::experiment::action::IncrementParameter::~IncrementParameter()
{
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

bool cedar::proc::experiment::action::IncrementParameter::checkValidity
(
  std::vector<std::string>& errors,
  std::vector<std::string>& /* warnings */
)
const
{
  std::string parameter_error;
  bool parameter_valid = this->_mStepParameter->checkValidity(parameter_error);
  if (!parameter_valid)
  {
    errors.push_back("Cannot increment parameter: " + parameter_error);
    return false;
  }

  return true;
}

void cedar::proc::experiment::action::IncrementParameter::run()
{
  if (!_mStepParameter->isParameterSelected())
  {
    return;
  }

  //check the type of the parameter
  if (auto parameter = boost::dynamic_pointer_cast<cedar::aux::DoubleParameter>(_mStepParameter->getParameter()))
  {
    auto parameter_copy = boost::dynamic_pointer_cast<cedar::aux::DoubleParameter>(_mStepParameter->getParameterCopy());
    parameter->setValue(parameter->getValue() + parameter_copy->getValue());
  }

  if (auto parameter = boost::dynamic_pointer_cast<cedar::aux::UIntParameter>(_mStepParameter->getParameter()))
  {
    auto parameter_copy = boost::dynamic_pointer_cast<cedar::aux::UIntParameter>(_mStepParameter->getParameterCopy());
    parameter->setValue(parameter->getValue() + parameter_copy->getValue());
  }
}
