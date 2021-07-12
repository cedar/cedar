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

    File:        CreateDeleteElement.h

    Maintainer:  Yogeshwar Agnihotri,
    						 Lars Janssen
    Email:       yogeshwar.agnihotri@ini.ruhr-uni-bochum.de,
    						 lars.janssen@ini.rub.de
    Date:        2021 04 07

    Description: Header file for the class cedar::proc::undoRedo::commands::CreateDeleteElement.

    Credits:

======================================================================================================================*/

#ifndef CEDAR_PROC_UNDO_REDO_COMMANDS_CREATE_DELETE_ELEMENT_H
#define CEDAR_PROC_UNDO_REDO_COMMANDS_CREATE_DELETE_ELEMENT_H

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/processing/undoRedo/UndoCommand.h"
#include "cedar/processing/gui/Group.h"
#include "cedar/processing/gui/Element.h"
#include "cedar/processing/gui/Connectable.h"

// FORWARD DECLARATIONS
#include "cedar/processing/undoRedo/commands/CreateDeleteElement.fwd.h"

// SYSTEM INCLUDES
#include <QPointF>

/*!
 *  Undo/redo command for element creation/deletion
 */
class cedar::proc::undoRedo::commands::CreateDeleteElement : public UndoCommand
{
  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief Constructor for creating an element
  CreateDeleteElement(QPointF position,std::string classId, cedar::proc::GroupPtr group,cedar::proc::gui::Scene* scene,
  				bool isCreateCommand);

  //!@brief Constructor for deleting an element
  CreateDeleteElement(cedar::proc::gui::Element* element, cedar::proc::gui::Scene* scene,
                bool isCreateCommand);

  //!@brief Destructor
  virtual ~CreateDeleteElement();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
	void undo() override;
	void redo() override;
  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  //!@brief Creates an element and sets its mpGuiElement
  void createElement();

  //!@brief Deletes an element
  void deleteElement();

  //!@brief Saves the configuration of the element into the configurationNode 'mElementConfiguration'
  void saveElementConfiguration();

  //!@brief Loads the configuration from the configurationNode 'mElementConfiguration' into mpGuiElement
  void loadElementConfiguration();

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  // none yet
  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet
private:
  //!@brief pointer to different instances of objects that are needed
  cedar::proc::gui::Element* mpGuiElement;
	cedar::proc::GroupPtr mpTargetGroup;
	cedar::proc::gui::Scene* mpScene;

  //!@brief full paths to identify the elements when their pointer is outdated
  std::string mElementFullPath;
  std::string mClassId;

  cedar::aux::ConfigurationNode mElementConfiguration;

  //!@brief positions and sizes
  QPointF mPosition;
  qreal mWidthOfGroup;
  qreal mHeightOfGroup;
  std::vector<QPointF> mPositionOfElementsInGroup;

  //!@brief instance of the action enum. Saves for what to command was intended (create or delete)
  bool mIsCreateCommand;

  //!@brief if the element is a group this stores if its collapsed or not
  bool mGroupIsCollapsed;

  //!@brief boolean to realize the first redo
  bool mIsInitialRedo;
  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

private:
  // none yet

};// class cedar::proc::undoRedo::commands::CreateDeleteElement

#endif // CEDAR_PROC_UNDO_REDO_COMMANDS_CREATE_DELETE_ELEMENT_H

