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

//----------------------------------------------------------------------------------------------------------------------
GrabberInterface::GrabberInterface(std::string configFileName)
    :   LoopedThread(configFileName),
        mpReadWriteLock(0)
{
#if defined DEBUG_GRABBER_INTERFACE
  std::cout <<"[GrabberInterface::GrabberInterface]" << std::endl;
#endif

  //-----------------------------
  //initialize local member
  mpReadWriteLock = new QReadWriteLock();
  mRecord = false;                          //by default no recording
}


//----------------------------------------------------------------------------------------------------------------------
GrabberInterface::~GrabberInterface()
{

  //stop LoopedThread
  if (QThread::isRunning())
     cedar::aux::LoopedThread::stop();

  //Matrices are released within the vector mImageMatVector

  //write out the parameter of the configurationInterface
  cedar::aux::ConfigurationInterface::writeConfiguration();

  if (mpReadWriteLock)
  {
    delete mpReadWriteLock;
    mpReadWriteLock=NULL;
  }

  #if defined DEBUG_GRABBER_INTERFACE
    std::cout <<"[GrabberInterface::~GrabberInterface]"<< std::endl;
  #endif

}


//----------------------------------------------------------------------------------------------------------------------
void GrabberInterface::doInit(std::string& grabberName,unsigned int numCams)
{
  //-----------------------------
  //restore configuration from configfile

  mNumCams = numCams;

  //Parameters for all Grabber

  if (cedar::aux::ConfigurationInterface::getConfigFileName() == "")
  {
      std::cout << "[GrabberInterface::GrabberInterface] No config filename was given. Use default values." << std::endl;
  }

  //Parameters in the derived classes
  if (!onDeclareParameters())
  {
    CEDAR_THROW(cedar::aux::exc::InitializationException,"GrabberInterface::doInit - Error in onDeclareParameters in derived class");
  }

  cedar::aux::ConfigurationInterface::readOrDefaultConfiguration();

  //the new grabber name should be used, overwrite the restored name
  cedar::aux::ConfigurationInterface::_mName = grabberName;

  //initialize the snapshot and recording names with default values
  //depends on no. of cameras
  setSnapshotName("snapshot.jpg");
  setRecordName("record.avi");

  //initialize derived class
  if (!onInit())
  {
    CEDAR_THROW(cedar::aux::exc::InitializationException,"GrabberInterface::doInit - Error in onInit in derived class");
  }

}


//----------------------------------------------------------------------------------------------------------------------
unsigned int GrabberInterface::getNumCams() const
{
    return mNumCams;
} 

//----------------------------------------------------------------------------------------------------------------------
double GrabberInterface::getFps() const
{
	//unsigned int fps = round(1000 / LoopedThread::_mStepSize);
	double fps = 1000. / LoopedThread::_mStepSize;
    return fps;
} 


//----------------------------------------------------------------------------------------------------------------------
void GrabberInterface::setFps(double fps)
{
	bool wasRunning = QThread::isRunning();
	
	if (wasRunning) LoopedThread::stop();
	
	 //cycle time in ms: 1000ms/frames_per_second
	double milliseconds = 1000. / fps;
	
#if defined DEBUG_GRABBER_INTERFACE
	if (wasRunning) 
		std::cout <<"[GrabberInterface::setFps] grabberthread stopped"<< std::endl;
		
	std::cout <<"[GrabberInterface::setFps] new fps "<< fps << std::endl;
	std::cout <<"[GrabberInterface::setFps] stepsize[in milliseconds]: "<< milliseconds << std::endl;
	
 #endif
	
	
	//TODO: check if it was saved in configfile without modified _mStepSize
	LoopedThread::_mStepSize = milliseconds;  //set param for configuration file
	LoopedThread::setStepSize(milliseconds);  //change speed in thread
	
	
	if (wasRunning) LoopedThread::start();
	
#if defined DEBUG_GRABBER_INTERFACE
	if (QThread::isRunning()) 
		std::cout <<"[GrabberInterface::setFps]: grabberthread running"<< std::endl;
		
	std::cout <<"[GrabberInterface::setFps]: getFps "<< getFps() << std::endl;
	std::cout <<"[GrabberInterface::setFps]: stepsize[in milliseconds]: "<< milliseconds << std::endl;
	
 #endif

} 



