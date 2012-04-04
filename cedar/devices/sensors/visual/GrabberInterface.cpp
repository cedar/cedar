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

    File:        GrabberInterface.h

    Maintainer:  Georg.Hartinger
    Email:       georg.hartinger@ini.rub.de
    Date:        2011 08 01

    Description: Header for the @em cedar::Grabber class.

    Credits:

======================================================================================================================*/

// CEDAR INCLUDES
#include "cedar/devices/sensors/visual/GrabberInterface.h"

// SYSTEM INCLUDES
#include <signal.h>


const std::string cedar::dev::sensors::visual::GrabberInterface::mGrabberDefaultRecordExtension =  ".avi";
const std::string cedar::dev::sensors::visual::GrabberInterface::mGrabberDefaultSnapshotExtension = ".jpg";

//Initialize the static member vector containing all instances
cedar::dev::sensors::visual::GrabberInterface::GrabberInstancesVector cedar::dev::sensors::visual::GrabberInterface::mInstances;
bool cedar::dev::sensors::visual::GrabberInterface::mRegisterTerminationHandler;

//----------------------------------------------------------------------------------------------------------------------
//constructors and destructor
//----------------------------------------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------------------------------------
cedar::dev::sensors::visual::GrabberInterface::GrabberInterface(const std::string& configFileName)
:
LoopedThread(configFileName)
{
  #ifdef DEBUG_GRABBER_INTERFACE
    std::cout << "[GrabberInterface::GrabberInterface]" << std::endl;
  #endif

  //initialize local member
  mpReadWriteLock = new QReadWriteLock();
  mRecord = false;
  mGrabberThreadStartedOnRecording = false;
  mCleanUpAlreadyDone = false;
  mFpsMeasureStart = boost::posix_time::microsec_clock::local_time();
  mFpsMeasureStop = boost::posix_time::microsec_clock::local_time();
  mFpsCounter = 0;
  mFpsMeasured = 0;
}


//--------------------------------------------------------------------------------------------------------------------
cedar::dev::sensors::visual::GrabberInterface::~GrabberInterface()
{
  #ifdef DEBUG_GRABBER_INTERFACE
    std::cout << "[GrabberInterface::~GrabberInterface]" << std::endl;
  #endif


  //remove this grabber-instance from the InstancesVector
  std::vector<GrabberInterface *>::iterator it = mInstances.begin();

  while (((*it) != this) && (it != mInstances.end()))
  {
    ++it;
  }

  if ((*it) == this)
  {
    mInstances.erase(it);
    #ifdef DEBUG_GRABBER_INTERFACE
      std::cout << "[GrabberInterface::~GrabberInterface] Grabber " << ConfigurationInterface::_mName
                << " deleted from list of all instances." << std::endl;
    #endif
  }
}

//----------------------------------------------------------------------------------------------------------------------
//methods
//----------------------------------------------------------------------------------------------------------------------

//this function handles abnormal termination of the program (signal: interrupt and abort)
void cedar::dev::sensors::visual::GrabberInterface::interruptSignalHandler(int)
{
  emergencyCleanup();
  std::exit(1);
}

//--------------------------------------------------------------------------------------------------------------------
void cedar::dev::sensors::visual::GrabberInterface::emergencyCleanup()
{
  #ifdef DEBUG_GRABBER_INTERFACE
    std::cout << "[GrabberInterface::emergencyCleanup]" << std::endl;
  #endif
  for (std::vector<GrabberInterface*>::iterator it = mInstances.begin(); it != mInstances.end(); ++it)
  {
    //only cedar::dev::sensors::visual::GrabberInterface::doCleanUp() and xxxGrabber::onCleanUp() methods invoked
    //Note: no destructor involved here
    (*it)->doCleanUp();
  }
}

//--------------------------------------------------------------------------------------------------------------------
void cedar::dev::sensors::visual::GrabberInterface::installCrashHandler()
{
   signal(SIGINT,&GrabberInterface::interruptSignalHandler);
   signal(SIGABRT,&GrabberInterface::interruptSignalHandler);
   signal(SIGSEGV,&GrabberInterface::interruptSignalHandler);
}


