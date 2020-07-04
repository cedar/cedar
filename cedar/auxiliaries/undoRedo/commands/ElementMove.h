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

    File:        ElementMove.h

    Maintainer:  Yogeshwar Agnihotri
    Email:       yogeshwar.agnihotri@ini.ruhr-uni-bochum.de
    Date:        2020 06 21

    Description: Header file for the class cedar::aux::undoRedo::commands::ElementMove.

    Credits:

======================================================================================================================*/

#ifndef CEDAR_AUX_UNDO_REDO_COMMANDS_ELEMENT_MOVE_COMMAND_H
#define CEDAR_AUX_UNDO_REDO_COMMANDS_ELEMENT_MOVE_COMMAND_H

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/auxiliaries/undoRedo/UndoCommand.h"

// FORWARD DECLARATIONS
#include "cedar/auxiliaries/undoRedo/commands/ElementMove.fwd.h"
#include "cedar/processing/gui/GraphicsBase.fwd.h"

// SYSTEM INCLUDES
#include <QPointF>

/*!@todo describe.
 *F
 * @todo describe more.
 */
class cedar::aux::undoRedo::commands::ElementMove : public UndoCommand
{
  //--------------------------------------------------------------------------------------------------------------------
  // nested types
  //--------------------------------------------------------------------------------------------------------------------

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  ElementMove(cedar::proc::gui::GraphicsBase* element, const QPointF sourcePosition);

  //!@brief Destructor
  virtual ~ElementMove();

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
  cedar::proc::gui::GraphicsBase* mpElement;
  const QPointF mSourcePosition;
  const QPointF mTargetPosition;

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

private:
  // none yet

};

#endif // CEDAR_AUX_UNDO_REDO_COMMANDS_ELEMENT_MOVE_COMMAND_H

