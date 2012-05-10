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

    File:        OglGrabber.cpp

    Maintainer:  Georg Hartinger
    Email:       georg.hartinger@ini.rub.de
    Date:        2012 04 23

    Description: Implementation of a Grabber to grab from OpenGL, i.e from a QGLWidget

    Credits:

======================================================================================================================*/

// CEDAR INCLUDES
#include "cedar/devices/sensors/visual/OglGrabber.h"

// SYSTEM INCLUDES

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------
// Constructor for a single-channel grabber
cedar::dev::sensors::visual::OglGrabber::OglGrabber(std::string configFileName, QGLWidget* oglWidget)
:
cedar::dev::sensors::visual::GrabberInterface(configFileName)
{
  // debug information logging
  cedar::aux::LogSingleton::getInstance()->allocating(this);

  // read initialization values from configuration file
  readInit(1,"OglGrabber");

  // set additional parameters from the constuctor
  getChannel(0)->mpOglWidget = oglWidget;

  // now apply the whole configuration
  applyInit();
}


//----------------------------------------------------------------------------------------------------
// Constructor for a stereo grabber
cedar::dev::sensors::visual::OglGrabber::OglGrabber
(
  std::string configFileName,
  QGLWidget* oglWidget0,
  QGLWidget* oglWidget1
)
:
cedar::dev::sensors::visual::GrabberInterface(configFileName)
{
  // debug information logging
  cedar::aux::LogSingleton::getInstance()->allocating(this);

  // read initialization values from configuration file
  readInit(2,"StereoOglGrabber");

  // set additional parameters from the constuctor
  getChannel(0)->mpOglWidget = oglWidget0;
  getChannel(1)->mpOglWidget = oglWidget1;

  // now apply the whole configuration
  applyInit();
}

//----------------------------------------------------------------------------------------------------
// Destructor
cedar::dev::sensors::visual::OglGrabber::~OglGrabber()
{
  doCleanUp();
  cedar::aux::LogSingleton::getInstance()->freeing(this);

}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------
bool cedar::dev::sensors::visual::OglGrabber::onInit()
{
  std::stringstream init_message;
  init_message << ": Initialize grabber with " << mNumCams << " channels ..." << std::endl;
  for (unsigned int i = 0; i < mNumCams; ++i)
  {
    init_message << "Channel " << i <<std::endl;
        //<< ": capture from Source: " << getChannel(i)->mpOglWidget->WindowTitle() << std::endl;
  }
  cedar::aux::LogSingleton::getInstance()->message
                                           (
                                             ConfigurationInterface::getName() + init_message.str(),
                                             "cedar::dev::sensors::visual::OglGrabber::onInit()"
                                           );
  //Grab first frames
  onGrab();

  // all grabbers successfully initialized
  cedar::aux::LogSingleton::getInstance()->debugMessage
                                          (
                                           ConfigurationInterface::getName() + ": Initialization finished",
                                            "cedar::dev::sensors::visual::CameraGrabber::onInit()"
                                          );
  return true;
}

//----------------------------------------------------------------------------------------------------
void cedar::dev::sensors::visual::OglGrabber::onCleanUp()
{
  // do the cleanup of used hardware in this method
  // on an exception or a CTRL-C only onCleanUp will be invoked (no destructor)

  // delete all pointer-references to external widgets
  for (unsigned int channel = 0; channel < mNumCams; ++channel)
  {
    getChannel(channel)->mpOglWidget = NULL;
  }
}

//----------------------------------------------------------------------------------------------------
void cedar::dev::sensors::visual::OglGrabber::onAddChannel()
{
  // create the channel structure for one channel
  OglChannelPtr channel(new OglChannel);
  channel->mpOglWidget = NULL;
  mChannels.push_back(channel);
}

//----------------------------------------------------------------------------------------------------
bool cedar::dev::sensors::visual::OglGrabber::onDeclareParameters()
{
  // declare and initialize parameters and members of your derived class here
  // mCounter = 0 ;

  // if your parameters should be stored in the configfile,
  // the default-values will be set on new grabbers
  // return addParameter(&_mTest, "testparam", 123) == CONFIG_SUCCESS;
  return true;
}

//----------------------------------------------------------------------------------------------------
void cedar::dev::sensors::visual::OglGrabber::onUpdateSourceInfo(unsigned int channel)
{
  //value of channel is already checked by GraberInterface::getSourceInfo()
  getChannel(channel)->mChannelInfo = "QT::OGLWidget on Channel " + boost::lexical_cast<std::string>(channel);
}

//----------------------------------------------------------------------------------------------------
bool cedar::dev::sensors::visual::OglGrabber::onGrab()
{
  bool ogl_valid = true;
  for(unsigned int channel=0; channel<mNumCams;++channel)
  {
    if (getChannel(channel)->mpOglWidget != NULL)
    {
      QGLWidget* p_channel_widget = getChannel(channel)->mpOglWidget;

      // grab framebuffer without alpha-channel. possible values
      // GL_FRONT_LEFT, GL_FRONT_RIGHT, GL_BACK_LEFT, GL_BACK_RIGHT, GL_FRONT, GL_BACK, GL_LEFT, GL_RIGHT, GL_AUXi,
      // where i is between 0 and the value of GL_AUX_BUFFERS minus 1.

      glReadBuffer(GL_FRONT_RIGHT);
      QImage qimage = p_channel_widget->grabFrameBuffer(false);

      // QImage to cv::Mat
      cv::Mat mat = cv::Mat(qimage.height(), qimage.width(), CV_8UC4,(uchar*)qimage.bits(), qimage.bytesPerLine());
      cv::Mat mat2 = cv::Mat(mat.rows, mat.cols, CV_8UC3 );
      int from_to[] = { 0,0, 1,1, 2,2 };
      cv::mixChannels( &mat, 1, &mat2, 1, from_to, 3 );

      //apply the new content to the channel image
      getChannel(channel)->mImageMat = mat2.clone();
    }
    else
    {
      ogl_valid = false;
    }

    // if opengl context isn't valid, then an empty matrix will be return
    if (!ogl_valid)
    {
      getChannel(channel)->mImageMat = cv::Mat();
    }
  }

  return ogl_valid;
}


//----------------------------------------------------------------------------------------------------
void cedar::dev::sensors::visual::OglGrabber::setWidget(unsigned int channel, QGLWidget *oglWidget)
{
  if (channel >= mNumCams)
  {
    CEDAR_THROW
    (
      cedar::aux::IndexOutOfRangeException,
      "cedar::dev::sensors::visual::OglGrabber::setWidget"
    );
  }
  if (oglWidget != NULL)
  {

    bool restart_grabber = LoopedThread::isRunning();

    // stop grabbing thread if running
    if (restart_grabber)
    {
      this->stopGrabber();
    }

    // change source
    getChannel(channel)->mpOglWidget = oglWidget;
    cedar::aux::LogSingleton::getInstance()->message
                                            (
                                             ConfigurationInterface::getName() + ": New Widget applied",
                                              "cedar::dev::sensors::visual::OglGrabber::setWidget"
                                            );
    if (restart_grabber)
    {
      this->startGrabber();
    }
    else
    {
      this->grab();
    }
  }
}

