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

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/devices/sensors/visual/CameraGrabber.h"
#include "cedar/auxiliaries/exceptions.h"
#include "cedar/auxiliaries/utilities.h"


// SYSTEM INCLUDES
#include <boost/lexical_cast.hpp>
#include <boost/math/special_functions/round.hpp> //rounding from double to int in cv::VideoCapture get and set methods


//----------------------------------------------------------------------------------------------------------------------
// register the class
//----------------------------------------------------------------------------------------------------------------------
namespace
{
  bool declared
    = cedar::dev::sensors::visual::Grabber::ChannelManagerSingleton::getInstance()
        ->registerType<cedar::dev::sensors::visual::CameraGrabber::CameraChannelPtr>();
}

//----------------------------------------------------------------------------------------------------------------------
//constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------
//Constructor for single-file grabber
cedar::dev::sensors::visual::CameraGrabber::CameraGrabber
(
  unsigned int cameraId,
  bool isGuid,
  cedar::dev::sensors::visual::CameraBackendType::Id backendType,
  const std::string& grabberName
)
:
cedar::dev::sensors::visual::Grabber
(
  grabberName,
  cedar::dev::sensors::visual::CameraGrabber::CameraChannelPtr
  (
    new cedar::dev::sensors::visual::CameraGrabber::CameraChannel(cameraId, isGuid, backendType)
  )
)
{
  cedar::aux::LogSingleton::getInstance()->allocating(this);
  cedar::aux::LogSingleton::getInstance()->debugMessage
                                           (
                                             this->getName() + ": Single channel instance",
                                             "cedar::dev::sensors::visual::CameraGrabber::CameraGrabber()"
                                           );

  mCreateGrabberByGuid = isGuid;

/*
  //declareParameters and read it from configfile
  //readInit(1);

  //change/overwrite camera-ids with new parameters from constructor here
  mCreateGrabberByGuid = isGuid;
  setChannelId(0,camera,isGuid);

  //now apply the values, also invoke the onInit() member function
  //applyInit();

  std::stringstream debug_info;
  debug_info << ": cam0: guid " << getCameraChannel(0)->mCamId.guid << " busid: " << getCameraChannel(0)->mCamId.busId;
  cedar::aux::LogSingleton::getInstance()->debugMessage
                                           (
                                             this->getName() + debug_info.str(),
                                             "cedar::dev::sensors::visual::CameraGrabber::CameraGrabber()"
                                           );
  //cedar::aux::ConfigurationInterface::writeConfiguration();
   *
   */
}

//----------------------------------------------------------------------------------------------------
//Constructor for stereo-file grabber
cedar::dev::sensors::visual::CameraGrabber::CameraGrabber
(
  unsigned int cameraId0,
  unsigned int cameraId1,
  bool isGuid,
  cedar::dev::sensors::visual::CameraBackendType::Id backendType,
  const std::string& grabberName
)
:
cedar::dev::sensors::visual::Grabber
(
  grabberName,
  cedar::dev::sensors::visual::CameraGrabber::CameraChannelPtr
  (
    new cedar::dev::sensors::visual::CameraGrabber::CameraChannel(cameraId0, isGuid, backendType)
  ),
  cedar::dev::sensors::visual::CameraGrabber::CameraChannelPtr
  (
    new cedar::dev::sensors::visual::CameraGrabber::CameraChannel(cameraId1, isGuid, backendType)
  )
)

