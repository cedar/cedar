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

// CEDAR INCLUDES
#include "cedar/devices/sensors/visual/CameraGrabber.h"
#include "cedar/auxiliaries/exceptions/IndexOutOfRangeException.h"
#include "cedar/auxiliaries/utilities.h"

// SYSTEM INCLUDES
#include <boost/lexical_cast.hpp>
#include <boost/math/special_functions/round.hpp> //rounding from double to int in cv::VideoCapture get and set methods


//----------------------------------------------------------------------------------------------------------------------
//constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------
//Constructor for single-file grabber
cedar::dev::sensors::visual::CameraGrabber::CameraGrabber
(
  const std::string& configFileName,
  unsigned int camera,
  bool isGuid,
  bool finishInitialization
)
:
cedar::dev::sensors::visual::GrabberInterface(configFileName)
//mCreateFromConfigFile(false)
{
  #ifdef DEBUG_CAMERAGRABBER
    std::cout << "[cedar::dev::sensors::visual::CameraGrabber::CameraGrabber] Create single channel instance" << std::endl;
  #endif

  mFinishInitialization = finishInitialization;
  mCreateGrabberByGuid = isGuid;

  //declareParameters and read it from configfile
  readInit(1,"CameraGrabber");

  //change/overwrite camera-ids with new parameters from constructor here
  mCreateGrabberByGuid = isGuid;
  setChannelId(0,camera,isGuid);

  //now apply the values, also invoke the onInit() member function
  applyInit();

  //and read the info from cv::VideoCapture
  setChannelInfo(0);

  #ifdef DEBUG_CAMERAGRABBER
    std::cout << "[cedar::dev::sensors::visual::CameraGrabber::CameraGrabber] Update configuration file" << std::endl;
  #endif

  cedar::aux::ConfigurationInterface::writeConfiguration();
}

//----------------------------------------------------------------------------------------------------
//Constructor for stereo-file grabber
cedar::dev::sensors::visual::CameraGrabber::CameraGrabber
               (
                 const std::string& configFileName,
                 unsigned int camera0,
                 unsigned int camera1,
                 bool isGuid,
                 bool finishInitialization
               )
:
cedar::dev::sensors::visual::GrabberInterface(configFileName)
{
  #ifdef DEBUG_CAMERAGRABBER
    std::cout << "[cedar::dev::sensors::visual::CameraGrabber::CameraGrabber] Create stereo channel instance"
              << std::endl;
  #endif

  mFinishInitialization = finishInitialization;
  mCreateGrabberByGuid = isGuid;

  //declareParameters and read it from configfile
  readInit(2,"CameraGrabber");

  //change/overwrite camera-ids with new parameters from constructor here
  setChannelId(0,camera0,isGuid);
  setChannelId(1,camera1,isGuid);

  //now apply the values, also invoke the onInit() member function
  applyInit();

  //add channel info
  setChannelInfo(0);
  setChannelInfo(1);

  #ifdef DEBUG_CAMERAGRABBER
    std::cout << "cam0: guid " << getChannel(0)->camId.guid << " busid: " << getChannel(0)->camId.busId << std::endl;
    std::cout << "cam1: guid " << getChannel(1)->camId.guid << " busid: " << getChannel(1)->camId.busId << std::endl;
    std::cout << "[cedar::dev::sensors::visual::CameraGrabber::CameraGrabber] Update configuration file" << std::endl;
  #endif
  cedar::aux::ConfigurationInterface::writeConfiguration();
}

//----------------------------------------------------------------------------------------------------
//constructor that reads configuration only from configuration file
cedar::dev::sensors::visual::CameraGrabber::CameraGrabber
               (
                 const std::string& configFileName,
                 unsigned int numChannels
               )
:
cedar::dev::sensors::visual::GrabberInterface(configFileName)
{
  //in case of configuration file, always finish the initialization and grab the first frame
  mFinishInitialization = true;

  //will be overwritten with values from configfile
  mCreateGrabberByGuid = false;

  readInit(numChannels,"CameraGrabber");
  applyInit();

  //add channel info
  for (unsigned int channel = 0; channel < mNumCams; channel++)
  {
    setChannelInfo(channel);
  }

  #ifdef DEBUG_CAMERAGRABBER
    std::cout << "[cedar::dev::sensors::visual::CameraGrabber::CameraGrabber] Update configuration file" << std::endl;
  #endif

  cedar::aux::ConfigurationInterface::writeConfiguration();
}

