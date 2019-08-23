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
#include <chrono>
#include <sstream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>


// SYSTEM INCLUDES

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::aux::gui::Viewer::Viewer(cedar::aux::gl::ScenePtr pScene, bool readFromFile)
:
mpScene(pScene),
mpGrabberLock(nullptr),
mGrabberBuffer(cv::Mat()),
mGrabberConnected(false),
mReadFromFile(readFromFile),
mRegisteredGrabber(""),
mViewerLabel("")
{
    mpScene->addViewer(this);
#ifdef CEDAR_USE_QGLVIEWER
    this->camera()->setUpVector(qglviewer::Vec(0,0,1));
    mOldPos = camera()->position();
    mOldDir = camera()->viewDirection();
#endif
}

cedar::aux::gui::Viewer::Viewer(bool readFromFile)
:
mpScene(cedar::aux::gl::ScenePtr(new cedar::aux::gl::Scene)),
mpGrabberLock(nullptr),
mGrabberBuffer(cv::Mat()),
mGrabberConnected(false),
mReadFromFile(readFromFile),
mRegisteredGrabber(""),
mViewerLabel("")
{
#ifdef CEDAR_USE_QGLVIEWER
    this->camera()->setUpVector(qglviewer::Vec(0,0,1));
    mOldPos = camera()->position();
    mOldDir = camera()->viewDirection();
#endif
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
  if (mGrabberConnected) {
    grabBuffer();
  }
}

void cedar::aux::gui::Viewer::timerEvent(QTimerEvent*)
{
    bool move =false;
#ifdef CEDAR_USE_QGLVIEWER
    if(!this->isVisible()){
        hiddenUpdate();
    }
    else{
        update();
    }
    if(mOldPos != camera()->position()){
        mOldPos = camera()->position();
        move = true;
    }
    if(mOldDir != camera()->viewDirection()){
        mOldDir = camera()->viewDirection();
        move = true;
    }
#endif
    if (move)
        emit cameraMoved();
    emit updated();
}

#ifdef CEDAR_USE_QGLVIEWER
void cedar::aux::gui::Viewer::hiddenUpdate(){
    // regular draw doesn't work while the Widget is hidden
    //!@note DO NOT CALL makeCurrent() as it will lead to a segfault when instancing a new Robot while the Viewer is hidden
    //makeCurrent();

    paintGL();

    if (!m_fbo || m_fbo->width() != width() || m_fbo->height() != height())
    {
       //allocate additional? FBO for rendering or resize it if widget size changed
       delete m_fbo;
       QGLFramebufferObjectFormat format;
       format.setAttachment(QGLFramebufferObject::CombinedDepthStencil);
       m_fbo = new QGLFramebufferObject(width(), height(), format);
       resizeGL(width(), height());
    }


   //bind FBO and render stuff with paintGL() call
   m_fbo->bind();
   paintGL();
   //You could now grab the content of the framebuffer we've rendered to
   QImage qimage = m_fbo->toImage();

   cv::Mat  mat( qimage.height(), qimage.width(),
                 CV_8UC4,
                 const_cast<uchar*>(qimage.bits()),
                 static_cast<size_t>(qimage.bytesPerLine())
   );
   cv::Mat mat2 = cv::Mat(mat.rows, mat.cols, CV_8UC3);
   int from_to[] = { 0,0, 1,1, 2,2 };
   cv::mixChannels(&mat, 1, &mat2, 1, from_to, 3);

   //apply the new content to the channel image
   mpGrabberLock->lockForWrite();
   mGrabberBuffer = mat2;
   mpGrabberLock->unlock();

   m_fbo->release();

   //bind default framebuffer again. not sure if this necessary
   //and isn't supposed to use defaultFramebuffer()...
   m_fbo->bindDefault();
   //doneCurrent();
}
#endif

void cedar::aux::gui::Viewer::grabBuffer()
{
#ifdef CEDAR_USE_QGLVIEWER
  // grab framebuffer without alpha-channel. possible values
  // GL_FRONT_LEFT, GL_FRONT_RIGHT, GL_BACK_LEFT, GL_BACK_RIGHT, GL_FRONT, GL_BACK, GL_LEFT, GL_RIGHT,
  // GL_AUXi, where i is between 0 and the value of GL_AUX_BUFFERS minus 1.
  //glReadBuffer(GL_FRONT_RIGHT);
  QImage qimage = this->QGLWidget::grabFrameBuffer(false);


  // QImage to cv::Mat
  cv::Mat  mat( qimage.height(), qimage.width(),
                  CV_8UC4,
                  const_cast<uchar*>(qimage.bits()),
                  static_cast<size_t>(qimage.bytesPerLine())
    );
  cv::Mat mat2 = cv::Mat(mat.rows, mat.cols, CV_8UC3);
  int from_to[] = { 0,0, 1,1, 2,2 };
  cv::mixChannels(&mat, 1, &mat2, 1, from_to, 3);

  //apply the new content to the channel image
  mpGrabberLock->lockForWrite();
  mGrabberBuffer = mat2;
  mpGrabberLock->unlock();
#endif // CEDAR_USE_QGLVIEWER
}

