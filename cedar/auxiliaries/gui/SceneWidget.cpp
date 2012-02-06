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

    Description: Visualization of a cylinder

    Credits: initially designed by Denis Hakenberg

    small guide to add other DrawableObjects in the widget
      1.) in function init() add 'mpComboBoxType->addItem("Your_ObjectType");'
      2.) add in the functions setRadius(), setLength(), setWidth() and setHeight()
          your ObjectType (which paramaters you need)
      3.) add in the function createObject() your ObjectType
      4.) add in the function setWidgetObjectParameters() your ObjectType

======================================================================================================================*/

// CEDAR INCLUDES
#include "cedar/auxiliaries/gui/SceneWidget.h"
#include "cedar/auxiliaries/gui/RigidBodyWidget.h"
#include "cedar/auxiliaries/gui/RigidBodyVisualizationWidget.h"
#include "cedar/auxiliaries/Object.h"

#include <QtGui/QLabel>


// SYSTEM INCLUDES

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
    if (mpActiveObject->getObjectType().compare("Cylinder") == 0)
    {
      ((cedar::aux::gl::Cylinder*)mpActiveObject.get())->setRadius(value);
    }
    else if (mpActiveObject->getObjectType().compare("Sphere") == 0)
    {
      ((cedar::aux::gl::Sphere*)mpActiveObject.get())->setRadius(value);
    }
    else if (mpActiveObject->getObjectType().compare("Cone") == 0)
    {
      ((cedar::aux::gl::Cone*)mpActiveObject.get())->setRadius(value);
    }
    else if (mpActiveObject->getObjectType().compare("Torus") == 0)
    {
      ((cedar::aux::gl::Torus*)mpActiveObject.get())->setRadius(value);
    }
  }
}

void cedar::aux::gui::SceneWidget::setThickness(double value)
{
  if(!mSwitchingSelectedObject)
  {
    if (mpActiveObject->getObjectType().compare("Torus") == 0)
    {
      ((cedar::aux::gl::Torus*)mpActiveObject.get())->setThickness(value);
    }
    else if (mpActiveObject->getObjectType().compare("Ellipse") == 0)
    {
      ((cedar::aux::gl::Ellipse*)mpActiveObject.get())->setThickness(value);
    }
  }
}

void cedar::aux::gui::SceneWidget::setLength(double value)
{
  if(!mSwitchingSelectedObject)
  {
    if (mpActiveObject->getObjectType().compare("Block") == 0)
    {
      ((cedar::aux::gl::Block*)mpActiveObject.get())->setLength(value);
    }
    else if (mpActiveObject->getObjectType().compare("Pyramid") == 0)
    {
      ((cedar::aux::gl::Pyramid*)mpActiveObject.get())->setLength(value);
    }
    else if (mpActiveObject->getObjectType().compare("Chessboard") == 0)
    {
      ((cedar::aux::gl::Chessboard*)mpActiveObject.get())->setLength(value);
    }
    else if (mpActiveObject->getObjectType().compare("Ellipse") == 0)
    {
      ((cedar::aux::gl::Ellipse*)mpActiveObject.get())->setLength(value);
    }
  }
}

void cedar::aux::gui::SceneWidget::setWidth(double value)
{
  if(!mSwitchingSelectedObject)
  {
    if (mpActiveObject->getObjectType().compare("Block") == 0)
    {
      ((cedar::aux::gl::Block*)mpActiveObject.get())->setWidth(value);
    }
    else if (mpActiveObject->getObjectType().compare("Pyramid") == 0)
    {
      ((cedar::aux::gl::Pyramid*)mpActiveObject.get())->setWidth(value);
    }
    else if (mpActiveObject->getObjectType().compare("Chessboard") == 0)
    {
      ((cedar::aux::gl::Chessboard*)mpActiveObject.get())->setWidth(value);
    }
    else if (mpActiveObject->getObjectType().compare("Ellipse") == 0)
    {
      ((cedar::aux::gl::Ellipse*)mpActiveObject.get())->setWidth(value);
    }
  }
}

