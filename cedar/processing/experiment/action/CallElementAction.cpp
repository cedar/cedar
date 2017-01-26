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

    File:        CallElementAction.cpp

    Maintainer:  Stephan Zibner
    Email:       stephan.zibner@ini.rub.de
    Date:        2015 01 16

    Description: Source file for the class cedar::proc::experiment::CallElementAction.

    Credits:

======================================================================================================================*/

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/processing/experiment/action/CallElementAction.h"
#include "cedar/auxiliaries/ParameterDeclaration.h"
#include "cedar/processing/experiment/ConnectableParameter.h"

// SYSTEM INCLUDES

//----------------------------------------------------------------------------------------------------------------------
// register class
//----------------------------------------------------------------------------------------------------------------------

namespace
{
  bool declare()
  {
    cedar::proc::experiment::action::ActionManagerSingleton::getInstance()->
      registerType<cedar::proc::experiment::action::CallElementActionPtr>();
    return true;
  }

  bool declared = declare();
}
//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::proc::experiment::action::CallElementAction::CallElementAction()
:
_mConnectableParameter
(
  new cedar::proc::experiment::ConnectableParameter(this, "connectable")
),
_mActionParameter
(
  new cedar::proc::experiment::ActionParameter(this, "action", this->_mConnectableParameter)
)
{
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

bool cedar::proc::experiment::action::CallElementAction::checkValidity
(
  std::vector<std::string>& errors,
  std::vector<std::string>& /* warnings */
)
const
{
  std::string parameter_error;

  if (!this->_mConnectableParameter->checkValidity(parameter_error))
  {
    errors.push_back("No connectable set: " + parameter_error);
    return false;
  }
  if (!this->_mActionParameter->checkValidity(parameter_error))
  {
    errors.push_back("Action not set: " + parameter_error);
    return false;
  }

  return true;
}

cedar::proc::experiment::ConnectableParameterPtr cedar::proc::experiment::action::CallElementAction::getConnectableParameter() const
{
  return this->_mConnectableParameter;
}

void cedar::proc::experiment::action::CallElementAction::run()
{
  std::string error;
  if (!this->_mActionParameter->checkValidity(error))
  {
    return;
  }
  else
  {
    _mActionParameter->callAction();
  }
}
