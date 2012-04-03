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

    File:        SceneWidget.cpp

    Maintainer:  Hendrik Reimann
    Email:       hendrik.reimann@ini.rub.de
    Date:        2010 11 27

    Description: manages a cedar::aux::gl::Scene of cedar::aux::gl::RigidBodyVisualizations

    Credits: initially designed by Denis Hakenberg

======================================================================================================================*/

// CEDAR INCLUDES
#include "cedar/auxiliaries/gui/SceneWidget.h"
#include "cedar/auxiliaries/gui/RigidBodyWidget.h"
#include "cedar/auxiliaries/gui/RigidBodyVisualizationWidget.h"
#include "cedar/auxiliaries/RigidBody.h"
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

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

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

void cedar::aux::gui::SceneWidget::setRadius(double value)
{
  if(!mSwitchingSelectedObject)
  {
    if (mpActiveVisualization->getRigidBodyVisualizationType().compare("Cylinder") == 0)
    {
      ((cedar::aux::gl::Cylinder*)mpActiveVisualization.get())->setRadius(value);
    }
    else if (mpActiveVisualization->getRigidBodyVisualizationType().compare("Sphere") == 0)
    {
      ((cedar::aux::gl::Sphere*)mpActiveVisualization.get())->setRadius(value);
    }
    else if (mpActiveVisualization->getRigidBodyVisualizationType().compare("Cone") == 0)
    {
      ((cedar::aux::gl::Cone*)mpActiveVisualization.get())->setRadius(value);
    }
    else if (mpActiveVisualization->getRigidBodyVisualizationType().compare("Torus") == 0)
    {
      ((cedar::aux::gl::Torus*)mpActiveVisualization.get())->setRadius(value);
    }
  }
}

void cedar::aux::gui::SceneWidget::setThickness(double value)
{
  if(!mSwitchingSelectedObject)
  {
    if (mpActiveVisualization->getRigidBodyVisualizationType().compare("Torus") == 0)
    {
      ((cedar::aux::gl::Torus*)mpActiveVisualization.get())->setThickness(value);
    }
    else if (mpActiveVisualization->getRigidBodyVisualizationType().compare("Ellipse") == 0)
    {
      ((cedar::aux::gl::Ellipse*)mpActiveVisualization.get())->setThickness(value);
    }
  }
}

void cedar::aux::gui::SceneWidget::setLength(double value)
{
  if(!mSwitchingSelectedObject)
  {
    if (mpActiveVisualization->getRigidBodyVisualizationType().compare("Block") == 0)
    {
      ((cedar::aux::gl::Block*)mpActiveVisualization.get())->setLength(value);
    }
    else if (mpActiveVisualization->getRigidBodyVisualizationType().compare("Pyramid") == 0)
    {
      ((cedar::aux::gl::Pyramid*)mpActiveVisualization.get())->setLength(value);
    }
    else if (mpActiveVisualization->getRigidBodyVisualizationType().compare("Chessboard") == 0)
    {
      ((cedar::aux::gl::Chessboard*)mpActiveVisualization.get())->setLength(value);
    }
    else if (mpActiveVisualization->getRigidBodyVisualizationType().compare("Ellipse") == 0)
    {
      ((cedar::aux::gl::Ellipse*)mpActiveVisualization.get())->setLength(value);
    }
  }
}

void cedar::aux::gui::SceneWidget::setWidth(double value)
{
  if(!mSwitchingSelectedObject)
  {
    if (mpActiveVisualization->getRigidBodyVisualizationType().compare("Block") == 0)
    {
      ((cedar::aux::gl::Block*)mpActiveVisualization.get())->setWidth(value);
    }
    else if (mpActiveVisualization->getRigidBodyVisualizationType().compare("Pyramid") == 0)
    {
      ((cedar::aux::gl::Pyramid*)mpActiveVisualization.get())->setWidth(value);
    }
    else if (mpActiveVisualization->getRigidBodyVisualizationType().compare("Chessboard") == 0)
    {
      ((cedar::aux::gl::Chessboard*)mpActiveVisualization.get())->setWidth(value);
    }
    else if (mpActiveVisualization->getRigidBodyVisualizationType().compare("Ellipse") == 0)
    {
      ((cedar::aux::gl::Ellipse*)mpActiveVisualization.get())->setWidth(value);
    }
  }
}

