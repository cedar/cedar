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

    File:        NetGrabber.cpp

    Maintainer:  Georg.Hartinger
    Email:       georg.hartinger@rub.de
    Date:        2011 08 01

    Description: Implementation of the @em cedar::dev::sensors::visual::NetGrabber class.

    Credits:

======================================================================================================================*/


// MAKE YARP OPTIONAL
#include "cedar/configuration.h"
#ifdef CEDAR_USE_YARP

// CEDAR INCLUDES
#include "cedar/devices/sensors/visual/NetGrabber.h"
#include "cedar/devices/sensors/visual/exceptions.h"
#include "cedar/auxiliaries/net/Reader.h"
#include "cedar/auxiliaries/net/exceptions.h"
#include "cedar/auxiliaries/exceptions.h"
#include "cedar/auxiliaries/sleepFunctions.h"
#include "cedar/units/Time.h"
#include "cedar/units/prefixes.h"

// SYSTEM INCLUDES
#include <sstream>


//----------------------------------------------------------------------------------------------------------------------
// register the class
//----------------------------------------------------------------------------------------------------------------------
namespace
{
  bool declared
    = cedar::dev::sensors::visual::Grabber::ChannelManagerSingleton::getInstance()
        ->registerType<cedar::dev::sensors::visual::NetChannelPtr>();
}

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------


// Constructor for single-channel grabber
cedar::dev::sensors::visual::NetGrabber::NetGrabber
(
  const std::string& yarpChannelName
)
:
cedar::dev::sensors::visual::Grabber
(
  "NetGrabber",
  cedar::dev::sensors::visual::NetChannelPtr
  (
    new cedar::dev::sensors::visual::NetChannel(yarpChannelName)
  )
)
{
  cedar::aux::LogSingleton::getInstance()->allocating(this);
}


// Constructor for stereo-channel grabber
cedar::dev::sensors::visual::NetGrabber::NetGrabber
(
  const std::string& yarpChannelName0,
  const std::string& yarpChannelName1
)
:
cedar::dev::sensors::visual::Grabber
(
  "StereoNetGrabber",
  cedar::dev::sensors::visual::NetChannelPtr
  (
    new cedar::dev::sensors::visual::NetChannel(yarpChannelName0)
  ),
  cedar::dev::sensors::visual::NetChannelPtr
  (
    new cedar::dev::sensors::visual::NetChannel(yarpChannelName1)
  )
)
{
  cedar::aux::LogSingleton::getInstance()->allocating(this);
}

