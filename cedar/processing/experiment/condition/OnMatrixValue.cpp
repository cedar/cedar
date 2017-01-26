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

    File:        OnMatrixValue.cpp

    Maintainer:  Christian Bodenstein
    Email:       christian.bodenstein@ini.rub.de
    Date:        2014 04 04

    Description: Source file for the class cedar::proc::experiment::OnMatrixValue.

    Credits:

======================================================================================================================*/

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/processing/experiment/condition/OnMatrixValue.h"
#include "cedar/processing/experiment/Experiment.h"
#include "cedar/auxiliaries/Data.h"
#include "cedar/auxiliaries/MatData.h"

// SYSTEM INCLUDES

//----------------------------------------------------------------------------------------------------------------------
// register the class
//----------------------------------------------------------------------------------------------------------------------
namespace
{
  bool register_it()
  {
    cedar::proc::experiment::condition::ConditionManagerSingleton::getInstance()->
          registerType<cedar::proc::experiment::condition::OnMatrixValuePtr>();

    cedar::proc::experiment::condition::ConditionManagerSingleton::getInstance()->
        addDeprecatedName<cedar::proc::experiment::condition::OnMatrixValuePtr>("cedar.proc.experiment.condition.CheckData");

    return true;
  }
  bool declared = register_it();
}

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::proc::experiment::condition::OnMatrixValue::OnMatrixValue()
:
_stepData(new cedar::proc::experiment::StepPropertyParameter(this,"Step Data")),
_mCheckFullMatrix(new cedar::aux::BoolParameter(this,"check full matrix", true)),
_mNumberOfElements(new cedar::aux::UIntParameter(this,"number of elements",1)),
_mCompareMethode
(
  new cedar::aux::EnumParameter
  (
    this,
    "compare operator",
    cedar::proc::experiment::Experiment::CompareMethod::typePtr(),
    cedar::proc::experiment::Experiment::CompareMethod::Greater
  )
),
_mDesiredValue(new cedar::aux::DoubleParameter(this,"value",0.0))
{
  _stepData->setType(cedar::proc::experiment::StepPropertyParameter::OUTPUT);
  this->toggleNumberOfElements();
  QObject::connect(this->_mCheckFullMatrix.get(), SIGNAL(valueChanged()), this, SLOT(toggleNumberOfElements()));
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

bool cedar::proc::experiment::condition::OnMatrixValue::checkValidity(std::vector<std::string>& errors, std::vector<std::string>& /* warnings */) const
{
  cedar::aux::ConstDataPtr data = this->_stepData->getData();

  if (!data)
  {
    errors.push_back("No proper data slot selected in OnMatrixValue.");
    return false;
  }

  if (boost::dynamic_pointer_cast<cedar::aux::ConstMatData>(data))
  {
    return true;
  }
  else
  {
    errors.push_back("Selected data slot does not contain matrix data.");
    return false;
  }
}

bool cedar::proc::experiment::condition::OnMatrixValue::check() const
{
  if (cedar::aux::ConstDataPtr data = _stepData->getData())
  {
    if (cedar::aux::ConstMatDataPtr value = boost::dynamic_pointer_cast<cedar::aux::ConstMatData>(data))
    {
      if (_mCheckFullMatrix->getValue())
      {
        bool inRange = false;
        QReadLocker locker(&(value->getLock()));
        switch (_mCompareMethode->getValue())
        {
         case cedar::proc::experiment::Experiment::CompareMethod::Lower:
         {
           inRange = cv::checkRange(value->getData(), true, nullptr, -DBL_MAX, _mDesiredValue->getValue());
           break;
         }
         case cedar::proc::experiment::Experiment::CompareMethod::Greater:
         {
           inRange = cv::checkRange(value->getData(), true, nullptr, _mDesiredValue->getValue(), DBL_MAX);
           break;
         }
         case cedar::proc::experiment::Experiment::CompareMethod::Equal:
         {
           inRange = cv::checkRange(value->getData(), true, nullptr, _mDesiredValue->getValue(), _mDesiredValue->getValue());
           break;
         }
        }
        if (inRange)
        {
          return true;
        }
      }
      else
      {
        QReadLocker locker(&(value->getLock()));
        cv::Mat matrix = value->getData();
        unsigned int counter = 0;

        // Iterate through all matrix elements
        for (int x = 0; x < matrix.rows; x++)
        {
          for (int y = 0; y < matrix.cols; y++)
          {
            double current_val =value->getValue<double>(x,y);
            bool in_range = false;
            switch(_mCompareMethode->getValue())
            {
             case cedar::proc::experiment::Experiment::CompareMethod::Lower:
             {
               in_range = current_val < _mDesiredValue->getValue();
               break;
             }
             case cedar::proc::experiment::Experiment::CompareMethod::Greater:
             {
               in_range = current_val > _mDesiredValue->getValue();
               break;
             }
             case cedar::proc::experiment::Experiment::CompareMethod::Equal:
             {
               in_range = current_val == _mDesiredValue->getValue();
               break;
              }
            }
            if (in_range)
            {
              counter++;
            }
          }
        }
        if (counter >= _mNumberOfElements->getValue())
        {
          return true;
        }
      }
    }
  }
  return false;
}

void cedar::proc::experiment::condition::OnMatrixValue::toggleNumberOfElements()
{
  this->_mNumberOfElements->setConstant(_mCheckFullMatrix->getValue());
}
