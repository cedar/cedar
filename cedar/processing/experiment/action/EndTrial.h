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

    File:        EndTrial.h

    Maintainer:  Christian Bodenstein
    Email:       christian.bodenstein@ini.rub.de
    Date:        2014 03 09

    Description: Header file for the class cedar::proc::experiment::EndTrial.

    Credits:

======================================================================================================================*/

#ifndef CEDAR_PROC_EXPERIMENT_ACTION_END_TRIAL_H
#define CEDAR_PROC_EXPERIMENT_ACTION_END_TRIAL_H

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/processing/experiment/action/Action.h"
#include "cedar/auxiliaries/EnumParameter.h"
#include "cedar/auxiliaries/BoolParameter.h"
#include "cedar/auxiliaries/StringParameter.h"

// FORWARD DECLARATIONS
#include "cedar/processing/experiment/action/EndTrial.fwd.h"

// SYSTEM INCLUDES


/*!@brief Stops a trial of the experiment framework
 *
 *        It also provides different kinds of reset methods
 */
class cedar::proc::experiment::action::EndTrial : public cedar::proc::experiment::action::Action
{
  //--------------------------------------------------------------------------------------------------------------------
  // nested types
  //--------------------------------------------------------------------------------------------------------------------

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  EndTrial();

  //!@brief Destructor
  virtual ~EndTrial();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief Stops the trial, applies the reset method and send a message to the log file
  void run();

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
  //!@brief The reset method that should be applied right after the stop
  cedar::aux::EnumParameterPtr _mResetType;

  //!@brief Indicates if the trial is a success or a failure
  cedar::aux::BoolParameterPtr _mSuccess;

  //!@brief The message that will be send to the log file
  cedar::aux::StringParameterPtr _mMessage;

  //!@brief Indicates if the triggers are stopped after this trial
  cedar::aux::BoolParameterPtr _mStopTriggers;

}; // class cedar::proc::experiment::ActionStop

#endif // StopAllTriggers

