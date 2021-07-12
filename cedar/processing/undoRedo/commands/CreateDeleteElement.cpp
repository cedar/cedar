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

    File:        CreateDeleteElement.cpp

    Maintainer:  Yogeshwar Agnihotri,
    						 Lars Janssen
    Email:       yogeshwar.agnihotri@ini.ruhr-uni-bochum.de,
    						 lars.janssen@ini.rub.de
    Date:        2021 04 07

    Description: Source file for the class cedar::proc::undoRedo::commands::CreateDeleteElement.

    Credits:

======================================================================================================================*/

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CLASS HEADER
#include "cedar/processing/undoRedo/commands/CreateDeleteElement.h"

// CEDAR INCLUDES
#include "cedar/processing/DeclarationRegistry.h"

// SYSTEM INCLUDES
#include <string>

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

//Constructor for creating an element
cedar::proc::undoRedo::commands::CreateDeleteElement::CreateDeleteElement(QPointF position,std::string classId,
				cedar::proc::GroupPtr group,cedar::proc::gui::Scene* scene,
				bool isCreateCommand)
:
mpGuiElement(nullptr),
mpTargetGroup(group),
mpScene(scene),
mClassId(classId),
mPosition(position),
mGroupIsCollapsed(false),
mIsCreateCommand(isCreateCommand),
mIsInitialRedo(true)
{
}

//Constructor for deleting an element
cedar::proc::undoRedo::commands::CreateDeleteElement::CreateDeleteElement(cedar::proc::gui::Element* element,
				cedar::proc::gui::Scene* scene, bool isCreateCommand)
:
mpGuiElement(element),
mpScene(scene),
mGroupIsCollapsed(false),
mIsCreateCommand(isCreateCommand),
mIsInitialRedo(true)
{
	//set some vars since the element already exists
  this->mPosition = element->pos();
	this->mClassId = cedar::proc::ElementManagerSingleton::getInstance()->getTypeId(element->getElement());

  //Finding the highest level group the element is in
  QGraphicsItem* parentItem = element->parentItem();
  while(parentItem != nullptr)
  {
    //Check if the parentItem is a group, if yes set the parentItem as the group of the element
    if (auto* groupOfElement = dynamic_cast<cedar::proc::gui::Group*>(parentItem))
    {
			this->mpTargetGroup = groupOfElement->getGroup();
      break;
    }
    else
    {
      parentItem = parentItem->parentItem();
    }
  }

  if(parentItem == nullptr)
  {
		this->mpTargetGroup = scene->getRootGroup()->getGroup();
  }

  //Since the element already exists, set the elementFullPath
	this->mElementFullPath = mpGuiElement->getElement()->getFullPath();

  //Set text for the 'Undo/Redo Stack'
  setText(QString::fromStdString("Deleted element: " + this->mElementFullPath));
}

