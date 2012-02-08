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

    File:        CameraGrabber.cpp

    Maintainer:  Georg.Hartinger
    Email:       georg.hartinger@ini.rub.de
    Date:        2011 08 01

    Description: Implementation of the @em cedar::devices::visual::CameraGrabber class.

    Credits:

======================================================================================================================*/

// LOCAL INCLUDES
#include "cedar/devices/sensors/visual/CameraGrabber.h"
#include "cedar/auxiliaries/exceptions/IndexOutOfRangeException.h"

// PROJECT INCLUDES

// SYSTEM INCLUDES
#include <boost/lexical_cast.hpp> //for reading values from the configfile
#include <boost/math/special_functions/round.hpp> //rounding from double to int in cv::VideoCapture get and set methods


using namespace cv;
using namespace cedar::dev::sensors::visual;

//----------------------------------------------------------------------------------------------------------------------
//constructors and destructor
//----------------------------------------------------------------------------------------------------------------------


//----------------------------------------------------------------------------------------------------
//Constructor for single-file grabber
CameraGrabber::CameraGrabber(
                              const std::string& configFileName,
                              unsigned int camera,
                              bool isGuid,
                              bool finishInitialization
                            )
:
GrabberInterface(configFileName)
//mCreateFromConfigFile(false)
{
  #ifdef DEBUG_CAMERAGRABBER
    std::cout << "[CameraGrabber::CameraGrabber] Create single channel instance" << std::endl;
  #endif

  mFinishInitialization = finishInitialization;
  mCreateGrabberByGuid = isGuid;

  //the lock for concurrent access to the cv::VideoCapture
  QReadWriteLockPtr p_video_capture_lock = QReadWriteLockPtr(new QReadWriteLock());
  mVideoCaptureLocks.push_back(p_video_capture_lock);

  //Camera Id's
  CameraId cam_id = {0,0};
  mCamIds.push_back(cam_id);

  //storage for filenames from config-file
  mCameraCapabilitiesFileNames.push_back("camera_default.capabilities");

  //do all declareParameters and read it from configfile
  readInit(mCamIds.size(),"CameraGrabber");

  //change/overwrite camera-ids with new parameters from constructor here
  mCreateGrabberByGuid = isGuid;
  if (isGuid)
  {
    mCamIds.at(0).guid = camera;
    mCamIds.at(0).busId = 0;
  }
  else
  {
    mCamIds.at(0).guid = 0;
    mCamIds.at(0).busId = camera;
  }

  //now apply the values, also invoke the onInit() member function
  applyInit();
}


//----------------------------------------------------------------------------------------------------
//Constructor for stereo-file grabber
CameraGrabber::CameraGrabber(
                              const std::string& configFileName,
                              unsigned int camera0,
                              unsigned int camera1,
                              bool isGuid,
                              bool finishInitialization
                            )
:
GrabberInterface(configFileName)
//mCreateFromConfigFile(false)
{
  #ifdef DEBUG_CAMERAGRABBER
    std::cout << "[CameraGrabber::CameraGrabber] Create stereo channel instance" << std::endl;
  #endif

  mFinishInitialization = finishInitialization;
  mCreateGrabberByGuid = isGuid;

  //the lock for concurrent access to the cv::VideoCapture
  QReadWriteLockPtr p_video_capture_lock_0 = QReadWriteLockPtr(new QReadWriteLock());
  mVideoCaptureLocks.push_back(p_video_capture_lock_0);
  QReadWriteLockPtr p_video_capture_lock_1 = QReadWriteLockPtr(new QReadWriteLock());
  mVideoCaptureLocks.push_back(p_video_capture_lock_1);

  //Camera Id's
  CameraId cam_id_0 = {0,0};
  mCamIds.push_back(cam_id_0);
  CameraId cam_id_1 = {0,0};
  mCamIds.push_back(cam_id_1);

  //storage for filenames from config-file
  mCameraCapabilitiesFileNames.push_back("camera_default.capabilities");
  mCameraCapabilitiesFileNames.push_back("camera_default.capabilities");

  //do all declareParameters and read it from configfile
  readInit(mCamIds.size(),"CameraGrabber");

  //change/overwrite camera-ids with new parameters from constructor here
  mCreateGrabberByGuid = isGuid;
  if (isGuid)
  {
    mCamIds.at(0).guid = camera0;
    mCamIds.at(1).guid = camera1;
    mCamIds.at(0).busId = 0;
    mCamIds.at(1).busId = 0;
  }
  else
  {
    mCamIds.at(0).guid = 0;
    mCamIds.at(1).guid = 0;
    mCamIds.at(0).busId = camera0;
    mCamIds.at(1).busId = camera1;
  }

  //now apply the values, also invoke the onInit() member function
  applyInit();

}

