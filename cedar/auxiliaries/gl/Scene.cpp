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

    File:        Scene.cpp

    Maintainer:  Hendrik Reimann
    Email:       hendrik.reimann@ini.rub.de
    Date:        2010 10 28

    Description: Scene for organizing instances of cedar::aux::gl::ObjectVisualization

    Credits:

======================================================================================================================*/

// CEDAR INCLUDES
#include "cedar/auxiliaries/gl/Scene.h"
#include "cedar/auxiliaries/gui/Viewer.h"

// SYSTEM INCLUDES

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------
cedar::aux::gl::Scene::Scene()
:
mObjectVisualizationLock(QReadWriteLock::Recursive) //!Todo: this locker is not used everywhere it needs to be used
{
  init();
}

cedar::aux::gl::Scene::~Scene()
{

}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

boost::signals2::connection cedar::aux::gl::Scene::connectToSceneChangedSignal(boost::function<void ()> slot)
{
  return mSceneChanged.connect(slot);
}

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

int cedar::aux::gl::Scene::addObjectVisualization(cedar::aux::gl::ObjectVisualizationPtr pObjectVisualization)
{
  QWriteLocker write_locker(&mObjectVisualizationLock);

  mObjectVisualizations.push_back(pObjectVisualization);
  for (int i=0; i<mViewers.size(); i++)
  {
    mViewers[i]->initGl(pObjectVisualization);
  }
  mSceneChanged();
  return mObjectVisualizations.size() - 1;
}

int cedar::aux::gl::Scene::addViewer(cedar::aux::gui::Viewer* pViewer)
{
  mViewers.push_back(pViewer);
  return mViewers.size() - 1;
}

int cedar::aux::gl::Scene::removeViewer(cedar::aux::gui::Viewer* pViewer)
{
  for (int i=0; i<mViewers.size(); i++)
  {
    if (mViewers[i] == pViewer)
    {
      mViewers.removeAt(i);
      return i;
    }
  }
  return 0;
}

void cedar::aux::gl::Scene::deleteObjectVisualization(const std::string &name)
{
    QWriteLocker write_locker(&mObjectVisualizationLock);

    for(int i=0; i<mObjectVisualizations.size(); ++i)
    {
      if(mObjectVisualizations.at(i)->objectName().toStdString() == name)
      {
        mObjectVisualizations.removeAt(i);
        mSceneChanged();
      }
    }
}

void cedar::aux::gl::Scene::deleteObjectVisualizationPtr(cedar::aux::gl::ObjectVisualizationPtr &pObjectVisualization)
{
  QWriteLocker write_locker(&mObjectVisualizationLock);
  mObjectVisualizations.removeAt(mObjectVisualizations.indexOf(pObjectVisualization));
  mSceneChanged();
}

void cedar::aux::gl::Scene::clear()
{
  mObjectVisualizations.clear();
  mSceneChanged();
}

void cedar::aux::gl::Scene::draw()
{
  QReadLocker read_locker(&mObjectVisualizationLock);
  // save origin transformation to stack
  glPushMatrix();

  // draw all items in the scene
  for (int i = 0; i < mObjectVisualizations.size(); ++i)
  {
    mObjectVisualizations[i]->draw();
  }
  
  // return to origin transformation
  glPopMatrix();
  
  // draw the floor
  if (mIsDrawingFloor)
  {     
    glTranslated(-mSceneLimit, -mSceneLimit, 0);
    for (int i=-mSceneLimit; i < mSceneLimit; i++)
    {
      for (int j=-mSceneLimit; j < mSceneLimit; j++)
      {
        if(((i+j) % 2) == 0)
        {
          gl::setColor(0.333, 0.333, 0.333);
        }
        else
        {
          gl::setColor(0.666, 0.666, 0.666);
        } // end if
        drawBlock(1, 1, 0.01, false);
        glTranslated(0, 1, 0);
      } // end for (cols)
      glTranslated(1, -2*mSceneLimit, 0);
    } // end for (rows)
    glTranslated(mSceneLimit, mSceneLimit, 0);
  }
}

int cedar::aux::gl::Scene::getNumberOfObjectVisualizations() const
{
  return mObjectVisualizations.size();
}

bool cedar::aux::gl::Scene::isEmpty() const
{
  return (mObjectVisualizations.size() == 0);
}

cedar::aux::gl::ObjectVisualizationPtr cedar::aux::gl::Scene::getObjectVisualization(int index)
{
  return mObjectVisualizations[index];
}

void cedar::aux::gl::Scene::initGl()
{
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_MAP2_VERTEX_3);
  glEnable(GL_AUTO_NORMAL);

  // set light
  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);
  
  // Create light components
  GLfloat ambientLight[] = { 0.2f, 0.2f, 0.2f, 1.0f };
  GLfloat diffuseLight[] = { 0.8f, 0.8f, 0.8f, 1.0f };
  GLfloat specularLight[] = { 0.5f, 0.5f, 0.5f, 1.0f };
  GLfloat position[] = { 0.5f, 1.0f, 1.0f, 1.0f };
  
  // Assign created components to GL_LIGHT0
  glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLight);
  glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseLight);
  glLightfv(GL_LIGHT0, GL_SPECULAR, specularLight);
  glLightfv(GL_LIGHT0, GL_POSITION, position);

  // let all items in the scene initialize their resources in the current Gl context
  for (int i = 0; i < mObjectVisualizations.size(); ++i)
  {
    mObjectVisualizations[i]->initializeGl();
  }

}

void cedar::aux::gl::Scene::init()
{
  mIsDrawingFloor = true;
  mSceneLimit = 10;
}
