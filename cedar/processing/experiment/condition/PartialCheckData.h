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

    File:        ConditionPartialCheckMat.h

    Maintainer:  Christian Bodenstein
    Email:       christian.bodenstein@ini.rub.de
    Date:        2014 04 04

    Description: Header file for the class cedar::proc::experiment::ConditionPartialCheckMat.

    Credits:

======================================================================================================================*/

#ifndef CEDAR_PROC_EXPERIMENT_CONDITION_PARTIAL_CHECK_MAT_H
#define CEDAR_PROC_EXPERIMENT_CONDITION_PARTIAL_CHECK_MAT_H

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/processing/experiment/condition/Condition.h"
#include "cedar/processing/experiment/StepPropertyParameter.h"
#include "cedar/auxiliaries/DoubleParameter.h"
#include "cedar/auxiliaries/UIntParameter.h"
#include "cedar/auxiliaries/StringParameter.h"
#include "cedar/auxiliaries/EnumParameter.h"

// FORWARD DECLARATIONS
#include "cedar/processing/experiment/condition/PartialCheckData.fwd.h"

// SYSTEM INCLUDES


/*!@brief Checks if a part of the step data fulfills the condition
 */
class cedar::proc::experiment::condition::PartialCheckData : public cedar::proc::experiment::condition::Condition
{
  //--------------------------------------------------------------------------------------------------------------------
  // nested types
  //--------------------------------------------------------------------------------------------------------------------

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  PartialCheckData();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  //@brief Checks if a part of the step data fulfills the condition
  bool check();

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

  //@brief The number of elements that should at least need to fulfill the condition
  cedar::aux::UIntParameterPtr _mNumberOfElements;

  //!@brief The compare method. Can be greater, lower or equal
  cedar::aux::EnumParameterPtr _mCompareMethode;

  //!@brief The value the step data is compared to
  cedar::aux::DoubleParameterPtr _desiredValue;

}; // class cedar::proc::experiment::ConditionPartialCheckMat

#endif // CEDAR_PROC_EXPERIMENT_CONDITION_PARTIAL_CHECK_MAT_H