//----------------------------------------------------------------------------------------------------
//constructor that reads configuration only from configuration file
CameraGrabber::CameraGrabber(
               const std::string& configFileName,
               unsigned int numChannels
             )
:
GrabberInterface(configFileName)
{
  //in case of configuration file, always finish the initialization and grab the first frame
  mFinishInitialization = true;
  //mCreateFromConfigFile = true;

  //will be overwritten with values from configfile
  mCreateGrabberByGuid = false;

  for(unsigned int channel = 0; channel < numChannels; channel++)
  {
    //storage for id's, if there is no configuration file, then create one by one
    CameraId cam_id = {channel,0};
    mCamIds.push_back(cam_id);

    //storage for filenames from config-file
    mCameraCapabilitiesFileNames.push_back("camera_default.capabilities");

    //the lock for concurrent access to the cv::VideoCapture
    QReadWriteLockPtr p_video_capture_lock = QReadWriteLockPtr(new QReadWriteLock());
    mVideoCaptureLocks.push_back(p_video_capture_lock);
  }
  readInit(mCamIds.size(),"CameraGrabber");
  applyInit();
}


//----------------------------------------------------------------------------------------------------
CameraGrabber::~CameraGrabber()
{
  doCleanUp();

  //done by shared_pointer
  // mCamCapabilities
  // mVideoCaptureLocks

  #ifdef DEBUG_CAMERAGRABBER
    std::cout<<"[CameraGrabber::Destructor]"<< std::endl;
  #endif
}


//----------------------------------------------------------------------------------------------------------------------
//methods
//----------------------------------------------------------------------------------------------------------------------


//----------------------------------------------------------------------------------------------------
bool CameraGrabber::onDeclareParameters()
{
  // The cameragrabber stores only 4 values : guid, busId, createbyguid and the capability-filename for the camera
  // the rest will be managed by the CameraConfiguration class

  bool result = true;

  bool byGuid = mCreateGrabberByGuid;
  result = (addParameter(&mCreateGrabberByGuid, "createByGuid",byGuid) == CONFIG_SUCCESS) && result;

  for (unsigned int channel=0; channel<mNumCams; channel++)
  {
    //channel string
    std::string ch = "ch"+boost::lexical_cast<std::string>(channel)+"_";

    //get default value from constructor-value. Value will be overwritten by configuration interface
    //so we store it
    CameraId cam_Id = mCamIds.at(channel);
    result = (addParameter(&mCamIds.at(channel).guid,ch+"guid",cam_Id.guid)== CONFIG_SUCCESS) && result;
    result = (addParameter(&mCamIds.at(channel).busId,ch+"busId",cam_Id.busId) == CONFIG_SUCCESS) && result;

    std::string prop_name = ch+"cameraCapabilityFileName";
    std::string prop_default = CAMERAGRABBER_CAM_CAPABILITIES_FILENAME(cam_Id.guid);
    result = (addParameter(&mCameraCapabilitiesFileNames.at(channel),prop_name,prop_default)== CONFIG_SUCCESS) && result;
  }

  return result;
}

//----------------------------------------------------------------------------------------------------
bool CameraGrabber::onWriteConfiguration()
{
  #ifdef DEBUG_CAMERAGRABBER
    std::cout<<"[CameraGrabber::onWriteConfiguration]"<< std::endl;
  #endif

  bool result = true;

  //write local resources (guid, busid and createByGuid
  //result = ConfigurationInterface::writeConfiguration() && result;
  ConfigurationInterface::writeConfiguration();

  //write properties of the cameras
  for (unsigned int channel=0; channel<mNumCams; channel++)
  {
    result = mCamConfigurations.at(channel)->writeConfiguration() && result;
  }

  #ifdef DEBUG_CAMERAGRABBER
    std::cout<<"[CameraGrabber::onWriteConfiguration] result: "
             << std::boolalpha << result << std::noboolalpha << std::endl;
  #endif

  return result;
}

