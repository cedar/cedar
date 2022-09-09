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

		Maintainer:  Lars Janssen,
                 Yogeshwar Agnihotri,
    Email:       lars.janssen@ini.rub.de,
                 yogeshwar.agnihotri@ini.rub.de
    Date:        2020 07 23

    Description: Source file for the class cedar::proc::undoRedo::commands::CreateDeleteConnection.

    Credits:

======================================================================================================================*/

// CEDAR CONFIGURATION

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
//Constructor for creating a connection
cedar::proc::undoRedo::commands::CreateDeleteConnection::CreateDeleteConnection(
        cedar::proc::gui::GraphicsBase* source,
        cedar::proc::gui::GraphicsBase* target,
        bool isCreateCommand,
        bool createConnectorGroup)
:
mpSource(source),
mpTarget(target),
mIsCreateCommand(isCreateCommand),
mCreateConnectorGroup(createConnectorGroup)
{
  this->mpScene = dynamic_cast<cedar::proc::gui::Scene*>(this->mpTarget->scene());
  CEDAR_DEBUG_ASSERT(this->mpScene)
  setIdentifier();
  setTextForUndoRedoStackVisualizer();
}

//Constructor for deleting a connection
cedar::proc::undoRedo::commands::CreateDeleteConnection::CreateDeleteConnection(
				cedar::proc::gui::Connection* connection,
				bool isCreateCommand,
				bool createConnectorGroup)
:
mpSource(connection->getSource()),
mpTarget(connection->getTarget()),
mIsCreateCommand(isCreateCommand),
mCreateConnectorGroup(createConnectorGroup)
{
  this->mpScene = dynamic_cast<cedar::proc::gui::Scene *>(this->mpTarget->scene());
  CEDAR_DEBUG_ASSERT(this->mpScene)
  setIdentifier();
  setTextForUndoRedoStackVisualizer();
}

cedar::proc::undoRedo::commands::CreateDeleteConnection::~CreateDeleteConnection()
{
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::proc::undoRedo::commands::CreateDeleteConnection::undo()
{
  switch(this->mIsCreateCommand)
  {
    case true:
      deleteConnection();
      break;
    case false:
      createConnection();
      break;
  }
}

void cedar::proc::undoRedo::commands::CreateDeleteConnection::redo()
{
  switch(this->mIsCreateCommand)
  {
    case true:
      createConnection();
      break;
    case false:
      deleteConnection();
      break;
  }
}

void cedar::proc::undoRedo::commands::CreateDeleteConnection::createConnection()
{
  updateSourceAndTargetConnectors();

  if(this->mpSource != nullptr && this->mpTarget != nullptr && this->mpScene != nullptr)
  {
    this->mpScene->createConnection(this->mpSource, this->mpTarget,
    				this->mCreateConnectorGroup);
  }
}

void cedar::proc::undoRedo::commands::CreateDeleteConnection::deleteConnection()
{
  updateSourceAndTargetConnectors();

  auto* sourceGuiDataSlot = dynamic_cast<cedar::proc::gui::DataSlotItem*>(this->mpSource);
  auto* targetGuiDataSlot = dynamic_cast<cedar::proc::gui::DataSlotItem*>(this->mpTarget);
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

void cedar::proc::undoRedo::commands::CreateDeleteConnection::setIdentifier()
{
  setIdentifierInternal(this->mpSource, this->mSourceSlotName, this->mSourceElementFullPath,
  				this->mSourceExternalName);
  setIdentifierInternal(this->mpTarget, this->mTargetSlotName, this->mTargetElementFullPath,
  				this->mTargetExternalName);
}

void cedar::proc::undoRedo::commands::CreateDeleteConnection::setIdentifierInternal(
				cedar::proc::gui::GraphicsBase* slot, std::string& slotName, std::string& elementFullPath,
				std::string& external)
{
  // (graphicsBase) casted down to a dataSlotItem (graphical representation of DataSlot)
  if(auto* dataSlotItem = dynamic_cast<cedar::proc::gui::DataSlotItem*>(slot))
  {
    //Name of the slot itself
    slotName = dataSlotItem->getSlot()->getName();

    if (auto* element = dynamic_cast<cedar::proc::Element *>(dataSlotItem->getSlot()->getParentPtr()))
    {
      //Normal case
      if (cedar::proc::gui::Element *guiElement = this->mpScene->getGraphicsItemFor(element))
      {
        elementFullPath = guiElement->getElement()->getFullPath();
        external = "";
      }
        //This happens if the externalElement is used
      else
      {
        auto* group = dynamic_cast<cedar::proc::Element *>(element->getGroup().get());
        CEDAR_ASSERT(group != nullptr)
        elementFullPath = this->mpScene->getGraphicsItemFor(group)->getElement()->getFullPath();
        external = element->getName();
      }
    }
  }
}

void cedar::proc::undoRedo::commands::CreateDeleteConnection::updateSourceAndTargetConnectors()
{
  updateSourceAndTargetConnectorsInternal(this->mSourceElementFullPath, this->mSourceExternalName,
  				this->mSourceSlotName, this->mpSource, cedar::proc::DataRole::OUTPUT);
	updateSourceAndTargetConnectorsInternal(this->mTargetElementFullPath, this->mTargetExternalName,
					this->mTargetSlotName, this->mpTarget, cedar::proc::DataRole::INPUT);

}

void cedar::proc::undoRedo::commands::CreateDeleteConnection::updateSourceAndTargetConnectorsInternal(
				std::string fullPath,
				std::string external,
				std::string slotName,
				cedar::proc::gui::GraphicsBase*& slot,
				cedar::proc::DataRole::Id dataRole)
{
	cedar::proc::gui::Element* guiElement = this->mpScene->getElementByFullPath(fullPath);

	if(cedar::proc::gui::Connectable* connectable = dynamic_cast<cedar::proc::gui::Connectable*>(guiElement))
	{
		//Normal case
		if(external == "")
		{
			slot = connectable->getSlotItem(dataRole, slotName);
		}
			//mSourceExternalName was set => The connection source is a externalOutput
		else
		{
			//Get the group of the externalOutput
			if(cedar::proc::gui::Group* group = dynamic_cast<cedar::proc::gui::Group*>(guiElement))
			{
				//Get externalConnector (externalOutput/-Input) itself
				cedar::proc::Connectable* externalConnector = dynamic_cast<cedar::proc::Connectable*>(
								group->getGroup()->getElement(external).get());
				CEDAR_ASSERT(externalConnector!= nullptr);
				//Get source of the externalOutput using the sourceSlotName
        if(dataRole == cedar::proc::DataRole::OUTPUT)
        {
          slot = group->getSourceConnectorItem(externalConnector->getOutputSlot(slotName));
        }
        else
        {
          slot = group->getSinkConnectorItem(externalConnector->getInputSlot(slotName));
        }
			}
		}
	}
}

void cedar::proc::undoRedo::commands::CreateDeleteConnection::setTextForUndoRedoStackVisualizer()
{
  std::string text;
  if(this->mIsCreateCommand)
  {
    text = "Added";
  }
  else
  {
    text = "Removed";
  }
  setText(QString::fromStdString(text + " connection: " + this->mSourceElementFullPath + "-> " +
  this->mTargetElementFullPath));
}