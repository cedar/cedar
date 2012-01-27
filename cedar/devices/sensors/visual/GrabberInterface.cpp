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

    File:        Grabber.h

    Maintainer:  Georg.Hartinger
    Email:       georg.hartinger@ini.rub.de
    Date:        2011 08 01

    Description: Header for the @em cedar::Grabber class.

    Credits:

======================================================================================================================*/

// LOCAL INCLUDES
#include "cedar/devices/sensors/visual/GrabberInterface.h"

// PROJECT INCLUDES

// SYSTEM INCLUDES



using namespace cedar::dev::sensors::visual;


#ifdef ENABLE_CTRL_C_HANDLER
  //Initialize the static member vector containing all instances
  GrabberInstancesVector GrabberInterface::mInstances;
#endif

//----------------------------------------------------------------------------------------------------------------------
//constructors and destructor
//----------------------------------------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------------------------------------
GrabberInterface::GrabberInterface(const std::string& configFileName)
:
LoopedThread(configFileName)
{
  #ifdef DEBUG_GRABBER_INTERFACE
    std::cout << "[GrabberInterface::GrabberInterface]" << std::endl;
  #endif

  //initialize local member
  mpReadWriteLock = new QReadWriteLock();
  mRecord = false;                  //by default no recording
  mGrabberThreadStartedOnRecording = false;
  mCleanUpAlreadyDone = false;
  mFpsMeasureStart = boost::posix_time::microsec_clock::local_time();
  mFpsMeasureStop = boost::posix_time::microsec_clock::local_time();
  mFpsCounter = 0;
  mFpsMeasured = 0;
}


//--------------------------------------------------------------------------------------------------------------------
GrabberInterface::~GrabberInterface()
{

  #ifdef DEBUG_GRABBER_INTERFACE
    std::cout << "[GrabberInterface::~GrabberInterface]" << std::endl;
  #endif

  //Matrices are released within the vector mImageMatVector

  //remove this grabber-instance from the InstancesVector
  #ifdef ENABLE_CTRL_C_HANDLER

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

    #ifdef ENABLE_GRABBER_WARNING_OUTPUT
      else
      {
        std::cout << "[GrabberInterface::~GrabberInterface] Warning: Grabber " << ConfigurationInterface::_mName
                  << "not found in list of all instances" << std::endl;
      }
    #endif
  #endif

}

//----------------------------------------------------------------------------------------------------------------------
//methods
//----------------------------------------------------------------------------------------------------------------------

#ifdef ENABLE_CTRL_C_HANDLER

  //this function handles the ctrl-c (signal: interrupt)
  //void GrabberInterface::sigIntHandler(int signalNo)
  void GrabberInterface::sigIntHandler(int)
  {
      #ifdef DEBUG_GRABBER_INTERFACE
        std::cout << "[GrabberInterface::sigIntHandler] Abnormal program termination catched" << std::endl;
      #endif

      for (std::vector<GrabberInterface*>::iterator it = mInstances.begin() ; it != mInstances.end();++it)
      {
        //only GrabberInterface::doCleanUp() and xxxGrabber::onCleanUp() methods invoked
        //Note: no destructor involved here
        (*it)->doCleanUp();
      }
      std::exit(1);
  }
#endif

//--------------------------------------------------------------------------------------------------------------------
void GrabberInterface::doCleanUp()
{
  if (! mCleanUpAlreadyDone)
  {
  
    #ifdef DEBUG_GRABBER_INTERFACE
      std::cout << "[GrabberInterface::doCleanUp]" << std::endl;
    #endif

    mCleanUpAlreadyDone = true;

    //stop LoopedThread
    if (QThread::isRunning())
    {
      cedar::aux::LoopedThread::stop();
    }

    //stop Recording
    stopRecording();

    //do the cleanup in derived class
    onCleanUp();

    if (mpReadWriteLock)
    {
      delete mpReadWriteLock;
      mpReadWriteLock = NULL;
    }
  }
}

