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
#include "cedar/auxiliaries/ObjectListParameter.h"
#include "cedar/auxiliaries/ObjectParameter.h"
#include "cedar/auxiliaries/math/TransferFunction.h"
#include "cedar/auxiliaries/kernel/Kernel.h"
#include "cedar/processing/auxiliaries/gui/ObjectListParameter.h"
#include "cedar/processing/auxiliaries/gui/ObjectParameter.h"
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
  ChangeParameterValue(ParameterType* parameter, ValueType oldValue, ValueType newValue, cedar::aux::NamedConfigurable* owner,
                       cedar::proc::gui::Scene* scene = nullptr)
  :
  mpParameter(parameter),
  mOldValue(oldValue),
  mNewValue(newValue),
  mpScene(scene),
  mLockSet(false),
  mIsInitialRedo(true)
  {
    CEDAR_ASSERT(mpParameter != nullptr)
    if(owner == nullptr)
    {
      owner = parameter->getNamedConfigurableOwner();
    }
    if(owner != nullptr)
    {
      if(auto element = dynamic_cast<cedar::proc::Element*>(owner))
      {
        this->mParentIdentifier = element->getFullPath();
      }
      else
      {
        this->mParentIdentifier = owner->getName();
      }
      this->mParameterIdentifier = owner->findParameterPath(parameter);
      this->setText(QString::fromStdString(this->mParentIdentifier + ": " + this->mParameterIdentifier)); //TODO change to something more readable, maybe include value?
    }
    else
    {
      this->setText(QString::fromStdString(parameter->getName() + ": "));
    }
  }

  ChangeParameterValue(ParameterType* parameter, ValueType oldValue, ValueType newValue, cedar::aux::NamedConfigurable* owner, bool lock,
                       cedar::proc::gui::Scene* scene = nullptr)
  :
  mpParameter(parameter),
  mOldValue(oldValue),
  mNewValue(newValue),
  mpScene(scene),
  mLockSet(true),
  mLock(lock),
  mIsInitialRedo(true)
  {
    CEDAR_ASSERT(mpParameter != nullptr)
    if(owner == nullptr)
    {
      owner = parameter->getNamedConfigurableOwner();
    }
    if(owner != nullptr)
    {
      if(auto element = dynamic_cast<cedar::proc::Element*>(owner))
      {
        this->mParentIdentifier = element->getFullPath();
      }
      else
      {
        this->mParentIdentifier = owner->getName();
      }
      this->mParameterIdentifier = owner->findParameterPath(parameter);
      this->setText(QString::fromStdString(this->mParentIdentifier + ": " + this->mParameterIdentifier));
    }
    else
    {
      this->mParentIdentifier = "";
      this->mParameterIdentifier = "";
      this->setText(QString::fromStdString(parameter->getName() + ": "));
    }
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
    if(!this->mParentIdentifier.compare(""))
    {
      return;
    }
    this->mpParameter = nullptr;

    this->mpParentElement = this->mpScene->getElementByFullPath(this->mParentIdentifier);

    if(this->mpParentElement != nullptr)
    {
      this->mpParameter = this->mpParentElement->getElement()->getParameter<ParameterType>(this->mParameterIdentifier).get(); //TODO error
    }
    else
    {
      if (this->mpScene != nullptr && this->mParentIdentifier.compare(""))
      {
        QList<QGraphicsItem *> sceneItems = this->mpScene->items();
        for (QGraphicsItem *item : sceneItems)
        {
          if (auto gui_element = dynamic_cast<cedar::proc::gui::Element *>(item))
          {
            cedar::proc::Element *element = gui_element->getElement().get();
            if (element != nullptr)
            {
              if (!element->getName().compare(this->mParentIdentifier))
              {
                this->mpParameter = element->getParameter<ParameterType>(this->mParameterIdentifier).get();
                this->mpParentElement = gui_element;
                return;
              }
            }
          }
        }
      }
    }
  }

  void undo()
  {
    this->mIsInitialRedo = false;

    this->updateParameterPointer();
    if(this->mpParameter != nullptr)
    {
      this->undoAction();
      this->mpParameter->emitChangedSignal();

      //If name property was changed, update name
      cedar::aux::NamedConfigurable* owner = this->mpParameter->getNamedConfigurableOwner();
      if(owner != nullptr)
      {
        if(auto element = dynamic_cast<cedar::proc::Element*>(owner))
        {
          this->mParentIdentifier = element->getFullPath();
        }
        else
        {
          this->mParentIdentifier = owner->getName();
        }
        this->mParameterIdentifier = owner->findParameterPath(this->mpParameter);
      }

      //Select owner in scene
      if(this->mpParentElement != nullptr && this->mpScene != nullptr)
      {
        QList<QGraphicsItem *> selected_items = this->mpScene->selectedItems();
        if(selected_items.size() != 1)
        {
          this->mpScene->selectNone();
          this->mpParentElement->setSelected(true);
        }
        else
        {
          if(selected_items.at(0) != this->mpParentElement)
          {
            this->mpScene->selectNone();
            this->mpParentElement->setSelected(true);
          }
        }
      }
    }
  }

  void redo()
  {
    this->updateParameterPointer();
    if(this->mpParameter != nullptr)
    {
      this->redoAction();
      this->mpParameter->emitChangedSignal();

      //If name property was changed, update name
      cedar::aux::NamedConfigurable* owner = this->mpParameter->getNamedConfigurableOwner();
      if(owner != nullptr)
      {
        if(auto element = dynamic_cast<cedar::proc::Element*>(owner))
        {
          this->mParentIdentifier = element->getFullPath();
        }
        else
        {
          this->mParentIdentifier = owner->getName();
        }
        this->mParameterIdentifier = owner->findParameterPath(this->mpParameter);
      }

      //Select owner in scene
      if(!this->mIsInitialRedo && this->mpParentElement != nullptr && this->mpScene != nullptr)
      {
        QList<QGraphicsItem *> selected_items = this->mpScene->selectedItems();
        if(selected_items.size() != 1)
        {
          this->mpScene->selectNone();
          this->mpParentElement->setSelected(true);
        }
        else
        {
          if(selected_items.at(0) != this->mpParentElement)
          {
            this->mpScene->selectNone();
            this->mpParentElement->setSelected(true);
          }
        }
      }
    }
  }

  virtual bool mergeWith(const QUndoCommand* other)
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

  //TODO comments

  std::string getMacroIdentifier() const override
  {
    return this->mParentIdentifier + "." + this->mParameterIdentifier;
  }

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:

  virtual void undoAction() = 0;

  virtual void redoAction() = 0;

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  std::string mParentIdentifier;
  std::string mParameterIdentifier;
  ParameterType* mpParameter;
  cedar::proc::gui::Element* mpParentElement;

  ValueType mOldValue;
  ValueType mNewValue;
  bool mLock;
  bool mLockSet;
  bool mIsInitialRedo;

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

