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
    Email:       georg.hartinger@ini.rub.de
    Date:        2011 08 01

    Description: Implementation of the @em cedar::dev::sensors::visual::VideoGrabber class.

    Credits:

======================================================================================================================*/

// CEDAR INCLUDES
#include "cedar/devices/sensors/visual/VideoGrabber.h"
#include "cedar/auxiliaries/exceptions/IndexOutOfRangeException.h"

// SYSTEM INCLUDES


//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------


//----------------------------------------------------------------------------------------------------
//Constructor for single-file grabber
cedar::dev::sensors::visual::VideoGrabber::VideoGrabber(
                            const std::string& configFileName,
                            const std::string& aviFileName
                          )
:
cedar::dev::sensors::visual::GrabberInterface(configFileName)
{
  readInit(1,"VideoGrabber");

  //overwrite configuration with constructor-values
  getChannel(0)->sourceFileName = aviFileName;

  applyInit();
}

//----------------------------------------------------------------------------------------------------
//Constructor for stereo-file grabber
cedar::dev::sensors::visual::VideoGrabber::VideoGrabber(
                            const std::string& configFileName,
                            const std::string& aviFileName0,
                            const std::string& aviFileName1
                          )
:
cedar::dev::sensors::visual::GrabberInterface(configFileName)
{
  readInit(2,"VideoGrabber");

  //overwrite configuration with constructor-values
  getChannel(0)->sourceFileName = aviFileName0;
  getChannel(1)->sourceFileName = aviFileName1;
  applyInit();
}

//----------------------------------------------------------------------------------------------------
bool cedar::dev::sensors::visual::VideoGrabber::onDeclareParameters()
{
  bool result1 = addParameter(&_mLoop, "loop", true) == CONFIG_SUCCESS;
  bool result2 = addParameter(&_mSpeedFactor, "speedFactor", 1) == CONFIG_SUCCESS;
  return result1 && result2;
}

//----------------------------------------------------------------------------------------------------
cedar::dev::sensors::visual::VideoGrabber::~VideoGrabber()
{
  doCleanUp();
  #ifdef DEBUG_VIDEOGRABBER
    std::cout<<"[VideoGrabber::Destructor]"<< std::endl;
  #endif
}

//----------------------------------------------------------------------------------------------------------------------
//methods
//----------------------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------
void cedar::dev::sensors::visual::VideoGrabber::onAddChannel()
{
  //create the channel structure for one channel
  VideoChannelPtr channel(new VideoChannel);
  mChannels.push_back(channel);
}


//----------------------------------------------------------------------------------------------------
void cedar::dev::sensors::visual::VideoGrabber::onCleanUp()
{
  #ifdef DEBUG_CAMERAGRABBER
    std::cout<<"[VideoGrabber::onCleanUp]"<< std::endl;
  #endif

  //close all captures
  //mChannels.clear() is done in Grabberinterface
}

