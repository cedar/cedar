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
#include "cedar/auxiliaries/NamedConfigurable.h"
#include "cedar/processing/gui/Settings.h"
#include "cedar/processing/gui/Scene.h"
#include "cedar/processing/gui/Element.h"
#include "cedar/processing/Element.h"
#include "cedar/processing/gui/Connectable.h"
#include "cedar/auxiliaries/Configurable.h"
#include "cedar/processing/gui/GraphicsBase.h"

// FORWARD DECLARATIONS
#include "cedar/processing/undoRedo/commands/ChangeParameterValue.fwd.h"

// SYSTEM INCLUDES
#include <string>

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
  ChangeParameterValue(ParameterType* parameter, ValueType oldValue, ValueType newValue,
                       cedar::proc::gui::Scene* scene = nullptr)
  :
  mpParameter(parameter),
  mOldValue(oldValue),
  mNewValue(newValue),
  mpScene(scene),
  mLockSet(false),
  isInitialRedo(true)
  {
    mParentName = getParentName(parameter);
    mParameterName = parameter->getName();
    setText(QString::fromStdString(mParameterName + ":" + mParentName));
  }

  ChangeParameterValue(ParameterType* parameter, ValueType oldValue, ValueType newValue, bool lock, cedar::proc::gui::Scene* scene = nullptr)
  :
  mpParameter(parameter),
  mOldValue(oldValue),
  mNewValue(newValue),
  mpScene(scene),
  mLockSet(true),
  mLock(lock),
  isInitialRedo(true)
  {
    mParentName = getParentName(parameter);
    mParameterName = parameter->getName();
    setText(QString::fromStdString(mParameterName + ":" + mParentName));
  }

  //!@brief Destructor
  virtual ~ChangeParameterValue()
  {
  }

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:

  void updateParameterPointer()
  {
    if(this->mpScene != nullptr)
    {
      QList<QGraphicsItem *> sceneItems = this->mpScene->items();
      for (QGraphicsItem *item : sceneItems)
      {
        if (auto gui_element = dynamic_cast<cedar::proc::gui::Element*>(item))
        {
          cedar::proc::Element* element = gui_element->getElement().get();
          if (element != nullptr)
          {
            if(!element->getName().compare(this->mParentName))
            {
              this->mpParameter = element->getParameter<ParameterType>(this->mParameterName).get();
              this->mpParentElement = gui_element;
              return;
            }
          }
        }
      }
    }
  }

  void undo()
  {
    this->isInitialRedo = false;

    this->updateParameterPointer();
    if(this->mpParameter != nullptr)
    {
      if (this->mLockSet)
      {
        this->mpParameter->setValue(this->mOldValue, this->mLock);
      }
      else
      {
        this->mpParameter->setValue(this->mOldValue);
      }
      this->mpParameter->emitChangedSignal();
      if(this->mpParentElement != nullptr && this->mpScene != nullptr)
      {
        this->mpScene->selectNone();
        this->mpParentElement->setSelected(true);
      }
    }
    else
    {
      std::cout << "parameter is null" << std::endl;
    }
  }

  void redo()
  {
    this->updateParameterPointer();
    if(this->mpParameter != nullptr)
    {
      if (this->mLockSet)
      {
        this->mpParameter->setValue(this->mNewValue, this->mLock);
      }
      else
      {
        this->mpParameter->setValue(this->mNewValue);
      }
      this->mpParameter->emitChangedSignal();
      if(!this->isInitialRedo && this->mpParentElement != nullptr && this->mpScene != nullptr)
      {
        this->mpScene->selectNone();
        this->mpParentElement->setSelected(true);
      }
    }
    else
    {
      std::cout << "parameter is null" << std::endl;
    }
  }

  bool mergeWith(const QUndoCommand* other)
  {
    if(!cedar::proc::gui::SettingsSingleton::getInstance()->getUndoRedoAutoMacro())
    {
      return false;
    }
    if(auto command = dynamic_cast<const cedar::proc::undoRedo::commands::ChangeParameterValue<ValueType, ParameterType>*>(other))
    {
      if(!getMacroIdentifier().compare(command->getMacroIdentifier()))
      {
        this->mNewValue = command->mNewValue;
        return true;
      }
    }
    return false;
  }

  //TODO cpp file?

  std::string getMacroIdentifier() const override
  {
    if(!this->mParentName.compare(""))
    {
      return "";
    }
    return this->mParentName + "." + this->mParameterName;
  }

  std::string getParentName(ParameterType* parameter) const
  {
    if(auto namedConfig = dynamic_cast<cedar::aux::NamedConfigurable*>(parameter->getOwner()))
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
  std::string mParentName;
  std::string mParameterName;
  ParameterType* mpParameter;
  cedar::proc::gui::Element* mpParentElement;

  ValueType mOldValue;
  ValueType mNewValue;
  bool mLock;
  bool mLockSet;
  bool isInitialRedo;

  cedar::proc::gui::Scene* mpScene;

private:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

private:
  // none yet

}; // class cedar::proc::undoRedo::commands::ChangeParameterValue

#endif // CEDAR_PROC_UNDO_REDO_COMMANDS_CHANGE_PARAMETER_VALUE_H

