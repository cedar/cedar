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
#include "cedar/auxiliaries/VectorParameter.h"
#include "cedar/processing/undoRedo/UndoCommand.h"
#include "cedar/processing/gui/Element.h"
#include "cedar/processing/Element.h"

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
  ChangeVectorParameterValue(cedar::aux::VectorParameter<ValueType>* parameter, std::vector<ValueType> oldValue,
                             std::vector<ValueType> newValue, cedar::proc::gui::Scene* scene = nullptr)
  :
  mpParameter(parameter),
  mOldValue(oldValue),
  mNewValue(newValue),
  mpScene(scene),
  isInitialRedo(true)
  {
    mParentName = getParentName(parameter);
    mParameterName = parameter->getName();
    setText(QString::fromStdString(mParameterName + ":" + mParentName));
  }

  //!@brief Destructor
  ChangeVectorParameterValue()
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
              this->mpParameter = element->getParameter<cedar::aux::VectorParameter<ValueType>>(this->mParameterName).get();
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
      this->mpParameter->setValue(this->mOldValue);
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
      this->mpParameter->setValue(this->mNewValue);
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
    if(auto command = dynamic_cast<const cedar::proc::undoRedo::commands::ChangeVectorParameterValue<ValueType>*>(other))
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
    if(!this->mParentName.compare(""))
    {
      return "";
    }
    return this->mParentName + "." + this->mParameterName;
  }

  std::string getParentName(cedar::aux::VectorParameter<ValueType>* parameter) const
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
  // none yet
private:
  std::string mParentName;
  std::string mParameterName;
  cedar::aux::VectorParameter<ValueType>* mpParameter;
  cedar::proc::gui::Element* mpParentElement;

  std::vector<ValueType> mOldValue;
  std::vector<ValueType> mNewValue;
  bool isInitialRedo;

  cedar::proc::gui::Scene* mpScene;

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

private:
  // none yet

}; // class cedar::proc::undoRedo::commands::ChangeVectorParameterValue

#endif // CEDAR_PROC_UNDO_REDO_COMMANDS_CHANGE_VECTOR_PARAMETER_VALUE_H

