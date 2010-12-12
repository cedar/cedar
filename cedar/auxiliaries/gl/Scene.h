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

    File:        Scene.h

    Maintainer:  Hendrik Reimann
    Email:       hendrik.reimann@ini.rub.de
    Date:        2010 11 25

    Description: Scene for organizing instances of cedar::aux::gl::Object

    Credits:

======================================================================================================================*/


#ifndef CEDAR_AUX_GL_SCENE_H
#define CEDAR_AUX_GL_SCENE_H

// LOCAL INCLUDES
#include "auxiliaries/gl/namespace.h"
#include "auxiliaries/gl/Object.h"

// PROJECT INCLUDES

// SYSTEM INCLUDES
#include <string>
#include <QList>

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
  // TODO: something is not destroyed properly, find out what happens and fix it
  
  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  /*!@brief tells the scene whether a simple floor in the xy-plane should be drawn or not
   * @param state    truth value of 'floor being drawn'
   */
	void drawFloor(bool state=true);

  /*!@brief sets a rough delimiter of the scene within which things are guaranteed to be visualized
   * @param value    new scene delimiter
   */
  void setSceneLimit(double value);
  
  /*!@brief returns the rough delimiter of the scene within which things are guaranteed to be visualized
   * @return    scene limit
   */
  double getSceneLimit();
  
  /*!@brief adds an object to the scene
   * @param pObject    pointer to the object
   * @return    index of the object in the scene
   */
  int addObject(cedar::aux::gl::ObjectPtr& rpObject);
  
  /*!@brief removes an object from the scene
   * @param index    index of the object in the scene
   */
  void deleteObject(int index);
  
  /*!@brief removes all objects from the scene */
  void clear();
  
  /*!@brief draws all objects in the scene */
  void draw();
  
  /*!@brief convenience function to check whether a scene containts any objects
   * @return    true if there is no object in the scene
   */
  bool isEmpty();
  
  /*!@brief gives the number of objects currently in the scene
   * @return    number of objects currently in the scene
   */
  int numberOfObjects();
  
  /*!@brief access to a single object in the scene
   * @param index    index of the desired object within the scene
   * @return    pointer to the object with the specified index
   */
  cedar::aux::gl::ObjectPtr getObject(int index);
  
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
private:
	bool mIsDrawingFloor;
  double mSceneLimit;
  QList<cedar::aux::gl::ObjectPtr> mObjects;
};

#endif  // CEDAR_AUX_GL_SCENE_H
