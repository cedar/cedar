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

    File:        GroupWidget.cpp

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2014 11 19

    Description: Source file for the class cedar::proc::gui::GroupWidget.

    Credits:

======================================================================================================================*/

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/processing/gui/GroupWidget.h"
#include "cedar/processing/gui/Group.h"
#include "cedar/processing/gui/Scene.h"
#include "cedar/processing/gui/View.h"
#include "cedar/auxiliaries/Configurable.h"

// SYSTEM INCLUDES
#include <QVBoxLayout>

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::proc::gui::GroupWidget::GroupWidget(QWidget* pParent)
:
QWidget(pParent),
mpView(new cedar::proc::gui::View())
{
  auto layout = new QVBoxLayout();
  this->setLayout(layout);
  layout->setContentsMargins(0, 0, 0, 0);
  layout->addWidget(this->mpView);

  this->mpView->resetViewport();

  this->mGroup = cedar::proc::gui::GroupPtr(new cedar::proc::gui::Group(nullptr, this->mpView->getScene()));

  this->mpView->getScene()->setGroup(this->mGroup);

  this->setAcceptDrops(true);
}

cedar::proc::gui::GroupWidget::GroupWidget(cedar::proc::gui::Group* pGroup, QWidget* pParent)
:
QWidget(pParent),
mpView(new cedar::proc::gui::View())
{
  auto layout = new QVBoxLayout();
  this->setLayout(layout);
  layout->setContentsMargins(0, 0, 0, 0);
  layout->addWidget(this->mpView);

  this->mpView->resetViewport();

  this->mGroup = cedar::proc::gui::GroupPtr(new cedar::proc::gui::Group(nullptr, this->mpView->getScene()));
  this->mpView->getScene()->setGroup(this->mGroup);
  this->mGroup->setGroup(pGroup->getGroup());
  this->mGroup->addElementsToGroup();
  cedar::aux::ConfigurationNode node;
  pGroup->writeConfiguration(node);
  this->mGroup->tryRestoreUIConfigurationsOfElements(node);
  this->mGroup->restoreConnections();
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

cedar::proc::gui::GroupPtr cedar::proc::gui::GroupWidget::getGroup() const
{
  return this->mGroup;
}

void cedar::proc::gui::GroupWidget::setConfigurableWidget(cedar::aux::gui::Configurable* pConfigurableWidget)
{
  this->mpView->getScene()->setConfigurableWidget(pConfigurableWidget);
}

void cedar::proc::gui::GroupWidget::setRecorderWidget(cedar::proc::gui::RecorderWidget* pRecorderWidget)
{
  this->mpView->getScene()->setRecorderWidget(pRecorderWidget);
}
