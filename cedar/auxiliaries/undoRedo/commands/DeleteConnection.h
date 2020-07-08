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

    File:        DeleteConnection.h

    Maintainer:  Yogeshwar Agnihotri
    Email:       yogeshwar.agnihotri@ini.ruhr-uni-bochum.de
    Date:        2020 07 04

    Description: Header file for the class cedar::aux::undoRedo::commands::DeleteConnection.

    Credits:

======================================================================================================================*/

#ifndef CEDAR_AUX_UNDO_REDO_COMMANDS_DELETE_CONNECTION_H
#define CEDAR_AUX_UNDO_REDO_COMMANDS_DELETE_CONNECTION_H

// CEDAR CONFIGURATION
#include <cedar/processing/gui/GraphicsBase.h>
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/auxiliaries/undoRedo/UndoCommand.h"

// FORWARD DECLARATIONS
#include "cedar/auxiliaries/undoRedo/commands/DeleteConnection.fwd.h"
#include "cedar/processing/gui/Connection.fwd.h"
#include "cedar/processing/gui/GraphicsBase.fwd.h"
#include "cedar/processing/gui/Group.fwd.h"

// SYSTEM INCLUDES


/*!@ Delete Connection Command
 *
 * UndoCommand Implementation for deleting a Connection
 */
class cedar::aux::undoRedo::commands::DeleteConnection : public cedar::aux::undoRedo::UndoCommand
{
  //--------------------------------------------------------------------------------------------------------------------
  // nested types
  //--------------------------------------------------------------------------------------------------------------------

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  DeleteConnection(cedar::proc::gui::Connection* connection, cedar::proc::gui::Group* group);

  //!@brief Destructor
  virtual ~DeleteConnection();

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

  cedar::proc::gui::GraphicsBase* source;
  cedar::proc::gui::GraphicsBase* target;

  QString sourceSlotName;
  QString targetSlotName;

  cedar::proc::gui::Group* group;

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

private:
  // none yet

}; // class cedar::aux::undoRedo::commands::DeleteConnection

#endif // CEDAR_AUX_UNDO_REDO_COMMANDS_DELETE_CONNECTION_H

