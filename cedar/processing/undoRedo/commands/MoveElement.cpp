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
// SYSTEM INCLUDES
#include <boost/algorithm/string.hpp>


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
  mElementIdentifier = getElementIdentifier(mpGuiElement);
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
  mpGuiElement = getElementAddress(mElementIdentifier,mpScene);

  if(this->mpGuiElement != nullptr && this->mpScene->items().contains(this->mpGuiElement))
  {
    this->mpGuiElement->setPos(this->mSourcePosition);
  }
}

// Move element to the target
void cedar::proc::undoRedo::commands::MoveElement::redo()
{
  mpGuiElement = getElementAddress(mElementIdentifier,mpScene);

  if(this->mpGuiElement != nullptr && this->mpScene->items().contains(this->mpGuiElement))
  {
    this->mpGuiElement->setPos(this->mTargetPosition);
  }
}

std::string cedar::proc::undoRedo::commands::MoveElement::getElementIdentifier(cedar::proc::gui::Element* guiElement)
{
  //Get parentItem of the element
  QGraphicsItem* parentItem = guiElement->parentItem();
  std::string elementName = guiElement->getElement()->getName();

  while(parentItem != nullptr)
  {
    if (cedar::proc::gui::Group* group = dynamic_cast<cedar::proc::gui::Group*>(parentItem))
    {
      std::string groupName = group->getGroup()->getName();
      elementName = groupName + "." + elementName;
    }
    //Get the next parentItem of the parentItem before.
    parentItem = parentItem->parentItem();
  }
  return elementName;
}

cedar::proc::gui::Element* cedar::proc::undoRedo::commands::MoveElement::getElementAddress(std::string elementIdentifier, cedar::proc::gui::Scene* scene)
{
  std::vector<std::string> mElementNameSplitted;
  boost::split(mElementNameSplitted, elementIdentifier, boost::is_any_of("."));

  cedar::proc::gui::GroupPtr rootGroup = scene->getRootGroup();

  cedar::proc::gui::Group* currentGroup = rootGroup.get();
  //Go through all subgroups
  for (std::size_t i = 0; i < mElementNameSplitted.size()-1; i++)
  {
    if(currentGroup->getGroup()->contains(mElementNameSplitted[i]))
    {
      if (cedar::proc::ElementPtr element = currentGroup->getGroup()->getElement(mElementNameSplitted[i]))
      {
        cedar::proc::gui::Element* guiElement = scene->getGraphicsItemFor(element);
        if (cedar::proc::gui::Group* group = dynamic_cast<cedar::proc::gui::Group*>(guiElement))
        {
          currentGroup = group;
        }
      }
    }
  }

  //Set the guiElement
  if(currentGroup->getGroup()->contains(mElementNameSplitted[mElementNameSplitted.size() - 1]))
  {
    //Search in the group of the element
    if (cedar::proc::ElementPtr element = currentGroup->getGroup()->getElement(mElementNameSplitted[mElementNameSplitted.size() - 1]))
    {
      if (cedar::proc::gui::Element* guiElement = scene->getGraphicsItemFor(element))
      {
        return guiElement;
      }
    }
  }
}