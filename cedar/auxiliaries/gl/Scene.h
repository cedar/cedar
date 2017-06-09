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

    File:        Scene.h

    Maintainer:  Hendrik Reimann
    Email:       hendrik.reimann@ini.rub.de
    Date:        2010 11 25

    Description: Scene for organizing instances of cedar::aux::gl::ObjectVisualization

    Credits:

======================================================================================================================*/

#ifndef CEDAR_AUX_GL_SCENE_H
#define CEDAR_AUX_GL_SCENE_H

// CEDAR INCLUDES
#include "cedar/auxiliaries/gl/Scene.fwd.h"
#include "cedar/auxiliaries/gl/ObjectVisualization.h"
#include "cedar/auxiliaries/gl/drawShapes.h"

// FORWARD DECLARATIONS
#include "cedar/auxiliaries/gui/Viewer.fwd.h"

// SYSTEM INCLUDES
#ifndef Q_MOC_RUN
  #include <boost/signals2/signal.hpp>
  #include <boost/signals2/connection.hpp>
#endif
#include <string>
#include <QList>
#include <QReadWriteLock>

/*!@brief This class visualizes several objects in the same environment
 *
 * An instance of this class manages pointers to several instances of gl::ObjectVisualization.
 *
 * @remarks To get a simple visualization of the ObjectVisualizations in the scene, create a aux::gui::Viewer for it
 */
class cedar::aux::gl::Scene
{
public:
  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
  /*!@brief the constructor */
  Scene();
  
  /*!@brief the destructor */
  ~Scene();
  
  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  /*!@brief register a function pointer with this function to react to any changes in the scene (e.g. an object is
   * added or deleted)
   */
  boost::signals2::connection connectToSceneChangedSignal(boost::function<void ()> slot);

  /*!@brief tells the scene whether a simple floor in the xy-plane should be drawn or not
   * @param state truth value of 'floor being drawn'
   */
  void drawFloor(bool state=true);

  /*!@brief sets a rough delimiter of the scene within which things are guaranteed to be visualized
   * @param value new scene delimiter
   */
  void setSceneLimit(double value);
  
  /*!@brief returns the rough delimiter of the scene within which things are guaranteed to be visualized
   * @return scene limit
   */
  double getSceneLimit() const;
  
  /*!@brief adds an object to the scene
   * @param pObjectVisualization pointer to the object
   * @return index of the object in the scene
   */
  int addObjectVisualization(cedar::aux::gl::ObjectVisualizationPtr pObjectVisualization);
  
  /*!@brief adds an object to the scene
   * @param pViewer pointer to the viewer
   * @return index of the object in the scene
   */
  int addViewer(cedar::aux::gui::Viewer* pViewer);

  /*!@brief removes the viewer from the scene
   * @param pViewer pointer to the viewer
   */
  int removeViewer(cedar::aux::gui::Viewer* pViewer);

  /*!@brief removes an object from the scene via its name
   * @param name name of the object in the scene
   */
  void deleteObjectVisualization(const std::string &name);
  void deleteObjectVisualizationPtr(cedar::aux::gl::ObjectVisualizationPtr &pObjectVisualization);
  
  /*!@brief removes all objects from the scene */
  void clear();
  
  /*!@brief draws all objects in the scene */
  void draw();
  
  /*!@brief convenience function to check whether a scene containts any objects
   * @return true if there is no object in the scene
   */
  bool isEmpty() const;
  
  /*!@brief gives the number of objects currently in the scene
   * @return number of objects currently in the scene
   */
  int getNumberOfObjectVisualizations() const;
  
  /*!@brief access to a single object in the scene
   * @param index index of the desired object within the scene
   * @return pointer to the object with the specified index
   */
  cedar::aux::gl::ObjectVisualizationPtr getObjectVisualization(int index);
  
  /*!@brief initializes gl setup, should be called from all viewers */
  void initGl();
  
  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  /*!@brief class initialization */
  void init();
  
  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  //!@brief a boost signal that is emitted if a change in the scene occurs
  boost::signals2::signal<void ()> mSceneChanged;
private:
  bool mIsDrawingFloor;
  double mSceneLimit;
  QList<cedar::aux::gl::ObjectVisualizationPtr> mObjectVisualizations;
  // list of viewers currently displaying this scene
  QList<cedar::aux::gui::Viewer*> mViewers;
  
  mutable QReadWriteLock mObjectVisualizationLock;
};

#endif  // CEDAR_AUX_GL_SCENE_H
