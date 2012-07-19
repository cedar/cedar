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

    File:        Grabber.cpp

    Maintainer:  Georg.Hartinger
    Email:       georg.hartinger@ini.rub.de
    Date:        2011 08 01

    Description: Implementation of the @em cedar::Grabber class.

    Credits:

======================================================================================================================*/

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/devices/sensors/visual/Grabber.h"

// SYSTEM INCLUDES
#include <signal.h>

//----------------------------------------------------------------------------------------------------------------------
//constants and static variables
//----------------------------------------------------------------------------------------------------------------------

//!@todo check if needed
const std::string cedar::dev::sensors::visual::Grabber::mGrabberDefaultRecordExtension =  ".avi";
const std::string cedar::dev::sensors::visual::Grabber::mGrabberDefaultSnapshotExtension = ".jpg";

//Initialize the static member vector containing all instances
cedar::dev::sensors::visual::Grabber::GrabberInstancesVector cedar::dev::sensors::visual::Grabber::mInstances;
bool cedar::dev::sensors::visual::Grabber::mRegisterTerminationHandler;


//----------------------------------------------------------------------------------------------------------------------
// register the class
//----------------------------------------------------------------------------------------------------------------------
namespace
{
  bool declared
    = cedar::dev::sensors::visual::Grabber::ChannelManagerSingleton::getInstance()
        ->registerType<cedar::dev::sensors::visual::Grabber::ChannelPtr>();
}


//----------------------------------------------------------------------------------------------------------------------
//constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------------------------------------
cedar::dev::sensors::visual::Grabber::Grabber(const std::string& grabberName, ChannelPtr pChannel)
:
cedar::aux::LoopedThread()
{
  cedar::aux::LogSingleton::getInstance()->allocating(this);
  this->setName(grabberName);

  //instantiate the default grabber with one channel with the right derived class
  std::vector<cedar::dev::sensors::visual::Grabber::ChannelPtr> grabberchannels;
  grabberchannels.push_back(pChannel);

  this->init(grabberchannels);
}

//--------------------------------------------------------------------------------------------------------------------
cedar::dev::sensors::visual::Grabber::Grabber
(
  const std::string& grabberName,
  ChannelPtr pChannel0,
  ChannelPtr pChannel1
)
:
cedar::aux::LoopedThread()
{
  cedar::aux::LogSingleton::getInstance()->allocating(this);
  this->setName(grabberName);

  //instantiate the default grabber with two channels of the derived grabber
  std::vector<cedar::dev::sensors::visual::Grabber::ChannelPtr> grabberchannels;
  grabberchannels.push_back(pChannel0);
  grabberchannels.push_back(pChannel1);

  this->init(grabberchannels);
}


//--------------------------------------------------------------------------------------------------------------------
cedar::dev::sensors::visual::Grabber::~Grabber()
{
  //remove this grabber-instance from the InstancesVector
  std::vector<Grabber *>::iterator it = mInstances.begin();

  while (((*it) != this) && (it != mInstances.end()))
  {
    ++it;
  }

  CEDAR_DEBUG_ASSERT(it != mInstances.end());

  if ((*it) == this)
  {
    cedar::aux::LogSingleton::getInstance()->debugMessage
                                            (
                                              this->getName() + ": Deleted this grabber from list of all instances.",
                                              "cedar::dev::sensors::visual::Grabber::~Grabber()"
                                            );
    mInstances.erase(it);
  }
  cedar::aux::LogSingleton::getInstance()->freeing(this);
}

//----------------------------------------------------------------------------------------------------------------------
//methods for signal handling and cleanup
//----------------------------------------------------------------------------------------------------------------------

//this function handles abnormal termination of the program (like the signals interrupt, abort or seg.fault)
void cedar::dev::sensors::visual::Grabber::interruptSignalHandler(int)
{
  emergencyCleanup();
  std::exit(1);
}

//--------------------------------------------------------------------------------------------------------------------
void cedar::dev::sensors::visual::Grabber::emergencyCleanup()
{
  cedar::aux::LogSingleton::getInstance()->message
                                           (
                                             "Emergency Cleanup",
                                             "cedar::dev::sensors::visual::Grabber::emergencyCleanup()"
                                           );

  for (std::vector<Grabber*>::iterator it = mInstances.begin(); it != mInstances.end(); ++it)
  {
    //only cedar::dev::sensors::visual::Grabber::doCleanUp() and xxxGrabber::onCleanUp() methods invoked
    //Note: no destructor involved here
    (*it)->doCleanUp();
  }
}