//----------------------------------------------------------------------------------------------------
bool CameraGrabber::onInit()
{
  //local and/or stored Parameters are already initialized

  #ifdef DEBUG_CAMERAGRABBER
    std::cout<<"[CameraGrabber::onInit]"<< std::endl;
  #endif

  //----------------------------------------
  //open capture one by one, and create storage (cv::Mat) for the frames
  mImageMatVector.clear();
  mVideoCaptures.clear();
  mCamConfigurations.clear();

  #ifdef SHOW_INIT_INFORMATION_CAMERAGRABBER
    std::cout << "CameraGrabber: Initialize Grabber with " << mNumCams << " camera(s) ..." << std::endl;
    for (unsigned int i = 0; i < mNumCams; ++i)
    {
      std::cout << "Channel " << i << ": capture from ";

      if (mCreateGrabberByGuid)
      {
        std::cout << "GUID: "<< mCamIds.at(i).guid << "\n";
      }
      else
      {
        std::cout << "bus-ID " << mCamIds.at(i).busId << "\n";
      }
    }
    std::cout << std::flush;
  #endif

  //if create by grabber id, search all cameras and get the device-nr on the bus
  if (mCreateGrabberByGuid)
  {

    //create all videocaptures needed, but don't open them
    for (unsigned int channel = 0; channel < mNumCams; ++channel)
    {
      cv::VideoCapture capture;
      mVideoCaptures.push_back(capture);
    }


    //search for each channel
    //cv::VideoCapture *capture=NULL;

    // search for the right guid, so open it one by one
    unsigned int cam = 0;
    unsigned int found_cams=0;
    bool all_cams_found = false;
    bool no_more_cams = false;

    //search all cams, i.e. all bus-ids
    #ifdef SHOW_INIT_INFORMATION_CAMERAGRABBER
      std::cout << "Searching for cameras on the bus..." << std::endl;
    #endif

    while (! (all_cams_found || no_more_cams))
    {
      //open one by one
      cv::VideoCapture capture(cam);
      if (capture.isOpened())
      {
        unsigned int guid = capture.get((int)CV_CAP_PROP_GUID);

        #ifdef SHOW_INIT_INFORMATION_CAMERAGRABBER
          std::cout << "\tFound at Bus-Id " << cam << ": GUID " << guid << std::endl;
        #endif

        //check if guid of camera is in our guid-vector
        for (unsigned int channel = 0; channel < mNumCams; ++channel)
        {
          unsigned int test_guid = mCamIds.at(channel).guid;
          if (guid == test_guid )
          {
            //yes: store bus_id and videocapture
            mCamIds.at(channel).busId = cam;
            mVideoCaptures.at(channel) = capture;
            found_cams++;
          }
        }

        //check if all cams found
        if (found_cams == mCamIds.size())
        {
          all_cams_found = true;
          #ifdef SHOW_INIT_INFORMATION_CAMERAGRABBER
            std::cout << "All needed cameras found. Stop searching." << std::endl;
          #endif
        }

      }
      else
      {
        //there are no more cams connected
        no_more_cams = true;
      }

      cam++;
    }

    if (! all_cams_found)
    {
      std::cout << "ERROR: Camera not found!" << std::endl;
              //  << "There are only "
              //  << found_cams << " of " << mNumCams << " available" << std::endl;
      return false; //throws an initialization-exception
    }

   /* #ifdef SHOW_INIT_INFORMATION_CAMERAGRABBER
      std::cout << "Bus mapping:" << std::endl;
      for (unsigned int i = 0; i < mNumCams; ++i)
      {
        std::cout << "bus-ID " << mCamIds.at(i).busId << ": GUID "<< mCamIds.at(i).guid << "\n";
      }
      std::cout << std::flush;
    #endif
  */
  }
  else
  {
  // create by ongoing number
    for (unsigned int channel = 0; channel < mNumCams; ++channel)
    {

      //open the camera
      cv::VideoCapture capture(mCamIds.at(channel).busId);
      if (! capture.isOpened())
      {
        std::cout << "ERROR: Couldn't open camera (Channel " << channel << ")." << std::endl;
        return false; //throws an initialization-exception
      }

      //store it in our vector
      mVideoCaptures.push_back(capture);
    }
  }


  //now initialize all videocaptures
  for (unsigned int channel = 0; channel < mNumCams; ++channel)
   {

    //initialize the image buffer
    cv::Mat frame = cv::Mat();
    mImageMatVector.push_back(frame);

    //if grabber created by busId, guid is zero, so read it
    if (!mCreateGrabberByGuid)
    {
      mCamIds.at(channel).guid = static_cast<unsigned int>
                                 (
                                   mVideoCaptures.at(channel).get(static_cast<int>(CV_CAP_PROP_GUID))
                                 );
    }

    //create and load the camera capabilities
    std::string conf_file_name = ConfigurationInterface::getConfigFileName();
    std::string cap_file_name = mCameraCapabilitiesFileNames.at(channel);
    std::string channel_prefix = "ch"+boost::lexical_cast<std::string>(channel) + "_";

    try
    {
      CameraConfigurationPtr cam_conf( new CameraConfiguration
                                           (
                                             mVideoCaptures.at(channel),
                                             mVideoCaptureLocks.at(channel),
                                             channel_prefix,
                                             conf_file_name,
                                             cap_file_name
                                           )
                                     );
      mCamConfigurations.push_back(cam_conf);
    }
    catch (...)
    {
      return false; //throws an initialization exception
    }

  }


  //Check if initialization should be finished.
  if (mFinishInitialization)
  {
    for (unsigned int i = 0; i < mNumCams; ++i)
    {
      mVideoCaptures.at(i) >> mImageMatVector.at(i);
      if (mImageMatVector.at(i).empty())
      {
        std::cout << "ERROR: Couldn't retrieve frame from camera (Channel " << i << ")." << std::endl;
        return false; //throws an initialization-exception
      }
    }
  }

  #ifdef SHOW_INIT_INFORMATION_CAMERAGRABBER
    std::cout << "Camera opened:" << std::endl;
    for (unsigned int i = 0; i < mNumCams; ++i)
    {
      std::cout << "Channel " << i << ": GUID: "<< mCamIds.at(i).guid << "\n";
    }
    std::cout << std::flush;
  #endif

  //all grabbers successfully initialized

  //default initialization
  //----------------------------------------
  //check for largest FPS
  double fps = mVideoCaptures.at(0).get(CV_CAP_PROP_FPS);
  for (unsigned int i = 1; i < mNumCams; ++i)
  {
    double fps_test = mVideoCaptures.at(i).get(CV_CAP_PROP_FPS);
    if (fps_test > fps)
    {
      fps = fps_test;
    }
  }

  //set stepsize for LoopedThread
  setFps(fps);

  #ifdef DEBUG_CAMERAGRABBER
    std::cout << "[CameraGrabber::onInit] Initialize... finished" << std::endl;
  # endif

  return true;
} //onInit()