//----------------------------------------------------------------------------------------------------
cedar::dev::sensors::visual::CameraGrabber::~CameraGrabber()
{
  doCleanUp();

  #ifdef DEBUG_CAMERAGRABBER
    std::cout<<"[cedar::dev::sensors::visual::CameraGrabber::Destructor]"<< std::endl;
  #endif
}


//----------------------------------------------------------------------------------------------------------------------
//methods
//----------------------------------------------------------------------------------------------------------------------


//----------------------------------------------------------------------------------------------------
void cedar::dev::sensors::visual::CameraGrabber::onAddChannel()
{
  //create the channel structure

  CameraChannelPtr channel(new CameraChannel);

   //the lock for concurrent access to the cv::VideoCapture
   QReadWriteLock *p_video_capture_lock = new QReadWriteLock();
   channel->pVideoCaptureLock = p_video_capture_lock;

   //Camera Id's
   CameraId cam_id = {0,0};
   channel->camId = cam_id;

   //storage for filenames for camera capabilities from config-file
   channel->camCapabilitiesFileName = useAutogeneratedFilenameString();

   //default constructor is invoked for the rest:
   //channelInfo, videoCapture, videoCaptureLock, camStateAndConfig, camCapabilitiesFileName

   mChannels.push_back(channel);
}

//----------------------------------------------------------------------------------------------------
void cedar::dev::sensors::visual::CameraGrabber::setChannelInfo(unsigned int channel)
{
  //add channel info
  std::stringstream s;
  s << "Camera channel 0"
    << ": DeviceID: " << getChannel(channel)->camId.busId
    << ", GUID: " << getChannel(channel)->camId.guid
    << " (0x"<<std::hex<<getChannel(channel)->camId.guid<<")"<<std::dec
    << ", Mode: " << CameraVideoMode::type().get(this->getCameraMode(0)).name();
  getChannel(channel)->channelInfo = s.str();
}

//----------------------------------------------------------------------------------------------------
void cedar::dev::sensors::visual::CameraGrabber::setChannelId(unsigned int channel, unsigned int id, bool isGuid)
{
  if (isGuid)
  {
    getChannel(channel)->camId.guid = id;
    getChannel(channel)->camId.busId = 0;
  }
  else
  {
    getChannel(channel)->camId.guid = 0;
    getChannel(channel)->camId.busId = id;
  }
}
//----------------------------------------------------------------------------------------------------
bool cedar::dev::sensors::visual::CameraGrabber::onDeclareParameters()
{
  // The cameragrabber stores only 4 values : guid, busId, createbyguid and the capability-filename for the camera
  // the rest will be managed by the CameraStateAndConfig class

  bool result = true;

  bool byGuid = mCreateGrabberByGuid;
  result = (addParameter(&mCreateGrabberByGuid, "createByGuid",byGuid) == CONFIG_SUCCESS) && result;

  for (unsigned int channel=0; channel<mNumCams; channel++)
  {
    //channel string
    std::string ch = "ch"+boost::lexical_cast<std::string>(channel)+"_";

    //get default value from constructor-value. Value will be overwritten by configuration interface
    //so we store it
    CameraId cam_Id = getChannel(channel)->camId;
    result = (addParameter(&getChannel(channel)->camId.guid,ch+"guid",cam_Id.guid)== CONFIG_SUCCESS) && result;
    result = (addParameter(&getChannel(channel)->camId.busId,ch+"busId",cam_Id.busId) == CONFIG_SUCCESS) && result;

    std::string prop_name = ch+"cameraCapabilityFileName";
    std::string prop_default = useAutogeneratedFilenameString();
    result = 
      (addParameter(&getChannel(channel)->camCapabilitiesFileName,prop_name,prop_default) == CONFIG_SUCCESS) && result;
  }
  return result;
}

