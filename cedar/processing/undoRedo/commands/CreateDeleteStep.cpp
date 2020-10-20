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
}

//Constructor for deleting an element
cedar::proc::undoRedo::commands::CreateDeleteStep::CreateDeleteStep(cedar::proc::gui::Element* element, cedar::proc::gui::Scene* scene, cedar::proc::undoRedo::commands::CreateDeleteStep::Action action)
:
mpGuiElement(element),
mpElement(element->getElement()),
mAction(action)
{
  mPosition = element->pos();
  mpScene = scene;
  mClassId = cedar::proc::ElementManagerSingleton::getInstance()->getTypeId(element->getElement());
  //@TODO: For now the Root
  mpGroup = scene->getRootGroup()->getGroup();
  //GetParent Check and loop to first
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
      //Before deleting a step we have to save its configuration, so when we create it again in redo we load its old parameter
      saveStepConfiguration();
      //Redo of deleteStep = deleteStep
      deleteStep();
      break;
  }
}

void cedar::proc::undoRedo::commands::CreateDeleteStep::createStep()
{
  mpElement = mpScene->createElement(mpGroup,mClassId,mPosition);
  mpGuiElement = mpScene->getGraphicsItemFor(mpElement);
}

void cedar::proc::undoRedo::commands::CreateDeleteStep::deleteStep()
{
  mpGuiElement->deleteElement();
}

void cedar::proc::undoRedo::commands::CreateDeleteStep::saveStepConfiguration()
{
  mpElement->writeConfiguration(mElementConfiguration);
}

void cedar::proc::undoRedo::commands::CreateDeleteStep::loadStepConfiguration()
{
  if(mElementConfiguration.empty() == false)
  {
    mpElement->readConfiguration(mElementConfiguration);
  }
}
