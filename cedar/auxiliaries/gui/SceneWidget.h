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

    File:        SceneWidget.h

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

#ifndef CEDAR_AUX_GUI_SCENE_WIDGET_H
#define CEDAR_AUX_GUI_SCENE_WIDGET_H

// CEDAR INCLUDES
#include "cedar/auxiliaries/gui/namespace.h"
#include "cedar/auxiliaries/gl/Scene.h"
#include "cedar/auxiliaries/gl/Object.h"
#include "cedar/auxiliaries/gl/Block.h"
#include "cedar/auxiliaries/gl/Sphere.h"
#include "cedar/auxiliaries/gl/Cone.h"
#include "cedar/auxiliaries/gl/Cylinder.h"
#include "cedar/auxiliaries/gl/Pyramid.h"
#include "cedar/auxiliaries/gl/Prism.h"
#include "cedar/auxiliaries/gl/Torus.h"
#include "cedar/auxiliaries/gl/Ellipse.h"
#include "cedar/auxiliaries/gl/Chessboard.h"
#include "cedar/auxiliaries/gui/BaseWidget.h"
#include "cedar/auxiliaries/gui/RigidBodyWidget.h"
#include "cedar/auxiliaries/gui/RigidBodyVisualizationWidget.h"
#include "cedar/auxiliaries/gui/ui_SceneWidget.h"

// SYSTEM INCLUDES


/*!@brief Widget to observe and change a scene of geometric objects
 *
 * The widget gives a list of all Objects currently in the scene. The dimensions and other attributes of each
 * object can be changed by spin boxes. Objects can be deleted from and added to the scene.
 *
 * @remarks Not all functionalities are implemented yet.
 */
class cedar::aux::gui::SceneWidget : public cedar::aux::gui::BaseWidget, private Ui_SceneWidget
{
private:
  Q_OBJECT

public:
  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
  /*!@brief constructor with parent widget */
  SceneWidget(cedar::aux::gl::ScenePtr p_scene, QWidget* parent=0);
  
  /*!@brief the destructor */
  ~SceneWidget();

  //--------------------------------------------------------------------------------------------------------------------
  // slots
  //--------------------------------------------------------------------------------------------------------------------
public slots:
  /*!@brief sets the wire frame state of the current object according to the widget control */
  void setWireFrame(int state);

  /*!@brief sets the position of the current object according to the widget control */
//  void setPosition();

  /*!@brief sets the orientation of the current object according to the widget control */
//  void setOrientationQuaternion();

  /*!@brief rotates the currently active object by the values specified in the rotation boxes */
//  void rotate();

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
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  /*!@brief updates the parameters displayed in the widget */
  void updateWidgetObjectParameters();
  
  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  void init();

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
private:
  //! pointer to the scene being displayed
  cedar::aux::gl::ScenePtr mpScene;

  // pointer to the currently selected object
  cedar::aux::gl::ObjectPtr mpActiveObject;

  // pointer to the RigidBodyVisualizationWidget
  cedar::aux::gui::RigidBodyVisualizationWidget* mpRigidBodyVisualizationWidget;

  // pointer to the RigidBodyWidget
  cedar::aux::gui::RigidBodyWidget* mpRigidBodyWidget;

  //! checks whether the widget is currently being changed due to a switch in the selected object
  bool mSwitchingSelectedObject;
};

#endif // CEDAR_AUX_GUI_SCENE_WIDGET_H
