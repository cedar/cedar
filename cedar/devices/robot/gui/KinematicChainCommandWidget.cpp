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

    File:        KinematicChainCommandWidget.cpp

    Maintainer:  Hendrik Reimann
    Email:       hendrik.reimann@ini.rub.de
    Date:        2012 02 01

    Description: Implementation of the @em cedar::dev::robot::KinematicChainCommandWidget class.

    Credits:

======================================================================================================================*/

// CEDAR INCLUDES
#include "cedar/devices/robot/gui/KinematicChainCommandWidget.h"
#include "cedar/auxiliaries/exceptions.h"

// SYSTEM INCLUDES
#include "stdio.h"
#include <iostream>
#include <QtGui/QApplication>
#include <QtGui/QDoubleSpinBox>
#include <QtGui/QLabel>
#include <QtGui/QRadioButton>
#include <QtGui/QPushButton>

//----------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------

cedar::dev::robot::gui::KinematicChainCommandWidget::KinematicChainCommandWidget
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
  mSingleStep = 0.01;

  initWindow();
  return;
}


cedar::dev::robot::gui::KinematicChainCommandWidget::KinematicChainCommandWidget
(
  const cedar::dev::robot::KinematicChainPtr &kinematicChain,
  const std::string& configFileName,
  QWidget* parent,
  Qt::WindowFlags
)
:
QWidget(parent),
cedar::aux::ConfigurationInterface(configFileName)
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
    std::cout << "KinematicChainCommandWidget: Error reading 'kinematicChainWidgetDecimals' from config file!" << std::endl;
  }

  if(addParameter(&mSingleStep, "kinematicChainWidgetSingleStep", 0.01) != CONFIG_SUCCESS)
  {
    std::cout << "KinematicChainCommandWidget: Error reading 'kinematicChainWidgetSingleStep' from config file!" << std::endl;
  }

  readOrDefaultConfiguration();

  initWindow();
  return;
}


cedar::dev::robot::gui::KinematicChainCommandWidget::KinematicChainCommandWidget
(
  const std::vector<cedar::dev::robot::KinematicChainPtr> &kinematicChains,
  QWidget* parent,
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
      std::cout << "KinematicChainCommandWidget: Error, kinematic chains do not have the same number of joints!" << std::endl;
      CEDAR_THROW(cedar::aux::InitializationException, "Kinematic chains do not have the same number of joints!");
    }
  }

  // store smart pointers to KinematicChains
  mpKinematicChains = kinematicChains;
  mDecimals = 2;
  mSingleStep = 0.01;

  initWindow();
  return;
}


cedar::dev::robot::gui::KinematicChainCommandWidget::KinematicChainCommandWidget
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
      std::cout << "KinematicChainCommandWidget: Error, kinematic chains do not have the same number of joints!" << std::endl;
      CEDAR_THROW(cedar::aux::InitializationException, "Kinematic chains do not have the same number of joints!");
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
    std::cout << "KinematicChainCommandWidget: Error reading 'kinematicChainWidgetDecimals' from config file!" << std::endl;
  }

  if(addParameter(&mSingleStep, "kinematicChainWidgetSingleStep", 0.01) != CONFIG_SUCCESS)
  {
    std::cout << "KinematicChainCommandWidget: Error reading 'kinematicChainWidgetSingleStep' from config file!" << std::endl;
  }

  readOrDefaultConfiguration();

  initWindow();
  return;
}


cedar::dev::robot::gui::KinematicChainCommandWidget::~KinematicChainCommandWidget()
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

void cedar::dev::robot::gui::KinematicChainCommandWidget::timerEvent(QTimerEvent*)
{
  update();
}

void cedar::dev::robot::gui::KinematicChainCommandWidget::changeWorkingMode(int mode)
{
  for(unsigned int i = 0; i < mpKinematicChains.size(); ++i)
  {
    mpKinematicChains[i]->setWorkingMode(static_cast<cedar::dev::robot::KinematicChain::ActionType>(mode));
  }
}

//void cedar::dev::robot::gui::KinematicChainCommandWidget::radioButtonAngleClicked()
//{
//  for(unsigned int i = 0; i < mpKinematicChains.size(); ++i)
//  {
//    mpKinematicChains[i]->stop();
//    mpKinematicChains[i]->setWorkingMode(KinematicChain::ANGLE);
//  }
//}
//
//
//void cedar::dev::robot::gui::KinematicChainCommandWidget::radioButtonVelocityClicked()
//{
//  for(unsigned int i = 0; i < mpKinematicChains.size(); ++i)
//  {
//    mpKinematicChains[i]->stop();
//    mpKinematicChains[i]->setWorkingMode(KinematicChain::VELOCITY);
//
//    // here we check if velocity has to be integrated -- HR: shouldn't this be checked when setting the working mode?
//    if(!mpKinematicChains[i]->setJointVelocity(0, 0.0))
//    {
//      mpKinematicChains[i]->start();
//    }
//  }
//}
//
//
//void cedar::dev::robot::gui::KinematicChainCommandWidget::radioButtonAccelerationClicked()
//{
//  for(unsigned int i = 0; i < mpKinematicChains.size(); ++i)
//  {
//    mpKinematicChains[i]->stop();
//    mpKinematicChains[i]->setWorkingMode(KinematicChain::ACCELERATION);
//
//    // here we check if acceleration has to be integrated
//    if(!mpKinematicChains[i]->setJointAcceleration(0, 0.0))
//    {
//      mpKinematicChains[i]->start();
//    }
//  }
//}



