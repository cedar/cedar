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

 ----- Institute:   Ruhr-Universitaet-Bochum
                    Institut fuer Neuroinformatik
 
 ----- File:        SceneWidget.cpp
 
 ----- Maintainer:  Hendrik Reimann
 ------Email:       hendrik.reimann@ini.rub.de
 ----- Date:        2010 11 27
 
 ----- Description: widget for cedar::aux::gl::Scene
 
 ----- Credits:     initially designed by Denis Hakenberg
 
  small guide to add other DrawableObjects in the widget
  1.) in function init() add 'mpComboBoxType->addItem("Your_ObjectType");'
  2.) add in the functions setRadius(), setLength(), setWidth() and setHeight()
      your ObjectType (which paramaters you need)
  3.) add in the function createObject() your ObjectType
  4.) add in the function setWidgetObjectParameters() your ObjectType
 ---------------------------------------------------------------------------------------------------------------------*/

// LOCAL INCLUDES
#include "auxiliaries/gui/SceneWidget.h"

// PROJECT INCLUDES
#include "auxiliaries/Object.h"

// SYSTEM INCLUDES

using namespace cedar::aux::gl;
using namespace cedar::aux::gui;
using namespace std;
using namespace cv;

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

SceneWidget::SceneWidget(cedar::aux::gl::ScenePtr p_scene, QWidget*)
:
mpScene(p_scene)
{
	setupUi(this);
	init();
}

SceneWidget::~SceneWidget()
{

}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void SceneWidget::setWireFrame(int state)
{
	if(!mSwitchingSelectedObject)
	{
		mpActiveObject->drawAsWireFrame(state);
	}
}

void SceneWidget::setPosition()
{
	if(!mSwitchingSelectedObject)
	{
		mpActiveObject->getObject()->setPosition(
		                                          mpDoubleSpinBoxPositionX->value(),
                                              mpDoubleSpinBoxPositionY->value(),
                                              mpDoubleSpinBoxPositionZ->value()
                                            );
	}
}

void SceneWidget::rotate()
{
  mpRotateXSpinBox->blockSignals(true);
  mpRotateYSpinBox->blockSignals(true);
  mpRotateZSpinBox->blockSignals(true);
  mpDoubleSpinBoxRotation0->blockSignals(true);
  mpDoubleSpinBoxRotation1->blockSignals(true);
  mpDoubleSpinBoxRotation2->blockSignals(true);
  mpDoubleSpinBoxRotation3->blockSignals(true);

  double angle;
  angle = mpRotateXSpinBox->value();
  mpActiveObject->getObject()->rotate(0, angle);
  mpRotateXSpinBox->setValue(0.0);
  angle = mpRotateYSpinBox->value();
  mpActiveObject->getObject()->rotate(1, angle);
  mpRotateYSpinBox->setValue(0.0);
  angle = mpRotateZSpinBox->value();
  mpActiveObject->getObject()->rotate(2, angle);
  mpRotateZSpinBox->setValue(0.0);

  mpDoubleSpinBoxRotation0->setValue(mpActiveObject->getObject()->getOrientationQuaternion(0));
  mpDoubleSpinBoxRotation1->setValue(mpActiveObject->getObject()->getOrientationQuaternion(1));
  mpDoubleSpinBoxRotation2->setValue(mpActiveObject->getObject()->getOrientationQuaternion(2));
  mpDoubleSpinBoxRotation3->setValue(mpActiveObject->getObject()->getOrientationQuaternion(3));

  mpRotateXSpinBox->blockSignals(false);
  mpRotateYSpinBox->blockSignals(false);
  mpRotateZSpinBox->blockSignals(false);
  mpDoubleSpinBoxRotation0->blockSignals(false);
  mpDoubleSpinBoxRotation1->blockSignals(false);
  mpDoubleSpinBoxRotation2->blockSignals(false);
  mpDoubleSpinBoxRotation3->blockSignals(false);
}

