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
  cedar::aux::LogSingleton::getInstance()->debugMessage
                                          (
                                           ConfigurationInterface::getName() + ": Create a single channel grabber",
                                            "cedar::dev::sensors::visual::CameraGrabber::OglGrabber()"
                                          );

  //debug information logging
  cedar::aux::LogSingleton::getInstance()->allocating(this);

  //read initialization values from configuration file
  readInit(1,"OglGrabber");

  //overwrite parameters from configfiles with values from constuctor
  getChannel(0)->mpOglWidget = oglWidget;

  //now apply the whole configuration
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
  //debug information logging
  cedar::aux::LogSingleton::getInstance()->allocating(this);

  cedar::aux::LogSingleton::getInstance()->debugMessage
                                          (
                                           ConfigurationInterface::getName() + ": Create a stereo channel grabber",
                                            "cedar::dev::sensors::visual::CameraGrabber::OglGrabber()"
                                          );

  //read initialization values from configuration file
  readInit(2,"StereoOglGrabber");

  //overwrite parameters from configfiles with values from constuctor
  getChannel(0)->mpOglWidget = oglWidget0;
  getChannel(1)->mpOglWidget = oglWidget1;

  //now apply the whole configuration
  applyInit();
}

//----------------------------------------------------------------------------------------------------
// Destructor
cedar::dev::sensors::visual::OglGrabber::~OglGrabber()
{
  //call of doCleanup, to do the necessarily cleanup in GrabberInterface
  doCleanUp();

  //do memory de-allocation in the destructor
  //all stuff in the mChannels vector is cleared by the shared pointer

  cedar::aux::LogSingleton::getInstance()->debugMessage
                                          (
                                           ConfigurationInterface::getName() + ": destructor",
                                            "cedar::dev::sensors::visual::CameraGrabber::~OglGrabber()"
                                          );


  //debug logging
  cedar::aux::LogSingleton::getInstance()->freeing(this);

}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------
bool cedar::dev::sensors::visual::OglGrabber::onInit()
{

  //do the initialization of your Grabber in this method,
  //grab the first pictures and initialize the mImageMatVector with
  //these pictures

  //-------------------------------------------------
  std::stringstream init_message;
  init_message << ": Initialize test grabber with " << mNumCams << " channels ..." << std::endl;
  for (unsigned int i = 0; i < mNumCams; ++i)
  {
    init_message << "Channel " << i <<std::endl;
        //<< ": capture from Source: " << getChannel(i)->mpOglWidget->WindowTitle() << std::endl;
  }
  cedar::aux::LogSingleton::getInstance()->systemInfo
                                           (
                                             ConfigurationInterface::getName() + init_message.str(),
                                             "cedar::dev::sensors::visual::OglGrabber::onInit()"
                                           );

  //-------------------------------------------------
  //load pictures one by one
  for(unsigned int channel=0; channel<mNumCams;++channel)
  {
    //there is no need to create new matrices, empty ones are
    //already initialized within the channel structure
    cv::Mat frame=cv::Mat();

    //apply the new content to the channel image
    getChannel(channel)->mImageMat = frame;
  }


  // all grabbers successfully initialized
  cedar::aux::LogSingleton::getInstance()->debugMessage
                                          (
                                           ConfigurationInterface::getName() + ": Initialization finished",
                                            "cedar::dev::sensors::visual::CameraGrabber::onInit()"
                                          );
  return true;

  //to test the initialize exception
  //return false;
}

//----------------------------------------------------------------------------------------------------
void cedar::dev::sensors::visual::OglGrabber::onCleanUp()
{
  //do the cleanup of used hardware in this method
  //on an exception or a CTRL-C only onCleanUp will be invoked (no destructor)
  cedar::aux::LogSingleton::getInstance()->debugMessage
                                          (
                                           ConfigurationInterface::getName() + ": Cleaning up",
                                            "cedar::dev::sensors::visual::CameraGrabber::onCleanUp()"
                                          );
}

//----------------------------------------------------------------------------------------------------
void cedar::dev::sensors::visual::OglGrabber::onAddChannel()
{
  //create the channel structure for one channel
  OglChannelPtr channel(new OglChannel);
  channel->mpOglWidget = NULL;
  mChannels.push_back(channel);
}

//----------------------------------------------------------------------------------------------------
bool cedar::dev::sensors::visual::OglGrabber::onDeclareParameters()
{
  //declare and initialize parameters and members of your derived class here
  mCounter = 0 ;

  //if your parameters should be stored in the configfile,
  //the default-values will be set on new grabbers
  return addParameter(&_mTest, "testparam", 123) == CONFIG_SUCCESS;
}

//----------------------------------------------------------------------------------------------------
const std::string& cedar::dev::sensors::visual::OglGrabber::onGetSourceInfo(unsigned int channel) const
{
  //this is the only pure virtual method of the GrabberInterface class

  //no range-check is needed, because this is done in the GrabberInterface::getSourceInfo method

  //give some information about the used source like channelname, filename, devicename
  //or something like that

  //return getChannel(channel)->mSourceFileName;
  return "QT::OGLWidget on Channel " + boost::lexical_cast<std::string>(channel);
}

//----------------------------------------------------------------------------------------------------
bool cedar::dev::sensors::visual::OglGrabber::onGrab()
{
  //this is the main grabbing method.
  //read a new picture from the source and set the picture in the mImageMatVector.at()

  for(unsigned int channel=0; channel<mNumCams;++channel)
   {
     //apply the new content to the channel image
     //getChannel(channel)->mImageMat = <grab_new_content>;
   }

  //here we just want to count how often onGrab is invoked, due to a fps-check
  mCounter ++;
  return true;
}

//----------------------------------------------------------------------------------------------------
unsigned int cedar::dev::sensors::visual::OglGrabber::getCounter()
{
  //a simple get-method
  unsigned int ct = mCounter;
  mCounter = 0;
  return ct;
}

//----------------------------------------------------------------------------------------------------
int cedar::dev::sensors::visual::OglGrabber::getTestParam()
{
  //a simple get-method
  return _mTest;
}

//----------------------------------------------------------------------------------------------------
void cedar::dev::sensors::visual::OglGrabber::setTestParam(int mTest)
{
  //a simple set-method
  _mTest=mTest;
}
