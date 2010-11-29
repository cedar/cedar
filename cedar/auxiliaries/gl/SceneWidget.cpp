/*----------------------------------------------------------------------------------------------------------------------
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

SceneWidget::SceneWidget(Scene* pScene,	QWidget*)
{
	setupUi(this);
	init(pScene);
}

SceneWidget::~SceneWidget()
{

}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void SceneWidget::updateWidgetObjectParameters()
{
	mSwitchingSelectedObject = true;
  
	// general parameters
	mpDoubleSpinBoxPositionX->setValue(mpActiveObject->positionX());
	mpDoubleSpinBoxPositionY->setValue(mpActiveObject->positionY());
	mpDoubleSpinBoxPositionZ->setValue(mpActiveObject->positionZ());
	mpDoubleSpinBoxRotationX->setValue(mpActiveObject->orientationAngleAlpha());
	mpDoubleSpinBoxRotationY->setValue(mpActiveObject->orientationAngleBeta());
	mpDoubleSpinBoxRotationZ->setValue(mpActiveObject->orientationAngleGamma());
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
  
	if (mpActiveObject->objectType().compare("Cylinder") == 0)
	{
		// enable those elements that apply
		mpDoubleSpinBoxRadius->setEnabled(true);
		mpDoubleSpinBoxHeight->setEnabled(true);
		// set values
		mpDoubleSpinBoxHeight->setValue(((Cylinder*)mpActiveObject)->height());
		mpDoubleSpinBoxRadius->setValue(((Cylinder*)mpActiveObject)->radius());
	}
	else if (mpActiveObject->objectType().compare("Sphere") == 0)
	{
		mpDoubleSpinBoxRadius->setEnabled(true);
		mpDoubleSpinBoxRadius->setValue(((Sphere*)mpActiveObject)->radius());
	}
	else if (mpActiveObject->objectType().compare("Block") == 0)
	{
		mpDoubleSpinBoxLength->setEnabled(true);
		mpDoubleSpinBoxWidth->setEnabled(true);
		mpDoubleSpinBoxHeight->setEnabled(true);
		mpDoubleSpinBoxHeight->setValue(((Block*)mpActiveObject)->height());
		mpDoubleSpinBoxWidth->setValue(((Block*)mpActiveObject)->width());
		mpDoubleSpinBoxLength->setValue(((Block*)mpActiveObject)->length());
	}
	else if (mpActiveObject->objectType().compare("Cone") == 0)
	{
		mpDoubleSpinBoxRadius->setEnabled(true);
		mpDoubleSpinBoxHeight->setEnabled(true);
		mpDoubleSpinBoxHeight->setValue(((Cone*)mpActiveObject)->height());
		mpDoubleSpinBoxRadius->setValue(((Cone*)mpActiveObject)->radius());
	}
	else if (mpActiveObject->objectType().compare("Pyramid") == 0)
	{
		mpDoubleSpinBoxLength->setEnabled(true);
		mpDoubleSpinBoxWidth->setEnabled(true);
		mpDoubleSpinBoxHeight->setEnabled(true);
    
		mpDoubleSpinBoxHeight->setValue(((Block*)mpActiveObject)->height());
		mpDoubleSpinBoxWidth->setValue(((Block*)mpActiveObject)->width());
		mpDoubleSpinBoxLength->setValue(((Block*)mpActiveObject)->length());
	}
	else if (mpActiveObject->objectType().compare("Chessboard") == 0)
	{
		mpDoubleSpinBoxLength->setEnabled(true);
		mpDoubleSpinBoxWidth->setEnabled(true);
		mpDoubleSpinBoxHeight->setEnabled(true);
		mpSpinBoxChessboardRows->setEnabled(true);
		mpSpinBoxChessboardColumns->setEnabled(true);
		mpDoubleSpinBoxSecondColorR->setEnabled(true);
		mpDoubleSpinBoxSecondColorG->setEnabled(true);
		mpDoubleSpinBoxSecondColorB->setEnabled(true);
    
		mpDoubleSpinBoxHeight->setValue(((Chessboard*)mpActiveObject)->height());
		mpDoubleSpinBoxWidth->setValue(((Chessboard*)mpActiveObject)->width());
		mpDoubleSpinBoxLength->setValue(((Chessboard*)mpActiveObject)->length());
		mpSpinBoxChessboardRows->setValue(((Chessboard*)mpActiveObject)->numberOfRows());
		mpSpinBoxChessboardColumns->setValue(((Chessboard*)mpActiveObject)->numberOfColumns());
		mpDoubleSpinBoxSecondColorR->setValue(((Chessboard*)mpActiveObject)->secondColorR());
		mpDoubleSpinBoxSecondColorG->setValue(((Chessboard*)mpActiveObject)->secondColorG());
		mpDoubleSpinBoxSecondColorB->setValue(((Chessboard*)mpActiveObject)->secondColorB());
	}
	else if (mpActiveObject->objectType().compare("Torus") == 0)
	{
		mpDoubleSpinBoxRadius->setEnabled(true);
		mpDoubleSpinBoxThickness->setEnabled(true);
		mpDoubleSpinBoxRadius->setValue(((Torus*)mpActiveObject)->radius());
		mpDoubleSpinBoxThickness->setValue(((Torus*)mpActiveObject)->thickness());
	}
	else if (mpActiveObject->objectType().compare("Ellipse") == 0)
	{
		mpDoubleSpinBoxThickness->setEnabled(true);
		mpDoubleSpinBoxLength->setEnabled(true);
		mpDoubleSpinBoxWidth->setEnabled(true);
		mpDoubleSpinBoxThickness->setValue(((Ellipse*)mpActiveObject)->thickness());
		mpDoubleSpinBoxLength->setValue(((Ellipse*)mpActiveObject)->length());
		mpDoubleSpinBoxWidth->setValue(((Ellipse*)mpActiveObject)->width());
	}
  
	mSwitchingSelectedObject = false;
}

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
		mpActiveObject->setPosition(mpDoubleSpinBoxPositionX->value(),
                                mpDoubleSpinBoxPositionY->value(),
                                mpDoubleSpinBoxPositionZ->value()
                              );
	}
}

void SceneWidget::setOrientationAngles()
{
	if(!mSwitchingSelectedObject)
	{
		mpActiveObject->setOrientationAngles(mpDoubleSpinBoxRotationX->value(),
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
		if (mpActiveObject->objectType().compare("Chessboard") == 0)
		{
			((Chessboard*)mpActiveObject)->setSecondColor(mpDoubleSpinBoxSecondColorR->value(),
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
		if (mpActiveObject->objectType().compare("Cylinder") == 0)
		{
			((Cylinder*)mpActiveObject)->setRadius(value);
		}
		else if (mpActiveObject->objectType().compare("Sphere") == 0)
		{
			((Sphere*)mpActiveObject)->setRadius(value);
		}
		else if (mpActiveObject->objectType().compare("Cone") == 0)
		{
			((Cone*)mpActiveObject)->setRadius(value);
		}
		else if (mpActiveObject->objectType().compare("Torus") == 0)
		{
			((Torus*)mpActiveObject)->setRadius(value);
		}
	}
}

void SceneWidget::setThickness(double value)
{
	if(!mSwitchingSelectedObject)
	{
		if (mpActiveObject->objectType().compare("Torus") == 0)
		{
			((Torus*)mpActiveObject)->setThickness(value);
		}
		else if (mpActiveObject->objectType().compare("Ellipse") == 0)
		{
			((Ellipse*)mpActiveObject)->setThickness(value);
		}
	}
}

void SceneWidget::setLength(double value)
{
	if(!mSwitchingSelectedObject)
	{
		if (mpActiveObject->objectType().compare("Block") == 0)
		{
			((Block*)mpActiveObject)->setLength(value);
		}
		else if (mpActiveObject->objectType().compare("Pyramid") == 0)
		{
			((Pyramid*)mpActiveObject)->setLength(value);
		}
		else if (mpActiveObject->objectType().compare("Chessboard") == 0)
		{
			((Chessboard*)mpActiveObject)->setLength(value);
		}
		else if (mpActiveObject->objectType().compare("Ellipse") == 0)
		{
			((Ellipse*)mpActiveObject)->setLength(value);
		}
	}
}

void SceneWidget::setWidth(double value)
{
	if(!mSwitchingSelectedObject)
	{
		if (mpActiveObject->objectType().compare("Block") == 0)
		{
			((Block*)mpActiveObject)->setWidth(value);
		}
		else if (mpActiveObject->objectType().compare("Pyramid") == 0)
		{
			((Pyramid*)mpActiveObject)->setWidth(value);
		}
		else if (mpActiveObject->objectType().compare("Chessboard") == 0)
		{
			((Chessboard*)mpActiveObject)->setWidth(value);
		}
		else if (mpActiveObject->objectType().compare("Ellipse") == 0)
		{
			((Ellipse*)mpActiveObject)->setWidth(value);
		}
	}
}

void SceneWidget::setHeight(double value)
{
	if(!mSwitchingSelectedObject) {
		if (mpActiveObject->objectType().compare("Cylinder") == 0)
		{
			((Cylinder*)mpActiveObject)->setHeight(value);
		}
		else if (mpActiveObject->objectType().compare("Block") == 0)
		{
			((Block*)mpActiveObject)->setHeight(value);
		}
		else if (mpActiveObject->objectType().compare("Cone") == 0)
		{
			((Cone*)mpActiveObject)->setHeight(value);
		}
		else if (mpActiveObject->objectType().compare("Pyramid") == 0)
		{
			((Pyramid*)mpActiveObject)->setHeight(value);
		}
		else if (mpActiveObject->objectType().compare("Chessboard") == 0)
		{
			((Chessboard*)mpActiveObject)->setHeight(value);
		}

	}
}

void SceneWidget::setNumberOfRows(int value)
{
	if(!mSwitchingSelectedObject)
	{
		if (mpActiveObject->objectType().compare("Chessboard") == 0)
		{
			((Chessboard*)mpActiveObject)->setNumberOfRows(value);
		}
	}
}

void SceneWidget::setNumberOfColumns(int value)
{
	if(!mSwitchingSelectedObject)
	{
		if (mpActiveObject->objectType().compare("Chessboard") == 0)
		{
			((Chessboard*)mpActiveObject)->setNumberOfColumns(value);
		}
	}
}

void SceneWidget::createObject()
{
  // create the new object
  Object* newObject;
  if(mpComboBoxType->currentText().compare("Cylinder") == 0)
  {
    newObject = new Cylinder(mpLineEditName->text().simplified().toStdString());
  }
  else if (mpComboBoxType->currentText().compare("Sphere") == 0)
  {
    newObject = new Sphere(mpLineEditName->text().simplified().toStdString());
  }
  else if (mpComboBoxType->currentText().compare("Block") == 0)
  {
    newObject = new Block(mpLineEditName->text().simplified().toStdString());
  }
  else if (mpComboBoxType->currentText().compare("Cone") == 0)
  {
    newObject = new Cone(mpLineEditName->text().simplified().toStdString());
  }
  else if (mpComboBoxType->currentText().compare("Pyramid") == 0)
  {
    newObject = new Pyramid(mpLineEditName->text().simplified().toStdString());
  }
  else if (mpComboBoxType->currentText().compare("Chessboard") == 0)
  {
    newObject = new Chessboard(mpLineEditName->text().simplified().toStdString());
  }
  else if(mpComboBoxType->currentText().compare("Torus") == 0)
  {
    newObject = new Torus(mpLineEditName->text().simplified().toStdString());
  }
  else if(mpComboBoxType->currentText().compare("Ellipse") == 0)
  {
    newObject = new Ellipse(mpLineEditName->text().simplified().toStdString());
  }
  else if(mpComboBoxType->currentText().compare("Prism") == 0)
  {
    newObject = new Prism(mpLineEditName->text().simplified().toStdString());
  }
  
  // try to add it to the scene and the combo box
  if (mpScene->addObject(newObject))
  {
    mpComboBoxName->addItem(QString(mpLineEditName->text()));
    mpComboBoxName->setCurrentIndex(mpComboBoxName->count() - 1);
    mpLineEditName->setText("");
    mpObjectSettingsBox->setEnabled(true);
  }
  else
  {
    delete newObject;
  }
}

void SceneWidget::deleteAllObjects()
{
  mpScene->clear();
  mpComboBoxName->clear();
  mpActiveObject = NULL;
  mpObjectSettingsBox->setEnabled(false);
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
		mpActiveObject = NULL;
		mpObjectSettingsBox->setEnabled(false);
	}
}

void SceneWidget::setActiveObject()
{
  if (mpScene->isEmpty())
  {
    mpActiveObject = NULL;
  }
  else
  {
    mpActiveObject = mpScene->object(mpComboBoxName->currentIndex());
    updateWidgetObjectParameters();
  }
}

void SceneWidget::init(Scene* pScene)
{
	mpScene = pScene;
  // fill combo box with names of objects in the scene
  for (int i=0; i<mpScene->numberOfObjects(); i++)
  {
    mpComboBoxName->addItem(QString(mpScene->object(i)->objectName().c_str()));
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





