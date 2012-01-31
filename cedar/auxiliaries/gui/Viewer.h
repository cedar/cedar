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

    File:        Viewer.h

    Maintainer:  Hendrik Reimann
    Email:       hendrik.reimann@ini.rub.de
    Date:        2010 10 28

    Description: Simple viewer for visualizing a scene of objects

    Credits:

======================================================================================================================*/

#ifndef CEDAR_AUX_GUI_VIEWER_H
#define CEDAR_AUX_GUI_VIEWER_H

// CEDAR INCLUDES
#include "cedar/auxiliaries/gui/namespace.h"
#include "cedar/auxiliaries/gl/namespace.h"
#include "cedar/auxiliaries/gl/Scene.h"

// SYSTEM INCLUDES
#include <QGLViewer/qglviewer.h>
#include <QList>

/*!@brief A simple viewer for OpenGL drawing routines, based on QGLViewer
 *
 * Use this class to draw instances of classes inherited from cedar::aux::gl::Object. This viewer has freely movable
 * camera position and other perks, coming from QGLViewer.
 *
 * @remarks To visualize an object, add it to an instance of cedar::aux::gl::Scene, then create a Viewer for this Scene
 *
 */
class cedar::aux::gui::Viewer : public QGLViewer
{
private:
  Q_OBJECT
  
public:
  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
  /*!@brief the constructor */
  Viewer(cedar::aux::gl::ScenePtr pScene);

  /*!@brief the constructor */
  ~Viewer();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  /*!@brief draws all objects in the scene */
  void draw();

  /*!@brief function being called automatically when a timer is up, usually in a loop */
  void timerEvent(QTimerEvent* pEvent);

  //!@brief call this function to initialize Gl resources for the passed visualization object
  //!@todo why does this function just calls another function at pVisualization?
  void initGl(cedar::aux::gl::RigidBodyVisualizationPtr pVisualization);
  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  /*!@brief initialization */
  void init();
  
  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
  cedar::aux::gl::ScenePtr const mpScene;
};
#endif  // CEDAR_AUX_GUI_VIEWER_H
