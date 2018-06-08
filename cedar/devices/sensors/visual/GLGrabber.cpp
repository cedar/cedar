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
    along with cedar. If not, see <http:// www.gnu.org/licenses/>.

========================================================================================================================

    Institute:   Ruhr-Universitaet Bochum
                 Institut fuer Neuroinformatik

    File:        GLGrabber.cpp

    Maintainer:  Georg Hartinger
    Email:       georg.hartinger@ini.rub.de
    Date:        2012 04 23

    Description: Implementation of a Grabber to grab from a QGLWidget

    Credits:

======================================================================================================================*/

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

#ifdef CEDAR_USE_QGLVIEWER

// CEDAR INCLUDES
#include "cedar/devices/sensors/visual/GLGrabber.h"
#include "cedar/devices/sensors/visual/exceptions.h"

// SYSTEM INCLUDES

//----------------------------------------------------------------------------------------------------------------------
// register the class
//----------------------------------------------------------------------------------------------------------------------

namespace
{
  bool declared
    = cedar::dev::sensors::visual::Grabber::ChannelManagerSingleton::getInstance()
        ->registerType<cedar::dev::sensors::visual::GLChannelPtr>();
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
  cedar::dev::sensors::visual::GLChannelPtr
  (
    new cedar::dev::sensors::visual::GLChannel(qglWidget)
  )
)
{
  cedar::aux::LogSingleton::getInstance()->allocating(this);
}


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
  cedar::dev::sensors::visual::GLChannelPtr
  (
    new cedar::dev::sensors::visual::GLChannel(qglWidget0)
  ),
  cedar::dev::sensors::visual::GLChannelPtr
  (
    new cedar::dev::sensors::visual::GLChannel(qglWidget1)
  )
)
{
  cedar::aux::LogSingleton::getInstance()->allocating(this);
}

// Destructor
cedar::dev::sensors::visual::GLGrabber::~GLGrabber()
{
  doCleanUp();
  cedar::aux::LogSingleton::getInstance()->freeing(this);
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::dev::sensors::visual::GLGrabber::onCreateGrabber()
{
  // nothing to do
}


void cedar::dev::sensors::visual::GLGrabber::onCloseGrabber()
{
  this->onCleanUp();
}


void cedar::dev::sensors::visual::GLGrabber::onCleanUp()
{
  // do the cleanup of used hardware in this method
  // on an exception or a CTRL-C only onCleanUp will be invoked (no destructor)

  // delete all pointer-references to external widgets
  unsigned int num_channels = getNumChannels();
  for(unsigned int channel = 0; channel < num_channels; ++channel)
  {
    getGLChannel(channel)->mpQGLWidget = NULL;
  }
}


std::string cedar::dev::sensors::visual::GLGrabber::onGetSourceInfo(unsigned int channel)
{
  // value of channel is already checked by GraberInterface::getSourceInfo()
  return "Channel " + cedar::aux::toString(channel)
                                      + ": QT::OGLWidget class \""
                                      + typeid(getGLChannel(channel)->mpQGLWidget).name()
                                      + "\"";
}


void cedar::dev::sensors::visual::GLGrabber::onGrab(unsigned int channel)
{
  // pointer to the QGLWidget
  QGLWidget* p_channel_widget = getGLChannel(channel)->mpQGLWidget;

  if (p_channel_widget == NULL)
  {
    std::string msg = "Channel " + cedar::aux::toString(channel) + ": "
                      + "The Widget to grab from is not valid (i.e. it is a NULL-pointer)";
    CEDAR_THROW(cedar::dev::sensors::visual::GrabberGrabException,msg)
  }

  // grab framebuffer without alpha-channel. possible values
  // GL_FRONT_LEFT, GL_FRONT_RIGHT, GL_BACK_LEFT, GL_BACK_RIGHT, GL_FRONT, GL_BACK, GL_LEFT, GL_RIGHT, GL_AUXi,
  // where i is between 0 and the value of GL_AUX_BUFFERS minus 1.

  // activate this thread for painting
  // p_channel_widget->makeCurrent();

  // problem: qgl-widget painting also have to be multithreaded, i.e also have to invoke makeCurrent(), doneCurrent()
  //   It is not possible to reach this from this grabber. So keep in mind:
  // ATTENTION: This grabbing is only possible in the GUI-Thread!!!

  glReadBuffer(GL_FRONT_RIGHT);
  QImage qimage = p_channel_widget->grabFrameBuffer(false);
  // p_channel_widget->doneCurrent();

  // convert QImage to cv::Mat
  cv::Mat mat =
     cv::Mat(qimage.height(), qimage.width(), CV_8UC4, static_cast<uchar*>(qimage.bits()), qimage.bytesPerLine());
  cv::Mat mat2 = cv::Mat(mat.rows, mat.cols, CV_8UC3 );
  int from_to[] = { 0,0, 1,1, 2,2 };
  cv::mixChannels( &mat, 1, &mat2, 1, from_to, 3 );

  // apply the new content to the channel image
  getImageMat(channel) = mat2.clone();
}


void cedar::dev::sensors::visual::GLGrabber::setWidget(unsigned int channel, QGLWidget *qglWidget)
{
  if (channel >= getNumChannels())
  {
    CEDAR_THROW(cedar::aux::IndexOutOfRangeException,"The wanted channel is out of range");
  }

  // check if a valid widget is there
  if (qglWidget == NULL)
  {
    CEDAR_THROW
    (
      cedar::dev::sensors::visual::InvalidParameterException,
      "The new Widget to grab from is invalid (NULL-pointer)"
    );
  }

  // stop grabbing thread if running
  bool restart_grabber = LoopedThread::isRunningNolocking();
  if (restart_grabber)
  {
    this->stop();
  }

  // change source
  getGLChannel(channel)->mpQGLWidget = qglWidget;

  // get first new image
  this->grab();

  // restart grabbing-thread (if running before)
  if (restart_grabber)
  {
    this->start();
  }
}

#endif //CEDAR_USE_QGLVIEWER
