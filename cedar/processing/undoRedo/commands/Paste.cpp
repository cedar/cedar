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

    File:        Paste.cpp

    Maintainer:  Yogeshwar Agnihotri,
    						 Lars Janssen
    Email:       yogeshwar.agnihotri@ini.ruhr-uni-bochum.de,
    						 lars.janssen@ini.rub.de
    Email:       yogeshwar.agnihotri@ini.rub.de
    Date:        2021 09 20

    Description: Source file for the class cedar::proc::undoRedo::commands::Paste.

    Credits:

======================================================================================================================*/

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CLASS HEADER
#include "cedar/processing/undoRedo/commands/Paste.h"

// CEDAR INCLUDES
#include "cedar/auxiliaries/Configurable.h"
#include "cedar/processing/gui/Ide.h"

// SYSTEM INCLUDES
#include <boost/property_tree/json_parser.hpp>
#include <boost/algorithm/string/classification.hpp>

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::proc::undoRedo::commands::Paste::Paste(std::string json, cedar::proc::gui::GroupPtr group, QPointF mousePositionScenePos)
{
	this->mJson = json;
	this->mGroup = group;
	this->mMousePositionScenePos = mousePositionScenePos;
	this->mpScene = group->getScene();

	//Set text for the 'Undo/Redo Stack'
	setText(QString::fromStdString("Pasted elements"));
}