//----------------------------------------------------------------------------------------------------
void CameraGrabber::printAllSettings(unsigned int channel)
{
  if (channel >= mNumCams)
  {
    CEDAR_THROW(cedar::aux::exc::IndexOutOfRangeException,"CameraGrabber::printAllSettings");
  }

  std::cout << "\nAll settings of channel "<< channel<<" :" << std::endl;
  std::string mode = CameraVideoMode::type().get(getCameraSetting(channel,CameraSetting::SETTING_MODE )).name();
  std::string fps = CameraFrameRate::type().get(getCameraSetting(channel,CameraSetting::SETTING_FPS )).name();
  std::string iso = CameraIsoSpeed::type().get(getCameraSetting(channel,CameraSetting::SETTING_ISO_SPEED )).name();
  unsigned int width = getCameraSetting(channel, CameraSetting::SETTING_FRAME_WIDTH);
  unsigned int height = getCameraSetting(channel, CameraSetting::SETTING_FRAME_HEIGHT);

  std::cout << "Camera video mode:\t" << mode << std::endl;
  std::cout << "Camera framerate:\t" << fps << std::endl;
  std::cout << "Camera ISO-speed:\t" << iso << std::endl;
  std::cout << "Camera frame size:\t" << width << " x " << height << std::endl;
}