//--------------------------------------------------------------------------------------------------------------------
void cedar::dev::sensors::visual::GrabberInterface::doCleanUp()
{
  #ifdef DEBUG_GRABBER_INTERFACE
    std::cout << "[GrabberInterface::doCleanUp] " << std::boolalpha << mCleanUpAlreadyDone << std::endl;
  #endif

  if (! mCleanUpAlreadyDone)
  {

    #ifdef DEBUG_GRABBER_INTERFACE
      std::cout << "[GrabberInterface::doCleanUp] cleanup" << std::endl;
    #endif

    mCleanUpAlreadyDone = true;

    //stopGrabber LoopedThread
    if (QThread::isRunning())
    {
      cedar::aux::LoopedThread::stop();
      sleep(500);
    }

    //stopGrabber Recording
    stopRecording();

    //do the cleanup in derived class
    onCleanUp();
    mChannels.clear();

    if (mpReadWriteLock)
    {
      delete mpReadWriteLock;
      mpReadWriteLock = NULL;
    }
  }
}

//--------------------------------------------------------------------------------------------------------------------
bool cedar::dev::sensors::visual::GrabberInterface::onInit()
{
  return true;
}

//--------------------------------------------------------------------------------------------------------------------
void cedar::dev::sensors::visual::GrabberInterface::onCleanUp()
{
}

//--------------------------------------------------------------------------------------------------------------------
bool cedar::dev::sensors::visual::GrabberInterface::onGrab()
{
  return true;
}

//--------------------------------------------------------------------------------------------------------------------
bool cedar::dev::sensors::visual::GrabberInterface::onDeclareParameters()
{
  return true;
}

//--------------------------------------------------------------------------------------------------------------------
bool cedar::dev::sensors::visual::GrabberInterface::onWriteConfiguration()
{
  return true;
}

//--------------------------------------------------------------------------------------------------------------------
void cedar::dev::sensors::visual::GrabberInterface::onAddChannel()
{
  GrabberChannelPtr channel(new GrabberChannel);
  mChannels.push_back(channel);
}

//--------------------------------------------------------------------------------------------------------------------
void cedar::dev::sensors::visual::GrabberInterface::readInit(unsigned int numCams,const std::string& defaultGrabberName)
{
  mNumCams = numCams;

  //create number of channels
  for (unsigned int channel=0; channel < mNumCams; channel++)
  {
    this->onAddChannel();
  }

  mInstances.push_back(this);

  //Parameters for all Grabber
  bool result =
    cedar::aux::ConfigurationInterface::addParameter(&_mName,"grabbername",defaultGrabberName) == CONFIG_SUCCESS;

  if (!result)
  {
    CEDAR_THROW
    (
      cedar::aux::exc::InitializationException,
      "GrabberInterface::doInit - Error in cedar::aux::ConfigurationInterface::addParameter"
    );
  }

  //Parameters in the derived classes
  if (!onDeclareParameters())
  {
    CEDAR_THROW
    (
      cedar::aux::exc::InitializationException,
      "GrabberInterface::doInit - Error in onDeclareParameters in derived class"
    );
  }

  //restore configuration from config-file
  cedar::aux::ConfigurationInterface::readOrDefaultConfiguration();
}

//--------------------------------------------------------------------------------------------------------------------
void cedar::dev::sensors::visual::GrabberInterface::applyInit()
{
  //values from constructor (for guid) override settings from configfile

  //initialize the snapshot and recording names with default values
  //depends on no. of cameras
  setSnapshotName("snapshot.jpg");
  setRecordName("record.avi");

  //initialize derived class
  if (!onInit())
  {
    //cleanup already initialized channels
    doCleanUp();

    CEDAR_THROW
    (
      cedar::aux::exc::InitializationException,
      "[GrabberInterface::doInit] ERROR: onInit in class"+ this->getName()
    );
  }
}

//--------------------------------------------------------------------------------------------------------------------
bool cedar::dev::sensors::visual::GrabberInterface::writeConfiguration()
{    
  //update the parameters (if needed) in derived class
  if (!onWriteConfiguration())
  {
    std::cout << "[GrabberInterface::writeConfiguration] Error while updating the parameters in the derived class. "
              << "Configuration NOT saved!!"<< std::endl;
    return false;
  }

  //write out the parameter of the Grabber
  int result = cedar::aux::ConfigurationInterface::writeConfiguration();

  #ifdef DEBUG_GRABBER_INTERFACE
    std::cout << "[GrabberInterface::writeConfiguration] Configuration saved" << std::endl;
  #endif
  return static_cast<bool>(result);
}