//----------------------------------------------------------------------------------------------------
bool cedar::dev::sensors::visual::CameraGrabber::onWriteConfiguration()
{
  #ifdef DEBUG_CAMERAGRABBER
    std::cout<<"[cedar::dev::sensors::visual::CameraGrabber::onWriteConfiguration]"<< std::endl;
  #endif

  //write local resources (guid, busid and createByGuid
    cedar::aux::ConfigurationInterface::writeConfiguration();

  //write properties of the cameras
  bool result = true;
  for (unsigned int channel=0; channel<mNumCams; channel++)
  {
    result = getChannel(channel)->camStateAndConfig->saveConfiguration() && result;
  }

  #ifdef DEBUG_CAMERAGRABBER
    std::cout<<"[cedar::dev::sensors::visual::CameraGrabber::onWriteConfiguration] result: "
             << std::boolalpha << result << std::noboolalpha << std::endl;
  #endif
  return result;
}

//----------------------------------------------------------------------------------------------------
bool cedar::dev::sensors::visual::CameraGrabber::onInit()
{
  //local and/or stored Parameters are already initialized

  #ifdef DEBUG_CAMERAGRABBER
    std::cout<<"[cedar::dev::sensors::visual::CameraGrabber::onInit]"<< std::endl;
  #endif

  //----------------------------------------
  //open capture one by one

  #ifdef SHOW_INIT_INFORMATION_CAMERAGRABBER
    std::cout << "CameraGrabber: Initialize Grabber with " << mNumCams << " camera(s) ..." << std::endl;
    for (unsigned int i = 0; i < mNumCams; ++i)
    {
      std::cout << "Channel " << i << ": capture from ";

      if (mCreateGrabberByGuid)
      {
        std::cout << "GUID: "<< getChannel(i)->camId.guid << "\n";
      }
      else
      {
        std::cout << "bus-ID " << getChannel(i)->camId.busId << "\n";
      }
    }
    std::cout << std::flush;
  #endif

  //if create by grabber id, search all cameras and get the device-nr on the bus
  if (mCreateGrabberByGuid)
  {
    //all videocaptures already created, but not opened

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
          unsigned int test_guid = getChannel(channel)->camId.guid;
          if (guid == test_guid )
          {
            //yes: store bus_id and videocapture
            getChannel(channel)->camId.busId = cam;
            getChannel(channel)->videoCapture = capture;
            found_cams++;
          }
        }

        //check if all cams found
        if (found_cams == mNumCams)
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
  }
  else
  {
  // create by ongoing number
    for (unsigned int channel = 0; channel < mNumCams; ++channel)
    {
      //open the camera
      cv::VideoCapture capture(getChannel(channel)->camId.busId);
      if (! capture.isOpened())
      {
        std::cout << "ERROR: Couldn't open camera (Channel " << channel << ")." << std::endl;
        return false; //throws an initialization-exception
      }

      //store it in our vector
      getChannel(channel)->videoCapture = capture;
    }
  }

  //now initialize all videocaptures
  for (unsigned int channel = 0; channel < mNumCams; ++channel)
   {

    //if grabber created by busId, guid is zero, so read it
    if (!mCreateGrabberByGuid)
    {
      getChannel(channel)->camId.guid =
        static_cast<unsigned int>(getChannel(channel)->videoCapture.get(CV_CAP_PROP_GUID));
    }

    //create and load the camera capabilities
    std::string conf_file_name = 
      ConfigurationInterface::getConfigFileName()+"_ch"+boost::lexical_cast<std::string>(channel);

    //if capabilities filename match with definition for autogenerated filename, create filename
    std::string cap_file_name = getChannel(channel)->camCapabilitiesFileName;
    unsigned int guid = getChannel(channel)->camId.guid;

    if (cap_file_name == useAutogeneratedFilenameString())
    {
      if (guid != 0)
      {
        std::string guid_str = boost::lexical_cast<std::string>(guid);
        size_t guid_found = cap_file_name.find(guid_str);
        if (guid_found == std::string::npos)
        {
          cap_file_name = getCapabilitiesFilename(guid);
        }
      }
      else
      {
        cap_file_name = getCapabilitiesFilename(channel);
      }
    }

    //create camera configuration
    try
    {
      CameraStateAndConfigPtr cam_conf
                              (
                                new CameraStateAndConfig
                                    (
                                      getChannel(channel)->videoCapture,
                                      getChannel(channel)->pVideoCaptureLock,
                                      channel,
                                      conf_file_name,
                                      cap_file_name
                                    )
                              );
      getChannel(channel)->camStateAndConfig = cam_conf;
    }
    catch (...)
    {
      return false; //throws an initialization exception
    }
  }

  //Check if initialization should be finished.
  if (mFinishInitialization)
  {
    for (unsigned int channel = 0; channel < mNumCams; ++channel)
    {
      getChannel(channel)->videoCapture >> getChannel(channel)->imageMat;
      if (getChannel(channel)->imageMat.empty())
      {
        std::cout << "ERROR: Couldn't retrieve frame from camera (Channel " << channel << ")." << std::endl;
        return false; //throws an initialization-exception
      }
    }
  }

  #ifdef SHOW_INIT_INFORMATION_CAMERAGRABBER
    std::cout << "Camera opened:" << std::endl;
    for (unsigned int i = 0; i < mNumCams; ++i)
    {
      std::cout << "Channel " << i << ": GUID: "<< getChannel(i)->camId.guid << "\n";
    }
    std::cout << std::flush;
  #endif

  //all grabbers successfully initialized

  //default initialization
  //----------------------------------------
  //check for largest FPS
  double fps = getChannel(0)->videoCapture.get(CV_CAP_PROP_FPS);
  for (unsigned int i = 1; i < mNumCams; ++i)
  {
    double fps_test = getChannel(i)->videoCapture.get(CV_CAP_PROP_FPS);
    if (fps_test > fps)
    {
      fps = fps_test;
    }
  }

  //set stepsize for LoopedThread
  setFps(fps);

  #ifdef DEBUG_CAMERAGRABBER
    std::cout << "[cedar::dev::sensors::visual::CameraGrabber::onInit] Initialize... finished" << std::endl;
  # endif

  return true;
} //onInit()