//----------------------------------------------------------------------------------------------------
void CameraGrabber::printAllProperties(unsigned int channel)
{
  if (channel >= mNumCams)
  {
    CEDAR_THROW(cedar::aux::exc::IndexOutOfRangeException,"CameraGrabber::printAllProperties");
  }

  //write out all properties directly read from cam
  std::cout << "\nAll properties of channel "<< channel<<" :" << std::endl;
  int num_properties = cedar::dev::sensors::visual::CameraProperty::type().list().size();
  for (int i=0; i<num_properties; i++)
  {
    cedar::dev::sensors::visual::CameraProperty::Id prop_id;
    prop_id = cedar::dev::sensors::visual::CameraProperty::type().list().at(i).id();
    std::string prop_name = cedar::dev::sensors::visual::CameraProperty::type().get(prop_id).name();
    std::string prop_description = cedar::dev::sensors::visual::CameraProperty::type().get(prop_id).prettyString();
    int prop_value = getCameraProperty(channel,prop_id);

    std::stringstream prop_value_text;
    if (prop_value == CAMERA_PROPERTY_NOT_SUPPORTED)
    {
      prop_value_text << "CAMERA_PROPERTY_NOT_SUPPORTED";
    }
    else
    {
      prop_value_text << prop_value;

    }

    std::cout << "\tId: " << prop_id
              << "\tName: " << prop_name
              << "\tValue: " << prop_value_text.str()
              << "\tDescription: " << prop_description
              << std::endl;
  }
/*
  #ifdef DEBUG_CAMERAGRABBER
    //show internal stored values
    std::cout << "\nInternal stored values :\n";
    std::map<unsigned int,int>::iterator it;

    for ( it=mCamProperties.at(channel).begin(); it != mCamProperties.at(channel).end(); it++ )
    {
      cedar::aux::EnumId param_id = static_cast<cedar::aux::EnumId>(it->first);
      std::stringstream param_name;
      param_name << "ch" << channel << "_"+CameraProperty::type().get(param_id).name();

      int param_value = it->second;
      std::cout << "Param: " << param_id << "\tName: " << param_name.str() << "\tValue: " << param_value << std::endl;
    }
  #endif
  */
}



//----------------------------------------------------------------------------------------------------
bool CameraGrabber::onGrab()
{
  int result = true;

  //grab and retrieve
  /*
   * for(unsigned int i = 0; i < mNumCams; ++i)
   * {
   * (mVideoCaptures.at(i))>> mImageMatVector.at(i);
   * }
   * if (mVideoCaptures.at(i).empty)
   * {
   * return false
   * }
   */

  //OpenCV documentation:
  //for better synchronizing between the cameras,
  //first grab internally in camera
  //lock for concurrent access in the grabber-thread and in the get/set properties
  for (unsigned int i = 0; i < mNumCams; ++i)
  {
    mVideoCaptureLocks.at(i)->lockForWrite();
    result = mVideoCaptures.at(i).grab() && result;
    mVideoCaptureLocks.at(i)->unlock();
  }

  //and then retrieve the frames
  if (result)
  {
    for (unsigned int i = 0; i < mNumCams; ++i)
    {
      mVideoCaptureLocks.at(i)->lockForWrite();
      result = mVideoCaptures.at(i).retrieve(mImageMatVector.at(i)) && result;
      mVideoCaptureLocks.at(i)->unlock();
    }
  }

  return result;
}


//----------------------------------------------------------------------------------------------------
std::string CameraGrabber::onGetSourceInfo(unsigned int channel) const
{
  //value of channel is already checked by getSourceInfo
  //if (channel >= mNumCams)
  //{
  //  CEDAR_THROW(cedar::aux::exc::IndexOutOfRangeException,"CameraGrabber::onGetSourceInfo");
  //}

  //std::string cam_mode = CameraVideoMode::type().get(getCameraSetting(channel,CameraSetting::SETTING_MODE )).name();

  std::stringstream s;
  s << "Camera channel " << channel
    << ": DeviceID: " << mCamIds.at(channel).busId
    << ", GUID> " << mCamIds.at(channel).guid;
    //<< ", Mode: " << CameraVideoMode::type().get(getCameraMode(channel)).name();
  return s.str();
}

//----------------------------------------------------------------------------------------------------
void CameraGrabber::onCleanUp()
{
  #ifdef DEBUG_CAMERAGRABBER
    std::cout<<"[CameraGrabber::onCleanUp]"<< std::endl;
  #endif

  //close all captures
  mVideoCaptures.clear();
}

//----------------------------------------------------------------------------------------------------
unsigned int CameraGrabber::getCameraGuid(unsigned int channel)
{
  if (channel >= mNumCams)
  {
    CEDAR_THROW(cedar::aux::exc::IndexOutOfRangeException,"CameraGrabber::getCameraProperty");
  }
  //return mVideoCaptures.at(channel).get((int)CV_CAP_PROP_GUID);
  return mCamIds.at(channel).guid;
}
//----------------------------------------------------------------------------------------------------
double CameraGrabber::getCameraProperty(unsigned int channel,CameraProperty::Id propId)
{
  if (channel >= mNumCams)
  {
    CEDAR_THROW(cedar::aux::exc::IndexOutOfRangeException,"CameraGrabber::getCameraProperty");
  }

  std::string info="";

  if (mCamConfigurations.at(channel)->isSupported(propId))
  {
    if (mCamConfigurations.at(channel)->isReadable(propId))
    {
      return getProperty(channel,static_cast<unsigned int>(propId));
    }
    else
    {
      info = " is not readable";
    }
  }
  else
  {
    info = " is not supported";
  }

  #ifdef ENABLE_GRABBER_WARNING_OUTPUT
    std::cout << "[CameraGrabber::getCameraProperty] property "
              << CameraProperty::type().get(propId).name()
              << info
              << std::endl;
  #endif

  return CAMERA_PROPERTY_NOT_SUPPORTED;
}

