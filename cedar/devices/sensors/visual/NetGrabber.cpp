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

    File:        NetGrabber.cpp

    Maintainer:  Georg.Hartinger
    Email:       georg.hartinger@rub.de
    Date:        2011 08 01

    Description: Implementation of the @em cedar::dev::sensors::visual::NetGrabber class.

    Credits:

======================================================================================================================*/

// CEDAR INCLUDES
#include "cedar/devices/sensors/visual/NetGrabber.h"
#include "cedar/auxiliaries/net/NetReader.h"
#include "cedar/auxiliaries/net/exceptions/NetException.h"
#include "cedar/auxiliaries/exceptions/IndexOutOfRangeException.h"

// SYSTEM INCLUDES

//----------------------------------------------------------------------------------------------------------------------
//constructors and destructor
//----------------------------------------------------------------------------------------------------------------------


//----------------------------------------------------------------------------------------------------
//Constructor for single-channel grabber
cedar::dev::sensors::visual::NetGrabber::NetGrabber
(
  const std::string& configFileName,
  const std::string& yarpChannelName
)
:
cedar::dev::sensors::visual::GrabberInterface(configFileName)
{
  readInit(1,"NetGrabber");
  getChannel(0)->mChannelName = yarpChannelName;
  applyInit();
}

//----------------------------------------------------------------------------------------------------
//Constructor for stereo-channel grabber
cedar::dev::sensors::visual::NetGrabber::NetGrabber
(
  const std::string& configFileName,
  const std::string& yarpChannelName0,
  const std::string& yarpChannelName1
)
:
cedar::dev::sensors::visual::GrabberInterface(configFileName)
{
  readInit(2,"NetGrabber");

  //overwrite with settings from constructor
  getChannel(0)->mChannelName = yarpChannelName0;
  getChannel(1)->mChannelName = yarpChannelName1;
  applyInit();
}

//----------------------------------------------------------------------------------------------------
cedar::dev::sensors::visual::NetGrabber::~NetGrabber()
{
  doCleanUp();
  #ifdef DEBUG_NETGRABBER
    std::cout << "YarpGrabber::Destructor\n";
  #endif

  //done by smart-pointer in doCleanup
  /*for (unsigned int i = 0; i < mNumCams; ++i)
  {
    delete &mYarpChannels.at(i);
    mYarpChannels.pop_back();
  }
  */
}

//----------------------------------------------------------------------------------------------------------------------
//methods
//----------------------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------
void cedar::dev::sensors::visual::NetGrabber::onAddChannel()
{
  //create the channel structure for one channel
  NetChannelPtr channel(new NetChannel);
  mChannels.push_back(channel);
}

//----------------------------------------------------------------------------------------------------
void cedar::dev::sensors::visual::NetGrabber::onCleanUp()
{
  #ifdef DEBUG_CAMERAGRABBER
    std::cout<<"[NetGrabber::onCleanUp]"<< std::endl;
  #endif

  //close all captures
  //mNetReaders.clear(); done in GrabberInterface
}