//--------------------------------------------------------------------------------------------------------------------
void cedar::dev::sensors::visual::Grabber::installCrashHandler()
{
   signal(SIGINT,&Grabber::interruptSignalHandler);
   signal(SIGABRT,&Grabber::interruptSignalHandler);
   signal(SIGSEGV,&Grabber::interruptSignalHandler);

   cedar::aux::LogSingleton::getInstance()->message
                                            (
                                              "Crash handler installed",
                                              "cedar::dev::sensors::visual::Grabber::installCrashHandler()"
                                            );
}


//--------------------------------------------------------------------------------------------------------------------
void cedar::dev::sensors::visual::Grabber::doCleanUp()
{
  if (! mCleanUpAlreadyDone)
  {

    cedar::aux::LogSingleton::getInstance()->debugMessage
                                             (
                                              this->getName() + ": cleanup",
                                               "cedar::dev::sensors::visual::Grabber::doCleanUp()"
                                             );

    mCleanUpAlreadyDone = true;

    //stopGrabber LoopedThread
    if (QThread::isRunning())
    {
      cedar::aux::LoopedThread::stop();
      this->wait();
    }

    //stopGrabber Recording
    stopRecording();

    //do the cleanup in derived class
    onCleanUp();

    unsigned int num_cams = getNumCams();
    for (unsigned int channel = 0; channel < num_cams; channel++)
    {
      // call delete to each used pointer
      // getChannel(channel)->cleanup();
    }



    if (mpReadWriteLock)
    {
      delete mpReadWriteLock;
      mpReadWriteLock = NULL;
    }
  }
}

//--------------------------------------------------------------------------------------------------------------------
// configurable interface
//--------------------------------------------------------------------------------------------------------------------

/*
void cedar::dev::sensors::visual::Grabber::readConfiguration(const cedar::aux::ConfigurationNode& node)
{
cedar::aux::NamedConfigurable

  //do readConfiguration for all childs. This is implemented in the base class
  cedar::aux::NamedConfigurable::readConfiguration(node);

}
*/

//--------------------------------------------------------------------------------------------------------------------
// class methods
//--------------------------------------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------------------------------------
void cedar::dev::sensors::visual::Grabber::init
(
  std::vector<cedar::dev::sensors::visual::Grabber::ChannelPtr> grabberChannels
)
{
  //initialize local member
  mpReadWriteLock = new QReadWriteLock();
  mGrabberThreadStartedOnRecording = false;
  mCleanUpAlreadyDone = false;
  mFpsMeasureStart = boost::posix_time::microsec_clock::local_time();
  mFpsMeasureStop = boost::posix_time::microsec_clock::local_time();
  mFpsCounter = 0;
  mFpsMeasured = 0;
  mCaptureDeviceCreated = false;
  mRecording = false;


  //insert this instance to our instance-vector (used for emergency-cleanup)
  mInstances.push_back(this);

  //add the channels to the configurable interface
  _mChannels = cedar::dev::sensors::visual::Grabber::ChannelParameterPtr
               (
                  new cedar::dev::sensors::visual::Grabber::ChannelParameter
                  (
                    this,
                    "channels",
                    grabberChannels
                  )
               );
}


//--------------------------------------------------------------------------------------------------------------------
void cedar::dev::sensors::visual::Grabber::onCleanUp()
{
}

//--------------------------------------------------------------------------------------------------------------------
bool cedar::dev::sensors::visual::Grabber::onGrab()
{
  return true;
}

//--------------------------------------------------------------------------------------------------------------------
bool cedar::dev::sensors::visual::Grabber::applyParameter()
{
  //check on first channel if there is already channel-information there.
  //this is an indicator, that the channel is open and active
  //if so, close all grabber
  if (mCaptureDeviceCreated)
  {
    this->closeGrabber();
  }

  //create the channels in the derived classes
  if (this->onCreateGrabber())
  {
    mCaptureDeviceCreated = true;
  }
  else
  {
    const std::string message = ": Couldn't apply Parameter! Change Parameter and try again.";
    cedar::aux::LogSingleton::getInstance()->warning
                                             (
                                               this->getName() + message,
                                               "cedar::dev::sensors::visual::Grabber::applyParameter()"
                                             );
    this->closeGrabber();
  }
  return mCaptureDeviceCreated;
}