void SceneWidget::setOrientationQuaternion()
{
  if(!mSwitchingSelectedObject)
  {
    cv::Mat q(4, 1, CV_64FC1);
    q.at<double>(0, 0) = mpDoubleSpinBoxRotation0->value();
    q.at<double>(1, 0) = mpDoubleSpinBoxRotation1->value();
    q.at<double>(2, 0) = mpDoubleSpinBoxRotation2->value();
    q.at<double>(3, 0) = mpDoubleSpinBoxRotation3->value();
    q = q * 1 / norm(q);
    mpActiveObject->getObject()->setOrientationQuaternion(q);
    mpDoubleSpinBoxRotation0->blockSignals(true);
    mpDoubleSpinBoxRotation1->blockSignals(true);
    mpDoubleSpinBoxRotation2->blockSignals(true);
    mpDoubleSpinBoxRotation3->blockSignals(true);
    mpDoubleSpinBoxRotation0->setValue(mpActiveObject->getObject()->getOrientationQuaternion(0));
    mpDoubleSpinBoxRotation1->setValue(mpActiveObject->getObject()->getOrientationQuaternion(1));
    mpDoubleSpinBoxRotation2->setValue(mpActiveObject->getObject()->getOrientationQuaternion(2));
    mpDoubleSpinBoxRotation3->setValue(mpActiveObject->getObject()->getOrientationQuaternion(3));
    mpDoubleSpinBoxRotation0->blockSignals(false);
    mpDoubleSpinBoxRotation1->blockSignals(false);
    mpDoubleSpinBoxRotation2->blockSignals(false);
    mpDoubleSpinBoxRotation3->blockSignals(false);
  }
  //!\todo this does not really make sense yet, should think of better representation, e.g. remove the first box and always norm the others
}

void SceneWidget::setColor()
{
	if(!mSwitchingSelectedObject)
	{
		mpActiveObject->setColor(
		                          mpDoubleSpinBoxColorR->value(),
                              mpDoubleSpinBoxColorG->value(),
                              mpDoubleSpinBoxColorB->value()
                            );
	}
}

void SceneWidget::setSecondColor()
{
	if(!mSwitchingSelectedObject)
	{
		if (mpActiveObject->getObjectType().compare("Chessboard") == 0)
		{
      ((Chessboard*)mpActiveObject.get())->setSecondColor(
                                                           mpDoubleSpinBoxSecondColorR->value(),
                                                           mpDoubleSpinBoxSecondColorG->value(),
                                                           mpDoubleSpinBoxSecondColorB->value()
                                                         );
		}
	}
}

void SceneWidget::setRadius(double value)
{
	if(!mSwitchingSelectedObject)
	{
		if (mpActiveObject->getObjectType().compare("Cylinder") == 0)
		{
      ((Cylinder*)mpActiveObject.get())->setRadius(value);
		}
		else if (mpActiveObject->getObjectType().compare("Sphere") == 0)
		{
			((Sphere*)mpActiveObject.get())->setRadius(value);
		}
		else if (mpActiveObject->getObjectType().compare("Cone") == 0)
		{
			((Cone*)mpActiveObject.get())->setRadius(value);
		}
		else if (mpActiveObject->getObjectType().compare("Torus") == 0)
		{
			((Torus*)mpActiveObject.get())->setRadius(value);
		}
	}
}

void SceneWidget::setThickness(double value)
{
	if(!mSwitchingSelectedObject)
	{
		if (mpActiveObject->getObjectType().compare("Torus") == 0)
		{
			((Torus*)mpActiveObject.get())->setThickness(value);
		}
		else if (mpActiveObject->getObjectType().compare("Ellipse") == 0)
		{
			((Ellipse*)mpActiveObject.get())->setThickness(value);
		}
	}
}

void SceneWidget::setLength(double value)
{
	if(!mSwitchingSelectedObject)
	{
		if (mpActiveObject->getObjectType().compare("Block") == 0)
		{
			((Block*)mpActiveObject.get())->setLength(value);
		}
		else if (mpActiveObject->getObjectType().compare("Pyramid") == 0)
		{
			((Pyramid*)mpActiveObject.get())->setLength(value);
		}
		else if (mpActiveObject->getObjectType().compare("Chessboard") == 0)
		{
			((Chessboard*)mpActiveObject.get())->setLength(value);
		}
		else if (mpActiveObject->getObjectType().compare("Ellipse") == 0)
		{
			((Ellipse*)mpActiveObject.get())->setLength(value);
		}
	}
}

