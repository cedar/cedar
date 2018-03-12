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

    File:        CallElementAction.h

    Maintainer:  Stephan Zibner
    Email:       stephan.zibner@ini.rub.de
    Date:        2015 01 16

    Description: Header file for the class cedar::proc::experiment::CallElementAction.

    Credits:

======================================================================================================================*/

#ifndef CEDAR_PROC_EXPERIMENT_ACTION_CALL_ELEMENT_ACTION_H
#define CEDAR_PROC_EXPERIMENT_ACTION_CALL_ELEMENT_ACTION_H

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/processing/experiment/action/Action.h"
#include "cedar/processing/experiment/ConnectableParameter.h"
#include "cedar/processing/experiment/ActionParameter.h"
#include "cedar/auxiliaries/DoubleParameter.h"
#include "cedar/auxiliaries/StringParameter.h"

// FORWARD DECLARATIONS
#include "cedar/processing/experiment/action/CallElementAction.fwd.h"

// SYSTEM INCLUDES
#include <QObject>
#include <vector>
#include <string>


/*!@brief Calls an action of a connectable
 */
class cedar::proc::experiment::action::CallElementAction : public cedar::proc::experiment::action::Action
{
  //--------------------------------------------------------------------------------------------------------------------
  // nested types
  //--------------------------------------------------------------------------------------------------------------------

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  CallElementAction();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief Call action.
  void run();

  //!@brief checks the validity of this action and returns errors and warnings
  bool checkValidity(std::vector<std::string>& errors, std::vector<std::string>& warnings) const;

public:

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  //! Returns the step parameter chosen for this action.
  cedar::proc::experiment::ConnectableParameterPtr getConnectableParameter() const;

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
  //!@brief The step parameter to set
  cedar::proc::experiment::ConnectableParameterPtr _mConnectableParameter;
  cedar::proc::experiment::ActionParameterPtr _mActionParameter;

}; // class cedar::proc::experiment::CallElementAction

#endif // CEDAR_PROC_EXPERIMENT_ACTION_CALL_ELEMENT_ACTION_H

