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

    File:        KinematicChainWidget.cpp

    Maintainer:  Bjoern Weghenkel
    Email:       bjoern.weghenkel@ini.rub.de
    Date:        2011 01 06

    Description: Implementation of the @em cedar::dev::KinematicChainWidget class.

    Credits:

======================================================================================================================*/

// CEDAR INCLUDES
#include "cedar/devices/gui/KinematicChainWidget.h"
#include "cedar/auxiliaries/exceptions.h"
#include "cedar/devices/gui/KinematicChainMonitorWidget.h"
#include "cedar/devices/gui/KinematicChainCommandWidget.h"
#include "cedar/devices/ComponentSlot.h"

// SYSTEM INCLUDES
#include <iostream>
#include <QApplication>
#include <QGridLayout>
#include <QGroupBox>
#include <QPushButton>

//----------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------

cedar::dev::gui::KinematicChainWidget::KinematicChainWidget
(cedar::dev::KinematicChainPtr kinematicChain,
  QWidget *parent,
  Qt::WindowFlags
)
:
QWidget(parent)
{
  init(kinematicChain);
  return;
}

cedar::dev::gui::KinematicChainWidget::~KinematicChainWidget()
{

}


//------------------------------------------------------------------------------
// methods
//------------------------------------------------------------------------------

void cedar::dev::gui::KinematicChainWidget::init(cedar::dev::KinematicChainPtr kinematicChain)
{
  setWindowTitle(QString::fromStdString(kinematicChain->getName()));

  QGridLayout* p_layout = new QGridLayout();
  QGroupBox* p_monitor_group = new QGroupBox();
  p_layout->addWidget(p_monitor_group, 0, 0);
  mpMonitorWidget = new cedar::dev::gui::KinematicChainMonitorWidget(kinematicChain, this);
  p_layout->addWidget(mpMonitorWidget, 0, 0);
  QGroupBox* p_command_group = new QGroupBox();
  p_layout->addWidget(p_command_group, 0, 1);
  mpCommandWidget = new cedar::dev::gui::KinematicChainCommandWidget(kinematicChain);
  p_layout->addWidget(mpCommandWidget, 0, 1);

  QPushButton* close_button = new QPushButton(QApplication::translate("KinematicChainWindow", "close"));
  close_button->setMaximumWidth(75);
  QObject::connect(close_button, SIGNAL(pressed()), this, SLOT(close()));
  p_layout->addWidget(close_button, 1, 1);

  this->setLayout(p_layout);

  //this->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
  this->setFixedHeight(375);

  mpKinematicChain = kinematicChain;
}

std::string cedar::dev::gui::KinematicChainWidget::getPath() const
{
  return mpKinematicChain->getSlot().lock()->getPath();
}

cedar::dev::gui::KinematicChainMonitorWidget* cedar::dev::gui::KinematicChainWidget::getMonitorWidget()
{
  return mpMonitorWidget;
}

cedar::dev::gui::KinematicChainCommandWidget* cedar::dev::gui::KinematicChainWidget::getCommandWidget()
{
  return mpCommandWidget;
}
