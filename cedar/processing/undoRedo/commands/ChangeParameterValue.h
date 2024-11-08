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

    Maintainer:  Yogeshwar Agnihotri,
    						 Lars Janssen
    Email:       yogeshwar.agnihotri@ini.ruhr-uni-bochum.de,
    						 lars.janssen@ini.rub.de
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
 * UndoCommand Implementation for changing multiple types of Parameters (ParameterTemplate<>)
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
  //!@brief The standard constructors.
  ChangeParameterValue(ParameterType* parameter, ValueType oldValue, ValueType newValue,
                       cedar::aux::NamedConfigurable* owner, cedar::proc::gui::Scene* scene = nullptr)
  :
  mpParameter(parameter),
  mpParentElement(nullptr),
  mOldValue(oldValue),
  mNewValue(newValue),
  mLockSet(false),
  mIsInitialRedo(true),
  mpScene(scene)
  {
    init(owner);
  }

  ChangeParameterValue(ParameterType* parameter, ValueType oldValue, ValueType newValue,
                       cedar::aux::NamedConfigurable* owner, bool lock, cedar::proc::gui::Scene* scene = nullptr)
  :
  mpParameter(parameter),
  mpParentElement(nullptr),
  mOldValue(oldValue),
  mNewValue(newValue),
  mLock(lock),
  mLockSet(true),
  mIsInitialRedo(true),
  mpScene(scene)
  {
    init(owner);
  }

  // This mainly sets the fullPaths for both the parent element and the parameter inside the parent element
  void init(cedar::aux::NamedConfigurable* owner)
  {
    CEDAR_ASSERT(this->mpParameter != nullptr)
    if(owner == nullptr)
    {
      owner = this->mpParameter->getNamedConfigurableOwner();
    }
    if(owner != nullptr)
    {
      // Found parent, get its full path
      if(auto element = dynamic_cast<cedar::proc::Element*>(owner))
      {
        this->mParentFullPath = element->getFullPath();
      }
      else
      {
        this->mParentFullPath = owner->getName();
      }
      this->mOldParentFullPath = this->mParentFullPath;
      this->mParameterFullPath = owner->findParameterPath(this->mpParameter);
      this->setText(QString::fromStdString("Parameter changed: " + this->mParentFullPath + "::" +
            this->mParameterFullPath));  //TODO Maybe include value?
    }
    else
    {
      // Did not find the parent
      this->mParentFullPath = "";
      this->mOldParentFullPath = "";
      this->mParameterFullPath = "";
      this->setText(QString::fromStdString("Parameter changed: unknown parent::" + this->mpParameter->getName()));
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
  // Update the pointers of the parameter and the parent element using the full paths
  void updateParameterPointer()
  {
    // If no parent was found earlier, do not attempt to update the pointer
    if(!this->mParentFullPath.compare(""))
    {
      CEDAR_THROW(cedar::aux::UnknownNameException, "Parameter was not found.");
      return;
    }
    this->mpParameter = nullptr;
    this->mpParentElement = this->mpScene->getElementByFullPath(this->mParentFullPath);

    if(this->mpParentElement != nullptr)
    {
      this->mpParameter = this->mpParentElement->getElement()->template getParameter<ParameterType>(
              this->mParameterFullPath).get();
    }
    else
    {
      //! this can probably be deleted, as getElementByFullPath() should already handle this
      if (this->mpScene != nullptr && this->mParentFullPath.compare(""))
      {
        QList<QGraphicsItem *> sceneItems = this->mpScene->items();
        for (QGraphicsItem *item : sceneItems)
        {
          if (auto gui_element = dynamic_cast<cedar::proc::gui::Element *>(item))
          {
            cedar::proc::Element *element = gui_element->getElement().get();
            if (element != nullptr)
            {
              if (!element->getName().compare(this->mParentFullPath))
              {
                this->mpParameter = element->getParameter<ParameterType>(this->mParameterFullPath).get();
                this->mpParentElement = gui_element;
                return;
              }
            }
          }
        }
      }
      this->mpParameter = nullptr;
      CEDAR_THROW(cedar::aux::UnknownNameException, "Parameter \"" + this->mParameterFullPath + "\" was not found.");
    }
  }

  void undo() override
  {
    this->mIsInitialRedo = false;

    this->updateParameterPointer();
    if(this->mpParameter != nullptr)
    {
      // Do the undo operation
      this->undoAction();

      // Handle things that have to be done after every parameter change
      this->postUndoRedo();

      // Select owner in scene
      this->selectParentElementInScene();
    }
  }

  void redo() override
  {
    this->updateParameterPointer();
    if(this->mpParameter != nullptr)
    {
      // Do the redo operation
      this->redoAction();
      // Handle things that have to be done after every parameter change
      this->postUndoRedo();

      //Select owner in scene
      if(!this->mIsInitialRedo)
      {
        this->selectParentElementInScene();
      }
    }
  }

  void postUndoRedo()
  {
    //If name property was changed, update name
    this->updateFullPaths();
  }

  void updateFullPaths()
  {
    cedar::aux::NamedConfigurable* owner = this->mpParameter->getNamedConfigurableOwner();
    if(owner != nullptr)
    {
      if(auto element = dynamic_cast<cedar::proc::Element*>(owner))
      {
        this->mParentFullPath = element->getFullPath();
      }
      else
      {
        this->mParentFullPath = owner->getName();
      }
      this->mParameterFullPath = owner->findParameterPath(this->mpParameter);
    }
  }

  void selectParentElementInScene()
  {
    if(this->mpParentElement != nullptr && this->mpScene != nullptr)
    {
      QList<QGraphicsItem *> selected_items = this->mpScene->selectedItems();

      // If element is not already selected, select it
      bool already_selected = false;
      if(selected_items.size() == 1)
      {
        already_selected = selected_items.at(0) == this->mpParentElement;
      }

      if(!already_selected)
      {
        this->mpScene->selectNone();
        this->mpParentElement->setSelected(true);
      }
    }
  }

  // Merges this command to another one if possible. This is used to build the parameter-change macro on the fly
  virtual bool mergeWith(const QUndoCommand* other)
  {
    if(!cedar::proc::gui::SettingsSingleton::getInstance()->getUndoRedoAutoMacro())
    {
      return false;
    }
    if(auto command =
            dynamic_cast<const cedar::proc::undoRedo::commands::ChangeParameterValue<ValueType, ParameterType>*>(other))
    {
      std::string otherIdentifier = command->getMacroIdentifier();
      if(!this->mParameterFullPath.compare("name"))
      {
        otherIdentifier = command->getMacroIdentifier(true);
      }
      if(!getMacroIdentifier().compare(otherIdentifier))
      {
        this->mNewValue = command->mNewValue;
        this->updateFullPaths();
        return true;
      }
    }
    return false;
  }

  // Used to identify the parameter this command changes, for the use with mergeWith()
  std::string getMacroIdentifier(bool old = false) const override
  {
    if(old)
    {
      return this->mOldParentFullPath + "." + this->mParameterFullPath;
    }
    else
    {
      return this->mParentFullPath + "." + this->mParameterFullPath;
    }
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
  std::string mParentFullPath;
  std::string mOldParentFullPath;
  std::string mParameterFullPath;
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