//--------------------------------------------------------------------------------------------------------------------
bool cedar::dev::sensors::visual::Grabber::isCreated()
{
  return mCaptureDeviceCreated;
}


//--------------------------------------------------------------------------------------------------------------------
void cedar::dev::sensors::visual::Grabber::closeGrabber()
{
  if (this->isRunning())
  {
    this->stopGrabber();
  }

  //close the grabber-things in the derived class
  this->onCloseGrabber();

  //reset the common-things from all grabbers
  unsigned int num_cams = _mChannels->size();
  for (unsigned int channel=0; channel < num_cams; channel++)
  {
    this->getChannel(channel)->mChannelInfo="";
    this->getChannel(channel)->mImageMat = cv::Mat();
  }
  mCaptureDeviceCreated = false;
  cedar::aux::LogSingleton::getInstance()->debugMessage
                                           (
                                             this->getName() + ": Close Capture channels",
                                             "cedar::dev::sensors::visual::Grabber::closeGrabber()"
                                           );
}

//--------------------------------------------------------------------------------------------------------------------
unsigned int cedar::dev::sensors::visual::Grabber::getNumCams() const
{
  return _mChannels->size();
}

//--------------------------------------------------------------------------------------------------------------------
double cedar::dev::sensors::visual::Grabber::getFpsMeasured() const
{
  return mFpsMeasured;
}

//--------------------------------------------------------------------------------------------------------------------
double cedar::dev::sensors::visual::Grabber::getFps() const
{
  double fps = 1000. / LoopedThread::getStepSize();
  return fps;
}


//--------------------------------------------------------------------------------------------------------------------
void cedar::dev::sensors::visual::Grabber::setFps(double fps)
{
  bool wasRunning = QThread::isRunning();

  if (wasRunning)
  {
    stopGrabber();

    cedar::aux::LogSingleton::getInstance()->debugMessage
                                             (
                                               this->getName() + ": Grabbing stopped",
                                               "cedar::dev::sensors::visual::Grabber::setFps()"
                                             );
  }

  cedar::aux::LogSingleton::getInstance()->debugMessage
                                           (
                                             this->getName()
                                               + ": Switch to " + boost::lexical_cast<std::string>(fps) + " fps",
                                             "cedar::dev::sensors::visual::Grabber::setFps()"
                                           );

  //cycle time in ms: 1000ms/frames_per_second
  double milliseconds = 1000. / fps;

  //LoopedThread::_mStepSize = milliseconds;  //set param for configuration file (bug in LoopedThread)
  LoopedThread::setStepSize(milliseconds);        //change speed in thread

  if (wasRunning)
  {
    startGrabber();
    cedar::aux::LogSingleton::getInstance()->debugMessage
                                             (
                                               this->getName() + ": Grabbing started",
                                               "cedar::dev::sensors::visual::Grabber::setFps()"
                                             );
  }

  std::string info;
  if (QThread::isRunning())
  {
    info =  ": Thread running";
  }
  else
  {
    info =  ": Thread not running";
  }
  cedar::aux::LogSingleton::getInstance()->debugMessage
                                           (
                                             this->getName() + info,
                                             "cedar::dev::sensors::visual::Grabber::setFps()"
                                           );
}

//--------------------------------------------------------------------------------------------------------------------
void cedar::dev::sensors::visual::Grabber::stopGrabber()
{
  if (this->isRunning())
  {
    this->LoopedThread::stop();
    cedar::aux::LogSingleton::getInstance()->debugMessage
                                             (
                                               this->getName() + ": Grabbing stopped",
                                               "cedar::dev::sensors::visual::Grabber::stopGrabber()"
                                             );
    mFpsMeasured = 0;
  }
  if (mRecording)
  {
    this->stopRecording();
  }
}

//--------------------------------------------------------------------------------------------------------------------
void cedar::dev::sensors::visual::Grabber::startGrabber()
{
  mFpsMeasured = 0;
  mFpsCounter = 0;
  mFpsMeasureStart = boost::posix_time::microsec_clock::local_time();
  LoopedThread::start();
  cedar::aux::LogSingleton::getInstance()->debugMessage
                                           (
                                             this->getName() + ": Grabbing started with "
                                               + boost::lexical_cast<std::string>(getFps()) + " FPS.",
                                             "cedar::dev::sensors::visual::Grabber::startGrabber()"
                                           );
}