{
  cedar::aux::LogSingleton::getInstance()->allocating(this);
  cedar::aux::LogSingleton::getInstance()->debugMessage
                                           (
                                             this->getName() + ": Stereo channel instance",
                                             "cedar::dev::sensors::visual::CameraGrabber::CameraGrabber()"
                                           );

  mCreateGrabberByGuid = isGuid;

/*
  mFinishInitialization = finishInitialization;
  mCreateGrabberByGuid = isGuid;

  //declareParameters and read it from configfile
  //readInit(2);

  //change/overwrite camera-ids with new parameters from constructor here
  setChannelId(0,camera0,isGuid);
  setChannelId(1,camera1,isGuid);

  //now apply the values, also invoke the onInit() member function
  //applyInit();

  std::stringstream debug_info;
  debug_info << "Create" << std::endl
             << "\tcam0: guid " << getCameraChannel(0)->mCamId.guid << " busid: " << getCameraChannel(0)->mCamId.busId << std::endl
             << "\tcam1: guid " << getCameraChannel(1)->mCamId.guid << " busid: " + getCameraChannel(1)->mCamId.busId;

  cedar::aux::LogSingleton::getInstance()->debugMessage
                                           (
                                             this->getName() + debug_info.str(),
                                             "cedar::dev::sensors::visual::CameraGrabber::CameraGrabber()"
                                           );
  //cedar::aux::ConfigurationInterface::writeConfiguration();
   *
   */
}

//----------------------------------------------------------------------------------------------------
//constructor that reads configuration only from configuration file
/*cedar::dev::sensors::visual::CameraGrabber::CameraGrabber
               (
                 const std::string& configFileName,
                 unsigned int numChannels
               )
:
cedar::dev::sensors::visual::Grabber(configFileName)
{
  cedar::aux::LogSingleton::getInstance()->allocating(this);
  cedar::aux::LogSingleton::getInstance()->debugMessage
                                           (
                                             this->getName() + ": Created from config-file",
                                             "cedar::dev::sensors::visual::CameraGrabber::CameraGrabber()"
                                           );

  //in case of configuration file, always finish the initialization and grab the first frame
  mFinishInitialization = true;

  //will be overwritten with values from configfile
  mCreateGrabberByGuid = false;

  //readInit(numChannels);
  //applyInit();

  std::stringstream debug_info;
  debug_info << ": ";
  for (unsigned int channel = 0; channel < getNumCams(); channel++)
  {
    debug_info << "cam" << channel
               << ": guid " << getCameraChannel(channel)->mCamId.guid
               << " busid " << getCameraChannel(channel)->mCamId.busId;
  }
  cedar::aux::LogSingleton::getInstance()->debugMessage
                                           (
                                             this->getName() + debug_info.str(),
                                             "cedar::dev::sensors::visual::CameraGrabber::CameraGrabber()"
                                           );

  //cedar::aux::ConfigurationInterface::writeConfiguration();
}
*/

//----------------------------------------------------------------------------------------------------
cedar::dev::sensors::visual::CameraGrabber::~CameraGrabber()
{
  doCleanUp();
  cedar::aux::LogSingleton::getInstance()->freeing(this);
}


//----------------------------------------------------------------------------------------------------------------------
//methods
//----------------------------------------------------------------------------------------------------------------------


//----------------------------------------------------------------------------------------------------
//void cedar::dev::sensors::visual::CameraGrabber::onAddChannel()
//{
//  //create the channel structure
//
//  CameraChannelPtr channel(new CameraChannel);
//
//  //the lock for concurrent access to the cv::VideoCapture
//  QReadWriteLock *p_video_capture_lock = new QReadWriteLock();
//  channel->pmVideoCaptureLock = p_video_capture_lock;
//
//  //Camera Id's
//  CameraId cam_id = {0,0};
//  channel->mCamId = cam_id;
//
//  //storage for filenames for camera capabilities from config-file
//  channel->mCamCapabilitiesFileName = useAutogeneratedFilenameString();
//
//  //default constructor is invoked for the rest:
//  //channelInfo, videoCapture, videoCaptureLock, camStateAndConfig, camCapabilitiesFileName
//  mChannels.push_back(channel);
//}

