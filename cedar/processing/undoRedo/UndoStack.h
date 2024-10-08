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

    File:        UndoStack.h

    Maintainer:  Yogeshwar Agnihotri,
    						 Lars Janssen
    Email:       yogeshwar.agnihotri@ini.ruhr-uni-bochum.de,
    						 lars.janssen@ini.rub.de
    Date:        2020 07 23

    Description: Header file for the class cedar::proc::undoRedo::UndoStack.

    Credits:

======================================================================================================================*/

#ifndef CEDAR_PROC_UNDO_REDO_UNDO_STACK_H
#define CEDAR_PROC_UNDO_REDO_UNDO_STACK_H

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/processing/undoRedo/UndoCommand.fwd.h"

// FORWARD DECLARATIONS
#include "cedar/processing/undoRedo/UndoStack.fwd.h"

// SYSTEM INCLUDES
#include <QUndoStack>

/*! Undo/Redo stack
 *
 * Implementation of QTs QUndoStack to control commands regarding the undo/redo system
 */
class cedar::proc::undoRedo::UndoStack : public QUndoStack
{
  //--------------------------------------------------------------------------------------------------------------------
  // nested types
  //--------------------------------------------------------------------------------------------------------------------

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief Constructor initializing the superclass
  UndoStack(QObject *parent = nullptr);

  //!@brief Destructor
  virtual ~UndoStack();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:

  // Searches for idIndex of given string or creates it
  int idIndex(std::string id);

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
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

private:

  std::vector<std::string> idList;

}; // class cedar::proc::undoRedo::UndoStack

#endif // CEDAR_PROC_UNDO_REDO_UNDO_STACK_H

