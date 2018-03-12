/*======================================================================================================================

    Copyright 2011, 2012, 2013, 2014, 2015, 2016, 2017 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany

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
    along with cedar. If not, see <http:// www.gnu.org/licenses/>.

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
#include "cedar/devices/sensors/visual/exceptions.h"
#include "cedar/auxiliaries/sleepFunctions.h"

// SYSTEM INCLUDES
#include <signal.h>
#include <exception>
#include <sstream>
#include <iostream>


//----------------------------------------------------------------------------------------------------------------------
// constants and static variables
//----------------------------------------------------------------------------------------------------------------------

// constants for the default file endings
const std::string cedar::dev::sensors::visual::Grabber::mGrabberDefaultRecordExtension =  ".avi";
const std::string cedar::dev::sensors::visual::Grabber::mGrabberDefaultSnapshotExtension = ".jpg";

// Initialize the static member vector containing all instances
cedar::dev::sensors::visual::Grabber::GrabberInstancesVector cedar::dev::sensors::visual::Grabber::mInstances;
bool cedar::dev::sensors::visual::Grabber::mRegisterTerminationHandler;


//----------------------------------------------------------------------------------------------------------------------
// register the class
//----------------------------------------------------------------------------------------------------------------------
namespace
{
  bool declared
    = cedar::dev::sensors::visual::Grabber::ChannelManagerSingleton::getInstance()
        ->registerType<cedar::dev::sensors::visual::GrabberChannelPtr>();
}


//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::dev::sensors::visual::Grabber::Grabber(const std::string& grabberName, GrabberChannelPtr pChannel)
:
cedar::aux::LoopedThread()
{
  // cedar logging
  cedar::aux::LogSingleton::getInstance()->allocating(this);

  // instantiate the default grabber with one channel with the right derived class
  this->setName(grabberName);
  std::vector<cedar::dev::sensors::visual::GrabberChannelPtr> grabberchannels;
  grabberchannels.push_back(pChannel);
  this->init(grabberchannels);

  this->connectToStartSignal(boost::bind(&cedar::dev::sensors::visual::Grabber::prepareStart, this));
  this->connectToQuitSignal(boost::bind(&cedar::dev::sensors::visual::Grabber::processQuit, this));
}


cedar::dev::sensors::visual::Grabber::Grabber
(
  const std::string& grabberName,
  GrabberChannelPtr pChannel0,
  GrabberChannelPtr pChannel1
)
:
cedar::aux::LoopedThread()
{
  // cedar logging
  cedar::aux::LogSingleton::getInstance()->allocating(this);

  // instantiate the default grabber with two channels of the derived grabber
  this->setName(grabberName);
  std::vector<cedar::dev::sensors::visual::GrabberChannelPtr> grabberchannels;
  grabberchannels.push_back(pChannel0);
  grabberchannels.push_back(pChannel1);
  this->init(grabberchannels);
}


cedar::dev::sensors::visual::Grabber::~Grabber()
{
  // freeing the readwrite-locks
  if (mpReadWriteLock)
  {
    delete mpReadWriteLock;
    mpReadWriteLock = NULL;
  }

  if (mpLockCaptureDeviceCreated)
  {
    delete mpLockCaptureDeviceCreated;
    mpLockCaptureDeviceCreated = NULL;
  }

  if (mpLockIsGrabbing)
  {
    delete mpLockIsGrabbing;
    mpLockIsGrabbing = NULL;
  }

  if (mpLockIsCreating)
  {
    delete mpLockIsCreating;
    mpLockIsCreating = NULL;
  }

  // remove this grabber-instance from the InstancesVector
  std::vector<Grabber *>::iterator it = mInstances.begin();

  while (((*it) != this) && (it != mInstances.end()))
  {
    ++it;
  }

  CEDAR_DEBUG_ASSERT(it != mInstances.end());

  if ((*it) == this)
  {
    mInstances.erase(it);
  }
  cedar::aux::LogSingleton::getInstance()->freeing(this);
}

//----------------------------------------------------------------------------------------------------------------------
// methods for signal handling and cleanup
//----------------------------------------------------------------------------------------------------------------------

// this function handles abnormal termination of the program (like the signals interrupt, abort or seg.fault)
void cedar::dev::sensors::visual::Grabber::interruptSignalHandler(int signal)
{
  std::string sig_name = "";
  switch (signal)
  {
    case SIGINT: sig_name = "SIGINT"; break;
    case SIGABRT: sig_name = "SIGABRT"; break;
    case SIGSEGV: sig_name = "SIGSEGV"; break;
    default:
      sig_name = "UNKNOWN";
      break;
  }
  
  // write message, do hardwarecleanup and exit
  std::cout << "Signal " << sig_name << " (" << signal << ") caught! Terminate program!" << std::endl;
  emergencyCleanup();
  std::exit(1);
}


void cedar::dev::sensors::visual::Grabber::emergencyCleanup()
{
  cedar::aux::LogSingleton::getInstance()->message
                                           (
                                             "Emergency Cleanup",
                                             "cedar::dev::sensors::visual::Grabber::emergencyCleanup()"
                                           );

  //cleanup every instantiated grabber
  for (std::vector<Grabber*>::iterator it = mInstances.begin(); it != mInstances.end(); ++it)
  {
    // only cedar::dev::sensors::visual::Grabber::doCleanUp() and xxxGrabber::onCleanUp() methods invoked
    // Note: no destructor involved here
    (*it)->doCleanUp();
  }
}

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

void cedar::dev::sensors::visual::Grabber::doCleanUp()
{
  if (!mCleanUpAlreadyDone)
  {
    mCleanUpAlreadyDone = true;

    // stop LoopedThread
    if (this->isRunningNolocking())
    {
      cedar::aux::LoopedThread::stop();
      this->wait();
    }

    // stop Recording
    stopRecording();

    // do the cleanup in derived class
    onCleanUp();

    // set flag
    setIsCreated(false);
  }
}

//--------------------------------------------------------------------------------------------------------------------
// class methods
//--------------------------------------------------------------------------------------------------------------------

void cedar::dev::sensors::visual::Grabber::init
(
  std::vector<cedar::dev::sensors::visual::GrabberChannelPtr> grabberChannels
)
{
  // initialize local member
  mpReadWriteLock = new QReadWriteLock();
  mGrabberThreadStartedOnRecording = false;
  mCleanUpAlreadyDone = false;
  mFpsMeasureStart = boost::posix_time::microsec_clock::local_time();
  mFpsMeasureStop = boost::posix_time::microsec_clock::local_time();
  mFpsCounter = 0;
  mFpsMeasured = 0;
  mRecording = false;
  mIsGrabbing = false;
  mpLockIsGrabbing = new QReadWriteLock();
  mCaptureDeviceCreated = false;
  mpLockCaptureDeviceCreated = new QReadWriteLock();
  mpLockIsCreating = new QReadWriteLock();

  // insert this instance to our instance-vector (used for emergency-cleanup)
  mInstances.push_back(this);

  // add the channels to the configurable interface
  _mChannels = cedar::dev::sensors::visual::ChannelParameterPtr
               (
                  new cedar::dev::sensors::visual::ChannelParameter
                  (
                    this,
                    "channels",
                    grabberChannels
                  )
               );
}

void cedar::dev::sensors::visual::Grabber::onCleanUp()
{
}

bool cedar::dev::sensors::visual::Grabber::applyParameter()
{
  // lock creation of grabber
  mpLockIsCreating->lockForWrite();

  // state-flag
  bool grabber_created=false;

  // check on first channel if there is already channel-information there.
  // this is an indicator, that the channel is open and active
  // if so, close all grabber
  if (isCreated())
  {
    this->closeGrabber();
  }

  // create the channels in the derived classes
  try
  {
    this->onCreateGrabber();
    grabber_created = true;
  }
  catch(cedar::dev::sensors::visual::CreateGrabberException& e)
  {
    cedar::aux::LogSingleton::getInstance()->error(e.getMessage(),"cedar::dev::sensors::visual::Grabber::applyParameter()");
    grabber_created = false;
  }

  if (grabber_created)
  {
    // update state
    for (unsigned int channel = 0; channel < getNumChannels(); ++channel)
    {
      std::string channelinfo = onGetSourceInfo(channel);
      setChannelInfoString(channel,channelinfo);
    }
    setIsCreated(true);
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

  mpLockIsCreating->unlock();
  return grabber_created;
}

bool cedar::dev::sensors::visual::Grabber::isCreated() const
{
  // lock, because grabbing is probably done in an additional thread
  bool result;
  mpLockCaptureDeviceCreated->lockForRead();
  result = mCaptureDeviceCreated;
  mpLockCaptureDeviceCreated->unlock();

  return result;
}

void cedar::dev::sensors::visual::Grabber::setIsCreated(bool isCreated)
{
  // lock, because creation is probably done in an additional thread
  mpLockCaptureDeviceCreated->lockForWrite();
  mCaptureDeviceCreated = isCreated;
  mpLockCaptureDeviceCreated->unlock();
}

void cedar::dev::sensors::visual::Grabber::closeGrabber()
{
  setIsCreated(false);

  if (this->isRunningNolocking())
  {
    this->stop();
  }

  // close the grabber-things in the derived class
  this->onCloseGrabber();

  // reset the common-things from all grabbers
  unsigned int num_channels = _mChannels->size();
  for (unsigned int channel=0; channel < num_channels; channel++)
  {
    this->getGrabberChannel(channel)->mChannelInfo="";
    this->getGrabberChannel(channel)->mImageMat = cv::Mat();
  }

  cedar::aux::LogSingleton::getInstance()->debugMessage
                                           (
                                             this->getName() + ": Close Capture channels",
                                             "cedar::dev::sensors::visual::Grabber::closeGrabber()"
                                           );
}

unsigned int cedar::dev::sensors::visual::Grabber::getNumChannels() const
{
  return _mChannels->size();
}

double cedar::dev::sensors::visual::Grabber::getMeasuredFramerate() const
{
  return mFpsMeasured;
}

double cedar::dev::sensors::visual::Grabber::getFramerate() const
{
  double fps = cedar::unit::Time(1.0 * cedar::unit::second) / LoopedThread::getStepSize();
  return fps;
}

void cedar::dev::sensors::visual::Grabber::setFramerate(double fps)
{
  bool wasRunning = this->isRunningNolocking();

  if (wasRunning)
  {
    this->stop();

    cedar::aux::LogSingleton::getInstance()->debugMessage
                                             (
                                               this->getName()
                                                 + ": Switch to " + cedar::aux::toString(fps) + " fps",
                                               "cedar::dev::sensors::visual::Grabber::setFps()"
                                             );
  }
  // cycle time in ms: 1000ms/frames_per_second
  cedar::unit::Time one_second(1.0 * cedar::unit::second);
  cedar::unit::Time cycle_time(one_second / fps);

  //!@todo this is how it should be done, but this creates a deadlock
//  LoopedThread::setStepSize(cycle_time);        // change speed in thread
  auto param = boost::dynamic_pointer_cast<cedar::aux::TimeParameter>(this->getParameter("step size"));
  param->setValue(cycle_time, false);

  if (wasRunning)
  {
    this->start();
  }

  std::string info;
  if (this->isRunningNolocking())
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

void cedar::dev::sensors::visual::Grabber::processQuit()
{
  cedar::aux::LogSingleton::getInstance()->debugMessage
                                           (
                                             this->getName() + ": Grabbing stopped",
                                             "cedar::dev::sensors::visual::Grabber::stop()"
                                           );
  mFpsMeasured = 0;

  if (mRecording)
  {
    this->stopRecording();
  }
  mpLockIsGrabbing->lockForWrite();
  mIsGrabbing = false;
  mpLockIsGrabbing->unlock();
}

void cedar::dev::sensors::visual::Grabber::setIsGrabbing(bool isGrabbing)
{
  mpLockIsGrabbing->lockForWrite();
  mIsGrabbing = isGrabbing;
  mpLockIsGrabbing->unlock();
}

void cedar::dev::sensors::visual::Grabber::prepareStart()
{
  mpLockIsGrabbing->lockForWrite();
  mIsGrabbing = true;
  mpLockIsGrabbing->unlock();

  mFpsMeasured = 0;
  mFpsCounter = 0;
  mFpsMeasureStart = boost::posix_time::microsec_clock::local_time();
  cedar::aux::LogSingleton::getInstance()->debugMessage
                                           (
                                             this->getName() + ": Grabbing started with "
                                               + cedar::aux::toString(getFramerate()) + " FPS.",
                                             "cedar::dev::sensors::visual::Grabber::start()"
                                           );
}

void cedar::dev::sensors::visual::Grabber::grab()
{
  bool result = true;
  std::string error_info = "";

  //lock grabber to block recreation due to parameter changes
  mpLockIsCreating->lockForRead();

  // lock channel image matrix
  mpReadWriteLock->lockForWrite();
  try
  {
    unsigned int num_channels = getNumChannels();
    for(unsigned int channel = 0; channel < num_channels; ++channel)
    {
      try
      {
        this->onGrab(channel);
      }
      catch(cedar::dev::sensors::visual::GrabberGrabException)
      {
        if (error_info != "")
        {
          error_info = error_info + "\n";
        }
        error_info = error_info + "Channel " + cedar::aux::toString(channel) + ": cvCapture.read() error!";
        result = false;
      }
    }
  }
  catch (std::exception& e)
  {
    error_info = e.what();
    result = false;
  }

  // unlock channel image matrix
  mpReadWriteLock->unlock();
  mpLockIsCreating->unlock();

  if (! result)
  {
    cedar::aux::LogSingleton::getInstance()->error
                                             (
                                               this->getName() + ": " + error_info,
                                               "cedar::dev::sensors::visual::Grabber::grab()"
                                             );
    this->closeGrabber();
    CEDAR_THROW
    (
      cedar::dev::sensors::visual::GrabberGrabException,
      "[Grabber::grab] Error in grabber \""+ this->getName() + "\" on grabbing: " + error_info
    );
  }

  // grabbing ok, then
  // calculate fps every 5 (default) grabbing frames
  if (! (++mFpsCounter %= UPDATE_FPS_MEASURE_FRAME_COUNT))
  {
    mFpsMeasureStop = boost::posix_time::microsec_clock::local_time();

    // calculate real diff:
    boost::posix_time::time_duration measure_duration = mFpsMeasureStop - mFpsMeasureStart;
    mFpsMeasured = double(UPDATE_FPS_MEASURE_FRAME_COUNT * 1000000. / measure_duration.total_microseconds());

    // set new start-time
    mFpsMeasureStart = boost::posix_time::microsec_clock::local_time();
  }

  // check if recording is on
  if (mRecording)
  {
    unsigned int num_channels = getNumChannels();
    for(unsigned int channel = 0; channel < num_channels; ++channel)
    {
      try
      {
        getGrabberChannel(channel)->mVideoWriter << getGrabberChannel(channel)->mImageMat;
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
        this->closeGrabber();
        CEDAR_THROW(cedar::dev::sensors::visual::GrabberRecordingException,info.str());
      }
    }
  }
}

cv::Mat cedar::dev::sensors::visual::Grabber::getImage(unsigned int channel) const
{
  if (channel >= getNumChannels())
  {
    CEDAR_THROW(cedar::aux::IndexOutOfRangeException,buildChannelErrorMessage(channel));
  }
  return getGrabberChannel(channel)->mImageMat;
}

QReadWriteLock* cedar::dev::sensors::visual::Grabber::getReadWriteLockPointer() const
{
  return mpReadWriteLock;
}

std::string cedar::dev::sensors::visual::Grabber::getSourceInfo(unsigned int channel) const
{
  if (channel >= getNumChannels())
  {
    CEDAR_THROW(cedar::aux::IndexOutOfRangeException,buildChannelErrorMessage(channel));
  }
  return getGrabberChannel(channel)->mChannelInfo;
}

void cedar::dev::sensors::visual::Grabber::setSnapshotName(const std::string& snapshotName)
{
  if (snapshotName == "")
  {
    return;
  }

  // initialize snapshot-names
  std::string name = snapshotName;
  std::string ext  = "";

  // check if extension in filename
  std::size_t pos = name.rfind(".");

  if (pos != std::string::npos)
  {
    // yes: extract it
    int cnt = pos;
    ext  = name.substr(pos);
    name = name.substr(0,cnt);
  }
  else
  {
    // no: use default
    ext  = mGrabberDefaultSnapshotExtension;
    name = snapshotName;
  }

  unsigned int num_channels = getNumChannels();
  if (num_channels == 1)
  {
    getGrabberChannel(0)->_mSnapshotName->setValue(name + ext);
  }
  else
  {
    for(unsigned int channel = 0; channel < num_channels; ++channel)
    {
      getGrabberChannel(channel)->_mSnapshotName->setValue(name + this->getChannelSaveFilenameAddition(channel) + ext);
    }
  }
}

void cedar::dev::sensors::visual::Grabber::setSnapshotName
(
  unsigned int channel,
  const std::string& snapshotName
)
{
  if (channel >= getNumChannels())
  {
    CEDAR_THROW(cedar::aux::IndexOutOfRangeException,buildChannelErrorMessage(channel));
  }

  if (snapshotName != "")
  {
    // check if there is an extension
    std::string name = snapshotName;
    std::size_t pos = name.rfind(".");

    if (pos == std::string::npos)
    {
      // no: use default extension
      name.append(mGrabberDefaultSnapshotExtension);
    }
    getGrabberChannel(channel)->_mSnapshotName->setValue(name);
  }
}

std::string cedar::dev::sensors::visual::Grabber::getSnapshotName(unsigned int channel) const
{
  if (channel >= getNumChannels())
  {
    CEDAR_THROW(cedar::aux::IndexOutOfRangeException,buildChannelErrorMessage(channel));
  }
  return getGrabberChannel(channel)->_mSnapshotName->getPath(true);
}

void cedar::dev::sensors::visual::Grabber::saveSnapshot(unsigned int channel) const
{
  if (channel >= getNumChannels())
  {
    CEDAR_THROW(cedar::aux::IndexOutOfRangeException,buildChannelErrorMessage(channel));
  }

  // copy image to local buffer for slow imwrite-function and for error-checking
  cv::Mat imgBuffer;

  if (!getGrabberChannel(channel)->mImageMat.empty())
  {
    mpReadWriteLock->lockForRead();
    getGrabberChannel(channel)->mImageMat.copyTo(imgBuffer);
    mpReadWriteLock->unlock();

    try
    {
      cv::imwrite(getGrabberChannel(channel)->_mSnapshotName->getPath(true), imgBuffer);
    }
    catch (std::exception &e)
    {
      std::string msg = this->getName() + " Channel " + cedar::aux::toString(channel)
                          + ": could not save snapshot! " + e.what();
      CEDAR_THROW(cedar::dev::sensors::visual::GrabberSnapshotException,msg);
    }
  }
  else
  {
    std::string info = "[Grabber::saveSnapshot] Channel "
                       + cedar::aux::toString(channel)
                       + ": Image matrix is empty!";
    CEDAR_THROW(cedar::dev::sensors::visual::GrabberSnapshotException,info);
  }
}

void cedar::dev::sensors::visual::Grabber::saveSnapshotAllCams() const
{
  unsigned int num_channels = getNumChannels();
  for(unsigned int channel = 0; channel < num_channels; ++channel)
  {
    saveSnapshot(channel);
  }
}

cv::Size cedar::dev::sensors::visual::Grabber::getSize(unsigned int channel) const
{
  if (channel >= getNumChannels())
  {
    CEDAR_THROW(cedar::aux::IndexOutOfRangeException,buildChannelErrorMessage(channel));
  }
  return getGrabberChannel(channel)->mImageMat.size();
}

void cedar::dev::sensors::visual::Grabber::setRecordName(const std::string& recordName)
{
  if (recordName == "")
  {
    return;
  }

  // initialize snapshot-names
  std::string name = recordName;
  std::string ext  = "";

  // simple check, if there is an extension in the given filename
  std::size_t pos = name.rfind(".");

  if (pos != std::string::npos)
  {
    // yes: extract it
    ext  = name.substr(pos);
    name = name.substr(0,pos);
  }
  else
  {
    // no: use default
    ext  = mGrabberDefaultRecordExtension;
    name = recordName;
  }

  // filename depends on no. of channels
  unsigned int num_channels = getNumChannels();
  if (num_channels == 1)
  {
    getGrabberChannel(0)->_mRecordName->setValue(name + ext);
  }
  else
  {
    for(unsigned int channel = 0; channel < num_channels; ++channel)
    {
      getGrabberChannel(channel)->_mRecordName->setValue(name + this->getChannelSaveFilenameAddition(channel) + ext);
    }
  }
}

void cedar::dev::sensors::visual::Grabber::setRecordName(unsigned int channel, const std::string& recordName )
{
  if (channel >= getNumChannels())
  {
    CEDAR_THROW(cedar::aux::IndexOutOfRangeException,buildChannelErrorMessage(channel));
  }

  if (recordName != "")
  {
    // check if there is an extension
    std::string name = recordName;
    std::size_t pos = name.rfind(".");

    if (pos == std::string::npos)
    {
      // no: use default extension
      name.append(mGrabberDefaultRecordExtension);
    }
    getGrabberChannel(channel)->_mRecordName->setValue(name);
  }
}

const std::string cedar::dev::sensors::visual::Grabber::getRecordName(unsigned int channel) const
{
  if (channel >= getNumChannels())
  {
    CEDAR_THROW(cedar::aux::IndexOutOfRangeException,buildChannelErrorMessage(channel));
  }
  return getGrabberChannel(channel)->_mRecordName->getPath(true);
}

void cedar::dev::sensors::visual::Grabber::startRecording
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
    return;
  }

  // set the record-flag
  mRecording = true;
  unsigned int recording_channels = 0;

  // write the video-file with the actual grabbing-speed
  // this is independent from the speed of the avi-file or the camera framerate

  unsigned int num_channels = getNumChannels();
  for(unsigned int channel = 0; channel < num_channels; ++channel)
  {
    // create writer
    std::string record_name = getGrabberChannel(channel)->_mRecordName->getPath(true);
    cv::VideoWriter writer
                    (
                      record_name,
                      static_cast<unsigned int>(recFormat),
                      fps,
                      getGrabberChannel(channel)->mImageMat.size(),
                      color
                    );

    if (writer.isOpened())
    {
      // save it in channel-struct
      getGrabberChannel(channel)->mVideoWriter = writer;

      std::string info = ": Channel " + cedar::aux::toString(channel) + " recording to " + record_name;

      cedar::aux::LogSingleton::getInstance()->message
                                               (
                                                 this->getName() + info,
                                                 "cedar::dev::sensors::visual::Grabber::startRecording()"
                                               );
      recording_channels++;
    }
  }

  if (recording_channels != num_channels)
  {
    std::string msg = "Start recording: only " + cedar::aux::toString(recording_channels)
                        + " of " + cedar::aux::toString(num_channels) + " recording!";
    CEDAR_THROW(cedar::dev::sensors::visual::GrabberRecordingException,msg);
  }

  // start the grabberthread if needed
  if (!mIsGrabbing)
  {
    if (!this->isRunningNolocking() && startThread)
    {
      cedar::aux::LogSingleton::getInstance()->message
                                               (
                                                 this->getName() + ": Grabbing started on recording",
                                                 "cedar::dev::sensors::visual::Grabber::startRecording()"
                                               );
      mGrabberThreadStartedOnRecording = true;
      this->start();
    }
  }
}

void cedar::dev::sensors::visual::Grabber::stopRecording()
{
  if (mRecording)
  {
    mRecording = false;
    if (mGrabberThreadStartedOnRecording)
    {
      this->stop();
      mGrabberThreadStartedOnRecording = false;
      cedar::aux::LogSingleton::getInstance()->debugMessage
                                               (
                                                 this->getName() + ": Recording stopped",
                                                 "cedar::dev::sensors::visual::Grabber::stopRecording()"
                                               );
    }

    // delete the videowriter
    unsigned int num_channels = getNumChannels();
    for(unsigned int channel = 0; channel < num_channels; ++channel)
    {
      getGrabberChannel(channel)->mVideoWriter = cv::VideoWriter();
    }
  }
}

bool cedar::dev::sensors::visual::Grabber::isRecording() const
{
  return mRecording;
}

void cedar::dev::sensors::visual::Grabber::step(cedar::unit::Time)
{
  // if something went wrong on grabbing,
  // an exception is being thrown in the grab function
  // right after a cleanup
  grab();
}

std::string cedar::dev::sensors::visual::Grabber::getChannelSaveFilenameAddition(int channel) const
{
  return "_ch[" + cedar::aux::toString(channel) + "]";
}

cv::Mat& cedar::dev::sensors::visual::Grabber::getImageMat(unsigned int channel)
{
  if (channel >= getNumChannels())
  {
    CEDAR_THROW(cedar::aux::IndexOutOfRangeException,buildChannelErrorMessage(channel));
  }
  return getGrabberChannel(channel)->mImageMat;
}

void cedar::dev::sensors::visual::Grabber::setChannelInfoString(unsigned int channel, std::string info)
{
  if (channel >= getNumChannels())
  {
    CEDAR_THROW(cedar::aux::IndexOutOfRangeException,buildChannelErrorMessage(channel));
  }
  getGrabberChannel(channel)->mChannelInfo = info;
}

