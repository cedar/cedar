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
#include "cedar/auxiliaries/casts.h"
#include "cedar/devices/sensors/visual/camera/enums/CameraSetting.h"

#include "cedar/devices/sensors/visual/camera/backends/CameraDeviceCvVideoCapture.h"
#include "cedar/devices/sensors/visual/camera/backends/CameraDeviceVfl.h"

#ifdef CEDAR_USE_LIB_DC1394
#include "cedar/devices/sensors/visual/camera/backends/CameraDeviceDc1394.h"
#endif

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
//        ->registerType<cedar::dev::sensors::visual::CameraGrabber::CameraChannelPtr>();
        ->registerType<cedar::dev::sensors::visual::CameraChannelPtr>();
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
  cedar::dev::sensors::visual::DeBayerFilter::Id debayerFilter
)
:
cedar::dev::sensors::visual::Grabber
(
  "CameraGrabber",
  //cedar::dev::sensors::visual::CameraGrabber::CameraChannelPtr
  cedar::dev::sensors::visual::CameraChannelPtr
  (
//    new cedar::dev::sensors::visual::CameraGrabber::CameraChannel(cameraId, isGuid, backendType, debayerFilter)
    new cedar::dev::sensors::visual::CameraChannel(cameraId, isGuid, backendType, debayerFilter)
  )
)
{
  init();
}

//----------------------------------------------------------------------------------------------------
//Constructor for stereo-file grabber
cedar::dev::sensors::visual::CameraGrabber::CameraGrabber
(
  unsigned int cameraId0,
  unsigned int cameraId1,
  bool isGuid,
  cedar::dev::sensors::visual::CameraBackendType::Id backendType,
  cedar::dev::sensors::visual::DeBayerFilter::Id debayerFilter
)
:
cedar::dev::sensors::visual::Grabber
(
  "StereoCameraGrabber",
//  cedar::dev::sensors::visual::CameraGrabber::CameraChannelPtr
  cedar::dev::sensors::visual::CameraChannelPtr
  (
//    new cedar::dev::sensors::visual::CameraGrabber::CameraChannel(cameraId0, isGuid, backendType, debayerFilter)
    new cedar::dev::sensors::visual::CameraChannel(cameraId0, isGuid, backendType, debayerFilter)
  ),
//  cedar::dev::sensors::visual::CameraGrabber::CameraChannelPtr
  cedar::dev::sensors::visual::CameraChannelPtr
  (
//    new cedar::dev::sensors::visual::CameraGrabber::CameraChannel(cameraId1, isGuid, backendType, debayerFilter)
    new cedar::dev::sensors::visual::CameraChannel(cameraId1, isGuid, backendType, debayerFilter)
  )
)
{
  init();
}

//----------------------------------------------------------------------------------------------------
void cedar::dev::sensors::visual::CameraGrabber::init()
{
  cedar::aux::LogSingleton::getInstance()->allocating(this);

  for (unsigned int channel=0; channel<_mChannels->size(); ++channel)
  {
    QObject::connect
             (
               getCameraChannel(channel)->_mpBusId.get(),
               SIGNAL(valueChanged()),
               this,
               SLOT(cameraChanged())
             );
    QObject::connect
             (
               getCameraChannel(channel)->_mpGuid.get(),
               SIGNAL(valueChanged()),
               this,
               SLOT(cameraChanged())
             );
    QObject::connect
             (
               getCameraChannel(channel)->_mpBackendType.get(),
               SIGNAL(valueChanged()),
               this,
               SLOT(cameraChanged())
             );
//    QObject::connect
//             (
//               getCameraChannel(channel)->_mpDebayerFilter.get(),
//               SIGNAL(valueChanged()),
//               this,
//               SLOT(cameraChanged())
//             );
  }

  //watch if channel added to ObjectParameterList
  _mChannels->connectToObjectAddedSignal
              (
                boost::bind(&cedar::dev::sensors::visual::CameraGrabber::channelAdded,this,_1)
              );

}


//----------------------------------------------------------------------------------------------------
cedar::dev::sensors::visual::CameraGrabber::~CameraGrabber()
{
  doCleanUp();
  cedar::aux::LogSingleton::getInstance()->freeing(this);
}

