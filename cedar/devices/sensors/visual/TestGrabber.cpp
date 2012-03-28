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

    File:        TestGrabber.cpp

    Maintainer:  Georg Hartinger
    Email:       georg.hartinger@ini.rub.de
    Date:        2011 08 01

    Description: Implementation of a Grabber for testing the Grabber interface

    Credits:

======================================================================================================================*/

// CEDAR INCLUDES
#include "cedar/devices/sensors/visual/TestGrabber.h"

// SYSTEM INCLUDES

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------
// Constructor for a single-channel grabber
cedar::dev::sensors::visual::TestGrabber::TestGrabber(std::string configFileName, std::string channelName)
:
cedar::dev::sensors::visual::GrabberInterface(configFileName)
{
  std::cout<<"[TestGrabber::TestGrabber] Create a single channel grabber\n";

  //read initialization values from configuration file
  readInit(1,"TestGrabber");

  //overwrite parameters from configfiles with values from constuctor
  getChannel(0)->sourceFileName = channelName;

  //now apply the whole configuration
  applyInit();
}


//----------------------------------------------------------------------------------------------------
// Constructor for a stereo grabber
cedar::dev::sensors::visual::TestGrabber::TestGrabber(
                          std::string configFileName,
                          std::string channelName0,
                          std::string channelName1
                        )
:
cedar::dev::sensors::visual::GrabberInterface(configFileName)
{
  std::cout<<"[TestGrabber::TestGrabber] Create a stereo channel grabber\n";

  //read initialization values from configuration file
  readInit(2,"StereoTestGrabber");

  //overwrite parameters from configfiles with values from constuctor
  getChannel(0)->sourceFileName = channelName0;
  getChannel(1)->sourceFileName = channelName1;

  //now apply the whole configuration
  applyInit();
}

//----------------------------------------------------------------------------------------------------
// Destructor
cedar::dev::sensors::visual::TestGrabber::~TestGrabber()
{
  //call of doCleanup, to do the necessarily cleanup in GrabberInterface
  doCleanUp();

  //do memory de-allocation in the destructor
  mChannelVector.clear();

  //some debug-output
  std::cout<<"[TestGrabber::~TestGrabber] GrabberName: " << getName() << std::endl;
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------
bool cedar::dev::sensors::visual::TestGrabber::onInit()
{

  //do the initialization of your Grabber in this method,
  //grab the first pictures and initialize the mImageMatVector with
  //these pictures

  //-------------------------------------------------
  std::cout << "[TestGrabber::onInit] Initialize Grabber with " << mNumCams << " channels ...\n";

  for (unsigned int i = 0; i < mNumCams; ++i)
  {
    std::cout << "Channel " << i << ": " << mChannelVector.at(i) << "\n";
  }
  std::cout << std::flush;


  //-------------------------------------------------
  //load pictures one by one
  for(unsigned int channel=0; channel<mNumCams;++channel)
  {
    //there is no need to create new matrices, empty ones are
    //already initialized within the channel structure
    cv::Mat frame=cv::Mat();

    //apply the new content to the channel image
    getChannel(channel)->imageMat = frame;
  }


  // all grabbers successfully initialized
  std::cout << "[TestGrabber::onInit] Initialize... finished" << std::endl;

  return true;

  //test the initialize exception
  //return false;
}

//----------------------------------------------------------------------------------------------------
void cedar::dev::sensors::visual::TestGrabber::onCleanUp()
{
  //do the cleanup of used hardware in this method
  //on an exception or a CTRL-C only onCleanUp will be invoked (no destructor)

  std::cout << "[TestGrabber::onCleanUp] GrabberName: " << getName() << std::endl;
}

//----------------------------------------------------------------------------------------------------
void cedar::dev::sensors::visual::TestGrabber::onAddChannel()
{
  //create the channel structure for one channel
  TestChannelPtr channel(new TestChannel);
  mChannels.push_back(channel);
}

//----------------------------------------------------------------------------------------------------
bool cedar::dev::sensors::visual::TestGrabber::onDeclareParameters()
{
  //declare and initialize parameters and members of your derived class here
  mCounter = 0 ;

  //if your parameters should be stored in the configfile,
  //the default-values will be set on new grabbers
  return addParameter(&_mTest, "testparam", 123) == CONFIG_SUCCESS;
}

//----------------------------------------------------------------------------------------------------
const std::string& cedar::dev::sensors::visual::TestGrabber::onGetSourceInfo(unsigned int channel) const
{
  //this is the only pure virtual method of the GrabberInterface class

  //no range-check is needed, because this is done in the GrabberInterface::getSourceInfo method

  //give some information about the used source like channelname, filename, devicename
  //or something like that
  return mChannelVector.at(channel);
}

//----------------------------------------------------------------------------------------------------
bool cedar::dev::sensors::visual::TestGrabber::onGrab()
{
  //this is the main grabbing method.
  //read a new picture from the source and set the picture in the mImageMatVector.at()

  for(unsigned int channel=0; channel<mNumCams;++channel)
   {
     //apply the new content to the channel image
     //getChannel(channel)->imageMat = <grab_new_content>;
   }

  //here we just want to count how often onGrab is invoked, due to a fps-check
  mCounter ++;
  return true;
}

//----------------------------------------------------------------------------------------------------
unsigned int cedar::dev::sensors::visual::TestGrabber::getCounter()
{
  //a simple get-method
  unsigned int ct = mCounter;
  mCounter = 0;
  return ct;
}

//----------------------------------------------------------------------------------------------------
int cedar::dev::sensors::visual::TestGrabber::getTestParam()
{
  //a simple get-method
  return _mTest;
}

//----------------------------------------------------------------------------------------------------
void cedar::dev::sensors::visual::TestGrabber::setTestParam(int mTest)
{
  //a simple set-method
  _mTest=mTest;
}

