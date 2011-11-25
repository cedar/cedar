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

    File:        CameraGrabber.cpp

    Maintainer:  Georg.Hartinger
    Email:       georg.hartinger@ini.rub.de
    Date:        2011 08 01

    Description: Implementation of the @em cedar::devices::visual::CameraGrabber class.

    Credits:

======================================================================================================================*/

// LOCAL INCLUDES
#include "CameraGrabber.h"
#include "../../../auxiliaries/exceptions/IndexOutOfRangeException.h"

// PROJECT INCLUDES

// SYSTEM INCLUDES
#include <dc1394/dc1394.h> //for camera-settings


using namespace cv;
using namespace cedar::dev::sensors::visual;

//----------------------------------------------------------------------------------------------------------------------
//constructors and destructor
//----------------------------------------------------------------------------------------------------------------------


//----------------------------------------------------------------------------------------------------
//Constructor for single-file grabber
CameraGrabber::CameraGrabber(
                              const std::string& configFileName,
                              unsigned int camera,
                              bool isGuid,
                              bool finishInitialization
                            )
:
GrabberInterface(configFileName),
mFinishInitialization(finishInitialization),
mCreateGrabberByGuid(isGuid)
{
  #ifdef DEBUG_CAMERAGRABBER
    std::cout << "[CameraGrabber::CameraGrabber] Create single channel instance" << std::endl;
  #endif

  mCameraIds.push_back(camera);
  doInit(mCameraIds.size(),"CameraGrabber");
}


//----------------------------------------------------------------------------------------------------
//Constructor for stereo-file grabber
CameraGrabber::CameraGrabber(
                              const std::string& configFileName,
                              unsigned int camera0,
                              unsigned int camera1,
                              bool isGuid,
                              bool finishInitialization
                            )
:
GrabberInterface(configFileName),
mFinishInitialization(finishInitialization),
mCreateGrabberByGuid(isGuid)
{
  #ifdef DEBUG_CAMERAGRABBER
    std::cout << "[CameraGrabber::CameraGrabber] Create stereo channel instance" << std::endl;
  #endif

  mCameraIds.push_back(camera0);
  mCameraIds.push_back(camera1);

  doInit(mCameraIds.size(),"CameraGrabber");
}

//----------------------------------------------------------------------------------------------------
//constructor that reads configuration only from configuration file
/*CameraGrabber::CameraGrabber(
               const std::string& configFileName,
               unsigned int numCameras,
               bool finishInitialization
             )
:
GrabberInterface(configFileName),
mFinishInitialization(finishInitialization)
{
  doInit(numCameras,"CameraGrabber");
}
*/

//----------------------------------------------------------------------------------------------------
CameraGrabber::~CameraGrabber()
{
  doCleanUp();

  #ifdef DEBUG_CAMERAGRABBER
    std::cout<<"[CameraGrabber::Destructor]"<< std::endl;
  #endif
  //VideoCaptures are released automatically within the Vector mVideoCaptures
}


//----------------------------------------------------------------------------------------------------------------------
//methods
//----------------------------------------------------------------------------------------------------------------------


//----------------------------------------------------------------------------------------------------
bool CameraGrabber::onDeclareParameters()
{

  /*
  //Params from old grabber
  Name= "ieee camera settings";
  Channel=     0;
  UseCameraChannel= true;
  Brightness=   128;
  AutoExposure=   128;
  Sharpness=   128;
  WhiteBalanceBlue=   135;
  WhiteBalanceRed=   209;
  Hue=   128;
  Saturation=   128;
  Gamma=   128;
  Shutter=  2048;
  Gain=     0;
  Iris=  2816;
  Focus=   256;
  Zoom=    64;
  Filter=     0;
  */
  return true;
}

