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

    Maintainer:  Yogeshwar Agnihotri
    Email:       yogeshwar.agnihotri@ini.ruhr-uni-bochum.de
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
cedar::proc::undoRedo::commands::CreateDeleteElement::CreateDeleteElement(QPointF position,std::string classId, cedar::proc::GroupPtr group,cedar::proc::gui::Scene* scene,cedar::proc::undoRedo::commands::CreateDeleteElement::Action action)
:
mAction(action),
mIsInitialRedo(true),
mPosition(position),
mClassId(classId),
mpGroup(group),
mpScene(scene)
{
}

//Constructor for deleting an element
cedar::proc::undoRedo::commands::CreateDeleteElement::CreateDeleteElement(cedar::proc::gui::Element* element, cedar::proc::gui::Scene* scene, cedar::proc::undoRedo::commands::CreateDeleteElement::Action action)
:
mpGuiElement(element),
mAction(action),
mIsInitialRedo(true),
mpScene(scene)
{
  mPosition = element->pos();
  mClassId = cedar::proc::ElementManagerSingleton::getInstance()->getTypeId(element->getElement());

  //Finding the highest level group the element is in
  QGraphicsItem* parentItem = element->parentItem();
  while(parentItem != nullptr)
  {
    //Check if the parentItem is a group, if yes set the parentItem as the group of the element
    if (cedar::proc::gui::Group* groupOfElement = dynamic_cast<cedar::proc::gui::Group*>(parentItem))
    {
      mpGroup = groupOfElement->getGroup();
      break;
    }
    else
    {
      parentItem = parentItem->parentItem();
    }
  }

  if(parentItem == nullptr)
  {
    mpGroup = scene->getRootGroup()->getGroup();
  }

  //Since the element already exists, set the elementIdentifier
  mElementIdentifier = mpGuiElement->getElement()->getFullPath();

  //Set text for the 'Undo/Redo Stack'
  setText(QString::fromStdString("Deleted element:" + mElementIdentifier));
}

cedar::proc::undoRedo::commands::CreateDeleteElement::~CreateDeleteElement()
{
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::proc::undoRedo::commands::CreateDeleteElement::undo()
{
  switch(mAction)
  {
    case Action::CREATE:
      //Before deleting the element update the address of the element and its parentGroup in case it has been changed through a create
      //Uses elementIdentifier
      mpGuiElement = mpScene->getElementByFullPath(mElementIdentifier);
      mpGroup = mpScene->getGroupOfElementByFullPath(mElementIdentifier);

      if(mpGuiElement != nullptr)
      {
        //Before deleting the element the configuration has to be saved, so when its created it again in redo, the load old parameter are loaded
        saveElementConfiguration();
        //Undo of createElement = deleteElement
        deleteElement();
      }
      break;
    case Action::DELETE:
      //Update group since it could have changed
      mpGroup = mpScene->getGroupOfElementByFullPath(mElementIdentifier);
      createElement();
      //Loadings its old values, that we saved when it was deleted
      loadElementConfiguration();
      //Since the group the element is in could have changed its name the elementIdentifier has to be updated
      mElementIdentifier = mpGuiElement->getElement()->getFullPath();
      break;
  }
}

void cedar::proc::undoRedo::commands::CreateDeleteElement::redo()
{
  switch(mAction)
  {
    case Action::CREATE:
      if(mIsInitialRedo)
      {
        mIsInitialRedo = false;
        createElement();
        mElementIdentifier = mpGuiElement->getElement()->getFullPath();
        //Set text for the 'Undo/Redo Stack'
        setText(QString::fromStdString("Created element:" + mElementIdentifier));
      }
      else
      {
        //Group could have been changed
        mpGroup = mpScene->getGroupOfElementByFullPath(mElementIdentifier);
        createElement();
        //Reload old values which have been saved
        loadElementConfiguration();

        mElementIdentifier = mpGuiElement->getElement()->getFullPath();
      }
      break;

    case Action::DELETE:
      //Update guiElement and group before deleting if they have been changed
      mpGuiElement = mpScene->getElementByFullPath(mElementIdentifier);
      mpGroup = mpScene->getGroupOfElementByFullPath(mElementIdentifier);

      if(mpGuiElement != nullptr)
      {
        //Before deleting the element the configuration has to be saved, so when its created it again in redo, the load old parameter are loaded
        saveElementConfiguration();
        //Undo of createElement = deleteElement
        deleteElement();
      }
      break;
  }
}

void cedar::proc::undoRedo::commands::CreateDeleteElement::createElement()
{
  cedar::proc::Element* Element = mpScene->createElement(mpGroup,mClassId,mPosition).get();
  mpGuiElement = mpScene->getGraphicsItemFor(Element);
}

void cedar::proc::undoRedo::commands::CreateDeleteElement::deleteElement()
{
  if(this->mpGuiElement != nullptr && this->mpScene->items().contains(this->mpGuiElement))
  {
    mpGuiElement->deleteElement();
  }
}

void cedar::proc::undoRedo::commands::CreateDeleteElement::saveElementConfiguration()
{
  mElementConfiguration.clear();
  mpGuiElement->getElement()->writeConfiguration(mElementConfiguration);
}

void cedar::proc::undoRedo::commands::CreateDeleteElement::loadElementConfiguration()
{
  if(!mElementConfiguration.empty())
  {
    mpGuiElement->getElement()->readConfiguration(mElementConfiguration);
  }
}