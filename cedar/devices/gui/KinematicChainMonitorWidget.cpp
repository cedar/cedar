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

    File:        KinematicChainMonitorWidget.cpp

    Maintainer:  Hendrik Reimann
    Email:       hendrik.reimann@ini.rub.de
    Date:        2012 02 01

    Description: Implementation of the @em cedar::dev::KinematicChainMonitorWidget class.

    Credits:

======================================================================================================================*/

// CEDAR INCLUDES
#include "cedar/devices/gui/KinematicChainMonitorWidget.h"
#include "cedar/auxiliaries/exceptions.h"
#include "cedar/auxiliaries/stringFunctions.h"

// SYSTEM INCLUDES
#include <QApplication>
#include <QLabel>

//----------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------

cedar::dev::gui::KinematicChainMonitorWidget::KinematicChainMonitorWidget
(
  cedar::dev::KinematicChainPtr kinematicChain,
  QWidget* parent,
  Qt::WindowFlags f
)
:
QWidget(parent, f),
mpKinematicChain(kinematicChain),
mTimerId(0)
{
  mDecimals = 6;

  initWindow();
  return;
}

cedar::dev::gui::KinematicChainMonitorWidget::~KinematicChainMonitorWidget()
{

}

//------------------------------------------------------------------------------
// methods
//------------------------------------------------------------------------------

void cedar::dev::gui::KinematicChainMonitorWidget::timerEvent(QTimerEvent*)
{
  update();
}

void cedar::dev::gui::KinematicChainMonitorWidget::initWindow()
{
  setWindowTitle(QApplication::translate("KinematicChainWindow", "Monitor"));

  mpGridLayout = new QGridLayout();
  mpGridLayout->setColumnStretch(0,1);
  mpGridLayout->setColumnStretch(1,2);
  mpGridLayout->setColumnStretch(2,2);
  mpGridLayout->setColumnStretch(3,2);

  QFont font;
  font.setBold(true);

  QLabel* mon_label = new QLabel(QApplication::translate("KinematicChainWindow", "Monitor"));
  mon_label->setFont(font);
  mpGridLayout->addWidget(mon_label, 0, 0);

  for(unsigned int i = 0; i < mpKinematicChain->getNumberOfJoints(); ++i)
  {
    // add joint label
    char labelText[10];
    sprintf(labelText, "Joint %d", i);
    QLabel *label = new QLabel(QApplication::translate("KinematicChainWindow", labelText));
    mpGridLayout->addWidget(label, i+2, 0);

    // add value labels
    for(unsigned int j = 0; j < 3; ++j)
    {
      QLabel* p_label = new QLabel;
      p_label->setText("0.0");
      mpGridLayout->addWidget(p_label, i+2, j+1);
    }
  }

  // add joint label
  QLabel* pos_label = new QLabel(QApplication::translate("KinematicChainWindow", "pos"));
  pos_label->setAlignment(Qt::AlignRight);
  mpGridLayout->addWidget(pos_label, 1, 1);
  QLabel* vel_label = new QLabel(QApplication::translate("KinematicChainWindow", "vel"));
  vel_label->setAlignment(Qt::AlignRight);
  mpGridLayout->addWidget(vel_label, 1, 2);
  QLabel* acc_label = new QLabel(QApplication::translate("KinematicChainWindow", "acc"));
  acc_label->setAlignment(Qt::AlignRight);
  mpGridLayout->addWidget(acc_label, 1, 3);

  setLayout(mpGridLayout);

  // start a timer to update the interface
  if (!mTimerId)
  {
    mTimerId =  startTimer(mUpdateInterval);
  }

  return;
}

void cedar::dev::gui::KinematicChainMonitorWidget::setDecimals(unsigned int decimals)
{
  mDecimals = decimals;
}

void cedar::dev::gui::KinematicChainMonitorWidget::update()
{
  for(unsigned i = 0; i < mpKinematicChain->getNumberOfJoints(); ++i)
  {
    QLabel* p_angle_label = static_cast<QLabel*>(mpGridLayout->itemAtPosition(i+2, 1)->widget());
    QLabel* p_velocity_label = static_cast<QLabel*>(mpGridLayout->itemAtPosition(i+2, 2)->widget());
    QLabel* p_acceleration_label = static_cast<QLabel*>(mpGridLayout->itemAtPosition(i+2, 3)->widget());

    p_angle_label->setText(QString("%1").arg(mpKinematicChain->getJointAngle(i), 0, 'g', mDecimals, '0'));
    p_velocity_label->setText(QString("%1").arg(mpKinematicChain->getJointVelocity(i), 0, 'g', mDecimals, '0'));
    p_acceleration_label->setText(QString("%1").arg(mpKinematicChain->getJointAcceleration(i), 0, 'g', mDecimals, '0'));
  }
}

