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
#include "cedar/auxiliaries/exceptions.h"

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
cedar::dev::sensors::visual::Grabber(configFileName),
mFramesCount(0),
_mLooped(new cedar::aux::BoolParameter(this, "looped", false)),
_mSpeedFactor(new cedar::aux::IntParameter(this, "speedFactor", 1))
{
  cedar::aux::LogSingleton::getInstance()->allocating(this);
  readInit(1);

  //overwrite configuration with constructor-values
  getChannel(0)->mSourceFileName = aviFileName;
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
cedar::dev::sensors::visual::Grabber(configFileName),
mFramesCount(0),
_mLooped(new cedar::aux::BoolParameter(this, "looped", false)),
_mSpeedFactor(new cedar::aux::IntParameter(this, "speedFactor", 1))
{
  cedar::aux::LogSingleton::getInstance()->allocating(this);
  readInit(2);

  //overwrite configuration with constructor-values
  getChannel(0)->mSourceFileName = aviFileName0;
  getChannel(1)->mSourceFileName = aviFileName1;
  applyInit();
}

//----------------------------------------------------------------------------------------------------
bool cedar::dev::sensors::visual::VideoGrabber::onDeclareParameters()
{
  //bool result1 = addParameter(&_mLooped, "looped", true) == CONFIG_SUCCESS;
  //bool result2 = addParameter(&_mSpeedFactor, "speedFactor", 1) == CONFIG_SUCCESS;
  //return result1 && result2;
  return true;
}

//----------------------------------------------------------------------------------------------------
cedar::dev::sensors::visual::VideoGrabber::~VideoGrabber()
{
  doCleanUp();
  cedar::aux::LogSingleton::getInstance()->freeing(this);
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
  //close all captures
  //mChannels.clear() is done in Grabberinterface
}

//----------------------------------------------------------------------------------------------------
bool cedar::dev::sensors::visual::VideoGrabber::onInit()
{
  //local and/or stored Parameters are already initialized

  std::stringstream init_message;
  init_message << ": Initialize video grabber with " << mNumCams << " files ..." << std::endl;
  for (unsigned int i = 0; i < mNumCams; ++i)
  {
    init_message << "Channel " << i << ": capture from: " << getChannel(i)->mSourceFileName << std::endl;
  }
  cedar::aux::LogSingleton::getInstance()->systemInfo
                                           (
                                             this->getName() + init_message.str(),
                                             "cedar::dev::sensors::visual::VideoGrabber::onInit()"
                                           );


  //----------------------------------------
  //open capture one by one
  for (unsigned int channel = 0; channel < mNumCams; ++channel)
  {
    cv::VideoCapture capture(getChannel(channel)->mSourceFileName);

    if (capture.isOpened())
    {
      getChannel(channel)->mVideoCapture = capture;

      //grab first frame to initialize the frame-memory
      getChannel(channel)->mVideoCapture >> getChannel(channel)->mImageMat;
    }
    else
    {
      cedar::aux::LogSingleton::getInstance()->error
                                               (
                                                 this->getName() + ": Grabbing failed on Channel "
                                                  + boost::lexical_cast<std::string>(channel) + " from \""
                                                  + getChannel(channel)->mSourceFileName + "\"",
                                                "cedar::dev::sensors::visual::VideoGrabber::onInit()"
                                               );
      return false;  //throws an initialization-exception
    }
  }
  //all grabbers successfully initialized

  //----------------------------------------
  //search for the smallest avi-file
  unsigned int smallest = UINT_MAX;
  for (unsigned int channel = 0; channel < mNumCams; ++channel)
  {
    unsigned int len = getChannel(channel)->mVideoCapture.get(CV_CAP_PROP_FRAME_COUNT);
    if (len < smallest)
    {
      smallest = len;
    }
  }
  mFramesCount = smallest;

  //----------------------------------------
  //check for equal FPS
  double fps_ch0 = getChannel(0)->mVideoCapture.get(CV_CAP_PROP_FPS);
  if (mNumCams > 1)
  {
    double fps_ch1 = getChannel(1)->mVideoCapture.get(CV_CAP_PROP_FPS);
    if (fps_ch0 != fps_ch1)
    {
      cedar::aux::LogSingleton::getInstance()->error
                                               (
                                                 this->getName()
                                                   + ": Different framerates of channels 0 and 1",
                                                 "cedar::dev::sensors::visual::VideoGrabber::onInit()"
                                               );
      return false;  //throws an initialization-exception
    }
  }

  //----------------------------------------
  //set stepsize for LoopedThread
  setFps(fps_ch0 * _mSpeedFactor->getValue());

  return true;
} //onInit()

//----------------------------------------------------------------------------------------------------
bool cedar::dev::sensors::visual::VideoGrabber::onGrab()
{
  int result = true;

  //grab on all channels
  for (unsigned int channel = 0; channel < mNumCams; ++channel)
  {
    (getChannel(channel)->mVideoCapture) >> getChannel(channel)->mImageMat;

    //check if the end of a channel is reached
    if (getChannel(channel)->mImageMat.empty())
    {

      /*
      unsigned int pos_Abs = getChannel(0)->mVideoCapture.get(CV_CAP_PROP_POS_FRAMES);
      std::stringstream debug_message;
      debug_message << "[VideoGrabber::onGrab] Channel  :" << channel << " empty" << std::endl;
      debug_message << "[VideoGrabber::onGrab] Frame nr.:" << pos_Abs << std::endl;
      cedar::aux::LogSingleton::getInstance()->debugMessage
                                               (
                                                 this->getName() + ": " + debug_message.str(),
                                                 "cedar::dev::sensors::visual::VideoGrabber::onGrab()"
                                               );
      */
      //error or end of file?
      if (getPositionAbsolute() == (mFramesCount))
      //if (getPositionRelative() > 0.99)
      {

        if (_mLooped)
        {
          //rewind all channels and grab first frame
          setPositionAbsolute(0);
          for (unsigned int i = 0; i < mNumCams; ++i)
          {
            (getChannel(i)->mVideoCapture) >> getChannel(i)->mImageMat;
          }
          cedar::aux::LogSingleton::getInstance()->debugMessage
                                                    (
                                                      this->getName() + ": Video restarted",
                                                      "cedar::dev::sensors::visual::VideoGrabber::onGrab()"
                                                    );

          //all Channels grabbed
          return true;
        }
        else
        {
          // loop is false and the last frame was already grabbed
          // the last frame of the file doesn't remain in image buffer
          result = true;
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
void cedar::dev::sensors::visual::VideoGrabber::onUpdateSourceInfo(unsigned int channel)
{
  //value of channel is already checked by GraberInterface::getSourceInfo()
  getChannel(channel)->mChannelInfo = getChannel(channel)->mSourceFileName;
}

//----------------------------------------------------------------------------------------------------
void cedar::dev::sensors::visual::VideoGrabber::setSpeedFactor(double speedFactor)
{
  _mSpeedFactor->setValue(speedFactor);
  double fps = getChannel(0)->mVideoCapture.get(CV_CAP_PROP_FPS);

  //set fps and restart the thread if running
  setFps(fps * _mSpeedFactor->getValue());
}

//----------------------------------------------------------------------------------------------------
double cedar::dev::sensors::visual::VideoGrabber::getSpeedFactor() const
{
  return _mSpeedFactor->getValue();
}

//----------------------------------------------------------------------------------------------------
void cedar::dev::sensors::visual::VideoGrabber::setLooped(bool loop)
{
  std::string message = "Set Loop ";
  if (loop)
  {
    message = message + "ON";
  }
  else
  {
    message = message + "OFF";
  }
  cedar::aux::LogSingleton::getInstance()->debugMessage(message,"cedar::dev::sensors::visual::VideoGrabber::setLoop()");
  _mLooped->setValue(loop);
}
//----------------------------------------------------------------------------------------------------
bool cedar::dev::sensors::visual::VideoGrabber::getLooped()
{
  return _mLooped;
}

//----------------------------------------------------------------------------------------------------
void cedar::dev::sensors::visual::VideoGrabber::setPositionRelative(double newPositionRel)
{
  //getChannel(0)->mVideoCapture->set(CV_CAP_PROP_POS_AVI_RATIO,newPositionRel); //does nothing

  if (newPositionRel < 0 || newPositionRel > 1)
  {
    CEDAR_THROW(cedar::aux::IndexOutOfRangeException,"VideoGrabber::setPositionRelative");
  }

  double max_pos = static_cast<double>(mFramesCount - 1);

  //newPositionRel = [0..1]
  double new_pos_abs = max_pos * newPositionRel;

  //5 frames offset from the end
  int max_wanted_pos = max_pos-5;
  if (new_pos_abs > max_wanted_pos)
  {
    new_pos_abs = max_wanted_pos;
  }

  if (new_pos_abs < 0)
  {
    new_pos_abs = 0;
  }

  std::stringstream debug_message;
  debug_message << std::endl <<"\tNew position wanted (relative 0..1): " << newPositionRel << std::endl;
  debug_message << "\tPosition set to frame: " << new_pos_abs << std::endl;
  cedar::aux::LogSingleton::getInstance()->debugMessage
                                            (
                                              this->getName() + ": " + debug_message.str(),
                                              "cedar::dev::sensors::visual::VideoGrabber::setPositionRelative()"
                                            );

  for (unsigned int channel = 0; channel < mNumCams; ++channel)
  {
    getChannel(channel)->mVideoCapture.set(CV_CAP_PROP_POS_FRAMES,new_pos_abs);
  }
}

//----------------------------------------------------------------------------------------------------
double cedar::dev::sensors::visual::VideoGrabber::getPositionRelative()
{
  //the shortest file defines the length of the avi
  //it is possible, that the first file isn't the shortest
  //so we use absolute positions instead of relative
  //return  getChannel(0)->mVideoCapture.get(CV_CAP_PROP_POS_AVI_RATIO); //from 0..1;

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
    CEDAR_THROW(cedar::aux::IndexOutOfRangeException,"VideoGrabber::setPositionAbsolute");
  }
  std::stringstream debug_message;
  debug_message << std::endl <<"\tCurrent position: " << getPositionAbsolute() << std::endl
                << "\tPosition set to frame: " << newPositionAbs << std::endl;
  cedar::aux::LogSingleton::getInstance()->debugMessage
                                            (
                                              this->getName() + ": " + debug_message.str(),
                                              "cedar::dev::sensors::visual::VideoGrabber::setPositionAbsolute()"
                                            );

  for (unsigned int channel = 0; channel < mNumCams; ++channel)
  {
    (getChannel(channel)->mVideoCapture).set(CV_CAP_PROP_POS_FRAMES,newPositionAbs);
  }
}

//----------------------------------------------------------------------------------------------------
unsigned int cedar::dev::sensors::visual::VideoGrabber::getPositionAbsolute()
{
  //the position in all avi's should be the same
  return getChannel(0)->mVideoCapture.get(CV_CAP_PROP_POS_FRAMES);
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
    CEDAR_THROW(cedar::aux::IndexOutOfRangeException,"VideoGrabber::getAviParam");
  }
  return getChannel(channel)->mVideoCapture.get(propId);
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