//----------------------------------------------------------------------------------------------------
double CameraGrabber::getCameraProperty(CameraProperty::Id propId)
{
  return getCameraProperty(0,propId);
}

//----------------------------------------------------------------------------------------------------
bool CameraGrabber::setCameraProperty(unsigned int channel,CameraProperty::Id propId, double value)
{
  #ifdef DEBUG_CAMERAGRABBER
    std::cout << "[CameraGrabber::setCameraProperty] try to set "
              << CameraProperty::type().get(propId).name() << " to "
              << boost::math::iround(value) << std::endl;
  #endif

    if (channel >= mNumCams)
  {
    CEDAR_THROW(cedar::aux::exc::IndexOutOfRangeException,"CameraGrabber::setCameraProperty");
  }

  int wanted_value = boost::math::iround(value);// static_cast<int>(value);

  #ifdef ENABLE_GRABBER_WARNING_OUTPUT
    std::string prop_name = CameraProperty::type().get(propId).name();
    int prop_id = static_cast<int>(propId);
  #endif

  //------------------------------------------------------
  //check if undefined properties
  if ( (propId == cedar::aux::Enum::UNDEFINED) || (value == CAMERA_PROPERTY_NOT_SUPPORTED))
  {
    #ifdef ENABLE_GRABBER_WARNING_OUTPUT
      std::cout << "[CameraGrabber::setCameraProperty] Undefined or unsupported property \""
                << prop_name << "\" (ID: "<< prop_id << ")" << std::endl;
    #endif

    return false;
  }

  //------------------------------------------------------
  //check if property is supported from the cam
  if (!mCamConfigurations.at(channel)->isSupported(propId))
  {
    #ifdef ENABLE_GRABBER_WARNING_OUTPUT
      std::cout << "[CameraGrabber::setCameraProperty] Property \"" << prop_name
                << "\" unsupported on channel " << channel << std::endl;
    #endif

    return false;
  }

  //------------------------------------------------------
  //check property modes
  if (wanted_value == CAMERA_PROPERTY_MODE_AUTO)
  {
    if (mCamConfigurations.at(channel)->isAutoCapable(propId))
    {
      if (setProperty(channel,prop_id, static_cast<double>(CAMERA_PROPERTY_MODE_AUTO)))
      {
         return true;
      }
      else
      {
        #ifdef ENABLE_GRABBER_WARNING_OUTPUT
        std::cout << "[CameraGrabber::setCameraProperty] Channel "<< channel << ": Property \"" << prop_name
                  << "\" couldn't set to mode \"auto\""  << std::endl;
        #endif
        return false;
      }
    }
    else
    {
      #ifdef ENABLE_GRABBER_WARNING_OUTPUT
      std::cout << "[CameraGrabber::setCameraProperty] Channel "<< channel << ": Property \"" << prop_name
                << "\" doesn't support \"CAMERA_PROPERTY_MODE_AUTO\""  << std::endl;
      #endif
      return false;
    }
  }
  else if (wanted_value == CAMERA_PROPERTY_MODE_OFF)
  {
    if (mCamConfigurations.at(channel)->isOnOffCapable(propId))
    {
      if (setProperty(channel,prop_id, static_cast<double>(CAMERA_PROPERTY_MODE_OFF)))
      {
         return true;
      }
      else
      {
        #ifdef ENABLE_GRABBER_WARNING_OUTPUT
        std::cout << "[CameraGrabber::setCameraProperty] Channel "<< channel << ": Property \"" << prop_name
                  << "\" couldn't switched off"  << std::endl;
        #endif
        return false;
      }
    }
    else
    {
      #ifdef ENABLE_GRABBER_WARNING_OUTPUT
      std::cout << "[CameraGrabber::setCameraProperty] Channel "<< channel << ": Property \"" << prop_name
                << "\" is not on/off capable"  << std::endl;
      #endif
      return false;
    }
  }
  else if (wanted_value == CAMERA_PROPERTY_MODE_ONE_PUSH_AUTO)
  {
    if (mCamConfigurations.at(channel)->isOnePushCapable(propId))
    {
      if (setProperty(channel,prop_id, static_cast<double>(CAMERA_PROPERTY_MODE_ONE_PUSH_AUTO)))
      {
         return true;
      }
      else
      {
        #ifdef ENABLE_GRABBER_WARNING_OUTPUT
        std::cout << "[CameraGrabber::setCameraProperty] Channel "<< channel << ": Property \"" << prop_name
                  << "\" couldn't set to mode \"OnePushAuto\""  << std::endl;
        #endif
        return false;
      }
    }
    else
    {
      #ifdef ENABLE_GRABBER_WARNING_OUTPUT
      std::cout << "[CameraGrabber::setCameraProperty] Channel "<< channel << ": Property \"" << prop_name
                << "\" is not \"one push auto\" capable"  << std::endl;
      #endif
      return false;
    }
  }
  else
  {
    //manual setting of property value
    if (! (mCamConfigurations.at(channel)->isManualCapable(propId)
           || mCamConfigurations.at(channel)->isAbsoluteCapable(propId)
          )
       )
    {
      #ifdef ENABLE_GRABBER_WARNING_OUTPUT
      std::cout << "[CameraGrabber::setCameraProperty] Channel "<< channel << ": Property \"" << prop_name
                << "\" couldn't set to manual mode"  << std::endl;
      #endif
      return false;
    }

    //check lower and upper boundaries for manual settings
    int max_value = mCamConfigurations.at(channel)->getMaxValue(propId);
    int min_value = mCamConfigurations.at(channel)->getMinValue(propId);

    #ifdef ENABLE_GRABBER_WARNING_OUTPUT
      if ( (wanted_value > max_value) || (wanted_value < min_value))
      {
          std::cout << "[CameraGrabber::setCameraProperty] Channel "<< channel << ": Property \"" << prop_name
                    << "\" has a range of [" << min_value << ","<< max_value << "]" << std::endl;
          std::cout << "[CameraGrabber::setCameraProperty] Your value of "<< value
                    << " exceeds this boundaries" << std::endl;
      }
    #endif

    if (value > max_value)
    {
      wanted_value = max_value;
    }
    else if (value < min_value)
    {
      wanted_value = min_value;
    }

    #ifdef ENABLE_GRABBER_WARNING_OUTPUT
      if (wanted_value != boost::math::iround(value))
      {
        std::cout << "[CameraGrabber::setCameraProperty] set " << prop_name << " to " << wanted_value << std::endl;
      }
    #endif


    //check if already set
    int old_value = boost::math::iround(getCameraProperty(channel,propId));
    if ( old_value == wanted_value)
    {
      return true;
    }

    //not set, set it
    int new_value = 0;
    //if (mVideoCaptures.at(channel).set(propId, static_cast<double>(wanted_value))) asdf
    if (setProperty(channel,prop_id, static_cast<double>(wanted_value)))
    {
      //and check if successful
      new_value = boost::math::iround(getCameraProperty(channel,propId));
      if (new_value == wanted_value)
      {
        return true;
      }
    }

    // an error occurred
    #ifdef ENABLE_GRABBER_WARNING_OUTPUT
      std::cout << "[CameraGrabber::setCameraProperty] Couldn't set Property "
                << prop_name
                << " to " << value << ". " << std::endl;
    #endif
    return false;
  }
}

