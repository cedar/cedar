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

    File:        GrabbableGrabber.cpp

    Maintainer:  Georg Hartinger
    Email:       georg.hartinger@ini.rub.de
    Date:        2011 08 01

    Description: Implementation of a Grabber for testing the Grabber interface

    Credits:

======================================================================================================================*/

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/devices/sensors/visual/GrabbableGrabber.h"

// SYSTEM INCLUDES

//----------------------------------------------------------------------------------------------------------------------
// register the class
//----------------------------------------------------------------------------------------------------------------------
namespace
{
  bool declared
    = cedar::dev::sensors::visual::Grabber::ChannelManagerSingleton::getInstance()
        ->registerType<cedar::dev::sensors::visual::GrabbableGrabber::GrabbableChannelPtr>();
}

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------
// Constructor for a single-channel grabber
cedar::dev::sensors::visual::GrabbableGrabber::GrabbableGrabber
(
  cedar::dev::sensors::visual::Grabbable* grabbable,
  const std::string& grabberName
)
:
cedar::dev::sensors::visual::Grabber
(
  grabberName,
  cedar::dev::sensors::visual::GrabbableGrabber::GrabbableChannelPtr
  (
    new cedar::dev::sensors::visual::GrabbableGrabber::GrabbableChannel(grabbable)
  )
)
{
  cedar::aux::LogSingleton::getInstance()->allocating(this);
}


//----------------------------------------------------------------------------------------------------
// Constructor for a stereo grabber
cedar::dev::sensors::visual::GrabbableGrabber::GrabbableGrabber
(
  cedar::dev::sensors::visual::Grabbable* grabbable0,
  cedar::dev::sensors::visual::Grabbable* grabbable1,
  const std::string& grabberName
)
:
cedar::dev::sensors::visual::Grabber
(
  grabberName,
  cedar::dev::sensors::visual::GrabbableGrabber::GrabbableChannelPtr
  (
    new cedar::dev::sensors::visual::GrabbableGrabber::GrabbableChannel(grabbable0)
  ),
  cedar::dev::sensors::visual::GrabbableGrabber::GrabbableChannelPtr
  (
    new cedar::dev::sensors::visual::GrabbableGrabber::GrabbableChannel(grabbable1)
  )
)
{
  cedar::aux::LogSingleton::getInstance()->allocating(this);
}

//----------------------------------------------------------------------------------------------------
// Destructor
cedar::dev::sensors::visual::GrabbableGrabber::~GrabbableGrabber()
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
bool cedar::dev::sensors::visual::GrabbableGrabber::onInit()
{
  std::stringstream init_message;
  init_message << ": Initialize test grabber with " << mNumCams << " channels ..." << std::endl;
  for (unsigned int i = 0; i < mNumCams; ++i)
  {
    init_message << "Channel " << i << ": capture from Source: "
                 << typeid(getGrabbableChannel(i)->mpSourceInterfaceClass).name() << std::endl;
  }
  cedar::aux::LogSingleton::getInstance()->debugMessage
                                           (
                                             this->getName() + init_message.str(),
                                             "cedar::dev::sensors::visual::GrabbableGrabber::onInit()"
                                           );

  //load pictures one by one
  for(unsigned int channel=0; channel<mNumCams;++channel)
  {
    getGrabbableChannel(channel)->mpGrabberLock = getGrabbableChannel(channel)->mpSourceInterfaceClass->registerGrabber();

    //check if successfully registered
    if (! getGrabbableChannel(channel)->mpGrabberLock)
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
void cedar::dev::sensors::visual::GrabbableGrabber::onCleanUp()
{
  //do the cleanup of used hardware in this method
  //on an exception or a CTRL-C only onCleanUp will be invoked (no destructor)

  for(unsigned int channel=0; channel<mNumCams;++channel)
  {
    getGrabbableChannel(channel)->mpSourceInterfaceClass->deregisterGrabber(getGrabbableChannel(channel)->mpGrabberLock);
    //remove the references to the external classes
    getGrabbableChannel(channel)->mpSourceInterfaceClass = NULL;
    getGrabbableChannel(channel)->mpGrabberLock = NULL;
  }

}

//----------------------------------------------------------------------------------------------------
//void cedar::dev::sensors::visual::GrabbableGrabber::onAddChannel()
//{
//  //create the channel structure for one channel
//  InterfaceChannelPtr channel(new InterfaceChannel);
//  channel->mpSourceInterfaceClass=NULL;
//  channel->mpGrabberLock=NULL;
//  mChannels.push_back(channel);
//}

//----------------------------------------------------------------------------------------------------
void cedar::dev::sensors::visual::GrabbableGrabber::onUpdateSourceInfo(unsigned int channel)
{
  getGrabbableChannel(channel)->mChannelInfo = this->getName() + ": Channel "
                                      + boost::lexical_cast<std::string>(channel) + ": "
                                      + typeid(getGrabbableChannel(channel)->mpSourceInterfaceClass).name();
}

//----------------------------------------------------------------------------------------------------
bool cedar::dev::sensors::visual::GrabbableGrabber::onGrab()
{
  bool result = true;
  for(unsigned int channel=0; channel<mNumCams;++channel)
   {
     //apply the new content to the channel image
     try
     {
       getGrabbableChannel(channel)->mpGrabberLock->lockForRead();
       getGrabbableChannel(channel)->mImageMat = getGrabbableChannel(channel)->mpSourceInterfaceClass->grabImage().clone();
       getGrabbableChannel(channel)->mpGrabberLock->unlock();
     }
     catch(...)
     {
       result = false;
     }
   }
  return result;
}