//----------------------------------------------------------------------------------------------------
bool CameraGrabber::onInit()
{
  //local and/or stored Parameters are already initialized

  #ifdef DEBUG_CAMERAGRABBER
    std::cout<<"[CameraGrabber::onInit]"<< std::endl;
  #endif

  #ifdef SHOW_INIT_INFORMATION_CAMERAGRABBER
    std::cout << "CameraGrabber: Initialize Grabber with " << mNumCams << " cameras ..." << std::endl;
    for (unsigned int i = 0; i < mNumCams; ++i)
    {
      std::cout << "Channel " << i << ": capture from: camera " << mCameraIds.at(i) << "\n";
    }
    std::cout << std::flush;
  #endif

  //----------------------------------------
  //open capture one by one, and create storage (cv::Mat) for it
  mImageMatVector.clear();
  mVideoCaptures.clear();

  //todo: check construction is by guid or by cameraId


  if (mCreateGrabberByGuid)
  {
    // search for the right guid, so open it one by one

    for (unsigned int i = 0; i < mNumCams; i++)
    {

    }

  }
  else  //create by ongoing number
  {
    for (unsigned int i = 0; i < mNumCams; ++i)
    {
      //VideoCapture capture(mCameraIds.at(i));
      cv::VideoCapture capture(mCameraIds.at(i));
      if (capture.isOpened())
      {
        mVideoCaptures.push_back(capture);

        //here is the only chance to set camera properties
        //capture.set(CV_CAP_PROP_FRAME_WIDTH, 320);
        //capture.set(CV_CAP_PROP_MODE,1);
        //capture.set(CV_CAP_PROP_FPS,7.5);

        //grab first frame to initialize the camera-memory
        cv::Mat frame = cv::Mat();
        //this starts the grabbing-mode inside the OpenCV-CvCaptureCAM_DC1394_v2
        //and it isn't possible to change the mode or the framerate.
        //so capture the first frame in your main-programm
        //mVideoCaptures.at(i) >> frame;
        mImageMatVector.push_back(frame);
      }
      else
      {
        std::cout << "ERROR: Grabbing failed (Channel " << i << ")." << std::endl;
        return false; //throws an initialization-exception
      }
    }

  }

  //Check if Initialization should be finished.
  if (mFinishInitialization)
  {
    for (unsigned int i = 0; i < mNumCams; ++i)
    {
      mVideoCaptures.at(i) >> mImageMatVector.at(i);
    }
  }

  //all grabbers successfully initialized

  //default initialization
  //----------------------------------------
  //check for smallest FPS
  double fps = mVideoCaptures.at(0).get(CV_CAP_PROP_FPS);
  for (unsigned int i = 1; i < mNumCams; ++i)
  {
    double fps_test = mVideoCaptures.at(i).get(CV_CAP_PROP_FPS);
    if (fps_test < fps)
    {
      fps = fps_test;
    }
  }

  //----------------------------------------
  //set stepsize for LoopedThread
  setFps(fps);

  #ifdef DEBUG_CAMERAGRABBER
    std::cout << "[CameraGrabber::onInit] Initialize... finished" << std::endl;
  # endif

  return true;
} //onInit()

//----------------------------------------------------------------------------------------------------
bool CameraGrabber::onGrab()
{
  int result = true;

  //grab and retrieve
  /*
   * for(unsigned int i = 0; i < mNumCams; ++i)
   * {
   * (mVideoCaptures.at(i))>> mImageMatVector.at(i);
   * }
   * if (mVideoCaptures.at(i).empty)
   * {
   * return false
   * }
   */

  //for better synchronizing between the cameras,
  //first grab internally in camera (OpenCV)
  for (unsigned int i = 0; i < mNumCams; ++i)
  {
    result = result && mVideoCaptures.at(i).grab();
  }

  //and then retrieve the frames
  if (result)
  {
    for (unsigned int i = 0; i < mNumCams; ++i)
    {
      result = result && mVideoCaptures.at(i).retrieve(mImageMatVector.at(i));
    }
  }

  return result;
}


