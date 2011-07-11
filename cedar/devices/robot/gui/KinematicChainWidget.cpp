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


// LOCAL INCLUDES

#include "devices/robot/gui/KinematicChainWidget.h"

// PROJECT INCLUDES
#include <auxiliaries/exceptions/InitializationException.h>

// SYSTEM INCLUDES

#include "stdio.h"
#include <iostream>

#include <QtGui/QApplication>
#include <QtGui/QDoubleSpinBox>
#include <QtGui/QLabel>
#include <QtGui/QRadioButton>

using namespace std;
using namespace cedar::dev::robot;


//----------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------

KinematicChainWidget::KinematicChainWidget(const KinematicChainPtr &kinematicChain, QWidget * parent, Qt::WindowFlags f) : QWidget(parent, f)
{
  // store a smart pointer to KinematicChain
  mpKinematicChains.push_back(kinematicChain);
  mDecimals = 2;
  mSingleStep = 0.01;

  initWindow();
  return;
}


KinematicChainWidget::KinematicChainWidget(const cedar::dev::robot::KinematicChainPtr &kinematicChain, const std::string& configFileName, QWidget *parent, Qt::WindowFlags /* f */)
:
QWidget(parent),
ConfigurationInterface(configFileName)
{
  // store a smart pointer to KinematicChain
  mpKinematicChains.push_back(kinematicChain);
  mDecimals = 2;
  mSingleStep = 0.01;

  //
  // read configuration file
  //

  if(addParameter(&mDecimals, "kinematicChainWidgetDecimals", 2) != CONFIG_SUCCESS)
  {
    cout << "KinematicChainWidget: Error reading 'kinematicChainWidgetDecimals' from config file!" << endl;
  }

  if(addParameter(&mSingleStep, "kinematicChainWidgetSingleStep", 0.01) != CONFIG_SUCCESS)
  {
    cout << "KinematicChainWidget: Error reading 'kinematicChainWidgetSingleStep' from config file!" << endl;
  }

  readOrDefaultConfiguration();

  initWindow();
  return;
}


KinematicChainWidget::KinematicChainWidget(const std::vector<KinematicChainPtr> &kinematicChains, QWidget * parent, Qt::WindowFlags f) : QWidget(parent, f)
{
  for(unsigned int i = 1; i < kinematicChains.size(); ++i)
  {
    // ideally, the reference geometry behind all the chains should be the same
    // here, at least we make sure that the number of joints is the same
    if(kinematicChains[i]->getNumberOfJoints() != kinematicChains[0]->getNumberOfJoints())
    {
      cout << "KinematicChainWidget: Error, kinematic chains do not have the same number of joints!" << endl;
      CEDAR_THROW(cedar::aux::exc::InitializationException, "Kinematic chains do not have the same number of joints!");
    }
  }

  // store smart pointers to KinematicChains
  mpKinematicChains = kinematicChains;
  mDecimals = 2;
  mSingleStep = 0.01;

  initWindow();
  return;
}


KinematicChainWidget::KinematicChainWidget(const std::vector<KinematicChainPtr> &kinematicChains, const std::string& configFileName, QWidget *parent, Qt::WindowFlags /* f */)
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
      cout << "KinematicChainWidget: Error, kinematic chains do not have the same number of joints!" << endl;
      CEDAR_THROW(cedar::aux::exc::InitializationException, "Kinematic chains do not have the same number of joints!");
    }
  }

  // store a smart pointer to KinematicChain
  mpKinematicChains = kinematicChains;
  mDecimals = 2;
  mSingleStep = 0.01;

  //
  // read configuration file
  //

  if(addParameter(&mDecimals, "kinematicChainWidgetDecimals", 2) != CONFIG_SUCCESS)
  {
    cout << "KinematicChainWidget: Error reading 'kinematicChainWidgetDecimals' from config file!" << endl;
  }

  if(addParameter(&mSingleStep, "kinematicChainWidgetSingleStep", 0.01) != CONFIG_SUCCESS)
  {
    cout << "KinematicChainWidget: Error reading 'kinematicChainWidgetSingleStep' from config file!" << endl;
  }

  readOrDefaultConfiguration();

  initWindow();
  return;
}


KinematicChainWidget::~KinematicChainWidget()
{
  for(unsigned int i = 0; i < mpKinematicChains.size(); ++i)
  {
    mpKinematicChains[i]->stop();
  }
  mpTimer->stop();
  delete mpTimer;
}


//------------------------------------------------------------------------------
// methods
//------------------------------------------------------------------------------

void KinematicChainWidget::setActiveColumn(unsigned int c)
{
  for(unsigned int i = 0; i < 3; ++i)
  {
    bool enabled = (i == c) ? true : false;
    for(unsigned int j = 0; j < mpKinematicChains[0]->getNumberOfJoints(); ++j)
    {
      mpGridLayout->itemAtPosition(j+1, i+1)->widget()->setEnabled(enabled);
    }
  }
}


void KinematicChainWidget::radioButtonAngleClicked()
{
  setActiveColumn(0);
  for(unsigned int i = 0; i < mpKinematicChains.size(); ++i)
  {
    mpKinematicChains[i]->stop();
    mpKinematicChains[i]->setWorkingMode(KinematicChain::ANGLE);
  }
  updateSpinBoxes();
}


void KinematicChainWidget::radioButtonVelocityClicked()
{
  setActiveColumn(1);

  for(unsigned int i = 0; i < mpKinematicChains.size(); ++i)
  {
    mpKinematicChains[i]->stop();
    mpKinematicChains[i]->setWorkingMode(KinematicChain::VELOCITY);

    // here we check if velocity has to be integrated
    if(!mpKinematicChains[i]->setJointVelocity(0, 0.0))
    {
      mpKinematicChains[i]->start();
    }
  }

  updateSpinBoxes();
}


