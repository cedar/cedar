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

    File:        RigidBodyWidget.cpp

    Maintainer:  Hendrik Reimann
    Email:       hendrik.reimann@ini.rub.de
    Date:        2012 02 01

    Description: Implementation of the @em cedar::aux::gui::RigidBodyWidget class.

    Credits:

======================================================================================================================*/

// CEDAR INCLUDES
#include "cedar/auxiliaries/gui/RigidBodyWidget.h"
#include "cedar/auxiliaries/exceptions.h"
#include "cedar/auxiliaries/math/tools.h"

// SYSTEM INCLUDES
#include <stdio.h>
#include <iostream>
#include <QtGui/QApplication>
#include <QtGui/QLabel>
#include <QtGui/QPushButton>
#include <QtGui/QDoubleSpinBox>

//----------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------

cedar::aux::gui::RigidBodyWidget::RigidBodyWidget
(
  const cedar::aux::RigidBodyPtr &rigidBody,
  QWidget* parent
)
:
QWidget(parent),
mpRigidBody(rigidBody)
{
  mDecimals = 2;
  mRotationInterval = 10;
  mSinglePositionStep = 0.1;
  mSingleRotationStep = 0.02;
  mXMin = -10.0;
  mXMax = 10.0;
  mYMin = -10.0;
  mYMax = 10.0;
  mZMin = -10.0;
  mZMax = 10.0;

  initWindow();
  return;
}


cedar::aux::gui::RigidBodyWidget::RigidBodyWidget
(
  const cedar::aux::RigidBodyPtr &rigidBody,
  const std::string& configFileName,
  QWidget* parent
)
:
QWidget(parent),
cedar::aux::ConfigurationInterface(configFileName),
mpRigidBody(rigidBody)
{
  // todo: make these configurable
  mDecimals = 2;
  mRotationInterval = 10;
  mSinglePositionStep = 0.1;
  mSingleRotationStep = 0.02;
  mXMin = -10.0;
  mXMax = 10.0;
  mYMin = -10.0;
  mYMax = 10.0;
  mZMin = -10.0;
  mZMax = 10.0;

  //
  // read configuration file
  //

  if(addParameter(&mDecimals, "kinematicChainWidgetDecimals", 2) != CONFIG_SUCCESS)
  {
    std::cout << "RigidBodyWidget: Error reading 'kinematicChainWidgetDecimals' from config file!" << std::endl;
  }

  readOrDefaultConfiguration();

  initWindow();
  return;
}

cedar::aux::gui::RigidBodyWidget::~RigidBodyWidget()
{

}


//------------------------------------------------------------------------------
// methods
//------------------------------------------------------------------------------

void cedar::aux::gui::RigidBodyWidget::timerEvent(QTimerEvent*)
{
  update();
}

void cedar::aux::gui::RigidBodyWidget::setRigidBody(cedar::aux::RigidBodyPtr pRigidBody)
{
  mpRigidBody = pRigidBody;
}

