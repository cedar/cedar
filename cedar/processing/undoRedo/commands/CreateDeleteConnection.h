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

    File:        CreateDeleteConnection.h

		Maintainer:  Lars Janssen,
                 Yogeshwar Agnihotri,
    Email:       lars.janssen@ini.rub.de,
                 yogeshwar.agnihotri@ini.rub.de
    Date:        2020 07 23

    Description: Header file for the class cedar::proc::undoRedo::commands::CreateDeleteConnection.

    Credits:

======================================================================================================================*/

#ifndef CEDAR_PROC_UNDO_REDO_COMMANDS_CREATE_DELETE_CONNECTION_H
#define CEDAR_PROC_UNDO_REDO_COMMANDS_CREATE_DELETE_CONNECTION_H

// CEDAR CONFIGURATION
#include <cedar/processing/gui/GraphicsBase.h>
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/processing/undoRedo/UndoCommand.h"
#include "cedar/processing/gui/Element.h"
#include "cedar/processing/gui/DataSlotItem.h"

// FORWARD DECLARATIONS
#include "cedar/processing/undoRedo/commands/CreateDeleteConnection.fwd.h"
#include "cedar/processing/gui/Connection.fwd.h"
#include "cedar/processing/gui/GraphicsBase.fwd.h"
#include "cedar/processing/gui/Group.fwd.h"

// SYSTEM INCLUDES


/*!@ Create/Delete Connection Command
 *
 * UndoCommand Implementation for creating or deleting a Connection
 */
class cedar::proc::undoRedo::commands::CreateDeleteConnection : public cedar::proc::undoRedo::UndoCommand
{
  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief Constructor for creating a connection
  CreateDeleteConnection(cedar::proc::gui::GraphicsBase* source, cedar::proc::gui::GraphicsBase* target,
          bool isInitialRedo, bool createConnectorGroup = false);
  //!@brief Constructor for deleting a connection
  CreateDeleteConnection(cedar::proc::gui::Connection* connection, bool isInitialRedo,
          bool createConnectorGroup = false);

  //!@brief Destructor
  virtual ~CreateDeleteConnection();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  void undo() override;
  void redo() override;

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  //!@brief Creates a connection between the source and target
  void createConnection();
  //!@brief Deletes a connection between the source and target
  void deleteConnection();
  //!@brief Updates the fullPaths of source and target
  void setIdentifier();
	//!@brief Internal function of setIdentifier
  void setIdentifierInternal(cedar::proc::gui::GraphicsBase* slot, std::string& slotName, std::string& elementFullPath,
														 std::string& external);
  //!@brief Updates the source and target elements by the fullPaths
  void updateSourceAndTargetConnectors();
	//!@brief Internal function of updateSourceAndTargetConnections
	void updateSourceAndTargetConnectorsInternal(std::string fullPath, std::string external, std::string slotName,
																							 cedar::proc::gui::GraphicsBase*& slot,
																							 cedar::proc::DataRole::Id dataRole);
  //!@brief Sets the text for the command in the undoRedo stack visualizer
  void setTextForUndoRedoStackVisualizer();




  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:


  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet
private:
	//!@brief Slots
  cedar::proc::gui::GraphicsBase* mpSource;
  cedar::proc::gui::GraphicsBase* mpTarget;

  //!@brief Different identifier
  std::string mSourceElementFullPath;
  std::string mTargetElementFullPath;
  std::string mSourceSlotName;
  std::string mTargetSlotName;
  //Identifiers when a connection is created between 2 different groups. ExternalOutput/ExternalInput is used.
  std::string mSourceExternalName;
  std::string mTargetExternalName;

  //!@brief saved instance of scene
  cedar::proc::gui::Scene* mpScene;

  //!@brief instance of action
  bool mIsCreateCommand;

	//!@brief if a connector group should be/is created. Default false if not given.
  bool mCreateConnectorGroup;

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

private:
  // none yet

}; // class cedar::proc::undoRedo::commands::CreateDeleteConnection

#endif // CEDAR_PROC_UNDO_REDO_COMMANDS_CREATE_DELETE_CONNECTION_H

