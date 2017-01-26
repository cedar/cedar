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

    File:        ObjectVisualizationWidget.cpp

    Maintainer:  Hendrik Reimann
    Email:       hendrik.reimann@ini.rub.de
    Date:        2012 02 01

    Description: Implementation of the @em cedar::aux::ObjectVisualizationWidget class.

    Credits:

======================================================================================================================*/

// CEDAR INCLUDES
#include "cedar/auxiliaries/gui/ObjectVisualizationWidget.h"
#include "cedar/auxiliaries/exceptions.h"

// SYSTEM INCLUDES
#include <QApplication>
#include <QLabel>
#include <QPushButton>
#include <QLineEdit>
#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QWidget>

//----------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------

cedar::aux::gui::ObjectVisualizationWidget::ObjectVisualizationWidget
(
  const cedar::aux::gl::ObjectVisualizationPtr rigidBodyVisualization,
  QWidget* parent
)
:
QWidget(parent),
mpObjectVisualization(rigidBodyVisualization)
{
  initWindow();
}

cedar::aux::gui::ObjectVisualizationWidget::~ObjectVisualizationWidget()
{

}

//------------------------------------------------------------------------------
// methods
//------------------------------------------------------------------------------

void cedar::aux::gui::ObjectVisualizationWidget::setObjectVisualization(cedar::aux::gl::ObjectVisualizationPtr pObjectVisualization)
{
  mpObjectVisualization = pObjectVisualization;
}

void cedar::aux::gui::ObjectVisualizationWidget::initWindow()
{
  setWindowTitle(QString("Object Visualization"));

  // create widgets and layouts
  QVBoxLayout* main_layout = new QVBoxLayout();
  QHBoxLayout* check_box_layout = new QHBoxLayout();
  QHBoxLayout* edit_layout = new QHBoxLayout();
  QHBoxLayout* color_layout = new QHBoxLayout();

  // layout first row
  mpVisibleCheckBox = new QCheckBox("visible");
  check_box_layout->addWidget(mpVisibleCheckBox);
  connect(mpVisibleCheckBox, SIGNAL(stateChanged(int)), this, SLOT(setVisibilityState(int)));
  mpWireFrameCheckBox = new QCheckBox("wire frame");
  check_box_layout->addWidget(mpWireFrameCheckBox);
  connect(mpWireFrameCheckBox, SIGNAL(stateChanged(int)), this, SLOT(setWireFrameState(int)));
  mpLcfCheckBox = new QCheckBox("local coordinate frame");
  check_box_layout->addWidget(mpLcfCheckBox);
  connect(mpLcfCheckBox, SIGNAL(stateChanged(int)), this, SLOT(setLcfState(int)));

  // layout second row
  QLabel* label = new QLabel(QString("coordinate frame axes length:"));
  label->setAlignment(Qt::AlignCenter);
  edit_layout->addWidget(label);
  mpAxisLengthLineEdit = new QLineEdit;
  edit_layout->addWidget(mpAxisLengthLineEdit);
  connect(mpAxisLengthLineEdit, SIGNAL(editingFinished()), this, SLOT(setAxisLength()));

  // layout third row
  label = new QLabel(QString("R:"));
  label->setAlignment(Qt::AlignRight);
  color_layout->addWidget(label);
  mpRedDoubleSpinBox = new QDoubleSpinBox();
  mpRedDoubleSpinBox->setMaximum(1.0);
  mpRedDoubleSpinBox->setSingleStep(0.05);
  color_layout->addWidget(mpRedDoubleSpinBox);
  connect(mpRedDoubleSpinBox, SIGNAL(valueChanged(double)), this, SLOT(setColor(double)));
  label = new QLabel(QString("G:"));
  label->setAlignment(Qt::AlignRight);
  color_layout->addWidget(label);
  mpGreenDoubleSpinBox = new QDoubleSpinBox();
  mpGreenDoubleSpinBox->setMaximum(1.0);
  mpGreenDoubleSpinBox->setSingleStep(0.05);
  color_layout->addWidget(mpGreenDoubleSpinBox);
  connect(mpGreenDoubleSpinBox, SIGNAL(valueChanged(double)), this, SLOT(setColor(double)));
  label = new QLabel(QString("B:"));
  label->setAlignment(Qt::AlignRight);
  color_layout->addWidget(label);
  mpBlueDoubleSpinBox = new QDoubleSpinBox();
  mpBlueDoubleSpinBox->setMaximum(1.0);
  mpBlueDoubleSpinBox->setSingleStep(0.05);
  color_layout->addWidget(mpBlueDoubleSpinBox);
  connect(mpBlueDoubleSpinBox, SIGNAL(valueChanged(double)), this, SLOT(setColor(double)));

  // layout main widget
  main_layout->addLayout(check_box_layout);
  main_layout->addLayout(edit_layout);
  main_layout->addLayout(color_layout);
  this->setLayout(main_layout);

  update();
}