//--------------------------------------------------------------------------------------------------------------------
void GrabberInterface::readInit(unsigned int numCams, const std::string& defaultGrabberName )
{
  mNumCams = numCams;

  #ifdef ENABLE_CTRL_C_HANDLER
     //if (signal((int) SIGINT, &GrabberInterface::sigIntHandler) == SIG_ERR)
     signal(SIGINT,&GrabberInterface::sigIntHandler);
     signal(SIGABRT,&GrabberInterface::sigIntHandler);
     mInstances.push_back(this);
  #endif

  //Parameters for all Grabber
  bool result
    = cedar::aux::ConfigurationInterface::addParameter(&_mName,"grabbername",defaultGrabberName) == CONFIG_SUCCESS;

  if (!result)
  {
    CEDAR_THROW(cedar::aux::exc::InitializationException,
                "GrabberInterface::doInit - Error in cedar::aux::ConfigurationInterface::addParameter");
  }

  //Parameters in the derived classes
  if (!onDeclareParameters())
  {
    CEDAR_THROW(cedar::aux::exc::InitializationException,
                "GrabberInterface::doInit - Error in onDeclareParameters in derived class");
  }

  //restore configuration from config-file
  cedar::aux::ConfigurationInterface::readOrDefaultConfiguration();
}

//--------------------------------------------------------------------------------------------------------------------
void GrabberInterface::applyInit()
{
  //initialize the snapshot and recording names with default values
  //depends on no. of cameras
  setSnapshotName("snapshot.jpg");
  setRecordName("record.avi");

  //initialize derived class
  if (!onInit())
  {
    //cleanup already initialized channels
    doCleanUp();

    CEDAR_THROW(cedar::aux::exc::InitializationException,
                "[GrabberInterface::doInit] ERROR: onInit in class"+ this->getName());
  }
}

//--------------------------------------------------------------------------------------------------------------------
bool GrabberInterface::writeConfiguration()
{    
  //update the parameters (if needed) in derived class
  if (!onWriteConfiguration())
  {
    std::cout << "[GrabberInterface::writeConfiguration] Error while updating the parameters in the derived class. "
              << "Configuration NOT saved!!";
    return false;
  }

  //write out the parameter of the configurationInterface
  bool result = cedar::aux::ConfigurationInterface::writeConfiguration();

  #ifdef DEBUG_GRABBER_INTERFACE
    std::cout << "[GrabberInterface::writeConfiguration] Configuration saved" << std::endl;
  #endif
  
  return result;
      
}

//--------------------------------------------------------------------------------------------------------------------
unsigned int GrabberInterface::getNumCams() const
{
  return mNumCams;
}

double GrabberInterface::getFpsMeasured() const
{
  return mFpsMeasured;
}

//--------------------------------------------------------------------------------------------------------------------
double GrabberInterface::getFps() const
{
  //unsigned int fps = round(1000 / LoopedThread::_mStepSize);
  double fps = 1000. / LoopedThread::_mStepSize;
  return fps;
}