void cedar::aux::gui::SceneWidget::setHeight(double value)
{
  if(!mSwitchingSelectedObject)
  {
    if (mpActiveVisualization->getRigidBodyVisualizationType().compare("Cylinder") == 0)
    {
      ((cedar::aux::gl::Cylinder*)mpActiveVisualization.get())->setHeight(value);
    }
    else if (mpActiveVisualization->getRigidBodyVisualizationType().compare("Block") == 0)
    {
      ((cedar::aux::gl::Block*)mpActiveVisualization.get())->setHeight(value);
    }
    else if (mpActiveVisualization->getRigidBodyVisualizationType().compare("Cone") == 0)
    {
      ((cedar::aux::gl::Cone*)mpActiveVisualization.get())->setHeight(value);
    }
    else if (mpActiveVisualization->getRigidBodyVisualizationType().compare("Pyramid") == 0)
    {
      ((cedar::aux::gl::Pyramid*)mpActiveVisualization.get())->setHeight(value);
    }
    else if (mpActiveVisualization->getRigidBodyVisualizationType().compare("Chessboard") == 0)
    {
      ((cedar::aux::gl::Chessboard*)mpActiveVisualization.get())->setHeight(value);
    }
  }
}

void cedar::aux::gui::SceneWidget::createRigidBody()
{
  // create the new object
  cedar::aux::RigidBodyPtr p_rigid_body(new cedar::aux::RigidBody());
  p_rigid_body->setName(mpLineEditName->text().simplified().toStdString());
  cedar::aux::gl::RigidBodyVisualizationPtr p_visualization;
  if(mpComboBoxType->currentText().compare("Cylinder") == 0)
  {
    p_visualization = cedar::aux::gl::RigidBodyVisualizationPtr(new cedar::aux::gl::Cylinder(p_rigid_body));
  }
  else if (mpComboBoxType->currentText().compare("Sphere") == 0)
  {
    p_visualization = cedar::aux::gl::RigidBodyVisualizationPtr(new cedar::aux::gl::Sphere(p_rigid_body));
  }
  else if (mpComboBoxType->currentText().compare("Block") == 0)
  {
    p_visualization = cedar::aux::gl::RigidBodyVisualizationPtr(new cedar::aux::gl::Block(p_rigid_body));
  }
  else if (mpComboBoxType->currentText().compare("Cone") == 0)
  {
    p_visualization = cedar::aux::gl::RigidBodyVisualizationPtr(new cedar::aux::gl::Cone(p_rigid_body));
  }
  else if (mpComboBoxType->currentText().compare("Pyramid") == 0)
  {
    p_visualization = cedar::aux::gl::RigidBodyVisualizationPtr(new cedar::aux::gl::Pyramid(p_rigid_body));
  }
  else if (mpComboBoxType->currentText().compare("Chessboard") == 0)
  {
    p_visualization = cedar::aux::gl::RigidBodyVisualizationPtr(new cedar::aux::gl::Chessboard(p_rigid_body));
  }
  else if(mpComboBoxType->currentText().compare("Torus") == 0)
  {
    p_visualization = cedar::aux::gl::RigidBodyVisualizationPtr(new cedar::aux::gl::Torus(p_rigid_body));
  }
  else if(mpComboBoxType->currentText().compare("Ellipse") == 0)
  {
    p_visualization = cedar::aux::gl::RigidBodyVisualizationPtr(new cedar::aux::gl::Ellipse(p_rigid_body));
  }
  else if(mpComboBoxType->currentText().compare("Prism") == 0)
  {
    p_visualization = cedar::aux::gl::RigidBodyVisualizationPtr(new cedar::aux::gl::Prism(p_rigid_body));
  }

  // add the new object to the scene and the combo box
  mpScene->addRigidBodyVisualization(p_visualization);
  mpComboBoxName->addItem(QString(mpLineEditName->text()));
  mpComboBoxName->setCurrentIndex(mpComboBoxName->count() - 1);
  mpLineEditName->setText("");
  mpObjectSettingsBox->setEnabled(true);
}

