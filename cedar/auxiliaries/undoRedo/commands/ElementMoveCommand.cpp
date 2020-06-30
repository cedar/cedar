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

    File:        ElementMoveCommand.cpp

    Maintainer:  Yogeshwar Agnihotri
    Email:       yogeshwar.agnihotri@ini.ruhr-uni-bochum.de
    Date:        2020 06 21

    Description: Source file for the class cedar::aux::undoRedo::commands::ElementMoveCommand.

    Credits:

======================================================================================================================*/

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CLASS HEADER
#include "cedar/auxiliaries/undoRedo/commands/ElementMoveCommand.h"

// CEDAR INCLUDES
#include "cedar/processing/gui/GraphicsBase.h"
// SYSTEM INCLUDES


//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::aux::undoRedo::commands::ElementMoveCommand::ElementMoveCommand(cedar::proc::gui::GraphicsBase* element, const QPointF sourcePosition)
:
mpElement(element),
mSourcePosition(sourcePosition),
mTargetPosition(element->pos())
{
}

cedar::aux::undoRedo::commands::ElementMoveCommand::~ElementMoveCommand()
{
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------
//Set element back to its source
void cedar::aux::undoRedo::commands::ElementMoveCommand::undo()
{
  mpElement->setPos(mSourcePosition);
}

void cedar::aux::undoRedo::commands::ElementMoveCommand::redo()
{
  mpElement->setPos(mTargetPosition);
}