//----------------------------------------------------------------------------------------------------
bool CameraGrabber::setCameraProperty(CameraProperty::Id propId, double value)
{
  return setCameraProperty(0,propId, value);
}


//----------------------------------------------------------------------------------------------------
bool CameraGrabber::setCameraSetting( unsigned int channel, CameraSetting::Id settingId, double value)
{
  if (channel >= mNumCams)
  {
    CEDAR_THROW(cedar::aux::exc::IndexOutOfRangeException,"CameraGrabber::setCameraSetting");
  }

  unsigned int setting_id = static_cast<unsigned int>(settingId);
  #ifdef ENABLE_GRABBER_WARNING_OUTPUT
    std::string setting_name = CameraSetting::type().get(settingId).name();
  #endif

  //check if undefined settings
  if ( (settingId == cedar::aux::Enum::UNDEFINED) )
  {
    #ifdef ENABLE_GRABBER_WARNING_OUTPUT
      std::cout << "[CameraGrabber::setCameraSetting] Channel "<< channel
                << ": Undefined or unsupported settings \""
                << setting_name << "\" (ID: "<< setting_id << ")" << std::endl;
    #endif
    return false;
  }


  #ifdef ENABLE_GRABBER_WARNING_OUTPUT
    if( !getImage(channel).empty() )
    {
      std::cout << "[CameraGrabber::setCameraSetting] Camera is already in grabbing mode. "
                << "Settings can only be changed on initialization of the grabber."<<std::endl
                << "To take changes effect, you have to save your settings (writeConfiguration()) "
                << "and then restart (i.e delete and create a new one) your grabber."<<std::endl;
    }
  #endif
  //return mVideoCaptures.at(channel).set(setting_id, value);
  return setProperty(channel,setting_id, value);
}