//TODO: Why is this function necessary if we do not use the QGLViewer?
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

void cedar::aux::gui::Viewer::writeToConfiguration(cedar::aux::ConfigurationNode &value_node,QPoint mainWindowSize)
{
  //This Code is duplicated again in Group. Maybe it should be Part of the Viewer to Serialize!
  value_node.add("position_x", this->parentWidget()->x());
  value_node.add("position_y", this->parentWidget()->y());
  value_node.add("width", this->parentWidget()->width());
  value_node.add("height", this->parentWidget()->height());

  //Todo: This is duplicate code from Plotwidget
  auto widthHeight = mainWindowSize;
  if(widthHeight.x()!= 0 && widthHeight.y()!=0)
  {
//    std::cout<<"Adding relative Viewer Positions" << std::endl;
    double xSizeFull = widthHeight.x();
    double ySizeFull = widthHeight.y();
    double relativeX = (double) this->parentWidget()->x() /  xSizeFull;
    double relativeY = (double) this->parentWidget()->y() /  ySizeFull;
    double relativeWidth = (double) this->parentWidget()->width() / xSizeFull;
    double relativeHeight = (double) this->parentWidget()->height() / ySizeFull;
//    std::cout<<"Writing a Widget configuration: x position is: " << this->parentWidget()->x() << " , the width is: " << xSizeFull << " netting a relative position of: " << relativeX << std::endl;
    value_node.add("position_relative_x",relativeX);
    value_node.add("position_relative_y",relativeY);
    value_node.add("width_relative",relativeWidth);
    value_node.add("height_relative",relativeHeight);
  }
  else
  {
    std::cout<<"Not adding because the Ide Size is Zeros? Adding relative Viewer Positions" << std::endl;
  }


  if(this->getViewerLabel() != "")
    value_node.add("viewerLabel",this->getViewerLabel());
  else
  {
    //Generate some unique Label here! This is not good as there might be duplicates!!!
    time_t someTimeVar = time(0);
    std::string curTime = boost::lexical_cast<std::string>(someTimeVar);


    std::string labelString =  boost::lexical_cast<std::string>(this->parentWidget()->x())+boost::lexical_cast<std::string>(this->parentWidget()->y())
                               +boost::lexical_cast<std::string>(this->parentWidget()->width())+boost::lexical_cast<std::string>(this->parentWidget()->height())
                               +curTime;

    value_node.add("viewerLabel", labelString);
    this->setViewerLabel(labelString);
  }


#ifdef CEDAR_USE_QGLVIEWER
//      QWidget* viewerAsWidget = dynamic_cast<QWidget *>(dock_widget_child);

#ifdef CEDAR_USE_GLEW
  GLenum err = glewInit();
      if (GLEW_OK != err)
      {
        /* Problem: glewInit failed, something is seriously wrong. */
        fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
      }
#endif //CEDAR_USE_GLEW

  QGLViewer *qgl = dynamic_cast<QGLViewer *>(this);
  value_node.add("camera position x", qgl->camera()->position().x);
  value_node.add("camera position y", qgl->camera()->position().y);
  value_node.add("camera position z", qgl->camera()->position().z);
  value_node.add("camera orientation 0", qgl->camera()->orientation()[0]);
  value_node.add("camera orientation 1", qgl->camera()->orientation()[1]);
  value_node.add("camera orientation 2", qgl->camera()->orientation()[2]);
  value_node.add("camera orientation 3", qgl->camera()->orientation()[3]);

#endif // CEDAR_USE_QGLVIEWER

}


void cedar::aux::gui::Viewer::changeCameraPosition(const double x , const double y, const double z)
{
#ifdef CEDAR_USE_QGLVIEWER
    qglviewer::Vec position = qglviewer::Vec(x,y,z);

    this->camera()->setPosition(position);
#endif
}

void cedar::aux::gui::Viewer::changeCameraOrientation(const double alpha , const double beta)
{
#ifdef CEDAR_USE_QGLVIEWER
    this->camera()->setUpVector(qglviewer::Vec(0,0,1));  // set up axis

    // based on the description this "should" do the same thing
    // --> this->camera()->setOrientation(beta,alpha);
    // but it doesn't

    double xVec = cv::cos(alpha) * cv::cos(beta);
    double yVec = cv::sin(alpha) * cv::cos(beta);
    double zVec = cv::sin(beta);

    qglviewer::Vec direction = qglviewer::Vec(xVec,yVec,zVec);


    this->camera()->setViewDirection(direction);
#endif
}

void cedar::aux::gui::Viewer::closeEvent(QCloseEvent *event)
{
    if(!this->isVisible()){
        event->accept();
    }
    else{
        this->setVisible(false);
        event->ignore();
    }

}


void cedar::aux::gui::Viewer::toggleVisible()
{
    if(this->isVisible())
        this->setVisible(false);
    else
        this->setVisible(true);
}

