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
#include "cedar/processing/experiment/condition/CheckData.h"
#include "cedar/auxiliaries/Data.h"
#include "cedar/auxiliaries/MatData.h"
#include "cedar/processing/experiment/Experiment.h"

// SYSTEM INCLUDES
#include <QReadWriteLock>

//----------------------------------------------------------------------------------------------------------------------
// register the class
//----------------------------------------------------------------------------------------------------------------------
namespace
{
  bool declared = cedar::proc::experiment::condition::ConditionManagerSingleton::getInstance()->
    registerType<cedar::proc::experiment::condition::CheckDataPtr>();
}

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::proc::experiment::condition::CheckData::CheckData()
:

_stepData
(
    new cedar::proc::experiment::StepPropertyParameter(this,"StepProperty")
)
,
_mCompareMethode
(
  new cedar::aux::EnumParameter
  (
    this,
    "reset type",
    cedar::proc::experiment::Experiment::CompareMethod::typePtr(),
    cedar::proc::experiment::Experiment::CompareMethod::Lower
  )
)
,
_desiredValue
(
    new cedar::aux::DoubleParameter(this,"DesiredValue",0.0)
)

{
  _stepData->setType(cedar::proc::experiment::StepPropertyParameter::OUTPUT);
}

cedar::proc::experiment::condition::CheckData::~CheckData()
{
}
//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------


bool cedar::proc::experiment::condition::CheckData::check()
{
  if (cedar::aux::ConstDataPtr data = _stepData->getData())
  {
    if (cedar::aux::ConstMatDataPtr value = boost::dynamic_pointer_cast<cedar::aux::ConstMatData>(data))
    {
      bool inRange = false;
      QReadLocker locker(&(value->getLock()));
      switch(_mCompareMethode->getValue())
      {
       case cedar::proc::experiment::Experiment::CompareMethod::Lower:
       {
         inRange = cv::checkRange(value->getData(),true,NULL,-DBL_MAX,_desiredValue->getValue());
         break;
       }
       case cedar::proc::experiment::Experiment::CompareMethod::Greater:
       {
         inRange = cv::checkRange(value->getData(),true,NULL,_desiredValue->getValue(),DBL_MAX);
         break;
       }
       case cedar::proc::experiment::Experiment::CompareMethod::Equal:
       {
         inRange = cv::checkRange(value->getData(),true,NULL,_desiredValue->getValue(),_desiredValue->getValue());
         break;
       }
      }
      if (inRange)
      {
        return true;
      }
    }
  }
  return false;
}

