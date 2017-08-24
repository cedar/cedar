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

    File:        Viewer.cpp

    Maintainer:  Hendrik Reimann
    Email:       hendrik.reimann@ini.rub.de
    Date:        2010 10 28

    Description: Simple viewer for visualizing a scene of objects

    Credits:

======================================================================================================================*/

// CEDAR INCLUDES
#include "cedar/auxiliaries/gui/Viewer.h"
#include "cedar/auxiliaries/gl/Scene.h"

// SYSTEM INCLUDES

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::aux::gui::Viewer::Viewer(cedar::aux::gl::ScenePtr pScene, bool readFromFile)
:
mpScene(pScene),
mpGrabberLock(NULL),
mGrabberBuffer(cv::Mat()),
mGrabberConnected(false),
mReadFromFile(readFromFile),
mRegisteredGrabber(""),
mViewerLabel("")
{
    mpScene->addViewer(this);
}

cedar::aux::gui::Viewer::Viewer(bool readFromFile)
:
mpScene(cedar::aux::gl::ScenePtr(new cedar::aux::gl::Scene)),
mpGrabberLock(NULL),
mGrabberBuffer(cv::Mat()),
mGrabberConnected(false),
mReadFromFile(readFromFile),
mRegisteredGrabber(""),
mViewerLabel("")
{

}

cedar::aux::gui::Viewer::~Viewer()
{
  if (mpScene)
  {
    mpScene->removeViewer(this);
  }

  if (mpGrabberLock)
  {
    delete mpGrabberLock;
  }
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::aux::gui::Viewer::init()
{
  if (mReadFromFile)
  {
#ifdef CEDAR_USE_QGLVIEWER
    restoreStateFromFile();
#endif // CEDAR_USE_QGLVIEWER
  }

  mpScene->initGl();
}

void cedar::aux::gui::Viewer::initGl(cedar::aux::gl::ObjectVisualizationPtr pVisualization)
{
  pVisualization->initializeGl();
}

void cedar::aux::gui::Viewer::draw()
{
  mpScene->draw();

  if (mGrabberConnected)
  {
    grabBuffer();
  }
}

void cedar::aux::gui::Viewer::timerEvent(QTimerEvent*)
{
  if (isVisible())
  {
    update();
  }
}

void cedar::aux::gui::Viewer::grabBuffer()
{
#ifdef CEDAR_USE_QGLVIEWER
  // grab framebuffer without alpha-channel. possible values
  // GL_FRONT_LEFT, GL_FRONT_RIGHT, GL_BACK_LEFT, GL_BACK_RIGHT, GL_FRONT, GL_BACK, GL_LEFT, GL_RIGHT,
  // GL_AUXi, where i is between 0 and the value of GL_AUX_BUFFERS minus 1.
  glReadBuffer(GL_FRONT_RIGHT);
  QImage qimage = this->QGLWidget::grabFrameBuffer(false);

  // QImage to cv::Mat
  cv::Mat mat = cv::Mat(qimage.height(), qimage.width(), CV_8UC4, static_cast<uchar*>(qimage.bits()), qimage.bytesPerLine());
  cv::Mat mat2 = cv::Mat(mat.rows, mat.cols, CV_8UC3);
  int from_to[] = { 0,0, 1,1, 2,2 };
  cv::mixChannels(&mat, 1, &mat2, 1, from_to, 3);

  //apply the new content to the channel image
  mpGrabberLock->lockForWrite();
  mGrabberBuffer = mat2;
  mpGrabberLock->unlock();
#endif // CEDAR_USE_QGLVIEWER
}

#ifndef CEDAR_USE_QGLVIEWER
QImage cedar::aux::gui::Viewer::grabFrameBuffer()
{
  return this->QGLWidget::grabFrameBuffer(false);
}
#endif // CEDAR_USE_QGLVIEWER

cv::Mat& cedar::aux::gui::Viewer::grabImage()
{
  return mGrabberBuffer;
}

QReadWriteLock* cedar::aux::gui::Viewer::registerGrabber()
{
  if (mGrabberConnected)
  {
    return NULL;
  }
  mpGrabberLock = new QReadWriteLock();
  mGrabberConnected = true;
  return mpGrabberLock;
}

void cedar::aux::gui::Viewer::deregisterGrabber(QReadWriteLock* lock)
{
  // only allow grabber with correct QReadWriteLock to disconnect
  // i.e., use the pointer-address of the QReadWriteLock as unique id
  if (mpGrabberLock && (lock == mpGrabberLock))
  {
    delete mpGrabberLock;
    mpGrabberLock = NULL;
    mGrabberConnected = false;
  }
  else
  {
    CEDAR_THROW(cedar::aux::NotFoundException, "Lock address does not match the stored address.");
  }
}

std::string cedar::aux::gui::Viewer::getViewerLabel() const
{
  return this->mViewerLabel;
}

void cedar::aux::gui::Viewer::setViewerLabel(std::string label)
{
  this->mViewerLabel = label;
}

