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

    File:        ChangeObjectParameterValue.h

    Maintainer:  Yogeshwar Agnihotri,
    						 Lars Janssen
    Email:       yogeshwar.agnihotri@ini.ruhr-uni-bochum.de,
    						 lars.janssen@ini.rub.de
    Date:        2020 07 28

    Description: Header file for the class cedar::proc::undoRedo::commands::ChangeParameterValue.

    Credits:

======================================================================================================================*/

#ifndef CEDAR_PROC_UNDO_REDO_COMMANDS_CHANGE_OBJECT_PARAMETER_VALUE_H
#define CEDAR_PROC_UNDO_REDO_COMMANDS_CHANGE_OBJECT_PARAMETER_VALUE_H

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/processing/undoRedo/commands/ChangeParameterValue.h"
#include "cedar/auxiliaries/ObjectParameter.h"

// FORWARD DECLARATIONS
#include "cedar/processing/undoRedo/commands/ChangeObjectParameterValue.fwd.h"


// SYSTEM INCLUDES

/*!@ Parameter change command
 *
 * UndoCommand Implementation for ObjectParameter
 */
class cedar::proc::undoRedo::commands::ChangeObjectParameterValue :
        public cedar::proc::undoRedo::commands::ChangeParameterValue<std::string, cedar::aux::ObjectParameter>
{
  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:

  ChangeObjectParameterValue(cedar::aux::ObjectParameter* parameter, std::string oldValue, std::string newValue,
                             cedar::aux::NamedConfigurable* owner, cedar::proc::gui::Scene* scene = nullptr)
  :
  ChangeParameterValue(parameter, oldValue, newValue, owner, scene)
  {

  }


  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:

  void undoAction() override
  {
    this->mpParameter->setType(this->mOldValue);
  }

  void redoAction() override
  {
    this->mpParameter->setType(this->mNewValue);
  }

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

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

private:
  // none yet

}; // class cedar::proc::undoRedo::commands::ChangeObjectParameterValue

#endif // CEDAR_PROC_UNDO_REDO_COMMANDS_CHANGE_OBJECT_PARAMETER_VALUE_H

