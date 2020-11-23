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

    File:        ToggleRelativePath.h

    Maintainer:  Lars Janssen
    Email:       lars.janssen@ini.rub.de
    Date:        2020 07 28

    Description: Header file for the class cedar::proc::undoRedo::commands::ChangeParameterValue.

    Credits:

======================================================================================================================*/

#ifndef CEDAR_PROC_UNDO_REDO_COMMANDS_TOGGLE_RELATIVE_PATH_H
#define CEDAR_PROC_UNDO_REDO_COMMANDS_TOGGLE_RELATIVE_PATH_H

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/processing/undoRedo/UndoCommand.h"
#include "cedar/auxiliaries/FileParameter.h"

// FORWARD DECLARATIONS
#include "cedar/processing/undoRedo/commands/ToggleRelativePath.fwd.h"

// SYSTEM INCLUDES
#include <QCheckBox>

/*!@ Parameter change command
 *
 * UndoCommand Implementation for changing the relative path checkbox in the FileParameter
 */

class cedar::proc::undoRedo::commands::ToggleRelativePath : public cedar::proc::undoRedo::UndoCommand
{
  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  ToggleRelativePath(QCheckBox *checkBox, bool targetState, cedar::aux::FileParameter* parameter)
  :
  mpCheckBox(checkBox),
  mTargetState(targetState),
  mpParameter(parameter)
  {
  }

  //!@brief Destructor
  virtual ~ToggleRelativePath()
  {
  }

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:

  void undo()
  {
    this->mpCheckBox->setChecked(!this->mTargetState);
    if(this->mTargetState)
    {
      this->mpParameter->setPathMode(cedar::aux::FileParameter::PathMode::PATH_MODE_ABSOLUTE);
    }
    else
    {
      this->mpParameter->setPathMode(cedar::aux::FileParameter::PathMode::PATH_MODE_RELATIVE_TO_CURRENT_ARCHITECTURE_DIR);
    }
  }

  void redo()
  {
    this->mpCheckBox->setChecked(this->mTargetState);
    if(this->mTargetState)
    {
      this->mpParameter->setPathMode(cedar::aux::FileParameter::PathMode::PATH_MODE_RELATIVE_TO_CURRENT_ARCHITECTURE_DIR);
    }
    else
    {
      this->mpParameter->setPathMode(cedar::aux::FileParameter::PathMode::PATH_MODE_ABSOLUTE);
    }
  }

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

  QCheckBox* mpCheckBox;
  bool mTargetState;
  cedar::aux::FileParameter* mpParameter;

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

private:
  // none yet

}; // class cedar::proc::undoRedo::commands::ToggleRelativePath

#endif // CEDAR_PROC_UNDO_REDO_COMMANDS_TOGGLE_RELATIVE_PATH_H