void cedar::aux::gui::RigidBodyWidget::initWindow()
{
  setWindowTitle(QApplication::translate("RigidBodyWidget", "Rigid Body"));

  mpGridLayout = new QGridLayout();

  // add labels
  QLabel* label;
  label = new QLabel(QString("x"));
  label->setAlignment(Qt::AlignCenter);
  mpGridLayout->addWidget(label, 0, 1, 1, 2);
  label = new QLabel(QString("y"));
  label->setAlignment(Qt::AlignCenter);
  mpGridLayout->addWidget(label, 0, 3, 1, 2);
  label = new QLabel(QString("z"));
  label->setAlignment(Qt::AlignCenter);
  mpGridLayout->addWidget(label, 0, 5, 1, 2);
  label = new QLabel(QString("pos"));
  label->setAlignment(Qt::AlignRight);
  mpGridLayout->addWidget(label, 1, 0);
  label = new QLabel(QString("rot"));
  label->setAlignment(Qt::AlignRight);
  mpGridLayout->addWidget(label, 2, 0);
  label = new QLabel(QString("R"));
  label->setAlignment(Qt::AlignRight);
  mpGridLayout->addWidget(label, 4, 0);

  // add position spin boxes
  mpPositionXSpinBox = new QDoubleSpinBox();
  mpPositionXSpinBox->setRange(mXMin, mXMax);
  mpPositionXSpinBox->setValue(mpRigidBody->getPositionX());
  mpPositionXSpinBox->setDecimals(mDecimals);
  mpPositionXSpinBox->setSingleStep(mSinglePositionStep);
  connect(mpPositionXSpinBox, SIGNAL(valueChanged(double)), this, SLOT(positionChanged(double)));
  mpGridLayout->addWidget(mpPositionXSpinBox, 1, 1, 1, 2);

  mpPositionYSpinBox = new QDoubleSpinBox();
  mpPositionYSpinBox->setRange(mYMin, mYMax);
  mpPositionYSpinBox->setValue(mpRigidBody->getPositionY());
  mpPositionYSpinBox->setDecimals(mDecimals);
  mpPositionYSpinBox->setSingleStep(mSinglePositionStep);
  connect(mpPositionYSpinBox, SIGNAL(valueChanged(double)), this, SLOT(positionChanged(double)));
  mpGridLayout->addWidget(mpPositionYSpinBox, 1, 3, 1, 2);

  mpPositionZSpinBox = new QDoubleSpinBox();
  mpPositionZSpinBox->setRange(mZMin, mZMax);
  mpPositionZSpinBox->setValue(mpRigidBody->getPositionZ());
  mpPositionZSpinBox->setDecimals(mDecimals);
  mpPositionZSpinBox->setSingleStep(mSinglePositionStep);
  connect(mpPositionZSpinBox, SIGNAL(valueChanged(double)), this, SLOT(positionChanged(double)));
  mpGridLayout->addWidget(mpPositionZSpinBox, 1, 5, 1, 2);


  // add rotation buttons
  QPushButton* rotation_button;
  rotation_button = new QPushButton(QString("-"));
  rotation_button->setMinimumWidth(0);
  rotation_button->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Preferred);
  rotation_button->setAutoRepeat(true);
  rotation_button->setAutoRepeatInterval(mRotationInterval);
  connect(rotation_button, SIGNAL(pressed()), this, SLOT(rotateXNeg()));
  mpGridLayout->addWidget(rotation_button, 2, 1);

  rotation_button = new QPushButton(QString("+"));
  rotation_button->setMinimumWidth(0);
  rotation_button->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Preferred);
  rotation_button->setAutoRepeat(true);
  rotation_button->setAutoRepeatInterval(mRotationInterval);
  connect(rotation_button, SIGNAL(pressed()), this, SLOT(rotateXPos()));
  mpGridLayout->addWidget(rotation_button, 2, 2);

  rotation_button = new QPushButton(QString("-"));
  rotation_button->setMinimumWidth(0);
  rotation_button->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Preferred);
  rotation_button->setAutoRepeat(true);
  rotation_button->setAutoRepeatInterval(mRotationInterval);
  connect(rotation_button, SIGNAL(pressed()), this, SLOT(rotateYNeg()));
  mpGridLayout->addWidget(rotation_button, 2, 3);

  rotation_button = new QPushButton(QString("+"));
  rotation_button->setMinimumWidth(0);
  rotation_button->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Preferred);
  rotation_button->setAutoRepeat(true);
  rotation_button->setAutoRepeatInterval(mRotationInterval);
  connect(rotation_button, SIGNAL(pressed()), this, SLOT(rotateYPos()));
  mpGridLayout->addWidget(rotation_button, 2, 4);

  rotation_button = new QPushButton(QString("-"));
  rotation_button->setMinimumWidth(0);
  rotation_button->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Preferred);
  rotation_button->setAutoRepeat(true);
  rotation_button->setAutoRepeatInterval(mRotationInterval);
  connect(rotation_button, SIGNAL(pressed()), this, SLOT(rotateZNeg()));
  mpGridLayout->addWidget(rotation_button, 2, 5);

  rotation_button = new QPushButton(QString("+"));
  rotation_button->setMinimumWidth(0);
  rotation_button->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Preferred);
  rotation_button->setAutoRepeat(true);
  rotation_button->setAutoRepeatInterval(mRotationInterval);
  connect(rotation_button, SIGNAL(pressed()), this, SLOT(rotateZPos()));
  mpGridLayout->addWidget(rotation_button, 2, 6);


  for(unsigned int i = 3; i < 6; i++)
  {
    for(unsigned int j = 1; j < 4; j++)
    {
      // add rotation label
      QLabel* p_label = new QLabel;
      p_label->setAlignment(Qt::AlignCenter);
      p_label->setText("0.00");
      mpGridLayout->addWidget(p_label, i, j*2-1, 1, 2);
    }
  }

  setLayout(mpGridLayout);
  mpGridLayout->setColumnStretch(0,1);
  mpGridLayout->setColumnStretch(1,2);
  mpGridLayout->setColumnStretch(2,2);
  mpGridLayout->setColumnStretch(3,2);
  mpGridLayout->setColumnStretch(4,2);
  mpGridLayout->setColumnStretch(5,2);
  mpGridLayout->setColumnStretch(6,2);

  // start a timer to update the interface
  startTimer(mUpdateInterval);

  return;
}