//--------------------------------------------------------------------------------------------------------------------
// slots
//--------------------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------
void cedar::dev::sensors::visual::CameraGrabber::channelAdded(int index)
{
  QObject::connect
           (
             getCameraChannel(index)->_mpBusId.get(),
             SIGNAL(valueChanged()),
             this,
             SLOT(cameraChanged())
           );
  QObject::connect
           (
             getCameraChannel(index)->_mpGuid.get(),
             SIGNAL(valueChanged()),
             this,
             SLOT(cameraChanged())
           );
  QObject::connect
           (
             getCameraChannel(index)->_mpBackendType.get(),
             SIGNAL(valueChanged()),
             this,
             SLOT(cameraChanged())
           );
}

//----------------------------------------------------------------------------------------------------
void cedar::dev::sensors::visual::CameraGrabber::cameraChanged()
{
  // deceide if setting

// get sender
  cedar::aux::ParameterPtr p_sender
    = cedar::aux::ParameterPtr(cedar::aux::asserted_cast<cedar::aux::Parameter * >(QObject::sender()));

  //search for the changed channel
  unsigned int num_channels = getNumCams();
  unsigned int channel = 0;

  while
  (
    (channel < num_channels)
    && (p_sender != getCameraChannel(channel)->_mpBusId.get())
    && (p_sender != getCameraChannel(channel)->_mpGuid.get())
    && (p_sender != getCameraChannel(channel)->_mpBackendType.get())
  )
  {
    channel++;
  }
//
//  //if found, create new content
//  if ((p_sender == getCameraChannel(channel)->_mpBusId->get()) || (p_sender == getCameraChannel(channel)->_mpGuid->get()))
//  {
//    this->applyParameter();
//  }

  if (this->getCameraChannel(channel))   
  {
    ;
  }
  this->applyParameter();

}
//----------------------------------------------------------------------------------------------------------------------
//methods
//----------------------------------------------------------------------------------------------------------------------


void cedar::dev::sensors::visual::CameraGrabber::onCloseGrabber()
{
  //close all captures
  unsigned int num_cams = getNumCams();
  for (unsigned int channel = 0; channel < num_cams; ++channel)
  {
    getCameraChannel(channel)->mVideoCapture = cv::VideoCapture();
  }
}

//----------------------------------------------------------------------------------------------------
bool cedar::dev::sensors::visual::CameraGrabber::onCreateGrabber()
{
  if (this->isCreated())
  {
    this->closeGrabber();
  }

  unsigned int num_cams = getNumCams();

  // init message
  std::stringstream init_message;
  init_message << ": Initialize Grabber with " << num_cams << " camera(s) ..." << std::endl;
  for (unsigned int channel = 0; channel < num_cams; ++channel)
  {
    init_message << "Channel " << channel << ": capture from ";

    if (getCameraChannel(channel)->_mpByGuid->getValue())
    {
      init_message << "Camera-GUID: "<< getCameraChannel(channel)->_mpGuid->getValue() << std::endl;
    }
    else
    {
      init_message << "Bus-ID " << getCameraChannel(channel)->_mpBusId->getValue() << std::endl;
    }
  }
  cedar::aux::LogSingleton::getInstance()->debugMessage
                                           (
                                             this->getName() + init_message.str(),
                                             "cedar::dev::sensors::visual::CameraGrabber::onCreateGrabber()"
                                           );

  // create capture device, which depends on the chosen backend
  try
  {
    bool all_devices_created = true;
    for (unsigned int channel = 0; channel < num_cams; ++channel)
    {
      bool device_created = true;

      switch ( (getCameraChannel(channel)->_mpBackendType->getValue()).id() )
      {

#ifdef CEDAR_USE_LIB_DC1394
        case cedar::dev::sensors::visual::CameraBackendType::DC1394 :
        {
          cedar::dev::sensors::visual::CameraDeviceDc1394Ptr dc1394_device
                                                             (
                                                               new cedar::dev::sensors::visual::CameraDeviceDc1394
                                                                   (
                                                                     getCameraChannel(channel)
                                                                   )
                                                             );
          device_created = dc1394_device->init();
          break;
        }
#endif

        case cedar::dev::sensors::visual::CameraBackendType::AUTO :
        case cedar::dev::sensors::visual::CameraBackendType::CVCAPTURE :
        case cedar::dev::sensors::visual::CameraBackendType::VFL :
        default:
        {
          cedar::dev::sensors::visual::CameraDeviceCvVideoCapturePtr cv_device
                                                                     (
                                                                       new cedar::dev::sensors::visual::CameraDeviceCvVideoCapture
                                                                       (
                                                                        getCameraChannel(channel)
                                                                       )
                                                                     );
          device_created = cv_device->init();
        }
      } // switch on backend

      if (device_created)
      {
        setChannelInfo(channel);
      }
      all_devices_created = all_devices_created && device_created;
    } // for every channel

    mCaptureDeviceCreated = all_devices_created;
  }
  catch (...)
  {
    mCaptureDeviceCreated = false;
  }

  return mCaptureDeviceCreated;
}


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