//----------------------------------------------------------------------------------------------------
bool cedar::dev::sensors::visual::VideoGrabber::onInit()
{
  //local and/or stored Parameters are already initialized

  #ifdef SHOW_INIT_INFORMATION_VIDEOGRABBER
    std::cout << "VideoGrabber: Initialize Grabber with " << mNumCams << " cameras ..." << std::endl;
    for (unsigned int channel = 0; channel < mNumCams; ++channel)
    {
      std::cout << "Channel " << channel << ": capture from: " << getChannel(channel)->sourceFileName << "\n";
    }
    std::cout << std::flush;
  #endif


  //----------------------------------------
  //open capture one by one
  for (unsigned int channel = 0; channel < mNumCams; ++channel)
  {
    cv::VideoCapture capture(getChannel(channel)->sourceFileName);

    if (capture.isOpened())
    {
      getChannel(channel)->videoCapture = capture;

      //grab first frame to initialize the frame-memory
      getChannel(channel)->videoCapture >> getChannel(channel)->imageMat;
    }
    else
    {
      std::cout << "[VideoGrabber::onInit] ERROR: Grabbing failed"
                << "\tChannel " << channel << ": "<< getChannel(channel)->sourceFileName << "\n"
                << std::endl;

      return false;  //throws an initialization-exception
    }
  }
  //all grabbers successfully initialized

  //----------------------------------------
  //search for the smallest avi-file
  unsigned int smallest = UINT_MAX;
  for (unsigned int channel = 0; channel < mNumCams; ++channel)
  {
    unsigned int len = getChannel(channel)->videoCapture.get(CV_CAP_PROP_FRAME_COUNT);
    if (len < smallest)
    {
      smallest = len;
    }
  }
  mFramesCount = smallest;

  //----------------------------------------
  //check for equal FPS
  double fps_ch0 = getChannel(0)->videoCapture.get(CV_CAP_PROP_FPS);
  if (mNumCams > 1)
  {
    double fps_ch1 = getChannel(1)->videoCapture.get(CV_CAP_PROP_FPS);
    if (fps_ch0 != fps_ch1)
    {
      std::cout << "[VideoGrabber::onInit] ERROR: Different framerates of channels 0 and 1"
                << std::endl;
      return false;  //throws an initialization-exception
    }
  }

  //----------------------------------------
  //set stepsize for LoopedThread
  setFps(fps_ch0 * _mSpeedFactor);
  #ifdef DEBUG_VIDEOGRABBER
    std::cout << "[VideoGrabber::onInit] Initialize... finished" << std::endl;
  # endif

  return true;
} //onInit()

//----------------------------------------------------------------------------------------------------
bool cedar::dev::sensors::visual::VideoGrabber::onGrab()
{
  int result = true;

  //grab on all channels
  for (unsigned int channel = 0; channel < mNumCams; ++channel)
  {
    (getChannel(channel)->videoCapture) >> getChannel(channel)->imageMat;

    //check if the end of a channel is reached
    if (getChannel(channel)->imageMat.empty())
    {

      unsigned int pos_Abs = getChannel(0)->videoCapture.get(CV_CAP_PROP_POS_FRAMES);

      #ifdef DEBUG_VIDEOGRABBER
        std::cout << "[VideoGrabber::onGrab] Channel  :" << channel << " empty" << std::endl;
        std::cout << "[VideoGrabber::onGrab] Frame nr.:" << pos_Abs << std::endl;
      #endif

      //error or end of file?
      if (getPositionAbsolute() == (mFramesCount-1))
      {

        if (_mLoop)
        {
          //rewind all channels and grab first frame
          setPositionAbsolute(0);
          for (unsigned int i = 0; i < mNumCams; ++i)
          {
            (getChannel(i)->videoCapture) >> getChannel(i)->imageMat;
          }
          #ifdef DEBUG_VIDEOGRABBER
            std::cout << "[VideoGrabber::onGrab] Video restart\n";
          #endif

          //all Channels grabbed
          return true;
        }
        else
        {
          //the last frame of the file remains in image buffer
          result = false;
        }
      }
      else
      {
        //error while reading
        result = false;
      }
    }
  }
  return result;
}

//----------------------------------------------------------------------------------------------------
const std::string& cedar::dev::sensors::visual::VideoGrabber::onGetSourceInfo(unsigned int channel) const
{
  if (channel >= mNumCams)
  {
    CEDAR_THROW(cedar::aux::exc::IndexOutOfRangeException,"VideoGrabber::onGetSourceInfo");
  }
  return getChannel(channel)->sourceFileName;
}

//----------------------------------------------------------------------------------------------------
void cedar::dev::sensors::visual::VideoGrabber::setSpeedFactor(double speedFactor)
{
  _mSpeedFactor = speedFactor;
  double fps = getChannel(0)->videoCapture.get(CV_CAP_PROP_FPS);

  //set fps and restart the thread if running
  setFps(fps * _mSpeedFactor);
}

//----------------------------------------------------------------------------------------------------
double cedar::dev::sensors::visual::VideoGrabber::getSpeedFactor() const
{
  return _mSpeedFactor;
}

//----------------------------------------------------------------------------------------------------
void cedar::dev::sensors::visual::VideoGrabber::setLoop(bool loop)
{
  _mLoop = loop;
}

