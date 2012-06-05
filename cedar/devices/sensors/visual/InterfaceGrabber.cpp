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

    File:        InterfaceGrabber.cpp

    Maintainer:  Georg Hartinger
    Email:       georg.hartinger@ini.rub.de
    Date:        2011 08 01

    Description: Implementation of a Grabber for testing the Grabber interface

    Credits:

======================================================================================================================*/

// CEDAR INCLUDES
#include "cedar/devices/sensors/visual/InterfaceGrabber.h"

// SYSTEM INCLUDES

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------
// Constructor for a single-channel grabber
cedar::dev::sensors::visual::InterfaceGrabber::InterfaceGrabber
(
  std::string configFileName,
  cedar::dev::sensors::visual::Grabbable* sourceInterfaceClass
)
:
cedar::dev::sensors::visual::Grabber(configFileName)
{
  //information logging
  cedar::aux::LogSingleton::getInstance()->allocating(this);
  cedar::aux::LogSingleton::getInstance()->message
                                          (
                                           ConfigurationInterface::getName() + ": Create a single channel grabber",
                                            "cedar::dev::sensors::visual::InterfaceGrabber::InterfaceGrabber()"
                                          );

  //read initialization values from configuration file
  readInit(1,"InterfaceGrabber");

  //overwrite parameters from configfiles with values from constuctor
  getChannel(0)->mpSourceInterfaceClass = sourceInterfaceClass;

  //now apply the whole configuration
  applyInit();

  std::cout << "InterfaceGrabber::InterfaceGrabber() finished" << std::endl;
}


//----------------------------------------------------------------------------------------------------
// Constructor for a stereo grabber
cedar::dev::sensors::visual::InterfaceGrabber::InterfaceGrabber
(
  std::string configFileName,
  cedar::dev::sensors::visual::Grabbable* sourceInterfaceClass0,
  cedar::dev::sensors::visual::Grabbable* sourceInterfaceClass1
)
:
cedar::dev::sensors::visual::Grabber(configFileName)
{
  // information logging
  cedar::aux::LogSingleton::getInstance()->allocating(this);

  cedar::aux::LogSingleton::getInstance()->message
                                          (
                                           ConfigurationInterface::getName() + ": Create a stereo channel grabber",
                                            "cedar::dev::sensors::visual::InterfaceGrabber::InterfaceGrabber()"
                                          );

  //read initialization values from configuration file
  readInit(2,"StereoInterfaceGrabber");

  //overwrite parameters from configfiles with values from constuctor
  getChannel(0)->mpSourceInterfaceClass = sourceInterfaceClass0;
  getChannel(1)->mpSourceInterfaceClass = sourceInterfaceClass1;

  //now apply the whole configuration
  applyInit();
}

//----------------------------------------------------------------------------------------------------
// Destructor
cedar::dev::sensors::visual::InterfaceGrabber::~InterfaceGrabber()
{
  //call of doCleanup, to do the necessarily cleanup in Grabber
  doCleanUp();

  //do memory de-allocation in the destructor
  //all stuff in the mChannels vector is cleared by the shared pointer

  //debug logging
  cedar::aux::LogSingleton::getInstance()->freeing(this);

}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------
bool cedar::dev::sensors::visual::InterfaceGrabber::onInit()
{
  std::stringstream init_message;
  init_message << ": Initialize test grabber with " << mNumCams << " channels ..." << std::endl;
  for (unsigned int i = 0; i < mNumCams; ++i)
  {
    init_message << "Channel " << i << ": capture from Source: "
                 << typeid(getChannel(i)->mpSourceInterfaceClass).name() << std::endl;
  }
  cedar::aux::LogSingleton::getInstance()->debugMessage
                                           (
                                             ConfigurationInterface::getName() + init_message.str(),
                                             "cedar::dev::sensors::visual::InterfaceGrabber::onInit()"
                                           );

  //load pictures one by one
  for(unsigned int channel=0; channel<mNumCams;++channel)
  {
    getChannel(channel)->mpGrabberLock = getChannel(channel)->mpSourceInterfaceClass->registerGrabber();

    //check if successfully registered
    if (! getChannel(channel)->mpGrabberLock)
    {
      return false;
    }
  }

  //grab first image
  onGrab();

  //no exception until now
  return true;
}

//----------------------------------------------------------------------------------------------------
void cedar::dev::sensors::visual::InterfaceGrabber::onCleanUp()
{
  //do the cleanup of used hardware in this method
  //on an exception or a CTRL-C only onCleanUp will be invoked (no destructor)

  for(unsigned int channel=0; channel<mNumCams;++channel)
  {
    getChannel(channel)->mpSourceInterfaceClass->deregisterGrabber(getChannel(channel)->mpGrabberLock);
    //remove the references to the external classes
    getChannel(channel)->mpSourceInterfaceClass = NULL;
    getChannel(channel)->mpGrabberLock = NULL;
  }

}

//----------------------------------------------------------------------------------------------------
void cedar::dev::sensors::visual::InterfaceGrabber::onAddChannel()
{
  //create the channel structure for one channel
  InterfaceChannelPtr channel(new InterfaceChannel);
  channel->mpSourceInterfaceClass=NULL;
  channel->mpGrabberLock=NULL;
  mChannels.push_back(channel);
}

//----------------------------------------------------------------------------------------------------
bool cedar::dev::sensors::visual::InterfaceGrabber::onDeclareParameters()
{
  return true;
}

//----------------------------------------------------------------------------------------------------
void cedar::dev::sensors::visual::InterfaceGrabber::onUpdateSourceInfo(unsigned int channel)
{
  getChannel(channel)->mChannelInfo = this->getName() + "> Channel " + boost::lexical_cast<std::string>(channel) + ": "
                                      + typeid(getChannel(channel)->mpSourceInterfaceClass).name();
}

//----------------------------------------------------------------------------------------------------
bool cedar::dev::sensors::visual::InterfaceGrabber::onGrab()
{
  bool result = true;
  for(unsigned int channel=0; channel<mNumCams;++channel)
   {
     //apply the new content to the channel image
     try
     {
       getChannel(channel)->mpGrabberLock->lockForRead();
       getChannel(channel)->mImageMat = getChannel(channel)->mpSourceInterfaceClass->grabImage().clone();
       getChannel(channel)->mpGrabberLock->unlock();
     }
     catch(...)
     {
       result = false;
     }
   }
  return result;
}

