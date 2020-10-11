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

    Maintainer:  Lars Janssen
    Email:       lars.janssen@ini.rub.de
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

  //!@brief Constructor for removeConnection
  CreateDeleteConnection(cedar::proc::gui::Connection* connection, Action action, bool createConnectorGroup = false);

  //!@brief Constructor for createConnection
  CreateDeleteConnection(cedar::proc::gui::GraphicsBase* source, cedar::proc::gui::GraphicsBase* target, Action action,
                   bool createConnectorGroup = false);

  //!@brief Destructor
  virtual ~CreateDeleteConnection();

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
  void createConnection();
  void deleteConnection();

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

  cedar::proc::gui::GraphicsBase* mpSource;
  cedar::proc::gui::GraphicsBase* mpTarget;

  cedar::proc::gui::Scene* mpScene;
  Action mAction;
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

