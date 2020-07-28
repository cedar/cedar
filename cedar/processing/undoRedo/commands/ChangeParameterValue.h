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

    File:        ChangeParameterValue.h

    Maintainer:  Lars Janssen
    Email:       lars.janssen@ini.rub.de
    Date:        2020 07 28

    Description: Header file for the class cedar::proc::undoRedo::commands::ChangeParameterValue.

    Credits:

======================================================================================================================*/

#ifndef CEDAR_PROC_UNDO_REDO_COMMANDS_CHANGE_PARAMETER_VALUE_H
#define CEDAR_PROC_UNDO_REDO_COMMANDS_CHANGE_PARAMETER_VALUE_H

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/processing/undoRedo/UndoCommand.h"

// FORWARD DECLARATIONS
#include "cedar/processing/undoRedo/commands/ChangeParameterValue.fwd.h"
#include "cedar/auxiliaries/NumericParameter.fwd.h"

// SYSTEM INCLUDES


/*!@ Parameter change command
 *
 * UndoCommand Implementation for changing multiple types of Parameters
 */
template <typename T>
class cedar::proc::undoRedo::commands::ChangeParameterValue : public cedar::proc::undoRedo::UndoCommand
{
  //--------------------------------------------------------------------------------------------------------------------
  // nested types
  //--------------------------------------------------------------------------------------------------------------------

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  ChangeParameterValue(cedar::aux::NumericParameter<T>*, T oldValue, T newValue);

  //!@brief Destructor
  virtual ~ChangeParameterValue();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:

  void undo();
  void redo();

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

  cedar::aux::NumericParameter<T>* mpParameter;
  T mOldValue;
  T mNewValue;

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

private:
  // none yet

}; // class cedar::proc::undoRedo::commands::ChangeParameterValue

#endif // CEDAR_PROC_UNDO_REDO_COMMANDS_CHANGE_PARAMETER_VALUE_H