void cedar::aux::gui::SceneWidget::setHeight(double value)
{
  if(!mSwitchingSelectedObject)
  {
    if (mpActiveObject->getObjectType().compare("Cylinder") == 0)
    {
      ((cedar::aux::gl::Cylinder*)mpActiveObject.get())->setHeight(value);
    }
    else if (mpActiveObject->getObjectType().compare("Block") == 0)
    {
      ((cedar::aux::gl::Block*)mpActiveObject.get())->setHeight(value);
    }
    else if (mpActiveObject->getObjectType().compare("Cone") == 0)
    {
      ((cedar::aux::gl::Cone*)mpActiveObject.get())->setHeight(value);
    }
    else if (mpActiveObject->getObjectType().compare("Pyramid") == 0)
    {
      ((cedar::aux::gl::Pyramid*)mpActiveObject.get())->setHeight(value);
    }
    else if (mpActiveObject->getObjectType().compare("Chessboard") == 0)
    {
      ((cedar::aux::gl::Chessboard*)mpActiveObject.get())->setHeight(value);
    }
  }
}

void cedar::aux::gui::SceneWidget::createObject()
{
  // create the new object
  cedar::aux::ObjectPtr p_object(new cedar::aux::Object());
  p_object->setName(mpLineEditName->text().simplified().toStdString());
  cedar::aux::gl::ObjectPtr p_gl_object;
  if(mpComboBoxType->currentText().compare("Cylinder") == 0)
  {
    p_gl_object = cedar::aux::gl::ObjectPtr(new cedar::aux::gl::Cylinder(p_object));
  }
  else if (mpComboBoxType->currentText().compare("Sphere") == 0)
  {
    p_gl_object = cedar::aux::gl::ObjectPtr(new cedar::aux::gl::Sphere(p_object));
  }
  else if (mpComboBoxType->currentText().compare("Block") == 0)
  {
    p_gl_object = cedar::aux::gl::ObjectPtr(new cedar::aux::gl::Block(p_object));
  }
  else if (mpComboBoxType->currentText().compare("Cone") == 0)
  {
    p_gl_object = cedar::aux::gl::ObjectPtr(new cedar::aux::gl::Cone(p_object));
  }
  else if (mpComboBoxType->currentText().compare("Pyramid") == 0)
  {
    p_gl_object = cedar::aux::gl::ObjectPtr(new cedar::aux::gl::Pyramid(p_object));
  }
  else if (mpComboBoxType->currentText().compare("Chessboard") == 0)
  {
    p_gl_object = cedar::aux::gl::ObjectPtr(new cedar::aux::gl::Chessboard(p_object));
  }
  else if(mpComboBoxType->currentText().compare("Torus") == 0)
  {
    p_gl_object = cedar::aux::gl::ObjectPtr(new cedar::aux::gl::Torus(p_object));
  }
  else if(mpComboBoxType->currentText().compare("Ellipse") == 0)
  {
    p_gl_object = cedar::aux::gl::ObjectPtr(new cedar::aux::gl::Ellipse(p_object));
  }
  else if(mpComboBoxType->currentText().compare("Prism") == 0)
  {
    p_gl_object = cedar::aux::gl::ObjectPtr(new cedar::aux::gl::Prism(p_object));
  }

  // add the new object to the scene and the combo box
  mpScene->addObject(p_gl_object);
  mpComboBoxName->addItem(QString(mpLineEditName->text()));
  mpComboBoxName->setCurrentIndex(mpComboBoxName->count() - 1);
  mpLineEditName->setText("");
  mpObjectSettingsBox->setEnabled(true);
}

void cedar::aux::gui::SceneWidget::deleteObject()
{
  // remove from scene
  mpScene->deleteObject(mpComboBoxName->currentIndex());

  // remove from comboBox
  mpComboBoxName->removeItem(mpComboBoxName->currentIndex());

  // if this was the last one
  if(mpScene->numberOfObjects() == 0)
  {
    mpActiveObject = cedar::aux::gl::ObjectPtr();
    mpObjectSettingsBox->setEnabled(false);
  }
}

