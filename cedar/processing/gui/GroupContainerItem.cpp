/*======================================================================================================================

    Copyright 2011, 2012, 2013, 2014, 2015 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany
 
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

    File:        GroupContainerItem.cpp

    Maintainer:  Stephan Zibner
    Email:       stephan.zibner@ini.ruhr-uni-bochum.de
    Date:        2015 01 20

    Description: Source file for the class cedar::proc::gui::GroupContainerItem.

    Credits:

======================================================================================================================*/

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/processing/gui/GroupContainerItem.h"
#include "cedar/processing/gui/GroupWidget.h"
#include "cedar/processing/gui/Group.h"
#include "cedar/processing/Group.h"
#include "cedar/auxiliaries/Parameter.h"

// SYSTEM INCLUDES

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::proc::gui::GroupContainerItem::GroupContainerItem(cedar::proc::gui::Group* pGroup)
:
mpGroupWidget(new cedar::proc::gui::GroupWidget(pGroup))
{
  this->setWidget(this->mpGroupWidget);

  QObject::connect(pGroup->getGroup()->getParameter("name").get(), SIGNAL(valueChanged()), this, SLOT(groupNameChanged()));

  this->groupNameChanged();
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::proc::gui::GroupContainerItem::setConfigurableWidget(cedar::aux::gui::Configurable* pConfigurableWidget)
{
  this->mpGroupWidget->setConfigurableWidget(pConfigurableWidget);
}

void cedar::proc::gui::GroupContainerItem::setRecorderWidget(cedar::proc::gui::RecorderWidget* pRecorderWidget)
{
  this->mpGroupWidget->setRecorderWidget(pRecorderWidget);
}

void cedar::proc::gui::GroupContainerItem::groupNameChanged()
{
  auto group = this->mpGroupWidget->getGroup();
  this->setTitle(group->getGroup()->getName());
}