//--------------------------------------------------------------------------------------------------------------------
void cedar::dev::sensors::visual::Grabber::grab()
{
  bool result = true;
  std::string error_info;

  mpReadWriteLock->lockForWrite();        //Lock
  try
  {
    result = onGrab();                    //Grab
  }
  catch (std::exception& e)
  {
    error_info = e.what();
    result = false;
  }
  mpReadWriteLock->unlock();              //Unlock

  if (! result)
  {
    cedar::aux::LogSingleton::getInstance()->error
                                             (
                                               this->getName() + ": Grabbed image is empty! The grabber instance will"
                                               + "be cleaned up, so don't use this instance anymore!",
                                               "cedar::dev::sensors::visual::Grabber::grab()"
                                             );
    doCleanUp();
    CEDAR_THROW
    (
      cedar::dev::sensors::visual::GrabberGrabException,
      "[Grabber::grab] Error in grabber \""+ this->getName() + "\" on grabbing: " + error_info
    );
  }

  //grabbing ok, then
  //calculate fps every 5 (default) grabbing frames
  if (! (++mFpsCounter %= UPDATE_FPS_MEASURE_FRAME_COUNT))
  {
    mFpsMeasureStop = boost::posix_time::microsec_clock::local_time();

    //calculate real diff:
    boost::posix_time::time_duration measure_duration = mFpsMeasureStop - mFpsMeasureStart;
    mFpsMeasured = double(UPDATE_FPS_MEASURE_FRAME_COUNT * 1000000. / measure_duration.total_microseconds());

    mFpsMeasureStart = boost::posix_time::microsec_clock::local_time();
  }

  //check if recording is on
  if (mRecording)
  {
    unsigned int num_cams = getNumCams();
    for(unsigned int channel = 0; channel < num_cams; ++channel)
    {
      try
      {
        getChannel(channel)->mVideoWriter << getChannel(channel)->mImageMat;
      }
      catch (std::exception& e)
      {
        std::stringstream info;
        info << "[Grabber::grab] Error recording channel" << channel << ": " << e.what();
        cedar::aux::LogSingleton::getInstance()->error
                                                 (
                                                   this->getName() + ": info ",
                                                   "cedar::dev::sensors::visual::Grabber::grab()"
                                                 );
        doCleanUp();
        CEDAR_THROW(cedar::dev::sensors::visual::GrabberRecordingException,info.str());
      }
    }
  }
}

//--------------------------------------------------------------------------------------------------------------------
cv::Mat cedar::dev::sensors::visual::Grabber::getImage(unsigned int channel) const
{
  if (channel >= getNumCams())
  {
    CEDAR_THROW(cedar::aux::IndexOutOfRangeException,"Grabber::getImage");
  }
  return getChannel(channel)->mImageMat;
}

//--------------------------------------------------------------------------------------------------------------------
QReadWriteLock* cedar::dev::sensors::visual::Grabber::getReadWriteLockPointer() const
{
  return mpReadWriteLock;
}

//--------------------------------------------------------------------------------------------------------------------
std::string& cedar::dev::sensors::visual::Grabber::getSourceInfo(unsigned int channel)
{
  if (channel >= getNumCams())
  {
    CEDAR_THROW(cedar::aux::IndexOutOfRangeException,"Grabber::getSourceInfo");
  }
  //onUpdateSourceInfo(channel);
  return getChannel(channel)->mChannelInfo;
}

//--------------------------------------------------------------------------------------------------------------------
void cedar::dev::sensors::visual::Grabber::setSnapshotName(const std::string& snapshotName)
{
  if (snapshotName == "")
  {
    return;
  }

  //initialize snapshot-names
  std::string name = snapshotName;
  std::string ext  = "";

  //check if extension in filename
  std::size_t pos = name.rfind(".");

  if (pos != std::string::npos)
  {
    //yes: extract it
    int cnt = pos;
    ext  = name.substr(pos);
    name = name.substr(0,cnt);
  }
  else
  {
    //no: use default
    ext  = mGrabberDefaultSnapshotExtension;
    name = snapshotName;
  }

  unsigned int num_cams = getNumCams();
  if (num_cams == 1)
  {
    getChannel(0)->_mSnapshotName->setValue(name + ext);
  }
  else
  {
    for(unsigned int channel = 0; channel < num_cams; ++channel)
    {
      getChannel(channel)->_mSnapshotName->setValue(name + this->getChannelSaveFilenameAddition(channel) + ext);
    }
  }
}

