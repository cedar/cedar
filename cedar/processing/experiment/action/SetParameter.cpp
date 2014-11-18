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

    File:        ActionSetParameter.cpp

    Maintainer:  Christian Bodenstein
    Email:       christian.bodenstein@ini.rub.de
    Date:        2014 03 07

    Description: Source file for the class cedar::proc::experiment::ActionSetParameter.

    Credits:

======================================================================================================================*/

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/processing/experiment/action/SetParameter.h"
#include "cedar/auxiliaries/ParameterDeclaration.h"
#include "cedar/processing/experiment/StepPropertyParameter.h"

// SYSTEM INCLUDES

//----------------------------------------------------------------------------------------------------------------------
// register class
//----------------------------------------------------------------------------------------------------------------------

namespace
{
  bool declared = cedar::proc::experiment::action::ActionManagerSingleton::getInstance()->
      registerType<cedar::proc::experiment::action::SetParameterPtr>();
}
//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::proc::experiment::action::SetParameter::SetParameter()
:
_mStepParameter
(
    new cedar::proc::experiment::StepPropertyParameter(this,"Step Parameter")
)
{
  _mStepParameter->setType(cedar::proc::experiment::StepPropertyParameter::PARAMETER_VALUE);
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

cedar::proc::experiment::StepPropertyParameterPtr cedar::proc::experiment::action::SetParameter::getStepParameter() const
{
  return this->_mStepParameter;
}

void cedar::proc::experiment::action::SetParameter::run()
{
  if (!_mStepParameter->isParameterSelected())
  {
    return;
  }
  cedar::aux::ParameterPtr parameter = _mStepParameter->getParameter();
  parameter->copyValueFrom(_mStepParameter->getParameterCopy());
}

void cedar::proc::experiment::action::SetParameter::preExperiment()
{
  if (!_mStepParameter->isParameterSelected())
  {
    return;
  }


  auto parameter = this->_mStepParameter->getParameter();
  if (!parameter)
  {
    CEDAR_THROW(cedar::aux::NotFoundException, "Could not find parameter \"" + this->_mStepParameter->getProperty() + "\" for set action.");
  }

  cedar::aux::Parameter::ReadLocker locker(parameter.get());
  parameter->writeToNode(this->mOriginalParameterValue);
}

void cedar::proc::experiment::action::SetParameter::postExperiment()
{
  if (!_mStepParameter->isParameterSelected())
  {
    return;
  }

  //!@todo This "complicated" approach is only necessary due to the asymmetry in readFromNode/writeToNode. Fix that!
  auto parameter = this->_mStepParameter->getParameter();
  cedar::aux::Parameter::WriteLocker locker(parameter.get());
  auto node_it = this->mOriginalParameterValue.find(parameter->getName());
  CEDAR_DEBUG_ASSERT(node_it != this->mOriginalParameterValue.not_found());
  parameter->readFromNode(node_it->second);
  locker.unlock();
  parameter->emitChangedSignal();
}
