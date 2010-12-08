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

 ----- Institute:   Ruhr-Universitaet-Bochum Institut fuer Neuroinformatik
 
 ----- File:        Scene.cpp
 
 ----- Author:      Hendrik Reimann
 
 ----- contact:     hendrik.reimann@ini.rub.de
 
 ----- Date:        2010 10 28
 
 ----- Description: scene for organizing instances of GlObjectVisualization
 
 --------------------------------------------------------------------------*/

// LOCAL INCLUDES
#include "Scene.h"

// PROJECT INCLUDES

// SYSTEM INCLUDES

using namespace cedar::aux::gl;
using namespace std;
using namespace cv;

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

Scene::Scene(void)
{
  init();
}

Scene::~Scene(void)
{

}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void Scene::drawFloor(bool state)
{
  mIsDrawingFloor = state;
}

void Scene::setSceneLimit(double value)
{
  mSceneLimit = value;
}

double Scene::getSceneLimit()
{
  return mSceneLimit;
}

int Scene::addObject(cedar::aux::gl::ObjectPtr& rpObject)
{
  // TODO: prevent different objects with same names
//  if (object name exists)
//  {
//    return false;
//  }
	mObjects.push_back(rpObject);
  return mObjects.size() - 1;
}

void Scene::deleteObject(int index)
{
  mObjects.removeAt(index);
}

void Scene::clear()
{
  mObjects.clear();
}

void Scene::draw()
{
	// save origin transformation to stack
	glPushMatrix();

	// draw all items in the scene
	for (int i=0; i<mObjects.size(); i++)
	{
		mObjects[ i ]->draw();
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

int Scene::numberOfObjects()
{
  return mObjects.size();
}

bool Scene::isEmpty()
{
  return (mObjects.size() == 0);
}

cedar::aux::gl::ObjectPtr Scene::getObject(int index)
{
  return mObjects[index];
}

void Scene::initGl()
{
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_MAP2_VERTEX_3);
  glEnable(GL_AUTO_NORMAL);

  // TODO: this somehow sets the light relative to the camera, which might not be wanted. check!
	// set light
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
  
	// Create light components
	GLfloat ambientLight[] = { 0.2f, 0.2f, 0.2f, 1.0f };
	GLfloat diffuseLight[] = { 0.8f, 0.8f, 0.8, 1.0f };
	GLfloat specularLight[] = { 0.5f, 0.5f, 0.5f, 1.0f };
	GLfloat position[] = { -0.5f, 1.0f, -1.0f, 1.0f };
  
	// Assign created components to GL_LIGHT0
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLight);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseLight);
	glLightfv(GL_LIGHT0, GL_SPECULAR, specularLight);
	glLightfv(GL_LIGHT0, GL_POSITION, position);
}

void Scene::init()
{
  mIsDrawingFloor = true;
  mSceneLimit = 10;
}




