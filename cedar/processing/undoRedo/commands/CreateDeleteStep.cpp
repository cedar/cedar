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

    File:        CreateDeleteStep.cpp

    Maintainer:  Yogeshwar Agnihotri
    Email:       yogeshwar.agnihotri@ini.rub.de
    Date:        2020 07 23

    Description: Source file for the class cedar::proc::undoRedo::commands::CreateDeleteStep.

    Credits:

======================================================================================================================*/

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CLASS HEADER
#include "cedar/processing/undoRedo/commands/CreateDeleteStep.h"
#include "cedar/processing/DeclarationRegistry.h"

// CEDAR INCLUDES

// SYSTEM INCLUDES

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

//Constructor for creating an element
cedar::proc::undoRedo::commands::CreateDeleteStep::CreateDeleteStep(QPointF position,std::string classId, cedar::proc::GroupPtr group,cedar::proc::gui::Scene* scene,cedar::proc::undoRedo::commands::CreateDeleteStep::Action action)
:
mAction(action)
{
  //These 3 are needed for creating an element
  mPosition = position;
  mClassId = classId;
  mpGroup = group;
  //Scene is needed to create the element in the scene
  mpScene = scene;
  setText(QString::fromStdString("Created element"));
}

//Constructor for deleting an element
cedar::proc::undoRedo::commands::CreateDeleteStep::CreateDeleteStep(cedar::proc::gui::Element* element, cedar::proc::gui::Scene* scene, cedar::proc::undoRedo::commands::CreateDeleteStep::Action action)
:
mpGuiElement(element),
mAction(action)
{
  mPosition = element->pos();
  mpScene = scene;
  mClassId = cedar::proc::ElementManagerSingleton::getInstance()->getTypeId(element->getElement());

  //GetParent Check and loop to first
  QGraphicsItem* parentItem = element->parentItem();
  while(true)
  {
    //Check if the parentItem is a group, if yes set the parentItem as the Group of the Element. If not go one step above and so on
    if (cedar::proc::gui::Group* groupOfElement = dynamic_cast<cedar::proc::gui::Group *>(parentItem))
    {
      mpGroup = groupOfElement->getGroup();
      break;
    }
    else if(parentItem == nullptr)
    {
      mpGroup = scene->getRootGroup()->getGroup();
      break;
    }
    else
    {
      //Get the next parentItem of the parentItem before.
      parentItem = parentItem->parentItem();
    }
  }
  updateElementName();
  setText(QString::fromStdString("Deleted element"));
}

cedar::proc::undoRedo::commands::CreateDeleteStep::~CreateDeleteStep()
{
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

//STRG Z
void cedar::proc::undoRedo::commands::CreateDeleteStep::undo()
{
  switch(mAction)
  {
    case Action::CREATE:
      updateElementAddress();
      //Before deleting a step we have to save its configuration, so when we create it again in redo we load its old parameter
      saveStepConfiguration();
      //Undo of createStep = deleteStep
      deleteStep();
      break;
    case Action::DELETE:
      //Undo of deleteStep = createStep
      createStep();
      //Loadings its old values, that we saved when it was deleted
      loadStepConfiguration();
      break;
  }
}

//STRG Y
void cedar::proc::undoRedo::commands::CreateDeleteStep::redo()
{
  switch(mAction)
  {
    case Action::CREATE:
      //Redo of createStep = createStep
      createStep();
      //Loadings its old values, that we saved when it was deleted
      loadStepConfiguration();
      break;
    case Action::DELETE:
      updateElementAddress();
      //Before deleting a step we have to save its configuration, so when we create it again in redo we load its old parameter
      saveStepConfiguration();
      //Redo of deleteStep = deleteStep
      deleteStep();
      break;
  }
}

void cedar::proc::undoRedo::commands::CreateDeleteStep::createStep()
{
  cedar::proc::Element* Element = mpScene->createElement(mpGroup,mClassId,mPosition).get();
  mpGuiElement = mpScene->getGraphicsItemFor(Element);
  updateElementName();
}

void cedar::proc::undoRedo::commands::CreateDeleteStep::deleteStep()
{
  if(this->mpGuiElement != nullptr && this->mpScene->items().contains(this->mpGuiElement))
  {
    mpGuiElement->deleteElement();
  }
}

void cedar::proc::undoRedo::commands::CreateDeleteStep::saveStepConfiguration()
{
  mpGuiElement->getElement()->writeConfiguration(mElementConfiguration);
}

void cedar::proc::undoRedo::commands::CreateDeleteStep::loadStepConfiguration()
{
  if(mElementConfiguration.empty() == false)
  {
    mpGuiElement->getElement()->readConfiguration(mElementConfiguration);
  }
}

void cedar::proc::undoRedo::commands::CreateDeleteStep::updateElementName()
{
  mElementName = mpGuiElement->getElement()->getName();
}

void cedar::proc::undoRedo::commands::CreateDeleteStep::updateElementAddress()
{
  QList<QGraphicsItem *> items = this->mpScene->items();

  for(QGraphicsItem* item : items)
  {

    if(cedar::proc::gui::Element* element = dynamic_cast<cedar::proc::gui::Element*>(item))
    {
      if(!element->getElement()->getName().compare(mElementName))
      {
        mpGuiElement = element;
      }
    }
  }
}
