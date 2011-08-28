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

    File:        VideoGrabber.cpp

    Maintainer:  Georg.Hartinger
    Email:       georg.hartinger@rub.de
    Date:        2011 08 01

    Description: Header for the @em cedar::Grabber class.

    Credits:

======================================================================================================================*/

// LOCAL INCLUDES
#include "VideoGrabber.h"

// PROJECT INCLUDES

// SYSTEM INCLUDES
#include <auxiliaries/exceptions/IndexOutOfRangeException.h>

using namespace cv;
using namespace cedar::dev::sensors::visual;


//----------------------------------------------------------------------------------------------------
// Constructor for single-file grabber
VideoGrabber::VideoGrabber(std::string grabberName,
                       std::string configFileName,
                       std::string aviFileName)
    :   GrabberInterface(configFileName)
{
    mSourceFileName.push_back(aviFileName);

    doInit(grabberName, mSourceFileName.size());
}


//----------------------------------------------------------------------------------------------------
// Constructor for stereo-file grabber
VideoGrabber::VideoGrabber(std::string grabberName,
           std::string configFileName,
           std::string aviFileName0,
           std::string aviFileName1)
:   GrabberInterface(configFileName)
{
  mSourceFileName.push_back(aviFileName0);
  mSourceFileName.push_back(aviFileName1);

  doInit(grabberName,mSourceFileName.size());
}

//----------------------------------------------------------------------------------------------------
bool VideoGrabber::onDeclareParameters()
{
  bool result1 = addParameter(&_mLoop, "Loop", true) == CONFIG_SUCCESS;
  bool result2 = addParameter(&_mSpeedFactor, "SpeedFactor", 1) == CONFIG_SUCCESS;
  return result1 && result2;
}

//----------------------------------------------------------------------------------------------------
VideoGrabber::~VideoGrabber()
{
  //std::cout<<"VideoGrabber::Destructor\n";
  //VideoCaptures are released automatically within the Vector mCaptureVector
}


//----------------------------------------------------------------------------------------------------
bool VideoGrabber::onInit()
{

	//local and/or stored Parameters are already initialized

#if defined SHOW_INIT_INFORMATION_VIDEOGRABBER
  std::cout << "AviGrabber: Initialize Grabber with " << mNumCams << " cameras ..." << std::endl;
  for(unsigned int i=0; i<mNumCams;++i)
  {
     std::cout << "Channel "<< i<<": capture from: " << mSourceFileName.at(i) << "\n";
  }
  std::cout << std::flush;
#endif

  mImageMatVector.clear();
  mCaptureVector.clear();

	//----------------------------------------
  //open capture one by one, and create storage (cv::Mat) for it
  for(unsigned int i=0; i<mNumCams;++i)
  {
    VideoCapture capture(mSourceFileName.at(i));

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
      std::cout << "ERROR: Grabbing failed (Channel "<< i << ")." << std::endl;
      return false;
      //throws an initialization-exception, so programm will terminate
    }


  }
  // all grabbers successfully initialized

	//----------------------------------------
  // search for the smallest avi-file
  //unsigned int smallest=100000; //2000sec = 33h at 50Hz framerate
  unsigned int smallest= UINT_MAX;
  for(unsigned int i = 0; i < mNumCams; ++i)
  {
    unsigned int len = mCaptureVector.at(i).get(CV_CAP_PROP_FRAME_COUNT);
    if (len<smallest) smallest=len;
  }
  mFramesCount = smallest;

  //rewind
  //setPositionAbs(0);

	//----------------------------------------
  //check for equal FPS
  double fps = mCaptureVector.at(0).get(CV_CAP_PROP_FPS);
	if ( mNumCams > 1 ) //remove
	{

	  for(unsigned int i = 1; i < mNumCams; ++i)
	  {
		  double fps_test = mCaptureVector.at(i).get(CV_CAP_PROP_FPS);
		  if (fps!=fps_test)
		  {
			 std::cout << "ERROR: Different framerate in channel 0 and channel "<< i << "." << std::endl;
			 return false;
			 //throws an initialization-exception, so programm will terminate
		  }

	  }

	}

	//----------------------------------------
  //set stepsize for LoopedThread 
  setFps(fps*_mSpeedFactor);

  #if defined DEBUG_VIDEOGRABBER
        std::cout << "[AviGrabber::onInit] Initialize... finished" << std::endl;
  # endif

  return true;
} // onInit()





//----------------------------------------------------------------------------------------------------
bool VideoGrabber::onGrab()
{
    int result = true;

    for(unsigned int i = 0; i < mNumCams; ++i)                    //grab on all channels
    {
        (mCaptureVector.at(i))>> mImageMatVector.at(i);

        //check if the end of a channel is reached
        if(mImageMatVector.at(i).empty())
        {


            if (_mLoop)
            {
              setPositionAbs(0);                                  //rewind and grab first frame
              for(unsigned int i = 0; i < mNumCams; ++i)
              {
                  (mCaptureVector.at(i))>> mImageMatVector.at(i);
              }

              #if defined DEBUG_VIDEOGRABBER
                std::cout << "Video restart\n";
              #endif

              return true;
            }
            else
            {
              for(unsigned int i = 0; i < mNumCams; ++i)          //set all images to empty matrices
              {
                  mImageMatVector.at(i) = cv::Mat();
              }
              result = false;
            }
        }
    }


    return result;
}