//----------------------------------------------------------------------------------------------------
std::vector<std::string> cedar::dev::sensors::visual::CameraGrabber::getAllSettings(unsigned int channel)
{
  if (channel >= mNumCams)
  {
    CEDAR_THROW
    (
      cedar::aux::exc::IndexOutOfRangeException,
      "cedar::dev::sensors::visual::CameraGrabber::printAllSettings"
    );
  }

  std::vector<std::string> settings;
  settings.push_back("All settings of channel " + boost::lexical_cast<std::string>(channel) + ":");

  std::string mode = CameraVideoMode::type().get(getCameraSetting(channel,CameraSetting::SETTING_MODE )).name();
  std::string fps = CameraFrameRate::type().get(getCameraSetting(channel,CameraSetting::SETTING_FPS )).name();
  std::string iso = CameraIsoSpeed::type().get(getCameraSetting(channel,CameraSetting::SETTING_ISO_SPEED )).name();
  unsigned int width = getCameraSetting(channel, CameraSetting::SETTING_FRAME_WIDTH);
  unsigned int height = getCameraSetting(channel, CameraSetting::SETTING_FRAME_HEIGHT);
  std::string size = boost::lexical_cast<std::string>(width) + " x "+ boost::lexical_cast<std::string>(height);

  settings.push_back("Camera video mode:\t" + mode);
  settings.push_back("Camera framerate:\t" + fps);
  settings.push_back("Camera ISO-speed:\t" + iso);
  settings.push_back("Camera frame size:\t" + size);

  return settings;
}

