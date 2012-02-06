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
#include "stdio.h"
#include <iostream>
#include <QtGui/QApplication>
#include <QtGui/QGridLayout>
#include <QtGui/QGroupBox>

//----------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------

//cedar::dev::robot::gui::KinematicChainWidget::KinematicChainWidget
//(
//  const cedar::dev::robot::KinematicChainPtr &kinematicChain,
//  QWidget * parent,
//  Qt::WindowFlags f
//)
//:
//QWidget(parent, f)
//{
//  // store a smart pointer to KinematicChain
//  mpKinematicChains.push_back(kinematicChain);
//  mDecimals = 2;
//  mSingleStep = 0.01;
//
//  initWindow();
//  return;
//}


cedar::dev::robot::gui::KinematicChainWidget::KinematicChainWidget
(
  const cedar::dev::robot::KinematicChainPtr &kinematicChain,
  const std::string& configFileName,
  QWidget *parent,
  Qt::WindowFlags
)
:
QWidget(parent),
cedar::aux::ConfigurationInterface(configFileName)
{
  std::vector<cedar::dev::robot::KinematicChainPtr> kinematic_chains;
  kinematic_chains.push_back(kinematicChain);

  init(kinematic_chains, configFileName);
  return;
}

cedar::dev::robot::gui::KinematicChainWidget::KinematicChainWidget
(
  const std::vector<cedar::dev::robot::KinematicChainPtr> &kinematicChains,
  const std::string& configFileName,
  QWidget *parent,
  Qt::WindowFlags
)
:
QWidget(parent),
ConfigurationInterface(configFileName)
{
  init(kinematicChains, configFileName);
  return;
}


cedar::dev::robot::gui::KinematicChainWidget::~KinematicChainWidget()
{

}


//------------------------------------------------------------------------------
// methods
//------------------------------------------------------------------------------

void cedar::dev::robot::gui::KinematicChainWidget::init(const std::vector<cedar::dev::robot::KinematicChainPtr> &kinematicChains, const std::string& configFileName)
{
  setWindowTitle(QApplication::translate("KinematicChainWindow", "KinematicChain"));

  QGridLayout* p_layout = new QGridLayout();
  QGroupBox* p_monitor_group = new QGroupBox();
  p_monitor_group->setTitle("Monitor");
  p_layout->addWidget(p_monitor_group, 0, 0);
  cedar::dev::robot::gui::KinematicChainMonitorWidget* monitor_widget = new cedar::dev::robot::gui::KinematicChainMonitorWidget(kinematicChains, configFileName);
  p_layout->addWidget(monitor_widget, 0, 0);
  QGroupBox* p_command_group = new QGroupBox();
  p_command_group->setTitle("Command");
  p_layout->addWidget(p_command_group, 0, 1);
  cedar::dev::robot::gui::KinematicChainCommandWidget* command_widget = new cedar::dev::robot::gui::KinematicChainCommandWidget(kinematicChains, configFileName);
  p_layout->addWidget(command_widget, 0, 1);
  this->setLayout(p_layout);

  this->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
}