void SceneWidget::setWidth(double value)
{
	if(!mSwitchingSelectedObject)
	{
		if (mpActiveObject->getObjectType().compare("Block") == 0)
		{
			((Block*)mpActiveObject.get())->setWidth(value);
		}
		else if (mpActiveObject->getObjectType().compare("Pyramid") == 0)
		{
			((Pyramid*)mpActiveObject.get())->setWidth(value);
		}
		else if (mpActiveObject->getObjectType().compare("Chessboard") == 0)
		{
			((Chessboard*)mpActiveObject.get())->setWidth(value);
		}
		else if (mpActiveObject->getObjectType().compare("Ellipse") == 0)
		{
			((Ellipse*)mpActiveObject.get())->setWidth(value);
		}
	}
}

void SceneWidget::setHeight(double value)
{
	if(!mSwitchingSelectedObject)
	{
		if (mpActiveObject->getObjectType().compare("Cylinder") == 0)
		{
			((Cylinder*)mpActiveObject.get())->setHeight(value);
		}
		else if (mpActiveObject->getObjectType().compare("Block") == 0)
		{
			((Block*)mpActiveObject.get())->setHeight(value);
		}
		else if (mpActiveObject->getObjectType().compare("Cone") == 0)
		{
			((Cone*)mpActiveObject.get())->setHeight(value);
		}
		else if (mpActiveObject->getObjectType().compare("Pyramid") == 0)
		{
			((Pyramid*)mpActiveObject.get())->setHeight(value);
		}
		else if (mpActiveObject->getObjectType().compare("Chessboard") == 0)
		{
			((Chessboard*)mpActiveObject.get())->setHeight(value);
		}
	}
}

void SceneWidget::setNumberOfRows(int value)
{
	if(!mSwitchingSelectedObject)
	{
		if (mpActiveObject->getObjectType().compare("Chessboard") == 0)
		{
			((Chessboard*)mpActiveObject.get())->setNumberOfRows(value);
		}
	}
}

void SceneWidget::setNumberOfColumns(int value)
{
	if(!mSwitchingSelectedObject)
	{
		if (mpActiveObject->getObjectType().compare("Chessboard") == 0)
		{
			((Chessboard*)mpActiveObject.get())->setNumberOfColumns(value);
		}
	}
}

void SceneWidget::createObject()
{
  // create the new object
  cedar::aux::ObjectPtr p_object(new cedar::aux::Object());
  p_object->setName(mpLineEditName->text().simplified().toStdString());
  cedar::aux::gl::ObjectPtr p_gl_object;
  if(mpComboBoxType->currentText().compare("Cylinder") == 0)
  {
    p_gl_object = cedar::aux::gl::ObjectPtr(new Cylinder(p_object));
  }
  if(mpComboBoxType->currentText().compare("Cylinder") == 0)
  {
    p_gl_object = cedar::aux::gl::ObjectPtr(new Cylinder(p_object));
  }
  else if (mpComboBoxType->currentText().compare("Sphere") == 0)
  {
    p_gl_object = cedar::aux::gl::ObjectPtr(new Sphere(p_object));
  }
  else if (mpComboBoxType->currentText().compare("Block") == 0)
  {
    p_gl_object = cedar::aux::gl::ObjectPtr(new Block(p_object));
  }
  else if (mpComboBoxType->currentText().compare("Cone") == 0)
  {
    p_gl_object = cedar::aux::gl::ObjectPtr(new Cone(p_object));
  }
  else if (mpComboBoxType->currentText().compare("Pyramid") == 0)
  {
    p_gl_object = cedar::aux::gl::ObjectPtr(new Pyramid(p_object));
  }
  else if (mpComboBoxType->currentText().compare("Chessboard") == 0)
  {
    p_gl_object = cedar::aux::gl::ObjectPtr(new Chessboard(p_object));
  }
  else if(mpComboBoxType->currentText().compare("Torus") == 0)
  {
    p_gl_object = cedar::aux::gl::ObjectPtr(new Torus(p_object));
  }
  else if(mpComboBoxType->currentText().compare("Ellipse") == 0)
  {
    p_gl_object = cedar::aux::gl::ObjectPtr(new Ellipse(p_object));
  }
  else if(mpComboBoxType->currentText().compare("Prism") == 0)
  {
    p_gl_object = cedar::aux::gl::ObjectPtr(new Prism(p_object));
  }

  // add the new object to the scene and the combo box
  mpScene->addObject(p_gl_object);
  mpComboBoxName->addItem(QString(mpLineEditName->text()));
  mpComboBoxName->setCurrentIndex(mpComboBoxName->count() - 1);
  mpLineEditName->setText("");
  mpObjectSettingsBox->setEnabled(true);
}

