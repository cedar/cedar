/*======================================================================================================================

    Copyright 2011, 2012 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany

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

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/devices/sensors/visual/VideoGrabber.h"
#include "cedar/auxiliaries/exceptions.h"
#include "cedar/auxiliaries/casts.h"

// SYSTEM INCLUDES

//----------------------------------------------------------------------------------------------------------------------
// register the class
//----------------------------------------------------------------------------------------------------------------------
namespace
{
  bool declared
    = cedar::dev::sensors::visual::Grabber::ChannelManagerSingleton::getInstance()
        ->registerType<cedar::dev::sensors::visual::VideoGrabber::VideoChannelPtr>();
}


//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------


//----------------------------------------------------------------------------------------------------
//Constructor for single-file grabber
cedar::dev::sensors::visual::VideoGrabber::VideoGrabber
(
  const std::string& videoFileName,
  bool looped,
  bool speedFactor
)
:
cedar::dev::sensors::visual::Grabber
(
  "VideoGrabber",
  cedar::dev::sensors::visual::VideoGrabber::VideoChannelPtr
  (
    new cedar::dev::sensors::visual::VideoGrabber::VideoChannel(videoFileName)
  )
),
_mLooped(new cedar::aux::BoolParameter(this, "looped", looped)),
_mSpeedFactor(new cedar::aux::IntParameter(this, "speed factor", speedFactor,1,20))
{
  init();
}

//----------------------------------------------------------------------------------------------------
//Constructor for stereo-file grabber
cedar::dev::sensors::visual::VideoGrabber::VideoGrabber
(
  const std::string& videoFileName0,
  const std::string& videoFileName1,
  bool looped,
  bool speedFactor
)
:
cedar::dev::sensors::visual::Grabber
(
  "StereoVideoGrabber",
  cedar::dev::sensors::visual::VideoGrabber::VideoChannelPtr
  (
    new cedar::dev::sensors::visual::VideoGrabber::VideoChannel(videoFileName0)
  ),
  cedar::dev::sensors::visual::VideoGrabber::VideoChannelPtr
  (
    new cedar::dev::sensors::visual::VideoGrabber::VideoChannel(videoFileName1)
  )
),
_mLooped(new cedar::aux::BoolParameter(this, "looped", looped)),
_mSpeedFactor(new cedar::aux::IntParameter(this, "speed factor", speedFactor,1,20))
{
  init();
}

//----------------------------------------------------------------------------------------------------
void cedar::dev::sensors::visual::VideoGrabber::init()
{
  cedar::aux::LogSingleton::getInstance()->allocating(this);

  mFramesCount = 0;

  QObject::connect (_mSpeedFactor.get(),SIGNAL(valueChanged()),this, SLOT(speedFactorChanged()));

  //watch filename on every channel
  for (unsigned int channel=0; channel<_mChannels->size(); ++channel)
  {
    QObject::connect
             (
               getVideoChannel(channel)->_mSourceFileName.get(),
               SIGNAL(valueChanged()),
               this,
               SLOT(fileNameChanged())
             );
  }

  //watch if channel added to ObjectParameterList
  _mChannels->connectToObjectAddedSignal
              (
                boost::bind(&cedar::dev::sensors::visual::VideoGrabber::channelAdded,this,_1)
              );
}

//----------------------------------------------------------------------------------------------------
cedar::dev::sensors::visual::VideoGrabber::~VideoGrabber()
{
  doCleanUp();
  cedar::aux::LogSingleton::getInstance()->freeing(this);
}

//--------------------------------------------------------------------------------------------------------------------
// slots
//--------------------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------
void cedar::dev::sensors::visual::VideoGrabber::channelAdded(int index)
{
  QObject::connect
           (
             getVideoChannel(index)->_mSourceFileName.get(),
             SIGNAL(valueChanged()),
             this,
             SLOT(fileNameChanged())
           );
}

//----------------------------------------------------------------------------------------------------
void cedar::dev::sensors::visual::VideoGrabber::speedFactorChanged()
{
  if (mCaptureDeviceCreated)
  {
    std::cout << "VideoGrabber::speedFactorChanged" << std::endl;

    double fps = getVideoChannel(0)->mVideoCapture.get(CV_CAP_PROP_FPS);
    setFps(fps * _mSpeedFactor->getValue());
    emit doSpeedFactorChanged();
  }
}

//----------------------------------------------------------------------------------------------------
void cedar::dev::sensors::visual::VideoGrabber::fileNameChanged()
{
  // reset all channels
  if (Grabber::applyParameter())
  {
    emit doVideoChanged();
  }
}

//----------------------------------------------------------------------------------------------------------------------
//methods
//----------------------------------------------------------------------------------------------------------------------


//----------------------------------------------------------------------------------------------------
void cedar::dev::sensors::visual::VideoGrabber::onCleanUp()
{
  //close all captures
  //mChannels.clear() is done in Grabberinterface
}

//----------------------------------------------------------------------------------------------------
bool cedar::dev::sensors::visual::VideoGrabber::onCreateGrabber()
{
  //local and/or stored Parameters are already initialized

  unsigned int num_cams = getNumCams();

  //check if filenames are there
  if (getVideoChannel(0)->_mSourceFileName->getPath() == ".")
  {
    return false;
  }

  std::stringstream init_message;
  init_message << ": Initialize video grabber with " << num_cams << " files ..." << std::endl;
  for(unsigned int channel = 0; channel < num_cams; ++channel)
  {
    init_message << "Channel " << channel << ": capture from: "
                 << getVideoChannel(channel)->_mSourceFileName->getPath() << std::endl;
  }
  cedar::aux::LogSingleton::getInstance()->debugMessage
                                           (
                                             this->getName() + init_message.str(),
                                             "cedar::dev::sensors::visual::VideoGrabber::onInit()"
                                           );


  //----------------------------------------
  //open capture one by one
  for (unsigned int channel = 0; channel < num_cams; ++channel)
  {
    cv::VideoCapture capture(getVideoChannel(channel)->_mSourceFileName->getPath());

    if (capture.isOpened())
    {
      getVideoChannel(channel)->mVideoCapture = capture;
      getVideoChannel(channel)->mVideoCapture >> getVideoChannel(channel)->mImageMat;
      setChannelInfo(channel);
    }
    else
    {
      cedar::aux::LogSingleton::getInstance()->error
                                               (
                                                 this->getName() + ": Grabbing failed on Channel "
                                                  + boost::lexical_cast<std::string>(channel) + " from \""
                                                  + getVideoChannel(channel)->_mSourceFileName->getPath() + "\"",
                                                "cedar::dev::sensors::visual::VideoGrabber::onInit()"
                                               );
      return false;
    }
  }
  //all grabbers successfully initialized

  //----------------------------------------
  //search for the smallest avi-file
  unsigned int smallest = UINT_MAX;
  for (unsigned int channel = 0; channel < num_cams; ++channel)
  {
    unsigned int len = getVideoChannel(channel)->mVideoCapture.get(CV_CAP_PROP_FRAME_COUNT);
    if (len < smallest)
    {
      smallest = len;
    }
  }
  mFramesCount = smallest;

  //----------------------------------------
  //check for equal FPS
  double fps_ch0 = getVideoChannel(0)->mVideoCapture.get(CV_CAP_PROP_FPS);
  if (num_cams > 1)
  {
    double fps_ch1 = getVideoChannel(1)->mVideoCapture.get(CV_CAP_PROP_FPS);
    if (fps_ch0 != fps_ch1)
    {
      cedar::aux::LogSingleton::getInstance()->error
                                               (
                                                 this->getName()
                                                   + ": Different framerates of channels 0 and 1",
                                                 "cedar::dev::sensors::visual::VideoGrabber::onInit()"
                                               );
      return false;
    }
  }

  //----------------------------------------
  //set stepsize for LoopedThread
  setFps(fps_ch0 * _mSpeedFactor->getValue());

  return true;
}

//----------------------------------------------------------------------------------------------------
void cedar::dev::sensors::visual::VideoGrabber::onCloseGrabber()
{
  this->stopRecording();
  unsigned int num_cams = getNumCams();
  for(unsigned int channel = 0; channel < num_cams; ++channel)
  {
    //there is no cv::VideoCapture.close() method, so we assign an empty one. The old will be released automatically
    getVideoChannel(channel)->mVideoCapture = cv::VideoCapture();
  }
}


//----------------------------------------------------------------------------------------------------
bool cedar::dev::sensors::visual::VideoGrabber::onGrab()
{
  int result = true;

  //grab on all channels
  unsigned int num_cams = getNumCams();
  for(unsigned int channel = 0; channel < num_cams; ++channel)
  {
    (getVideoChannel(channel)->mVideoCapture) >> getVideoChannel(channel)->mImageMat;

    //check if the end of a channel is reached
    if (getVideoChannel(channel)->mImageMat.empty())
    {

      /*
      unsigned int pos_Abs = getVideoChannel(0)->mVideoCapture.get(CV_CAP_PROP_POS_FRAMES);
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

        if (_mLooped->getValue())
        {
          //rewind all channels and grab first frame
          setPositionAbsolute(0);
          for (unsigned int i = 0; i < num_cams; ++i)
          {
            (getVideoChannel(i)->mVideoCapture) >> getVideoChannel(i)->mImageMat;
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
void cedar::dev::sensors::visual::VideoGrabber::setChannelInfo(unsigned int channel)
{
  getVideoChannel(channel)->mChannelInfo = this->getName()
                                           + " - channel " + boost::lexical_cast<std::string>(channel)
                                           + ": " + getVideoChannel(channel)->_mSourceFileName->getPath();
}

//----------------------------------------------------------------------------------------------------
void cedar::dev::sensors::visual::VideoGrabber::setSpeedFactor(double speedFactor)
{
  //if (_mSpeedFactor->getValue() != speedFactor)
  //{
    _mSpeedFactor->setValue(speedFactor);
  //}
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
  return _mLooped->getValue();
}

//----------------------------------------------------------------------------------------------------
void cedar::dev::sensors::visual::VideoGrabber::setPositionRelative(double newPositionRel)
{
  //getVideoChannel(0)->mVideoCapture->set(CV_CAP_PROP_POS_AVI_RATIO,newPositionRel); //does nothing

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

  unsigned int num_cams = getNumCams();
  for(unsigned int channel = 0; channel < num_cams; ++channel)
  {
    getVideoChannel(channel)->mVideoCapture.set(CV_CAP_PROP_POS_FRAMES,new_pos_abs);
  }
}

//----------------------------------------------------------------------------------------------------
double cedar::dev::sensors::visual::VideoGrabber::getPositionRelative()
{
  //the shortest file defines the length of the avi
  //it is possible, that the first file isn't the shortest
  //so we use absolute positions instead of relative
  //return  getVideoChannel(0)->mVideoCapture.get(CV_CAP_PROP_POS_AVI_RATIO); //from 0..1;

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

  unsigned int num_cams = getNumCams();
  for(unsigned int channel = 0; channel < num_cams; ++channel)
  {
    (getVideoChannel(channel)->mVideoCapture).set(CV_CAP_PROP_POS_FRAMES,newPositionAbs);
  }
}

//----------------------------------------------------------------------------------------------------
unsigned int cedar::dev::sensors::visual::VideoGrabber::getPositionAbsolute()
{
  //the position in all avi's should be the same
  return getVideoChannel(0)->mVideoCapture.get(CV_CAP_PROP_POS_FRAMES);
}

//----------------------------------------------------------------------------------------------------
unsigned int cedar::dev::sensors::visual::VideoGrabber::getFrameCount() const
{
  return mFramesCount;
}

//----------------------------------------------------------------------------------------------------
double cedar::dev::sensors::visual::VideoGrabber::getSourceProperty(unsigned int channel,int propId)
{
  if (channel >= getNumCams())
  {
    CEDAR_THROW(cedar::aux::IndexOutOfRangeException,"VideoGrabber::getSourceProperty");
  }
  return getVideoChannel(channel)->mVideoCapture.get(propId);
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

//----------------------------------------------------------------------------------------------------
const std::string cedar::dev::sensors::visual::VideoGrabber::getSourceFile(unsigned int channel)
{
  if (channel >= getNumCams())
  {
    CEDAR_THROW(cedar::aux::IndexOutOfRangeException,"VideoGrabber::setSourceFile");
  }

  return getVideoChannel(channel)->_mSourceFileName->getPath();
}
