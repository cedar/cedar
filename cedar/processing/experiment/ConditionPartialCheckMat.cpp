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

    File:        ConditionPartialCheckMat.cpp

    Maintainer:  Christian Bodenstein
    Email:       christian.bodenstein@ini.rub.de
    Date:        2014 04 04

    Description: Source file for the class cedar::proc::experiment::ConditionPartialCheckMat.

    Credits:

======================================================================================================================*/

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/processing/experiment/ConditionPartialCheckMat.h"
#include "cedar/processing/experiment/Experiment.h"
#include "cedar/auxiliaries/Data.h"
#include "cedar/auxiliaries/MatData.h"

// SYSTEM INCLUDES

//----------------------------------------------------------------------------------------------------------------------
// register the class
//----------------------------------------------------------------------------------------------------------------------
namespace
{
  bool declared = cedar::proc::experiment::ConditionManagerSingleton::getInstance()->
    registerType<cedar::proc::experiment::ConditionPartialCheckMatPtr>();
}

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::proc::experiment::ConditionPartialCheckMat::ConditionPartialCheckMat()
:

_stepValue
(
    new cedar::proc::experiment::StepPropertyParameter(this,"StepProperty")
)
,
_mNumberOfElements(new cedar::aux::UIntParameter(this,"Elements at least",1))
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
_desiredValue( new cedar::aux::DoubleParameter(this,"DesiredValue",0.0))
{
  _stepValue->setType(cedar::proc::experiment::StepPropertyParameter::OUTPUT);
}

cedar::proc::experiment::ConditionPartialCheckMat::~ConditionPartialCheckMat()
{
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

bool cedar::proc::experiment::ConditionPartialCheckMat::check()
{
  if (cedar::aux::ConstDataPtr data = _stepValue->getData())
  {
    if (cedar::aux::ConstMatDataPtr value = boost::dynamic_pointer_cast<cedar::aux::ConstMatData>(data))
    {
      QReadLocker locker(&(value->getLock()));
      cv::Mat matrix = value->getData();
      unsigned int counter = 0;
      for(int x = 0; x < matrix.rows; x++)
      {
        for(int y = 0; y < matrix.cols; y++)
        {
          double current_val =value->getValue<double>(x,y);
          bool in_range = false;
          switch(_mCompareMethode->getValue())
          {
           case cedar::proc::experiment::Experiment::CompareMethod::Lower:
           {
             in_range = current_val < _desiredValue->getValue();
             break;
           }
           case cedar::proc::experiment::Experiment::CompareMethod::Greater:
           {
             in_range = current_val > _desiredValue->getValue();
             break;
           }
           case cedar::proc::experiment::Experiment::CompareMethod::Equal:
           {
             in_range = current_val == _desiredValue->getValue();
             break;
            }
          }
          if (in_range)
          {
            counter++;
          }
        }
      }
      if(counter >= _mNumberOfElements->getValue())
      {
        return true;
      }
    }
  }
  return false;
}

