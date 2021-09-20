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

    File:        Paste.h

    Maintainer:  Yogeshwar Agnihotri,
    						 Lars Janssen
    Email:       yogeshwar.agnihotri@ini.ruhr-uni-bochum.de,
    						 lars.janssen@ini.rub.de
    Date:        2021 09 20

    Description: Header file for the class cedar::proc::undoRedo::commands::Paste.

    Credits:

======================================================================================================================*/

#ifndef CEDAR_PROC_UNDO_REDO_COMMANDS_PASTE_H
#define CEDAR_PROC_UNDO_REDO_COMMANDS_PASTE_H

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/processing/undoRedo/UndoCommand.h"
#include <cedar/processing/gui/Group.h>

// FORWARD DECLARATIONS
#include "cedar/processing/undoRedo/commands/Paste.fwd.h"

// SYSTEM INCLUDES


/*!@todo describe.
 *
 * @todo describe more.
 */
class cedar::proc::undoRedo::commands::Paste : public UndoCommand
{
  //--------------------------------------------------------------------------------------------------------------------
  // nested types
  //--------------------------------------------------------------------------------------------------------------------

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  Paste(std::string json, cedar::proc::gui::GroupPtr group, QPointF mousePositionScenePos);

  //!@brief Destructor
  virtual ~Paste();

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
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
	//!@brief change an elements name in the connection section of json
	void renameElementInConnection(boost::property_tree::ptree& connectionTree, std::string oldName, std::string newName, std::string sourceSlotName, std::string targetSlotName);

	//!@brief paste one element using json data nodes
	void pasteConfigurationNodes(cedar::aux::ConfigurationNode stepNode, cedar::aux::ConfigurationNode uiNode, cedar::aux::ConfigurationNode connectionNode, cedar::aux::ConfigurationNode groupNode);
  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet
private:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

private:
	cedar::proc::gui::GroupPtr mGroup;
	std::string mJson;
	QPointF mMousePositionScenePos;
	cedar::proc::gui::Scene* mpScene;
  // none yet

}; // class cedar::proc::undoRedo::commands::Paste

#endif // CEDAR_PROC_UNDO_REDO_COMMANDS_PASTE_H