/*
//----------------------------------------------------------------------------------------------------
void cedar::dev::sensors::visual::CameraGrabber::setChannelId(unsigned int channel, unsigned int id, bool isGuid)
{
  if (isGuid)
  {
    getCameraChannel(channel)->mCamId.guid = id;
    getCameraChannel(channel)->mCamId.busId = 0;
  }
  else
  {
    getCameraChannel(channel)->mCamId.guid = 0;
    getCameraChannel(channel)->mCamId.busId = id;
  }
}
//----------------------------------------------------------------------------------------------------
bool cedar::dev::sensors::visual::CameraGrabber::onDeclareParameters()
{
  // The cameragrabber stores only 4 values : guid, busId, createbyguid and the capability-filename for the camera
  // the rest will be managed by the CameraStateAndConfig class

  bool result = true;

  //!@todo Use this value.
//  bool byGuid = mCreateGrabberByGuid;
  //@todo config:
  //result = (addParameter(&mCreateGrabberByGuid, "createByGuid",byGuid) == CONFIG_SUCCESS) && result;

  for (unsigned int channel=0; channel<getNumCams(); channel++)
  {
    //channel string
    std::string ch = "ch"+boost::lexical_cast<std::string>(channel)+"_";

    //get default value from constructor-value. Value will be overwritten by configuration interface
    //so we store it
    CameraId cam_Id = getCameraChannel(channel)->mCamId;
    //@todo config:
    //result = (addParameter(&getCameraChannel(channel)->mCamId.guid,ch+"guid",cam_Id.guid)== CONFIG_SUCCESS) && result;
    //@todo config:
    //result = (addParameter(&getCameraChannel(channel)->mCamId.busId,ch+"busId",cam_Id.busId) == CONFIG_SUCCESS) && result;

    std::string prop_name = ch+"cameraCapabilityFileName";
    std::string prop_default = useAutogeneratedFilenameString();
    //@todo config:
    //result =
    //  (addParameter(&getCameraChannel(channel)->mCamCapabilitiesFileName,prop_name,prop_default) == CONFIG_SUCCESS) && result;
  }
  return result;
}
*/
/*
//----------------------------------------------------------------------------------------------------
bool cedar::dev::sensors::visual::CameraGrabber::onWriteConfiguration()
{
  //write local resources (guid, busid and createByGuid
  //@todo config:
  //cedar::aux::ConfigurationInterface::writeConfiguration();

  //write properties of the cameras
  bool result = true;
  for (unsigned int channel=0; channel<getNumCams(); channel++)
  {
    result = getCameraChannel(channel)->mCamStateAndConfig->saveConfiguration() && result;
  }
  return result;
}
*/
//----------------------------------------------------------------------------------------------------
bool cedar::dev::sensors::visual::CameraGrabber::onInit()
{

  unsigned int num_cams = getNumCams();

  //local and/or stored Parameters are already initialized

  cedar::aux::LogSingleton::getInstance()->debugMessage
                                           (
                                             this->getName() + ": onInit",
                                             "cedar::dev::sensors::visual::CameraGrabber::onInit()"
                                           );

  //open capture one by one
  std::stringstream init_message;
  init_message << ": Initialize Grabber with " << num_cams << " camera(s) ..." << std::endl;
  for (unsigned int i = 0; i < num_cams; ++i)
  {
    init_message << "Channel " << i << ": capture from ";

    if (mCreateGrabberByGuid)
    {
      init_message << "Camera-GUID: "<< getCameraChannel(i)->mGuid << std::endl;
    }
    else
    {
      init_message << "Bus-ID " << getCameraChannel(i)->mBusId << std::endl;
    }
  }
  cedar::aux::LogSingleton::getInstance()->systemInfo
                                           (
                                             this->getName() + init_message.str(),
                                             "cedar::dev::sensors::visual::CameraGrabber::onInit()"
                                           );

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
    cedar::aux::LogSingleton::getInstance()->debugMessage
                                            (
                                              "Searching for cameras on the bus...",
                                              "cedar::dev::sensors::visual::CameraGrabber::onInit()"
                                            );

    while (! (all_cams_found || no_more_cams))
    {
      //open one by one
      cv::VideoCapture capture(cam);
      if (capture.isOpened())
      {
        unsigned int guid = capture.get((int)CV_CAP_PROP_GUID);

        cedar::aux::LogSingleton::getInstance()->debugMessage
                                                (
                                                  "\tFound at Bus-Id " + boost::lexical_cast<std::string>(cam)
                                                    + ": GUID " + boost::lexical_cast<std::string>(guid),
                                                  "cedar::dev::sensors::visual::CameraGrabber::onInit()"
                                                );

        //check if guid of camera is in our guid-vector
        for (unsigned int channel = 0; channel < num_cams; ++channel)
        {
          unsigned int test_guid = getCameraChannel(channel)->mGuid;
          if (guid == test_guid )
          {
            //yes: store bus_id and videocapture
            getCameraChannel(channel)->mBusId = cam;
            getCameraChannel(channel)->mVideoCapture = capture;
            found_cams++;
          }
        }

        //check if all cams found
        if (found_cams == num_cams)
        {
          all_cams_found = true;
          cedar::aux::LogSingleton::getInstance()->debugMessage
                                                  (
                                                    "All needed cameras found. Stop searching.",
                                                    "cedar::dev::sensors::visual::CameraGrabber::onInit()"
                                                  );
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
      cedar::aux::LogSingleton::getInstance()->error
                                               (
                                                 this->getName() + "At least ore camera not found!",
                                                 "cedar::dev::sensors::visual::CameraGrabber::onInit()"
                                               );
      return false; //throws an initialization-exception
    }
  }
  else
  {
  // create by ongoing number
    for (unsigned int channel = 0; channel < num_cams; ++channel)
    {
      //open the camera
      cv::VideoCapture capture(getCameraChannel(channel)->mBusId);
      if (! capture.isOpened())
      {
        cedar::aux::LogSingleton::getInstance()->error
                                                 (
                                                   this->getName()
                                                     + ": Couldn't open camera (Channel "
                                                     + boost::lexical_cast<std::string>(channel) + ")",
                                                   "cedar::dev::sensors::visual::CameraGrabber::onInit()"
                                                 );
        return false; //throws an initialization-exception
      }

      //store it in our vector
      getCameraChannel(channel)->mVideoCapture = capture;
    }
  }

  //now initialize all videocaptures
  for (unsigned int channel = 0; channel < num_cams; ++channel)
   {

    //if grabber created by busId, guid is zero, so read it
    if (!mCreateGrabberByGuid)
    {
      getCameraChannel(channel)->mGuid =
        static_cast<unsigned int>(getCameraChannel(channel)->mVideoCapture.get(CV_CAP_PROP_GUID));
    }

    //create and load the camera capabilities
    //@todo config:
    std::string conf_file_name = "default_cam_capabilities";
    //conf_file_name = ConfigurationInterface::getConfigFileName()+"_ch"+boost::lexical_cast<std::string>(channel);

    //if capabilities filename match with definition for autogenerated filename, create filename
    std::string cap_file_name = "todo";//getCameraChannel(channel)->mCamCapabilitiesFileName;
    unsigned int guid = getCameraChannel(channel)->mGuid;

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
     /* CameraStateAndConfigPtr cam_conf
                              (
                                new CameraStateAndConfig
                                    (
                                      getCameraChannel(channel)->mVideoCapture,
                                      getCameraChannel(channel)->pmVideoCaptureLock,
                                      channel,
                                      conf_file_name,
                                      cap_file_name
                                    )
                              );
      getCameraChannel(channel)->mCamStateAndConfig = cam_conf;
      */
    }
    catch (...)
    {
      return false; //throws an initialization exception
    }
  }

  //Check if initialization should be finished.
  if (mFinishInitialization)
  {
    for (unsigned int channel = 0; channel < num_cams; ++channel)
    {
      getCameraChannel(channel)->mVideoCapture >> getCameraChannel(channel)->mImageMat;
      if (getCameraChannel(channel)->mImageMat.empty())
      {
        cedar::aux::LogSingleton::getInstance()->error
                                         (
                                           this->getName()
                                             + ": Couldn't retrieve frame from camera (Channel "
                                             + boost::lexical_cast<std::string>(channel) + ")",
                                           "cedar::dev::sensors::visual::CameraGrabber::onInit()"
                                         );
        return false; //throws an initialization-exception
      }
    }
  }

  init_message.clear();
  init_message << "Camera opened:" << std::endl;
  for (unsigned int i = 0; i < num_cams; ++i)
  {
    init_message << "Channel " << i << ": GUID: "<< getCameraChannel(i)->mGuid << "\n";
  }
  cedar::aux::LogSingleton::getInstance()->systemInfo
                                           (
                                             this->getName() + init_message.str(),
                                             "cedar::dev::sensors::visual::CameraGrabber::onInit()"
                                           );
  //all grabbers are now successfully initialized

  //check for largest FPS
  double fps = getCameraChannel(0)->mVideoCapture.get(CV_CAP_PROP_FPS);
  for (unsigned int i = 1; i < getNumCams(); ++i)
  {
    double fps_test = getCameraChannel(i)->mVideoCapture.get(CV_CAP_PROP_FPS);
    if (fps_test > fps)
    {
      fps = fps_test;
    }
  }

  //set stepsize for LoopedThread
  setFps(fps);

  return true;
} //onInit()


