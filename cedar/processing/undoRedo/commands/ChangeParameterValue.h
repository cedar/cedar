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
#include "cedar/auxiliaries/ParameterTemplate.h"

// FORWARD DECLARATIONS
#include "cedar/processing/undoRedo/commands/ChangeParameterValue.fwd.h"

// SYSTEM INCLUDES

/*!@ Parameter change command
 *
 * UndoCommand Implementation for changing multiple types of Parameters (PatameterTemplate<>)
 */
template <typename ValueT, typename ParameterT = cedar::aux::ParameterTemplate<ValueT>>
class cedar::proc::undoRedo::commands::ChangeParameterValue : public cedar::proc::undoRedo::UndoCommand
{
  //--------------------------------------------------------------------------------------------------------------------
  // nested types
  //--------------------------------------------------------------------------------------------------------------------

  //! Type of the values stored in this widget.
  typedef ValueT ValueType;
  typedef ParameterT ParameterType;

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  ChangeParameterValue(ParameterType* parameter, ValueType oldValue, ValueType newValue)
  :
  mpParameter(parameter),
  mOldValue(oldValue),
  mNewValue(newValue),
  mLockSet(false)
  {
  }

  ChangeParameterValue(ParameterType* parameter, ValueType oldValue, ValueType newValue, bool lock)
  :
  mpParameter(parameter),
  mOldValue(oldValue),
  mNewValue(newValue),
  mLockSet(true),
  mLock(lock)
  {
  }

  //!@brief Destructor
  virtual ~ChangeParameterValue()
  {
  }

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:

  void undo()
  {
    if(this->mLockSet)
    {
      this->mpParameter->setValue(this->mOldValue, this->mLock);
      //this->mpParameter->setValue(this->mOldValue);
    }
    else
    {
      this->mpParameter->setValue(this->mOldValue);
    }
    this->mpParameter->emitChangedSignal();
  }

  void redo()
  {
    if(this->mLockSet)
    {
      this->mpParameter->setValue(this->mNewValue, this->mLock);
      //this->mpParameter->setValue(this->mNewValue);
    }
    else
    {
      this->mpParameter->setValue(this->mNewValue);
    }
    this->mpParameter->emitChangedSignal();
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

  ParameterType* mpParameter;
  ValueType mOldValue;
  ValueType mNewValue;
  bool mLock;
  bool mLockSet;

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

private:
  // none yet

}; // class cedar::proc::undoRedo::commands::ChangeParameterValue

#endif // CEDAR_PROC_UNDO_REDO_COMMANDS_CHANGE_PARAMETER_VALUE_H