// ----------------------------------------------------------------------------------------------------
std::string VideoGrabber::onGetPhysicalSourceInformation(unsigned int channel) const
{
  return mSourceFileName.at(channel);
}



// ----------------------------------------------------------------------------------------------------
void VideoGrabber::setSpeedFactor(double speedFactor)
{
  _mSpeedFactor = speedFactor;
  double fps = mCaptureVector.at(0).get(CV_CAP_PROP_FPS);

  //will restart the thread if running
  setFps(fps*_mSpeedFactor);
}

// ----------------------------------------------------------------------------------------------------
double  VideoGrabber::getSpeedFactor()
{
  return _mSpeedFactor;
}

// ----------------------------------------------------------------------------------------------------
void VideoGrabber::setLoop(bool loop)
{
    _mLoop = loop;
}


// ----------------------------------------------------------------------------------------------------
void VideoGrabber::setPositionRel(double newPositionRel)
{
  //mCaptureVector.at(0)->set(CV_CAP_PROP_POS_AVI_RATIO,newPositionRel); //does nothing

  if (newPositionRel<0 || newPositionRel>1)
  {
    CEDAR_THROW(cedar::aux::exc::IndexOutOfRangeException,"VideoGrabber::setPositionRel");
  }

  double max_pos = static_cast<double>(mFramesCount-1);

  //newPositionRel = [0..1]
  double new_pos_abs = max_pos * newPositionRel;

  if (new_pos_abs > max_pos)
    new_pos_abs=max_pos;

  if (new_pos_abs < 0)
    new_pos_abs = 0;

  //double test_pos_rel = 1. / ( mFramesCount / act_pos_abs );
  //double test_new_pos_rel = (test_pos_rel * newPositionRel);
  //double new_pos_abs = test_new_pos_rel * mFramesCount;

  /*
  double new_pos_abs = (act_pos_abs / (act_pos_rel*100)) *newPositionRel;
  int    np = static_cast<int> (new_pos_abs);
  */
  #if defined DEBUG_VIDEOGRABBER
        std::cout << "[AviGrabber::setPositionRel] new position wanted (relative 0..1): "<<newPositionRel << std::endl;
        std::cout << "[AviGrabber::setPositionRel] position set to frame: "<<new_pos_abs << std::endl;
        
  # endif
  for(unsigned int i = 0; i < mNumCams; ++i)
  {
      mCaptureVector.at(i).set(CV_CAP_PROP_POS_FRAMES,new_pos_abs);
  }
}


// ----------------------------------------------------------------------------------------------------
double VideoGrabber::getPositionRel()
{
	//the shortest file defines the length of the avi
	//it is possible, that the first file isn't the shortest
	//so we use absolute positions instead of relative
  //return  mCaptureVector.at(0).get(CV_CAP_PROP_POS_AVI_RATIO); //from 0..1;
  
  double pos_abs = static_cast<double>(getPositionAbs());
  double count = static_cast<double>(mFramesCount) ;
  return pos_abs / count;
}

// ----------------------------------------------------------------------------------------------------
void VideoGrabber::setPositionAbs(unsigned int newPositionAbs)
{

  if (newPositionAbs > (mFramesCount-1) || newPositionAbs < 0)
  {
    CEDAR_THROW(cedar::aux::exc::IndexOutOfRangeException,"VideoGrabber::setPositionRel");
  }

  #if defined DEBUG_VIDEOGRABBER
        std::cout << "[AviGrabber::setPositionAbs] current position: "<<getPositionAbs() << std::endl;
        std::cout << "[AviGrabber::setPositionAbs] position set to frame: "<<newPositionAbs << std::endl;
  # endif
  
  for(unsigned int i = 0; i < mNumCams; ++i)
  {
      (mCaptureVector.at(i)).set(CV_CAP_PROP_POS_FRAMES,newPositionAbs);
  }
}

// ----------------------------------------------------------------------------------------------------
unsigned int VideoGrabber::getPositionAbs()
{
  //the position in all avi's should be the same
  //we use the first capture-device
  return (mCaptureVector.at(0)).get(CV_CAP_PROP_POS_FRAMES);
}

// ----------------------------------------------------------------------------------------------------
unsigned int VideoGrabber::getFrameCount()
{
  return mFramesCount;
}


// ----------------------------------------------------------------------------------------------------
double VideoGrabber::getAviParam(unsigned int channel,int propId)
{
	if (channel >= mNumCams)
  {
    CEDAR_THROW(cedar::aux::exc::IndexOutOfRangeException,"VideoGrabber::getAviParam");
  }
  return mCaptureVector.at(channel).get(propId);
}

  // ----------------------------------------------------------------------------------------------------
double VideoGrabber::getAviParamFps (unsigned int channel)
{
  return getAviParam(channel,CV_CAP_PROP_FPS);
}

  // ----------------------------------------------------------------------------------------------------
double VideoGrabber::getAviParamFourcc (unsigned int channel)
{
  return getAviParam(channel,CV_CAP_PROP_FOURCC);
}



