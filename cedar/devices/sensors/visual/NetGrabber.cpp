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

    File:        Grabber.h

    Maintainer:  Georg.Hartinger
    Email:       georg.hartinger@rub.de
    Date:        2011 08 01

    Description: Header for the @em cedar::dev::sensors::visual::NetGrabber class.

    Credits:

======================================================================================================================*/

// LOCAL INCLUDES
#include "NetGrabber.h"
#include "GrabberInterface.h"

// PROJECT INCLUDES
#include <auxiliaries/net/NetReader.h>
#include <auxiliaries/net/exceptions/NetException.h>

#include <auxiliaries/exceptions/IndexOutOfRangeException.h>

// SYSTEM INCLUDES


using namespace cedar::dev::sensors::visual;

//----------------------------------------------------------------------------------------------------
// Constructor for single-channel grabber
NetGrabber::NetGrabber(std::string grabberName,
                       std::string configFileName,
                       std::string YarpChannel)
    :   GrabberInterface(configFileName)
{
    mYarpChannels.push_back(YarpChannel);
    doInit(grabberName,mYarpChannels.size());
}



//----------------------------------------------------------------------------------------------------
// Constructor for stereo-channel grabber
NetGrabber::NetGrabber(std::string grabberName,
           std::string configFileName,
           std::string YarpChannel0,
           std::string YarpChannel1)
:   GrabberInterface( configFileName)
{
  mYarpChannels.push_back(YarpChannel0);
  mYarpChannels.push_back(YarpChannel1);
  doInit(grabberName, mYarpChannels.size());

}

//----------------------------------------------------------------------------------------------------
NetGrabber::~NetGrabber()
{
  std::cout<<"YarpGrabber::Destructor\n";
  for(unsigned int i=0; i<mNumCams;++i)
  {
    delete &mYarpChannels.at(i);
    mYarpChannels.pop_back();
  }
}

//----------------------------------------------------------------------------------------------------
bool NetGrabber::onInit()
{
#if defined SHOW_INIT_INFORMATION_NETGRABBER
  //-------------------------------------------------
  std::cout << "\n\nYarpGrabber.onInit()\n";
  std::cout << "YarpGrabber: Initialize Grabber with " << mNumCams << " channels ..." << std::endl;

 for(unsigned int i=0; i<mNumCams;++i)
  {
    std::cout << "Channel "<< i <<": capture from yarpchannel: " << mYarpChannels.at(i) << "\n";
  }
  std::cout << std::flush;
#endif

  mImageMatVector.clear();
  mYarpReaderVector.clear();

  //-------------------------------------------------
  //open capture one by one
  cedar::aux::net::NetReader<cv::Mat> *YarpReader=NULL;

  for(unsigned int i=0; i<mNumCams;++i)
  {

    //loop until connection established
    std::cout << "NetGrabber: Create channel "<< i << ": "<<mYarpChannels.at(i)<<" "<<std::flush;
    do
    {
      try
      {
        std::cout << "."<<std::flush;
        YarpReader = new cedar::aux::net::NetReader<cv::Mat>(mYarpChannels.at(i));
        std::cout << "ok"<< std::endl;

        mYarpReaderVector.push_back(YarpReader);
        std::cout << "push_back ok"<< std::endl;

        //grab first frame on initializtion

      }
      catch (cedar::aux::exc::NetWaitingForWriterException &E)

      {
        // somehow YARP doesnt work ... :( typically fatal.
        std::cout << "ERROR [NetWaitingForWriterException]: Initialization failed (Channel "<< i << ")." << std::endl;
         //TODO: undo the already initialized grabbers ???
         return false;
      }
      catch(...)
      {
        std::cout << "ERROR [undefined]: Initialization failed (Channel "<< i << ")." << std::endl;
      }
    } while(!YarpReader);


    //loop until first image received
    std::cout << "Yarp-Grabber: Try to read from channel "<<i<<" ";

    cv::Mat frame=cv::Mat();
    bool reading_ok=true;

    do
    {
      try
      {
        std::cout << "."<<std::flush;
        frame = mYarpReaderVector.at(i)->read();
        std::cout << "ok"<<std::endl;
        mImageMatVector.push_back(frame);
        reading_ok=true;
      }
      catch (cedar::aux::exc::NetUnexpectedDataException &E)
      //catch(...)
      {
        // somehow YARP doesnt work ... :( typically fatal.
        reading_ok = false;
        usleep(5000);
        //std::cout << "ERROR: Initialization failed (Channel "<< i << ")." << std::endl;
         //TODO: undo the already initialized grabbers ???
         //return false;
      }
    } while(!reading_ok);

    std::cout << std::endl;
  }


  // all grabbers successfully initialized
#if defined DEBUG_NETGRABBER
    std::cout << "YarpGrabber: Initialize... finished" << std::endl;
# endif

    //TODO
    //set fps 
    //until now, it is set to default value of loopedThread
    //maybe read fps and check against default value from loopedthread
    //to decide if it was load from config-file 

  return true;
} // onInit()


//----------------------------------------------------------------------------------------------------
bool NetGrabber::onDeclareParameters()
{
	return true;
}

//----------------------------------------------------------------------------------------------------
std::string NetGrabber::getPhysicalSourceInformation(unsigned int channel) const
{
    if (channel >= mNumCams)
    {
      CEDAR_THROW(cedar::aux::exc::IndexOutOfRangeException,"NetGrabber::getPhysicalSourceInformation");
    }	
    
	//TODO: gather information of used yarp-server too
	return mYarpChannels.at(channel);
}

//----------------------------------------------------------------------------------------------------
bool NetGrabber::onGrab()
{
    //unsigned int numCams = getNumCams();
    int result = true;

    for(unsigned int i = 0; i < mNumCams; ++i)
    {
        //nonblocking version of netreader

       // on exception leave programm, so we don't catch it here
       // try
       // {
          mImageMatVector.at(i) = mYarpReaderVector.at(i)->read() ;
       /* }
        catch (cedar::aux::exc::NetUnexpectedDataException &E)
        {

          //

          //@TODO: Timeout for last matrix
          //    how to detect the end of a stream??

          //howto detect a false matrix
          //result = false;

        }
        */
        
        if(mImageMatVector.at(i).empty())
        {
          result = false;
        }
     }
     return result;
} // onGrab()

