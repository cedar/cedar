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

    File:        CreateGroupTemplate.h

    Maintainer:  Yogeshwar Agnihotri
    Email:       yogeshwar.agnihotri@ini.ruhr-uni-bochum.de
    Date:        2021 01 20

    Description: Header file for the class cedar::proc::undoRedo::commands::CreateGroupTemplate.

    Credits:

======================================================================================================================*/

#ifndef CEDAR_PROC_UNDO_REDO_COMMANDS_CREATE_GROUP_H
#define CEDAR_PROC_UNDO_REDO_COMMANDS_CREATE_GROUP_H

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/processing/GroupDeclaration.h"
#include "cedar/processing/gui/Scene.h"
#include "cedar/processing/undoRedo/UndoCommand.h"

// FORWARD DECLARATIONS
#include "cedar/processing/undoRedo/commands/CreateGroupTemplate.fwd.h"

// SYSTEM INCLUDES


/*!
 *  Undo/redo command for creating a groupTemplate. This is only for creating a groupTemplate. Once a groupTemplate is created CEDAR treats it as a element so the deletion is managed via CreateDeleteElement class
 */
class cedar::proc::undoRedo::commands::CreateGroupTemplate : public UndoCommand
{
  //--------------------------------------------------------------------------------------------------------------------
  // nested types
  //--------------------------------------------------------------------------------------------------------------------

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  CreateGroupTemplate(const cedar::proc::GroupDeclaration*, cedar::proc::GroupPtr, QGraphicsSceneDragDropEvent *pEvent, QPointF mapped, cedar::proc::gui::Scene* scene);

  //!@brief Destructor
  virtual ~CreateGroupTemplate();

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
  //!@brief Creates a groupTemplate and sets its mpGuiElement, mpGroup and mClassId values
  void createGroupTemplate();

  //!@brief Creates a empty element. This is used to redo groupTemplates, since createGroupTemplate is only used once on the initalRedo
  void createElement();

  //!@brief Deletes the element
  void deleteGroupTemplate();

  //!@brief Saves the configuration of the element into the configurationNode 'mElementConfiguration'
  void saveElementConfiguration();

  //!@brief Loads the configuration from the configurationNode 'mElementConfiguration' into mpGuiElement
  void loadElementConfiguration();
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
	//!@brief This is given to the command. Stores which declaration should be used
  const cedar::proc::GroupDeclaration* mpGroupDeclaration;
	//!@brief Target group of the create command. Rootgroup = nullptr
	cedar::proc::GroupPtr mpTargetGroup;
  //TODO: needed for what?
  QGraphicsSceneDragDropEvent* mpEvent;

  QPointF mPosition;
  cedar::aux::ConfigurationNode mElementConfiguration;
  cedar::proc::gui::Element* mpGuiElement;
  std::string mElementFullPath;
  std::string mClassId;
  bool mIsInitialRedo;
	cedar::proc::gui::Scene* mpScene;
}; //class cedar::proc::undoRedo::commands::CreateGroupTemplate

#endif // CEDAR_PROC_UNDO_REDO_COMMANDS_CREATE_GROUP_H