//--------------------------------------------------------------------------------------------------------------------
void cedar::dev::sensors::visual::Grabber::setSnapshotName
(
  unsigned int channel,
  const std::string& snapshotName
)
{
  if (channel >= getNumCams())
  {
    CEDAR_THROW(cedar::aux::IndexOutOfRangeException,"Grabber::setSnapshotName");
  }

  if (snapshotName != "")
  {
    //check if there is an extension
    std::string name = snapshotName;
    std::size_t pos = name.rfind(".");

    if (pos == std::string::npos)
    {
      //no: use default extension
      name.append(mGrabberDefaultSnapshotExtension);
    }
    getChannel(channel)->_mSnapshotName->setValue(name);
  }
}

//--------------------------------------------------------------------------------------------------------------------
std::string cedar::dev::sensors::visual::Grabber::getSnapshotName(unsigned int channel) const
{
  if (channel >= getNumCams())
  {
    CEDAR_THROW(cedar::aux::IndexOutOfRangeException,"Grabber::getSnapshotName");
  }
  return getChannel(channel)->_mSnapshotName->getPath();
}

//--------------------------------------------------------------------------------------------------------------------
void cedar::dev::sensors::visual::Grabber::saveSnapshot(unsigned int channel) const
{
  if (channel >= getNumCams())
  {
    CEDAR_THROW(cedar::aux::IndexOutOfRangeException,"Grabber::saveSnapshot");
  }

  //copy image to local buffer for slow imwrite-function and for error-checking
  cv::Mat imgBuffer;

  if (!getChannel(channel)->mImageMat.empty())
  {
    mpReadWriteLock->lockForRead();
    getChannel(channel)->mImageMat.copyTo(imgBuffer);
    mpReadWriteLock->unlock();

    try
    {
      cv::imwrite(getChannel(channel)->_mSnapshotName->getPath(), imgBuffer);
    }
    catch (std::exception &e)
    {
      std::stringstream info;
      info << "[Grabber::saveSnapshot] Channel " << channel << ": " << e.what();
      CEDAR_THROW(cedar::dev::sensors::visual::GrabberSnapshotException,info.str());
    }
  }
  else
  {
    std::string info = "[Grabber::saveSnapshot] Channel "
                       + boost::lexical_cast<std::string>(channel)
                       +": Image matrix is empty!";
    CEDAR_THROW(cedar::dev::sensors::visual::GrabberSnapshotException,info);
  }
}

//--------------------------------------------------------------------------------------------------------------------
void cedar::dev::sensors::visual::Grabber::saveSnapshotAllCams() const
{
  unsigned int num_cams = getNumCams();
  for(unsigned int channel = 0; channel < num_cams; ++channel)
  {
    saveSnapshot(channel);
  }
}

//--------------------------------------------------------------------------------------------------------------------
cv::Size cedar::dev::sensors::visual::Grabber::getSize(unsigned int channel) const
{
  if (channel >= getNumCams())
  {
    CEDAR_THROW(cedar::aux::IndexOutOfRangeException,"Grabber::getSize");
  }
  return getChannel(channel)->mImageMat.size();
}

//--------------------------------------------------------------------------------------------------------------------
void cedar::dev::sensors::visual::Grabber::setRecordName(const std::string& recordName)
{
  if (recordName == "")
  {
    return;
  }

  //initialize snapshot-names
  std::string name = recordName;
  std::string ext  = "";

  //check if extension in filename
  std::size_t pos = name.rfind(".");

  if (pos != std::string::npos)
  {
    //yes: extract it
    ext  = name.substr(pos);
    name = name.substr(0,pos);
  }
  else
  {
    //no: use default
    ext  = mGrabberDefaultRecordExtension;
    name = recordName;
  }

  //filename depends on no. of cams
  unsigned int num_cams = getNumCams();
  if (num_cams == 1)
  {
    getChannel(0)->_mRecordName->setValue(name + ext);
  }
  else
  {
    for(unsigned int channel = 0; channel < num_cams; ++channel)
    {
      getChannel(channel)->_mRecordName->setValue(name + this->getChannelSaveFilenameAddition(channel) + ext);
    }
  }
}

//--------------------------------------------------------------------------------------------------------------------
void cedar::dev::sensors::visual::Grabber::setRecordName(unsigned int channel, const std::string& recordName )
{
  if (channel >= getNumCams())
  {
    CEDAR_THROW(cedar::aux::IndexOutOfRangeException,"Grabber::setRecordName");
  }

  if (recordName != "")
  {
    //check if there is an extension
    std::string name = recordName;
    std::size_t pos = name.rfind(".");

    if (pos == std::string::npos)
    {
      //no: use default extension
      name.append(mGrabberDefaultRecordExtension);
    }
    getChannel(channel)->_mRecordName->setValue(name);
  }
}