//----------------------------------------------------------------------------------------------------
bool cedar::dev::sensors::visual::NetGrabber::onInit()
{
  #ifdef DEBUG_NETGRABBER
    std::cout << "\n\nYarpGrabber.onInit()\n";
  #endif

  #ifdef SHOW_INIT_INFORMATION_NETGRABBER
    std::cout << "YarpGrabber: Initialize Grabber with " << mNumCams << " channels ..." << std::endl;
    for (unsigned int i = 0; i < mNumCams; ++i)
    {
      std::cout << "Channel " << i << ": capture from YARP-hannel: " << getChannel(i)->mChannelName << "\n";
    }
    std::cout << std::flush;
  #endif

  //-------------------------------------------------
  //open capture one by one
  //cedar::aux::net::NetReader<cv::Mat> *YarpReader = NULL;
  MatNetReaderPtr yarp_reader;

  //loop until connection established or an error occurs
  for (unsigned int channel = 0; channel < mNumCams; ++channel)
  {
    #ifdef SHOW_INIT_INFORMATION_NETGRABBER
      std::cout << "NetGrabber: Create channel " << channel << ": " << getChannel(channel)->mChannelName
                << " " << std::flush;
    #endif

    //try to connect for about 1 to 2 minutes per channel
    int counter_get_writer = 0;

    do
    {
      try
      {
        #ifdef SHOW_INIT_INFORMATION_NETGRABBER
          std::cout << "." << std::flush;
        #endif

        //establish connection
          yarp_reader = MatNetReaderPtr(new cedar::aux::net::NetReader<cv::Mat>(getChannel(channel)->mChannelName));

        #ifdef SHOW_INIT_INFORMATION_NETGRABBER
          std::cout << "ok" << std::endl;
        #endif

      }

      //ERROR: No Yarp writer with appropriate channel name
      catch (cedar::aux::exc::NetWaitingForWriterException &E)
      {
        #ifndef SHOW_INIT_INFORMATION_NETGRABBER
          std::cout << "[NetGrabber::onInit] WARNING: waiting for yarp-writer failed\n"
                    << "\t\tChannel " << channel << ": "<< getChannel(channel)->mChannelName << "\n"
                    << E.exceptionInfo()
                    << std::endl;
        #endif
        if (++counter_get_writer > 10)
        {
          std::cout << "[NetGrabber::onInit] ERROR: Waiting for yarp-writer failed\n"
                    << "\tChannel " << channel << ": "<< getChannel(channel)->mChannelName << "\n"
                    << "\t" << E.exceptionInfo()
                    << std::endl;
          return false;  //throws an initialization-exception
        }
        else
        {
          usleep(1000);
        }
      }

      //ERROR: Somehow YARP doesnt work ... :( typically fatal.
      catch (cedar::aux::exc::ExceptionBase &e)
      {
        //todo: throw weiter
        std::cout << "[NetGrabber::onInit] ERROR: Initialization failed\n"
                  << "\tChannel " << channel << ": "<< getChannel(channel)->mChannelName << "\n"
                  << "\t" << e.exceptionInfo()
                  << std::endl;
        return false;  //throws an initialization-exception
      }

      //ERROR: Default
      catch (...)
      {
        std::cout << "[NetGrabber::onInit] ERROR: Unknown Error on initialization of yarp-writer\n"
                  << "\tChannel " << channel << ": "<< getChannel(channel)->mChannelName << "\n"
                  << std::endl;
                  return false;  //throws an initialization-exception
      }

    } while (!yarp_reader.get()); //check the raw pointer

    getChannel(channel)->mMatNetReader = yarp_reader;

    //Channel i initialized, try to receive the first image
    #ifdef SHOW_INIT_INFORMATION_NETGRABBER
      std::cout << "Yarp-Grabber: Try to read from channel " << channel << " ";
    #endif

    cv::Mat frame;
    bool reading_ok = false;
    int counter_get_image = 0;

    //loop until first image received
    do
    {
      try
      {
        #ifdef SHOW_INIT_INFORMATION_NETGRABBER
          std::cout << "." << std::flush;
        #endif
        frame = getChannel(channel)->mMatNetReader->read();
        #ifdef SHOW_INIT_INFORMATION_NETGRABBER
          std::cout << "ok" << std::endl;
        #endif
        getChannel(channel)->mImageMat = frame;
        reading_ok = true;
      }
      catch (cedar::aux::exc::NetUnexpectedDataException &E)
      {
        if (++counter_get_image > 10)
        {
          std::cout << "[NetGrabber::onInit] ERROR: Couldn't retrieve an image\n"
                    << "\tChannel " << channel << ": "<< getChannel(channel)->mChannelName << "\n"
                    << "\t" << E.exceptionInfo()
                    << std::endl;
          return false;  //throws an initialization-exception
        }
        else
        {
          reading_ok = false;
          usleep(5000);
        }
      }
      catch (...)
      {
        std::cout << "[NetGrabber::onInit] ERROR: Couldn't retrieve an image\n"
                  << "\tChannel " << channel << ": "<< getChannel(channel)->mChannelName << "\n"
                  << std::endl;
        return false;  //throws an initialization-exception
      }

    } while (!reading_ok);
  }

  //all grabbers successfully initialized
  #ifdef DEBUG_NETGRABBER
    std::cout << "YarpGrabber: Initialize... finished" << std::endl;
  #endif

  //TODO
  //set fps
  //until now, it is set to default value of loopedThread
  //maybe read fps and check against default value from loopedthread
  //to decide if it was load from config-file

  return true;
} //onInit()


//----------------------------------------------------------------------------------------------------
bool cedar::dev::sensors::visual::NetGrabber::onDeclareParameters()
{
  return true;
}

//----------------------------------------------------------------------------------------------------
const std::string& cedar::dev::sensors::visual::NetGrabber::onGetSourceInfo(unsigned int channel) const
{
  if (channel >= mNumCams)
  {
    CEDAR_THROW(cedar::aux::exc::IndexOutOfRangeException,"NetGrabber::onGetSourceInfo");
  }
  //TODO: perhaps it is possible to gather information of used yarp-server too
  return getChannel(channel)->mChannelName;
}

//----------------------------------------------------------------------------------------------------
bool cedar::dev::sensors::visual::NetGrabber::onGrab()
{
  //unsigned int numCams = getNumCams();
  int result = true;

  for (unsigned int channel = 0; channel < mNumCams; ++channel)
  {
    //nonblocking version of netreader

    //on exception leave programm, so we don't catch it here
    //try
    //{
    getChannel(channel)->mImageMat = getChannel(channel)->mMatNetReader->read();
    /* }
     * catch (cedar::aux::exc::NetUnexpectedDataException &E)
     * {
     *
     * //
     *
     * //@TODO: Timeout for last matrix
     * //    how to detect the end of a stream??
     *
     * //howto detect a false matrix
     * //result = false;
     *
     * }
     */

    if (getChannel(channel)->mImageMat.empty())
    {
      result = false;
    }
  }
  return result;
} //onGrab()

