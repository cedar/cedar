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

    File:        TestGrabber.h

    Maintainer:  Georg.Hartinger
    Email:       georg.hartinger@ini.rub.de
    Date:        2011 08 01

    Description: Implementation of a Grabber for testing the Grabber interface

    Credits:

======================================================================================================================*/

// LOCAL INCLUDES
#include <devices/sensors/visual.h>

// PROJECT INCLUDES


// SYSTEM INCLUDES
#include <opencv2/opencv.hpp>


namespace cedar
{
  namespace dev
  {
    namespace sensors
      {
      namespace visual
      {
        class TestGrabber;
      }
    }
  }
}


//!@brief A simple Grabber class for testing the Grabber interface
//
class cedar::dev::sensors::visual::TestGrabber : public cedar::dev::sensors::visual::GrabberInterface
{
private:
  std::vector<std::string> mChannelVector;
  unsigned int mCounter;
  int _mTest;

public:

  //----------------------------------------------------------------------------------------------------
  // Constructor for single-channel grabber
  TestGrabber( std::string configFileName,
               std::string ChannelName)
    :   GrabberInterface(configFileName)
  {
    std::cout<<"[TestGrabber::TestGrabber] Create a single channel grabber\n";
    mChannelVector.push_back(ChannelName);
    doInit( mChannelVector.size(),"TestGrabber");
  }



  //----------------------------------------------------------------------------------------------------
  TestGrabber( std::string configFileName,
         std::string ChannelName0,
         std::string ChannelName1)
  :   GrabberInterface(configFileName)
  {
    std::cout<<"[TestGrabber::TestGrabber] Create a dual channel grabber\n";
    mChannelVector.push_back(ChannelName0);
    mChannelVector.push_back(ChannelName1);
    doInit( mChannelVector.size(),"TestGrabber");

  }

  //----------------------------------------------------------------------------------------------------
  ~TestGrabber()
  {
    onDestroy();
    std::cout<<"[TestGrabber::~TestGrabber] GrabberName: " << getName() << std::endl;
  }


  //----------------------------------------------------------------------------------------------------
  bool onInit()
  {

    //-------------------------------------------------
    std::cout << "[TestGrabber::onInit] Initialize Grabber with " << mNumCams << " channels ..." << std::endl;
    std::cout << "\tchannels:\n\t - " << mChannelVector.at(0) << "\n";
    if (mChannelVector.size()>1)
    {
    std::cout << "\t - " << mChannelVector.at(1) << "\n";
    }

    //-------------------------------------------------
    //create empty picture-matrices one by one
    for(unsigned int i=0; i<mNumCams;++i)
    {
    cv::Mat frame=cv::Mat();
    mImageMatVector.push_back(frame);
    }


    // all grabbers successfully initialized
    std::cout << "[TestGrabber::onInit] Initialize... finished" << std::endl;

    return true;
  }

  //----------------------------------------------------------------------------------------------------
  bool onDestroy()
  {
    std::cout << "[TestGrabber::onDestroy] GrabberName: " << getName() << std::endl;
    return true;
  }

  //----------------------------------------------------------------------------------------------------
  bool onDeclareParameters()
  {
    mCounter = 0 ;
    return addParameter(&_mTest, "testparam", 123) == CONFIG_SUCCESS;
  }

  //----------------------------------------------------------------------------------------------------
  std::string onGetSourceInfo(unsigned int channel) const
  {
    //not needed. Vector does range-check
    if (channel > mNumCams-1)
    {
      //TODO EXCEPTION
    }

    //TODO: gather information of used yarp-server too
    return mChannelVector.at(channel);
  }

  //----------------------------------------------------------------------------------------------------
  bool onGrab()
  {
    //just count how often onGrab is invoked
    mCounter ++;
    return true;
  }

  //----------------------------------------------------------------------------------------------------
  unsigned int getCounter()
  {
    unsigned int ct = mCounter;
    mCounter = 0;
    return ct;
  }

  //----------------------------------------------------------------------------------------------------
  int getTestParam()
  {
    return _mTest;
  }

  //----------------------------------------------------------------------------------------------------
  void setTestParam(int mTest)
  {
    _mTest=mTest;
  }


} ;// cedar::dev::sensors::visual::TestGrabber