cedar::proc::undoRedo::commands::CreateDeleteElement::~CreateDeleteElement()
{
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::proc::undoRedo::commands::CreateDeleteElement::undo()
{
  switch(this->mIsCreateCommand)
  {
    case true:
      //Before deleting the element update the address of the element and its parentGroup in case it has been changed
      // through a create
      //Uses elementFullPath
			this->mpGuiElement = mpScene->getElementByFullPath(this->mElementFullPath);
			this->mpTargetGroup = mpScene->getGroupOfElementByFullPath(this->mElementFullPath);

      if(this->mpGuiElement != nullptr)
      {
        //Undo of createElement = deleteElement
        deleteElement();
      }
      break;
    case false:
      //Update group since it could have changed
			this->mpTargetGroup = this->mpScene->getGroupOfElementByFullPath(this->mElementFullPath);
      createElement();
      break;
  }
}

void cedar::proc::undoRedo::commands::CreateDeleteElement::redo()
{
  switch(this->mIsCreateCommand)
  {
    case true:
      if(this->mIsInitialRedo)
      {
        createElement();
        this->mElementFullPath = this->mpGuiElement->getElement()->getFullPath();
        //Set text for the 'Undo/Redo Stack'
        setText(QString::fromStdString("Created element: " + this->mElementFullPath));
				this->mIsInitialRedo = false;
      }
      else
      {
        //Group could have been changed
				this->mpTargetGroup = this->mpScene->getGroupOfElementByFullPath(this->mElementFullPath);
        createElement();
      }
      break;

    case false:
      //Update guiElement and group before deleting if they have been changed
			this->mpGuiElement = this->mpScene->getElementByFullPath(this->mElementFullPath);
			this->mpTargetGroup = this->mpScene->getGroupOfElementByFullPath(this->mElementFullPath);

      if(this->mpGuiElement != nullptr)
      {
        //Undo of createElement = deleteElement
        deleteElement();
      }
			this->mIsInitialRedo = false;
      break;
  }
}

void cedar::proc::undoRedo::commands::CreateDeleteElement::createElement()
{
  cedar::proc::Element* element = this->mpScene->createElement(this->mpTargetGroup, this->mClassId,
  				this->mPosition).get();
	this->mpGuiElement = this->mpScene->getGraphicsItemFor(element);

  //Loadings its old values, that we saved when it was deleted
	loadElementConfiguration();

	if(!this->mIsInitialRedo)
	{
		//If the element that is being created is a group, load the measurements of the group and the location of the
		// elements in the group
		if (auto group = dynamic_cast<cedar::proc::gui::Group *>(this->mpGuiElement))
		{
      group->setSize(this->mWidthOfGroup, this->mHeightOfGroup);
		  if(this->mGroupIsCollapsed)
      {
        group->setCollapsed(true);
      }
			//Set positions of elements
			std::map<std::string, cedar::proc::ElementPtr> elements = group->getGroup()->getElements();
			int i = 0;
			for (std::pair<std::string, cedar::proc::ElementPtr> element:elements)
			{
				cedar::proc::gui::Element *guiElement = this->mpScene->getGraphicsItemFor(element.second.get());
				if (guiElement != nullptr)
				{
					guiElement->setPos(this->mPositionOfElementsInGroup[i]);
					i++;
				}
			}
		}
	}
	//Since the group the element is in could have changed its name the elementFullPath has to be updated
	this->mElementFullPath = this->mpGuiElement->getElement()->getFullPath();
}

void cedar::proc::undoRedo::commands::CreateDeleteElement::deleteElement()
{
  if(this->mpGuiElement != nullptr && this->mpScene->items().contains(this->mpGuiElement))
  {
		//Before deleting the element the configuration has to be saved, so when its created it again in redo, the load old
		// parameter are loaded
		saveElementConfiguration();

		//If the element that is being deleted is a group, save the measurements of the group and the location of the
		// elements in the group
		if(auto* group = dynamic_cast<cedar::proc::gui::Group*>(mpGuiElement))
		{
			//Save size of the group
      this->mGroupIsCollapsed = group->isCollapsed();
      if(this->mGroupIsCollapsed)
      {
        this->mWidthOfGroup = group->getUncollapsedWidth();
        this->mHeightOfGroup = group->getUncollapsedHeight();
      }
      else
      {
        this->mWidthOfGroup = group->width();
        this->mHeightOfGroup = group->height();
      }

			//Save to positions of the elements
			std::map<std::string, cedar::proc::ElementPtr> elements = group->getGroup()->getElements();
			for(std::pair<std::string, cedar::proc::ElementPtr> element:elements)
			{
				cedar::proc::gui::Element* guiElement = this->mpScene->getGraphicsItemFor(element.second.get());
				if(guiElement!=nullptr)
				{
					this->mPositionOfElementsInGroup.push_back(guiElement->pos());
				}
			}
		}
		this->mpGuiElement->deleteElement();
  }
}

void cedar::proc::undoRedo::commands::CreateDeleteElement::saveElementConfiguration()
{
	this->mElementConfiguration.clear();
	this->mpGuiElement->getElement()->writeConfiguration(this->mElementConfiguration);
}

void cedar::proc::undoRedo::commands::CreateDeleteElement::loadElementConfiguration()
{
  if(!this->mElementConfiguration.empty())
  {
		this->mpGuiElement->getElement()->readConfiguration(this->mElementConfiguration);
  }
}