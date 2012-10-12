/*======================================================================================================================

    Copyright 2011, 2012 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany

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
#include "cedar/auxiliaries/net/Reader.h"
#include "cedar/auxiliaries/net/exceptions.h"
#include "cedar/auxiliaries/exceptions.h"

// SYSTEM INCLUDES


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


//----------------------------------------------------------------------------------------------------
// Constructor for single-channel grabber
cedar::dev::sensors::visual::NetGrabber::NetGrabber
(
  const std::string& yarpChannelName,
  const std::string& grabberName
)
:
cedar::dev::sensors::visual::Grabber
(
  grabberName,
  cedar::dev::sensors::visual::NetChannelPtr
  (
    new cedar::dev::sensors::visual::NetChannel(yarpChannelName)
  )
)
{
  cedar::aux::LogSingleton::getInstance()->allocating(this);
}


//----------------------------------------------------------------------------------------------------
// Constructor for stereo-channel grabber
cedar::dev::sensors::visual::NetGrabber::NetGrabber
(
  const std::string& yarpChannelName0,
  const std::string& yarpChannelName1,
  const std::string& grabberName
)
:
cedar::dev::sensors::visual::Grabber
(
  grabberName,
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

//----------------------------------------------------------------------------------------------------
cedar::dev::sensors::visual::NetGrabber::~NetGrabber()
{
  doCleanUp();
  cedar::aux::LogSingleton::getInstance()->freeing(this);


  // done by smart-pointer in doCleanup
  /*for (unsigned int i = 0; i < mNumCams; ++i)
  {
    delete &mYarpChannels.at(i);
    mYarpChannels.pop_back();
  }
  */
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------


//----------------------------------------------------------------------------------------------------
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