//--------------------------------------------------------------------------------------------------------------------
unsigned int cedar::dev::sensors::visual::GrabberInterface::getNumCams() const
{
  return mNumCams;
}

//--------------------------------------------------------------------------------------------------------------------
double cedar::dev::sensors::visual::GrabberInterface::getFpsMeasured() const
{
  return mFpsMeasured;
}

//--------------------------------------------------------------------------------------------------------------------
double cedar::dev::sensors::visual::GrabberInterface::getFps() const
{
  double fps = 1000. / LoopedThread::_mStepSize;
  return fps;
}


//--------------------------------------------------------------------------------------------------------------------
void cedar::dev::sensors::visual::GrabberInterface::setFps(double fps)
{
  bool wasRunning = QThread::isRunning();

  if (wasRunning)
  {
    stopGrabber();
  }

  //cycle time in ms: 1000ms/frames_per_second
  double milliseconds = 1000. / fps;

  #ifdef DEBUG_GRABBER_INTERFACE
    if (wasRunning)
    {
      std::cout << "[GrabberInterface::setFps] thread stopped" << std::endl;
    }
    std::cout << "[GrabberInterface::setFps] switch to " << fps <<" fps"<< std::endl;
  #endif

  //LoopedThread::_mStepSize = milliseconds;  //set param for configuration file (bug in LoopedThread)
  LoopedThread::setStepSize(milliseconds);        //change speed in thread

  if (wasRunning)
  {
    startGrabber();
  }

  #ifdef DEBUG_GRABBER_INTERFACE
    if (QThread::isRunning())
    {
      std::cout << "[GrabberInterface::setFps] grabberthread running" << std::endl;
    }
    else
    {
      std::cout << "[GrabberInterface::setFps] grabberthread not running" << std::endl;
    }
  #endif
}

//--------------------------------------------------------------------------------------------------------------------
void cedar::dev::sensors::visual::GrabberInterface::stopGrabber()
{
  LoopedThread::stop();
  mFpsMeasured = 0;
  if (mRecord)
  {
    stopRecording();
  }
}

//--------------------------------------------------------------------------------------------------------------------
void cedar::dev::sensors::visual::GrabberInterface::startGrabber()
{
  mFpsMeasured = 0;
  mFpsCounter = 0;
  mFpsMeasureStart = boost::posix_time::microsec_clock::local_time();
  LoopedThread::start();
}

//--------------------------------------------------------------------------------------------------------------------
void cedar::dev::sensors::visual::GrabberInterface::grab()
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
    doCleanUp();
    CEDAR_THROW
    ( 
      cedar::aux::exc::GrabberGrabException,
      "[GrabberInterface::grab] Error in grabber \""+ _mName + "\" on grabbing: " + error_info
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

    #ifdef DEBUG_GRABBER_STEP
      std::cout << "[GrabberInterface::grab] update getFpsMeasured to " << mFpsMeasured << std::endl;
    #endif

    mFpsMeasureStart = boost::posix_time::microsec_clock::local_time();
  }

  //check if recording is on
  if (mRecord)
  {
    for (unsigned int channel = 0; channel < mNumCams; ++channel)
    {
      try
      {
        getChannel(channel)->mVideoWriter << getChannel(channel)->mImageMat;
      }
      catch (std::exception& e)
      {
        std::stringstream info;
        info << "[GrabberInterface::grab] Error recording channel" << channel << ": " << e.what();
        doCleanUp();
        CEDAR_THROW(cedar::aux::exc::GrabberRecordingException,info.str());
      }
    }
  }
}

//--------------------------------------------------------------------------------------------------------------------
cv::Mat cedar::dev::sensors::visual::GrabberInterface::getImage(unsigned int channel) const
{
  if (channel >= mNumCams)
  {
    CEDAR_THROW(cedar::aux::exc::IndexOutOfRangeException,"GrabberInterface::getImage");
  }
  return getChannel(channel)->mImageMat;
}

//--------------------------------------------------------------------------------------------------------------------
QReadWriteLock* cedar::dev::sensors::visual::GrabberInterface::getReadWriteLockPointer() const
{
  return mpReadWriteLock;
}

