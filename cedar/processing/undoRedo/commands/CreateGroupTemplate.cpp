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

    Maintainer:  Yogeshwar Agnihotri
    Email:       yogeshwar.agnihotri@ini.ruhr-uni-bochum.de
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

cedar::proc::undoRedo::commands::CreateGroupTemplate::CreateGroupTemplate(const cedar::proc::GroupDeclaration* groupDeclaration, cedar::proc::GroupPtr group, QGraphicsSceneDragDropEvent* pEvent, QPointF mapped, cedar::proc::gui::Scene* scene)
:
mpGroupDeclaration(groupDeclaration),
mpGroup(group),
mpEvent(pEvent),
mPosition(mapped),
mpScene(scene),
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
  //Before deleting the element update the address of the element and its parentGroup in case it has been changed through a create
  //Uses elementFullPath
  mpGuiElement = mpScene->getElementByFullPath(mElementFullPath);
  mpGroup = mpScene->getGroupOfElementByFullPath(mElementFullPath);

  if(mpGuiElement != nullptr)
  {
    //Before deleting the element the configuration has to be saved, so when its created it again in redo, the load old parameter are loaded
    saveElementConfiguration();
    //Undo of createElement = deleteElement
    deleteGroupTemplate();
  }
}

void cedar::proc::undoRedo::commands::CreateGroupTemplate::redo()
{
  //Only create groupTemplate once at initial redo, else just create a normal element and loads its s
  if(mIsInitialRedo)
  {
    mIsInitialRedo = false;
    createGroupTemplate();
		mElementFullPath = mpGuiElement->getElement()->getFullPath();
    //Set text for the 'Undo/Redo Stack'
    setText(QString::fromStdString("Created groupTemplate: " + mpGuiElement->getElement()->getName()));
  }
  else
  {
    //Group could have been changed
    mpGroup = mpScene->getGroupOfElementByFullPath(mElementFullPath);
    createElement();
    //Loadings its old values, that we saved when it was deleted
    loadElementConfiguration();

		mElementFullPath = mpGuiElement->getElement()->getFullPath();
  }
}

void cedar::proc::undoRedo::commands::CreateGroupTemplate::createGroupTemplate()
{
  auto element = cedar::proc::GroupDeclarationManagerSingleton::getInstance()->addGroupTemplateToGroup(mpGroupDeclaration->getClassName(),mpGroup,mpEvent->modifiers().testFlag(Qt::ControlModifier));

  mpGuiElement = mpScene->getGraphicsItemFor(element);
  mpGuiElement->setPos(mPosition);
  mpGroup = mpScene->getGroupOfElementByFullPath(mElementFullPath);
  mClassId = cedar::proc::ElementManagerSingleton::getInstance()->getTypeId(element);
}

void cedar::proc::undoRedo::commands::CreateGroupTemplate::createElement()
{
  cedar::proc::Element* Element = mpScene->createElement(mpGroup,mClassId,mPosition).get();
  mpGuiElement = mpScene->getGraphicsItemFor(Element);
}

void cedar::proc::undoRedo::commands::CreateGroupTemplate::deleteGroupTemplate()
{
  if(this->mpGuiElement != nullptr && this->mpScene->items().contains(this->mpGuiElement))
  {
    mpGuiElement->deleteElement();
  }
}

void cedar::proc::undoRedo::commands::CreateGroupTemplate::saveElementConfiguration()
{
  mElementConfiguration.clear();
  mpGuiElement->getElement()->writeConfiguration(mElementConfiguration);
}

void cedar::proc::undoRedo::commands::CreateGroupTemplate::loadElementConfiguration()
{
  if(mElementConfiguration.empty() == false)
  {
    mpGuiElement->getElement()->readConfiguration(mElementConfiguration);
  }
}