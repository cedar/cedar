/*======================================================================================================================

    Copyright 2011, 2012 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany

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
#include "cedar/auxiliaries/exceptions.h"
#include "cedar/auxiliaries/utilities.h"
#include "cedar/auxiliaries/casts.h"
#include "cedar/devices/sensors/visual/CameraGrabber.h"
#include "cedar/devices/sensors/visual/camera/enums/CameraSetting.h"
#include "cedar/devices/sensors/visual/camera/backends/CameraDeviceCvVideoCapture.h"

#ifdef CEDAR_USE_VIDEO_FOR_LINUX
#include "cedar/devices/sensors/visual/camera/backends/CameraDeviceVfl.h"
#endif // CEDAR_USE_VIDEO_FOR_LINUX

#ifdef CEDAR_USE_LIB_DC1394
#include "cedar/devices/sensors/visual/camera/backends/CameraDeviceDc1394.h"
#endif // CEDAR_USE_LIB_DC1394

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
  cedar::dev::sensors::visual::CameraChannelPtr
  (
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
  cedar::dev::sensors::visual::CameraChannelPtr
  (
    new cedar::dev::sensors::visual::CameraChannel(cameraId0, isGuid, backendType, debayerFilter)
  ),
  cedar::dev::sensors::visual::CameraChannelPtr
  (
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
  this->connectSignals();
  //watch if channel added to ObjectParameterList
  _mChannels->connectToObjectAddedSignal
              (
                boost::bind(&cedar::dev::sensors::visual::CameraGrabber::channelAdded,this,_1)
              );
}


//----------------------------------------------------------------------------------------------------
void cedar::dev::sensors::visual::CameraGrabber::connectSignals()
{
  for (unsigned int channel=0; channel<_mChannels->size(); ++channel)
  {
    QObject::connect
             (
               getCameraChannel(channel)->_mpBackendType.get(),
               SIGNAL(valueChanged()),
               this,
               SLOT(cameraChanged())
             );

    QObject::connect
             (
               getCameraChannel(channel)->mpSettings.get(),
               SIGNAL(settingsChanged()),
               this,
               SLOT(cameraChanged())
             );
  }
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
void cedar::dev::sensors::visual::CameraGrabber::channelAdded(int) // int index
{
  this->connectSignals();
}

//----------------------------------------------------------------------------------------------------
void cedar::dev::sensors::visual::CameraGrabber::cameraChanged()
{

  cedar::aux::LogSingleton::getInstance()->debugMessage
                                           (
                                             this->getName() + ": cameraChanged signal",
                                             "cedar::dev::sensors::visual::CameraGrabber::cameraChanged()"
                                           );

//  getCameraChannel(0)->mpSettings->setBackendType(getCameraChannel(0)->_mpBackendType->getValue());

  // only apply parameters directly, if the camera is already in use.
  // which is set by a loopedTrigger or by the applyParameter-action
  if (mCaptureDeviceCreated)
  {
    // delete and recreate all channels
    this->applyParameter();
  }

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

  bool result = false;
  unsigned int num_cams = getNumCams();

  // init message
  std::stringstream init_message;
  init_message << ": Initialize Grabber with " << num_cams << " camera(s) ..." << std::endl;
  for (unsigned int channel = 0; channel < num_cams; ++channel)
  {
    init_message << "Channel " << channel << ": capture from ";

    if (getCameraChannel(channel)->mpSettings->getByGuid())
    {
      init_message << "Camera-GUID: ";
    }
    else
    {
      init_message << "Bus-ID: ";
    }
    init_message << getCameraChannel(channel)->mpSettings->getCameraId() << std::endl;
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
                                                                     this,
                                                                     getCameraChannel(channel)
                                                                   )
                                                             );
          device_created = dc1394_device->init();
          break;
        }
#endif

#ifdef CEDAR_USE_VIDEO_FOR_LINUX
        case cedar::dev::sensors::visual::CameraBackendType::VFL :
#endif // CEDAR_USE_VIDEO_FOR_LINUX

        case cedar::dev::sensors::visual::CameraBackendType::AUTO :
        case cedar::dev::sensors::visual::CameraBackendType::CVCAPTURE :


        default:
        {
          cedar::dev::sensors::visual::CameraDeviceCvVideoCapturePtr cv_device
                                                                     (
                                                                       new cedar::dev::sensors::visual::CameraDeviceCvVideoCapture
                                                                       (
                                                                        this,
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

    result = all_devices_created;
  }
  catch (...)
  {
    result = false;
  }

  return result;
}

//----------------------------------------------------------------------------------------------------
void cedar::dev::sensors::visual::CameraGrabber::setDebayerFilter
(
  cedar::dev::sensors::visual::DeBayerFilter::Id filterId
)
{
  this->getCameraChannel(0)->_mpDebayerFilter->setValue(filterId);
}


//----------------------------------------------------------------------------------------------------
void cedar::dev::sensors::visual::CameraGrabber::setDebayerFilter
(
  unsigned int channel,
  cedar::dev::sensors::visual::DeBayerFilter::Id filterId
)
{
  if (channel >= getNumCams())
  {
    CEDAR_THROW
    (
      cedar::aux::IndexOutOfRangeException,
      "cedar::dev::sensors::visual::CameraGrabber::setDebayerFilter"
    );
  }
  this->getCameraChannel(channel)->_mpDebayerFilter->setValue(filterId);
}

//----------------------------------------------------------------------------------------------------
cedar::dev::sensors::visual::DeBayerFilter::Id
  cedar::dev::sensors::visual::CameraGrabber::getDebayerFilter(unsigned int channel)
{
  if (channel >= getNumCams())
  {
    CEDAR_THROW
    (
      cedar::aux::IndexOutOfRangeException,
      "cedar::dev::sensors::visual::CameraGrabber::getDebayerFilter"
    );
  }
  return this->getCameraChannel(channel)->_mpDebayerFilter->getValue();
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

  cedar::dev::sensors::visual::CameraSettingsPtr p_settings = this->getCameraChannel(channel)->mpSettings;

  std::vector<std::string> settings;
  settings.push_back("all settings of channel " + boost::lexical_cast<std::string>(channel) + ":");


  std::string s;
  s = CameraVideoMode::type().get(p_settings->getVideoMode()).prettyString();
  settings.push_back("camera video mode:\t" + s);

  cv::Size size = this->getCameraFrameSize(channel);
  s = boost::lexical_cast<std::string>(size.width) + " x "+ boost::lexical_cast<std::string>(size.height);
  settings.push_back("camera framesize:\t" + s);

  if (getCameraChannel(channel)->_mpBackendType->getValue() == cedar::dev::sensors::visual::CameraBackendType::DC1394)
  {
    s = cedar::dev::sensors::visual::CameraFrameRate::type().get(p_settings->getFPS()).prettyString();
    settings.push_back("camera FPS mode:\t" + s);

#ifdef CEDAR_USE_LIB_DC1394
    s = cedar::dev::sensors::visual::CameraIsoSpeed::type().get(p_settings->getIsoSpeed()).prettyString();
    settings.push_back("camera ISO-speed mode:\t" + s);
#endif // CEDAR_USE_LIB_DC1394
  }

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
  std::stringstream ss;

  ss << "Camera channel " << channel << "; ";

  if (getCameraChannel(channel)->mpSettings->getByGuid())
  {
    ss << "Camera-GUID: ";
  }
  else
  {
    ss << "Bus-ID: ";
  }
  ss << getCameraChannel(channel)->mpSettings->getCameraId()
     << "; Mode: " << CameraVideoMode::type().get(this->getCameraVideoMode(0)).prettyString();

  getCameraChannel(channel)->mChannelInfo = ss.str();
}

//----------------------------------------------------------------------------------------------------
void cedar::dev::sensors::visual::CameraGrabber::onCleanUp()
{
  cedar::aux::LogSingleton::getInstance()->debugMessage
                                           (
                                             this->getName() + ": onCleanUp",
                                             "cedar::dev::sensors::visual::CameraGrabber::onCleanUp()"
                                           );

  //close all captures
  this->onCloseGrabber();
//  unsigned int num_cams = getNumCams();
//  for (unsigned int channel = 0; channel < num_cams; channel++)
//  {
//    //is done in the onClose() function
//    //getCameraChannel(channel)->mVideoCapture.release();
//  }

}

//----------------------------------------------------------------------------------------------------
unsigned int cedar::dev::sensors::visual::CameraGrabber::getCameraId(unsigned int channel)
{
  if (channel >= getNumCams())
  {
    CEDAR_THROW
    (
      cedar::aux::IndexOutOfRangeException,
      "cedar::dev::sensors::visual::CameraGrabber::getCameraId"
    );
  }
  return getCameraChannel(channel)->mpSettings->getCameraId();
}

bool cedar::dev::sensors::visual::CameraGrabber::isGuid(unsigned int channel)
{
  if (channel >= getNumCams())
  {
    CEDAR_THROW
    (
      cedar::aux::IndexOutOfRangeException,
      "cedar::dev::sensors::visual::CameraGrabber::isGuid"
    );
  }
  return getCameraChannel(channel)->mpSettings->getByGuid();
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

  cedar::dev::sensors::visual::CameraPropertiesPtr p_prop = this->getCameraChannel(channel)->mpProperties;
  double value = p_prop->getProperty(propId);

  if (value == CAMERA_PROPERTY_NOT_SUPPORTED)
  {
    std::string info = "is not supported!";

    std::string prop_name = CameraProperty::type().get(propId).prettyString();
    cedar::aux::LogSingleton::getInstance()->warning
                                             (
                                               this->getName() + ": Property " + prop_name + info,
                                               "cedar::dev::sensors::visual::CameraGrabber::getCameraProperty()"
                                             );
  }
  return value;
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

  cedar::dev::sensors::visual::CameraPropertiesPtr p_prop = this->getCameraChannel(channel)->mpProperties;

  if (p_prop->isSupported(propId))
  {
    if (p_prop->isReadable(propId))
    {
      return p_prop->getPropertyFromCamera(propId);
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

  std::string prop_name = CameraProperty::type().get(propId).prettyString();
  cedar::aux::LogSingleton::getInstance()->warning
                                           (
                                             this->getName() + ": Property " + prop_name + info,
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
  if (channel >= getNumCams())
  {
    CEDAR_THROW
    (
      cedar::aux::IndexOutOfRangeException,
      "cedar::dev::sensors::visual::CameraGrabber::setCameraProperty"
    );
  }

  std::string prop = CameraProperty::type().get(propId).prettyString();
  std::string info = boost::lexical_cast<std::string>(boost::math::iround(value));
  cedar::aux::LogSingleton::getInstance()->debugMessage
                                           (
                                             this->getName() + ": try to set " + prop + " to " + info,
                                             "cedar::dev::sensors::visual::CameraGrabber::setCameraProperty()"
                                           );

  return getCameraChannel(channel)->mpProperties->setProperty(propId, value);
}


//----------------------------------------------------------------------------------------------------
cedar::dev::sensors::visual::CameraPropertyMode::Id cedar::dev::sensors::visual::CameraGrabber::getCameraPropertyMode
(
  unsigned int channel,
  cedar::dev::sensors::visual::CameraProperty::Id propId
)
{
  return getCameraChannel(channel)->mpProperties->getPropertyMode(propId);
}



//----------------------------------------------------------------------------------------------------
bool cedar::dev::sensors::visual::CameraGrabber::setCameraPropertyMode
(
  unsigned int channel,
  cedar::dev::sensors::visual::CameraProperty::Id propId,
  cedar::dev::sensors::visual::CameraPropertyMode::Id modeId
)
{
  return getCameraChannel(channel)->mpProperties->setPropertyMode(propId, modeId);
}

//----------------------------------------------------------------------------------------------------
bool cedar::dev::sensors::visual::CameraGrabber::setCameraProperty(CameraProperty::Id propId, double value)
{
  return setCameraProperty(0,propId, value);
}

//----------------------------------------------------------------------------------------------------
void cedar::dev::sensors::visual::CameraGrabber::setCameraVideoMode
(
  unsigned int channel,
  cedar::dev::sensors::visual::CameraVideoMode::Id modeId
)
{
  this->getCameraChannel(channel)->mpSettings->setVideoMode(modeId);
}

//----------------------------------------------------------------------------------------------------
cedar::dev::sensors::visual::CameraVideoMode::Id
  cedar::dev::sensors::visual::CameraGrabber::getCameraVideoMode(unsigned int channel)
{
  return this->getCameraChannel(channel)->mpSettings->getVideoMode();
}

//----------------------------------------------------------------------------------------------------
void cedar::dev::sensors::visual::CameraGrabber::setCameraFps(unsigned int channel,CameraFrameRate::Id fpsId)
{
  this->getCameraChannel(channel)->mpSettings->setFPS(fpsId);
}

//----------------------------------------------------------------------------------------------------
cedar::dev::sensors::visual::CameraFrameRate::Id
  cedar::dev::sensors::visual::CameraGrabber::getCameraFps(unsigned int channel)
{
  return this->getCameraChannel(channel)->mpSettings->getFPS();
}

#ifdef CEDAR_USE_LIB_DC1394
//----------------------------------------------------------------------------------------------------
void cedar::dev::sensors::visual::CameraGrabber::setCameraIsoSpeed(unsigned int channel,CameraIsoSpeed::Id isoSpeedId)
{
  this->getCameraChannel(channel)->mpSettings->setIsoSpeed(isoSpeedId);
}

//----------------------------------------------------------------------------------------------------
cedar::dev::sensors::visual::CameraIsoSpeed::Id
  cedar::dev::sensors::visual::CameraGrabber::getCameraIsoSpeed(unsigned int channel)
{
  return this->getCameraChannel(channel)->mpSettings->getIsoSpeed();
}
#endif

//----------------------------------------------------------------------------------------------------
cv::Size cedar::dev::sensors::visual::CameraGrabber::getCameraFrameSize( unsigned int channel)
{
  cv::Size size;
  size.width = getPropertyFromCamera(channel,cedar::dev::sensors::visual::CameraSetting::FRAME_WIDTH);
  size.height = getPropertyFromCamera(channel,cedar::dev::sensors::visual::CameraSetting::FRAME_HEIGHT);
  return size;
}

//----------------------------------------------------------------------------------------------------
double cedar::dev::sensors::visual::CameraGrabber::getPropertyFromCamera(unsigned int channel, unsigned int propertyId)
{
  if (channel >= getNumCams())
  {
    CEDAR_THROW
    (
      cedar::aux::IndexOutOfRangeException,
      "cedar::dev::sensors::visual::CameraGrabber::getPropertyFromCamera"
    );
  }

  double result = -1;
  cedar::dev::sensors::visual::CameraChannelPtr p_channel = getCameraChannel(channel);
  p_channel->mpVideoCaptureLock->lockForWrite();
  if (p_channel->mVideoCapture.isOpened())
  {
    result = p_channel->mVideoCapture.get(propertyId);
  }
  p_channel->mpVideoCaptureLock->unlock();
  return result;
}


