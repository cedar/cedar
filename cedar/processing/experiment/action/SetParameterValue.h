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

    File:        SetParameterValueValue.h

    Maintainer:  Christian Bodenstein
    Email:       christian.bodenstein@ini.rub.de
    Date:        2014 03 07

    Description: Header file for the class cedar::proc::experiment::SetParameterValueValue.

    Credits:

======================================================================================================================*/

#ifndef CEDAR_PROC_EXPERIMENT_ACTION_SET_PARAMETER_VALUE_H
#define CEDAR_PROC_EXPERIMENT_ACTION_SET_PARAMETER_VALUE_H

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/processing/experiment/action/Action.h"
#include "cedar/auxiliaries/DoubleParameter.h"
#include "cedar/auxiliaries/StringParameter.h"
#include "cedar/processing/experiment/StepPropertyParameter.h"

// FORWARD DECLARATIONS
#include "cedar/processing/experiment/action/SetParameterValue.fwd.h"

// SYSTEM INCLUDES
#include <QObject>
#include <vector>
#include <string>
#include <cedar/auxiliaries/BoolParameter.h>


/*!@brief Sets the parameter of a step to a desired value
 */
class cedar::proc::experiment::action::SetParameterValue : public cedar::proc::experiment::action::Action
{
  //--------------------------------------------------------------------------------------------------------------------
  // nested types
  //--------------------------------------------------------------------------------------------------------------------

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  SetParameterValue();

//  SetParameterValue(cedar::proc::experiment::ExperimentPtr experiment);

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief Sets the parameter of a step to a desired value
  void run();

  void preExperiment();

  void postExperiment();

  bool checkValidity(std::vector<std::string>& errors, std::vector<std::string>& warnings) const;

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  //! Returns the step parameter chosen for this action.
  cedar::proc::experiment::StepPropertyParameterPtr getStepParameter() const;

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
  cedar::aux::ConfigurationNode mOriginalParameterValue;

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

private:
  //!@brief The step parameter to set
  cedar::proc::experiment::StepPropertyParameterPtr _mStepParameter;
  cedar::aux::BoolParameterPtr _mAppendTrialNumber;

}; // class cedar::proc::experiment::SetParameterValue

#endif // CEDAR_PROC_EXPERIMENT_ACTION_SET_PARAMETER_VALUE_H

