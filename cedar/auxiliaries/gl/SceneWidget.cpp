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

#include "SceneWidget.h"

using namespace cedar::aux::gl;
using namespace std;
using namespace cv;

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

SceneWidget::SceneWidget(QWidget*)
{
  // TODO: this is the constructor for an empty scene. Implement accordingly.
	setupUi(this);
}

SceneWidget::SceneWidget(cedar::aux::gl::ScenePtr p_scene,	QWidget*)
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
		mpActiveObject->getObject()->setPosition(mpDoubleSpinBoxPositionX->value(),
                                mpDoubleSpinBoxPositionY->value(),
                                mpDoubleSpinBoxPositionZ->value()
                              );
	}
}

void SceneWidget::setOrientationAngles()
{
	if(!mSwitchingSelectedObject)
	{
		mpActiveObject->getObject()->setOrientationAngles(mpDoubleSpinBoxRotationX->value(),
                                         mpDoubleSpinBoxRotationY->value(),
                                         mpDoubleSpinBoxRotationZ->value()
                                       );
	}
}

void SceneWidget::setColor()
{
	if(!mSwitchingSelectedObject)
	{
		mpActiveObject->setColor(mpDoubleSpinBoxColorR->value(),
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
      ((Chessboard*)mpActiveObject.get())->setSecondColor(mpDoubleSpinBoxSecondColorR->value(),
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
	if(!mSwitchingSelectedObject) {
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
//  // create the new object
//  Object* newObject;
//  if(mpComboBoxType->currentText().compare("Cylinder") == 0)
//  {
//    newObject = new Cylinder(mpLineEditName->text().simplified().toStdString());
//  }
//  else if (mpComboBoxType->currentText().compare("Sphere") == 0)
//  {
//    newObject = new Sphere(mpLineEditName->text().simplified().toStdString());
//  }
//  else if (mpComboBoxType->currentText().compare("Block") == 0)
//  {
//    newObject = new Block(mpLineEditName->text().simplified().toStdString());
//  }
//  else if (mpComboBoxType->currentText().compare("Cone") == 0)
//  {
//    newObject = new Cone(mpLineEditName->text().simplified().toStdString());
//  }
//  else if (mpComboBoxType->currentText().compare("Pyramid") == 0)
//  {
//    newObject = new Pyramid(mpLineEditName->text().simplified().toStdString());
//  }
//  else if (mpComboBoxType->currentText().compare("Chessboard") == 0)
//  {
//    newObject = new Chessboard(mpLineEditName->text().simplified().toStdString());
//  }
//  else if(mpComboBoxType->currentText().compare("Torus") == 0)
//  {
//    newObject = new Torus(mpLineEditName->text().simplified().toStdString());
//  }
//  else if(mpComboBoxType->currentText().compare("Ellipse") == 0)
//  {
//    newObject = new Ellipse(mpLineEditName->text().simplified().toStdString());
//  }
//  else if(mpComboBoxType->currentText().compare("Prism") == 0)
//  {
//    newObject = new Prism(mpLineEditName->text().simplified().toStdString());
//  }
//
//  // try to add it to the scene and the combo box
//  if (mpScene->addObject(newObject))
//  {
//    mpComboBoxName->addItem(QString(mpLineEditName->text()));
//    mpComboBoxName->setCurrentIndex(mpComboBoxName->count() - 1);
//    mpLineEditName->setText("");
//    mpObjectSettingsBox->setEnabled(true);
//  }
//  else
//  {
//    delete newObject;
//  }
}

void SceneWidget::deleteAllObjects()
{
//  mpScene->clear();
//  mpComboBoxName->clear();
//  mpActiveObject = NULL;
//  mpObjectSettingsBox->setEnabled(false);
}

void SceneWidget::deleteObject()
{
//  // remove from scene
//  mpScene->deleteObject(mpComboBoxName->currentIndex());
//
//  // remove from comboBox
//  mpComboBoxName->removeItem(mpComboBoxName->currentIndex());
//
//  // if this was the last one
//	if(mpScene->numberOfObjects() == 0)
//	{
//		mpActiveObject = NULL;
//		mpObjectSettingsBox->setEnabled(false);
//	}
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
  mpDoubleSpinBoxRotationX->setValue(mpActiveObject->getObject()->getOrientationAngleAlpha());
  mpDoubleSpinBoxRotationY->setValue(mpActiveObject->getObject()->getOrientationAngleBeta());
  mpDoubleSpinBoxRotationZ->setValue(mpActiveObject->getObject()->getOrientationAngleGamma());
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
	connect(mpDoubleSpinBoxRotationX, SIGNAL(valueChanged(double)), this, SLOT(setOrientationAngles()));
	connect(mpDoubleSpinBoxRotationY, SIGNAL(valueChanged(double)), this, SLOT(setOrientationAngles()));
	connect(mpDoubleSpinBoxRotationZ, SIGNAL(valueChanged(double)), this, SLOT(setOrientationAngles()));
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
  
	mpDoubleSpinBoxPositionX->setMaximum(mpScene->sceneLimit());
	mpDoubleSpinBoxPositionY->setMaximum(mpScene->sceneLimit());
	mpDoubleSpinBoxPositionZ->setMaximum(mpScene->sceneLimit());
	mpDoubleSpinBoxPositionX->setMinimum(-mpScene->sceneLimit());
	mpDoubleSpinBoxPositionY->setMinimum(-mpScene->sceneLimit());
	mpDoubleSpinBoxPositionZ->setMinimum(-mpScene->sceneLimit());
  
}