void cedar::aux::gui::SceneWidget::deleteAllObjects()
{
  mpScene->clear();
  mpComboBoxName->clear();
  mpActiveObject = cedar::aux::gl::ObjectPtr();
  mpObjectSettingsBox->setEnabled(false);
}

void cedar::aux::gui::SceneWidget::setActiveObject()
{
  if (!mpScene->isEmpty())
  {
    mpActiveObject = mpScene->getObject(mpComboBoxName->currentIndex());
    updateWidgetObjectParameters();
    cedar::aux::ObjectPtr p_active_object(mpActiveObject->getObject());
    mpRigidBodyWidget->setRigidBody(p_active_object);
    mpRigidBodyWidget->update();
    mpRigidBodyVisualizationWidget->setRigidBodyVisualization(mpActiveObject);
    mpRigidBodyVisualizationWidget->update();
  }
}

void cedar::aux::gui::SceneWidget::updateWidgetObjectParameters()
{
  mSwitchingSelectedObject = true;

  // disable all elements
  mpDoubleSpinBoxLength->setEnabled(false);
  mpDoubleSpinBoxWidth->setEnabled(false);
  mpDoubleSpinBoxHeight->setEnabled(false);
  mpDoubleSpinBoxRadius->setEnabled(false);
  mpDoubleSpinBoxThickness->setEnabled(false);

  if (mpActiveObject->getObjectType().compare("Cylinder") == 0)
  {
    // enable those elements that apply
    mpDoubleSpinBoxRadius->setEnabled(true);
    mpDoubleSpinBoxHeight->setEnabled(true);
    // set values
    mpDoubleSpinBoxHeight->setValue(((cedar::aux::gl::Cylinder*)mpActiveObject.get())->height());
    mpDoubleSpinBoxRadius->setValue(((cedar::aux::gl::Cylinder*)mpActiveObject.get())->radius());
  }
  else if (mpActiveObject->getObjectType().compare("Sphere") == 0)
  {
    mpDoubleSpinBoxRadius->setEnabled(true);
    mpDoubleSpinBoxRadius->setValue(((cedar::aux::gl::Sphere*)mpActiveObject.get())->radius());
  }
  else if (mpActiveObject->getObjectType().compare("Block") == 0)
  {
    mpDoubleSpinBoxLength->setEnabled(true);
    mpDoubleSpinBoxWidth->setEnabled(true);
    mpDoubleSpinBoxHeight->setEnabled(true);
    mpDoubleSpinBoxHeight->setValue(((cedar::aux::gl::Block*)mpActiveObject.get())->height());
    mpDoubleSpinBoxWidth->setValue(((cedar::aux::gl::Block*)mpActiveObject.get())->width());
    mpDoubleSpinBoxLength->setValue(((cedar::aux::gl::Block*)mpActiveObject.get())->length());
  }
  else if (mpActiveObject->getObjectType().compare("Cone") == 0)
  {
    mpDoubleSpinBoxRadius->setEnabled(true);
    mpDoubleSpinBoxHeight->setEnabled(true);
    mpDoubleSpinBoxHeight->setValue(((cedar::aux::gl::Cone*)mpActiveObject.get())->height());
    mpDoubleSpinBoxRadius->setValue(((cedar::aux::gl::Cone*)mpActiveObject.get())->radius());
  }
  else if (mpActiveObject->getObjectType().compare("Pyramid") == 0)
  {
    mpDoubleSpinBoxLength->setEnabled(true);
    mpDoubleSpinBoxWidth->setEnabled(true);
    mpDoubleSpinBoxHeight->setEnabled(true);

    mpDoubleSpinBoxHeight->setValue(((cedar::aux::gl::Block*)mpActiveObject.get())->height());
    mpDoubleSpinBoxWidth->setValue(((cedar::aux::gl::Block*)mpActiveObject.get())->width());
    mpDoubleSpinBoxLength->setValue(((cedar::aux::gl::Block*)mpActiveObject.get())->length());
  }
  else if (mpActiveObject->getObjectType().compare("Chessboard") == 0)
  {
    mpDoubleSpinBoxLength->setEnabled(true);
    mpDoubleSpinBoxWidth->setEnabled(true);
    mpDoubleSpinBoxHeight->setEnabled(true);

    mpDoubleSpinBoxHeight->setValue(((cedar::aux::gl::Chessboard*)mpActiveObject.get())->height());
    mpDoubleSpinBoxWidth->setValue(((cedar::aux::gl::Chessboard*)mpActiveObject.get())->width());
    mpDoubleSpinBoxLength->setValue(((cedar::aux::gl::Chessboard*)mpActiveObject.get())->length());
  }
  else if (mpActiveObject->getObjectType().compare("Torus") == 0)
  {
    mpDoubleSpinBoxRadius->setEnabled(true);
    mpDoubleSpinBoxThickness->setEnabled(true);
    mpDoubleSpinBoxRadius->setValue(((cedar::aux::gl::Torus*)mpActiveObject.get())->radius());
    mpDoubleSpinBoxThickness->setValue(((cedar::aux::gl::Torus*)mpActiveObject.get())->thickness());
  }
  else if (mpActiveObject->getObjectType().compare("Ellipse") == 0)
  {
    mpDoubleSpinBoxThickness->setEnabled(true);
    mpDoubleSpinBoxLength->setEnabled(true);
    mpDoubleSpinBoxWidth->setEnabled(true);
    mpDoubleSpinBoxThickness->setValue(((cedar::aux::gl::Ellipse*)mpActiveObject.get())->thickness());
    mpDoubleSpinBoxLength->setValue(((cedar::aux::gl::Ellipse*)mpActiveObject.get())->length());
    mpDoubleSpinBoxWidth->setValue(((cedar::aux::gl::Ellipse*)mpActiveObject.get())->width());
  }

  mSwitchingSelectedObject = false;
}