//----------------------------------------------------------------------------------------------------------------------
bool GrabberInterface::grab()
{
    bool result = true;

    // --------------------- Lock
    mpReadWriteLock->lockForWrite();

    result = onGrab();
    
    // --------------------- Unlock
    mpReadWriteLock->unlock();
    

    //recording
    if (mRecord && result)
    {
      for(unsigned int channel=0; channel<mNumCams;++channel)
      {
        try
        {
          mVideoWriterVector.at(channel) << mImageMatVector.at(channel);
        }
        catch (std::exception& e)
        {
          std::stringstream info;
          info << "GrabberInterface::grab[ch"<< channel <<"]_record - " << e.what();
          CEDAR_THROW(cedar::aux::exc::GrabberRecordingException,info.str());
        }

      }

    }

    return result;
}




//----------------------------------------------------------------------------------------------------------------------
cv::Mat GrabberInterface::getImage(unsigned int channel) const
{
    if (channel >= mNumCams)
    {
      CEDAR_THROW(cedar::aux::exc::IndexOutOfRangeException,"GrabberInterface::getImage");
    }

    if(channel >= mImageMatVector.size() || mImageMatVector.empty())
    {
        std::stringstream ss;
        ss << "cv::Mat with index " << channel << " does not exist.";
        //SZ not allowed to mutate members in const method, just print error to cout
        //mErrorString = ss.str();
        std::cout << ss.str() << std::endl;
        
        return cv::Mat(); //a new empty matrix
    } // if
    
    return mImageMatVector.at(channel);
}




//----------------------------------------------------------------------------------------------------------------------
QReadWriteLock* GrabberInterface::getReadWriteLockPointer() const
{
    return mpReadWriteLock;
}

//----------------------------------------------------------------------------------------------------
std::string GrabberInterface::getPhysicalSourceInformation(unsigned int channel) const
{
    if (channel >= mNumCams)
    {
      CEDAR_THROW(cedar::aux::exc::IndexOutOfRangeException,"GrabberInterface::getPhysicalSourceInformation");
    }	
	return onGetPhysicalSourceInformation(channel);
}


//----------------------------------------------------------------------------------------------------------------------
void GrabberInterface::setSnapshotName(const std::string& snapshotName)
{

    mSnapshotNames.clear();

    //initialize snapshot-names
    std::string name=snapshotName;
    std::string ext;

    //check if extension in filename
    std::size_t pos = name.rfind(".");
    if (pos!=std::string::npos)
     {
        //yes: extract it
        int cnt = pos;
        ext  = name.substr(pos);
        name = name.substr(0,cnt);
     }
    else
    {
      //no: use default
      std::cout << "[GrabberInterface::setSnapshotName] Warning: No extension in filename! Using .jpg\n";
      ext = ".jpg";
      name = snapshotName;
    }

    //filename depends on no. of cams
    if (mNumCams > 1)
    {

      //insert all into vector
      mSnapshotNames.push_back(name + "[ch0]" + ext);
      mSnapshotNames.push_back(name + "[ch1]" + ext);
    }
    else
    {
      mSnapshotNames.push_back(name + ext);
    }
}




//----------------------------------------------------------------------------------------------------------------------
void GrabberInterface::setSnapshotName(unsigned int channel, const std::string& snapshotName )
{
  if (channel >= mNumCams)
  {
    CEDAR_THROW(cedar::aux::exc::IndexOutOfRangeException,"GrabberInterface::setSnapshotName");
  }
  mSnapshotNames.at(channel) = snapshotName;
}




//----------------------------------------------------------------------------------------------------------------------
std::string GrabberInterface::getSnapshotName(unsigned int channel) const
{
  if (channel >= mNumCams)
  {
    CEDAR_THROW(cedar::aux::exc::IndexOutOfRangeException,"GrabberInterface::getSnapshotName");
  }
   return mSnapshotNames.at(channel);
}




//----------------------------------------------------------------------------------------------------------------------
bool GrabberInterface::saveSnapshot(unsigned int channel) const
{
  if (channel >= mNumCams)
  {
    CEDAR_THROW(cedar::aux::exc::IndexOutOfRangeException,"GrabberInterface::saveSnapshot");
  }

  //copy image to local buffer for slow imwrite-function and for error-checking
  cv::Mat imgBuffer;

  if(! mImageMatVector.at(channel).empty())
  {
    // Do the snapshot...
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
      info << "GrabberInterface::saveSnapshot[ch"<< channel <<"] - " << e.what();
      CEDAR_THROW(cedar::aux::exc::GrabberSnapshotException,info.str());
    }

  }
  else
  {
    std::cout << "Nothing to save. Image matrix is empty!\n";
    return false;
  }

  return true;
}