//--------------------------------------------------------------------------------------------------------------------
void GrabberInterface::setFps(double fps)
{
  bool wasRunning = QThread::isRunning();

  if (wasRunning)
  {
    stop();
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


  //LoopedThread::_mStepSize = milliseconds;  //set param for configuration file
  LoopedThread::setStepSize(milliseconds);        //change speed in thread

  if (wasRunning)
  {
    start();
  }

  #ifdef DEBUG_GRABBER_INTERFACE
    //std::cout << "[GrabberInterface::setFps] new values: getFps: " << getFps()
    //         << ", stepsize " << milliseconds << "ms"<< std::endl;

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
void GrabberInterface::stop()
{
  LoopedThread::stop();
  mFpsMeasured = 0;
  if (mRecord)
  {
    stopRecording();
  }
}

//--------------------------------------------------------------------------------------------------------------------
void GrabberInterface::start()
{
  mFpsMeasured = 0;
  mFpsCounter = 0;
  mFpsMeasureStart = boost::posix_time::microsec_clock::local_time();
  LoopedThread::start();
}

//--------------------------------------------------------------------------------------------------------------------
bool GrabberInterface::grab()
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
    CEDAR_THROW(cedar::aux::exc::GrabberGrabException,"[GrabberInterface::grab] Error in grabber \""
                + _mName + "\" on grabbing: " + error_info);
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
  //std::cout << "[GrabberInterface::grab] mFpsCounter: " << mFpsCounter << std::endl;

  //check if recording is on
  if (mRecord)
  {
    for (unsigned int channel = 0; channel < mNumCams; ++channel)
    {
      try
      {
        mVideoWriterVector.at(channel) << mImageMatVector.at(channel);
      }
      catch (std::exception& e)
      {
        std::stringstream info;
        info << "[GrabberInterface::grab] Error recording channel" << channel << ": " << e.what();
        CEDAR_THROW(cedar::aux::exc::GrabberRecordingException,info.str());
      }
    }
  }
  return result;
}

//--------------------------------------------------------------------------------------------------------------------
cv::Mat GrabberInterface::getImage(unsigned int channel) const
{
  if (channel >= mNumCams)
  {
    CEDAR_THROW(cedar::aux::exc::IndexOutOfRangeException,"GrabberInterface::getImage");
  }
  return mImageMatVector.at(channel);
}

//--------------------------------------------------------------------------------------------------------------------
QReadWriteLock* GrabberInterface::getReadWriteLockPointer() const
{
  return mpReadWriteLock;
}

//--------------------------------------------------------------------------------------------------------------------
std::string GrabberInterface::getSourceInfo(unsigned int channel) const
{
  if (channel >= mNumCams)
  {
    CEDAR_THROW(cedar::aux::exc::IndexOutOfRangeException,"GrabberInterface::getPhysicalSourceInformation");
  }
  return onGetSourceInfo(channel);
}

//--------------------------------------------------------------------------------------------------------------------
void GrabberInterface::setSnapshotName(const std::string& snapshotName)
{

  if (snapshotName == "")
  {
    return;
  }

  mSnapshotNames.clear();

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
    ext  = GRABBER_DEFAULT_SNAPSHOT_EXTENSION;
    name = snapshotName;
  }

  //filename depends on no. of cams
  if (mNumCams > 1)
  {
    //insert all into vector
    mSnapshotNames.push_back(name + GRABBER_SAVE_FILENAMES_ADDITION_CHANNEL_0 + ext);
    mSnapshotNames.push_back(name + GRABBER_SAVE_FILENAMES_ADDITION_CHANNEL_1 + ext);
  }
  else
  {
    mSnapshotNames.push_back(name + ext);
  }
}

//--------------------------------------------------------------------------------------------------------------------
void GrabberInterface::setSnapshotName(unsigned int channel, const std::string& snapshotName )
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
      name.append(GRABBER_DEFAULT_SNAPSHOT_EXTENSION);
    }

    mSnapshotNames.at(channel) = name;
  }
}

//--------------------------------------------------------------------------------------------------------------------
std::string GrabberInterface::getSnapshotName(unsigned int channel) const
{
  if (channel >= mNumCams)
  {
    CEDAR_THROW(cedar::aux::exc::IndexOutOfRangeException,"GrabberInterface::getSnapshotName");
  }
  return mSnapshotNames.at(channel);
}

//--------------------------------------------------------------------------------------------------------------------
bool GrabberInterface::saveSnapshot(unsigned int channel) const
{
  if (channel >= mNumCams)
  {
    CEDAR_THROW(cedar::aux::exc::IndexOutOfRangeException,"GrabberInterface::saveSnapshot");
  }

  //copy image to local buffer for slow imwrite-function and for error-checking
  cv::Mat imgBuffer;

  if (!mImageMatVector.at(channel).empty())
  {
    //Do the snapshot...
    mpReadWriteLock->lockForRead();
    mImageMatVector.at(channel).copyTo(imgBuffer);
    mpReadWriteLock->unlock();

    try
    {
      cv::imwrite(mSnapshotNames.at(channel), imgBuffer);
    }
    catch (std::exception &e)
    {
      std::stringstream info;
      info << "GrabberInterface::saveSnapshot[ch" << channel << "] - " << e.what();
      CEDAR_THROW(cedar::aux::exc::GrabberSnapshotException,info.str());
    }
  }
  else
  {
    #ifdef ENABLE_GRABBER_WARNING_OUTPUT
      std::cout << "[GrabberInterface::saveSnapshot] Warning: Nothing to save. Image matrix is empty!\n";
    #endif
    return false;
  }

  return true;
}

//--------------------------------------------------------------------------------------------------------------------
bool GrabberInterface::saveSnapshotAllCams() const
{
  bool result = true;

  for (unsigned int cam = 0; cam < mNumCams; ++cam)
  {
    result = saveSnapshot(cam) && result;
  }
  return result;
}