//  std::string mode = CameraVideoMode::type().get(getCameraSetting(channel,CameraSetting::SETTING_MODE )).name();
//  std::string fps = CameraFrameRate::type().get(getCameraSetting(channel,CameraSetting::SETTING_FPS )).name();
//#ifdef CEDAR_USE_LIB_DC1394
//  std::string iso = CameraIsoSpeed::type().get(getCameraSetting(channel,CameraSetting::SETTING_ISO_SPEED )).name();
//#endif // defined CEDAR_USE_LIB_DC1394
//  unsigned int width = getCameraSetting(channel, CameraSetting::SETTING_FRAME_WIDTH);
//  unsigned int height = getCameraSetting(channel, CameraSetting::SETTING_FRAME_HEIGHT);
//  std::string size = boost::lexical_cast<std::string>(width) + " x "+ boost::lexical_cast<std::string>(height);

//  settings.push_back("Camera video mode:\t" + mode);
//  settings.push_back("Camera framerate:\t" + fps);
//#ifdef CEDAR_USE_LIB_DC1394
//  settings.push_back("Camera ISO-speed:\t" + iso);
//#endif // defined CEDAR_USE_LIB_DC1394
//  settings.push_back("Camera frame size:\t" + size);

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

  //grab and retrieve
   for(unsigned int channel = 0; channel < num_cams; ++channel)
   {

     getCameraChannel(channel)->mpVideoCaptureLock->lockForWrite();
     result = getCameraChannel(channel)->mVideoCapture.read(getCameraChannel(channel)->mImageMat) & result;

     //check if conversion from bayer-pattern to cv::Mat BGR format is needed
     cedar::dev::sensors::visual::DeBayerFilter::Id debayer_fiter;
     debayer_fiter = this->getCameraChannel(channel)->_mpDebayerFilter->getValue();

     if (debayer_fiter != cedar::dev::sensors::visual::DeBayerFilter::NONE)
     {
       cv::cvtColor(getCameraChannel(channel)->mImageMat,getCameraChannel(channel)->mImageMat,debayer_fiter);
     }

     getCameraChannel(channel)->mpVideoCaptureLock->unlock();

   }

  //OpenCV documentation:
  //for better synchronizing between the cameras,
  //first grab internally in camera
  //lock for concurrent access in the grabber-thread and in the get/set properties

//  for(unsigned int channel = 0; channel < num_cams; ++channel)
//  {
//    getCameraChannel(channel)->mpVideoCaptureLock->lockForWrite();
//    result = getCameraChannel(channel)->mVideoCapture.grab() && result;
//    getCameraChannel(channel)->mpVideoCaptureLock->unlock();
//  }
//
//  //and then retrieve the frames
//  if (result)
//  {
//    for(unsigned int channel = 0; channel < num_cams; ++channel)
//    {
//      getCameraChannel(channel)->mpVideoCaptureLock->lockForWrite();
//      result = getCameraChannel(channel)->mVideoCapture.retrieve(getCameraChannel(channel)->mImageMat) && result;
//      getCameraChannel(channel)->mpVideoCaptureLock->unlock();
//    }
//  }
  return result;
}