//----------------------------------------------------------------------------------------------------
double CameraGrabber::getCameraSetting( unsigned int channel, CameraSetting::Id settingId)
{
  if (channel >= mNumCams)
  {
    CEDAR_THROW(cedar::aux::exc::IndexOutOfRangeException,"CameraGrabber::getCameraSetting");
  }

  unsigned int setting_id = static_cast<unsigned int>(settingId);
  #ifdef ENABLE_GRABBER_WARNING_OUTPUT
    std::string setting_name = CameraSetting::type().get(settingId).name();
  #endif

  //check if undefined settings
  if ( (settingId == cedar::aux::Enum::UNDEFINED) )
  {
    #ifdef ENABLE_GRABBER_WARNING_OUTPUT
      std::cout << "[CameraGrabber::getCameraSetting] Channel "<< channel
                << ": Undefined or unsupported settings \""
                << setting_name << "\" (ID: "<< setting_id << ")" << std::endl;
    #endif
    return -1;
  }

  //return mVideoCaptures.at(channel).get(setting_id);
  return getProperty(channel,setting_id);
}

//----------------------------------------------------------------------------------------------------
bool CameraGrabber::setCameraMode(unsigned int channel, CameraVideoMode::Id modeId)
{
  return setCameraSetting(channel,CameraSetting::SETTING_MODE,static_cast<unsigned int>(modeId));
}


//----------------------------------------------------------------------------------------------------
CameraVideoMode::Id CameraGrabber::getCameraMode(unsigned int channel)
{
  return static_cast<CameraVideoMode::Id> (getCameraSetting(channel,CameraSetting::SETTING_MODE));
}

//----------------------------------------------------------------------------------------------------
bool CameraGrabber::setCameraFps(unsigned int channel,CameraFrameRate::Id fpsId)
{
  return setCameraSetting(channel,CameraSetting::SETTING_FPS,static_cast<unsigned int>(fpsId));
}


//----------------------------------------------------------------------------------------------------
CameraFrameRate::Id CameraGrabber::getCameraFps(unsigned int channel)
{
  return static_cast<CameraVideoMode::Id> (getCameraSetting(channel,CameraSetting::SETTING_FPS));
}

//----------------------------------------------------------------------------------------------------
bool CameraGrabber::setCameraIsoSpeed( unsigned int channel,CameraIsoSpeed::Id isoSpeedId )
{
  return setCameraSetting(channel,CameraSetting::SETTING_ISO_SPEED, static_cast<unsigned int>(isoSpeedId));
}


//----------------------------------------------------------------------------------------------------
CameraIsoSpeed::Id CameraGrabber::getCameraIsoSpeed(unsigned int channel)
{
  return static_cast<CameraVideoMode::Id> (getCameraSetting(channel,CameraSetting::SETTING_ISO_SPEED));
}

//----------------------------------------------------------------------------------------------------
cv::Size CameraGrabber::getCameraFrameSize( unsigned int channel)
{
  cv::Size size;
  size.width = boost::math::iround(getCameraSetting(channel,CameraSetting::SETTING_FRAME_WIDTH));
  size.height = boost::math::iround(getCameraSetting(channel,CameraSetting::SETTING_FRAME_HEIGHT));
  return size;
}


//----------------------------------------------------------------------------------------------------
bool CameraGrabber::setProperty(unsigned int channel, unsigned int prop_id, double value)
{
  bool result;
  mVideoCaptureLocks.at(channel)->lockForWrite();
  result = mVideoCaptures.at(channel).set(prop_id,value);
  mVideoCaptureLocks.at(channel)->unlock();

  return result;
}

//----------------------------------------------------------------------------------------------------
double CameraGrabber::getProperty(unsigned int channel, unsigned int prop_id)
{
  double value;
  mVideoCaptureLocks.at(channel)->lockForWrite();
  value = mVideoCaptures.at(channel).get(prop_id);
  mVideoCaptureLocks.at(channel)->unlock();

  return value;
}
