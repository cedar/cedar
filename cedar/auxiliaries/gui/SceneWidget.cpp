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

    File:        SceneWidget.cpp

    Maintainer:  Hendrik Reimann
    Email:       hendrik.reimann@ini.rub.de
    Date:        2010 11 27

    Description: manages a cedar::aux::gl::Scene of cedar::aux::gl::ObjectVisualizations

    Credits: initially designed by Denis Hakenberg

======================================================================================================================*/

// CEDAR INCLUDES
#include "cedar/auxiliaries/gui/SceneWidget.h"
#include "cedar/auxiliaries/gui/LocalCoordinateFrameWidget.h"
#include "cedar/auxiliaries/gui/ObjectVisualizationWidget.h"
#include "cedar/auxiliaries/LocalCoordinateFrame.h"
#include "cedar/auxiliaries/gl/Block.h"
#include "cedar/auxiliaries/gl/Sphere.h"
#include "cedar/auxiliaries/gl/Cone.h"
#include "cedar/auxiliaries/gl/Cylinder.h"
#include "cedar/auxiliaries/gl/Pyramid.h"
#include "cedar/auxiliaries/gl/Prism.h"
#include "cedar/auxiliaries/gl/Torus.h"
#include "cedar/auxiliaries/gl/Ellipse.h"
#include "cedar/auxiliaries/gl/Chessboard.h"

// SYSTEM INCLUDES
#include <QLabel>


// SYSTEM INCLUDES

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::aux::gui::SceneWidget::SceneWidget(QWidget *parent)
:
cedar::aux::gui::BaseWidget("SceneControl", parent)
{
}

cedar::aux::gui::SceneWidget::SceneWidget(cedar::aux::gl::ScenePtr p_scene, QWidget*)
:
mpScene(p_scene)
{
  setupUi(this);
  init();
}