//----------------------------------------------------------------------------------------------------
std::vector<std::string> cedar::dev::sensors::visual::CameraGrabber::getAllProperties(unsigned int channel)
{
  if (channel >= mNumCams)
  {
    CEDAR_THROW
    (
      cedar::aux::exc::IndexOutOfRangeException,
      "cedar::dev::sensors::visual::CameraGrabber::printAllProperties"
    );
  }


  //write out all properties directly read from cam
  std::vector<std::string> properties;
  properties.push_back("All properties of channel " + boost::lexical_cast<std::string>(channel) + ":");

  int num_properties = cedar::dev::sensors::visual::CameraProperty::type().list().size();
  for (int i=0; i<num_properties; i++)
  {
    cedar::dev::sensors::visual::CameraProperty::Id prop_id;
    prop_id = cedar::dev::sensors::visual::CameraProperty::type().list().at(i).id();
    std::string prop_name = cedar::dev::sensors::visual::CameraProperty::type().get(prop_id).name();
    std::string prop_description = cedar::dev::sensors::visual::CameraProperty::type().get(prop_id).prettyString();
    double prop_value = getCameraProperty(channel,prop_id);
    double prop_raw_value = getCameraPropertyValue(channel,prop_id);


    std::stringstream prop_value_text;
    if (prop_value == CAMERA_PROPERTY_NOT_SUPPORTED)
    {
      prop_value_text << "CAMERA_PROPERTY_NOT_SUPPORTED";
    }
    else
    {
      prop_value_text << prop_value;
    }

    properties.push_back("\tId: " + boost::lexical_cast<std::string>(prop_id));
    properties.push_back("\tName: " + prop_name);
    properties.push_back("\tDescription: " + prop_description);
    properties.push_back("\tValue: " + prop_value_text.str());
    properties.push_back("\tRaw-Value: " + boost::lexical_cast<std::string>(prop_raw_value));
  }

  return properties;
}



//----------------------------------------------------------------------------------------------------
bool cedar::dev::sensors::visual::CameraGrabber::onGrab()
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
  for (unsigned int channel = 0; channel < mNumCams; ++channel)
  {
    getChannel(channel)->pVideoCaptureLock->lockForWrite();
    result = getChannel(channel)->videoCapture.grab() && result;
    getChannel(channel)->pVideoCaptureLock->unlock();
  }

  //and then retrieve the frames
  if (result)
  {
    for (unsigned int channel = 0; channel < mNumCams; ++channel)
    {
      getChannel(channel)->pVideoCaptureLock->lockForWrite();
      result = getChannel(channel)->videoCapture.retrieve(getChannel(channel)->imageMat) && result;
      getChannel(channel)->pVideoCaptureLock->unlock();
    }
  }
  return result;
}


//----------------------------------------------------------------------------------------------------
const std::string& cedar::dev::sensors::visual::CameraGrabber::onGetSourceInfo(unsigned int channel) const
{
  //value of channel is already checked by getSourceInfo
  
  return getChannel(channel)->channelInfo;
}

//----------------------------------------------------------------------------------------------------
void cedar::dev::sensors::visual::CameraGrabber::onCleanUp()
{
  #ifdef DEBUG_CAMERAGRABBER
    std::cout<<"[cedar::dev::sensors::visual::CameraGrabber::onCleanUp]"<< std::endl;
  #endif

  //close all captures
  //mChannels.clear(); done in GrabberInterface
}

//----------------------------------------------------------------------------------------------------
unsigned int cedar::dev::sensors::visual::CameraGrabber::getCameraGuid(unsigned int channel)
{
  if (channel >= mNumCams)
  {
    CEDAR_THROW
    (
      cedar::aux::exc::IndexOutOfRangeException,
      "cedar::dev::sensors::visual::CameraGrabber::getCameraProperty"
    );
  }
  return getChannel(channel)->camId.guid;
}