void cedar::aux::gui::RigidBodyWidget::update()
{
  cv::Mat T = mpRigidBody->getTransformation().clone();

  // update rotation matrix
  for(unsigned int i = 0; i < 3; i++)
  {
    for(unsigned int j = 0; j < 3; j++)
    {
      QLabel* p_label = static_cast<QLabel*>(mpGridLayout->itemAtPosition(i+3, (j+1)*2)->widget());
      double r = T.at<double>(i, j);
      if (IsZero(r))
      {
        r = 0;
      }
      p_label->setText(QString("%1").arg(r, 0, 'g', mDecimals, '0'));
    }
  }

  // update position spin boxes
  mpPositionXSpinBox->blockSignals(true);
  mpPositionYSpinBox->blockSignals(true);
  mpPositionZSpinBox->blockSignals(true);
  mpPositionXSpinBox->setValue(T.at<double>(0, 3));
  mpPositionYSpinBox->setValue(T.at<double>(1, 3));
  mpPositionZSpinBox->setValue(T.at<double>(2, 3));
  mpPositionXSpinBox->blockSignals(false);
  mpPositionYSpinBox->blockSignals(false);
  mpPositionZSpinBox->blockSignals(false);
}

void cedar::aux::gui::RigidBodyWidget::positionChanged(double)
{
  mpRigidBody->setPosition(
                            mpPositionXSpinBox->value(),
                            mpPositionYSpinBox->value(),
                            mpPositionZSpinBox->value()
                          );
}

void cedar::aux::gui::RigidBodyWidget::rotateXPos()
{
  mpRigidBody->rotate(0, mSingleRotationStep);
}

void cedar::aux::gui::RigidBodyWidget::rotateXNeg()
{
  mpRigidBody->rotate(0, -mSingleRotationStep);
}

void cedar::aux::gui::RigidBodyWidget::rotateYPos()
{
  mpRigidBody->rotate(1, mSingleRotationStep);
}

void cedar::aux::gui::RigidBodyWidget::rotateYNeg()
{
  mpRigidBody->rotate(1, -mSingleRotationStep);
}

void cedar::aux::gui::RigidBodyWidget::rotateZPos()
{
  mpRigidBody->rotate(2, mSingleRotationStep);
}

void cedar::aux::gui::RigidBodyWidget::rotateZNeg()
{
  mpRigidBody->rotate(2, -mSingleRotationStep);
}

