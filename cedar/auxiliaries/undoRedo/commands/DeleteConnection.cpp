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

    File:        DeleteConnection.cpp

    Maintainer:  Yogeshwar Agnihotri
    Email:       yogeshwar.agnihotri@ini.ruhr-uni-bochum.de
    Date:        2020 07 04

    Description: Source file for the class cedar::aux::undoRedo::commands::DeleteConnection.

    Credits:

======================================================================================================================*/

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CLASS HEADER
#include "cedar/auxiliaries/undoRedo/commands/DeleteConnection.h"
#include "cedar/processing/gui/GraphicsBase.h"
#include "cedar/processing/gui/Connection.h"
#include "cedar/processing/gui/Group.h"
#include "cedar/processing/gui/StepItem.h"
#include "cedar/processing/gui/Scene.h"

// CEDAR INCLUDES

// SYSTEM INCLUDES

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::aux::undoRedo::commands::DeleteConnection::DeleteConnection(cedar::proc::gui::Connection* connection, cedar::proc::gui::Group* group)
:
source(connection->getSource()),
target(connection->getTarget()),
sourceSlotName(connection->getSourceSlotName()),
targetSlotName(connection->getTargetSlotName()),
group(group)
{
  //p_con->setSmartMode(group->getSmartConnection());

}

cedar::aux::undoRedo::commands::DeleteConnection::~DeleteConnection()
{
}

void cedar::aux::undoRedo::commands::DeleteConnection::undo() {
  this->group->getScene()->createConnection(source, target, false);
  /*if(source->canConnectTo(target) != cedar::proc::gui::CONNECT_NO)
  {
    if(auto element = dynamic_cast<cedar::proc::gui::Element*>(source))
    {
      if(auto group = dynamic_cast<cedar::proc::gui::Group*>(element->getGroup()))
      {

      }
    }
    //cedar::proc::gui::Connection *p_con = source_slot->connectTo(target_slot, sourceName + "." + sourceSlot, targetName + "." + targetSlot);
  }*/
}

void cedar::aux::undoRedo::commands::DeleteConnection::redo() {

}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------
