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

    File:        ElementMove.cpp

    Maintainer:  Yogeshwar Agnihotri
    Email:       yogeshwar.agnihotri@ini.ruhr-uni-bochum.de
    Date:        2020 06 21

    Description: Source file for the class cedar::aux::undoRedo::commands::ElementMove.

    Credits:

======================================================================================================================*/

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CLASS HEADER
#include "cedar/auxiliaries/undoRedo/commands/ElementMove.h"

// CEDAR INCLUDES
#include "cedar/processing/gui/GraphicsBase.h"
#include "cedar/processing/gui/Scene.h"
// SYSTEM INCLUDES


//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::aux::undoRedo::commands::ElementMove::ElementMove(cedar::proc::gui::GraphicsBase* element, const QPointF sourcePosition, cedar::proc::gui::Scene* scene)
:
mpElement(element),
mSourcePosition(sourcePosition),
mTargetPosition(element->pos()),
pScene(scene)
{
  //TODO move element between groups
}

cedar::aux::undoRedo::commands::ElementMove::~ElementMove()
{
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

// Move element back to the source
void cedar::aux::undoRedo::commands::ElementMove::undo()
{
  if(this->mpElement != nullptr && this->pScene->items().contains(this->mpElement))
  {
    this->mpElement->setPos(this->mSourcePosition);
  }
}

// Move element to the target
void cedar::aux::undoRedo::commands::ElementMove::redo()
{
  if(this->mpElement != nullptr && this->pScene->items().contains(this->mpElement))
  {
    this->mpElement->setPos(this->mTargetPosition);
  }
}
