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

    File:        CreateDeleteConnection.cpp

    Maintainer:  Lars Janssen
    Email:       lars.janssen@ini.rub.de
    Date:        2020 07 23

    Description: Source file for the class cedar::proc::undoRedo::commands::CreateDeleteConnection.

    Credits:

======================================================================================================================*/

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/processing/undoRedo/commands/CreateDeleteConnection.h"
#include "cedar/processing/gui/GraphicsBase.h"
#include "cedar/processing/gui/Connection.h"
#include "cedar/processing/gui/Group.h"
#include "cedar/processing/gui/StepItem.h"
#include "cedar/processing/gui/Scene.h"
#include "cedar/processing/gui/DataSlotItem.h"

// SYSTEM INCLUDES


//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------
cedar::proc::undoRedo::commands::CreateDeleteConnection::CreateDeleteConnection(cedar::proc::gui::GraphicsBase* source,
                                                                                cedar::proc::gui::GraphicsBase* target,
                                                                                cedar::proc::undoRedo::commands::CreateDeleteConnection::Action action,
                                                                                bool createConnectorGroup)
:
mpSource(source),
mpTarget(target),
mAction(action),
mCreateConnectorGroup(createConnectorGroup)
{
  this->mpScene = dynamic_cast<cedar::proc::gui::Scene *>(this->mpTarget->scene());
  CEDAR_DEBUG_ASSERT(this->mpScene)
  updateSourceTargetNameAndSlotName();

  std::string text;
  if(action == CREATE)
  {
    text = "Added";
  }
  else if(action == DELETE)
  {
    text = "Removed";
  }
  setText(QString::fromStdString(text + " connection: " + mSourceElementIdentifier + "->" + mTargetElementIdentifier));
}

cedar::proc::undoRedo::commands::CreateDeleteConnection::CreateDeleteConnection(cedar::proc::gui::Connection* connection,
                                                                                cedar::proc::undoRedo::commands::CreateDeleteConnection::Action action,
                                                                                bool createConnectorGroup)
:
mpSource(connection->getSource()),
mpTarget(connection->getTarget()),
mAction(action),
mCreateConnectorGroup(createConnectorGroup)
{
  this->mpScene = dynamic_cast<cedar::proc::gui::Scene *>(this->mpTarget->scene());
  CEDAR_DEBUG_ASSERT(this->mpScene)

  updateSourceTargetNameAndSlotName();

  std::string text;
  if(action == CREATE)
  {
    text = "Added";
  }
  else if(action == DELETE)
  {
    text = "Removed";
  }
  setText(QString::fromStdString(text + " connection: " + mSourceElementIdentifier + "->" + mTargetElementIdentifier));
}

