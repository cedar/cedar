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

    File:        Viewer.h

    Maintainer:  Hendrik Reimann
    Email:       hendrik.reimann@ini.rub.de
    Date:        2010 10 28

    Description: Simple viewer for visualizing a scene of objects

    Credits:

======================================================================================================================*/

#ifndef CEDAR_AUX_GUI_VIEWER_H
#define CEDAR_AUX_GUI_VIEWER_H

#include "cedar/configuration.h"


// CEDAR INCLUDES
#include "cedar/auxiliaries/gl/Scene.fwd.h"
#include "cedar/auxiliaries/gl/ObjectVisualization.fwd.h"
#include "cedar/auxiliaries/Grabbable.h"
#include "cedar/auxiliaries/Configurable.fwd.h"

// FORWARD DECLARATIONS
#include "cedar/auxiliaries/gui/Viewer.fwd.h"

// SYSTEM INCLUDES
#ifdef CEDAR_USE_QGLVIEWER
#if defined CEDAR_USE_GLEW
#include <glew.h>
#endif
#include <qglviewer.h>
#ifdef CEDAR_OS_WINDOWS //This looks like it should be QT5 vs QT4 ?
#include <manipulatedFrame.h>
#else
#include <QGLViewer/manipulatedFrame.h>
#endif // CEDAR_OS_WINDOWS

#else
#include <QWidget>
#endif // CEDAR_USE_QGLVIEWER
#include <QList>
#include <QReadWriteLock>
#include <string>
#include <QPoint>
#include <QGLWidget>

/*!@brief A simple viewer for OpenGL drawing routines, based on QGLViewer
 *
 * Use this class to draw instances of classes inherited from cedar::aux::gl::Object. This viewer has freely movable
 * camera position and other perks, coming from QGLViewer.
 *
 * @remarks To visualize an object, add it to an instance of cedar::aux::gl::Scene, then create a Viewer for this Scene
 * @remarks This class uses QGLViewer. Without it, it will not function properly.
 *
 */
class cedar::aux::gui::Viewer
:
#ifdef CEDAR_USE_QGLVIEWER
public QGLViewer,
#else
public QGLWidget,
#endif // CEDAR_USE_QGLVIEWER
public cedar::aux::Grabbable
{
private:
  Q_OBJECT
  
public:
  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------

  /*!@brief the constructor */
  Viewer(cedar::aux::gl::ScenePtr pScene, bool readFromFile = true);

  /*!@brief the constructor without a scene pointer */
  Viewer(bool readFromFile = true);

  /*!@brief the destructor */
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
  void initGl(cedar::aux::gl::ObjectVisualizationPtr pVisualization);

  // Grabbable Interface
  /*!@brief Grab the Image in the class
   *
   * @remarks
   *    This is a member of the grabbable interface
   *
   * @return The image in a cv::Mat structure
   */
  cv::Mat& grabImage();


  /*!@brief initialize the grabber specific parts in this method.
   *
   * The grabber invokes this method in it's constructor.
   * Have a look at the class cedar::aux::gui::Viewer for an implementation.
   * The QReadWriteLock is managed by this class, so don't delete it!
   *
   * @remarks
   *    This is a member of the grabbable interface
   *
   * @return returns the lock for the image-mat, if there isn't already a grabber connected.
   *         Otherwise it will return NULL
   */
  QReadWriteLock* registerGrabber();

  /*!@brief deinitialize the grabber specific parts in this method.
   *
   * The grabber invokes this method in it's destructor.
   * Have a look at the class cedar::aux::gui::Viewer for an implementation
   *
   * @param lock This is the lock which comes from the registerGrabber() method. This parameter is used to
   *    check, if the assigned grabber invokes the deregisterGrabber() method
   *
   * @remarks
   *    This is a member of the grabbable interface
   */
  void deregisterGrabber(QReadWriteLock* lock);

  std::string getViewerLabel() const;

  void setViewerLabel(std::string label);

  //Todo: Implement this!
//  void readFromConfiguration(cedar::aux::ConfigurationNode& node);

  void writeToConfiguration(cedar::aux::ConfigurationNode& root, QPoint mainWindowSize = QPoint());

#ifndef CEDAR_USE_QGLVIEWER
  //@cond SKIPPED_DOCUMENTATION
  // The following functions mimic part of the interface of QGLViewer to reduce compilation issues when the library is
  // not present. Rest assured that they do nothing.
  void setSceneRadius(double) {}
  void showEntireScene() {}

  QImage grabFrameBuffer();
  //@endcond
#endif // CEDAR_USE_QGLVIEWER

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  /*!@brief initialization */
  void init();
  
  ///!@brief grab the GL context
  void grabBuffer();

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
  cedar::aux::gl::ScenePtr mpScene;

  ///!@brief Read/write lock for the internal grabber buffer used for concurrent access
  QReadWriteLock* mpGrabberLock;

  ///!@brief the buffer for the grabber
  cv::Mat mGrabberBuffer;

  ///!@brief internal flag if grabber is connected
  bool mGrabberConnected;

  ///!@brief internal flag if saved state should be read
  bool mReadFromFile;

  std::string mRegisteredGrabber;

  ///!@brief flag in order to be stored in Plotgroups
  std::string mViewerLabel;
};

#endif  // CEDAR_AUX_GUI_VIEWER_H
