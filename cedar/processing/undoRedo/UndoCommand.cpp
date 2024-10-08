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

    File:        UndoCommand.cpp

    Maintainer:  Yogeshwar Agnihotri,
    						 Lars Janssen
    Email:       yogeshwar.agnihotri@ini.ruhr-uni-bochum.de,
    						 lars.janssen@ini.rub.de
    Date:        2020 07 23

    Description: Source file for the class cedar::proc::undoRedo::UndoCommand.

    Credits:

======================================================================================================================*/

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CLASS HEADER
#include "cedar/processing/undoRedo/UndoCommand.h"
#include "cedar/processing/undoRedo/UndoStack.h"
#include "cedar/processing/undoRedo/commands/ChangeParameterValue.h"
#include "cedar/processing/undoRedo/commands/ChangeParameterValueTemplate.h"
#include "cedar/processing/gui/ExperimentDialog.h"
#include "cedar/processing/gui/Ide.h"

// CEDAR INCLUDES

// SYSTEM INCLUDES

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::proc::undoRedo::UndoCommand::UndoCommand()
{
}

cedar::proc::undoRedo::UndoCommand::~UndoCommand()
{
}

int cedar::proc::undoRedo::UndoCommand::id() const
{
  // Reserve id 0 for string parameters, so "name" parameter works with the macro system
  if(dynamic_cast<const cedar::proc::undoRedo::commands::ChangeParameterValueTemplate<std::string>*>(this))
  {
    return 0;
  }
  return cedar::proc::gui::Ide::pUndoStack->idIndex(this->getMacroIdentifier());
}

cedar::proc::gui::Scene* cedar::proc::undoRedo::UndoCommand::sceneIfUndoable(cedar::aux::Parameter* parameter, QObject* reference)
{
  // If parameter belongs to a step/element it should be undoable (e.g. settings parameter should not be undoable)
  bool isUndoable = false;
  cedar::aux::NamedConfigurable* owner = parameter->getNamedConfigurableOwner();
  cedar::proc::gui::Scene* scene;
  if(owner != nullptr)
  {
    //Find the scene
    QObject* parent = reference;
    while(parent != nullptr)
    {
      if(dynamic_cast<cedar::proc::gui::ExperimentDialog*>(parent))
      {
        //Undo/Redo does not support experiment framework
        isUndoable = false;
        break;
      }
      if(auto ide = dynamic_cast<cedar::proc::gui::Ide*>(parent))
      {
        scene = ide->mpProcessingDrawer->getScene();
        isUndoable = true;
        break;
      }
      parent = parent->parent();
    }
  }
  if(!isUndoable)
  {
    scene = nullptr;
  }
  return scene;
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------