void cedar::aux::gui::SceneWidget::init()
{
  // fill combo box with names of objects in the scene
  for (int i=0; i<mpScene->numberOfObjects(); i++)
  {
    mpComboBoxName->addItem(QString(mpScene->getObject(i)->getObjectName().c_str()));
  }
  
  // initialize rigid body visualization widget
  mpRigidBodyVisualizationWidget = new cedar::aux::gui::RigidBodyVisualizationWidget(mpScene->getObject(0));
  mpRigidBodyVisualizationWidgetLayout->addWidget(mpRigidBodyVisualizationWidget);

  // initialize rigid body widget
  mpRigidBodyWidget = new cedar::aux::gui::RigidBodyWidget(mpScene->getObject(0)->getObject());
  mpRigidBodyWidgetLayout->addWidget(mpRigidBodyWidget);

  if (mpScene->isEmpty())
  {
    mpObjectSettingsBox->setEnabled(false);
  }
  setActiveObject();

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
  connect(mpComboBoxName, SIGNAL(currentIndexChanged(const QString &)), this, SLOT(setActiveObject()));
  connect(mpDoubleSpinBoxRadius, SIGNAL(valueChanged(double)), this, SLOT(setRadius(double)));
  connect(mpDoubleSpinBoxThickness, SIGNAL(valueChanged(double)), this, SLOT(setThickness(double)));
  connect(mpDoubleSpinBoxLength, SIGNAL(valueChanged(double)), this, SLOT(setLength(double)));
  connect(mpDoubleSpinBoxWidth, SIGNAL(valueChanged(double)), this, SLOT(setWidth(double)));
  connect(mpDoubleSpinBoxHeight, SIGNAL(valueChanged(double)), this, SLOT(setHeight(double)));
  
  // Buttons
  connect(mpPushButtonCreateObject, SIGNAL(pressed()), this, SLOT(createObject()));
  connect(mpPushButtonDeleteAllObjects, SIGNAL(pressed()), this, SLOT(deleteAllObjects()));
  connect(mpPushButtonDeleteObject, SIGNAL(pressed()), this, SLOT(deleteObject()));
}
