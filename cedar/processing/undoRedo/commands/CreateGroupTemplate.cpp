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

    File:        CreateGroupTemplate.cpp

    Maintainer:  Yogeshwar Agnihotri,
    						 Lars Janssen
    Email:       yogeshwar.agnihotri@ini.ruhr-uni-bochum.de,
    						 lars.janssen@ini.rub.de
    Date:        2021 01 20

    Description: Source file for the class cedar::proc::undoRedo::commands::CreateGroupTemplate.

    Credits:

======================================================================================================================*/

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CLASS HEADER
#include "cedar/processing/undoRedo/commands/CreateGroupTemplate.h"
#include "cedar/processing/gui/Group.h"

// CEDAR INCLUDES
#include "cedar/processing/GroupDeclarationManager.h"
#include "cedar/processing/gui/Element.h"
#include "cedar/processing/Element.h"
#include "cedar/processing/DeclarationRegistry.h"
#include <iostream>

// SYSTEM INCLUDES
#include <QGraphicsSceneDragDropEvent>

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::proc::undoRedo::commands::CreateGroupTemplate::CreateGroupTemplate(
				const cedar::proc::GroupDeclaration*groupDeclaration, cedar::proc::GroupPtr targetGroup,
				QGraphicsSceneDragDropEvent* pEvent, QPointF position, cedar::proc::gui::Scene* scene)
:
mpGroupDeclaration(groupDeclaration),
mpGuiElement(nullptr),
mpTargetGroup(targetGroup),
mpScene(scene),
mpEvent(pEvent),
mPosition(position),
mIsInitialRedo(true)
{
}

cedar::proc::undoRedo::commands::CreateGroupTemplate::~CreateGroupTemplate()
{
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------
void cedar::proc::undoRedo::commands::CreateGroupTemplate::undo()
{
  //Before deleting the element update the address of the element and its parentGroup in case it has been changed
  // through something that was on the stack after this command (e.g deletion of this element)
  this->mpGuiElement = this->mpScene->getElementByFullPath(this->mElementFullPath);
	this->mpTargetGroup = this->mpScene->getGroupOfElementByFullPath(this->mElementFullPath);

  if(this->mpGuiElement != nullptr)
  {
    deleteGroupTemplate();
  }
}

void cedar::proc::undoRedo::commands::CreateGroupTemplate::redo()
{
  if(this->mIsInitialRedo)
  {
		this->mIsInitialRedo = false;
    //Check if Ctrl key is pressed. If yes set the linkedGroup flag to true
		this->mLinkedGroupFlag = this->mpEvent->modifiers().testFlag(Qt::ControlModifier);
    createGroupTemplate();
    //Set text for the 'Undo/Redo Stack'
    setText(QString::fromStdString("Created group template: " + this->mpGuiElement->getElement()->getName()));
  }
  else
  {
    //Group could have changed
		this->mpTargetGroup = this->mpScene->getGroupOfElementByFullPath(this->mElementFullPath);
    createGroupTemplate();
  }
}

void cedar::proc::undoRedo::commands::CreateGroupTemplate::createGroupTemplate()
{
  cedar::proc::ElementPtr element = cedar::proc::GroupDeclarationManagerSingleton::getInstance()->
  				addGroupTemplateToGroup(this->mpGroupDeclaration->getClassName(), this->mpTargetGroup,
																	this->mLinkedGroupFlag);

  if(element!=nullptr)
  {
		this->mpGuiElement = this->mpScene->getGraphicsItemFor(element);
    if(this->mpGuiElement!=nullptr)
    {
			this->mpGuiElement->setPos(this->mPosition);
			//get now to full path since it has been created right now
			this->mElementFullPath = this->mpGuiElement->getElement()->getFullPath();
    }
		this->mClassId = cedar::proc::ElementManagerSingleton::getInstance()->getTypeId(element);
  }
}

void cedar::proc::undoRedo::commands::CreateGroupTemplate::deleteGroupTemplate()
{
  if(this->mpGuiElement != nullptr && this->mpScene->items().contains(this->mpGuiElement))
  {
		this->mpGuiElement->deleteElement();
  }
}
