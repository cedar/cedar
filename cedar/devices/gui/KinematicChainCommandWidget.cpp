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

    File:        KinematicChainCommandWidget.cpp

    Maintainer:  Hendrik Reimann
    Email:       hendrik.reimann@ini.rub.de
    Date:        2012 02 01

    Description: Implementation of the @em cedar::dev::KinematicChainCommandWidget class.

    Credits:

======================================================================================================================*/

// CEDAR INCLUDES
#include "cedar/devices/gui/KinematicChainCommandWidget.h"
#include "cedar/auxiliaries/assert.h"
#include "cedar/auxiliaries/exceptions.h"

// SYSTEM INCLUDES
#include <QApplication>
#include <QDoubleSpinBox>
#include <QLabel>
#include <QRadioButton>
#include <QPushButton>

//----------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------

cedar::dev::gui::KinematicChainCommandWidget::KinematicChainCommandWidget
(
  cedar::dev::KinematicChainPtr kinematicChain,
  QWidget * parent,
  Qt::WindowFlags f
)
:
QWidget(parent, f),
mpKinematicChain(kinematicChain),
mTimerId(0)
{
  initWindow();
}

cedar::dev::gui::KinematicChainCommandWidget::~KinematicChainCommandWidget()
{

}


//------------------------------------------------------------------------------
// methods
//------------------------------------------------------------------------------

void cedar::dev::gui::KinematicChainCommandWidget::timerEvent(QTimerEvent*)
{
  commandJoints();
}

void cedar::dev::gui::KinematicChainCommandWidget::setDecimals(unsigned int decimals)
{
  for(unsigned int j = 0; j < mpKinematicChain->getNumberOfJoints(); ++j)
  {
    mCommandBoxes[j]->setDecimals(decimals);
  }
}

void cedar::dev::gui::KinematicChainCommandWidget::setSingleStep(double singleStep)
{
  for(unsigned int j = 0; j < mpKinematicChain->getNumberOfJoints(); ++j)
  {
    mCommandBoxes[j]->setSingleStep(singleStep);
  }
}

void cedar::dev::gui::KinematicChainCommandWidget::changeWorkingMode(int mode)
{
  // we have to reset the used command in here
  mpModeBox->setCurrentIndex(mode);
  mpKinematicChain->clearUserCommand();
  update();
}

void cedar::dev::gui::KinematicChainCommandWidget::commandJoints()
{
  std::vector<double> command_vector;
  for(unsigned int i = 0; i < mpKinematicChain->getNumberOfJoints(); ++i)
  {
    command_vector.push_back(mCommandBoxes[i]->value());
  }

  switch(mpModeBox->currentIndex())
  {
  case 0:
    mpKinematicChain->setJointAngles(command_vector);
    break;
  case 1:
    mpKinematicChain->setJointVelocities(command_vector);
    break;
  case 2:
    mpKinematicChain->setJointAccelerations(command_vector);
    break;
  default:
    CEDAR_THROW(cedar::aux::UnhandledValueException, "This value is not handled here.");
  }
}

void cedar::dev::gui::KinematicChainCommandWidget::stopMovement()
{
  mpKeepMovingBox->setChecked(false);
  mpKinematicChain->clearUserCommand();
  for (unsigned int j = 0; j < mpKinematicChain->getNumberOfJoints(); ++j)
  {
    mpKinematicChain->setJointVelocity(j, 0);
  }
  update();
  mpKinematicChain->clearUserCommand();
}

