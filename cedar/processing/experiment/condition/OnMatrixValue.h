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

    File:        OnMatrixValue.h

    Maintainer:  Christian Bodenstein
    Email:       christian.bodenstein@ini.rub.de
    Date:        2014 04 04

    Description: Header file for the class cedar::proc::experiment::OnMatrixValue.

    Credits:

======================================================================================================================*/

#ifndef CEDAR_PROC_EXPERIMENT_CONDITION_ON_MATRIX_VALUE_H
#define CEDAR_PROC_EXPERIMENT_CONDITION_ON_MATRIX_VALUE_H

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/processing/experiment/condition/Condition.h"
#include "cedar/processing/experiment/StepPropertyParameter.h"
#include "cedar/auxiliaries/DoubleParameter.h"
#include "cedar/auxiliaries/UIntParameter.h"
#include "cedar/auxiliaries/StringParameter.h"
#include "cedar/auxiliaries/EnumParameter.h"
#include "cedar/auxiliaries/BoolParameter.h"

// FORWARD DECLARATIONS
#include "cedar/processing/experiment/condition/OnMatrixValue.fwd.h"

// SYSTEM INCLUDES
#include <QObject>
#include <vector>
#include <string>


/*!@brief Checks if a part of the step data fulfills the condition
 */
class cedar::proc::experiment::condition::OnMatrixValue : public QObject, public cedar::proc::experiment::condition::Condition
{
  //--------------------------------------------------------------------------------------------------------------------
  // nested types
  //--------------------------------------------------------------------------------------------------------------------
  Q_OBJECT
  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  OnMatrixValue();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief checks validity of condition
  bool checkValidity(std::vector<std::string>& errors, std::vector<std::string>& warnings) const;

public slots:
  //!@brief sets constness of number of elements parameter depending on check full matrix parameter
  void toggleNumberOfElements();
  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  //!@brief Checks if a part of the step data fulfills the condition
  bool check() const;

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet
private:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

private:
  //@brief The step data to check
  cedar::proc::experiment::StepPropertyParameterPtr _stepData;

  cedar::aux::BoolParameterPtr _mCheckFullMatrix;

  //@brief The number of elements that should at least need to fulfill the condition
  cedar::aux::UIntParameterPtr _mNumberOfElements;

  //!@brief The compare method. Can be greater, lower or equal
  cedar::aux::EnumParameterPtr _mCompareMethode;

  //!@brief The value the step data is compared to
  cedar::aux::DoubleParameterPtr _mDesiredValue;

}; // class cedar::proc::experiment::OnMatrixValue

#endif // CEDAR_PROC_EXPERIMENT_CONDITION_ON_MATRIX_VALUE_H