void cedar::aux::gui::ObjectVisualizationWidget::update()
{
  // update visibility check box
  mpVisibleCheckBox->blockSignals(true);
  if (mpObjectVisualization->isVisible())
  {
    mpVisibleCheckBox->setCheckState(Qt::Checked);
  }
  else
  {
    mpVisibleCheckBox->setCheckState(Qt::Unchecked);
  }
  mpVisibleCheckBox->blockSignals(false);

  // update wire frame check box
  mpWireFrameCheckBox->blockSignals(true);
  if (mpObjectVisualization->isDrawnAsWireFrame())
  {
    mpWireFrameCheckBox->setCheckState(Qt::Checked);
  }
  else
  {
    mpWireFrameCheckBox->setCheckState(Qt::Unchecked);
  }
  mpWireFrameCheckBox->blockSignals(false);

  // update local coordinate frame check box
  mpLcfCheckBox->blockSignals(true);
  if (mpObjectVisualization->isDrawingLocalCoordinateFrame())
  {
    mpLcfCheckBox->setCheckState(Qt::Checked);
  }
  else
  {
    mpLcfCheckBox->setCheckState(Qt::Unchecked);
  }
  mpLcfCheckBox->blockSignals(false);

  // update axis length line edit
  mpAxisLengthLineEdit->blockSignals(true);
  mpAxisLengthLineEdit->setText(QString("%1").arg(mpObjectVisualization->getAxisLength(), 0, 'g', 1, '0'));
  mpAxisLengthLineEdit->blockSignals(false);

  // update color spin boxes
  mpRedDoubleSpinBox->blockSignals(true);
  mpGreenDoubleSpinBox->blockSignals(true);
  mpBlueDoubleSpinBox->blockSignals(true);
  mpRedDoubleSpinBox->setValue(mpObjectVisualization->getColorR());
  mpGreenDoubleSpinBox->setValue(mpObjectVisualization->getColorG());
  mpBlueDoubleSpinBox->setValue(mpObjectVisualization->getColorB());
  mpRedDoubleSpinBox->blockSignals(false);
  mpGreenDoubleSpinBox->blockSignals(false);
  mpBlueDoubleSpinBox->blockSignals(false);
}

void cedar::aux::gui::ObjectVisualizationWidget::setVisibilityState(int state)
{
  mpObjectVisualization->setVisibility(state);
}

void cedar::aux::gui::ObjectVisualizationWidget::setWireFrameState(int state)
{
  mpObjectVisualization->setDrawAsWireFrame(state);
}

void cedar::aux::gui::ObjectVisualizationWidget::setLcfState(int state)
{
  mpObjectVisualization->setDrawLocalCoordinateFrame(state);
}

void cedar::aux::gui::ObjectVisualizationWidget::setAxisLength()
{
  mpObjectVisualization->setAxisLength(mpAxisLengthLineEdit->text().toDouble());
}

void cedar::aux::gui::ObjectVisualizationWidget::setColor(double)
{
  mpObjectVisualization->setColor(
      mpRedDoubleSpinBox->value(),
      mpGreenDoubleSpinBox->value(),
      mpBlueDoubleSpinBox->value()
    );
}
