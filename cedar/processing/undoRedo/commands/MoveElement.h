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

    File:        MoveElement.h

    Maintainer:  Lars Janssen,
    						 Yogeshwar Agnihotri
    Email:       lars.janssen@ini.rub.de,
								 yogeshwar.agnihotri@ini.ruhr-uni-bochum.de
    Date:        2020 07 23

    Description: Header file for the class cedar::proc::undoRedo::commands::MoveElement.

    Credits:

======================================================================================================================*/

#ifndef CEDAR_PROC_UNDO_REDO_COMMANDS_MOVE_ELEMENT_H
#define CEDAR_PROC_UNDO_REDO_COMMANDS_MOVE_ELEMENT_H

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/processing/undoRedo/UndoCommand.h"
#include "cedar/processing/gui/Element.h"
#include "cedar/processing/gui/Group.h"

// FORWARD DECLARATIONS
#include "cedar/processing/undoRedo/commands/MoveElement.fwd.h"
#include "cedar/processing/gui/GraphicsBase.fwd.h"
#include "cedar/processing/gui/Scene.fwd.h"

// SYSTEM INCLUDES
#include <QPointF>

/*! Move element command. An UndoCommand implementation for moving an element in the scene (e.g. a step)
 */
class cedar::proc::undoRedo::commands::MoveElement : public UndoCommand
{
  //--------------------------------------------------------------------------------------------------------------------
  // nested types
  //--------------------------------------------------------------------------------------------------------------------

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  MoveElement(std::list<QGraphicsItem*> elements,
              cedar::proc::gui::Group* sourceGroup,
              cedar::proc::gui::Group* targetGroup,
              std::vector<QPointF> sourcePositions,
              cedar::proc::gui::Scene* scene);
  //!@brief Destructor
  virtual ~MoveElement();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  void undo();
  void redo();

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
	//!@brief moves the mGuiElements to given position
	void move(std::vector<QPointF> positions);
	//!@brief adds the mGuiElements to given group
  void addElementsToGroup(cedar::proc::gui::Group* group);
	//!@brief updates full path of all guiElements, sourceGroup and targetGroup
  void updateFullPath();
		//!@brief updates pointer of all guiElements, sourceGroup and targetGroup
  void updatePointer();
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
	//!@brief instances of all guiElements used in the move
  std::list<cedar::proc::gui::Element*> mGuiElements;

	//!@brief source and target groups of the moving. Is set to nullptr if rootgroup.
  cedar::proc::gui::Group* mSourceGroup;
  cedar::proc::gui::Group* mTargetGroup;

	//!@brief positions
  std::vector<QPointF> mSourcePositions;
  std::vector<QPointF> mTargetPositions;

	//!@brief full paths to identify the elements when their pointer is outdated
  std::vector<std::string> mElementFullPaths;
  std::string mSourceGroupFullPath;
  std::string mTargetGroupFullPath;

	//!@brief instance of scene
  cedar::proc::gui::Scene* mpScene;

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

private:
  // none yet

}; // class cedar::proc::undoRedo::commands::MoveElement

#endif // CEDAR_PROC_UNDO_REDO_COMMANDS_MOVE_ELEMENT_H