//----------------------------------------------------------------------------------------------------
std::vector<std::string> cedar::dev::sensors::visual::CameraGrabber::getAllSettings(unsigned int channel)
{
  if (channel >= getNumCams())
  {
    CEDAR_THROW
    (
      cedar::aux::IndexOutOfRangeException,
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
  if (channel >= getNumCams())
  {
    CEDAR_THROW
    (
      cedar::aux::IndexOutOfRangeException,
      "cedar::dev::sensors::visual::CameraGrabber::printAllProperties"
    );
  }


  //write out all properties as set in our structure as well as the values directly read from cam
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
  unsigned int num_cams = getNumCams();

  //todo: test this for better performance
  //grab and retrieve
  /*
   * for(unsigned int i = 0; i < getNumCams(); ++i)
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

  for(unsigned int channel = 0; channel < num_cams; ++channel)
  {
    getCameraChannel(channel)->mpVideoCaptureLock->lockForWrite();
    result = getCameraChannel(channel)->mVideoCapture.grab() && result;
    getCameraChannel(channel)->mpVideoCaptureLock->unlock();
  }

  //and then retrieve the frames
  if (result)
  {
    for(unsigned int channel = 0; channel < num_cams; ++channel)
    {
      getCameraChannel(channel)->mpVideoCaptureLock->lockForWrite();
      result = getCameraChannel(channel)->mVideoCapture.retrieve(getCameraChannel(channel)->mImageMat) && result;
      getCameraChannel(channel)->mpVideoCaptureLock->unlock();
    }
  }
  return result;
}


//----------------------------------------------------------------------------------------------------
void cedar::dev::sensors::visual::CameraGrabber::onUpdateSourceInfo(unsigned int channel)
{
  //value of channel is already checked by GraberInterface::getSourceInfo()
  std::stringstream s;
  s << "Camera channel 0"
    << ": DeviceID: " << getCameraChannel(channel)->mBusId
    << ", GUID: " << getCameraChannel(channel)->mGuid
    << " (0x"<<std::hex<<getCameraChannel(channel)->mGuid<<")"<<std::dec
    << ", Mode: " << CameraVideoMode::type().get(this->getCameraMode(0)).name();
  getCameraChannel(channel)->mChannelInfo = s.str();
}

//----------------------------------------------------------------------------------------------------
void cedar::dev::sensors::visual::CameraGrabber::onCleanUp()
{
  cedar::aux::LogSingleton::getInstance()->debugMessage
                                           (
                                             this->getName() + ": onCleanUp",
                                             "cedar::dev::sensors::visual::CameraGrabber::onCleanUp()"
                                           );

  //close all captures mChannels.clear(); done in Grabber
}

//----------------------------------------------------------------------------------------------------
unsigned int cedar::dev::sensors::visual::CameraGrabber::getCameraGuid(unsigned int channel)
{
  if (channel >= getNumCams())
  {
    CEDAR_THROW
    (
      cedar::aux::IndexOutOfRangeException,
      "cedar::dev::sensors::visual::CameraGrabber::getCameraProperty"
    );
  }
  return getCameraChannel(channel)->mGuid;
}

//----------------------------------------------------------------------------------------------------
double cedar::dev::sensors::visual::CameraGrabber::getCameraProperty(unsigned int channel,CameraProperty::Id propId)
{
  if (channel >= getNumCams())
  {
    CEDAR_THROW
    (
      cedar::aux::IndexOutOfRangeException,
      "cedar::dev::sensors::visual::CameraGrabber::getCameraProperty"
    );
  }

  std::string info="";

  /*
  if (getCameraChannel(channel)->mCamStateAndConfig->isSupported(propId))
  {
    if (getCameraChannel(channel)->mCamStateAndConfig->isReadable(propId))
    {
      return getCameraChannel(channel)->mCamStateAndConfig->getProperty(static_cast<unsigned int>(propId));
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
*/
  std::string prop = CameraProperty::type().get(propId).name();

  cedar::aux::LogSingleton::getInstance()->warning
                                           (
                                             this->getName() + ": Property " + prop + info,
                                             "cedar::dev::sensors::visual::CameraGrabber::getCameraProperty()"
                                           );

  return CAMERA_PROPERTY_NOT_SUPPORTED;
}

//----------------------------------------------------------------------------------------------------
double cedar::dev::sensors::visual::CameraGrabber::getCameraPropertyValue
(
  unsigned int channel,
  CameraProperty::Id propId
)
{
  if (channel >= getNumCams())
  {
    CEDAR_THROW
    (
      cedar::aux::IndexOutOfRangeException,
      "cedar::dev::sensors::visual::CameraGrabber::getCameraProperty"
    );
  }

  std::string info="";
/*
  if (getCameraChannel(channel)->mCamStateAndConfig->isSupported(propId))
  {
    if (getCameraChannel(channel)->mCamStateAndConfig->isReadable(propId))
    {
      return getCameraChannel(channel)->mCamStateAndConfig->getPropertyValue(static_cast<unsigned int>(propId));
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
*/
  std::string prop = CameraProperty::type().get(propId).name();
  cedar::aux::LogSingleton::getInstance()->warning
                                           (
                                             this->getName() + ": Property " + prop + info,
                                             "cedar::dev::sensors::visual::CameraGrabber::getCameraPropertyValue()"
                                           );
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
  std::string prop = CameraProperty::type().get(propId).name();
  std::string info = boost::lexical_cast<std::string>(boost::math::iround(value));
  cedar::aux::LogSingleton::getInstance()->debugMessage
                                           (
                                             this->getName() + ": Try to set " + prop + " to " + info,
                                             "cedar::dev::sensors::visual::CameraGrabber::setCameraProperty()"
                                           );

  if (channel >= getNumCams())
  {
    CEDAR_THROW
    (
      cedar::aux::IndexOutOfRangeException,
      "cedar::dev::sensors::visual::CameraGrabber::setCameraProperty"
    );
  }
  return false; //getCameraChannel(channel)->mCamStateAndConfig->setProperty(propId, value);
}

//----------------------------------------------------------------------------------------------------
bool cedar::dev::sensors::visual::CameraGrabber::setCameraProperty(CameraProperty::Id propId, double value)
{
  return setCameraProperty(0,propId, value);
}

//----------------------------------------------------------------------------------------------------
bool cedar::dev::sensors::visual::CameraGrabber::setCameraSetting
(
  unsigned int channel,
  CameraSetting::Id settingId,
  double value
)
{
  if (channel >= getNumCams())
  {
    CEDAR_THROW(cedar::aux::IndexOutOfRangeException,"cedar::dev::sensors::visual::CameraGrabber::setCameraSetting");
  }

  unsigned int setting_id = static_cast<unsigned int>(settingId);
  std::string setting_name = CameraSetting::type().get(settingId).name();

  //check if undefined settings
  if ( (settingId == cedar::aux::Enum::UNDEFINED) )
  {
    std::string info = ": Channel " + boost::lexical_cast<std::string>(channel)
                       + ": Undefined or unsupported settings \""
                       + setting_name + "\" (ID: " + boost::lexical_cast<std::string>(setting_id) + ")";

      cedar::aux::LogSingleton::getInstance()->message
                                               (
                                                 this->getName() + info,
                                                 "cedar::dev::sensors::visual::CameraGrabber::setCameraSetting()"
                                               );
    return false;
  }

  if( !getImage(channel).empty() )
  {
    std::string info = ": Camera is already in grabbing mode";
    /*std::string info = ": Camera is already in grabbing mode. Settings can only be changed on initialization "
                       +""
                       +"of the grabber. "
                       +"To take changes effect, you have to save your settings (writeConfiguration()) "
                       +"and then restart (i.e delete and create a new one) your grabber.";
    */
    cedar::aux::LogSingleton::getInstance()->systemInfo
                                             (
                                               this->getName() + info,
                                               "cedar::dev::sensors::visual::CameraGrabber::setCameraSetting()"
                                             );

  }

  bool result = false; //getCameraChannel(channel)->mCamStateAndConfig->setSetting(setting_id, value);
  return result;
}

//----------------------------------------------------------------------------------------------------
double cedar::dev::sensors::visual::CameraGrabber::getCameraSetting( unsigned int channel, CameraSetting::Id settingId)
{
  if (channel >= getNumCams())
  {
    CEDAR_THROW(cedar::aux::IndexOutOfRangeException,"cedar::dev::sensors::visual::CameraGrabber::getCameraSetting");
  }

  unsigned int setting_id = static_cast<unsigned int>(settingId);

  //check if undefined settings
  if (settingId == cedar::aux::Enum::UNDEFINED)
  {
      std::string info = ": Channel " + boost::lexical_cast<std::string>(channel)
                          + ": Undefined or unsupported settings \""
                          + CameraSetting::type().get(settingId).name()
                          + "\" (ID: " + boost::lexical_cast<std::string>(setting_id) + ")";

      cedar::aux::LogSingleton::getInstance()->message
                                               (
                                                 this->getName() + info,
                                                 "cedar::dev::sensors::visual::CameraGrabber::getCameraSetting()"
                                               );
    return -1;
  }
  return false; //getCameraChannel(channel)->mCamStateAndConfig->getSetting(setting_id);
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

#ifdef CEDAR_USE_LIB_DC1394
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
#endif
//----------------------------------------------------------------------------------------------------
cv::Size cedar::dev::sensors::visual::CameraGrabber::getCameraFrameSize( unsigned int channel)
{
  cv::Size size;
  size.width = boost::math::iround(getCameraSetting(channel,CameraSetting::SETTING_FRAME_WIDTH));
  size.height = boost::math::iround(getCameraSetting(channel,CameraSetting::SETTING_FRAME_HEIGHT));
  return size;
}


//----------------------------------------------------------------------------------------------------
bool cedar::dev::sensors::visual::CameraGrabber::setRawProperty(unsigned int channel, unsigned int propId, double value)
{
  if (channel >= getNumCams())
  {
    CEDAR_THROW(cedar::aux::IndexOutOfRangeException,"cedar::dev::sensors::visual::CameraGrabber::setRawProperty");
  }
  return false; //getCameraChannel(channel)->mCamStateAndConfig->setRawProperty(propId, value);
}


//----------------------------------------------------------------------------------------------------
double cedar::dev::sensors::visual::CameraGrabber::getRawProperty(unsigned int channel, unsigned int propId)
{
  if (channel >= getNumCams())
  {
    CEDAR_THROW(cedar::aux::IndexOutOfRangeException,"cedar::dev::sensors::visual::CameraGrabber::getRawProperty");
  }
  return 0.0; //getCameraChannel(channel)->mCamStateAndConfig->getRawProperty(propId);
}