void SceneWidget::deleteObject()
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

void SceneWidget::deleteAllObjects()
{
  mpScene->clear();
  mpComboBoxName->clear();
  mpActiveObject = cedar::aux::gl::ObjectPtr();
  mpObjectSettingsBox->setEnabled(false);
}

void SceneWidget::setActiveObject()
{
  if (!mpScene->isEmpty())
  {
    mpActiveObject = mpScene->getObject(mpComboBoxName->currentIndex());
    updateWidgetObjectParameters();
  }
}

void SceneWidget::updateWidgetObjectParameters()
{
  mSwitchingSelectedObject = true;

  // general parameters
  mpDoubleSpinBoxPositionX->setValue(mpActiveObject->getObject()->getPositionX());
  mpDoubleSpinBoxPositionY->setValue(mpActiveObject->getObject()->getPositionY());
  mpDoubleSpinBoxPositionZ->setValue(mpActiveObject->getObject()->getPositionZ());
  mpDoubleSpinBoxRotation0->setValue(mpActiveObject->getObject()->getOrientationQuaternion(0));
  mpDoubleSpinBoxRotation1->setValue(mpActiveObject->getObject()->getOrientationQuaternion(1));
  mpDoubleSpinBoxRotation2->setValue(mpActiveObject->getObject()->getOrientationQuaternion(2));
  mpDoubleSpinBoxRotation3->setValue(mpActiveObject->getObject()->getOrientationQuaternion(3));
  mpDoubleSpinBoxColorR->setValue(mpActiveObject->colorR());
  mpDoubleSpinBoxColorG->setValue(mpActiveObject->colorG());
  mpDoubleSpinBoxColorB->setValue(mpActiveObject->colorB());
  mpWireFrameCheckBox->setChecked(mpActiveObject->isDrawnAsWireFrame());

  // disable all elements
  mpDoubleSpinBoxLength->setEnabled(false);
  mpDoubleSpinBoxWidth->setEnabled(false);
  mpDoubleSpinBoxHeight->setEnabled(false);
  mpDoubleSpinBoxRadius->setEnabled(false);
  mpDoubleSpinBoxThickness->setEnabled(false);
  mpSpinBoxChessboardRows->setEnabled(false);
  mpSpinBoxChessboardColumns->setEnabled(false);
  mpDoubleSpinBoxSecondColorR->setEnabled(false);
  mpDoubleSpinBoxSecondColorG->setEnabled(false);
  mpDoubleSpinBoxSecondColorB->setEnabled(false);

  if (mpActiveObject->getObjectType().compare("Cylinder") == 0)
  {
    // enable those elements that apply
    mpDoubleSpinBoxRadius->setEnabled(true);
    mpDoubleSpinBoxHeight->setEnabled(true);
    // set values
    mpDoubleSpinBoxHeight->setValue(((Cylinder*)mpActiveObject.get())->height());
    mpDoubleSpinBoxRadius->setValue(((Cylinder*)mpActiveObject.get())->radius());
  }
  else if (mpActiveObject->getObjectType().compare("Sphere") == 0)
  {
    mpDoubleSpinBoxRadius->setEnabled(true);
    mpDoubleSpinBoxRadius->setValue(((Sphere*)mpActiveObject.get())->radius());
  }
  else if (mpActiveObject->getObjectType().compare("Block") == 0)
  {
    mpDoubleSpinBoxLength->setEnabled(true);
    mpDoubleSpinBoxWidth->setEnabled(true);
    mpDoubleSpinBoxHeight->setEnabled(true);
    mpDoubleSpinBoxHeight->setValue(((Block*)mpActiveObject.get())->height());
    mpDoubleSpinBoxWidth->setValue(((Block*)mpActiveObject.get())->width());
    mpDoubleSpinBoxLength->setValue(((Block*)mpActiveObject.get())->length());
  }
  else if (mpActiveObject->getObjectType().compare("Cone") == 0)
  {
    mpDoubleSpinBoxRadius->setEnabled(true);
    mpDoubleSpinBoxHeight->setEnabled(true);
    mpDoubleSpinBoxHeight->setValue(((Cone*)mpActiveObject.get())->height());
    mpDoubleSpinBoxRadius->setValue(((Cone*)mpActiveObject.get())->radius());
  }
  else if (mpActiveObject->getObjectType().compare("Pyramid") == 0)
  {
    mpDoubleSpinBoxLength->setEnabled(true);
    mpDoubleSpinBoxWidth->setEnabled(true);
    mpDoubleSpinBoxHeight->setEnabled(true);

    mpDoubleSpinBoxHeight->setValue(((Block*)mpActiveObject.get())->height());
    mpDoubleSpinBoxWidth->setValue(((Block*)mpActiveObject.get())->width());
    mpDoubleSpinBoxLength->setValue(((Block*)mpActiveObject.get())->length());
  }
  else if (mpActiveObject->getObjectType().compare("Chessboard") == 0)
  {
    mpDoubleSpinBoxLength->setEnabled(true);
    mpDoubleSpinBoxWidth->setEnabled(true);
    mpDoubleSpinBoxHeight->setEnabled(true);
    mpSpinBoxChessboardRows->setEnabled(true);
    mpSpinBoxChessboardColumns->setEnabled(true);
    mpDoubleSpinBoxSecondColorR->setEnabled(true);
    mpDoubleSpinBoxSecondColorG->setEnabled(true);
    mpDoubleSpinBoxSecondColorB->setEnabled(true);

    mpDoubleSpinBoxHeight->setValue(((Chessboard*)mpActiveObject.get())->height());
    mpDoubleSpinBoxWidth->setValue(((Chessboard*)mpActiveObject.get())->width());
    mpDoubleSpinBoxLength->setValue(((Chessboard*)mpActiveObject.get())->length());
    mpSpinBoxChessboardRows->setValue(((Chessboard*)mpActiveObject.get())->numberOfRows());
    mpSpinBoxChessboardColumns->setValue(((Chessboard*)mpActiveObject.get())->numberOfColumns());
    mpDoubleSpinBoxSecondColorR->setValue(((Chessboard*)mpActiveObject.get())->secondColorR());
    mpDoubleSpinBoxSecondColorG->setValue(((Chessboard*)mpActiveObject.get())->secondColorG());
    mpDoubleSpinBoxSecondColorB->setValue(((Chessboard*)mpActiveObject.get())->secondColorB());
  }
  else if (mpActiveObject->getObjectType().compare("Torus") == 0)
  {
    mpDoubleSpinBoxRadius->setEnabled(true);
    mpDoubleSpinBoxThickness->setEnabled(true);
    mpDoubleSpinBoxRadius->setValue(((Torus*)mpActiveObject.get())->radius());
    mpDoubleSpinBoxThickness->setValue(((Torus*)mpActiveObject.get())->thickness());
  }
  else if (mpActiveObject->getObjectType().compare("Ellipse") == 0)
  {
    mpDoubleSpinBoxThickness->setEnabled(true);
    mpDoubleSpinBoxLength->setEnabled(true);
    mpDoubleSpinBoxWidth->setEnabled(true);
    mpDoubleSpinBoxThickness->setValue(((Ellipse*)mpActiveObject.get())->thickness());
    mpDoubleSpinBoxLength->setValue(((Ellipse*)mpActiveObject.get())->length());
    mpDoubleSpinBoxWidth->setValue(((Ellipse*)mpActiveObject.get())->width());
  }

  mSwitchingSelectedObject = false;
}

