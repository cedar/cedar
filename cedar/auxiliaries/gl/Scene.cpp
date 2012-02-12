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

    File:        Scene.cpp

    Maintainer:  Hendrik Reimann
    Email:       hendrik.reimann@ini.rub.de
    Date:        2010 10 28

    Description: Scene for organizing instances of cedar::aux::gl::RigidBodyVisualization

    Credits:

======================================================================================================================*/

// CEDAR INCLUDES
#include "cedar/auxiliaries/gl/Scene.h"

// SYSTEM INCLUDES

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------
cedar::aux::gl::Scene::Scene()
{
  init();
}

cedar::aux::gl::Scene::~Scene()
{

}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::aux::gl::Scene::drawFloor(bool state)
{
  mIsDrawingFloor = state;
}

void cedar::aux::gl::Scene::setSceneLimit(double value)
{
  mSceneLimit = value;
}

double cedar::aux::gl::Scene::getSceneLimit() const
{
  return mSceneLimit;
}

int cedar::aux::gl::Scene::addRigidBodyVisualization(cedar::aux::gl::RigidBodyVisualizationPtr& rpRigidBodyVisualization)
{
  //!\todo prevent different objects with same names
//  if (object name exists)
//  {
//    return false;
//  }
  mRigidBodyVisualizations.push_back(rpRigidBodyVisualization);
  return mRigidBodyVisualizations.size() - 1;
}

void cedar::aux::gl::Scene::deleteRigidBodyVisualization(int index)
{
  mRigidBodyVisualizations.removeAt(index);
}

void cedar::aux::gl::Scene::clear()
{
  mRigidBodyVisualizations.clear();
}

void cedar::aux::gl::Scene::draw()
{
  // save origin transformation to stack
  glPushMatrix();

  // draw all items in the scene
  for (int i=0; i<mRigidBodyVisualizations.size(); i++)
  {
    mRigidBodyVisualizations[ i ]->draw();
  }
  
  // return to origin transformation
  glPopMatrix();
  
  // draw the floor
  if (mIsDrawingFloor)
  {
    // Draw the ground
    glColor3f(1.0, 1.0, 1.0);
    const float numberOfPatches = 100;
    glNormal3f(0.0, 0.0, 1.0);
    for (int j=0; j<numberOfPatches; ++j)
    {
      glBegin(GL_QUAD_STRIP);
        for (int i=0; i<=numberOfPatches; ++i)
        {
          glVertex2f(
                      (mSceneLimit*2*i/numberOfPatches-mSceneLimit),
                      (mSceneLimit*2*j/numberOfPatches-mSceneLimit) 
                    );
          glVertex2f(
                      (mSceneLimit*2*i/numberOfPatches-mSceneLimit),
                      (mSceneLimit*2*(j+1)/numberOfPatches-mSceneLimit)
                    );
        }
      glEnd();
    }
  }
}

int cedar::aux::gl::Scene::getNumberOfRigidBodyVisualizations() const
{
  return mRigidBodyVisualizations.size();
}

bool cedar::aux::gl::Scene::isEmpty() const
{
  return (mRigidBodyVisualizations.size() == 0);
}

cedar::aux::gl::RigidBodyVisualizationPtr cedar::aux::gl::Scene::getRigidBodyVisualization(int index)
{
  return mRigidBodyVisualizations[index];
}

void cedar::aux::gl::Scene::initGl()
{
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_MAP2_VERTEX_3);
  glEnable(GL_AUTO_NORMAL);

  //!@todo this somehow sets the light relative to the camera, which might not be wanted. check!
  // set light
  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);
  
  // Create light components
  GLfloat ambientLight[] = { 0.2f, 0.2f, 0.2f, 1.0f };
  GLfloat diffuseLight[] = { 0.8f, 0.8f, 0.8f, 1.0f };
  GLfloat specularLight[] = { 0.5f, 0.5f, 0.5f, 1.0f };
  GLfloat position[] = { -0.5f, 1.0f, -1.0f, 1.0f };
  
  // Assign created components to GL_LIGHT0
  glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLight);
  glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseLight);
  glLightfv(GL_LIGHT0, GL_SPECULAR, specularLight);
  glLightfv(GL_LIGHT0, GL_POSITION, position);

  // let all items in the scene initialize their resources in the current Gl context
  for (int i=0; i<mRigidBodyVisualizations.size(); i++)
  {
    mRigidBodyVisualizations[ i ]->initializeGl();
  }

}

void cedar::aux::gl::Scene::init()
{
  mIsDrawingFloor = true;
  mSceneLimit = 10;
}
