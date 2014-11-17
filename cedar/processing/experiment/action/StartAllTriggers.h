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

    File:        ActionStart.h

    Maintainer:  Christian Bodenstein
    Email:       christian.bodenstein@ini.rub.de
    Date:        2014 02 06

    Description: Header file for the class cedar::proc::experiment::ActionStart.

    Credits:

======================================================================================================================*/

#ifndef CEDAR_PROC_EXPERIMENT_ACTION_START_ALL_TRIGGERS_H
#define CEDAR_PROC_EXPERIMENT_ACTION_START_ALL_TRIGGERS_H

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/processing/experiment/action/Action.h"

// FORWARD DECLARATIONS
#include "cedar/processing/experiment/action/StartAllTriggers.fwd.h"
#include "cedar/processing/experiment/Experiment.fwd.h"

// SYSTEM INCLUDES


/*!@brief Starts a trial of the experiment framework
 *
 *            This Action should only be called a single time in the initial condition
 */
class cedar::proc::experiment::action::StartAllTriggers : public cedar::proc::experiment::action::Action
{
  //--------------------------------------------------------------------------------------------------------------------
  // nested types
  //--------------------------------------------------------------------------------------------------------------------

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  StartAllTriggers();

  //!@brief Destructor
  virtual ~StartAllTriggers();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief Starts a trial of the experiment framework
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

}; // class cedar::proc::experiment::action::StartAllTriggers

#endif // CEDAR_PROC_EXPERIMENT_ACTION_START_ALL_TRIGGERS_H

