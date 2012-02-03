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

    File:        KinematicChainMonitorWidget.cpp

    Maintainer:  Hendrik Reimann
    Email:       hendrik.reimann@ini.rub.de
    Date:        2012 02 01

    Description: Implementation of the @em cedar::dev::robot::KinematicChainMonitorWidget class.

    Credits:

======================================================================================================================*/

// CEDAR INCLUDES
#include "cedar/devices/robot/gui/KinematicChainMonitorWidget.h"
#include "cedar/auxiliaries/exceptions.h"
#include "cedar/auxiliaries/stringFunctions.h"

// SYSTEM INCLUDES
#include "stdio.h"
#include <iostream>
#include <QtGui/QApplication>
#include <QtGui/QLabel>

//----------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------

cedar::dev::robot::gui::KinematicChainMonitorWidget::KinematicChainMonitorWidget
(
  const cedar::dev::robot::KinematicChainPtr &kinematicChain,
  QWidget * parent,
  Qt::WindowFlags f
)
:
QWidget(parent, f)
{
  // store a smart pointer to KinematicChain
  mpKinematicChains.push_back(kinematicChain);
  mDecimals = 2;

  initWindow();
  return;
}


cedar::dev::robot::gui::KinematicChainMonitorWidget::KinematicChainMonitorWidget
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
  // store a smart pointer to KinematicChain
  mpKinematicChains.push_back(kinematicChain);
  mDecimals = 2;

  //
  // read configuration file
  //

  if(addParameter(&mDecimals, "kinematicChainWidgetDecimals", 2) != CONFIG_SUCCESS)
  {
    std::cout << "KinematicChainMonitorWidget: Error reading 'kinematicChainWidgetDecimals' from config file!" << std::endl;
  }

  readOrDefaultConfiguration();

  initWindow();
  return;
}


cedar::dev::robot::gui::KinematicChainMonitorWidget::KinematicChainMonitorWidget
(
  const std::vector<cedar::dev::robot::KinematicChainPtr> &kinematicChains,
  QWidget * parent,
  Qt::WindowFlags f
)
:
QWidget(parent, f)
{
  for(unsigned int i = 1; i < kinematicChains.size(); ++i)
  {
    // ideally, the reference geometry behind all the chains should be the same
    // here, at least we make sure that the number of joints is the same
    if(kinematicChains[i]->getNumberOfJoints() != kinematicChains[0]->getNumberOfJoints())
    {
      std::cout << "KinematicChainMonitorWidget: Error, kinematic chains do not have the same number of joints!" << std::endl;
      CEDAR_THROW(cedar::aux::InitializationException, "Kinematic chains do not have the same number of joints!");
    }
  }

  // store smart pointers to KinematicChains
  mpKinematicChains = kinematicChains;
  mDecimals = 2;

  initWindow();
  return;
}


cedar::dev::robot::gui::KinematicChainMonitorWidget::KinematicChainMonitorWidget
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
  for(unsigned int i = 1; i < kinematicChains.size(); ++i)
  {
    // ideally, the reference geometry behind all the chains should be the same
    // here, at least we make sure that the number of joints is the same
    if(kinematicChains[i]->getNumberOfJoints() != kinematicChains[0]->getNumberOfJoints())
    {
      std::cout << "KinematicChainMonitorWidget: Error, kinematic chains do not have the same number of joints!" << std::endl;
      CEDAR_THROW(cedar::aux::InitializationException, "Kinematic chains do not have the same number of joints!");
    }
  }

  // store a smart pointer to KinematicChain
  mpKinematicChains = kinematicChains;
  mDecimals = 2;

  //
  // read configuration file
  //

  if(addParameter(&mDecimals, "kinematicChainWidgetDecimals", 2) != CONFIG_SUCCESS)
  {
    std::cout << "KinematicChainMonitorWidget: Error reading 'kinematicChainWidgetDecimals' from config file!" << std::endl;
  }

  readOrDefaultConfiguration();

  initWindow();
  return;
}


cedar::dev::robot::gui::KinematicChainMonitorWidget::~KinematicChainMonitorWidget()
{

}


//------------------------------------------------------------------------------
// methods
//------------------------------------------------------------------------------

void cedar::dev::robot::gui::KinematicChainMonitorWidget::timerEvent(QTimerEvent*)
{
  update();
}

void cedar::dev::robot::gui::KinematicChainMonitorWidget::initWindow()
{
  setWindowTitle(QApplication::translate("KinematicChainWindow", "Monitor"));

  mpGridLayout = new QGridLayout();
  mpGridLayout->setColumnStretch(0,1);
  mpGridLayout->setColumnStretch(1,2);
  mpGridLayout->setColumnStretch(2,2);
  mpGridLayout->setColumnStretch(3,2);

  // add joint label
  QLabel* pos_label = new QLabel(QApplication::translate("KinematicChainWindow", "pos"));
  pos_label->setAlignment(Qt::AlignRight);
  mpGridLayout->addWidget(pos_label, 0, 1);
  QLabel* vel_label = new QLabel(QApplication::translate("KinematicChainWindow", "vel"));
  vel_label->setAlignment(Qt::AlignRight);
  mpGridLayout->addWidget(vel_label, 0, 2);
  QLabel* acc_label = new QLabel(QApplication::translate("KinematicChainWindow", "acc"));
  acc_label->setAlignment(Qt::AlignRight);
  mpGridLayout->addWidget(acc_label, 0, 3);

  for(unsigned int i = 0; i < mpKinematicChains[0]->getNumberOfJoints(); ++i)
  {
    // add joint label
    char labelText[10];
    sprintf(labelText, "Joint %d", i);
    QLabel *label = new QLabel(QApplication::translate("KinematicChainWindow", labelText));
    mpGridLayout->addWidget(label, i+1, 0);

    // add value labels
    for(unsigned int j = 0; j < 3; ++j)
    {
      QLabel* p_label = new QLabel;
      p_label->setAlignment(Qt::AlignRight);
      p_label->setText("0.00");
      mpGridLayout->addWidget(p_label, i+1, j+1);
    }
  }

  setLayout(mpGridLayout);
  setMaximumHeight(0);

  // start a timer to update the interface
  startTimer(mUpdateInterval);

  return;
}

void cedar::dev::robot::gui::KinematicChainMonitorWidget::update()
{
  for(unsigned i = 0; i < mpKinematicChains[0]->getNumberOfJoints(); ++i)
  {
    // TODO: there's a static cast here, remove
    QLabel* p_angle_label = static_cast<QLabel*>(mpGridLayout->itemAtPosition(i+1, 1)->widget());
    QLabel* p_velocity_label = static_cast<QLabel*>(mpGridLayout->itemAtPosition(i+1, 2)->widget());
    QLabel* p_acceleration_label = static_cast<QLabel*>(mpGridLayout->itemAtPosition(i+1, 3)->widget());

    p_angle_label->setText(QString("%1").arg(mpKinematicChains[0]->getJointAngle(i), 0, 'g', mDecimals, '0'));
    p_velocity_label->setText(QString("%1").arg(mpKinematicChains[0]->getJointVelocity(i), 0, 'g', mDecimals, '0'));
    p_acceleration_label->setText(QString("%1").arg(mpKinematicChains[0]->getJointAcceleration(i), 0, 'g', mDecimals, '0'));
  }
}

