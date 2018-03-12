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

    File:        TestGrabber.cpp

    Maintainer:  Georg Hartinger
    Email:       georg.hartinger@ini.rub.de
    Date:        2011 08 01

    Description: Implementation of a Grabber for testing the Grabber interface

    Credits:

======================================================================================================================*/

// CEDAR INCLUDES
#include "cedar/testingUtilities/devices/TestGrabber.h"
#include "cedar/devices/sensors/visual/exceptions.h"


// SYSTEM INCLUDES
#include <iostream>
#include <sstream>

//----------------------------------------------------------------------------------------------------------------------
// register the class
//----------------------------------------------------------------------------------------------------------------------
namespace
{
  bool declared
    = cedar::dev::sensors::visual::Grabber::ChannelManagerSingleton::getInstance()
        ->registerType<cedar::test::dev::TestChannelPtr>();
}

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

// Constructor for a single-channel grabber
cedar::test::dev::TestGrabber::TestGrabber
(
  const std::string& sourceFileName
)
:
cedar::dev::sensors::visual::Grabber
(
  "TestGrabber",
  cedar::test::dev::TestChannelPtr
  (
    new cedar::test::dev::TestChannel(sourceFileName)
  )
),
_mTestParam(new cedar::aux::IntParameter(this, "testparameter", 0))
{

  std::cout << "TestGrabber::TestGrabber()" << std::endl;

  // information logging
  cedar::aux::LogSingleton::getInstance()->allocating(this);

  cedar::aux::LogSingleton::getInstance()->debugMessage
                                          (
                                           this->getName() + ": Create a single channel grabber",
                                            "cedar::test::dev::TestGrabber::TestGrabber()"
                                          );
}


// Constructor for a stereo grabber
cedar::test::dev::TestGrabber::TestGrabber
(
  const std::string& sourceFileName0,
  const std::string& sourceFileName1
)
:
cedar::dev::sensors::visual::Grabber
(
  "StereoTestGrabber",
  cedar::test::dev::TestChannelPtr
  (
    new cedar::test::dev::TestChannel(sourceFileName0)
  ),
  cedar::test::dev::TestChannelPtr
  (
    new cedar::test::dev::TestChannel(sourceFileName1)
  )
),
_mTestParam(new cedar::aux::IntParameter(this, "testparameter", 0))
{

  std::cout << "TestGrabber::TestGrabber() STEREO" << std::endl;

  // debug information logging
  cedar::aux::LogSingleton::getInstance()->allocating(this);

  cedar::aux::LogSingleton::getInstance()->debugMessage
                                          (
                                           this->getName() + ": Create a stereo channel grabber",
                                            "cedar::test::dev::TestGrabber::TestGrabber()"
                                          );
}


// Destructor
cedar::test::dev::TestGrabber::~TestGrabber()
{
  // call of doCleanup, to do the necessarily cleanup in Grabber
  doCleanUp();

  // do memory de-allocation in the destructor
  // all stuff in the mChannels vector is cleared by the shared pointer

  cedar::aux::LogSingleton::getInstance()->debugMessage
                                          (
                                           this->getName() + ": destructor",
                                            "cedar::test::dev::TestGrabber::~TestGrabber()"
                                          );


  // debug logging
  cedar::aux::LogSingleton::getInstance()->freeing(this);

}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::test::dev::TestGrabber::onCreateGrabber()
{
  std::cout << "TestGrabber::onCreateGrabber()" << std::endl;

  // do the initialization of your Grabber in this method,
  // grab the first pictures and initialize the mImageMatVector with
  // these pictures

  // if there where errors, throw a CreateGrabberException.
  // In this case, the base Grabber() class will envoke closeGrabber()
  // so be sure, that onCloseGrabber() resets the Grabber to initialization state.

  //-------------------------------------------------
  // init message
  std::stringstream init_message;
  unsigned int num_channels = getNumChannels();
  init_message << ": Initialize test grabber with " << getNumChannels() << " channels ..." << std::endl;
  for(unsigned int channel = 0; channel < num_channels; ++channel)
  {
    init_message << "Channel " << channel << ": capture from Source: "
                 << getTestChannel(channel)->_mSourceFileName->getPath(true) << std::endl;
  }
  cedar::aux::LogSingleton::getInstance()->message
                                           (
                                             this->getName() + init_message.str(),
                                             "cedar::test::dev::TestGrabber::onCreateGrabber()"
                                           );

  //-------------------------------------------------
  // load pictures one by one
  for(unsigned int channel = 0; channel < num_channels; ++channel)
  {

    // here we apply empty 1x1 matrizes to test the interface
    cv::Mat frame=cv::Mat::zeros(1, 1, CV_8UC3);

    // apply the new content to the channel image
    getImageMat(channel) = frame;

    // on errors, throw an exception
    if (frame.empty())
    {
      CEDAR_THROW(cedar::dev::sensors::visual::CreateGrabberException,"Error, frame is empty")
    }
  }


  // all grabbers successfully initialized
  cedar::aux::LogSingleton::getInstance()->debugMessage
                                          (
                                           this->getName() + ": Initialization finished",
                                            "cedar::test::dev::TestGrabber::onCreateGrabber()"
                                          );

}


void cedar::test::dev::TestGrabber::onCleanUp()
{
  // do the cleanup of used hardware in this method
  // on an exception or a CTRL-C only onCleanUp will be invoked (no destructor)
  cedar::aux::LogSingleton::getInstance()->debugMessage
                                          (
                                           this->getName() + ": Cleaning up",
                                            "cedar::test::dev::TestGrabber::onCleanUp()"
                                          );
}


void cedar::test::dev::TestGrabber::onCloseGrabber()
{
  // close all opened files here and reset the grabber to initialization state
  // no files should be opened.
  // after this method the createGrabber() can savely invoked.
  // the image-matrices of the channels are already reseted in the base Grabber.closeGrabber() method
}


std::string cedar::test::dev::TestGrabber::onGetSourceInfo(unsigned int channel)
{

  // no range-check is needed, because this method is a private method

  // give some information about the used source like channelname, filename, devicename
  // or something like that
  return this->getName() + " Channel " + cedar::aux::toString(channel)
                               + " : "+ getTestChannel(channel)->_mSourceFileName->getPath(true);
}


void cedar::test::dev::TestGrabber::onGrab(unsigned int channel)
{
  // this is the main grabbing method.
  // read a new picture from the source and set the picture in the channel image buffer

  // apply the new content to the channel image
  // getTestChannel(channel)->mImageMat = grab_new_content();

  // check on errors an throw an exception
  if (getTestChannel(channel)->mImageMat.empty())
  {
    std::string msg = "Channel " + cedar::aux::toString(channel) + ": An error occurred";
    CEDAR_THROW(cedar::dev::sensors::visual::GrabberGrabException,msg);
  }

  // here we just want to count how often onGrab is invoked, due to a fps-check
  if (channel == 0)
  {
    mCounter ++;
  }
}


unsigned int cedar::test::dev::TestGrabber::getCounter()
{
  // a simple get-method
  unsigned int ct = mCounter;
  mCounter = 0;
  return ct;
}


int cedar::test::dev::TestGrabber::getTestParam() const
{
  // a simple get-method
  return _mTestParam->getValue();
}


void cedar::test::dev::TestGrabber::setTestParam(int testParameter)
{
  // a simple set-method
  _mTestParam->setValue(testParameter);
}


std::string cedar::test::dev::TestGrabber:: getSourceFileName(unsigned int channel) const
{
  return getTestChannel(channel)->_mSourceFileName->getPath(true);
}