//void cedar::dev::robot::gui::KinematicChainCommandWidget::commandJoint()
//{
//  QDoubleSpinBox *sender = static_cast<QDoubleSpinBox*>(this->sender());
//  int index_sender = mpGridLayout->indexOf(sender);
//
//  int row, column;
//  int joint, mode;
//  int dummy1, dummy2;
//
//  mpGridLayout->getItemPosition(index_sender, &row, &column, &dummy1, &dummy2);
//  joint = row - 1;
//  mode = column - 1;
//
//  double value = sender->value();
//
//  switch(mode)
//  {
//  case 0:
//    for(unsigned int i = 0; i < mpKinematicChains.size(); ++i)
//    {
//      mpKinematicChains[i]->setJointAngle(joint, value);
//    }
//    break;
//
//  case 1:
//    for(unsigned int i = 0; i < mpKinematicChains.size(); ++i)
//    {
//      mpKinematicChains[i]->setJointVelocity(joint, value);
//    }
//    break;
//
//  case 2:
//    for(unsigned int i = 0; i < mpKinematicChains.size(); ++i)
//    {
//      mpKinematicChains[i]->setJointAcceleration(joint, value);
//    }
//    break;
//
//  default:
//    std::cout << "Error: I was not able to determine the corresponding working mode for this signal. "
//              << "This should never happen!" << std::endl;
//  }
//
//  return;
//}
void cedar::dev::robot::gui::KinematicChainCommandWidget::commandJoints()
{
  std::vector<double> command_vector;
  for(unsigned int i = 0; i < mpKinematicChains[0]->getNumberOfJoints(); ++i)
  {
    command_vector.push_back(mCommandBoxes[i]->value());
  }

  switch(mpModeBox->currentIndex())
  {
  case 0:
    for(unsigned int i = 0; i < mpKinematicChains.size(); ++i)
    {
      mpKinematicChains[i]->setJointAngles(command_vector);
    }
    break;
  case 1:
    for(unsigned int i = 0; i < mpKinematicChains.size(); ++i)
    {
      mpKinematicChains[i]->setJointVelocities(command_vector);
    }
    break;

  case 2:
    for(unsigned int i = 0; i < mpKinematicChains.size(); ++i)
    {
      mpKinematicChains[i]->setJointAccelerations(command_vector);
    }
    break;
  }
}

void cedar::dev::robot::gui::KinematicChainCommandWidget::initWindow()
{
  setWindowTitle(QApplication::translate("KinematicChainWindow", "Command"));

  mpGridLayout = new QGridLayout();

  // mode selection
  QLabel* mode_label = new QLabel(QApplication::translate("KinematicChainWindow", "mode:"));
  mode_label->setAlignment(Qt::AlignRight);
  mpGridLayout->addWidget(mode_label, 0, 0);
  mpModeBox = new QComboBox();
  mpModeBox->addItem(QString("pos"));
  mpModeBox->addItem(QString("vel"));
  mpModeBox->addItem(QString("acc"));
  mpModeBox->setCurrentIndex(mpKinematicChains[0]->getWorkingMode());
  mpGridLayout->addWidget(mpModeBox, 0, 1);
  connect(mpModeBox, SIGNAL(currentIndexChanged(int)), this, SLOT(changeWorkingMode(int)));

  for(unsigned int i = 0; i < mpKinematicChains[0]->getNumberOfJoints(); ++i)
  {
    // add label
    char labelText[10];
    sprintf(labelText, "Joint %d", i);
    QLabel *label = new QLabel(QApplication::translate("KinematicChainWindow", labelText));
    mpGridLayout->addWidget(label, i+1, 0);

    // add spinboxes
    QDoubleSpinBox* command_box = new QDoubleSpinBox();
    command_box->setRange(-999999.0, 999999.0);
    command_box->setValue(0.0);
    command_box->setDecimals(mDecimals);
    command_box->setSingleStep(mSingleStep);
    mpGridLayout->addWidget(command_box, i+1, 1);
    connect(command_box, SIGNAL(editingFinished(void)), this, SLOT(commandJoints(void)));
    mCommandBoxes.push_back(command_box);
  }

  QPushButton* move_button = new QPushButton(QApplication::translate("KinematicChainWindow", "move"));
  mpGridLayout->addWidget(move_button, mpKinematicChains[0]->getNumberOfJoints()+1, 0);
  connect(move_button, SIGNAL(pressed()), this, SLOT(commandJoints()));

  mpGridLayout->setColumnStretch(0,1);
  mpGridLayout->setColumnStretch(1,2);
  setLayout(mpGridLayout);
  setMaximumHeight(0);
  startTimer(mUpdateInterval);

  // todo: add a "command continuously" checkbox
  // todo: add a "set to measured state" button -- or maybe it's sufficient to do this when the mode is changed
  return;
}
