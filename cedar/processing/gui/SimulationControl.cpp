/*======================================================================================================================

    Copyright 2011, 2012, 2013, 2014 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany
 
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

    File:        SimulationControl.cpp

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2014 10 21

    Description: Source file for the class cedar::proc::gui::SimulationControl.

    Credits:

======================================================================================================================*/

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/processing/gui/SimulationControl.h"
#include "cedar/processing/gui/Group.h"
#include "cedar/processing/LoopedTrigger.h"
#include "cedar/auxiliaries/casts.h"

// SYSTEM INCLUDES

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::proc::gui::SimulationControl::SimulationControl()
{
  this->setupUi(this);

  auto element_filter = [] (cedar::proc::ConstElementPtr element)
  {
    return static_cast<bool>(boost::dynamic_pointer_cast<cedar::proc::ConstLoopedTrigger>(element));
  };
  this->mpTree->setFilter(boost::bind<bool>(element_filter, _1));

  this->mElementAddedConnection = this->mpTree->connectToElementAddedSignal
      (
        boost::bind
        (
          &cedar::proc::gui::SimulationControl::elementAdded,
          this,
          _1,
          _2
        )
      );
}

cedar::proc::gui::SimulationControl::~SimulationControl()
{
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::proc::gui::SimulationControl::setGroup(cedar::proc::gui::GroupPtr group)
{
  this->mGroup = group;
  this->mpTree->setGroup(this->mGroup->getGroup());
}

void cedar::proc::gui::SimulationControl::elementAdded(QTreeWidgetItem* pItem, cedar::proc::ElementPtr element)
{
  if (auto looped_trigger = boost::dynamic_pointer_cast<cedar::proc::LoopedTrigger>(element))
  {
    this->loopedTriggerAdded(pItem, looped_trigger);
  }
}

void cedar::proc::gui::SimulationControl::loopedTriggerAdded(QTreeWidgetItem* /* pItem */, cedar::proc::LoopedTriggerPtr /* loopedTrigger */)
{
}
