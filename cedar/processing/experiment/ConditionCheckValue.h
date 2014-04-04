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

    File:        ConditionCheckValue.h

    Maintainer:  Christian Bodenstein
    Email:       christian.bodenstein@ini.rub.de
    Date:        2014 02 06

    Description: Header file for the class cedar::proc::experiment::ConditionCheckValue.

    Credits:

======================================================================================================================*/

#ifndef CEDAR_PROC_EXPERIMENT_CONDITION_CHECK_VALUE_H
#define CEDAR_PROC_EXPERIMENT_CONDITION_CHECK_VALUE_H

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/processing/experiment/Condition.h"
#include "cedar/auxiliaries/DoubleParameter.h"
#include "cedar/auxiliaries/StringParameter.h"
#include "cedar/auxiliaries/EnumParameter.h"
#include "cedar/processing/experiment/StepPropertyParameter.h"

// FORWARD DECLARATIONS
#include "cedar/processing/experiment/ConditionCheckValue.fwd.h"

// SYSTEM INCLUDES


/*!@todo describe.
 *
 * @todo describe more.
 */
class cedar::proc::experiment::ConditionCheckValue : public cedar::proc::experiment::Condition
{

  //--------------------------------------------------------------------------------------------------------------------
  // nested types
  //--------------------------------------------------------------------------------------------------------------------

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  ConditionCheckValue();

  //!@brief Destructor
  virtual ~ConditionCheckValue();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  bool check();


  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

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
  cedar::proc::experiment::StepPropertyParameterPtr _stepValue;
  cedar::aux::EnumParameterPtr _mCompareMethode;
  cedar::aux::DoubleParameterPtr _desiredValue;

}; // class cedar::proc::experiment::ConditionCheckValue

#endif // CEDAR_PROC_EXPERIMENT_CONDITION_CHECK_VALUE_H

