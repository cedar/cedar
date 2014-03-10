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

    File:        ConditionCheckValue.cpp

    Maintainer:  Christian Bodenstein
    Email:       christian.bodenstein@ini.rub.de
    Date:        2014 02 06

    Description: Source file for the class cedar::proc::experiment::ConditionCheckValue.

    Credits:

======================================================================================================================*/

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/processing/experiment/ConditionCheckValue.h"
#include "cedar/processing/experiment/ExperimentController.h"
#include "cedar/auxiliaries/Data.h"
#include "cedar/auxiliaries/MatData.h"
#include "cedar/processing/experiment/Experiment.h"
#include "cedar/processing/experiment/ExperimentController.h"

// SYSTEM INCLUDES
#include <QReadWriteLock>

//----------------------------------------------------------------------------------------------------------------------
// register the class
//----------------------------------------------------------------------------------------------------------------------
namespace
{
  bool declared = cedar::proc::experiment::ConditionManagerSingleton::getInstance()->
    registerType<cedar::proc::experiment::ConditionCheckValuePtr>();
}

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::proc::experiment::ConditionCheckValue::ConditionCheckValue()
:
_stepToCheck
(
    new cedar::aux::StringParameter(this,"StepToCheck","")
)
,
_parameterToCheck
(
    new cedar::aux::StringParameter(this,"ParameterToCheck","")
)
,
_desiredValue
(
    new cedar::aux::DoubleParameter(this,"DesiredValue",0.0)
)
{
}

cedar::proc::experiment::ConditionCheckValue::~ConditionCheckValue()
{
}
//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------


bool cedar::proc::experiment::ConditionCheckValue::check()
{
  Experiment* experiment = ExperimentControllerSingleton::getInstance()->getExperiment();
  if (cedar::aux::DataPtr data = experiment->
      getStepValue(_stepToCheck->getValue(),_parameterToCheck->getValue()))
  {
    if (cedar::aux::MatDataPtr value = boost::dynamic_pointer_cast<cedar::aux::MatData>(data))
    {
      QReadLocker locker(&(value->getLock()));
      const double cVal = value->getValue<double>(0,0);
      if(cVal > _desiredValue->getValue())
      {
        return true;
      }
    }
  }
  return false;
}