cedar::proc::undoRedo::commands::CreateDeleteConnection::~CreateDeleteConnection()
{
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::proc::undoRedo::commands::CreateDeleteConnection::undo()
{
  switch(mAction)
  {
    case Action::CREATE:
      deleteConnection();
      break;
    case Action::DELETE:
      createConnection();
      break;
  }
}

void cedar::proc::undoRedo::commands::CreateDeleteConnection::redo()
{
  switch(mAction)
  {
    case Action::CREATE:
      createConnection();
      break;
    case Action::DELETE:
      deleteConnection();
      break;
  }
}

void cedar::proc::undoRedo::commands::CreateDeleteConnection::createConnection()
{
  updateSourceAndTargetConnectors();

  if(this->mpSource != nullptr && this->mpTarget != nullptr && this->mpScene != nullptr)
  {
    this->mpScene->createConnection(this->mpSource, this->mpTarget, mCreateConnectorGroup);
  }
}

void cedar::proc::undoRedo::commands::CreateDeleteConnection::deleteConnection()
{
  //Update the SourceElementName/SourceSlotName and TargetElementName/TargetSlotName before deleting
  updateSourceTargetNameAndSlotName();
  updateSourceAndTargetConnectors();

  cedar::proc::gui::DataSlotItem* sourceGuiDataSlot = dynamic_cast<cedar::proc::gui::DataSlotItem*>(mpSource);
  cedar::proc::gui::DataSlotItem* targetGuiDataSlot = dynamic_cast<cedar::proc::gui::DataSlotItem*>(mpTarget);
  if(sourceGuiDataSlot != nullptr && targetGuiDataSlot != nullptr)
  {
    cedar::proc::DataSlotPtr sourceDataSlot = sourceGuiDataSlot->getSlot();
    cedar::proc::DataSlotPtr targetDataSlot = targetGuiDataSlot->getSlot();

    if(cedar::proc::OwnedDataPtr source = boost::dynamic_pointer_cast<cedar::proc::OwnedData>(sourceDataSlot))
    {
      if(cedar::proc::ExternalDataPtr target = boost::dynamic_pointer_cast<cedar::proc::ExternalData>(targetDataSlot))
      {
        cedar::proc::Group::disconnectAcrossGroups(source,target);
      }
    }
  }
}

void cedar::proc::undoRedo::commands::CreateDeleteConnection::updateSourceTargetNameAndSlotName()
{
  //mpSource (graphicsBase) casted down to a dataSlotItem (graphical representation of DataSlot)
  if(auto sourceSlot = dynamic_cast<cedar::proc::gui::DataSlotItem*>(mpSource))
  {
    //Name of the slot itself
    this->mSourceSlotName = sourceSlot->getSlot()->getName();

    if(cedar::proc::Element* element = dynamic_cast<cedar::proc::Element*>(sourceSlot->getSlot()->getParentPtr()))
    {
      //Normal case
      if(cedar::proc::gui::Element* guiElement = this->mpScene->getGraphicsItemFor(element))
      {
        mSourceElementIdentifier = guiElement->getElement()->getFullPath();
        mSourceExternal = "";
      }
      //This happens if the externalElement is used
      else
      {
        auto group = dynamic_cast<cedar::proc::Element*>(element->getGroup().get());
        CEDAR_ASSERT(group!=nullptr)
        mSourceElementIdentifier = this->mpScene->getGraphicsItemFor(group)->getElement()->getFullPath();
        mSourceExternal = element->getName();
      }
    }
  }

  if(auto targetSlot = dynamic_cast<cedar::proc::gui::DataSlotItem*>(mpTarget))
  {
    //Name of the slot itself
    this->mTargetSlotName = targetSlot->getSlot()->getName();

    //This block checks if the connection is connected from a externalOutput
    if(cedar::proc::Element* element = dynamic_cast<cedar::proc::Element*>(targetSlot->getSlot()->getParentPtr()))
    {
      //Normal case
      if(cedar::proc::gui::Element* guiElement = this->mpScene->getGraphicsItemFor(element))
      {
        mTargetElementIdentifier = guiElement->getElement()->getFullPath();
        mTargetExternal = "";
      }
      //This happens if the externalElement is used
      else
      {
        auto group = dynamic_cast<cedar::proc::Element*>(element->getGroup().get());
        CEDAR_ASSERT(group!=nullptr)
        mTargetElementIdentifier = this->mpScene->getGraphicsItemFor(group)->getElement()->getFullPath();
        mTargetExternal = element->getName();
      }
    }
  }
}

void cedar::proc::undoRedo::commands::CreateDeleteConnection::updateSourceAndTargetConnectors()
{
  cedar::proc::gui::Element* sourceGuiElement = mpScene->getElementByFullPath(mSourceElementIdentifier);
  cedar::proc::gui::Element* targetGuiElement = mpScene->getElementByFullPath(mTargetElementIdentifier);


  if(cedar::proc::gui::Connectable* connectable = dynamic_cast<cedar::proc::gui::Connectable*>(sourceGuiElement))
  {
    //Normal case
    if(mSourceExternal == "")
    {
      mpSource = connectable->getSlotItem(cedar::proc::DataRole::OUTPUT, mSourceSlotName);
    }
    //mSourceExternal was set => The connection source is a externalOutput
    else
    {
      //Get the group of the externalOutput
      if(cedar::proc::gui::Group* group = dynamic_cast<cedar::proc::gui::Group*>(sourceGuiElement))
      {
        //Getting the externalOutput itself
        cedar::proc::Connectable* externalOutput = dynamic_cast<cedar::proc::Connectable*>(group->getGroup()->getElement(mSourceExternal).get());
        //Get the source of the externalOutput with using the sourceSlotName
        this->mpSource = group->getSourceConnectorItem(externalOutput->getOutputSlot(mSourceSlotName));
      }
    }
  }

  if(cedar::proc::gui::Connectable* connectable = dynamic_cast<cedar::proc::gui::Connectable*>(targetGuiElement))
  {
    if(mTargetExternal == "")
    {
      mpTarget = connectable->getSlotItem(cedar::proc::DataRole::INPUT, mTargetSlotName);
    }
    //mTargetExternal was set => The connection source is a externalInput
    else
    {
      //Get the group of the externalInput
      if(cedar::proc::gui::Group* group = dynamic_cast<cedar::proc::gui::Group*>(targetGuiElement))
      {
        //Getting the externalInput itself
        cedar::proc::Connectable* externalInput = dynamic_cast<cedar::proc::Connectable*>(group->getGroup()->getElement(mTargetExternal).get());
        //Get the target of the externalOutput with using the targetSlotName
        this->mpTarget = group->getSinkConnectorItem(externalInput->getInputSlot(mTargetSlotName));
      }
    }
  }
}