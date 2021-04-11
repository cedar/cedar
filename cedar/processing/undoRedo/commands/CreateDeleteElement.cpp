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
#include <boost/property_tree/json_parser.hpp>
#include <boost/algorithm/string.hpp>
#include <cstdlib>
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
  mElementIdentifier = getElementIdentifier(mpGuiElement);

  //Set text for the 'Undo/Redo Stack'
  setText(QString::fromStdString("Deleted element:" + mpGuiElement->getElement()->getName()));
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
      mpGuiElement = getElementAddress(mElementIdentifier,mpScene);
      mpGroup = getElementGroupAddress(mElementIdentifier,mpScene);

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
      mpGroup = getElementGroupAddress(mElementIdentifier,mpScene);
      createElement();
      //Loadings its old values, that we saved when it was deleted
      loadElementConfiguration();
      //Since the group the element is in could have changed its name the elementIdentifier has to be updated
      mElementIdentifier = getElementIdentifier(mpGuiElement);
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
        mElementIdentifier = getElementIdentifier(mpGuiElement);
        //Set text for the 'Undo/Redo Stack'
        setText(QString::fromStdString("Created element:" + mpGuiElement->getElement()->getName()));
      }
      else
      {
        //Group could have been changed
        mpGroup = getElementGroupAddress(mElementIdentifier,mpScene);
        createElement();
        //Reload old values which have been saved
        loadElementConfiguration();

        mElementIdentifier = getElementIdentifier(mpGuiElement);
      }
      break;

    case Action::DELETE:
      //Update guiElement and group before deleting if they have been changed
      mpGuiElement = getElementAddress(mElementIdentifier,mpScene);
      mpGroup = getElementGroupAddress(mElementIdentifier,mpScene);

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

std::string cedar::proc::undoRedo::commands::CreateDeleteElement::getElementIdentifier(cedar::proc::gui::Element* guiElement)
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

cedar::proc::gui::Element* cedar::proc::undoRedo::commands::CreateDeleteElement::getElementAddress(std::string elementIdentifier, cedar::proc::gui::Scene* scene)
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

cedar::proc::GroupPtr cedar::proc::undoRedo::commands::CreateDeleteElement::getElementGroupAddress(std::string elementIdentifier, cedar::proc::gui::Scene* scene)
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
  return currentGroup->getGroup();
}