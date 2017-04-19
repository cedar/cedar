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

    File:        OnTrialNumber.h

    Maintainer:  Christian Bodenstein
    Email:       christian.bodenstein@ini.rub.de
    Date:        2014 03 28

    Description: Header file for the class cedar::proc::experiment::OnTrialNumber.

    Credits:

======================================================================================================================*/

#ifndef CEDAR_PROC_EXPERIMENT_CONDITION_ON_TRIAL_NUMBER_H
#define CEDAR_PROC_EXPERIMENT_CONDITION_ON_TRIAL_NUMBER_H

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/processing/experiment/condition/Condition.h"
#include "cedar/auxiliaries/UIntParameter.h"

// FORWARD DECLARATIONS
#include "cedar/processing/experiment/condition/OnTrialNumber.fwd.h"

// SYSTEM INCLUDES
#include <vector>
#include <string>


/*!@brief Checks if the current trial has the number provided by this condition
 *
 */
class cedar::proc::experiment::condition::OnTrialNumber : public cedar::proc::experiment::condition::Condition
{
  //--------------------------------------------------------------------------------------------------------------------
  // nested types
  //--------------------------------------------------------------------------------------------------------------------

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  OnTrialNumber();

  //!@brief Destructor
  virtual ~OnTrialNumber();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  bool initialCheck() const;

  bool checkValidity(std::vector<std::string>& errors, std::vector<std::string>& warnings) const;

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  //!@brief Checks if the current trial has the number provided by this condition
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
 //!@brief The trial number that has to been reached
 cedar::aux::UIntParameterPtr _mTrial;

}; // class cedar::proc::experiment::OnTrialNumber

#endif // CEDAR_PROC_EXPERIMENT_CONDITION_ON_TRIAL_NUMBER_H