//----------------------------------------------------------------------------------------------------
std::string CameraGrabber::onGetSourceInfo(unsigned int channel) const
{
  //value of channel is already checked by getSourceInfo
  //if (channel >= mNumCams)
  //{
  //  CEDAR_THROW(cedar::aux::exc::IndexOutOfRangeException,"CameraGrabber::onGetSourceInfo");
  //}
  std::stringstream s;
  s << "Camera channel " << channel
    << ": DeviceID: "<< mCameraIds.at(channel);
    //<< " Mode: " << const_cast<double>(mVideoCaptures.at(channel).getCameraProperty(channel,CV_CAP_PROP_MODE));
  return s.str();
}

//----------------------------------------------------------------------------------------------------
void CameraGrabber::onCleanUp()
{
  #ifdef DEBUG_CAMERAGRABBER
    std::cout<<"[CameraGrabber::onCleanUp]"<< std::endl;
  #endif

  //close all captures
  mVideoCaptures.clear();
}

//----------------------------------------------------------------------------------------------------
double CameraGrabber::getCameraProperty(unsigned int channel,CameraProperty::Id propId)
{
  if (channel >= mNumCams)
  {
    CEDAR_THROW(cedar::aux::exc::IndexOutOfRangeException,"CameraGrabber::getCameraParam");
  }
  return mVideoCaptures.at(channel).get(propId);
}

//----------------------------------------------------------------------------------------------------
double CameraGrabber::getCameraProperty(CameraProperty::Id propId)
{
  return mVideoCaptures.at(0).get(propId);
}

//----------------------------------------------------------------------------------------------------
bool CameraGrabber::setCameraProperty(unsigned int channel,CameraProperty::Id propId, double value)
{
  if (channel >= mNumCams)
  {
    CEDAR_THROW(cedar::aux::exc::IndexOutOfRangeException,"CameraGrabber::setCameraParam");
  }

  #ifdef ENABLE_GRABBER_WARNING_OUTPUT

    //check if parameter is changeable
    if (!getImage().empty())
    {
      //there is already a grabbed image in the buffer, so some properties aren't changeable any more

      std::string info = "";
      if (propId == CV_CAP_PROP_FPS)
      {
        info = "FPS";
      }
      else if ((propId == CV_CAP_PROP_FRAME_WIDTH) || (propId == CV_CAP_PROP_FRAME_HEIGHT))
      {
        info = "size";
      }
      else if (propId == CV_CAP_PROP_MODE)
      {
        info = "video mode";
      }

      if (info!="")
      {
        std::cout << "[CameraGrabber::setCameraParam] To set the "<< info <<", you have to reinitialize "
                  << "(i.e. restart) your grabber!!" << std::endl;
      }

    }
  #endif

  return mVideoCaptures.at(channel).set(propId, value);
}

//----------------------------------------------------------------------------------------------------
bool CameraGrabber::setCameraProperty(CameraProperty::Id propId, double value)
{
  return mVideoCaptures.at(0).set(propId, value);
}


//----------------------------------------------------------------------------------------------------
bool CameraGrabber::setCameraSetting( unsigned int channel, CameraSetting::Id setting,double value)
{

}


//----------------------------------------------------------------------------------------------------
double CameraGrabber::getCameraSetting( unsigned int channel, CameraSetting::Id setting)
{

}

//----------------------------------------------------------------------------------------------------
bool CameraGrabber::setCameraMode(
                                       unsigned int channel,
                                       CameraVideoMode::Id mode
                                     )
{
  if (channel >= mNumCams)
  {
    CEDAR_THROW(cedar::aux::exc::IndexOutOfRangeException,"CameraGrabber::setCameraInitMode");
  }

  //check if there is already a grabbed image in the buffer,
  //if so, firewire-grabbing is already initialized and therefore it isn't possible to change this prop.
  if (!getImage(channel).empty())
  {

    #ifdef ENABLE_GRABBER_WARNING_OUTPUT
      std::cout << "[CameraGrabber::setCameraInitMode] To set the Mode, you have to reinitialize "
                << "(i.e. restart) your grabber!!" << std::endl;
    #endif
    return false;
  }
  return mVideoCaptures.at(channel).set(CameraSetting::SETTING_MODE, (int) mode);

}


