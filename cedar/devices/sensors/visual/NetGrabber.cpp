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

// LOCAL INCLUDES
#include "NetGrabber.h"
#include "GrabberInterface.h"
#include "../../../auxiliaries/net/NetReader.h"
#include "../../../auxiliaries/net/exceptions/NetException.h"
#include "../../../auxiliaries/exceptions/IndexOutOfRangeException.h"

// PROJECT INCLUDES


// SYSTEM INCLUDES


using namespace cedar::dev::sensors::visual;


//----------------------------------------------------------------------------------------------------------------------
//constructors and destructor
//----------------------------------------------------------------------------------------------------------------------


//----------------------------------------------------------------------------------------------------
//Constructor for single-channel grabber
NetGrabber::NetGrabber(
                        const std::string& configFileName,
                        const std::string& YarpChannel
                      )
:
GrabberInterface(configFileName)
{
  mYarpChannels.push_back(YarpChannel);
  doInit(mYarpChannels.size(),"NetGrabber");
}

//----------------------------------------------------------------------------------------------------
//Constructor for stereo-channel grabber
NetGrabber::NetGrabber(
                        const std::string& configFileName,
                        const std::string& YarpChannel0,
                        const std::string& YarpChannel1
                      )
:
GrabberInterface(configFileName)
{
  mYarpChannels.push_back(YarpChannel0);
  mYarpChannels.push_back(YarpChannel1);
  doInit( mYarpChannels.size(),"NetGrabber");
}

//----------------------------------------------------------------------------------------------------
NetGrabber::~NetGrabber()
{
  doCleanUp();
  #ifdef DEBUG_NETGRABBER
    std::cout << "YarpGrabber::Destructor\n";
  #endif

  for (unsigned int i = 0; i < mNumCams; ++i)
  {
    delete &mYarpChannels.at(i);
    mYarpChannels.pop_back();
  }
}

//----------------------------------------------------------------------------------------------------------------------
//methods
//----------------------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------
void NetGrabber::onCleanUp()
{
  #ifdef DEBUG_CAMERAGRABBER
    std::cout<<"[NetGrabber::onCleanUp]"<< std::endl;
  #endif

  //close all captures
  mYarpReaderVector.clear();
}

//----------------------------------------------------------------------------------------------------
bool NetGrabber::onInit()
{
  #ifdef DEBUG_NETGRABBER
    std::cout << "\n\nYarpGrabber.onInit()\n";
  #endif

  #ifdef SHOW_INIT_INFORMATION_NETGRABBER
    std::cout << "YarpGrabber: Initialize Grabber with " << mNumCams << " channels ..." << std::endl;
    for (unsigned int i = 0; i < mNumCams; ++i)
    {
      std::cout << "Channel " << i << ": capture from yarpchannel: " << mYarpChannels.at(i) << "\n";
    }
    std::cout << std::flush;
  #endif

  //clear old stuff (if something is there)
  mImageMatVector.clear();
  mYarpReaderVector.clear();

  //cedar::dev::sensors::visual::TestGrabberPtr
  //      grabber_4(new cedar::dev::sensors::visual::TestGrabber(CONFIG_FILE_NAME_2,CHANNEL_1_NAME ))

  //-------------------------------------------------
  //open capture one by one
  cedar::aux::net::NetReader<cv::Mat> *YarpReader = NULL;

  //loop until connection established or an error occurs
  for (unsigned int i = 0; i < mNumCams; ++i)
  {
    #ifdef SHOW_INIT_INFORMATION_NETGRABBER
      std::cout << "NetGrabber: Create channel " << i << ": " << mYarpChannels.at(i) << " " << std::flush;
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
        YarpReader = new cedar::aux::net::NetReader<cv::Mat>(mYarpChannels.at(i));

        #ifdef SHOW_INIT_INFORMATION_NETGRABBER
          std::cout << "ok" << std::endl;
        #endif

      }

      //ERROR: No Yarp writer with appropriate channel name
      catch (cedar::aux::exc::NetWaitingForWriterException &E)
      {
        #ifndef SHOW_INIT_INFORMATION_NETGRABBER
          std::cout << "[NetGrabber::onInit] WARNING: waiting for yarp-writer failed\n"
                    << "\t\tChannel " << i << ": "<< mYarpChannels.at(i) << "\n"
                    << E.exceptionInfo()
                    << std::endl;
        #endif
        if (++counter_get_writer > 10)
        {
          std::cout << "[NetGrabber::onInit] ERROR: Waiting for yarp-writer failed\n"
                    << "\tChannel " << i << ": "<< mYarpChannels.at(i) << "\n"
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
      catch (cedar::aux::exc::ExceptionBase &E)
      {
        std::cout << "[NetGrabber::onInit] ERROR: Initialization failed\n"
                  << "\tChannel " << i << ": "<< mYarpChannels.at(i) << "\n"
                  << "\t" << E.exceptionInfo()
                  << std::endl;
        return false;  //throws an initialization-exception
      }

      //ERROR: Default
      catch (...)
      {
        std::cout << "[NetGrabber::onInit] ERROR: Unknown Error on initialization of yarp-writer\n"
                  << "\tChannel " << i << ": "<< mYarpChannels.at(i) << "\n"
                  << std::endl;
                  return false;  //throws an initialization-exception
      }

    } while (!YarpReader);

    mYarpReaderVector.push_back(YarpReader);

    //Channel i initialized, try to receive the first image
    #ifdef SHOW_INIT_INFORMATION_NETGRABBER
      std::cout << "Yarp-Grabber: Try to read from channel " << i << " ";
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
        frame = mYarpReaderVector.at(i)->read();
        #ifdef SHOW_INIT_INFORMATION_NETGRABBER
          std::cout << "ok" << std::endl;
        #endif
        mImageMatVector.push_back(frame);
        reading_ok = true;
      }
      catch (cedar::aux::exc::NetUnexpectedDataException &E)
      {
        if (++counter_get_image > 10)
        {
          std::cout << "[NetGrabber::onInit] ERROR: Couldn't retrieve an image\n"
                    << "\tChannel " << i << ": "<< mYarpChannels.at(i) << "\n"
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
                  << "\tChannel " << i << ": "<< mYarpChannels.at(i) << "\n"
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
bool NetGrabber::onDeclareParameters()
{
  return true;
}

//----------------------------------------------------------------------------------------------------
std::string NetGrabber::onGetSourceInfo(unsigned int channel) const
{
  //TODO: gather information of used yarp-server too
  if (channel >= mNumCams)
  {
    CEDAR_THROW(cedar::aux::exc::IndexOutOfRangeException,"NetGrabber::onGetSourceInfo");
  }
  return mYarpChannels.at(channel);
}

//----------------------------------------------------------------------------------------------------
bool NetGrabber::onGrab()
{
  //unsigned int numCams = getNumCams();
  int result = true;

  for (unsigned int i = 0; i < mNumCams; ++i)
  {
    //nonblocking version of netreader

    //on exception leave programm, so we don't catch it here
    //try
    //{
    mImageMatVector.at(i) = mYarpReaderVector.at(i)->read();
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

    if (mImageMatVector.at(i).empty())
    {
      result = false;
    }
  }
  return result;
} //onGrab()

