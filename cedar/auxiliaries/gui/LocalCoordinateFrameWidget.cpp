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

    File:        LocalCoordinateFrameWidget.cpp

    Maintainer:  Hendrik Reimann
    Email:       hendrik.reimann@ini.rub.de
    Date:        2012 02 01

    Description: Implementation of the @em cedar::aux::gui::LocalCoordinateFrameWidget class.

    Credits:

======================================================================================================================*/

// CEDAR INCLUDES
#include "cedar/auxiliaries/gui/LocalCoordinateFrameWidget.h"
#include "cedar/auxiliaries/exceptions.h"
#include "cedar/auxiliaries/math/tools.h"
#include "cedar/units/Length.h"
#include "cedar/units/PlaneAngle.h"

// SYSTEM INCLUDES
#include <QApplication>
#include <QLabel>
#include <QPushButton>
#include <QDoubleSpinBox>

//----------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------

cedar::aux::gui::LocalCoordinateFrameWidget::LocalCoordinateFrameWidget
(
  const cedar::aux::LocalCoordinateFramePtr localCoordinateFrame,
  QWidget* parent
)
:
QWidget(parent),
mpLocalCoordinateFrame(localCoordinateFrame),
// parameters
_mDecimals(new cedar::aux::UIntParameter(this, "decimals", 2))
{
  mRotationInterval = 10;
  mSingleTranslationStep = 0.1;
  mSingleRotationStep = 0.02;
  mXMin = -10.0;
  mXMax = 10.0;
  mYMin = -10.0;
  mYMax = 10.0;
  mZMin = -10.0;
  mZMax = 10.0;

  initWindow();

  QObject::connect(this->_mDecimals.get(), SIGNAL(valueChanged()), this, SLOT(decimalsChanged()));
}

cedar::aux::gui::LocalCoordinateFrameWidget::~LocalCoordinateFrameWidget()
{

}


//------------------------------------------------------------------------------
// methods
//------------------------------------------------------------------------------

void cedar::aux::gui::LocalCoordinateFrameWidget::timerEvent(QTimerEvent*)
{
  update();
}

void cedar::aux::gui::LocalCoordinateFrameWidget::setLocalCoordinateFrame
(
  cedar::aux::LocalCoordinateFramePtr pLocalCoordinateFrame
)
{
  mpLocalCoordinateFrame = pLocalCoordinateFrame;
}

void cedar::aux::gui::LocalCoordinateFrameWidget::decimalsChanged()
{
  mpTranslationXSpinBox->setDecimals(static_cast<int>(this->getDecimals()));
  mpTranslationYSpinBox->setDecimals(static_cast<int>(this->getDecimals()));
  mpTranslationZSpinBox->setDecimals(static_cast<int>(this->getDecimals()));
}