//--------------------------------------------------------------------------------------------------------------------
cv::Size GrabberInterface::getSize(unsigned int channel) const
{
  if (channel >= mNumCams)
  {
    CEDAR_THROW(cedar::aux::exc::IndexOutOfRangeException,"GrabberInterface::getSize");
  }
  return mImageMatVector.at(channel).size();
}

//--------------------------------------------------------------------------------------------------------------------
void GrabberInterface::setRecordName(const std::string& recordName)
{
  if (recordName == "")
  {
    return;
  }

  mRecordNames.clear();

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
    #ifdef ENABLE_GRABBER_WARNING_OUTPUT
      std::cout << "[GrabberInterface::setRecordName] Warning: No extension in filename! Using .avi\n";
    #endif
    ext  = GRABBER_DEFAULT_RECORD_EXTENSION;
    name = recordName;
  }

  //filename depends on no. of cams
  if (mNumCams > 1)
  {
    std::string name_ch0 = name + GRABBER_SAVE_FILENAMES_ADDITION_CHANNEL_0 + ext;
    std::string name_ch1 = name + GRABBER_SAVE_FILENAMES_ADDITION_CHANNEL_1 + ext;
    //std::cout << "[GrabberInterface::setRecordName] : set both recordnames to " << name_ch0 <<"  "<< name_ch1 << std::endl;
    //insert all into vector
    mRecordNames.push_back(name_ch0);
    mRecordNames.push_back(name_ch1);
  }
  else
  {
    mRecordNames.push_back(name + ext);
  }
}

//--------------------------------------------------------------------------------------------------------------------
void GrabberInterface::setRecordName(unsigned int channel, const std::string& recordName )
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
      name.append(GRABBER_DEFAULT_RECORD_EXTENSION);
    }

    mRecordNames.at(channel) = name;
  }
}

//--------------------------------------------------------------------------------------------------------------------
std::string GrabberInterface::getRecordName(unsigned int channel) const
{

  if (channel >= mNumCams)
  {
    CEDAR_THROW(cedar::aux::exc::IndexOutOfRangeException,"GrabberInterface::getRecordName");
  }
  return mRecordNames.at(channel);
}

//--------------------------------------------------------------------------------------------------------------------
bool GrabberInterface::startRecording(double fps, int fourcc, bool color)
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

  //write the video-file with the actual grabbing-speed
  //this is independet from speed of the avi-file or the camera framerate
  //double fps = getFps();

  //if there are already any
  mVideoWriterVector.clear();

  for (unsigned int i = 0; i < mNumCams; ++i)
  {
    cv::VideoWriter writer(mRecordNames.at(i),fourcc,fps,mImageMatVector.at(i).size(),color);

    if (writer.isOpened())
    {
      mVideoWriterVector.push_back(writer);
    }
  }

  if (mVideoWriterVector.size() != (mNumCams ) )
  {
    CEDAR_THROW(cedar::aux::exc::GrabberRecordingException,"GrabberInterface::startRecording")
  }

  //start the grabberthread if needed
  if (!QThread::isRunning())
  {
    #ifdef DEBUG_GRABBER_INTERFACE
      std::cout << "[GrabberInterface::startRecording] autostart the grabberthread" << std::endl;
    #endif

    mGrabberThreadStartedOnRecording = true;
    start();
  }

  return true;
}

//--------------------------------------------------------------------------------------------------------------------
bool GrabberInterface::stopRecording()
{
  if (mRecord)
  {
    mRecord = false;
    if (mGrabberThreadStartedOnRecording)
    {
      #ifdef DEBUG_GRABBER_INTERFACE
        std::cout << "[GrabberInterface::stopRecording] autostop the grabberthread" << std::endl;
      #endif
      stop();
      mGrabberThreadStartedOnRecording = false;
    }
    mVideoWriterVector.clear();
    return true;
  }
  else
  {
    return true;
  }
}

//--------------------------------------------------------------------------------------------------------------------
bool GrabberInterface::isRecording() const
{
  return mRecord;
}

//--------------------------------------------------------------------------------------------------------------------
//void GrabberInterface::step(double time  __attribute__ ((__unused__))) //only supported by gcc
void GrabberInterface::step(double)
{
  #ifdef DEBUG_GRABBER_STEP
    std::cout << "GrabberInterface_Thread: step()" << std::endl;
  # endif

  //if something went wrong on grabbing,
  //an exception is being thrown in the grab function
  //right after a cleanup
  if (!grab())
  {
    //LoopedThread::stop();
  }
}