void cedar::dev::gui::KinematicChainCommandWidget::update()
{

  // update command boxes
  CEDAR_DEBUG_ASSERT(mpKinematicChain->getNumberOfJoints() == mCommandBoxes.size());
  switch(mpModeBox->currentIndex())
  {
  case 0:
    for (unsigned int j = 0; j < mpKinematicChain->getNumberOfJoints(); ++j)
    {
      mCommandBoxes[j]->blockSignals(true);
      mCommandBoxes[j]->setValue(mpKinematicChain->getJointAngle(j));
      mCommandBoxes[j]->blockSignals(false);
    }
    break;
  case 1:
    for (unsigned int j = 0; j < mpKinematicChain->getNumberOfJoints(); ++j)
    {
      mCommandBoxes[j]->blockSignals(true);
      mCommandBoxes[j]->setValue(mpKinematicChain->getJointVelocity(j));
      mCommandBoxes[j]->blockSignals(false);
    }
    break;

  case 2:
    for (unsigned int j = 0; j < mpKinematicChain->getNumberOfJoints(); ++j)
    {
      mCommandBoxes[j]->blockSignals(true);
      mCommandBoxes[j]->setValue(mpKinematicChain->getJointAcceleration(j));
      mCommandBoxes[j]->blockSignals(false);
    }
  case 3: // STOP
    break;
  default:
    CEDAR_THROW(cedar::aux::UnhandledValueException, "This is not a handled case.");
  }
}

void cedar::dev::gui::KinematicChainCommandWidget::setKeepSendingState(int state)
{
  if (state)
  {
    if (mTimerId)
    {
      killTimer(mTimerId);
    }
    mTimerId = startTimer(mUpdateInterval);
  }
  else
  {
    killTimer(mTimerId);
  }
}

void cedar::dev::gui::KinematicChainCommandWidget::initWindow()
{
  setWindowTitle(QApplication::translate("KinematicChainWindow", "Command"));

  mpGridLayout = new QGridLayout();

  // mode selection
  QLabel* mode_label = new QLabel(QApplication::translate("KinematicChainWindow", "operate on:"));
  mode_label->setAlignment(Qt::AlignLeft);
  mpGridLayout->addWidget(mode_label, 0, 0);
  mpModeBox = new QComboBox();
  mpModeBox->addItem(QString("position"));
  mpModeBox->addItem(QString("velocity"));
  mpModeBox->addItem(QString("acceleration"));

  mpModeBox->setCurrentIndex(0);

  mpGridLayout->addWidget(mpModeBox, 1, 0);
  connect(mpModeBox, SIGNAL(currentIndexChanged(int)), this, SLOT(changeWorkingMode(int)));

  // move button
  QPushButton* move_button = new QPushButton(QApplication::translate("KinematicChainWindow", "send"));
  mpGridLayout->addWidget(move_button, 2, 0);
  connect(move_button, SIGNAL(pressed()), this, SLOT(commandJoints()));

  // copy button
  QPushButton* copy_button = new QPushButton(QApplication::translate("KinematicChainWindow", "copy"));
  mpGridLayout->addWidget(copy_button, 3, 0);
  connect(copy_button, SIGNAL(pressed()), this, SLOT(update()));

  // stop button
  QPushButton* stop_button = new QPushButton(QApplication::translate("KinematicChainWindow", "stop!"));
  mpGridLayout->addWidget(stop_button, 4, 0);
  connect(stop_button, SIGNAL(pressed()), this, SLOT(stopMovement()));

  // keep moving
  mpKeepMovingBox = new QCheckBox(QApplication::translate("KinematicChainWindow", "keep sending"));
  mpGridLayout->addWidget(mpKeepMovingBox, 5, 0);
  connect(mpKeepMovingBox, SIGNAL(stateChanged(int)), this, SLOT(setKeepSendingState(int)));

  for (unsigned int i = 0; i < mpKinematicChain->getNumberOfJoints(); ++i)
  {
    // add label
    char labelText[10];
    sprintf(labelText, "Joint %d", i);
    QLabel *label = new QLabel(QApplication::translate("KinematicChainWindow", labelText));
    mpGridLayout->addWidget(label, i, 1);

    // add spinboxes
    QDoubleSpinBox* command_box = new QDoubleSpinBox();
    command_box->setRange(-999999.0, 999999.0);
    command_box->setValue(0.0);
    command_box->setDecimals(6);
    command_box->setSingleStep(0.01);
    mpGridLayout->addWidget(command_box, i, 2);
    mCommandBoxes.push_back(command_box);
  }
  update();

  mpGridLayout->setColumnStretch(0,1);
  mpGridLayout->setColumnStretch(1,2);
  setLayout(mpGridLayout);
}
