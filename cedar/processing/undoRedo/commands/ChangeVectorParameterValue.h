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

    File:        ChangeVectorParameterValue.h

    Maintainer:  Lars Janssen
    Email:       lars.janssen@ini.rub.de
    Date:        2020 08 15

    Description: Header file for the class cedar::proc::undoRedo::commands::ChangeVectorParameterValue.

    Credits:

======================================================================================================================*/

#ifndef CEDAR_PROC_UNDO_REDO_COMMANDS_CHANGE_VECTOR_PARAMETER_VALUE_H
#define CEDAR_PROC_UNDO_REDO_COMMANDS_CHANGE_VECTOR_PARAMETER_VALUE_H

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/processing/undoRedo/UndoCommand.h"
#include "cedar/auxiliaries/VectorParameter.h"

// FORWARD DECLARATIONS
#include "cedar/processing/undoRedo/commands/ChangeVectorParameterValue.fwd.h"

// SYSTEM INCLUDES


/*!@ Parameter change command
 *
 * UndoCommand Implementation for changing VectorParameters (double, uint, ...)
 */
template <typename ValueT>
class cedar::proc::undoRedo::commands::ChangeVectorParameterValue : public cedar::proc::undoRedo::UndoCommand
{
  //--------------------------------------------------------------------------------------------------------------------
  // nested types
  //--------------------------------------------------------------------------------------------------------------------

  //! Type of the values stored in this widget.
  typedef ValueT ValueType;

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  ChangeVectorParameterValue(cedar::aux::VectorParameter<ValueType>* parameter,
                             std::vector<ValueType> oldValue, std::vector<ValueType> newValue)
  {
    this->mpParameter = parameter;
    this->mOldValue = oldValue;
    this->mNewValue = newValue;
  }

  //!@brief Destructor
  ChangeVectorParameterValue()
  {
  }

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:

  void undo()
  {
    this->mpParameter->setValue(this->mOldValue);
  }
  void redo()
  {
    this->mpParameter->setValue(this->mNewValue);
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

  cedar::aux::VectorParameter<ValueType>* mpParameter;
  std::vector<ValueType> mOldValue;
  std::vector<ValueType> mNewValue;

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

private:
  // none yet

}; // class cedar::proc::undoRedo::commands::ChangeVectorParameterValue

#endif // CEDAR_PROC_UNDO_REDO_COMMANDS_CHANGE_VECTOR_PARAMETER_VALUE_H

