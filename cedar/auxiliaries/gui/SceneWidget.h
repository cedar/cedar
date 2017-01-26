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

    File:        SceneWidget.h

    Maintainer:  Hendrik Reimann
    Email:       hendrik.reimann@ini.rub.de
    Date:        2010 11 27

    Description: manages a cedar::aux::gl::Scene of cedar::aux::gl::ObjectVisualizations

    Credits: initially designed by Denis Hakenberg

======================================================================================================================*/

#ifndef CEDAR_AUX_GUI_SCENE_WIDGET_H
#define CEDAR_AUX_GUI_SCENE_WIDGET_H

// CEDAR INCLUDES
#include "cedar/auxiliaries/gl/Scene.h"
#include "cedar/auxiliaries/gl/ObjectVisualization.h"
#include "cedar/auxiliaries/gui/BaseWidget.h"
#include "cedar/auxiliaries/gui/LocalCoordinateFrameWidget.h"
#include "cedar/auxiliaries/gui/ObjectVisualizationWidget.h"
#include "cedar/auxiliaries/gui/ui_SceneWidget.h"

// FORWARD DECLARATIONS
#include "cedar/auxiliaries/gui/SceneWidget.fwd.h"

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

  /*!@brief standard constructor to be used by the Qt UI header generator*/
  SceneWidget(QWidget* parent=0);

  /*!@brief constructor with parent widget and scene pointer */
  SceneWidget(cedar::aux::gl::ScenePtr p_scene, QWidget* parent=0);
  
  /*!@brief the destructor */
  ~SceneWidget();

  //--------------------------------------------------------------------------------------------------------------------
  // slots
  //--------------------------------------------------------------------------------------------------------------------
public slots:
  /*!@brief deletes the currently selected object from the scene
   */
  void deleteObject();

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

  /*!@brief set the current object according to combo box status */
  void setActiveVisualization();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  /*!@brief updates the parameters displayed in the widget */
  void updateWidget();
  
  /*!@brief updates the object selection combo box */
  void updateObjectSelectionComboBox();

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
  cedar::aux::gl::ObjectVisualizationPtr mpActiveVisualization;

  // pointer to the ObjectVisualizationWidget
  cedar::aux::gui::ObjectVisualizationWidget* mpObjectVisualizationWidget;

  // pointer to the LocalCoordinateFrameWidget
  cedar::aux::gui::LocalCoordinateFrameWidget* mpLocalCoordinateFrameWidget;

  //! checks whether the widget is currently being changed due to a switch in the selected object
  bool mSwitchingSelectedObject;

  //! Connection to the displayed scene's changed signal.
  boost::signals2::connection mSlotConnection;

};

#endif // CEDAR_AUX_GUI_SCENE_WIDGET_H
