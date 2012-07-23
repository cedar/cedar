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

    File:        GLGrabber.cpp

    Maintainer:  Georg Hartinger
    Email:       georg.hartinger@ini.rub.de
    Date:        2012 04 23

    Description: Implementation of a Grabber to grab from OpenGL, i.e from a QGLWidget

    Credits:

======================================================================================================================*/

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/devices/sensors/visual/GLGrabber.h"

// SYSTEM INCLUDES

//----------------------------------------------------------------------------------------------------------------------
// register the class
//----------------------------------------------------------------------------------------------------------------------
namespace
{
  bool declared
    = cedar::dev::sensors::visual::Grabber::ChannelManagerSingleton::getInstance()
        ->registerType<cedar::dev::sensors::visual::GLGrabber::GLChannelPtr>();
}

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

// Constructor for a single-channel grabber
cedar::dev::sensors::visual::GLGrabber::GLGrabber
(
  QGLWidget *qglWidget,
  const std::string& grabberName
)
:
cedar::dev::sensors::visual::Grabber
(
  grabberName,
  cedar::dev::sensors::visual::GLGrabber::GLChannelPtr
  (
    new cedar::dev::sensors::visual::GLGrabber::GLChannel(qglWidget)
  )
)
{
  cedar::aux::LogSingleton::getInstance()->allocating(this);
}


//----------------------------------------------------------------------------------------------------------------------
// Constructor for a stereo grabber
cedar::dev::sensors::visual::GLGrabber::GLGrabber
(
  QGLWidget *qglWidget0,
  QGLWidget *qglWidget1,
  const std::string& grabberName
)
:
cedar::dev::sensors::visual::Grabber
(
  grabberName,
  cedar::dev::sensors::visual::GLGrabber::GLChannelPtr
  (
    new cedar::dev::sensors::visual::GLGrabber::GLChannel(qglWidget0)
  ),
  cedar::dev::sensors::visual::GLGrabber::GLChannelPtr
  (
    new cedar::dev::sensors::visual::GLGrabber::GLChannel(qglWidget1)
  )
)
{
  cedar::aux::LogSingleton::getInstance()->allocating(this);
}

//----------------------------------------------------------------------------------------------------------------------
// Destructor
cedar::dev::sensors::visual::GLGrabber::~GLGrabber()
{
  doCleanUp();
  cedar::aux::LogSingleton::getInstance()->freeing(this);
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

bool cedar::dev::sensors::visual::GLGrabber::onInit()
{
  unsigned int num_cams = getNumCams();
  std::stringstream init_message;
  init_message << ": Initialize grabber with " << getNumCams() << " channels ..." << std::endl;

  for(unsigned int channel = 0; channel < num_cams; ++channel)
  {
    init_message << "Channel " << channel
                 << ": QT::OGLWidget class \"" << typeid(getGLChannel(channel)->mpQGLWidget).name()
                 << "\"" << std::endl;
  }
  cedar::aux::LogSingleton::getInstance()->debugMessage
                                           (
                                            this->getName() + init_message.str(),
                                             "cedar::dev::sensors::visual::OglGrabber::onInit()"
                                           );
  //Grab first frames
  onGrab();

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
void cedar::dev::sensors::visual::GLGrabber::onCleanUp()
{
  // do the cleanup of used hardware in this method
  // on an exception or a CTRL-C only onCleanUp will be invoked (no destructor)

  // delete all pointer-references to external widgets
  unsigned int num_cams = getNumCams();
  for(unsigned int channel = 0; channel < num_cams; ++channel)
  {
    getGLChannel(channel)->mpQGLWidget = NULL;
  }
}

//----------------------------------------------------------------------------------------------------------------------
//void cedar::dev::sensors::visual::GLGrabber::onAddChannel()
//{
//  // create the channel structure for one channel
//  GLChannelPtr channel(new GLChannel);
//  channel->mpQGLWidget = NULL;
//  mChannels.push_back(channel);
//}

//----------------------------------------------------------------------------------------------------------------------
bool cedar::dev::sensors::visual::GLGrabber::onDeclareParameters()
{
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
void cedar::dev::sensors::visual::GLGrabber::onUpdateSourceInfo(unsigned int channel)
{
  //value of channel is already checked by GraberInterface::getSourceInfo()
  getGLChannel(channel)->mChannelInfo = "Channel " + boost::lexical_cast<std::string>(channel)
                                      + ": QT::OGLWidget class \""
                                      + typeid(getGLChannel(channel)->mpQGLWidget).name()
                                      + "\"";
}

//----------------------------------------------------------------------------------------------------------------------
bool cedar::dev::sensors::visual::GLGrabber::onGrab()
{
  bool ogl_valid = true;
  unsigned int num_cams = getNumCams();
  for(unsigned int channel = 0; channel < num_cams; ++channel)
  {
    if (getGLChannel(channel)->mpQGLWidget != NULL)
    {
      QGLWidget* p_channel_widget = getGLChannel(channel)->mpQGLWidget;

      // grab framebuffer without alpha-channel. possible values
      // GL_FRONT_LEFT, GL_FRONT_RIGHT, GL_BACK_LEFT, GL_BACK_RIGHT, GL_FRONT, GL_BACK, GL_LEFT, GL_RIGHT, GL_AUXi,
      // where i is between 0 and the value of GL_AUX_BUFFERS minus 1.

      //activate this thread for painting
      //problem: qgl-widget painting also have to be multithreaded, i.e also have to invoke makeCurrent(), doneCurrent()
      //p_channel_widget->makeCurrent();
      glReadBuffer(GL_FRONT_RIGHT);
      QImage qimage = p_channel_widget->grabFrameBuffer(false);
      //p_channel_widget->doneCurrent();

      // QImage to cv::Mat
      cv::Mat mat = cv::Mat(qimage.height(), qimage.width(), CV_8UC4,(uchar*)qimage.bits(), qimage.bytesPerLine());
      cv::Mat mat2 = cv::Mat(mat.rows, mat.cols, CV_8UC3 );
      int from_to[] = { 0,0, 1,1, 2,2 };
      cv::mixChannels( &mat, 1, &mat2, 1, from_to, 3 );

      //apply the new content to the channel image
      getGLChannel(channel)->mImageMat = mat2.clone();
    }
    else
    {
      // if opengl context isn't valid, then an empty matrix will be return
      ogl_valid = false;
      getGLChannel(channel)->mImageMat = cv::Mat();
    }
  }
  return ogl_valid;
}


//----------------------------------------------------------------------------------------------------------------------
void cedar::dev::sensors::visual::GLGrabber::setWidget(unsigned int channel, QGLWidget *qglWidget)
{
  if (channel >= getNumCams())
  {
    CEDAR_THROW
    (
      cedar::aux::IndexOutOfRangeException,
      "cedar::dev::sensors::visual::GLGrabber::setWidget"
    );
  }
  if (qglWidget != NULL)
  {

    bool restart_grabber = LoopedThread::isRunning();

    // stop grabbing thread if running
    if (restart_grabber)
    {
      this->stopGrabber();
    }

    // change source
    getGLChannel(channel)->mpQGLWidget = qglWidget;
    cedar::aux::LogSingleton::getInstance()->debugMessage
                                            (
                                             this->getName() + ": New Widget applied",
                                              "cedar::dev::sensors::visual::GLGrabber::setWidget"
                                            );
    this->grab();
    if (restart_grabber)
    {
      this->startGrabber();
    }
  }
}