void KinematicChainWidget::radioButtonAccelerationClicked()
{
  setActiveColumn(2);

  for(unsigned int i = 0; i < mpKinematicChains.size(); ++i)
  {
    mpKinematicChains[i]->stop();
    mpKinematicChains[i]->setWorkingMode(KinematicChain::ACCELERATION);

    // here we check if acceleration has to be integrated
    if(!mpKinematicChains[i]->setJointAcceleration(0, 0.0))
    {
      mpKinematicChains[i]->start();
    }
  }

  updateSpinBoxes();
}


void KinematicChainWidget::updateSpinBoxes()
{
  for(unsigned i = 0; i < mpKinematicChains[0]->getNumberOfJoints(); ++i)
  {
    QDoubleSpinBox *p_spin_box_angle = static_cast<QDoubleSpinBox*>(mpGridLayout->itemAtPosition(i+1, 1)->widget());
    QDoubleSpinBox *p_spin_box_velocity = static_cast<QDoubleSpinBox*>(mpGridLayout->itemAtPosition(i+1, 2)->widget());
    QDoubleSpinBox *p_spin_box_acceleration = static_cast<QDoubleSpinBox*>(mpGridLayout->itemAtPosition(i+1, 3)->widget());

    if(!p_spin_box_angle->hasFocus())
    {
      p_spin_box_angle->setValue(mpKinematicChains[0]->getJointAngle(i));
    }

    if(!p_spin_box_velocity->hasFocus())
    {
      p_spin_box_velocity->setValue(mpKinematicChains[0]->getJointVelocity(i));
    }

    if(!p_spin_box_acceleration->hasFocus())
    {
      p_spin_box_acceleration->setValue(mpKinematicChains[0]->getJointAcceleration(i));
    }
  }
}


void KinematicChainWidget::updateJointValue()
{
  QDoubleSpinBox *sender = static_cast<QDoubleSpinBox*>(this->sender());
  int index_sender = mpGridLayout->indexOf(sender);

  int row, column;
  int joint, mode;
  int dummy1, dummy2;

  mpGridLayout->getItemPosition(index_sender, &row, &column, &dummy1, &dummy2);
  joint = row - 1;
  mode = column - 1;

  double value = sender->value();

  switch(mode)
  {
  case 0:
    for(unsigned int i = 0; i < mpKinematicChains.size(); ++i)
    {
      mpKinematicChains[i]->setJointAngle(joint, value);
    }
    break;

  case 1:
    for(unsigned int i = 0; i < mpKinematicChains.size(); ++i)
    {
      mpKinematicChains[i]->setJointVelocity(joint, value);
    }
    break;

  case 2:
    for(unsigned int i = 0; i < mpKinematicChains.size(); ++i)
    {
      mpKinematicChains[i]->setJointAcceleration(joint, value);
    }
    break;

  default:
    cout << "Error: I was not able to determine the corresponding working mode for this signal. This should never happen!" << endl;
  }

  return;
}


void KinematicChainWidget::initWindow()
{
  setWindowTitle(QApplication::translate("KinematicChainWindow", "KinematicChain"));

  mpGridLayout = new QGridLayout();
  QRadioButton *radioButtonAngle = new QRadioButton(QApplication::translate("KinematicChainWindow", "Angle [rad]"));
  QRadioButton *radioButtonVelocity = new QRadioButton(QApplication::translate("KinematicChainWindow", "Velocity [rad/s]"));
  QRadioButton *radioButtonAcceleration = new QRadioButton(QApplication::translate("KinematicChainWindow", "Acceleration [rad/s^2]"));
  mpGridLayout->addWidget(radioButtonAngle, 0, 1);
  mpGridLayout->addWidget(radioButtonVelocity, 0, 2);
  mpGridLayout->addWidget(radioButtonAcceleration, 0, 3);
  connect(radioButtonAngle, SIGNAL(clicked()), this, SLOT(radioButtonAngleClicked()));
  connect(radioButtonVelocity, SIGNAL(clicked()), this, SLOT(radioButtonVelocityClicked()));
  connect(radioButtonAcceleration, SIGNAL(clicked()), this, SLOT(radioButtonAccelerationClicked()));

  mpGridLayout->setColumnStretch(0,1);
  mpGridLayout->setColumnStretch(1,2);
  mpGridLayout->setColumnStretch(2,2);
  mpGridLayout->setColumnStretch(3,2);

  for(unsigned int i = 0; i < mpKinematicChains[0]->getNumberOfJoints(); ++i)
  {
    // add label
    char labelText[10];
    sprintf(labelText, "Joint %d", i);
    QLabel *label = new QLabel(QApplication::translate("KinematicChainWindow", labelText));
    mpGridLayout->addWidget(label, i+1, 0);

    // add spinboxes
    for(unsigned int j = 0; j < 3; ++j)
    {
      QDoubleSpinBox *doubleSpinBox = new QDoubleSpinBox();
      doubleSpinBox->setRange(-999999.0, 999999.0);
      doubleSpinBox->setValue(0.0);
      doubleSpinBox->setDecimals(mDecimals);
      doubleSpinBox->setSingleStep(mSingleStep);
      mpGridLayout->addWidget(doubleSpinBox, i+1, j+1);
      connect(doubleSpinBox, SIGNAL(editingFinished(void)), this, SLOT(updateJointValue(void)));
    }
  }

  radioButtonAngle->click();
  setLayout(mpGridLayout);
  setMaximumHeight(0);

  // start a timer to update the interface
  mpTimer = new QTimer();
  connect(mpTimer, SIGNAL(timeout()), this, SLOT(updateSpinBoxes()));
  mpTimer->start(mUpdateInterval);

  return;
}