void SceneWidget::init()
{
  // fill combo box with names of objects in the scene
  for (int i=0; i<mpScene->numberOfObjects(); i++)
  {
    mpComboBoxName->addItem(QString(mpScene->getObject(i)->getObjectName().c_str()));
  }
  
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
	connect(mpWireFrameCheckBox, SIGNAL(stateChanged(int)), this, SLOT(setWireFrame(int)));
	connect(mpDoubleSpinBoxPositionX, SIGNAL(valueChanged(double)), this, SLOT(setPosition()));
	connect(mpDoubleSpinBoxPositionY, SIGNAL(valueChanged(double)), this, SLOT(setPosition()));
	connect(mpDoubleSpinBoxPositionZ, SIGNAL(valueChanged(double)), this, SLOT(setPosition()));
  connect(mpRotateXSpinBox, SIGNAL(valueChanged(double)), this, SLOT(rotate()));
  connect(mpRotateYSpinBox, SIGNAL(valueChanged(double)), this, SLOT(rotate()));
  connect(mpRotateZSpinBox, SIGNAL(valueChanged(double)), this, SLOT(rotate()));
	connect(mpDoubleSpinBoxRotation0, SIGNAL(valueChanged(double)), this, SLOT(setOrientationQuaternion()));
  connect(mpDoubleSpinBoxRotation1, SIGNAL(valueChanged(double)), this, SLOT(setOrientationQuaternion()));
  connect(mpDoubleSpinBoxRotation2, SIGNAL(valueChanged(double)), this, SLOT(setOrientationQuaternion()));
  connect(mpDoubleSpinBoxRotation3, SIGNAL(valueChanged(double)), this, SLOT(setOrientationQuaternion()));
	connect(mpDoubleSpinBoxColorR, SIGNAL(valueChanged(double)), this, SLOT(setColor()));
	connect(mpDoubleSpinBoxColorG, SIGNAL(valueChanged(double)), this, SLOT(setColor()));
	connect(mpDoubleSpinBoxColorB, SIGNAL(valueChanged(double)), this, SLOT(setColor()));
	connect(mpDoubleSpinBoxSecondColorR, SIGNAL(valueChanged(double)), this, SLOT(setSecondColor()));
	connect(mpDoubleSpinBoxSecondColorG, SIGNAL(valueChanged(double)), this, SLOT(setSecondColor()));
	connect(mpDoubleSpinBoxSecondColorB, SIGNAL(valueChanged(double)), this, SLOT(setSecondColor()));
	connect(mpDoubleSpinBoxRadius, SIGNAL(valueChanged(double)), this, SLOT(setRadius(double)));
	connect(mpDoubleSpinBoxThickness, SIGNAL(valueChanged(double)), this, SLOT(setThickness(double)));
	connect(mpDoubleSpinBoxLength, SIGNAL(valueChanged(double)), this, SLOT(setLength(double)));
	connect(mpDoubleSpinBoxWidth, SIGNAL(valueChanged(double)), this, SLOT(setWidth(double)));
	connect(mpDoubleSpinBoxHeight, SIGNAL(valueChanged(double)), this, SLOT(setHeight(double)));
	connect(mpSpinBoxChessboardRows, SIGNAL(valueChanged(int)), this, SLOT(setNumberOfRows(int)));
	connect(mpSpinBoxChessboardColumns, SIGNAL(valueChanged(int)), this, SLOT(setNumberOfColumns(int)));
  
	// Buttons
	connect(mpPushButtonCreateObject, SIGNAL(pressed()), this, SLOT(createObject()));
	connect(mpPushButtonDeleteAllObjects, SIGNAL(pressed()), this, SLOT(deleteAllObjects()));
	connect(mpPushButtonDeleteObject, SIGNAL(pressed()), this, SLOT(deleteObject()));
  
  mpDoubleSpinBoxPositionX->setRange(-mpScene->getSceneLimit(), mpScene->getSceneLimit());
  mpDoubleSpinBoxPositionX->setSingleStep(mpScene->getSceneLimit()*0.1);
  mpDoubleSpinBoxPositionY->setRange(-mpScene->getSceneLimit(), mpScene->getSceneLimit());
  mpDoubleSpinBoxPositionY->setSingleStep(mpScene->getSceneLimit()*0.1);
  mpDoubleSpinBoxPositionZ->setRange(-mpScene->getSceneLimit(), mpScene->getSceneLimit());
  mpDoubleSpinBoxPositionZ->setSingleStep(mpScene->getSceneLimit()*0.1);
  
}