//----------------------------------------------------------------------------------------------------------------------
bool GrabberInterface::saveSnapshotAllCams() const
{
    bool result = true;
    for(unsigned int cam=0; cam<mNumCams; ++cam)
    {
      result = result && saveSnapshot(cam);
    }

    return result;
}




//----------------------------------------------------------------------------------------------------------------------
CvSize GrabberInterface::getSize(unsigned int channel) const
{
  if (channel >= mNumCams)
  {
    CEDAR_THROW(cedar::aux::exc::IndexOutOfRangeException,"GrabberInterface::getSize");
  }
  return mImageMatVector.at(channel).size();
}



//----------------------------------------------------------------------------------------------------------------------
void GrabberInterface::releaseImagesAndCaptures()
{
    // ------- Release all existing Images...
    //automatically done when releasing the image_vector

    // ------- Release all existing captures...
    // automatically done when releasing the capture_vector

    //not really necessary, because of the default-destructors of the vector
    //if (mRecord) stopRecording()
  //std::cout<<"GrabberInterface::releaseImagesAndCaptures\n";

}



//----------------------------------------------------------------------------------------------------------------------
void GrabberInterface::setRecordName (const std::string& recordName)
{
  if (recordName=="")
  {
    return;
  }

  mRecordNames.clear();

  //initialze snapshot-names
  std::string name=recordName;
  std::string ext;

  //check if extension in filename
  std::size_t pos = name.rfind(".");
  if (pos!=std::string::npos)
   {
      //yes: extract it
      int cnt = pos;
      ext  = name.substr(pos);
      name = name.substr(0,cnt);
   }
  else
  {
    //no: use default
    std::cout << "[GrabberInterface::setRecordName] Warning: No extension in filename! Using .avi\n";
    ext = ".avi";
    name = recordName;
  }

  //filename depends on no. of cams
  if (mNumCams > 1)
  {

    //insert all into vector
    mRecordNames.push_back(name + "[ch0]" + ext);
    mRecordNames.push_back(name + "[ch1]" + ext);
  }
  else
  {
    mRecordNames.push_back(name + ext);
  }

}



//----------------------------------------------------------------------------------------------------------------------
void GrabberInterface::setRecordName (unsigned int channel, const std::string& recordName )
{
  if (channel >= mNumCams)
  {
    CEDAR_THROW(cedar::aux::exc::IndexOutOfRangeException,"GrabberInterface::setRecordName");
  }
  mRecordNames.at(channel) = recordName;
}

//----------------------------------------------------------------------------------------------------------------------
std::string GrabberInterface::getRecordName(unsigned int channel) const
{
  if (channel >= mNumCams)
  {
    CEDAR_THROW(cedar::aux::exc::IndexOutOfRangeException,"GrabberInterface::getRecordName");
  }
  return mRecordNames.at(channel);
}



//----------------------------------------------------------------------------------------------------------------------
bool GrabberInterface::startRecording(double fps, int fourcc, bool color)
{
  //TODO: think about exception
  if (mRecord) return true;

  //write the video-file with the actual grabbing-speed
  //this is independet from speed of an avi-file or the camera
  //double fps = getFps();
  
  
  //if there is already any
  mVideoWriterVector.clear();

  for(unsigned int i=0; i<mNumCams;++i)
  {
    cv::VideoWriter writer(mRecordNames.at(i),fourcc,fps,mImageMatVector.at(i).size(),color);

    if (writer.isOpened())
    {
      mVideoWriterVector.push_back(writer);
    }
  }

  if ( mVideoWriterVector.size() != (mNumCams-1) )
  {
    CEDAR_THROW(cedar::aux::exc::GrabberRecordingException,"GrabberInterface::startRecording")
  }

  mRecord = true;	//set the record-flag
  return true;
}



//----------------------------------------------------------------------------------------------------------------------
bool GrabberInterface::stopRecording()
{
  mRecord = false;
  mVideoWriterVector.clear();
  return true;
}



//----------------------------------------------------------------------------------------------------------------------
void GrabberInterface::step(double time  __attribute__ ((__unused__)))
{
#if defined DEBUG_GRABBER_STEP
    std::cout << "GrabberInterface_Thread: step() at "<< time << std::endl;
# endif

  //TODO: think about an exception
  if (! grab())
  {
    //LoopedThread::stop();
  }
}
