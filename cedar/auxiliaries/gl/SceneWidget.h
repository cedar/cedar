/*----------------------------------------------------------------------------------------------------------------------
 ----- Institute:   Ruhr-Universitaet-Bochum
                    Institut fuer Neuroinformatik
 
 ----- File:        SceneWidget.h
 
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

#ifndef CEDAR_AUX_GL_SCENE_WIDGET_H
#define CEDAR_AUX_GL_SCENE_WIDGET_H

// SYSTEM INCLUDES
#include <Qt>

// PROJECT INCLUDES
#include "build/cedar/auxiliaries/ui_SceneWidget.h"

// LOCAL INCLUDES
#include "namespace.h"
#include "Scene.h"
#include "Object.h"
#include "Block.h"
#include "Sphere.h"
#include "Cone.h"
#include "Cylinder.h"
#include "Pyramid.h"
#include "Prism.h"
#include "Torus.h"
#include "Ellipse.h"
#include "Chessboard.h"


class cedar::aux::gl::SceneWidget : public QWidget, private Ui_SceneWidget
{
private:

	Q_OBJECT;

public:
  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
  /*!@brief the constructor */
	SceneWidget(QWidget* parent=0);

  /*!@brief constructor with parent widget */
	SceneWidget(Scene* pScene, QWidget* parent=0);
  
  /*!@brief the destructor */
  ~SceneWidget();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  /*!@brief updates the parameters displayed in the widget */
	void updateWidgetObjectParameters();
  
public slots:
  /*!@brief sets the wire frame state of the current object according to the widget control */
	void setWireFrame(int state);

  /*!@brief sets the position of the current object according to the widget control */
	void setPosition();
	
  /*!@brief sets the orientation of the current object according to the widget control */
  void setOrientationAngles();
	
  /*!@brief sets the color of the current object according to the widget control */
  void setColor();
	
  /*!@brief sets the second color of the current object according to the widget control */
  void setSecondColor();
	
  /*!@brief sets the length of the current object
   * @param value    new length
   */
  void setLength(double value);
	
  /*!@brief sets the width of the current object according to the widget control
   * @param value    new width
   */
  void setWidth(double value);
	
  /*!@brief sets the height of the current object according to the widget control
   * @param value    new height
   */
  void setHeight(double value);
	
  /*!@brief sets the radius of the current object according to the widget control
   * @param value    new radius
   */
  void setRadius(double value);
	
  /*!@brief sets the thickness of the current object according to the widget control
   * @param value    new thickness
   */
  void setThickness(double value);
	
  /*!@brief sets the number of rows of the current object according to the widget control
   * @param value    new number of rows
   */
  void setNumberOfRows(int value);
	
  /*!@brief sets the number of columns of the current object according to the widget control
   * @param value    new number of columns
   */
  void setNumberOfColumns(int value);
	
  /*!@brief creates an object with type and name specified by the widget controls */
  void createObject();
	
  /*!@brief deletes the currently selected object */
  void deleteObject();
	
  /*!@brief deletes all objects in the scene */
  void deleteAllObjects();
	
  /*!@brief set the current object according to combo box status */
  void setActiveObject();
  
  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
	void init(Scene* pScene);

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
private:
  //! pointer to the scene being displayed
	cedar::aux::gl::Scene* mpScene;

	// pointer to the currently selected object
  cedar::aux::gl::Object* mpActiveObject;

  //! checks whether the widget is currently being changed due to a switch in the selected object
	bool mSwitchingSelectedObject;
};

#endif // CEDAR_AUX_GL_SCENE_WIDGET_H