cedar::aux::gui::SceneWidget::~SceneWidget()
{
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::aux::gui::SceneWidget::deleteObject()
{
  mpScene->deleteObjectVisualization(mpActiveVisualization->objectName().toStdString());
}

void cedar::aux::gui::SceneWidget::setRadius(double value)
{
  if(!mSwitchingSelectedObject)
  {
    if (mpActiveVisualization->getObjectVisualizationType().compare("Cylinder") == 0)
    {
      ((cedar::aux::gl::Cylinder*)mpActiveVisualization.get())->setRadius(value);
    }
    else if (mpActiveVisualization->getObjectVisualizationType().compare("Sphere") == 0)
    {
      ((cedar::aux::gl::Sphere*)mpActiveVisualization.get())->setRadius(value);
    }
    else if (mpActiveVisualization->getObjectVisualizationType().compare("Cone") == 0)
    {
      ((cedar::aux::gl::Cone*)mpActiveVisualization.get())->setRadius(value);
    }
    else if (mpActiveVisualization->getObjectVisualizationType().compare("Torus") == 0)
    {
      ((cedar::aux::gl::Torus*)mpActiveVisualization.get())->setRadius(value);
    }
  }
}

void cedar::aux::gui::SceneWidget::setThickness(double value)
{
  if(!mSwitchingSelectedObject)
  {
    if (mpActiveVisualization->getObjectVisualizationType().compare("Torus") == 0)
    {
      ((cedar::aux::gl::Torus*)mpActiveVisualization.get())->setThickness(value);
    }
    else if (mpActiveVisualization->getObjectVisualizationType().compare("Ellipse") == 0)
    {
      ((cedar::aux::gl::Ellipse*)mpActiveVisualization.get())->setThickness(value);
    }
  }
}

void cedar::aux::gui::SceneWidget::setLength(double value)
{
  if(!mSwitchingSelectedObject)
  {
    if (mpActiveVisualization->getObjectVisualizationType().compare("Block") == 0)
    {
      ((cedar::aux::gl::Block*)mpActiveVisualization.get())->setLength(value);
    }
    else if (mpActiveVisualization->getObjectVisualizationType().compare("Pyramid") == 0)
    {
      ((cedar::aux::gl::Pyramid*)mpActiveVisualization.get())->setLength(value);
    }
    else if (mpActiveVisualization->getObjectVisualizationType().compare("Chessboard") == 0)
    {
      ((cedar::aux::gl::Chessboard*)mpActiveVisualization.get())->setLength(value);
    }
    else if (mpActiveVisualization->getObjectVisualizationType().compare("Ellipse") == 0)
    {
      ((cedar::aux::gl::Ellipse*)mpActiveVisualization.get())->setLength(value);
    }
  }
}

void cedar::aux::gui::SceneWidget::setWidth(double value)
{
  if(!mSwitchingSelectedObject)
  {
    if (mpActiveVisualization->getObjectVisualizationType().compare("Block") == 0)
    {
      ((cedar::aux::gl::Block*)mpActiveVisualization.get())->setWidth(value);
    }
    else if (mpActiveVisualization->getObjectVisualizationType().compare("Pyramid") == 0)
    {
      ((cedar::aux::gl::Pyramid*)mpActiveVisualization.get())->setWidth(value);
    }
    else if (mpActiveVisualization->getObjectVisualizationType().compare("Chessboard") == 0)
    {
      ((cedar::aux::gl::Chessboard*)mpActiveVisualization.get())->setWidth(value);
    }
    else if (mpActiveVisualization->getObjectVisualizationType().compare("Ellipse") == 0)
    {
      ((cedar::aux::gl::Ellipse*)mpActiveVisualization.get())->setWidth(value);
    }
  }
}

void cedar::aux::gui::SceneWidget::setHeight(double value)
{
  if(!mSwitchingSelectedObject)
  {
    if (mpActiveVisualization->getObjectVisualizationType().compare("Cylinder") == 0)
    {
      ((cedar::aux::gl::Cylinder*)mpActiveVisualization.get())->setHeight(value);
    }
    else if (mpActiveVisualization->getObjectVisualizationType().compare("Block") == 0)
    {
      ((cedar::aux::gl::Block*)mpActiveVisualization.get())->setHeight(value);
    }
    else if (mpActiveVisualization->getObjectVisualizationType().compare("Cone") == 0)
    {
      ((cedar::aux::gl::Cone*)mpActiveVisualization.get())->setHeight(value);
    }
    else if (mpActiveVisualization->getObjectVisualizationType().compare("Pyramid") == 0)
    {
      ((cedar::aux::gl::Pyramid*)mpActiveVisualization.get())->setHeight(value);
    }
    else if (mpActiveVisualization->getObjectVisualizationType().compare("Chessboard") == 0)
    {
      ((cedar::aux::gl::Chessboard*)mpActiveVisualization.get())->setHeight(value);
    }
  }
}

void cedar::aux::gui::SceneWidget::setActiveVisualization()
{
  if (!mpScene->isEmpty())
  {
    mpActiveVisualization = mpScene->getObjectVisualization(mpComboBoxName->currentIndex());
    updateWidget();
    cedar::aux::LocalCoordinateFramePtr p_active_rigid_body(mpActiveVisualization->getLocalCoordinateFrame());
    mpLocalCoordinateFrameWidget->setLocalCoordinateFrame(p_active_rigid_body);
    mpLocalCoordinateFrameWidget->update();
    mpObjectVisualizationWidget->setObjectVisualization(mpActiveVisualization);
    mpObjectVisualizationWidget->update();
  }
}

void cedar::aux::gui::SceneWidget::updateWidget()
{
  mSwitchingSelectedObject = true;

  // disable all elements
  mpDoubleSpinBoxLength->setEnabled(false);
  mpDoubleSpinBoxWidth->setEnabled(false);
  mpDoubleSpinBoxHeight->setEnabled(false);
  mpDoubleSpinBoxRadius->setEnabled(false);
  mpDoubleSpinBoxThickness->setEnabled(false);

  if(mpScene->isEmpty())
  {
    return;
  }
  else
  {
    // initialize rigid body visualization widget
    mpObjectVisualizationWidget = new cedar::aux::gui::ObjectVisualizationWidget(mpActiveVisualization);
    mpGridLayout->addWidget(mpObjectVisualizationWidget, 2, 0, 1, 2);

    // initialize rigid body widget
    mpLocalCoordinateFrameWidget = new cedar::aux::gui::LocalCoordinateFrameWidget
    (
      mpActiveVisualization->getLocalCoordinateFrame()
    );

    mpGridLayout->addWidget(mpLocalCoordinateFrameWidget, 4, 0, 1, 2);
  }

  if (mpActiveVisualization->getObjectVisualizationType().compare("Cylinder") == 0)
  {
    // enable those elements that apply
    mpDoubleSpinBoxRadius->setEnabled(true);
    mpDoubleSpinBoxHeight->setEnabled(true);
    // set values
    mpDoubleSpinBoxHeight->setValue(((cedar::aux::gl::Cylinder*)mpActiveVisualization.get())->height());
    mpDoubleSpinBoxRadius->setValue(((cedar::aux::gl::Cylinder*)mpActiveVisualization.get())->radius());
  }
  else if (mpActiveVisualization->getObjectVisualizationType().compare("Sphere") == 0)
  {
    mpDoubleSpinBoxRadius->setEnabled(true);
    mpDoubleSpinBoxRadius->setValue(((cedar::aux::gl::Sphere*)mpActiveVisualization.get())->radius());
  }
  else if (mpActiveVisualization->getObjectVisualizationType().compare("Block") == 0)
  {
    mpDoubleSpinBoxLength->setEnabled(true);
    mpDoubleSpinBoxWidth->setEnabled(true);
    mpDoubleSpinBoxHeight->setEnabled(true);
    mpDoubleSpinBoxHeight->setValue(((cedar::aux::gl::Block*)mpActiveVisualization.get())->height());
    mpDoubleSpinBoxWidth->setValue(((cedar::aux::gl::Block*)mpActiveVisualization.get())->width());
    mpDoubleSpinBoxLength->setValue(((cedar::aux::gl::Block*)mpActiveVisualization.get())->length());
  }
  else if (mpActiveVisualization->getObjectVisualizationType().compare("Cone") == 0)
  {
    mpDoubleSpinBoxRadius->setEnabled(true);
    mpDoubleSpinBoxHeight->setEnabled(true);
    mpDoubleSpinBoxHeight->setValue(((cedar::aux::gl::Cone*)mpActiveVisualization.get())->height());
    mpDoubleSpinBoxRadius->setValue(((cedar::aux::gl::Cone*)mpActiveVisualization.get())->radius());
  }
  else if (mpActiveVisualization->getObjectVisualizationType().compare("Pyramid") == 0)
  {
    mpDoubleSpinBoxLength->setEnabled(true);
    mpDoubleSpinBoxWidth->setEnabled(true);
    mpDoubleSpinBoxHeight->setEnabled(true);

    mpDoubleSpinBoxHeight->setValue(((cedar::aux::gl::Block*)mpActiveVisualization.get())->height());
    mpDoubleSpinBoxWidth->setValue(((cedar::aux::gl::Block*)mpActiveVisualization.get())->width());
    mpDoubleSpinBoxLength->setValue(((cedar::aux::gl::Block*)mpActiveVisualization.get())->length());
  }
  else if (mpActiveVisualization->getObjectVisualizationType().compare("Chessboard") == 0)
  {
    mpDoubleSpinBoxLength->setEnabled(true);
    mpDoubleSpinBoxWidth->setEnabled(true);
    mpDoubleSpinBoxHeight->setEnabled(true);

    mpDoubleSpinBoxHeight->setValue(((cedar::aux::gl::Chessboard*)mpActiveVisualization.get())->height());
    mpDoubleSpinBoxWidth->setValue(((cedar::aux::gl::Chessboard*)mpActiveVisualization.get())->width());
    mpDoubleSpinBoxLength->setValue(((cedar::aux::gl::Chessboard*)mpActiveVisualization.get())->length());
  }
  else if (mpActiveVisualization->getObjectVisualizationType().compare("Torus") == 0)
  {
    mpDoubleSpinBoxRadius->setEnabled(true);
    mpDoubleSpinBoxThickness->setEnabled(true);
    mpDoubleSpinBoxRadius->setValue(((cedar::aux::gl::Torus*)mpActiveVisualization.get())->radius());
    mpDoubleSpinBoxThickness->setValue(((cedar::aux::gl::Torus*)mpActiveVisualization.get())->thickness());
  }
  else if (mpActiveVisualization->getObjectVisualizationType().compare("Ellipse") == 0)
  {
    mpDoubleSpinBoxThickness->setEnabled(true);
    mpDoubleSpinBoxLength->setEnabled(true);
    mpDoubleSpinBoxWidth->setEnabled(true);
    mpDoubleSpinBoxThickness->setValue(((cedar::aux::gl::Ellipse*)mpActiveVisualization.get())->thickness());
    mpDoubleSpinBoxLength->setValue(((cedar::aux::gl::Ellipse*)mpActiveVisualization.get())->length());
    mpDoubleSpinBoxWidth->setValue(((cedar::aux::gl::Ellipse*)mpActiveVisualization.get())->width());
  }

  mSwitchingSelectedObject = false;
}

void cedar::aux::gui::SceneWidget::updateObjectSelectionComboBox()
{
  mpComboBoxName->blockSignals(true);
  mpComboBoxName->clear();

  const int numObjects = mpScene->getNumberOfObjectVisualizations();

  if(numObjects == 0)
  {
    mpComboBoxName->setEnabled(false);
    mpDeleteObjectPushButton->setEnabled(false);
  }
  else
  {
    mpComboBoxName->setEnabled(true);
    mpDeleteObjectPushButton->setEnabled(true);
  }

  // fill combo box with names of objects in the scene
  for (int i=0; i<numObjects; ++i)
  {

    mpComboBoxName->addItem
    (
      QString::fromStdString
      (
        mpScene->getObjectVisualization(i)->getLocalCoordinateFrame()->getName()
      )
    );

  }

  setActiveVisualization();
  mpComboBoxName->blockSignals(false);
}

void cedar::aux::gui::SceneWidget::init()
{

  updateObjectSelectionComboBox();
  updateWidget();
  setActiveVisualization();

  // set widget properties
  QString name = QString("object scene widget");
  setWindowTitle(name);
  
  //connecting to slots
  connect(mpDeleteObjectPushButton, SIGNAL(clicked()), this, SLOT(deleteObject()));
  connect(mpComboBoxName, SIGNAL(currentIndexChanged(const QString &)), this, SLOT(setActiveVisualization()));
  connect(mpDoubleSpinBoxRadius, SIGNAL(valueChanged(double)), this, SLOT(setRadius(double)));
  connect(mpDoubleSpinBoxThickness, SIGNAL(valueChanged(double)), this, SLOT(setThickness(double)));
  connect(mpDoubleSpinBoxLength, SIGNAL(valueChanged(double)), this, SLOT(setLength(double)));
  connect(mpDoubleSpinBoxWidth, SIGNAL(valueChanged(double)), this, SLOT(setWidth(double)));
  connect(mpDoubleSpinBoxHeight, SIGNAL(valueChanged(double)), this, SLOT(setHeight(double)));

  mSlotConnection = mpScene->connectToSceneChangedSignal
  (
    boost::bind(&cedar::aux::gui::SceneWidget::updateObjectSelectionComboBox, this)
  );

}
