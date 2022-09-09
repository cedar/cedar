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

    File:        ChangeParameterValueTemplate.h

    Maintainer:  Lars Janssen
    Email:       lars.janssen@ini.rub.de
    Date:        2021 05 12

    Description: Header file for the class cedar::proc::undoRedo::commands::ChangeParameterValueTemplate.

    Credits:

======================================================================================================================*/

#ifndef CEDAR_PROC_UNDO_REDO_COMMANDS_CHANGE_PARAMETER_VALUE_TEMPLATE_H
#define CEDAR_PROC_UNDO_REDO_COMMANDS_CHANGE_PARAMETER_VALUE_TEMPLATE_H

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/processing/undoRedo/commands/ChangeParameterValue.h"

// FORWARD DECLARATIONS
#include "cedar/processing/undoRedo/commands/ChangeParameterValueTemplate.fwd.h"

// SYSTEM INCLUDES

/*!Parameter change command
 *
 * Implementation for the abstract class ChangeParameterValue. Can be used for numerous parameter types.
 * Currently not supported: ObjectParameter and ObjectListParameter. (use ChangeObject(List)Parameter class)
 */
template <typename ValueT, typename ParameterT = cedar::aux::ParameterTemplate<ValueT>>
class cedar::proc::undoRedo::commands::ChangeParameterValueTemplate : public ChangeParameterValue<ValueT, ParameterT>
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

  ChangeParameterValueTemplate(ParameterType* parameter, ValueType oldValue, ValueType newValue, cedar::aux::NamedConfigurable* owner,
                       cedar::proc::gui::Scene* scene = nullptr)
  :
  ChangeParameterValue<ValueT, ParameterT>(parameter, oldValue, newValue, owner, scene)
  {
  }

  ChangeParameterValueTemplate(ParameterType* parameter, ValueType oldValue, ValueType newValue, cedar::aux::NamedConfigurable* owner, bool lock,
          cedar::proc::gui::Scene* scene = nullptr)
  :
  ChangeParameterValue<ValueT, ParameterT>(parameter, oldValue, newValue, owner, lock, scene)
  {
  }

  //!@brief Destructor
  virtual ~ChangeParameterValueTemplate()
  {
  }

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:

  void undoAction()
  {
    if (this->mLockSet)
    {
      this->mpParameter->setValue(this->mOldValue, this->mLock);
    }
    else
    {
      this->mpParameter->setValue(this->mOldValue);
    }
  }

  void redoAction()
  {
    if (this->mLockSet)
    {
      this->mpParameter->setValue(this->mNewValue, this->mLock);
    }
    else
    {
      this->mpParameter->setValue(this->mNewValue);
    }
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
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

private:
  // none yet

}; // class cedar::proc::undoRedo::commands::ChangeParameterValueTemplate

#endif // CEDAR_PROC_UNDO_REDO_COMMANDS_CHANGE_PARAMETER_VALUE_TEMPLATE_H