//----------------------------------------------------------------------------------------------------
void cedar::dev::sensors::visual::CameraGrabber::setChannelInfo(unsigned int channel)
{
  std::stringstream s;
  s << "Camera channel 0"
    << ": DeviceID: " << getCameraChannel(channel)->_mpBusId->getValue()
    << ", GUID: " << getCameraChannel(channel)->_mpGuid->getValue()
    << " (0x"<<std::hex<<getCameraChannel(channel)->_mpGuid->getValue()<<")"<<std::dec
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
  return getCameraChannel(channel)->_mpGuid->getValue();
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
  cedar::dev::sensors::visual::CameraSetting::Id settingId,
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
bool cedar::dev::sensors::visual::CameraGrabber::setCameraMode
(
  unsigned int channel,
  cedar::dev::sensors::visual::CameraVideoMode::Id modeId
)
{
  return setCameraSetting
         (
           channel,
           cedar::dev::sensors::visual::CameraSetting::MODE,
           static_cast<unsigned int>(modeId)
         );
}

//----------------------------------------------------------------------------------------------------
cedar::dev::sensors::visual::CameraVideoMode::Id
  cedar::dev::sensors::visual::CameraGrabber::getCameraMode(unsigned int channel)
{
  return static_cast<cedar::dev::sensors::visual::CameraVideoMode::Id>
         (
           getCameraSetting(channel,CameraSetting::MODE)
         );
}

//----------------------------------------------------------------------------------------------------
bool cedar::dev::sensors::visual::CameraGrabber::setCameraFps(unsigned int channel,CameraFrameRate::Id fpsId)
{
  return setCameraSetting(channel,CameraSetting::FPS,static_cast<unsigned int>(fpsId));
}

//----------------------------------------------------------------------------------------------------
cedar::dev::sensors::visual::CameraFrameRate::Id
  cedar::dev::sensors::visual::CameraGrabber::getCameraFps(unsigned int channel)
{
  return static_cast<cedar::dev::sensors::visual::CameraVideoMode::Id>
         (
           getCameraSetting(channel,CameraSetting::FPS)
         );
}

#ifdef CEDAR_USE_LIB_DC1394
//----------------------------------------------------------------------------------------------------
bool cedar::dev::sensors::visual::CameraGrabber::setCameraIsoSpeed(unsigned int channel,CameraIsoSpeed::Id isoSpeedId)
{
  return setCameraSetting
         (
           channel,
           cedar::dev::sensors::visual::CameraSetting::ISO_SPEED,
           static_cast<unsigned int>(isoSpeedId)
         );
}

//----------------------------------------------------------------------------------------------------
cedar::dev::sensors::visual::CameraIsoSpeed::Id
  cedar::dev::sensors::visual::CameraGrabber::getCameraIsoSpeed(unsigned int channel)
{
  return static_cast<CameraVideoMode::Id>
         (
           getCameraSetting(channel, cedar::dev::sensors::visual::CameraSetting::ISO_SPEED)
         );
}
#endif
//----------------------------------------------------------------------------------------------------
cv::Size cedar::dev::sensors::visual::CameraGrabber::getCameraFrameSize( unsigned int channel)
{
  cv::Size size;
  size.width = boost::math::iround(getCameraSetting(channel,CameraSetting::FRAME_WIDTH));
  size.height = boost::math::iround(getCameraSetting(channel,CameraSetting::FRAME_HEIGHT));
  size.width = 0;
  size.height = 0;
  return size;
}


//----------------------------------------------------------------------------------------------------
bool cedar::dev::sensors::visual::CameraGrabber::setRawProperty(unsigned int channel, unsigned int propId, double value)
{
  if (channel >= getNumCams())
  {
    CEDAR_THROW(cedar::aux::IndexOutOfRangeException,"cedar::dev::sensors::visual::CameraGrabber::setRawProperty");
  }
  getCameraChannel(channel)->mpProperties->setPropertyRawValue(propId, value);
  //return false;
}


//----------------------------------------------------------------------------------------------------
double cedar::dev::sensors::visual::CameraGrabber::getRawProperty(unsigned int channel, unsigned int propId)
{
  if (channel >= getNumCams())
  {
    CEDAR_THROW(cedar::aux::IndexOutOfRangeException,"cedar::dev::sensors::visual::CameraGrabber::getRawProperty");
  }
  getCameraChannel(channel)->mpProperties->getPropertyRawValue(propId);
}