cedar::proc::undoRedo::commands::Paste::~Paste()
{
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------
void cedar::proc::undoRedo::commands::Paste::undo()
{
	std::vector<cedar::proc::gui::Element*> guiElements;
	for(std::string fullPath:mFullPathsOfPastedElements)
	{
		cedar::proc::gui::Element* guiElement = mpScene->getElementByFullPath(fullPath);
		if(guiElement!= nullptr)
		{
			guiElement->deleteElement();
		}
	}
}
void cedar::proc::undoRedo::commands::Paste::redo()
{
	////Make list of elements that were there before pasting
	QList<QGraphicsItem *> itemsBeforeImport =  mpScene->items();

	////Actual paste code
	//Convert back to ConfigurationNode
	cedar::aux::ConfigurationNode inputNode;
	std::stringstream ss;
	ss << this->mJson;
	read_json(ss, inputNode);

	//Get steps and ui tree
	cedar::aux::ConfigurationNode &stepsTree = inputNode.get_child("steps");
	cedar::aux::ConfigurationNode &uiTree = inputNode.get_child("ui");
	cedar::aux::ConfigurationNode &groupTree = inputNode.get_child("groups");

	//Get center of pasted elements to paste elements to the cursor position
	//Determine the position offset of the duplicates as the average of the positions of all selected elements
	QPointF center(0.0, 0.0);
	int counterofUis = 0;
	for (auto &uiPair:uiTree)
	{
		boost::property_tree::ptree::const_assoc_iterator it = uiPair.second.find("positionX");
		if(it != uiPair.second.not_found())
		{
			double positionX = uiPair.second.find("positionX")->second.get_value<double>();
			double positionY = uiPair.second.find("positionY")->second.get_value<double>();
			QPointF pointofStep(positionX, positionY);
			center += pointofStep;
			counterofUis++;
		}
		boost::property_tree::ptree::const_assoc_iterator itType = uiPair.second.find("type");
		if(itType != uiPair.second.not_found())
		{
			if (itType->second.get_value<std::string>() == "stickyNote")
			{
				double positionX = uiPair.second.find("x")->second.get_value<double>();
				double positionY = uiPair.second.find("y")->second.get_value<double>();
				QPointF pointofStep(positionX, positionY);
				center += pointofStep;
				counterofUis++;
			}
		}
	}
	for(auto &groupPair:groupTree)
	{
		auto ui_generic = groupPair.second.find("ui generic");
		double positionX = ui_generic->second.find("positionX")->second.get_value<double>();
		double positionY = ui_generic->second.find("positionY")->second.get_value<double>();
		double width = ui_generic->second.find("width")->second.get_value<double>();
		double height = ui_generic->second.find("height")->second.get_value<double>();
		QPointF pointofGroup(positionX + width / 2,positionY + height / 2);
		center += pointofGroup;
		counterofUis++;
	}

	center /= counterofUis;
	boost::optional<boost::property_tree::ptree &> connectionsTree = inputNode.get_child_optional("connections");
	//Get gui connections tree
	boost::optional<boost::property_tree::ptree &> uiConnectionsTree;
	for(auto &uiPair:uiTree)
	{
		boost::property_tree::ptree::const_assoc_iterator it = uiPair.second.find("type");
		if(it != uiPair.second.not_found())
		{
			std::string type = it->second.get_value<std::string>();
			if (type == "connections")
			{
				uiConnectionsTree = uiPair.second.get_child_optional("connections");
				if(uiConnectionsTree)
				{
					break;
				}
			}
		}
	}

	//Rename and add steps, rename connections
	for (auto &stepsPair:stepsTree)
	{
		cedar::aux::ConfigurationNode singleUiValue;
		cedar::aux::ConfigurationNode singleStepValue;
		std::string oldName = stepsPair.second.find("name")->second.get_value<std::string>();
		std::string newName = this->mGroup->getGroup()->getUniqueIdentifier(oldName);

		//In UI rename every occurrence of oldName to newName and change position
		for (auto &uiPair:uiTree)
		{
			boost::property_tree::ptree::const_assoc_iterator it = uiPair.second.find("step");
			if(it != uiPair.second.not_found())
			{
				std::string uiName = uiPair.second.find("step")->second.get_value<std::string>();

				if (uiName == oldName)
				{
					//Rename occurence
					uiPair.second.put("step", newName);

					////Change position
					double positionX = uiPair.second.find("positionX")->second.get_value<double>();
					double positionY = uiPair.second.find("positionY")->second.get_value<double>();
					QPointF pointofStep(positionX, positionY);

					//Add vector from center to step to mouse position
					QPointF vectorFromCenterToStep = pointofStep - center;
					QPointF newPositionOfStep = this->mMousePositionScenePos + vectorFromCenterToStep;

					//Set new position
					uiPair.second.put("positionX", newPositionOfStep.x());
					uiPair.second.put("positionY", newPositionOfStep.y());

					singleUiValue.push_back(cedar::aux::ConfigurationNode::value_type("", uiPair.second));
				}
			}
		}

		if(connectionsTree)
		{
			cedar::proc::gui::Ide::renameElementInConnection(*connectionsTree, oldName, newName, "source", "target");
		}
		if(uiConnectionsTree)
		{
			cedar::proc::gui::Ide::renameElementInConnection(*uiConnectionsTree, oldName, newName, "source slot", "target slot");
		}

		stepsPair.second.put("name", newName);

		cedar::aux::ConfigurationNode rootNode;

		singleStepValue.push_front(cedar::aux::ConfigurationNode::value_type(stepsPair.first, stepsPair.second));
		cedar::aux::ConfigurationNode emptyNode;

		this->pasteConfigurationNodes(singleStepValue, singleUiValue, emptyNode, emptyNode);
	}

	////StickyNotes
	for (auto &uiPair:uiTree)
	{
		cedar::aux::ConfigurationNode singleUiValue;

		boost::property_tree::ptree::const_assoc_iterator it = uiPair.second.find("type");
		if(it != uiPair.second.not_found() && it->second.get_value<std::string>() == "stickyNote")
		{
			////Change position
			//Get
			double positionX = uiPair.second.find("x")->second.get_value<double>();
			double positionY = uiPair.second.find("y")->second.get_value<double>();
			QPointF pointOfNote(positionX,positionY);

			//Add vector from center to stickynote to mouse position
			QPointF vectorFromCenterToNote = pointOfNote - center;
			QPointF newPostionOfNote = this->mMousePositionScenePos + vectorFromCenterToNote;

			//Set
			uiPair.second.put("x", std::to_string((int)newPostionOfNote.x()));
			uiPair.second.put("y",  std::to_string((int)newPostionOfNote.y()));
			singleUiValue.push_back(cedar::aux::ConfigurationNode::value_type("", uiPair.second));
		}

		cedar::aux::ConfigurationNode emptyNode;
		this->pasteConfigurationNodes(emptyNode,singleUiValue,emptyNode,emptyNode);
	}
	//Rename and add groups
	for(auto &groupPair:groupTree)
	{
		std::string oldName = groupPair.first;
		std::string newName = this->mGroup->getGroup()->getUniqueIdentifier(oldName);

		////Change position
		auto ui_generic = groupPair.second.find("ui generic");
		double positionX = ui_generic->second.find("positionX")->second.get_value<double>();
		double positionY = ui_generic->second.find("positionY")->second.get_value<double>();
		QPointF pointofGroup(positionX,positionY);

		//Manipulate: Get vector from center to step and add this vector to the mouse
		QPointF vectorFromCenterToGroup = pointofGroup - center;
		QPointF newPostionofGroup = this->mMousePositionScenePos + vectorFromCenterToGroup;
		//Set
		ui_generic->second.put("positionX", std::to_string(newPostionofGroup.x()));
		ui_generic->second.put("positionY",  std::to_string(newPostionofGroup.y()));

		//Rename other occurences at "name" and ui generic/group
		groupPair.second.put("name", newName);
		groupPair.second.find("ui generic")->second.put("group", newName);
		if (connectionsTree)
		{
			cedar::proc::gui::Ide::renameElementInConnection(*connectionsTree, oldName, newName,
							"source", "target");
		}
		if(uiConnectionsTree)
		{
			cedar::proc::gui::Ide::renameElementInConnection(*uiConnectionsTree, oldName, newName, "source slot", "target slot");
		}

		cedar::aux::ConfigurationNode singleGroupPair;
		singleGroupPair.push_front(cedar::aux::ConfigurationNode::value_type(newName, groupPair.second));

		cedar::aux::ConfigurationNode emptyNode;
		this->pasteConfigurationNodes(emptyNode,emptyNode,emptyNode,singleGroupPair);
	}

	////Generate Configuration node for ui connections
	cedar::aux::ConfigurationNode uiConnectionsNode;
	for (auto &uiPair:uiTree)
	{
		boost::property_tree::ptree::const_assoc_iterator it = uiPair.second.find("type");
		if(it != uiPair.second.not_found())
		{
			std::string type = it->second.get_value<std::string>();
			if (type == "connections")
			{
				uiConnectionsNode.push_front(cedar::aux::ConfigurationNode::value_type(uiPair.first, uiPair.second));
				break;
			}
		}
	}

	////Paste connections
	if (connectionsTree)
	{
		cedar::aux::ConfigurationNode emptyNode;
		this->pasteConfigurationNodes(emptyNode, uiConnectionsNode, *connectionsTree, emptyNode);
	}

	////Make list of elements that were there after pasting
	QList<QGraphicsItem *> itemsAfterImport =  mpScene->items();

	for(QGraphicsItem* itemAfterImport:itemsAfterImport)
	{
		if(!itemsBeforeImport.contains(itemAfterImport))
		{
			//This means the item is new. So it has to be added to the deletion list.
			mNewItemsAfterPasting.append(itemAfterImport);
			if(auto* element = dynamic_cast<cedar::proc::gui::Element*>(itemAfterImport))
			{
				mFullPathsOfPastedElements.push_back(element->getElement()->getFullPath());
			}
		}
	}
}

void cedar::proc::undoRedo::commands::Paste::pasteConfigurationNodes(cedar::aux::ConfigurationNode stepNode,
																										cedar::aux::ConfigurationNode uiNode,
																										cedar::aux::ConfigurationNode connectionNode,
																										cedar::aux::ConfigurationNode groupNode)
{
	cedar::aux::ConfigurationNode rootNode;

	//Add meta Infos
	cedar::aux::ConfigurationNode metaNode;
	metaNode.put("format", "1");

	rootNode.add_child("meta", metaNode);
	rootNode.add_child("steps", stepNode);
	rootNode.add_child("ui", uiNode);
	rootNode.add_child("connections", connectionNode);
	rootNode.add_child("groups", groupNode);

	std::stringstream stringstream;
	boost::property_tree::write_json(stringstream, rootNode);

	/*//Debug Option: Print modified pasted
	boost::property_tree::write_json("Paste: Modified Final Into read Function.json", rootNode);*/

	//Use readJsonFromString to paste the stringJson
	try
	{
		this->mGroup->readJsonFromString(stringstream.str(), false);
	}
	catch (const boost::property_tree::json_parser_error &e)
	{
		std::string info(e.what());
		std::cout << info << std::endl;
	}
}