//--------------------------------------------------------------------------------------------------------------------
const std::string cedar::dev::sensors::visual::Grabber::getRecordName(unsigned int channel) const
{
  if (channel >= getNumCams())
  {
    CEDAR_THROW(cedar::aux::IndexOutOfRangeException,"Grabber::getRecordName");
  }
  return getChannel(channel)->_mRecordName->getPath();
}

//--------------------------------------------------------------------------------------------------------------------
bool cedar::dev::sensors::visual::Grabber::startRecording
(
  double fps,
  cedar::dev::sensors::visual::RecordingFormat::Id recFormat,
  bool color,
  bool startThread
)
{
  if (mRecording)
  {
    cedar::aux::LogSingleton::getInstance()->warning
                                             (
                                               this->getName() + ": Grabber is already recording",
                                               "cedar::dev::sensors::visual::Grabber::startRecording()"
                                             );
    return true;
  }

  //set the record-flag
  mRecording = true;
  unsigned int recording_channels = 0;

  //write the video-file with the actual grabbing-speed
  //this is independent from the speed of the avi-file or the camera framerate

  unsigned int num_cams = getNumCams();
  for(unsigned int channel = 0; channel < num_cams; ++channel)
  {
    //create writer
    std::string record_name = getChannel(channel)->_mRecordName->getPath();
    cv::VideoWriter writer
                    (
                      record_name,
                      static_cast<unsigned int>(recFormat),
                      fps,
                      getChannel(channel)->mImageMat.size(),
                      color
                    );

    if (writer.isOpened())
    {
      //save it in channel-struct
      getChannel(channel)->mVideoWriter = writer;

      std::string info = ": Channel " + boost::lexical_cast<std::string>(channel) + " recording to " + record_name;

      cedar::aux::LogSingleton::getInstance()->message
                                               (
                                                 this->getName() + info,
                                                 "cedar::dev::sensors::visual::Grabber::startRecording()"
                                               );
      recording_channels++;
    }
  }

  if (recording_channels != num_cams)
  {
    CEDAR_THROW
      (
        cedar::dev::sensors::visual::GrabberRecordingException,
        "Start recording: only " + boost::lexical_cast<std::string>(recording_channels)
          + " of " + boost::lexical_cast<std::string>(num_cams) + " recording!"
      )
  }

  //startGrabber the grabberthread if needed
  if (!QThread::isRunning() && startThread)
  {
    cedar::aux::LogSingleton::getInstance()->message
                                             (
                                               this->getName() + ": Grabbing started on recording",
                                               "cedar::dev::sensors::visual::Grabber::startRecording()"
                                             );
    mGrabberThreadStartedOnRecording = true;
    startGrabber();
  }
  return true;
}

//--------------------------------------------------------------------------------------------------------------------
void cedar::dev::sensors::visual::Grabber::stopRecording()
{
  if (mRecording)
  {
    mRecording = false;
    if (mGrabberThreadStartedOnRecording)
    {
      stopGrabber();
      mGrabberThreadStartedOnRecording = false;
      cedar::aux::LogSingleton::getInstance()->debugMessage
                                               (
                                                 this->getName() + ": Recording stopped",
                                                 "cedar::dev::sensors::visual::Grabber::stopRecording()"
                                               );
    }

    //delete the videowriter
    unsigned int num_cams = getNumCams();
    for(unsigned int channel = 0; channel < num_cams; ++channel)
    {
      getChannel(channel)->mVideoWriter = cv::VideoWriter();
    }
  }
}

//--------------------------------------------------------------------------------------------------------------------
bool cedar::dev::sensors::visual::Grabber::isRecording() const
{
  return mRecording;
}

//--------------------------------------------------------------------------------------------------------------------
void cedar::dev::sensors::visual::Grabber::step(double)
{
  //if something went wrong on grabbing,
  //an exception is being thrown in the grab function
  //right after a cleanup
  grab();
}

//--------------------------------------------------------------------------------------------------------------------
std::string cedar::dev::sensors::visual::Grabber::getChannelSaveFilenameAddition(int channel) const
{
  return "_ch["+boost::lexical_cast<std::string>(channel)+"]";
}

