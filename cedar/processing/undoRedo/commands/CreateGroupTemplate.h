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
 *  Undo/redo command for creating a groupTemplate
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
  void createGroupTemplate();
  void createElement();
  void deleteGroupTemplate();
  void saveElementConfiguration();
  void loadElementConfiguration();
  void updateElementIdentifier();
  void updateElementAddress();

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
  const cedar::proc::GroupDeclaration* mpGroupDeclaration;
  cedar::proc::GroupPtr mpTargetGroup;
  QGraphicsSceneDragDropEvent* mpEvent;
  cedar::proc::gui::Scene* mpScene;
  cedar::proc::GroupPtr mpGroup;
  QPointF mPosition;
  cedar::aux::ConfigurationNode mElementConfiguration;
  cedar::proc::gui::Element* mpGuiElement;
  std::string mElementName;
  std::string mClassId;
  bool mIsInitialRedo;
}; //class cedar::proc::undoRedo::commands::CreateGroupTemplate

#endif // CEDAR_PROC_UNDO_REDO_COMMANDS_CREATE_GROUP_H