//--------------------------------------------------------------------------------------------------------------------
std::string cedar::dev::sensors::visual::GrabberInterface::getSourceInfo(unsigned int channel) const
{
  if (channel >= mNumCams)
  {
    CEDAR_THROW(cedar::aux::exc::IndexOutOfRangeException,"GrabberInterface::getPhysicalSourceInformation");
  }
  return onGetSourceInfo(channel);
}

//--------------------------------------------------------------------------------------------------------------------
void cedar::dev::sensors::visual::GrabberInterface::setSnapshotName(const std::string& snapshotName)
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
    #ifdef ENABLE_GRABBER_WARNING_OUTPUT
      std::cout << "[GrabberInterface::setSnapshotName] Warning: No extension in filename! Using .jpg\n";
    #endif
    ext  = mGrabberDefaultSnapshotExtension;
    name = snapshotName;
  }

  if (mNumCams==1)
  {
    getChannel(0)->mSnapshotName = name + ext;
  }
  else
  {
    for (unsigned int channel=0; channel<mNumCams;channel++)
    {
      getChannel(channel)->mSnapshotName = name + this->getChannelSaveFilenameAddition(channel) + ext;
    }
  }
}

//--------------------------------------------------------------------------------------------------------------------
void cedar::dev::sensors::visual::GrabberInterface::setSnapshotName
(
  unsigned int channel,
  const std::string& snapshotName
)
{
  if (channel >= mNumCams)
  {
    CEDAR_THROW(cedar::aux::exc::IndexOutOfRangeException,"GrabberInterface::setSnapshotName");
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
    getChannel(channel)->mSnapshotName = name;
  }
}

//--------------------------------------------------------------------------------------------------------------------
std::string cedar::dev::sensors::visual::GrabberInterface::getSnapshotName(unsigned int channel) const
{
  if (channel >= mNumCams)
  {
    CEDAR_THROW(cedar::aux::exc::IndexOutOfRangeException,"GrabberInterface::getSnapshotName");
  }
  return getChannel(channel)->mSnapshotName;
}

//--------------------------------------------------------------------------------------------------------------------
void cedar::dev::sensors::visual::GrabberInterface::saveSnapshot(unsigned int channel) const
{
  if (channel >= mNumCams)
  {
    CEDAR_THROW(cedar::aux::exc::IndexOutOfRangeException,"GrabberInterface::saveSnapshot");
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
      cv::imwrite(getChannel(channel)->mSnapshotName, imgBuffer);
    }
    catch (std::exception &e)
    {
      std::stringstream info;
      info << "[GrabberInterface::saveSnapshot] Channel " << channel << ": " << e.what();
      CEDAR_THROW(cedar::aux::exc::GrabberSnapshotException,info.str());
    }
  }
  else
  {
    std::string info = "[GrabberInterface::saveSnapshot] Channel "
                       + boost::lexical_cast<std::string>(channel)
                       +": Image matrix is empty!";
    #ifdef ENABLE_GRABBER_WARNING_OUTPUT
      std::cout << info << std::endl;
    #endif
    CEDAR_THROW(cedar::aux::exc::GrabberSnapshotException,info);
  }
}

//--------------------------------------------------------------------------------------------------------------------
void cedar::dev::sensors::visual::GrabberInterface::saveSnapshotAllCams() const
{
  for (unsigned int cam = 0; cam < mNumCams; ++cam)
  {
    saveSnapshot(cam);
  }
}

//--------------------------------------------------------------------------------------------------------------------
cv::Size cedar::dev::sensors::visual::GrabberInterface::getSize(unsigned int channel) const
{
  if (channel >= mNumCams)
  {
    CEDAR_THROW(cedar::aux::exc::IndexOutOfRangeException,"GrabberInterface::getSize");
  }
  return getChannel(channel)->mImageMat.size();
}

//--------------------------------------------------------------------------------------------------------------------
void cedar::dev::sensors::visual::GrabberInterface::setRecordName(const std::string& recordName)
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
  if (mNumCams == 1)
  {
    getChannel(0)->mRecordName = name + ext;
  }
  else
  {
    for (unsigned int channel=0; channel<mNumCams;channel++)
    {
      getChannel(channel)->mRecordName = name + this->getChannelSaveFilenameAddition(channel) + ext;
    }
  }
}