void cedar::aux::gui::LocalCoordinateFrameWidget::initWindow()
{
  setWindowTitle(QString("Local Coordinate Frame"));

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
  mpTranslationXSpinBox = new QDoubleSpinBox();
  mpTranslationXSpinBox->setRange(mXMin, mXMax);
  mpTranslationXSpinBox->setValue(mpLocalCoordinateFrame->getTranslationX() / cedar::unit::DEFAULT_LENGTH_UNIT);
  mpTranslationXSpinBox->setSingleStep(mSingleTranslationStep);
  connect(mpTranslationXSpinBox, SIGNAL(valueChanged(double)), this, SLOT(positionChanged(double)));
  mpGridLayout->addWidget(mpTranslationXSpinBox, 1, 1, 1, 2);

  mpTranslationYSpinBox = new QDoubleSpinBox();
  mpTranslationYSpinBox->setRange(mYMin, mYMax);
  mpTranslationYSpinBox->setValue(mpLocalCoordinateFrame->getTranslationY() / cedar::unit::DEFAULT_LENGTH_UNIT);
  mpTranslationYSpinBox->setSingleStep(mSingleTranslationStep);
  connect(mpTranslationYSpinBox, SIGNAL(valueChanged(double)), this, SLOT(positionChanged(double)));
  mpGridLayout->addWidget(mpTranslationYSpinBox, 1, 3, 1, 2);

  mpTranslationZSpinBox = new QDoubleSpinBox();
  mpTranslationZSpinBox->setRange(mZMin, mZMax);
  mpTranslationZSpinBox->setValue(mpLocalCoordinateFrame->getTranslationZ() / cedar::unit::DEFAULT_LENGTH_UNIT);
  mpTranslationZSpinBox->setSingleStep(mSingleTranslationStep);
  connect(mpTranslationZSpinBox, SIGNAL(valueChanged(double)), this, SLOT(positionChanged(double)));
  mpGridLayout->addWidget(mpTranslationZSpinBox, 1, 5, 1, 2);


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

void cedar::aux::gui::LocalCoordinateFrameWidget::update()
{
  cv::Mat T = mpLocalCoordinateFrame->getTransformation().clone();

  // update rotation matrix
  for(unsigned int i = 0; i < 3; i++)
  {
    for(unsigned int j = 0; j < 3; j++)
    {
      QLabel* p_label = static_cast<QLabel*>(mpGridLayout->itemAtPosition(i+3, (j+1)*2)->widget());
      float r = T.at<float>(i, j);
      if (cedar::aux::math::isZero(r))
      {
        r = 0;
      }
      p_label->setText(QString("%1").arg(r, 0, 'g', static_cast<int>(this->getDecimals()), '0'));
    }
  }

  // update position spin boxes
  mpTranslationXSpinBox->blockSignals(true);
  mpTranslationYSpinBox->blockSignals(true);
  mpTranslationZSpinBox->blockSignals(true);
  mpTranslationXSpinBox->setValue(T.at<float>(0, 3));
  mpTranslationYSpinBox->setValue(T.at<float>(1, 3));
  mpTranslationZSpinBox->setValue(T.at<float>(2, 3));
  mpTranslationXSpinBox->blockSignals(false);
  mpTranslationYSpinBox->blockSignals(false);
  mpTranslationZSpinBox->blockSignals(false);
}

void cedar::aux::gui::LocalCoordinateFrameWidget::positionChanged(double)
{
  mpLocalCoordinateFrame->setTranslation
  (
    std::vector<float>
    (
      {
        float(mpTranslationXSpinBox->value()),
        float(mpTranslationYSpinBox->value()),
        float(mpTranslationZSpinBox->value())
      }
    )
  );
}

void cedar::aux::gui::LocalCoordinateFrameWidget::rotateXPos()
{
  mpLocalCoordinateFrame->rotate(0, mSingleRotationStep * cedar::unit::DEFAULT_PLANE_ANGLE_UNIT);
}

void cedar::aux::gui::LocalCoordinateFrameWidget::rotateXNeg()
{
  mpLocalCoordinateFrame->rotate(0, -mSingleRotationStep * cedar::unit::DEFAULT_PLANE_ANGLE_UNIT);
}

void cedar::aux::gui::LocalCoordinateFrameWidget::rotateYPos()
{
  mpLocalCoordinateFrame->rotate(1, mSingleRotationStep * cedar::unit::DEFAULT_PLANE_ANGLE_UNIT);
}

void cedar::aux::gui::LocalCoordinateFrameWidget::rotateYNeg()
{
  mpLocalCoordinateFrame->rotate(1, -mSingleRotationStep * cedar::unit::DEFAULT_PLANE_ANGLE_UNIT);
}

void cedar::aux::gui::LocalCoordinateFrameWidget::rotateZPos()
{
  mpLocalCoordinateFrame->rotate(2, mSingleRotationStep * cedar::unit::DEFAULT_PLANE_ANGLE_UNIT);
}

void cedar::aux::gui::LocalCoordinateFrameWidget::rotateZNeg()
{
  mpLocalCoordinateFrame->rotate(2, -mSingleRotationStep * cedar::unit::DEFAULT_PLANE_ANGLE_UNIT);
}

