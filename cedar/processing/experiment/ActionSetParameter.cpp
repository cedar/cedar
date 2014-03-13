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
#include "cedar/processing/experiment/ActionSetParameter.h"
#include "cedar/processing/experiment/ExperimentController.h"
#include "cedar/auxiliaries/ParameterDeclaration.h"
#include "cedar/processing/experiment/StepPropertyParameter.h"

// SYSTEM INCLUDES

//----------------------------------------------------------------------------------------------------------------------
// register class
//----------------------------------------------------------------------------------------------------------------------

namespace
{
  bool declared = cedar::proc::experiment::ActionManagerSingleton::getInstance()->
      registerType<cedar::proc::experiment::ActionSetParameterPtr>();
}
//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::proc::experiment::ActionSetParameter::ActionSetParameter()
:
_mStepProperty
(
    new cedar::proc::experiment::StepPropertyParameter(this,"StepProperty")
)
,
_desiredValue
(
    new cedar::aux::StringParameter(this,"DesiredValue","")
)
{
  connect(_mStepProperty.get(),SIGNAL(valueChanged()),this,SLOT(updateParamter()));
}


cedar::proc::experiment::ActionSetParameter::~ActionSetParameter()
{
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------


void cedar::proc::experiment::ActionSetParameter::run()
{
  if (_mStepProperty->getStep() == "" || _mStepProperty->getProperty() == "")
  {
    return;
  }
  cedar::aux::ParameterPtr parameter = ExperimentControllerSingleton::getInstance()->
       getExperiment()->getStepParameter(_mStepProperty->getStep(),_mStepProperty->getProperty());
  parameter->copyValueFrom(_desiredValue);
}


void cedar::proc::experiment::ActionSetParameter::updateParamter()
{
  if (_mStepProperty->getStep() == "" || _mStepProperty->getProperty() == "")
  {
    return;
  }
  if(_desiredValue)
  {
    _desiredValue->unsetOwner();
  }
  cedar::aux::ParameterPtr parameter = ExperimentControllerSingleton::getInstance()->
         getExperiment()->getStepParameter(_mStepProperty->getStep(),_mStepProperty->getProperty());
  std::string type = cedar::aux::ParameterDeclarationManagerSingleton::getInstance()->getTypeId(parameter);
  _desiredValue = cedar::aux::ParameterDeclarationManagerSingleton::getInstance()->allocate(type);
  _desiredValue->setOwner(this);
  _desiredValue->setName("DesiredValue");
  _desiredValue->copyValueFrom(parameter);
}