void cedar::aux::gui::SceneWidget::deleteRigidBody()
{
  // remove from scene
  mpScene->deleteRigidBodyVisualization(mpComboBoxName->currentIndex());

  // remove from comboBox
  mpComboBoxName->removeItem(mpComboBoxName->currentIndex());

  // if this was the last one
  if(mpScene->getNumberOfRigidBodyVisualizations() == 0)
  {
    mpActiveVisualization = cedar::aux::gl::RigidBodyVisualizationPtr();
    mpObjectSettingsBox->setEnabled(false);
  }
}

void cedar::aux::gui::SceneWidget::deleteAllVisualizations()
{
  mpScene->clear();
  mpComboBoxName->clear();
  mpActiveVisualization = cedar::aux::gl::RigidBodyVisualizationPtr();
  mpObjectSettingsBox->setEnabled(false);
}

void cedar::aux::gui::SceneWidget::setActiveVisualization()
{
  if (!mpScene->isEmpty())
  {
    mpActiveVisualization = mpScene->getRigidBodyVisualization(mpComboBoxName->currentIndex());
    updateWidget();
    cedar::aux::RigidBodyPtr p_active_rigid_body(mpActiveVisualization->getRigidBody());
    mpRigidBodyWidget->setRigidBody(p_active_rigid_body);
    mpRigidBodyWidget->update();
    mpRigidBodyVisualizationWidget->setRigidBodyVisualization(mpActiveVisualization);
    mpRigidBodyVisualizationWidget->update();
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

  if (mpActiveVisualization->getRigidBodyVisualizationType().compare("Cylinder") == 0)
  {
    // enable those elements that apply
    mpDoubleSpinBoxRadius->setEnabled(true);
    mpDoubleSpinBoxHeight->setEnabled(true);
    // set values
    mpDoubleSpinBoxHeight->setValue(((cedar::aux::gl::Cylinder*)mpActiveVisualization.get())->height());
    mpDoubleSpinBoxRadius->setValue(((cedar::aux::gl::Cylinder*)mpActiveVisualization.get())->radius());
  }
  else if (mpActiveVisualization->getRigidBodyVisualizationType().compare("Sphere") == 0)
  {
    mpDoubleSpinBoxRadius->setEnabled(true);
    mpDoubleSpinBoxRadius->setValue(((cedar::aux::gl::Sphere*)mpActiveVisualization.get())->radius());
  }
  else if (mpActiveVisualization->getRigidBodyVisualizationType().compare("Block") == 0)
  {
    mpDoubleSpinBoxLength->setEnabled(true);
    mpDoubleSpinBoxWidth->setEnabled(true);
    mpDoubleSpinBoxHeight->setEnabled(true);
    mpDoubleSpinBoxHeight->setValue(((cedar::aux::gl::Block*)mpActiveVisualization.get())->height());
    mpDoubleSpinBoxWidth->setValue(((cedar::aux::gl::Block*)mpActiveVisualization.get())->width());
    mpDoubleSpinBoxLength->setValue(((cedar::aux::gl::Block*)mpActiveVisualization.get())->length());
  }
  else if (mpActiveVisualization->getRigidBodyVisualizationType().compare("Cone") == 0)
  {
    mpDoubleSpinBoxRadius->setEnabled(true);
    mpDoubleSpinBoxHeight->setEnabled(true);
    mpDoubleSpinBoxHeight->setValue(((cedar::aux::gl::Cone*)mpActiveVisualization.get())->height());
    mpDoubleSpinBoxRadius->setValue(((cedar::aux::gl::Cone*)mpActiveVisualization.get())->radius());
  }
  else if (mpActiveVisualization->getRigidBodyVisualizationType().compare("Pyramid") == 0)
  {
    mpDoubleSpinBoxLength->setEnabled(true);
    mpDoubleSpinBoxWidth->setEnabled(true);
    mpDoubleSpinBoxHeight->setEnabled(true);

    mpDoubleSpinBoxHeight->setValue(((cedar::aux::gl::Block*)mpActiveVisualization.get())->height());
    mpDoubleSpinBoxWidth->setValue(((cedar::aux::gl::Block*)mpActiveVisualization.get())->width());
    mpDoubleSpinBoxLength->setValue(((cedar::aux::gl::Block*)mpActiveVisualization.get())->length());
  }
  else if (mpActiveVisualization->getRigidBodyVisualizationType().compare("Chessboard") == 0)
  {
    mpDoubleSpinBoxLength->setEnabled(true);
    mpDoubleSpinBoxWidth->setEnabled(true);
    mpDoubleSpinBoxHeight->setEnabled(true);

    mpDoubleSpinBoxHeight->setValue(((cedar::aux::gl::Chessboard*)mpActiveVisualization.get())->height());
    mpDoubleSpinBoxWidth->setValue(((cedar::aux::gl::Chessboard*)mpActiveVisualization.get())->width());
    mpDoubleSpinBoxLength->setValue(((cedar::aux::gl::Chessboard*)mpActiveVisualization.get())->length());
  }
  else if (mpActiveVisualization->getRigidBodyVisualizationType().compare("Torus") == 0)
  {
    mpDoubleSpinBoxRadius->setEnabled(true);
    mpDoubleSpinBoxThickness->setEnabled(true);
    mpDoubleSpinBoxRadius->setValue(((cedar::aux::gl::Torus*)mpActiveVisualization.get())->radius());
    mpDoubleSpinBoxThickness->setValue(((cedar::aux::gl::Torus*)mpActiveVisualization.get())->thickness());
  }
  else if (mpActiveVisualization->getRigidBodyVisualizationType().compare("Ellipse") == 0)
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

void cedar::aux::gui::SceneWidget::init()
{
  // fill combo box with names of objects in the scene
  for (int i=0; i<mpScene->getNumberOfRigidBodyVisualizations(); i++)
  {
    mpComboBoxName->addItem(QString(mpScene->getRigidBodyVisualization(i)->getRigidBodyName().c_str()));
  }
  
  // initialize rigid body visualization widget
  mpRigidBodyVisualizationWidget = new cedar::aux::gui::RigidBodyVisualizationWidget(mpScene->getRigidBodyVisualization(0));
  mpGridLayout->addWidget(mpRigidBodyVisualizationWidget, 2, 0, 1, 2);

  // initialize rigid body widget
  mpRigidBodyWidget = new cedar::aux::gui::RigidBodyWidget(mpScene->getRigidBodyVisualization(0)->getRigidBody());
  mpGridLayout->addWidget(mpRigidBodyWidget, 3, 0, 1, 2);


  if (mpScene->isEmpty())
  {
    mpObjectSettingsBox->setEnabled(false);
  }
  setActiveVisualization();

  // set widget properties
  QString name = QString("object scene widget");
  setWindowTitle(name);
  
  mpComboBoxType->addItem("Cylinder");
  mpComboBoxType->addItem("Block");
  mpComboBoxType->addItem("Chessboard");
  mpComboBoxType->addItem("Cone");
  mpComboBoxType->addItem("Pyramid");
  mpComboBoxType->addItem("Sphere");
  mpComboBoxType->addItem("Torus");
  mpComboBoxType->addItem("Ellipse");
  mpComboBoxType->addItem("Prism");
  
  //connecting to slots
  connect(mpComboBoxName, SIGNAL(currentIndexChanged(const QString &)), this, SLOT(setActiveVisualization()));
  connect(mpDoubleSpinBoxRadius, SIGNAL(valueChanged(double)), this, SLOT(setRadius(double)));
  connect(mpDoubleSpinBoxThickness, SIGNAL(valueChanged(double)), this, SLOT(setThickness(double)));
  connect(mpDoubleSpinBoxLength, SIGNAL(valueChanged(double)), this, SLOT(setLength(double)));
  connect(mpDoubleSpinBoxWidth, SIGNAL(valueChanged(double)), this, SLOT(setWidth(double)));
  connect(mpDoubleSpinBoxHeight, SIGNAL(valueChanged(double)), this, SLOT(setHeight(double)));
  
  // Buttons
  connect(mpPushButtonCreateObject, SIGNAL(pressed()), this, SLOT(createRigidBody()));
  connect(mpPushButtonDeleteAllObjects, SIGNAL(pressed()), this, SLOT(deleteAllVisualizations()));
  connect(mpPushButtonDeleteObject, SIGNAL(pressed()), this, SLOT(deleteRigidBody()));
}
