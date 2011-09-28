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
    Email:       georg.hartinger@rub.de
    Date:        2011 08 01

    Description: Header for the @em cedar::devices::visual::CameraGrabber class.

    Credits:

======================================================================================================================*/

// LOCAL INCLUDES
#include "CameraGrabber.h"
#include "../../../auxiliaries/exceptions/IndexOutOfRangeException.h"

// PROJECT INCLUDES

// SYSTEM INCLUDES


using namespace cv;
using namespace cedar::dev::sensors::visual;

//----------------------------------------------------------------------------------------------------------------------
//constructors and destructor
//----------------------------------------------------------------------------------------------------------------------


//----------------------------------------------------------------------------------------------------
//Constructor for single-file grabber
CameraGrabber::CameraGrabber(
                              const std::string& configFileName,
                              unsigned int Camera0
                            )
  : GrabberInterface(configFileName)
{
  mCameraId.push_back(Camera0);
  doInit(mCameraId.size(),"CameraGrabber");
}


//----------------------------------------------------------------------------------------------------
//Constructor for stereo-file grabber
CameraGrabber::CameraGrabber(
                              const std::string& configFileName,
                              unsigned int Camera0,
                              unsigned int Camera1
                            )
  : GrabberInterface(configFileName)
{
  mCameraId.push_back(Camera0);
  mCameraId.push_back(Camera1);

  doInit(mCameraId.size(),"CameraGrabber");
}


//----------------------------------------------------------------------------------------------------------------------
//methods
//----------------------------------------------------------------------------------------------------------------------


//----------------------------------------------------------------------------------------------------
bool CameraGrabber::onDeclareParameters()
{
  return true;
}

//----------------------------------------------------------------------------------------------------
CameraGrabber::~CameraGrabber()
{
  #ifdef DEBUG_CAMERAGRABBER
    std::cout<<"[CameraGrabber::Destructor]"<< std::endl;
  #endif
  //VideoCaptures are released automatically within the Vector mCaptureVector
}


//----------------------------------------------------------------------------------------------------
bool CameraGrabber::onInit()
{

  //local and/or stored Parameters are already initialized

  #ifdef SHOW_INIT_INFORMATION_CAMERAGRABBER
    std::cout << "CameraGrabber: Initialize Grabber with " << mNumCams << " cameras ..." << std::endl;

    for (unsigned int i = 0; i < mNumCams; ++i)
    {
      std::cout << "Channel " << i << ": capture from: camera " << mCameraId.at(i) << "\n";
    }
    std::cout << std::flush;
  #endif

  mImageMatVector.clear();
  mCaptureVector.clear();

  //----------------------------------------
  //open capture one by one, and create storage (cv::Mat) for it
  for (unsigned int i = 0; i < mNumCams; ++i)
  {
    VideoCapture capture(mCameraId.at(i));

    if (capture.isOpened())
    {
      mCaptureVector.push_back(capture);

      //grab first frame to initialize the camera-memory
      cv::Mat frame;
      mCaptureVector.at(i) >> frame;
      mImageMatVector.push_back(frame);
    }
    else
    {
      std::cout << "ERROR: Grabbing failed (Channel " << i << ")." << std::endl;

      //throws an initialization-exception, so program will terminate
      return false;
    }
  }
  //all grabbers successfully initialized

  //----------------------------------------
  //check for highest FPS
  double fps = mCaptureVector.at(0).get(CV_CAP_PROP_FPS);

  for (unsigned int i = 1; i < mNumCams; ++i)
  {
    double fps_test = mCaptureVector.at(i).get(CV_CAP_PROP_FPS);

    if (fps_test > fps)
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
   * for(unsigned int i = 0; i < mNumCams; ++i)                    //grab on all channels
   * {
   * (mCaptureVector.at(i))>> mImageMatVector.at(i);
   * }
   * if (mCaptureVector.at(i).empty)
   * {
   * return false
   * }
   */

  //for better synchronizing between the cameras,
  //first grab (internally in camera) and then retrieve
  for (unsigned int i = 0; i < mNumCams; ++i)                     //grab on all channels
  {
    result = result && mCaptureVector.at(i).grab();
  }

  if (result)
  {
    for (unsigned int i = 0; i < mNumCams; ++i)                   //retrieve all channels
    {
      result = result && mCaptureVector.at(i).retrieve(mImageMatVector.at(i));
    }
  }

  return result;
}


//----------------------------------------------------------------------------------------------------
std::string CameraGrabber::onGetSourceInfo(unsigned int channel) const
{
  std::stringstream s;
  s << "Camera " << mCameraId.at(channel) << ": No informations available";
  return s.str();
}


//----------------------------------------------------------------------------------------------------
double CameraGrabber::getCameraParam(unsigned int channel,int propId)
{
  if (channel >= mNumCams)
  {
    CEDAR_THROW(cedar::aux::exc::IndexOutOfRangeException,"CameraGrabber::getCameraParam");
  }
  return mCaptureVector.at(channel).get(propId);
}

//----------------------------------------------------------------------------------------------------
/*double CameraGrabber::getCameraParamFps (unsigned int channel)
 * {
 * return getCameraParam(channel,CV_CAP_PROP_FPS);
 * }
 */
//----------------------------------------------------------------------------------------------------
/* double CameraGrabber::getCameraParamFourcc (unsigned int channel)
 * {
 * return getCameraParam(channel,CV_CAP_PROP_FOURCC);
 * }
 */