//----------------------------------------------------------------------------------------------------
double cedar::dev::sensors::visual::CameraGrabber::getCameraProperty(unsigned int channel,CameraProperty::Id propId)
{
  if (channel >= mNumCams)
  {
    CEDAR_THROW
    (
      cedar::aux::exc::IndexOutOfRangeException,
      "cedar::dev::sensors::visual::CameraGrabber::getCameraProperty"
    );
  }

  std::string info="";

  if (getChannel(channel)->camStateAndConfig->isSupported(propId))
  {
    if (getChannel(channel)->camStateAndConfig->isReadable(propId))
    {
      return getChannel(channel)->camStateAndConfig->getProperty(static_cast<unsigned int>(propId));
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
    std::cout << "[cedar::dev::sensors::visual::CameraGrabber::getCameraProperty] property "
              << CameraProperty::type().get(propId).name()
              << info
              << std::endl;
  #endif
  return CAMERA_PROPERTY_NOT_SUPPORTED;
}


//----------------------------------------------------------------------------------------------------
double cedar::dev::sensors::visual::CameraGrabber::getCameraPropertyValue
(
  unsigned int channel,
  CameraProperty::Id propId
)
{
  if (channel >= mNumCams)
  {
    CEDAR_THROW
    (
      cedar::aux::exc::IndexOutOfRangeException,
      "cedar::dev::sensors::visual::CameraGrabber::getCameraProperty"
    );
  }

  std::string info="";

  if (getChannel(channel)->camStateAndConfig->isSupported(propId))
  {
    if (getChannel(channel)->camStateAndConfig->isReadable(propId))
    {
      return getChannel(channel)->camStateAndConfig->getPropertyValue(static_cast<unsigned int>(propId));
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
    std::cout << "[cedar::dev::sensors::visual::CameraGrabber::getCameraProperty] property "
              << CameraProperty::type().get(propId).name()
              << info
              << std::endl;
  #endif
  return CAMERA_PROPERTY_NOT_SUPPORTED;
}

//----------------------------------------------------------------------------------------------------
double cedar::dev::sensors::visual::CameraGrabber::getCameraProperty(CameraProperty::Id propId)
{
  return getCameraProperty(0,propId);
}

//----------------------------------------------------------------------------------------------------
bool cedar::dev::sensors::visual::CameraGrabber::setCameraProperty
(
  unsigned int channel,
  CameraProperty::Id propId,
  double value
)
{
  #ifdef DEBUG_CAMERAGRABBER
    std::cout << "[cedar::dev::sensors::visual::CameraGrabber::setCameraProperty] try to set "
              << CameraProperty::type().get(propId).name() << " to "
              << boost::math::iround(value) << std::endl;
  #endif

  if (channel >= mNumCams)
  {
    CEDAR_THROW
    (
      cedar::aux::exc::IndexOutOfRangeException,
      "cedar::dev::sensors::visual::CameraGrabber::setCameraProperty"
    );
  }
  return getChannel(channel)->camStateAndConfig->setProperty(propId, value);
}

//----------------------------------------------------------------------------------------------------
bool cedar::dev::sensors::visual::CameraGrabber::setCameraProperty(CameraProperty::Id propId, double value)
{
  return setCameraProperty(0,propId, value);
}

//----------------------------------------------------------------------------------------------------
bool cedar::dev::sensors::visual::CameraGrabber::setCameraSetting( unsigned int channel, CameraSetting::Id settingId, double value)
{
  if (channel >= mNumCams)
  {
    CEDAR_THROW(cedar::aux::exc::IndexOutOfRangeException,"cedar::dev::sensors::visual::CameraGrabber::setCameraSetting");
  }

  unsigned int setting_id = static_cast<unsigned int>(settingId);
  #ifdef ENABLE_GRABBER_WARNING_OUTPUT
    std::string setting_name = CameraSetting::type().get(settingId).name();
  #endif

  //check if undefined settings
  if ( (settingId == cedar::aux::Enum::UNDEFINED) )
  {
    #ifdef ENABLE_GRABBER_WARNING_OUTPUT
      std::cout << "[cedar::dev::sensors::visual::CameraGrabber::setCameraSetting] Channel "<< channel
                << ": Undefined or unsupported settings \""
                << setting_name << "\" (ID: "<< setting_id << ")" << std::endl;
    #endif
    return false;
  }

  #ifdef ENABLE_GRABBER_WARNING_OUTPUT
    if( !getImage(channel).empty() )
    {
      std::cout << "[cedar::dev::sensors::visual::CameraGrabber::setCameraSetting] Camera is already in grabbing mode. "
                << "Settings can only be changed on initialization of the grabber."<<std::endl
                << "To take changes effect, you have to save your settings (writeConfiguration()) "
                << "and then restart (i.e delete and create a new one) your grabber."<<std::endl;
    }
  #endif
  return getChannel(channel)->camStateAndConfig->setSetting(setting_id, value);
}

//----------------------------------------------------------------------------------------------------
double cedar::dev::sensors::visual::CameraGrabber::getCameraSetting( unsigned int channel, CameraSetting::Id settingId)
{
  if (channel >= mNumCams)
  {
    CEDAR_THROW(cedar::aux::exc::IndexOutOfRangeException,"cedar::dev::sensors::visual::CameraGrabber::getCameraSetting");
  }

  unsigned int setting_id = static_cast<unsigned int>(settingId);

  //check if undefined settings
  if ( (settingId == cedar::aux::Enum::UNDEFINED) )
  {
    #ifdef ENABLE_GRABBER_WARNING_OUTPUT
      std::string setting_name = CameraSetting::type().get(settingId).name();
      std::cout << "[cedar::dev::sensors::visual::CameraGrabber::getCameraSetting] Channel "<< channel
                << ": Undefined or unsupported settings \""
                << setting_name << "\" (ID: "<< setting_id << ")" << std::endl;
    #endif
    return -1;
  }
  return getChannel(channel)->camStateAndConfig->getSetting(setting_id);
}

//----------------------------------------------------------------------------------------------------
bool cedar::dev::sensors::visual::CameraGrabber::setCameraMode(unsigned int channel, CameraVideoMode::Id modeId)
{
  return setCameraSetting(channel,CameraSetting::SETTING_MODE,static_cast<unsigned int>(modeId));
}

//----------------------------------------------------------------------------------------------------
cedar::dev::sensors::visual::CameraVideoMode::Id
  cedar::dev::sensors::visual::CameraGrabber::getCameraMode(unsigned int channel)
{
  return static_cast<cedar::dev::sensors::visual::CameraVideoMode::Id>
         (
           getCameraSetting(channel,CameraSetting::SETTING_MODE)
         );
}

//----------------------------------------------------------------------------------------------------
bool cedar::dev::sensors::visual::CameraGrabber::setCameraFps(unsigned int channel,CameraFrameRate::Id fpsId)
{
  return setCameraSetting(channel,CameraSetting::SETTING_FPS,static_cast<unsigned int>(fpsId));
}

//----------------------------------------------------------------------------------------------------
cedar::dev::sensors::visual::CameraFrameRate::Id
  cedar::dev::sensors::visual::CameraGrabber::getCameraFps(unsigned int channel)
{
  return static_cast<cedar::dev::sensors::visual::CameraVideoMode::Id>
         (
           getCameraSetting(channel,CameraSetting::SETTING_FPS)
         );
}

//----------------------------------------------------------------------------------------------------
bool cedar::dev::sensors::visual::CameraGrabber::setCameraIsoSpeed(unsigned int channel,CameraIsoSpeed::Id isoSpeedId)
{
  return setCameraSetting
         (
           channel,
           cedar::dev::sensors::visual::CameraSetting::SETTING_ISO_SPEED,
           static_cast<unsigned int>(isoSpeedId)
         );
}

//----------------------------------------------------------------------------------------------------
cedar::dev::sensors::visual::CameraIsoSpeed::Id
  cedar::dev::sensors::visual::CameraGrabber::getCameraIsoSpeed(unsigned int channel)
{
  return static_cast<CameraVideoMode::Id>
         (
           getCameraSetting(channel, cedar::dev::sensors::visual::CameraSetting::SETTING_ISO_SPEED)
         );
}

//----------------------------------------------------------------------------------------------------
cv::Size cedar::dev::sensors::visual::CameraGrabber::getCameraFrameSize( unsigned int channel)
{
  cv::Size size;
  size.width = boost::math::iround(getCameraSetting(channel,CameraSetting::SETTING_FRAME_WIDTH));
  size.height = boost::math::iround(getCameraSetting(channel,CameraSetting::SETTING_FRAME_HEIGHT));
  return size;
}


