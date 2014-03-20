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

    File:        ActionIncreaseParameter.cpp

    Maintainer:  Christian Bodenstein
    Email:       christian.bodenstein@ini.rub.de
    Date:        2014 03 19

    Description: Source file for the class cedar::proc::experiment::ActionIncreaseParameter.

    Credits:

======================================================================================================================*/

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/processing/experiment/ActionIncreaseParameter.h"
#include "cedar/auxiliaries/DoubleParameter.h"
#include "cedar/auxiliaries/UIntParameter.h"
#include "cedar/auxiliaries/IntParameter.h"

// SYSTEM INCLUDES

//----------------------------------------------------------------------------------------------------------------------
// register class
//----------------------------------------------------------------------------------------------------------------------

namespace
{
  bool declared = cedar::proc::experiment::ActionManagerSingleton::getInstance()->
      registerType<cedar::proc::experiment::ActionIncreaseParameterPtr>();
}
//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::proc::experiment::ActionIncreaseParameter::ActionIncreaseParameter()
:
_mStepParamter
(
    new cedar::proc::experiment::StepPropertyParameter(this,"StepParameter")
)
{
  _mStepParamter->setType(cedar::proc::experiment::StepPropertyParameter::PARAMETER);
  _mStepParamter->allowType("cedar.aux.DoubleParameter");
  _mStepParamter->allowType("cedar.aux.UIntParameterPtr");
}

cedar::proc::experiment::ActionIncreaseParameter::~ActionIncreaseParameter()
{
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::proc::experiment::ActionIncreaseParameter::run()
{
  if (_mStepParamter->getStep() == "" || _mStepParamter->getProperty() == "")
  {
    return;
  }
  if(cedar::aux::DoubleParameterPtr parameter =
      boost::dynamic_pointer_cast<cedar::aux::DoubleParameter>(_mStepParamter->getParameter()))
  {
    cedar::aux::DoubleParameterPtr parameter_copy =
        boost::dynamic_pointer_cast<cedar::aux::DoubleParameter>(_mStepParamter->getParameterCopy());
    parameter->setValue(parameter->getValue() + parameter_copy->getValue());
  }

  if(cedar::aux::UIntParameterPtr parameter =
      boost::dynamic_pointer_cast<cedar::aux::UIntParameter>(_mStepParamter->getParameter()))
  {
    cedar::aux::UIntParameterPtr parameter_copy =
        boost::dynamic_pointer_cast<cedar::aux::UIntParameter>(_mStepParamter->getParameterCopy());
    parameter->setValue(parameter->getValue() + parameter_copy->getValue());
  }
}
