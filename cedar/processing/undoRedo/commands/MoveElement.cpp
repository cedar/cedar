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

		Maintainer:  Lars Janssen,
                 Yogeshwar Agnihotri,
    Email:       lars.janssen@ini.rub.de,
                 yogeshwar.agnihotri@ini.rub.de

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

cedar::proc::undoRedo::commands::MoveElement::MoveElement(std::list<QGraphicsItem*> elements,
                                                          cedar::proc::gui::Group* sourceGroup,
                                                          cedar::proc::gui::Group* targetGroup,
                                                          std::vector<QPointF> sourcePositions,
                                                          cedar::proc::gui::Scene* scene)
:
mSourceGroup(sourceGroup),
mTargetGroup(targetGroup),
mSourcePositions(sourcePositions),
mpScene(scene)
{
  //Iterate over elements and extract their target position and elementFullPath
  for(QGraphicsItem* element : elements)
  {
    if(auto guiElement = dynamic_cast<cedar::proc::gui::Element*>(element))
    {
      this->mTargetPositions.push_back(guiElement->pos());
      this->mElementFullPaths.push_back(guiElement->getElement()->getFullPath());
      this->mGuiElements.push_back(guiElement);
    }
  }
  //TODO: is this here needed? Only for grouppaths? Can they be setted manually her?
  updateFullPath();

  if(elements.size() == 1)
  {
    if(auto element = dynamic_cast<cedar::proc::gui::Element*>(elements.front()))
    {
      setText(QString::fromStdString("Moved element: " + element->getElement()->getName()));
    }
  }
  else
  {
    //Set text for undo redo stack visualizer
    setText(QString::fromStdString("Moved all selected elements"));
  }
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
  //Update the pointer of all elements using their full path
  updatePointer();

  //Add to old group and move to old position. In undo they have to be added first then moved
	addElementsToGroup(this->mSourceGroup);
	move(this->mSourcePositions);
}

// Move element to the target
void cedar::proc::undoRedo::commands::MoveElement::redo()
{
  //Update the pointer of all elements using their full path
  updatePointer();

	//Add to new group and move to new position. In redo they have to be moved first then added
	move(this->mTargetPositions);
  addElementsToGroup(this->mTargetGroup);
}

void cedar::proc::undoRedo::commands::MoveElement::move(std::vector<QPointF> positions)
{
	if(!this->mGuiElements.empty() && !positions.empty())
	{
		int i = 0;
		for(cedar::proc::gui::Element* element : mGuiElements)
		{
			element->setPos(positions.at(i));
			i++;
		}
	}
}

void cedar::proc::undoRedo::commands::MoveElement::addElementsToGroup(cedar::proc::gui::Group* group)
{
  //GuiElements get deleted by the addElement() function, so the non gui elements have to be saved and later the guiElements get updated
  std::vector<cedar::proc::Element*> elements;

  //Add elements only accepts std::list<QGrapicsItem*>, so the guiElements have to be converted first
  std::list<QGraphicsItem*> qGraphicsItems;
  for(cedar::proc::gui::Element* guiElement:this->mGuiElements)
  {
    elements.push_back(guiElement->getElement().get());

    if(QGraphicsItem* item = dynamic_cast<QGraphicsItem*>(guiElement))
    {
      qGraphicsItems.push_back(item);
    }
  }

  if(this->mSourceGroup != this->mTargetGroup && !this->mGuiElements.empty())
  {
    //Add to the group if pointer is not nullptr, if nullptr that means the group where the elements should be moved is the rootgroup
    if(group != nullptr)
    {
      group->addElements(qGraphicsItems);
    }
    else
    {
      this->mpScene->getRootGroup()->addElements(qGraphicsItems);
    }

    //Update the guiElements again before leaving the function. There have been newly created by addElements()
    this->mGuiElements.clear();
    for(cedar::proc::Element* element:elements)
    {
      this->mGuiElements.push_back(this->mpScene->getGraphicsItemFor(element));
    }

    //Since the group of the elements has been changed, we need to update the full paths
		updateFullPath();
  }
}


void cedar::proc::undoRedo::commands::MoveElement::updatePointer()
{
  int i = 0;
  for(auto &element : this->mGuiElements)
  {
    cedar::proc::gui::Element* guiElement = this->mpScene->getElementByFullPath(mElementFullPaths.at(i));
    element = guiElement;
    i++;
  }

  if(mSourceGroupFullPath == "")
  {
    this->mSourceGroup = nullptr;
  }
  else
  {
    cedar::proc::gui::Element* sourceGroupElement = this->mpScene->getElementByFullPath(mSourceGroupFullPath);
    CEDAR_ASSERT(sourceGroupElement != nullptr)
    auto group = dynamic_cast<cedar::proc::gui::Group*>(sourceGroupElement);
    CEDAR_ASSERT(group != nullptr)
    this->mSourceGroup = group;
  }

  if(mTargetGroupFullPath == "")
  {
    this->mTargetGroup = nullptr;
  }
  else
  {
    cedar::proc::gui::Element* targetGroupElement = this->mpScene->getElementByFullPath(mTargetGroupFullPath);
    CEDAR_ASSERT(targetGroupElement != nullptr)
    auto group = dynamic_cast<cedar::proc::gui::Group*>(targetGroupElement);
    CEDAR_ASSERT(group != nullptr)
    this->mTargetGroup = group;
  }
}

void cedar::proc::undoRedo::commands::MoveElement::updateFullPath()
{
  int i = 0;
  for(QGraphicsItem* element : this->mGuiElements)
  {
    if(auto guiElement = dynamic_cast<cedar::proc::gui::Element*>(element))
    {
      this->mElementFullPaths.at(i) = guiElement->getElement()->getFullPath();
    }
    else
    {
      this->mElementFullPaths.at(i) = "";
    }
    i++;
  }

  if(this->mSourceGroup != nullptr)
  {
    this->mSourceGroupFullPath = this->mSourceGroup->getGroup()->getFullPath();
  }
  else
  {
    this->mSourceGroupFullPath = "";
  }

  if(this->mTargetGroup != nullptr)
  {
    this->mTargetGroupFullPath = this->mTargetGroup->getGroup()->getFullPath();
  }
  else
  {
    this->mTargetGroupFullPath = "";
  }
}