cedar::dev::sensors::visual::NetGrabber::~NetGrabber()
{
  doCleanUp();
  cedar::aux::LogSingleton::getInstance()->freeing(this);
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------


void cedar::dev::sensors::visual::NetGrabber::onCloseGrabber()
{
  unsigned int num_channels = getNumChannels();
  for(unsigned int channel = 0; channel < num_channels; ++channel)
  {
    getNetChannel(channel)->mpMatNetReader.reset();
  }
}

void cedar::dev::sensors::visual::NetGrabber::onCleanUp()
{
  cedar::aux::LogSingleton::getInstance()->debugMessage
                                           (
                                             this->getName() + ": onCleanUp()",
                                             "cedar::dev::sensors::visual::NetGrabber::onCleanUp()"
                                           );
  // close all captures
  // mNetReaders.clear(); done in Grabber
}

void cedar::dev::sensors::visual::NetGrabber::onCreateGrabber()
{
  // create debug-message
  const std::string method_name="cedar::dev::sensors::visual::NetGrabber::onCreateGrabber()";
  unsigned int num_channels = getNumChannels();
  std::stringstream info;
  info << "Initialize NetGrabber with " << num_channels << " channels ..." << std::endl;

  for(unsigned int channel = 0; channel < num_channels; ++channel)
  {
    info << "Channel " << channel << ": capture from YARP-hannel: "
         << getNetChannel(channel)->_mpYarpChannelName->getValue()<< "\n";
  }

  cedar::aux::LogSingleton::getInstance()->debugMessage(this->getName() + ": " + info.str(),method_name);

  //-------------------------------------------------
  // open capture one by one
  //-------------------------------------------------

  cedar::dev::sensors::visual::NetChannel::MatNetReaderPtr yarp_reader;

  // loop until connection established or an error occurs
  for (unsigned int channel = 0; channel < num_channels; ++channel)
  {
    const std::string channel_name = getNetChannel(channel)->_mpYarpChannelName->getValue();

    cedar::aux::LogSingleton::getInstance()->debugMessage
                                             (
                                               this->getName() + ": Create channel "
                                                 + cedar::aux::toString(channel) + ": "
                                                 + channel_name,
                                               method_name
                                             );

    //-------------------------------------------------
    // try to connect to channel for about 1 second per channel
    //-------------------------------------------------
    int timeout_counter = 0;
    do
    {
      try
      {
        // try to establish connection
        yarp_reader = cedar::dev::sensors::visual::NetChannel::MatNetReaderPtr
                      (
                        new cedar::dev::sensors::visual::NetChannel::MatNetReader(channel_name)
                      );

        cedar::aux::LogSingleton::getInstance()->debugMessage("Connection established", method_name);
      }
      catch (cedar::aux::net::NetWaitingForWriterException &E)
      {
        // An error occured: No Yarp writer with appropriate channel name
        // This could be normal on startup, so try again for about 10 times

        if (++timeout_counter > 10)
        {
          std::string msg = this->getName() + ": Couldn't open the YARP channel \""+channel_name+"\"\nGiving up!";
          CEDAR_THROW(cedar::dev::sensors::visual::CreateGrabberException,msg);
        }
        else
        {
          std::string msg = this->getName() + ": Couldn't open the YARP channel \""+channel_name+"\"\nTrying again...";
          cedar::aux::LogSingleton::getInstance()->warning(msg, method_name);
          cedar::aux::sleep(cedar::unit::Time(100.0 * cedar::unit::milli * cedar::unit::second));
        }
      }
      catch (cedar::aux::ExceptionBase& e)
      {
        std::string msg = this->getName() + ": Unknown Error on initialization of NetReader for channel \""
                                          + channel_name + "\" :" + e.getMessage();
        CEDAR_THROW(cedar::dev::sensors::visual::CreateGrabberException,msg);
      }

    } while (!yarp_reader.get()); // check the raw pointer

    //connection established
    getNetChannel(channel)->mpMatNetReader = yarp_reader;


    //-------------------------------------------------
    // try to get the first frame
    //-------------------------------------------------
    std::string msg = this->getName() + ": Try to grab from channel " + cedar::aux::toString(channel);
    cedar::aux::LogSingleton::getInstance()->debugMessage(msg, method_name);

    cv::Mat frame;
    timeout_counter = 0;
    bool reading_ok = false;

    // loop until first image received for about two seconds
    do
    {
      try
      {
        //try to get first frame, if no data there, an exception will be thrown
        frame = getNetChannel(channel)->mpMatNetReader->read();

        //got frame
        cedar::aux::LogSingleton::getInstance()->debugMessage("Got first frame", method_name);
        getImageMat(channel) = frame;
        reading_ok = true;
      }
      catch (cedar::aux::net::NetUnexpectedDataException &E)
      {
        if (++timeout_counter > 40)
        {
          std::string msg = this->getName() + ": Couldn't retrieve an image from YARP-channel \"" + channel_name + "\"";
          CEDAR_THROW(cedar::dev::sensors::visual::CreateGrabberException,msg);
        }
        else
        {
          reading_ok = false;
          cedar::aux::sleep(cedar::unit::Time(50.0 * cedar::unit::milli * cedar::unit::seconds));
        }
      }
      catch (std::exception& e)
      {
        std::string msg = this->getName() + ": Unknown error while trying to retrieve an image from YARP-channel \""
                          + channel_name + "\" :" + e.what();
        cedar::aux::LogSingleton::getInstance()->error(msg, method_name);
        CEDAR_THROW(cedar::dev::sensors::visual::CreateGrabberException,msg);
      }

    } while (!reading_ok);
  }

  // all grabbers successfully initialized
  cedar::aux::LogSingleton::getInstance()->debugMessage(this->getName() + ": Initializtion finished", method_name);


  //-------------------------------------------------
  // determine framerate of the source
  //-------------------------------------------------

  //!@todo get fps from source: measure the difference between two frames
  this->setFramerate(100.f);
}

std::string cedar::dev::sensors::visual::NetGrabber::onGetSourceInfo(unsigned int channel)
{
  return this->getName() + ": " + getNetChannel(channel)->_mpYarpChannelName->getValue();
}

void cedar::dev::sensors::visual::NetGrabber::onGrab(unsigned int channel)
{
  try
  {
    // the read() function pop the image from the NetReader class.
    // If already read, then an exception is thrown.
    getImageMat(channel) =  getNetChannel(channel)->mpMatNetReader->read();
  }
  catch (cedar::aux::net::NetUnexpectedDataException &E)
  {
    // no new frame (could be ignored, because the old frame is in the image buffer)
  }
  catch (std::exception& e)
  {
    std::string msg = "Unknown exception on channel " + cedar::aux::toString(channel)
                        + ": " + e.what();
    CEDAR_THROW(cedar::dev::sensors::visual::GrabberGrabException,msg)
  }
}

#endif // CEDAR_USE_YARP
