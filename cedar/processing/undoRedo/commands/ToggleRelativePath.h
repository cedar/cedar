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

    Maintainer:  Yogeshwar Agnihotri,
    						 Lars Janssen
    Email:       yogeshwar.agnihotri@ini.ruhr-uni-bochum.de,
    						 lars.janssen@ini.rub.de
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
  ToggleRelativePath(cedar::aux::FileParameter* parameter, bool targetState, cedar::proc::gui::Scene* scene = nullptr)
  :
  mpParameter(parameter),
  mpParentElement(nullptr),
  mTargetState(targetState),
  isInitialRedo(true),
  mpScene(scene)
  {
    mParentName = getParentName(parameter);
    mParameterName = parameter->getName();
    setText(QString::fromStdString("Toggled relative path: " + mParameterName + "::" + mParentName));
  }

  //!@brief Destructor
  virtual ~ToggleRelativePath()
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
              this->mpParameter = element->getParameter<cedar::aux::FileParameter>(this->mParameterName).get();
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
      if(this->mTargetState)
      {
        this->mpParameter->setPathMode(cedar::aux::FileParameter::PathMode::PATH_MODE_ABSOLUTE);
      }
      else
      {
        this->mpParameter->setPathMode(cedar::aux::FileParameter::PathMode::PATH_MODE_RELATIVE_TO_CURRENT_ARCHITECTURE_DIR);
      }
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
      if(this->mTargetState)
      {
        this->mpParameter->setPathMode(cedar::aux::FileParameter::PathMode::PATH_MODE_RELATIVE_TO_CURRENT_ARCHITECTURE_DIR);
      }
      else
      {
        this->mpParameter->setPathMode(cedar::aux::FileParameter::PathMode::PATH_MODE_ABSOLUTE);
      }
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

  std::string getMacroIdentifier() const override
  {
    if(!this->mParentName.compare(""))
    {
      return "";
    }
    return this->mParentName + "." + this->mParameterName;
  }

  std::string getParentName(cedar::aux::FileParameter* parameter) const
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
  cedar::aux::FileParameter* mpParameter;
  cedar::proc::gui::Element* mpParentElement;

  bool mTargetState;
  bool isInitialRedo;

  cedar::proc::gui::Scene* mpScene;

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

private:
  // none yet

}; // class cedar::proc::undoRedo::commands::ToggleRelativePath

#endif // CEDAR_PROC_UNDO_REDO_COMMANDS_TOGGLE_RELATIVE_PATH_H