//----------------------------------------------------------------------------------------------------
CameraVideoMode::Id CameraGrabber::getCameraMode(unsigned int channel)
{

}

//----------------------------------------------------------------------------------------------------
bool CameraGrabber::setCameraFps(unsigned int channel,CameraFrameRate::Id fps)
{
  if (channel >= mNumCams)
  {
    CEDAR_THROW(cedar::aux::exc::IndexOutOfRangeException,"CameraGrabber::setCameraInitFps");
  }

  //check if there is already a grabbed image in the buffer,
  //if so, firewire-grabbing is already initialized and therefore it isn't possible to change this prop.
  if (!getImage(channel).empty())
  {

    #ifdef ENABLE_GRABBER_WARNING_OUTPUT
      std::cout << "[CameraGrabber::setCameraInitFps] To set the FPS, you have to reinitialize "
                << "(i.e. restart) your grabber!!" << std::endl;
    #endif
    return false;
  }
  return mVideoCaptures.at(channel).set(CV_CAP_PROP_FPS, (int) fps);
}


//----------------------------------------------------------------------------------------------------
CameraFrameRate::Id CameraGrabber::getCameraFps(unsigned int channel)
{

}

//----------------------------------------------------------------------------------------------------
bool CameraGrabber::setCameraIsoSpeed( unsigned int channel,CameraIsoSpeed::Id isoSpeed )
{
  if (channel >= mNumCams)
  {
    CEDAR_THROW(cedar::aux::exc::IndexOutOfRangeException,"CameraGrabber::setCameraInitIsoSpeed");
  }

  if (!getImage(channel).empty())
  {

    #ifdef ENABLE_GRABBER_WARNING_OUTPUT
      std::cout << "[CameraGrabber::setCameraInitIsoSpeed] To set the ISO-Speed, you have to save"
                << "the configuration and then reinitialize "
                << "(i.e. restart) your grabber!!" << std::endl;
    #endif
    return false;
  }

  return mVideoCaptures.at(channel).set(/*CV_CAP_PROP_ISO_SPEED*/30, isoSpeed);
}


//----------------------------------------------------------------------------------------------------
CameraIsoSpeed::Id CameraGrabber::getCameraIsoSpeed(unsigned int channel)
{

}

//----------------------------------------------------------------------------------------------------
bool CameraGrabber::setCameraFrameSize(unsigned int channel, cv::Size size)
{
  if (channel >= mNumCams)
  {
    CEDAR_THROW(cedar::aux::exc::IndexOutOfRangeException,"CameraGrabber::setCameraInitFrameSize");
  }

  //check if there is already a grabbed image in the buffer,
  //if so, firewire-grabbing is already initialized and therefore it isn't possible to change this prop.
  if (!getImage(channel).empty())
  {

    #ifdef ENABLE_GRABBER_WARNING_OUTPUT
      std::cout << "[CameraGrabber::setCameraInitFrameSize] To set the size of the frames, you have to reinitialize "
                << "(i.e. restart) your grabber!!" << std::endl;
    #endif
    return false;
  }
  bool result = false;
  result = result && mVideoCaptures.at(channel).set(CameraSetting::SETTING_FRAME_WIDTH, size.width);
  result = result && mVideoCaptures.at(channel).set(CameraSetting::SETTING_FRAME_HEIGHT, size.height);
  return result;
}


//----------------------------------------------------------------------------------------------------
cv::Size CameraGrabber::getCameraFrameSize( unsigned int channel)
{

}