//--------------------------------------------------------------------------------------------------------------------
void cedar::dev::sensors::visual::GrabberInterface::setRecordName(unsigned int channel, const std::string& recordName )
{
  if (channel >= mNumCams)
  {
    CEDAR_THROW(cedar::aux::exc::IndexOutOfRangeException,"GrabberInterface::setRecordName");
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
    getChannel(channel)->mRecordName = name;
  }
}

//--------------------------------------------------------------------------------------------------------------------
const std::string& cedar::dev::sensors::visual::GrabberInterface::getRecordName(unsigned int channel) const
{
  if (channel >= mNumCams)
  {
    CEDAR_THROW(cedar::aux::exc::IndexOutOfRangeException,"GrabberInterface::getRecordName");
  }
  return getChannel(channel)->mRecordName;
}

//--------------------------------------------------------------------------------------------------------------------
bool cedar::dev::sensors::visual::GrabberInterface::startRecording(double fps, int fourcc, bool color)
{
  if (mRecord)
  {
    #ifdef ENABLE_GRABBER_WARNING_OUTPUT
      std::cout << "[GrabberInterface::startRecording] Warning: Grabber is already recording\n";
    #endif
    return true;
  }

  //set the record-flag
  mRecord = true;
  unsigned int recording_channels = 0;

  //write the video-file with the actual grabbing-speed
  //this is independent from the speed of the avi-file or the camera framerate

  for (unsigned int channel = 0; channel < mNumCams; ++channel)
  {
    cv::VideoWriter writer(getChannel(channel)->mRecordName,fourcc,fps,getChannel(channel)->mImageMat.size(),color);

    if (writer.isOpened())
    {
      getChannel(channel)->mVideoWriter = writer;
      #ifdef DEBUG_GRABBER_INTERFACE
        std::cout << "Channel " << channel << " recording to " << getChannel(channel)->mRecordName << std::endl;
      #endif
      recording_channels++;
    }
  }

  if (recording_channels != mNumCams)
  {
    CEDAR_THROW
      (
        cedar::aux::exc::GrabberRecordingException,
        "Start recording: only " + boost::lexical_cast<std::string>(recording_channels)
          + " of " + boost::lexical_cast<std::string>(mNumCams) + " recording!"
      )
  }

  //startGrabber the grabberthread if needed
  if (!QThread::isRunning())
  {
    #ifdef DEBUG_GRABBER_INTERFACE
      std::cout << "[GrabberInterface::startRecording] autostart the grabberthread" << std::endl;
    #endif

    mGrabberThreadStartedOnRecording = true;
    startGrabber();
  }
  return true;
}

//--------------------------------------------------------------------------------------------------------------------
void cedar::dev::sensors::visual::GrabberInterface::stopRecording()
{
  if (mRecord)
  {
    mRecord = false;
    if (mGrabberThreadStartedOnRecording)
    {
      #ifdef DEBUG_GRABBER_INTERFACE
        std::cout << "[GrabberInterface::stopRecording] autostop the grabberthread" << std::endl;
      #endif
      stopGrabber();
      mGrabberThreadStartedOnRecording = false;
    }

    //delete the videowriter
    for (unsigned int channel=0; channel<mNumCams;channel++)
    {
      getChannel(channel)->mVideoWriter = cv::VideoWriter();
    }
  }
}

//--------------------------------------------------------------------------------------------------------------------
bool cedar::dev::sensors::visual::GrabberInterface::isRecording() const
{
  return mRecord;
}

//--------------------------------------------------------------------------------------------------------------------
void cedar::dev::sensors::visual::GrabberInterface::step(double)
{
  #ifdef DEBUG_GRABBER_STEP
    std::cout << "GrabberInterface_Thread: step()" << std::endl;
  # endif

  //if something went wrong on grabbing,
  //an exception is being thrown in the grab function
  //right after a cleanup
  grab();
}

//--------------------------------------------------------------------------------------------------------------------
std::string cedar::dev::sensors::visual::GrabberInterface::getChannelSaveFilenameAddition(int channel) const
{
  return "_ch["+boost::lexical_cast<std::string>(channel)+"]";
}

