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
#include "cedar/processing/Element.h"
#include "cedar/processing/gui/Element.h"
// SYSTEM INCLUDES


//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::proc::undoRedo::commands::MoveElement::MoveElement(cedar::proc::gui::GraphicsBase* element, const QPointF sourcePosition, cedar::proc::gui::Scene* scene)
:
mpElement(element),
mSourcePosition(sourcePosition),
mTargetPosition(element->pos()),
mpScene(scene)
{
  updateElementName();
  //TODO move element between groups
  setText(QString::fromStdString("Moved element"));
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
  updateElementAddress();
  if(this->mpElement != nullptr && this->mpScene->items().contains(this->mpElement))
  {
    this->mpElement->setPos(this->mSourcePosition);
  }
}

// Move element to the target
void cedar::proc::undoRedo::commands::MoveElement::redo()
{
  updateElementAddress();
  if(this->mpElement != nullptr && this->mpScene->items().contains(this->mpElement))
  {
    this->mpElement->setPos(this->mTargetPosition);
  }
}

void cedar::proc::undoRedo::commands::MoveElement::updateElementName()
{
  if(auto element = dynamic_cast<cedar::proc::gui::Element*>(this->mpElement))
  {
    this->mElementName = element->getElement()->getName();
  }
}

void cedar::proc::undoRedo::commands::MoveElement::updateElementAddress()
{
  QList<QGraphicsItem *> items = this->mpScene->items();

  for(QGraphicsItem* item : items)
  {

    if(cedar::proc::gui::Element* element = dynamic_cast<cedar::proc::gui::Element*>(item))
    {
      if(!element->getElement()->getName().compare(mElementName))
      {
        this->mpElement = element;
      }
    }
  }
}