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

    File:        CreateDeleteStep.h

    Maintainer:  Yogeshwar Agnihotri
    Email:       yogeshwar.agnihotri@ini.rub.de
    Date:        2020 07 23

    Description: Header file for the class cedar::proc::undoRedo::commands::CreateDeleteStep.

    Credits:

======================================================================================================================*/

#ifndef CEDAR_PROC_UNDO_REDO_COMMANDS_CREATE_DELETE_STEP_H
#define CEDAR_PROC_UNDO_REDO_COMMANDS_CREATE_DELETE_STEP_H

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/processing/undoRedo/UndoCommand.h"
#include "cedar/processing/gui/Group.h"
#include "cedar/processing/gui/Element.h"
#include "cedar/processing/gui/Connectable.h"


// FORWARD DECLARATIONS
#include "cedar/processing/undoRedo/commands/CreateDeleteStep.fwd.h"


// SYSTEM INCLUDES
#include <QPointF>

/*! Create/Delete Step
 *
 * Undo/redo command for step creation/deletion
 */
class cedar::proc::undoRedo::commands::CreateDeleteStep : public UndoCommand
{
  //--------------------------------------------------------------------------------------------------------------------
  // nested types
  //--------------------------------------------------------------------------------------------------------------------
  //--------------------------------------------------------------------------------------------------------------------
  // enums
  //--------------------------------------------------------------------------------------------------------------------
public:

  enum Action
  {
    CREATE,
    DELETE
  };
  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //Constructor for creating an element
  CreateDeleteStep(QPointF position,std::string classId, cedar::proc::GroupPtr group,cedar::proc::gui::Scene* scene,cedar::proc::undoRedo::commands::CreateDeleteStep::Action action);
  //Constructor for deleting an element
  CreateDeleteStep(cedar::proc::gui::Element* element, cedar::proc::gui::Scene* scene, cedar::proc::undoRedo::commands::CreateDeleteStep::Action action);

  //!@brief Destructor
  virtual ~CreateDeleteStep();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  void undo();
  void redo();

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  void createStep();
  void deleteStep();
  void saveStepConfiguration();
  void loadStepConfiguration();

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
  cedar::aux::ConfigurationNode mElementConfiguration;
  cedar::proc::gui::Element* mpGuiElement;
  cedar::proc::ElementPtr mpElement;
  cedar::proc::GroupPtr mpGroup;
  cedar::proc::gui::Scene* mpScene;
  std::string mClassId;
  QPointF mPosition;
  Action mAction;

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

private:
  // none yet

};

#endif // CEDAR_PROC_UNDO_REDO_COMMANDS_CREATE_DELETE_STEP_H

