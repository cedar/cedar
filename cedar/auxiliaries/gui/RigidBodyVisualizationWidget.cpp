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

    File:        RigidBodyVisualizationWidget.cpp

    Maintainer:  Hendrik Reimann
    Email:       hendrik.reimann@ini.rub.de
    Date:        2012 02 01

    Description: Implementation of the @em cedar::aux::RigidBodyVisualizationWidget class.

    Credits:

======================================================================================================================*/

// CEDAR INCLUDES
#include "cedar/auxiliaries/gui/RigidBodyVisualizationWidget.h"
#include "cedar/auxiliaries/exceptions.h"

// SYSTEM INCLUDES
#include "stdio.h"
#include <iostream>
#include <QtGui/QApplication>
#include <QtGui/QLabel>
#include <QtGui/QPushButton>
#include <QtGui/QLineEdit>

//----------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------

cedar::aux::gui::RigidBodyVisualizationWidget::RigidBodyVisualizationWidget
(
  const cedar::aux::gl::ObjectPtr &rigidBodyVisualization,
  QWidget* parent
)
:
QWidget(parent),
mpRigidBodyVisualization(rigidBodyVisualization)
{
  initWindow();
  return;
}

cedar::aux::gui::RigidBodyVisualizationWidget::~RigidBodyVisualizationWidget()
{

}


//------------------------------------------------------------------------------
// methods
//------------------------------------------------------------------------------

void cedar::aux::gui::RigidBodyVisualizationWidget::setRigidBodyVisualization(cedar::aux::gl::ObjectPtr pRigidBodyVisualization)
{
  mpRigidBodyVisualization = pRigidBodyVisualization;
}

void cedar::aux::gui::RigidBodyVisualizationWidget::initWindow()
{
  setWindowTitle(QApplication::translate("RigidBodyVisualizationWidget", "Rigid Body Visualization"));

  mpGridLayout = new QGridLayout();

  mpVisibleCheckBox = new QCheckBox("visible");
  mpGridLayout->addWidget(mpVisibleCheckBox, 0, 0);
  connect(mpVisibleCheckBox, SIGNAL(stateChanged(int)), this, SLOT(setVisibilityState(int)));

  mpWireFrameCheckBox = new QCheckBox("wire frame");
  mpGridLayout->addWidget(mpWireFrameCheckBox, 0, 1);
  connect(mpWireFrameCheckBox, SIGNAL(stateChanged(int)), this, SLOT(setWireFrameState(int)));

  mpLcfCheckBox = new QCheckBox("local coordinate frame");
  mpGridLayout->addWidget(mpLcfCheckBox, 1, 0);
  connect(mpLcfCheckBox, SIGNAL(stateChanged(int)), this, SLOT(setLcfState(int)));

  mpAxisLengthLineEdit = new QLineEdit;
  mpGridLayout->addWidget(mpAxisLengthLineEdit, 1, 1);
  connect(mpAxisLengthLineEdit, SIGNAL(editingFinished()), this, SLOT(setAxisLength()));


  setLayout(mpGridLayout);
//  mpGridLayout->setColumnStretch(0,1);
//  mpGridLayout->setColumnStretch(1,1);

  update();
  return;

}

void cedar::aux::gui::RigidBodyVisualizationWidget::update()
{
  // update visibility check box
  mpVisibleCheckBox->blockSignals(true);
  if (mpRigidBodyVisualization->isVisible())
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
  if (mpRigidBodyVisualization->isDrawnAsWireFrame())
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
  if (mpRigidBodyVisualization->isDrawingLocalCoordinateFrame())
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
//  mpAxisLengthLineEdit->setText(QString("%1").arg(mpRigidBodyVisualization->getAxisLength(), 'g', 1));
  mpAxisLengthLineEdit->setText(QString("%1").arg(mpRigidBodyVisualization->getAxisLength(), 0, 'g', 1, '0'));
  mpAxisLengthLineEdit->blockSignals(false);
}

void cedar::aux::gui::RigidBodyVisualizationWidget::setVisibilityState(int state)
{
  mpRigidBodyVisualization->setVisibility(state);
}

void cedar::aux::gui::RigidBodyVisualizationWidget::setWireFrameState(int state)
{
  mpRigidBodyVisualization->drawAsWireFrame(state);
}

void cedar::aux::gui::RigidBodyVisualizationWidget::setLcfState(int state)
{
  mpRigidBodyVisualization->setDrawLocalCoordinateFrame(state);
}

void cedar::aux::gui::RigidBodyVisualizationWidget::setAxisLength()
{
  mpRigidBodyVisualization->setAxisLength(mpAxisLengthLineEdit->text().toDouble());
}
