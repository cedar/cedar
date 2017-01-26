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
#include "cedar/devices/sensors/visual/exceptions.h"


// SYSTEM INCLUDES

//----------------------------------------------------------------------------------------------------------------------
// register the class
//----------------------------------------------------------------------------------------------------------------------
namespace
{
  bool declared
    = cedar::dev::sensors::visual::Grabber::ChannelManagerSingleton::getInstance()
        ->registerType<cedar::dev::sensors::visual::GrabbableChannelPtr>();
}

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

// Constructor for a single-channel grabber
cedar::dev::sensors::visual::GrabbableGrabber::GrabbableGrabber
(
  cedar::aux::Grabbable* grabbableObject,
  const std::string& grabberName
)
:
cedar::dev::sensors::visual::Grabber
(
  grabberName,
  cedar::dev::sensors::visual::GrabbableChannelPtr
  (
    new cedar::dev::sensors::visual::GrabbableChannel(grabbableObject)
  )
)
{
  cedar::aux::LogSingleton::getInstance()->allocating(this);
}


// Constructor for a stereo grabber
cedar::dev::sensors::visual::GrabbableGrabber::GrabbableGrabber
(
  cedar::aux::Grabbable* grabbableObject0,
  cedar::aux::Grabbable* grabbableObject1,
  const std::string& grabberName
)
:
cedar::dev::sensors::visual::Grabber
(
  grabberName,
  cedar::dev::sensors::visual::GrabbableChannelPtr
  (
    new cedar::dev::sensors::visual::GrabbableChannel(grabbableObject0)
  ),
  cedar::dev::sensors::visual::GrabbableChannelPtr
  (
    new cedar::dev::sensors::visual::GrabbableChannel(grabbableObject1)
  )
)
{
  cedar::aux::LogSingleton::getInstance()->allocating(this);
}

// Destructor
cedar::dev::sensors::visual::GrabbableGrabber::~GrabbableGrabber()
{
  // call of doCleanup, to do the necessarily cleanup in Grabber
  doCleanUp();

  // do memory de-allocation in the destructor
  // all stuff in the mChannels vector is cleared by the shared pointer

  // cedar logging
  cedar::aux::LogSingleton::getInstance()->freeing(this);

}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::dev::sensors::visual::GrabbableGrabber::onCreateGrabber()
{
  // create
  unsigned int num_channels = getNumChannels();

  // register this grabber in the grabbable object and store the readwrite-lock
  for (unsigned int channel = 0; channel < num_channels;++channel)
  {
    getGrabbableChannel(channel)->mpGrabberLock
      = getGrabbableChannel(channel)->mpSourceInterfaceClass->registerGrabber();

    // check if successfully registered
    if (!getGrabbableChannel(channel)->mpGrabberLock)
    {
      std::string msg = "Error while register the grabber in the grabbable object "
                        "(perhaps already an other grabber active?)";
      CEDAR_THROW(cedar::dev::sensors::visual::CreateGrabberException,msg);
    }
  }
}

void cedar::dev::sensors::visual::GrabbableGrabber::onCloseGrabber()
{
  this->onCleanUp();
}


void cedar::dev::sensors::visual::GrabbableGrabber::onCleanUp()
{
  // do the cleanup of used hardware in this method
  // on an exception or a CTRL-C only onCleanUp will be invoked (no destructor)
  unsigned int num_channels = getNumChannels();
  for (unsigned int channel = 0; channel < num_channels; ++channel)
  {
    getGrabbableChannel(channel)->mpSourceInterfaceClass->deregisterGrabber(getGrabbableChannel(channel)->mpGrabberLock);
    // remove the references to the external classes
    getGrabbableChannel(channel)->mpSourceInterfaceClass = NULL;
    getGrabbableChannel(channel)->mpGrabberLock = NULL;
  }
}


std::string cedar::dev::sensors::visual::GrabbableGrabber::onGetSourceInfo(unsigned int channel)
{
  return this->getName() + ": Channel " + cedar::aux::toString(channel) + ": "
               + typeid(getGrabbableChannel(channel)->mpSourceInterfaceClass).name();
}


void cedar::dev::sensors::visual::GrabbableGrabber::onGrab(unsigned int channel)
{
  try
  {
    // lock grabbable object and grab frame
    QReadLocker locking(getGrabbableChannel(channel)->mpGrabberLock);
    getImageMat(channel) = getGrabbableChannel(channel)->mpSourceInterfaceClass->grabImage().clone();
  }
  catch(std::exception& e)
  {
    std::string msg = "Exception on channel " + cedar::aux::toString(channel) + ": " + e.what();
    CEDAR_THROW(cedar::dev::sensors::visual::GrabberGrabException,msg)
  }
  // unlocking done by QReadLocker
}

