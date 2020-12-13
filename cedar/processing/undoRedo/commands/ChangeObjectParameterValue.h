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

#ifndef CEDAR_PROC_UNDO_REDO_COMMANDS_CHANGE_OBJECT_PARAMETER_VALUE_H
#define CEDAR_PROC_UNDO_REDO_COMMANDS_CHANGE_OBJECT_PARAMETER_VALUE_H

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/processing/undoRedo/UndoCommand.h"
#include "cedar/auxiliaries/ObjectParameter.h"

// FORWARD DECLARATIONS
#include "cedar/processing/undoRedo/commands/ChangeObjectParameterValue.fwd.h"

// SYSTEM INCLUDES

/*!@ Parameter change command
 *
 * UndoCommand Implementation for ObjectParameter
 */
class cedar::proc::undoRedo::commands::ChangeObjectParameterValue : public cedar::proc::undoRedo::UndoCommand
{
  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  ChangeObjectParameterValue(cedar::aux::ObjectParameter* parameter, std::string oldValue, std::string newValue)
  :
  mpParameter(parameter),
  mOldValue(oldValue),
  mNewValue(newValue)
  {
    setText(QString::fromStdString(mpParameter->getName() + ":" + getOwnerName()));
  }

  //!@brief Destructor
  virtual ~ChangeObjectParameterValue()
  {
  }

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:

  void undo()
  {
    this->mpParameter->setType(this->mOldValue);
  }

  void redo()
  {
    this->mpParameter->setType(this->mNewValue);
  }

  bool mergeWith(const QUndoCommand* other)
  {
    if(!cedar::proc::gui::SettingsSingleton::getInstance()->getUndoRedoAutoMacro())
    {
      return false;
    }
    if(auto command = dynamic_cast<const cedar::proc::undoRedo::commands::ChangeObjectParameterValue*>(other))
    {
      if(!getMacroIdentifier().compare(command->getMacroIdentifier()))
      {
        CEDAR_ASSERT(this->mpParameter == command->mpParameter)
        this->mNewValue = command->mNewValue;
        return true;
      }
    }
    return false;
  }

  std::string getMacroIdentifier() const override
  {
    std::string macroId = getOwnerName();
    if(!macroId.compare(""))
    {
      return "";
    }
    return macroId + "." + this->mpParameter->getName();
  }

  std::string getOwnerName() const
  {
    if(auto namedConfig = dynamic_cast<cedar::aux::NamedConfigurable*>(this->mpParameter->getOwner()))
    {
      return namedConfig->getName();
    }
    else
    {
      return "";
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

  cedar::aux::ObjectParameter* mpParameter;
  std::string mOldValue;
  std::string mNewValue;

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

private:
  // none yet

}; // class cedar::proc::undoRedo::commands::ChangeObjectParameterValue

#endif // CEDAR_PROC_UNDO_REDO_COMMANDS_CHANGE_OBJECT_PARAMETER_VALUE_H

