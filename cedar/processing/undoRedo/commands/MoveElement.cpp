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

    File:        MoveElement.cpp

    Maintainer:  Lars Janssen
    Email:       lars.janssen@ini.rub.de
    Date:        2020 07 23

    Description: Source file for the class cedar::proc::undoRedo::commands::MoveElement.

    Credits:

======================================================================================================================*/

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CLASS HEADER
#include "cedar/processing/undoRedo/commands/MoveElement.h"

// CEDAR INCLUDES
#include "cedar/processing/gui/GraphicsBase.h"
#include "cedar/processing/gui/Scene.h"
// SYSTEM INCLUDES


//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::proc::undoRedo::commands::MoveElement::MoveElement(cedar::proc::gui::GraphicsBase* element, const QPointF sourcePosition, cedar::proc::gui::Scene* scene)
:
mpGuiElement(dynamic_cast<cedar::proc::gui::Element *>(element)),
mSourcePosition(sourcePosition),
mTargetPosition(element->pos()),
mpScene(scene)
{
  mElementIdentifier = mpGuiElement->getElement()->getFullPath();
  setText(QString::fromStdString("Moved element" + mElementIdentifier));
}

cedar::proc::undoRedo::commands::MoveElement::~MoveElement()
{
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

// Move element back to the source
void cedar::proc::undoRedo::commands::MoveElement::undo()
{
  mpGuiElement = mpScene->getElementByFullPath(mElementIdentifier);

  if(this->mpGuiElement != nullptr && this->mpScene->items().contains(this->mpGuiElement))
  {
    this->mpGuiElement->setPos(this->mSourcePosition);
  }
}

// Move element to the target
void cedar::proc::undoRedo::commands::MoveElement::redo()
{
  mpGuiElement = mpScene->getElementByFullPath(mElementIdentifier);

  if(this->mpGuiElement != nullptr && this->mpScene->items().contains(this->mpGuiElement))
  {
    this->mpGuiElement->setPos(this->mTargetPosition);
  }
}