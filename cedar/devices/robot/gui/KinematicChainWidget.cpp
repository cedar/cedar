/*======================================================================================================================

    Copyright 2011 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany

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

    File:        KinematicChainWidget.cpp

    Maintainer:  Bjoern Weghenkel
    Email:       bjoern.weghenkel@ini.rub.de
    Date:        2011 01 06

    Description: Implementation of the @em cedar::dev::robot::KinematicChainWidget class.

    Credits:

======================================================================================================================*/

// CEDAR INCLUDES
#include "cedar/devices/robot/gui/KinematicChainWidget.h"
#include "cedar/auxiliaries/exceptions.h"
#include "cedar/devices/robot/gui/KinematicChainMonitorWidget.h"
#include "cedar/devices/robot/gui/KinematicChainCommandWidget.h"

// SYSTEM INCLUDES
#include <iostream>
#include <QApplication>
#include <QGridLayout>
#include <QGroupBox>

//----------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------

cedar::dev::robot::gui::KinematicChainWidget::KinematicChainWidget
(
  cedar::dev::robot::KinematicChainPtr kinematicChain,
  QWidget *parent,
  Qt::WindowFlags
)
:
QWidget(parent)
{
  init(kinematicChain);
  return;
}

cedar::dev::robot::gui::KinematicChainWidget::~KinematicChainWidget()
{

}


//------------------------------------------------------------------------------
// methods
//------------------------------------------------------------------------------

void cedar::dev::robot::gui::KinematicChainWidget::init(cedar::dev::robot::KinematicChainPtr kinematicChain)
{
  setWindowTitle(QApplication::translate("KinematicChainWindow", "KinematicChain"));

  QGridLayout* p_layout = new QGridLayout();
  QGroupBox* p_monitor_group = new QGroupBox();
//  p_monitor_group->setTitle("Monitor");
  p_layout->addWidget(p_monitor_group, 0, 0);
  mpMonitorWidget = new cedar::dev::robot::gui::KinematicChainMonitorWidget(kinematicChain, this);
  p_layout->addWidget(mpMonitorWidget, 0, 0);
  QGroupBox* p_command_group = new QGroupBox();
//  p_command_group->setTitle("Command");
  p_layout->addWidget(p_command_group, 0, 1);
  mpCommandWidget = new cedar::dev::robot::gui::KinematicChainCommandWidget(kinematicChain);
  p_layout->addWidget(mpCommandWidget, 0, 1);
  this->setLayout(p_layout);

  this->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
}

cedar::dev::robot::gui::KinematicChainMonitorWidget* cedar::dev::robot::gui::KinematicChainWidget::getMonitorWidget()
{
  return mpMonitorWidget;
}

cedar::dev::robot::gui::KinematicChainCommandWidget* cedar::dev::robot::gui::KinematicChainWidget::getCommandWidget()
{
  return mpCommandWidget;
}
