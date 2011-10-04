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
    Email:       georg.hartinger@rub.de
    Date:        2011 08 01

    Description: Header for the @em cedar::Grabber class.

    Credits:

======================================================================================================================*/

// LOCAL INCLUDES
#include "GrabberInterface.h"

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
  : LoopedThread(configFileName)
{
  #ifdef DEBUG_GRABBER_INTERFACE
    std::cout << "[GrabberInterface::GrabberInterface]" << std::endl;
  #endif

  //-----------------------------
  //initialize local member
  mpReadWriteLock = new QReadWriteLock();
  mRecord         = false;                  //by default no recording
}


//--------------------------------------------------------------------------------------------------------------------
GrabberInterface::~GrabberInterface()
{

  #ifdef DEBUG_GRABBER_INTERFACE
    std::cout << "[GrabberInterface::~GrabberInterface]" << std::endl;
  #endif

  //stop LoopedThread
  if (QThread::isRunning())
  {
    cedar::aux::LoopedThread::stop();
  }

  //stop Recording
  stopRecording();

  //Matrices are released within the vector mImageMatVector

  //write out the parameter of the configurationInterface
  cedar::aux::ConfigurationInterface::writeConfiguration();

  if (mpReadWriteLock)
  {
    delete mpReadWriteLock;
    mpReadWriteLock = NULL;
  }

  //do the cleanup in derived class
  onDestroy();

  //remove this grabber-instance from the InstancesVector
  #ifdef ENABLE_CTRL_C_HANDLER

    std::vector<GrabberInterface*>::iterator it = mInstances.begin();

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
  void GrabberInterface::sigIntHandler(int sig)
  {
      //Documentation of std::exit():
      //Terminates the process normally, performing the regular cleanup for terminating processes.

      #ifdef DEBUG_GRABBER_INTERFACE
        std::cout << "[GrabberInterface::sigIntHandler] CTRL-C catched" << std::endl;
      #endif


      for (std::vector<GrabberInterface*>::iterator it = mInstances.begin() ; it != mInstances.end();++it)
      {
        //(*it)->~GrabberInterface;  //do that with shared-pointers??

        //stop LoopedThread if needed
        if ((*it)->isRunning())
        {
          (*it)->stop();
          #ifdef DEBUG_GRABBER_INTERFACE
            std::cout << "[GrabberInterface::sigIntHandler] Thread stopped" << std::endl;
          #endif
        }

        //stop Recording if needed
        if ((*it)->mRecord)
        {
          (*it)->stopRecording();
          #ifdef DEBUG_GRABBER_INTERFACE
            std::cout << "[GrabberInterface::sigIntHandler] Recording stopped" << std::endl;
          #endif
        }

        //delete dynamic var's
        if ((*it)->mpReadWriteLock)
        {
          delete (*it)->mpReadWriteLock;
          (*it)->mpReadWriteLock = NULL;
        }

        //do cleanup in derived classes
        (*it)->onDestroy();
      }
      std::exit(1);
  }
#endif

//--------------------------------------------------------------------------------------------------------------------
void GrabberInterface::doInit(unsigned int numCams, const std::string& defaultGrabberName )
{
  mNumCams = numCams;

  #ifdef ENABLE_CTRL_C_HANDLER
     signal(SIGINT,&GrabberInterface::sigIntHandler);
     mInstances.push_back(this);
  #endif

  //-----------------------------
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

  //initialize the snapshot and recording names with default values
  //depends on no. of cameras
  setSnapshotName("snapshot.jpg");
  setRecordName("record.avi");

  //initialize derived class
  if (!onInit())
  {
    CEDAR_THROW(cedar::aux::exc::InitializationException,
                "GrabberInterface::doInit - Error in onInit in derived class");
  }
}


//--------------------------------------------------------------------------------------------------------------------
unsigned int GrabberInterface::getNumCams() const
{
  return mNumCams;
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
    LoopedThread::stop();
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
    LoopedThread::start();
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
bool GrabberInterface::grab()
{
  bool result = true;

  mpReadWriteLock->lockForWrite();        //Lock
  result = onGrab();                      //Grab
  mpReadWriteLock->unlock();              //Unlock

  //recording
  if (mRecord && result)
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
        info << "GrabberInterface::grab[ch" << channel << "]_record - " << e.what();
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
    result = result && saveSnapshot(cam);
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
  //TODO: think about exception
  if (mRecord)
  {
    return true;
  }

  //write the video-file with the actual grabbing-speed
  //this is independet from speed of the avi-file or the camera framerate
  //double fps = getFps();


  //if there is already any
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

  mRecord = true;       //set the record-flag
  return true;
}



//--------------------------------------------------------------------------------------------------------------------
bool GrabberInterface::stopRecording()
{
  if (mRecord)
  {
    mRecord = false;
    mVideoWriterVector.clear();
  }
  return true;
}



//--------------------------------------------------------------------------------------------------------------------
//void GrabberInterface::step(double time  __attribute__ ((__unused__))) //only supported by gcc
void GrabberInterface::step(double)
{
  #ifdef DEBUG_GRABBER_STEP
    std::cout << "GrabberInterface_Thread: step()" << std::endl;
  # endif

  //TODO: think about an exception
  if (!grab())
  {
    //LoopedThread::stop();
  }
}
