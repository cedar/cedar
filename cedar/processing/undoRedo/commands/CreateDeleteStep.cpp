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

    Maintainer:  Lars Janssen
    Email:       lars.janssen@ini.rub.de
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
//@TODO: maybe put more stuff into the ":" part and find out how to ":" part is called
cedar::proc::undoRedo::commands::CreateDeleteStep::CreateDeleteStep(QPointF position,std::string classId, cedar::proc::GroupPtr group,cedar::proc::gui::Scene* scene,cedar::proc::undoRedo::commands::CreateDeleteStep::Action action)
:
mAction(action)
{
  //mPosition = mpElement->pos();
  mPosition = position;
  //mClassId = cedar::proc::ElementManagerSingleton::getInstance()->getTypeId(mpElement->getElement());
  mClassId = classId;
  //auto connectable = dynamic_cast<cedar::proc::gui::Connectable*>(mpElement);
  //mpGuiGroup = connectable->getGuiGroup();
  mpGroup = group;
  mpScene = scene;
}

//Constructor for deleting an element
cedar::proc::undoRedo::commands::CreateDeleteStep::CreateDeleteStep(cedar::proc::gui::Element* element, cedar::proc::undoRedo::commands::CreateDeleteStep::Action action)
:
mpGuiElement(element),
mAction(action)
{
}

cedar::proc::undoRedo::commands::CreateDeleteStep::~CreateDeleteStep()
{
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::proc::undoRedo::commands::CreateDeleteStep::undo()
{
  switch(mAction)
  {
    case Action::CREATE:
      //Undo of createStep = deleteStep
      deleteStep();
      break;
    case Action::DELETE:
      //Undo of deleteStep = createStep
      createStep();
      break;
  }
}

void cedar::proc::undoRedo::commands::CreateDeleteStep::redo()
{
  switch(mAction)
  {
    case Action::CREATE:
      //Redo of createStep = createStep
      createStep();
      break;
    case Action::DELETE:
      //Redo of deleteStep = deleteStep
      deleteStep();
      break;
  }
}

void cedar::proc::undoRedo::commands::CreateDeleteStep::createStep()
{
  mpElement = mpScene->createElement(mpGroup,mClassId,mPosition);
}

void cedar::proc::undoRedo::commands::CreateDeleteStep::deleteStep()
{
  mpScene->getGraphicsItemFor(mpElement)->deleteElement();
}