//----------------------------------------------------------------------------------------------------
bool cedar::dev::sensors::visual::NetGrabber::onInit()
{
  cedar::aux::LogSingleton::getInstance()->debugMessage
                                           (
                                             this->getName() + ": onInit()",
                                             "cedar::dev::sensors::visual::NetGrabber::onInit()"
                                           );

  unsigned int num_cams = getNumCams();
  std::stringstream info;
  info << "Initialize NetGrabber with " << num_cams << " channels ..." << std::endl;

  for(unsigned int channel = 0; channel < num_cams; ++channel)
  {
    info << "Channel " << channel << ": capture from YARP-hannel: "
         << getNetChannel(channel)->_mpYarpChannelName->getValue()<< "\n";
  }

  cedar::aux::LogSingleton::getInstance()->systemInfo
                                           (
                                             this->getName() + ": " + info.str(),
                                             "cedar::dev::sensors::visual::NetGrabber::onInit()"
                                           );


  //-------------------------------------------------
  // open capture one by one
  // cedar::aux::net::Reader<cv::Mat> *YarpReader = NULL;
  cedar::dev::sensors::visual::NetChannel::MatNetReaderPtr yarp_reader;

  // loop until connection established or an error occurs
  for (unsigned int channel = 0; channel < num_cams; ++channel)
  {
    const std::string channel_name = getNetChannel(channel)->_mpYarpChannelName->getValue();

    cedar::aux::LogSingleton::getInstance()->systemInfo
                                             (
                                               this->getName() + ": Create channel "
                                                 + boost::lexical_cast<std::string>(channel) + ": "
                                                 + channel_name,
                                               "cedar::dev::sensors::visual::NetGrabber::onInit()"
                                             );

    // try to connect for about 1 to 2 minutes per channel
    int counter_get_writer = 0;

    do
    {
      try
      {

        #ifdef SHOW_INIT_INFORMATION_NETGRABBER
          std::cout << "." << std::flush;
        #endif

        // establish connection
        yarp_reader
         = cedar::dev::sensors::visual::NetChannel::MatNetReaderPtr(new cedar::aux::net::Reader<cv::Mat>(channel_name));

        #ifdef SHOW_INIT_INFORMATION_NETGRABBER
          std::cout << "ok" << std::endl;
        #endif

      }

      // ERROR: No Yarp writer with appropriate channel name
      //@todo: GH commented on merge
      // catch (cedar::aux::NetWaitingForWriterException &E)
      catch (cedar::aux::ExceptionBase &E)
      {
        std::stringstream error_msg;
        error_msg << this->getName() << ": WARNING: waiting for yarp-writer failed" << std::endl
                    << "\t\tChannel " << channel << ": "<< channel_name << std::endl
                    << E.exceptionInfo();
        cedar::aux::LogSingleton::getInstance()->warning
                                                 (
                                                   error_msg.str(),
                                                   "cedar::dev::sensors::visual::NetGrabber::onInit()"
                                                 );
        if (++counter_get_writer > 10)
        {
          error_msg.clear();
          error_msg << this->getName() << ": ERROR: Waiting for yarp-writer failed" << std::endl
                    << "\tChannel " << channel << ": "<< channel_name << std::endl
                    << "\t" << E.exceptionInfo();
          cedar::aux::LogSingleton::getInstance()->error
                                                   (
                                                     error_msg.str(),
                                                     "cedar::dev::sensors::visual::NetGrabber::onInit()"
                                                   );
          return false;  // throws an initialization-exception
        }
        else
        {
          usleep(1000);
        }
      //@todo: GH commented on merge
      //}

      // ERROR: Somehow YARP doesnt work ... :( typically fatal.
      // catch (cedar::aux::ExceptionBase &E)
      //{
        // todo: throw weiter
        // std::stringstream error_msg;
        error_msg.clear();
        error_msg << this->getName() << ": ERROR: Initialization failed" << std::endl
                  << "\tChannel " << channel << ": "<< channel_name << std::endl
                  << "\t" << E.exceptionInfo();
        cedar::aux::LogSingleton::getInstance()->error
                                                 (
                                                   error_msg.str(),
                                                   "cedar::dev::sensors::visual::NetGrabber::onInit()"
                                                 );
        return false;  // throws an initialization-exception
      }

      // ERROR: Default
      catch (...)
      {
        // std::stringstream error_msg;
        std::stringstream error_msg;
        error_msg << this->getName() << ": ERROR: Unknown Error on initialization of yarp-writer"
                  << std::endl
                  << "\tChannel " << channel << ": "<< channel_name << std::endl;
        cedar::aux::LogSingleton::getInstance()->error
                                                 (
                                                   error_msg.str(),
                                                   "cedar::dev::sensors::visual::NetGrabber::onInit()"
                                                 );
        return false;  // throws an initialization-exception
      }

    } while (!yarp_reader.get()); // check the raw pointer

    getNetChannel(channel)->mpMatNetReader = yarp_reader;

    // Channel i initialized, try to receive the first image

    cedar::aux::LogSingleton::getInstance()->debugMessage
                                             (
                                               this->getName() + ": Try to grabb from channel "
                                                 + boost::lexical_cast<std::string>(channel),
                                               "cedar::dev::sensors::visual::NetGrabber::onInit()"
                                             );
    cv::Mat frame;
    bool reading_ok = false;
    int counter_get_image = 0;

    // loop until first image received
    do
    {
      try
      {
        #ifdef SHOW_INIT_INFORMATION_NETGRABBER
          std::cout << "." << std::flush;
        #endif
        frame = getNetChannel(channel)->mpMatNetReader->read();
        #ifdef SHOW_INIT_INFORMATION_NETGRABBER
          std::cout << "ok" << std::endl;
        #endif
        getImageMat(channel) = frame;
        reading_ok = true;
      }
      //@todo: GH commented on merge
      // catch (cedar::aux::NetUnexpectedDataException &E)
      catch (cedar::aux::ExceptionBase &E)
      {
        if (++counter_get_image > 10)
        {
          std::stringstream error_msg;
          error_msg << this->getName() << ": ERROR: Couldn't retrieve an image" << std::endl
                      << "\t\tChannel " << channel << ": "<< channel_name << std::endl
                      << E.exceptionInfo();
          cedar::aux::LogSingleton::getInstance()->error
                                                   (
                                                     error_msg.str(),
                                                     "cedar::dev::sensors::visual::NetGrabber::onInit()"
                                                   );
          return false;  // throws an initialization-exception
        }
        else
        {
          reading_ok = false;
          usleep(5000);
        }
      }
      catch (...)
      {
        std::stringstream error_msg;
        error_msg << this->getName() << ": ERROR: Couldn't retrieve an image" << std::endl
                    << "\t\tChannel " << channel << ": "<< channel_name << std::endl;
        cedar::aux::LogSingleton::getInstance()->error
                                                 (
                                                   error_msg.str(),
                                                   "cedar::dev::sensors::visual::NetGrabber::onInit()"
                                                 );
        return false;  // throws an initialization-exception
      }

    } while (!reading_ok);
  }

  // all grabbers successfully initialized
  cedar::aux::LogSingleton::getInstance()->debugMessage
                                           (
                                             this->getName() + ": Initializtion finished",
                                             "cedar::dev::sensors::visual::NetGrabber::onInit()"
                                           );

  // TODO
  // set fps
  // until now, it is set to default value of loopedThread
  // maybe read fps and check against default value from loopedthread
  // to decide if it was load from config-file

  return true;
} // onInit()

//----------------------------------------------------------------------------------------------------
void cedar::dev::sensors::visual::NetGrabber::onUpdateSourceInfo(unsigned int channel)
{
  // value of channel is already checked by GraberInterface::getSourceInfo()
  // TODO: perhaps it is possible to gather information of used yarp-server too
  std::string name = this->getName();
  std::string channel_name = getNetChannel(channel)->_mpYarpChannelName->getValue();
  setChannelInfoString(channel, name + ": " + channel_name);
}

//----------------------------------------------------------------------------------------------------
bool cedar::dev::sensors::visual::NetGrabber::onGrab()
{
  // unsigned int numCams = getNumCams();
  int result = true;

  unsigned int num_cams = getNumCams();
  for(unsigned int channel = 0; channel < num_cams; ++channel)
  {
    // nonblocking version of netreader

    // on exception leave programm, so we don't catch it here
    // try
    //{
    getImageMat(channel) = getNetChannel(channel)->mpMatNetReader->read();
    /* }
     * catch (cedar::aux::NetUnexpectedDataException &E)
     * {
     *
     * //
     *
     * //@TODO: Timeout for last matrix
     * //    how to detect the end of a stream??
     *
     * // howto detect a false matrix
     * // result = false;
     *
     * }
     */

    if (getImageMat(channel).empty())
    {
      result = false;
    }
  }
  return result;
} // onGrab()

#endif // CEDAR_USE_YARP
