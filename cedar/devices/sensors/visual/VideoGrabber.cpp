/*======================================================================================================================

    Copyright 2011, 2012, 2013 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany

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
#include "cedar/devices/sensors/visual/exceptions.h"
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
        ->registerType<cedar::dev::sensors::visual::VideoChannelPtr>();
}


//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------


// Constructor for single-file grabber
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
  cedar::dev::sensors::visual::VideoChannelPtr
  (
    new cedar::dev::sensors::visual::VideoChannel(videoFileName)
  )
),
_mLooped(new cedar::aux::BoolParameter(this, "looped", looped)),
_mSpeedFactor(new cedar::aux::IntParameter(this, "speed factor", speedFactor,1,20))
{
  init();
}


// Constructor for stereo-file grabber
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
  cedar::dev::sensors::visual::VideoChannelPtr
  (
    new cedar::dev::sensors::visual::VideoChannel(videoFileName0)
  ),
  cedar::dev::sensors::visual::VideoChannelPtr
  (
    new cedar::dev::sensors::visual::VideoChannel(videoFileName1)
  )
),
_mLooped(new cedar::aux::BoolParameter(this, "looped", looped)),
_mSpeedFactor(new cedar::aux::IntParameter(this, "speed factor", speedFactor,1,20))
{
  init();
}


void cedar::dev::sensors::visual::VideoGrabber::init()
{
  cedar::aux::LogSingleton::getInstance()->allocating(this);

  mFramesCount = 0;

  QObject::connect (_mSpeedFactor.get(),SIGNAL(valueChanged()),this, SLOT(speedFactorChanged()));

  // watch filename on every channel
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

  // watch if channel added to ObjectParameterList
  _mChannels->connectToObjectAddedSignal
              (
                boost::bind(&cedar::dev::sensors::visual::VideoGrabber::channelAdded,this,_1)
              );
}


cedar::dev::sensors::visual::VideoGrabber::~VideoGrabber()
{
  doCleanUp();
  cedar::aux::LogSingleton::getInstance()->freeing(this);
}

//--------------------------------------------------------------------------------------------------------------------
// slots
//--------------------------------------------------------------------------------------------------------------------


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


void cedar::dev::sensors::visual::VideoGrabber::speedFactorChanged()
{
  if (isCreated())
  {
    double fps = getVideoChannel(0)->mVideoCapture.get(CV_CAP_PROP_FPS);
    setFramerate(fps * _mSpeedFactor->getValue());
    emit doSpeedFactorChanged();
  }
}


void cedar::dev::sensors::visual::VideoGrabber::fileNameChanged()
{
  // set all channels to their parameters
  if (Grabber::applyParameter())
  {
    emit doVideoChanged();
  }
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------


void cedar::dev::sensors::visual::VideoGrabber::onCleanUp()
{
  // close all captures
  // mChannels.clear() is done in Grabberinterface
}


void cedar::dev::sensors::visual::VideoGrabber::onCreateGrabber()
{
  // local and/or stored Parameters are already initialized
  unsigned int num_channels = getNumChannels();

  // check if filenames are there (no filename could happen on startup in the processingGui)
  for (unsigned int channel = 0; channel < num_channels; ++channel)
  {
    if (getVideoChannel(channel)->_mSourceFileName->getPath() == ".")
    {
      std::string msg = this->getName() + " channel " + boost::lexical_cast<std::string>(channel)
                          + ": No video-file specified";
      CEDAR_THROW(cedar::dev::sensors::visual::CreateGrabberException,msg)
    }
  }

  std::stringstream init_message;
  init_message << ": Initialize video grabber with " << num_channels << " files ..." << std::endl;
  for (unsigned int channel = 0; channel < num_channels; ++channel)
  {
    init_message << "Channel " << channel << ": capture from: "
                 << getVideoChannel(channel)->_mSourceFileName->getPath() << std::endl;
  }
  cedar::aux::LogSingleton::getInstance()->debugMessage
                                           (
                                             this->getName() + init_message.str(),
                                             "cedar::dev::sensors::visual::VideoGrabber::onCreateGrabber()"
                                           );


  // open capture one by one
  for (unsigned int channel = 0; channel < num_channels; ++channel)
  {
    cv::VideoCapture capture(getVideoChannel(channel)->_mSourceFileName->getPath());

    if (!capture.isOpened())
    {
      std::string msg = this->getName() + ": Grabbing failed on Channel " + boost::lexical_cast<std::string>(channel)
                        + " from \"" + getVideoChannel(channel)->_mSourceFileName->getPath() + "\"";
      CEDAR_THROW(cedar::dev::sensors::visual::CreateGrabberException,msg)
    }

    // open file
    getVideoChannel(channel)->mVideoCapture = capture;

    // grab first frame
    getVideoChannel(channel)->mVideoCapture >> getImageMat(channel);
  }

  // all grabbers successfully initialized, now search for the smallest avi-file
  unsigned int smallest = UINT_MAX;
  for (unsigned int channel = 0; channel < num_channels; ++channel)
  {
    unsigned int len = getVideoChannel(channel)->mVideoCapture.get(CV_CAP_PROP_FRAME_COUNT);
    if (len < smallest)
    {
      smallest = len;
    }
  }
  mFramesCount = smallest;

  // check for equal FPS
  double fps_ch0 = getVideoChannel(0)->mVideoCapture.get(CV_CAP_PROP_FPS);

  if (fps_ch0 < 1)
  {
    fps_ch0 = 1;
  }

  if (num_channels > 1)
  {
    double fps_ch1 = getVideoChannel(1)->mVideoCapture.get(CV_CAP_PROP_FPS);
    if (fps_ch0 != fps_ch1)
    {
      std::string msg =  this->getName() + ": Different framerates of channels 0 and 1";
      CEDAR_THROW(cedar::dev::sensors::visual::CreateGrabberException,msg)
    }
  }

  // set stepsize for LoopedThread to the framerate from the video-files
  setFramerate(fps_ch0 * _mSpeedFactor->getValue());
}


void cedar::dev::sensors::visual::VideoGrabber::onCloseGrabber()
{
  unsigned int num_channels = getNumChannels();
  for(unsigned int channel = 0; channel < num_channels; ++channel)
  {
    // there is no cv::VideoCapture.close() method, so we assign an empty one. The old will be released automatically
    getVideoChannel(channel)->mVideoCapture = cv::VideoCapture();
  }
}


void cedar::dev::sensors::visual::VideoGrabber::onGrab(unsigned int channel)
{

  // rewinding is done very inefficient, because for the moment it is not possible to break the grabbing loop in
  // the base grabber-class.
  //! @todo Rewrite the video-grabbing and the rewinding more efficient

  // read next frame from file for this channel
  (getVideoChannel(channel)->mVideoCapture) >> getImageMat(channel);

  // check if the grabbed frame is empty
  if (getImageMat(channel).empty())
  {
    // is it a read error somewhere in the middle of the file?
    if (getVideoChannel(channel)->mVideoCapture.get(CV_CAP_PROP_POS_FRAMES) < (mFramesCount))
    {
      std::string msg = "Could not read from video file on channel " + boost::lexical_cast<std::string>(channel);
      CEDAR_THROW(cedar::dev::sensors::visual::GrabberGrabException,msg)
    }

    // otherwise end of file. Rewind if looped is on
    if (_mLooped->getValue())
    {
      // rewind all already grabbed channels and grab first frame
      for (unsigned int i = 0; i <= channel; ++i)
      {
        getVideoChannel(i)->mVideoCapture.set(CV_CAP_PROP_POS_FRAMES,0);
        getVideoChannel(i)->mVideoCapture >> getImageMat(i);
      }

      // set all other channels to their last frame
      for (unsigned int i = channel+1; i < getNumChannels(); ++i)
      {
        double last_frame = getVideoChannel(i)->mVideoCapture.get(CV_CAP_PROP_FRAME_COUNT)-1;
        getVideoChannel(i)->mVideoCapture.set(CV_CAP_PROP_POS_FRAMES,last_frame);
        getVideoChannel(i)->mVideoCapture >> getImageMat(i);
      }

      // debug message for rewinding (if last channel)
      if (channel == getNumChannels()-1)
      {
        cedar::aux::LogSingleton::getInstance()->debugMessage
                                                (
                                                  this->getName() + ": Video restarted",
                                                  "cedar::dev::sensors::visual::VideoGrabber::onGrab()"
                                                );
      }
    }
  }
}


std::string cedar::dev::sensors::visual::VideoGrabber::onGetSourceInfo(unsigned int channel)
{
  return this->getName() + " - channel " + boost::lexical_cast<std::string>(channel)
                         + ": " + getVideoChannel(channel)->_mSourceFileName->getPath();
}


void cedar::dev::sensors::visual::VideoGrabber::setSpeedFactor(double speedFactor)
{
  _mSpeedFactor->setValue(speedFactor);
}


double cedar::dev::sensors::visual::VideoGrabber::getSpeedFactor() const
{
  return _mSpeedFactor->getValue();
}


void cedar::dev::sensors::visual::VideoGrabber::setLooped(bool loop)
{
  _mLooped->setValue(loop);
}


bool cedar::dev::sensors::visual::VideoGrabber::getLooped() const
{
  return _mLooped->getValue();
}


void cedar::dev::sensors::visual::VideoGrabber::setPositionRelative(double newPositionRel)
{
  // calculate relative position which depends on the shortest file.

  // check range
  if (newPositionRel < 0 || newPositionRel > 1)
  {
    CEDAR_THROW(cedar::aux::IndexOutOfRangeException,"VideoGrabber::setPositionRelative");
  }

  double max_pos = static_cast<double>(mFramesCount - 1);

  // newPositionRel = [0..1]
  double new_pos_abs = max_pos * newPositionRel;

  // the maximum position should be 5 frames offset from the end
  int max_wanted_pos = max_pos-5;
  if (new_pos_abs > max_wanted_pos)
  {
    new_pos_abs = max_wanted_pos;
  }

  // test if not negative (for example for very short video-files)
  if (new_pos_abs < 0)
  {
    new_pos_abs = 0;
  }

  // create debug-message
  std::stringstream debug_message;
  debug_message << std::endl <<"\tNew position wanted (relative 0..1): " << newPositionRel << std::endl;
  debug_message << "\tPosition set to frame: " << new_pos_abs << std::endl;
  cedar::aux::LogSingleton::getInstance()->debugMessage
                                            (
                                              this->getName() + ": " + debug_message.str(),
                                              "cedar::dev::sensors::visual::VideoGrabber::setPositionRelative()"
                                            );

  // set to new values
  unsigned int num_channels = getNumChannels();
  for(unsigned int channel = 0; channel < num_channels; ++channel)
  {
    getVideoChannel(channel)->mVideoCapture.set(CV_CAP_PROP_POS_FRAMES,new_pos_abs);
  }
}


double cedar::dev::sensors::visual::VideoGrabber::getPositionRelative()
{
  // the shortest file defines the length of the avi
  // it is possible, that the first file isn't the shortest
  // so we use absolute positions instead of relative
  // return  getVideoChannel(0)->mVideoCapture.get(CV_CAP_PROP_POS_AVI_RATIO); // from 0..1;

  double pos_abs = static_cast<double>(this->getPositionAbsolute());
  double count   = static_cast<double>(mFramesCount);
  return pos_abs / count;
}


void cedar::dev::sensors::visual::VideoGrabber::setPositionAbsolute(unsigned int newPositionAbs)
{
  // newPos can't be smaller than zero, because it is an unsigned int. Only check upper border
  if (newPositionAbs + 1 > mFramesCount)
  {
    CEDAR_THROW(cedar::aux::IndexOutOfRangeException,"VideoGrabber::setPositionAbsolute");
  }

  // create debug-messages
  std::stringstream debug_message;
  debug_message << std::endl <<"\tCurrent position: " << getPositionAbsolute() << std::endl
                << "\tPosition set to frame: " << newPositionAbs << std::endl;
  cedar::aux::LogSingleton::getInstance()->debugMessage
                                            (
                                              this->getName() + ": " + debug_message.str(),
                                              "cedar::dev::sensors::visual::VideoGrabber::setPositionAbsolute()"
                                            );

  // set new position
  unsigned int num_channels = getNumChannels();
  for(unsigned int channel = 0; channel < num_channels; ++channel)
  {
    (getVideoChannel(channel)->mVideoCapture).set(CV_CAP_PROP_POS_FRAMES,newPositionAbs);
  }
}


unsigned int cedar::dev::sensors::visual::VideoGrabber::getPositionAbsolute()
{
  // the position in all avi's should be the same
  return getVideoChannel(0)->mVideoCapture.get(CV_CAP_PROP_POS_FRAMES);
}


unsigned int cedar::dev::sensors::visual::VideoGrabber::getFrameCount() const
{
  return mFramesCount;
}


double cedar::dev::sensors::visual::VideoGrabber::getSourceProperty(unsigned int channel,int propId)
{
  if (channel >= getNumChannels())
  {
    CEDAR_THROW(cedar::aux::IndexOutOfRangeException,"VideoGrabber::getSourceProperty");
  }
  return getVideoChannel(channel)->mVideoCapture.get(propId);
}


double cedar::dev::sensors::visual::VideoGrabber::getSourceFramerate(unsigned int channel)
{
  return getSourceProperty(channel,CV_CAP_PROP_FPS);
}


double cedar::dev::sensors::visual::VideoGrabber::getSourceEncoding(unsigned int channel)
{
  return getSourceProperty(channel,CV_CAP_PROP_FOURCC);
}


const std::string cedar::dev::sensors::visual::VideoGrabber::getSourceFile(unsigned int channel) const
{
  if (channel >= getNumChannels())
  {
    CEDAR_THROW(cedar::aux::IndexOutOfRangeException,"VideoGrabber::setSourceFile");
  }

  return getVideoChannel(channel)->_mSourceFileName->getPath();
}
