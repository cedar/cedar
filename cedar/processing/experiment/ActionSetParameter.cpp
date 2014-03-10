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
_stepToSet
(
    new cedar::aux::StringParameter(this,"StepToSet","")
)
,
_parameterToSet
(
    new cedar::aux::StringParameter(this,"ParameterToSet","")
)
,
_desiredValue
(
    new cedar::aux::DoubleParameter(this,"DesiredValue",0.0)
)
{
}


cedar::proc::experiment::ActionSetParameter::~ActionSetParameter()
{
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------


void cedar::proc::experiment::ActionSetParameter::run()
{
  cedar::aux::ParameterPtr parameter = ExperimentControllerSingleton::getInstance()->
       getExperiment()->getStepParameter(_stepToSet->getValue(),_parameterToSet->getValue());
  if (cedar::aux::DoubleParameterPtr casted_parameter = boost::dynamic_pointer_cast<cedar::aux::DoubleParameter>(parameter))
  {
    casted_parameter->setValue(_desiredValue->getValue());
  }
}