//----------------------------------------------------------------------------------------------------
void cedar::dev::sensors::visual::VideoGrabber::setPositionRelative(double newPositionRel)
{
  //getChannel(0)->videoCapture->set(CV_CAP_PROP_POS_AVI_RATIO,newPositionRel); //does nothing

  if (newPositionRel < 0 || newPositionRel > 1)
  {
    CEDAR_THROW(cedar::aux::exc::IndexOutOfRangeException,"VideoGrabber::setPositionRel");
  }

  double max_pos = static_cast<double>(mFramesCount - 1);

  //newPositionRel = [0..1]
  double new_pos_abs = max_pos * newPositionRel;

  if (new_pos_abs > max_pos)
  {
    new_pos_abs = max_pos;
  }

  if (new_pos_abs < 0)
  {
    new_pos_abs = 0;
  }

  #ifdef DEBUG_VIDEOGRABBER
    std::cout << "[VideoGrabber::setPositionRel] new position wanted (relative 0..1): " << newPositionRel
              << std::endl;
    std::cout << "[VideoGrabber::setPositionRel] position set to frame: " << new_pos_abs << std::endl;
  # endif

  for (unsigned int channel = 0; channel < mNumCams; ++channel)
  {
    getChannel(channel)->videoCapture.set(CV_CAP_PROP_POS_FRAMES,new_pos_abs);
  }
}

//----------------------------------------------------------------------------------------------------
double cedar::dev::sensors::visual::VideoGrabber::getPositionRelative()
{
  //the shortest file defines the length of the avi
  //it is possible, that the first file isn't the shortest
  //so we use absolute positions instead of relative
  //return  getChannel(0)->videoCapture.get(CV_CAP_PROP_POS_AVI_RATIO); //from 0..1;

  double pos_abs = static_cast<double>(this->getPositionAbsolute());
  double count   = static_cast<double>(mFramesCount);
  return pos_abs / count;
}

//----------------------------------------------------------------------------------------------------
void cedar::dev::sensors::visual::VideoGrabber::setPositionAbsolute(unsigned int newPositionAbs)
{
  //newPos can't be smaller than zero, because it is an unsigned int
  //only check upper border
  if (newPositionAbs + 1 > mFramesCount)
  {
    CEDAR_THROW(cedar::aux::exc::IndexOutOfRangeException,"VideoGrabber::setPositionRel");
  }

  #ifdef DEBUG_VIDEOGRABBER
    std::cout << "[VideoGrabber::setPositionAbs] current position: " << getPositionAbsolute() << std::endl;
    std::cout << "[VideoGrabber::setPositionAbs] position set to frame: " << newPositionAbs << std::endl;
  # endif

  for (unsigned int channel = 0; channel < mNumCams; ++channel)
  {
    (getChannel(channel)->videoCapture).set(CV_CAP_PROP_POS_FRAMES,newPositionAbs);
  }
}

//----------------------------------------------------------------------------------------------------
unsigned int cedar::dev::sensors::visual::VideoGrabber::getPositionAbsolute()
{
  //the position in all avi's should be the same
  //we use the first capture-device
  return getChannel(0)->videoCapture.get(CV_CAP_PROP_POS_FRAMES);
}

//----------------------------------------------------------------------------------------------------
unsigned int cedar::dev::sensors::visual::VideoGrabber::getFrameCount() const
{
  return mFramesCount;
}

//----------------------------------------------------------------------------------------------------
double cedar::dev::sensors::visual::VideoGrabber::getSourceProperty(unsigned int channel,int propId)
{
  if (channel >= mNumCams)
  {
    CEDAR_THROW(cedar::aux::exc::IndexOutOfRangeException,"VideoGrabber::getAviParam");
  }
  return getChannel(channel)->videoCapture.get(propId);
}

//----------------------------------------------------------------------------------------------------
double cedar::dev::sensors::visual::VideoGrabber::getSourceFps(unsigned int channel)
{
  return getSourceProperty(channel,CV_CAP_PROP_FPS);
}

//----------------------------------------------------------------------------------------------------
double cedar::dev::sensors::visual::VideoGrabber::getSourceEncoding(unsigned int channel)
{
  return getSourceProperty(channel,CV_CAP_PROP_FOURCC);
}

