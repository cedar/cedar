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
    along with cedar. If not, see <http:// www.gnu.org/licenses/>.

========================================================================================================================

    Institute:   Ruhr-Universitaet Bochum
                 Institut fuer Neuroinformatik

    File:        Grabber.cpp

    Maintainer:  Georg.Hartinger
    Email:       georg.hartinger@ini.rub.de
    Date:        2011 08 01

    Description: Implementation of the @em cedar::devices::camera::Grabber class.

    Credits:

======================================================================================================================*/

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/auxiliaries/exceptions.h"
#include "cedar/auxiliaries/utilities.h"
#include "cedar/auxiliaries/casts.h"
#include "cedar/devices/sensors/camera/Grabber.h"
#include "cedar/devices/sensors/camera/enums/Setting.h"
#include "cedar/devices/sensors/camera/backends/DeviceCvVideoCapture.h"

#ifdef CEDAR_USE_VIDEO_FOR_LINUX
#include "cedar/devices/sensors/camera/backends/DeviceVfl.h"
#endif // CEDAR_USE_VIDEO_FOR_LINUX

#ifdef CEDAR_USE_LIB_DC1394
#include "cedar/devices/sensors/camera/backends/DeviceDc1394.h"
#endif // CEDAR_USE_LIB_DC1394

// SYSTEM INCLUDES
#include <boost/lexical_cast.hpp>
#include <boost/math/special_functions/round.hpp> // rounding from double to int in cv::VideoCapture get and set methods

//----------------------------------------------------------------------------------------------------------------------
// register the class
//----------------------------------------------------------------------------------------------------------------------
namespace
{
  bool declared
    = cedar::dev::sensors::camera::Grabber::ChannelManagerSingleton::getInstance()
        ->registerType<cedar::dev::sensors::camera::ChannelPtr>();
}

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------
// Constructor for single-file grabber
cedar::dev::sensors::camera::Grabber::Grabber
(
  unsigned int cameraId,
  bool isGuid,
  cedar::dev::sensors::camera::BackendType::Id backendType,
  cedar::dev::sensors::camera::Decoding::Id decodeFilter
)
:
cedar::dev::sensors::visual::Grabber
(
  "Grabber",
  cedar::dev::sensors::camera::ChannelPtr
  (
    new cedar::dev::sensors::camera::Channel(cameraId, isGuid, backendType, decodeFilter)
  )
)
{
  init();
}

//----------------------------------------------------------------------------------------------------
// Constructor for stereo-file grabber
cedar::dev::sensors::camera::Grabber::Grabber
(
  unsigned int cameraId0,
  unsigned int cameraId1,
  bool isGuid,
  cedar::dev::sensors::camera::BackendType::Id backendType,
  cedar::dev::sensors::camera::Decoding::Id decodeFilter
)
:
cedar::dev::sensors::visual::Grabber
(
  "StereoGrabber",
  cedar::dev::sensors::camera::ChannelPtr
  (
    new cedar::dev::sensors::camera::Channel(cameraId0, isGuid, backendType, decodeFilter)
  ),
  cedar::dev::sensors::camera::ChannelPtr
  (
    new cedar::dev::sensors::camera::Channel(cameraId1, isGuid, backendType, decodeFilter)
  )
)
{
  init();
}

//----------------------------------------------------------------------------------------------------
void cedar::dev::sensors::camera::Grabber::init()
{
  cedar::aux::LogSingleton::getInstance()->allocating(this);
  this->connectSignals();
  // watch if channel added to ObjectParameterList
  _mChannels->connectToObjectAddedSignal
              (
                boost::bind(&cedar::dev::sensors::camera::Grabber::channelAdded,this,_1)
              );

  // enable/disable settings like byGuid or so
  for (unsigned int channel=0; channel<_mChannels->size(); ++channel)
  {
    getCameraChannel(channel)->setBackendType(getCameraChannel(channel)->_mpBackendType->getValue());
  }

}


//----------------------------------------------------------------------------------------------------
void cedar::dev::sensors::camera::Grabber::connectSignals()
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
               getCameraChannel(channel).get(),
               SIGNAL(settingsChanged()),
               this,
               SLOT(cameraChanged())
             );
  }
}


//----------------------------------------------------------------------------------------------------
cedar::dev::sensors::camera::Grabber::~Grabber()
{
  doCleanUp();
  cedar::aux::LogSingleton::getInstance()->freeing(this);
}

//--------------------------------------------------------------------------------------------------------------------
// slots
//--------------------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------
void cedar::dev::sensors::camera::Grabber::channelAdded(int) // int index
{
  this->connectSignals();
}

//----------------------------------------------------------------------------------------------------
void cedar::dev::sensors::camera::Grabber::cameraChanged()
{

  cedar::aux::LogSingleton::getInstance()->debugMessage
                                           (
                                             this->getName() + ": cameraChanged signal",
                                             "cedar::dev::sensors::camera::Grabber::cameraChanged()"
                                           );

//  getCameraChannel(0)->setBackendType(getCameraChannel(0)->_mpBackendType->getValue());

  // only apply parameters directly, if the camera is already in use.
  // which is set by a loopedTrigger or by the applyParameter-action
  if (mCaptureDeviceCreated)
  {
    // delete and recreate all channels
    this->applyParameter();
  }
  else
  {
    // otherwise only update visible fields
    unsigned int num_cams = getNumCams();
    for (unsigned int channel = 0; channel < num_cams; ++channel)
    {
//      std::cout << "set Backendtype to "
//        << cedar::dev::sensors::camera::BackendType::type().get(getCameraChannel(channel)->_mpBackendType->getValue()).prettyString() << std::endl;
      getCameraChannel(channel)->setBackendType(getCameraChannel(channel)->_mpBackendType->getValue());
    }
  }

}
//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------


void cedar::dev::sensors::camera::Grabber::onCloseGrabber()
{
  // close all captures
  unsigned int num_cams = getNumCams();
  for (unsigned int channel = 0; channel < num_cams; ++channel)
  {
    getCameraChannel(channel)->mVideoCapture = cv::VideoCapture();
  }
}

//----------------------------------------------------------------------------------------------------
bool cedar::dev::sensors::camera::Grabber::onCreateGrabber()
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

    if (getCameraChannel(channel)->getByGuid())
    {
      init_message << "Camera-GUID: ";
    }
    else
    {
      init_message << "Bus-ID: ";
    }
    init_message << getCameraChannel(channel)->getCameraId() << std::endl;
  }
  cedar::aux::LogSingleton::getInstance()->debugMessage
                                           (
                                             this->getName() + init_message.str(),
                                             "cedar::dev::sensors::camera::Grabber::onCreateGrabber()"
                                           );

  // create capture device, which depends on the chosen backend
  try
  {
    bool all_devices_created = true;
    for (unsigned int channel = 0; channel < num_cams; ++channel)
    {
      getCameraChannel(channel)->createBackend();
      bool device_created = getCameraChannel(channel)->mpBackend->init();

      if (device_created)
      {
        setChannelInfo(channel);
      }

      // Backend device not longer used. Channel::mVideoCapture does the job
      getCameraChannel(channel)->mpBackend.reset();

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
void cedar::dev::sensors::camera::Grabber::setDecodeFilter
(
  cedar::dev::sensors::camera::Decoding::Id filterId
)
{
  this->getCameraChannel(0)->_mpDecodeFilter->setValue(filterId);
}


//----------------------------------------------------------------------------------------------------
void cedar::dev::sensors::camera::Grabber::setDecodeFilter
(
  unsigned int channel,
  cedar::dev::sensors::camera::Decoding::Id filterId
)
{
  if (channel >= getNumCams())
  {
    CEDAR_THROW
    (
      cedar::aux::IndexOutOfRangeException,
      "cedar::dev::sensors::camera::Grabber::setDecodeFilter"
    );
  }
  this->getCameraChannel(channel)->_mpDecodeFilter->setValue(filterId);
}

//----------------------------------------------------------------------------------------------------
cedar::dev::sensors::camera::Decoding::Id
  cedar::dev::sensors::camera::Grabber::getDecodeFilter(unsigned int channel)
{
  if (channel >= getNumCams())
  {
    CEDAR_THROW
    (
      cedar::aux::IndexOutOfRangeException,
      "cedar::dev::sensors::camera::Grabber::getDecodeFilter"
    );
  }
  return this->getCameraChannel(channel)->_mpDecodeFilter->getValue();
}



//----------------------------------------------------------------------------------------------------
std::vector<std::string> cedar::dev::sensors::camera::Grabber::getAllSettings(unsigned int channel)
{
  if (channel >= getNumCams())
  {
    CEDAR_THROW
    (
      cedar::aux::IndexOutOfRangeException,
      "cedar::dev::sensors::camera::Grabber::printAllSettings"
    );
  }

  cedar::dev::sensors::camera::ChannelPtr p_channel = this->getCameraChannel(channel);

  std::vector<std::string> settings;
  settings.push_back("all settings of channel " + boost::lexical_cast<std::string>(channel) + ":");


  std::string s;
  s = VideoMode::type().get(p_channel->getVideoMode()).prettyString();
  settings.push_back("camera video mode:\t" + s);

  cv::Size size = this->getCameraFrameSize(channel);
  s = boost::lexical_cast<std::string>(size.width) + " x "+ boost::lexical_cast<std::string>(size.height);
  settings.push_back("camera framesize:\t" + s);

#ifdef CEDAR_USE_LIB_DC1394
  if (getCameraChannel(channel)->_mpBackendType->getValue() == cedar::dev::sensors::camera::BackendType::DC1394)
  {
    s = cedar::dev::sensors::camera::FrameRate::type().get(p_channel->getFPS()).prettyString();
    settings.push_back("camera FPS mode:\t" + s);

    s = cedar::dev::sensors::camera::IsoSpeed::type().get(p_channel->getIsoSpeed()).prettyString();
    settings.push_back("camera ISO-speed mode:\t" + s);
  }
#endif // CEDAR_USE_LIB_DC1394

  return settings;
}

//----------------------------------------------------------------------------------------------------
std::vector<std::string> cedar::dev::sensors::camera::Grabber::getAllProperties(unsigned int channel)
{
  if (channel >= getNumCams())
  {
    CEDAR_THROW
    (
      cedar::aux::IndexOutOfRangeException,
      "cedar::dev::sensors::camera::Grabber::printAllProperties"
    );
  }


  // write out all properties as set in our structure as well as the values directly read from cam
  std::vector<std::string> properties;
  properties.push_back("All properties of channel " + boost::lexical_cast<std::string>(channel) + ":");

  int num_properties = cedar::dev::sensors::camera::Property::type().list().size();
  for (int i=0; i<num_properties; i++)
  {
    cedar::dev::sensors::camera::Property::Id prop_id;
    prop_id = cedar::dev::sensors::camera::Property::type().list().at(i).id();
    std::string prop_name = cedar::dev::sensors::camera::Property::type().get(prop_id).name();
    std::string prop_description = cedar::dev::sensors::camera::Property::type().get(prop_id).prettyString();
    double prop_value = getProperty(channel,prop_id);
    double prop_raw_value = getPropertyValue(channel,prop_id);

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
bool cedar::dev::sensors::camera::Grabber::onGrab()
{
  //!@todo This should also be a bool
  int result = true;
  unsigned int num_cams = getNumCams();

  // grab and retrieve
   for(unsigned int channel = 0; channel < num_cams; ++channel)
   {

     getCameraChannel(channel)->mpVideoCaptureLock->lockForWrite();
     result = getCameraChannel(channel)->mVideoCapture.read(getImageMat(channel)) & result;

     // check if conversion from bayer-pattern to cv::Mat BGR format is needed
     cedar::dev::sensors::camera::Decoding::Id debayer_fiter;
     debayer_fiter = this->getCameraChannel(channel)->_mpDecodeFilter->getValue();

     if (debayer_fiter != cedar::dev::sensors::camera::Decoding::NONE)
     {
       cv::cvtColor(getImageMat(channel),getImageMat(channel),debayer_fiter);
     }

     getCameraChannel(channel)->mpVideoCaptureLock->unlock();

   }

  // OpenCV documentation:
  // for better synchronizing between the cameras,
  // first grab internally in camera
  // lock for concurrent access in the grabber-thread and in the get/set properties

//  for(unsigned int channel = 0; channel < num_cams; ++channel)
//  {
//    getCameraChannel(channel)->mpVideoCaptureLock->lockForWrite();
//    result = getCameraChannel(channel)->mVideoCapture.grab() && result;
//    getCameraChannel(channel)->mpVideoCaptureLock->unlock();
//  }
//
//  // and then retrieve the frames
//  if (result)
//  {
//    for(unsigned int channel = 0; channel < num_cams; ++channel)
//    {
//      getCameraChannel(channel)->mpVideoCaptureLock->lockForWrite();
//      result = getCameraChannel(channel)->mVideoCapture.retrieve(getImageMat(channel)) && result;
//      getCameraChannel(channel)->mpVideoCaptureLock->unlock();
//    }
//  }
  return result;
}



//----------------------------------------------------------------------------------------------------
void cedar::dev::sensors::camera::Grabber::setChannelInfo(unsigned int channel)
{
  std::stringstream ss;

  ss << "Camera channel " << channel << "; ";

  if (getCameraChannel(channel)->getByGuid())
  {
    ss << "Camera-GUID: ";
  }
  else
  {
    ss << "Bus-ID: ";
  }
  ss << getCameraChannel(channel)->getCameraId()
     << "; Mode: " << VideoMode::type().get(this->getCameraVideoMode(channel)).prettyString();

  setChannelInfoString(channel, ss.str());
}

//----------------------------------------------------------------------------------------------------
void cedar::dev::sensors::camera::Grabber::onCleanUp()
{
  cedar::aux::LogSingleton::getInstance()->debugMessage
                                           (
                                             this->getName() + ": onCleanUp",
                                             "cedar::dev::sensors::camera::Grabber::onCleanUp()"
                                           );

  // close all captures
  this->onCloseGrabber();
//  unsigned int num_cams = getNumCams();
//  for (unsigned int channel = 0; channel < num_cams; channel++)
//  {
//    // is done in the onClose() function
//    // getCameraChannel(channel)->mVideoCapture.release();
//  }

}

//----------------------------------------------------------------------------------------------------
unsigned int cedar::dev::sensors::camera::Grabber::getCameraId(unsigned int channel)
{
  if (channel >= getNumCams())
  {
    CEDAR_THROW
    (
      cedar::aux::IndexOutOfRangeException,
      "cedar::dev::sensors::camera::Grabber::getCameraId"
    );
  }
  return getCameraChannel(channel)->getCameraId();
}

bool cedar::dev::sensors::camera::Grabber::isGuid(unsigned int channel)
{
  if (channel >= getNumCams())
  {
    CEDAR_THROW
    (
      cedar::aux::IndexOutOfRangeException,
      "cedar::dev::sensors::camera::Grabber::isGuid"
    );
  }
  return getCameraChannel(channel)->getByGuid();
}
//----------------------------------------------------------------------------------------------------
double cedar::dev::sensors::camera::Grabber::getProperty(unsigned int channel, Property::Id propId)
{
  if (channel >= getNumCams())
  {
    CEDAR_THROW
    (
      cedar::aux::IndexOutOfRangeException,
      "cedar::dev::sensors::camera::Grabber::getProperty"
    );
  }

  cedar::dev::sensors::camera::PropertiesPtr p_prop = this->getCameraChannel(channel)->mpProperties;
  double value = p_prop->getProperty(propId);

  if (value == CAMERA_PROPERTY_NOT_SUPPORTED)
  {
    std::string info = "is not supported!";

    std::string prop_name = Property::type().get(propId).prettyString();
    cedar::aux::LogSingleton::getInstance()->warning
                                             (
                                               this->getName() + ": Property " + prop_name + info,
                                               "cedar::dev::sensors::camera::Grabber::getProperty()"
                                             );
  }
  return value;
}

//----------------------------------------------------------------------------------------------------
double cedar::dev::sensors::camera::Grabber::getPropertyValue
(
  unsigned int channel,
  Property::Id propId
)
{
  if (channel >= getNumCams())
  {
    CEDAR_THROW
    (
      cedar::aux::IndexOutOfRangeException,
      "cedar::dev::sensors::camera::Grabber::getProperty"
    );
  }

  std::string info="";

  cedar::dev::sensors::camera::PropertiesPtr p_prop = this->getCameraChannel(channel)->mpProperties;

  if (p_prop->isSupported(propId))
  {
    return p_prop->getPropertyFromCamera(propId);
  }
  else
  {
    info = " is not supported";
  }

  std::string prop_name = Property::type().get(propId).prettyString();
  cedar::aux::LogSingleton::getInstance()->warning
                                           (
                                             this->getName() + ": Property " + prop_name + info,
                                             "cedar::dev::sensors::camera::Grabber::getPropertyValue()"
                                           );
  return CAMERA_PROPERTY_NOT_SUPPORTED;
}

//----------------------------------------------------------------------------------------------------
double cedar::dev::sensors::camera::Grabber::getProperty(Property::Id propId)
{
  return getProperty(0,propId);
}

//----------------------------------------------------------------------------------------------------
bool cedar::dev::sensors::camera::Grabber::setProperty
(
  unsigned int channel,
  Property::Id propId,
  double value
)
{
  if (channel >= getNumCams())
  {
    CEDAR_THROW
    (
      cedar::aux::IndexOutOfRangeException,
      "cedar::dev::sensors::camera::Grabber::setCameraProperty"
    );
  }

  std::string prop = Property::type().get(propId).prettyString();
  std::string info = boost::lexical_cast<std::string>(boost::math::iround(value));
  cedar::aux::LogSingleton::getInstance()->debugMessage
                                           (
                                             this->getName() + ": try to set " + prop + " to " + info,
                                             "cedar::dev::sensors::camera::Grabber::setCameraProperty()"
                                           );

  return getCameraChannel(channel)->mpProperties->setProperty(propId, value);
}


//----------------------------------------------------------------------------------------------------
cedar::dev::sensors::camera::PropertyMode::Id cedar::dev::sensors::camera::Grabber::getPropertyMode
(
  unsigned int channel,
  cedar::dev::sensors::camera::Property::Id propId
)
{
  return getCameraChannel(channel)->mpProperties->getPropertyMode(propId);
}



//----------------------------------------------------------------------------------------------------
bool cedar::dev::sensors::camera::Grabber::setPropertyMode
(
  unsigned int channel,
  cedar::dev::sensors::camera::Property::Id propId,
  cedar::dev::sensors::camera::PropertyMode::Id modeId
)
{
  return getCameraChannel(channel)->mpProperties->setPropertyMode(propId, modeId);
}

//----------------------------------------------------------------------------------------------------
bool cedar::dev::sensors::camera::Grabber::setProperty(Property::Id propId, double value)
{
  return setProperty(0,propId, value);
}

//----------------------------------------------------------------------------------------------------
void cedar::dev::sensors::camera::Grabber::setCameraVideoMode
(
  unsigned int channel,
  cedar::dev::sensors::camera::VideoMode::Id modeId
)
{
  this->getCameraChannel(channel)->setVideoMode(modeId);
}

//----------------------------------------------------------------------------------------------------
cedar::dev::sensors::camera::VideoMode::Id
  cedar::dev::sensors::camera::Grabber::getCameraVideoMode(unsigned int channel)
{
  return this->getCameraChannel(channel)->getVideoMode();
}

//----------------------------------------------------------------------------------------------------
void cedar::dev::sensors::camera::Grabber::setCameraFps(unsigned int channel,FrameRate::Id fpsId)
{
  this->getCameraChannel(channel)->setFPS(fpsId);
}

//----------------------------------------------------------------------------------------------------
cedar::dev::sensors::camera::FrameRate::Id
  cedar::dev::sensors::camera::Grabber::getCameraFps(unsigned int channel)
{
  return this->getCameraChannel(channel)->getFPS();
}

#ifdef CEDAR_USE_LIB_DC1394
//----------------------------------------------------------------------------------------------------
void cedar::dev::sensors::camera::Grabber::setCameraIsoSpeed(unsigned int channel,IsoSpeed::Id isoSpeedId)
{
  this->getCameraChannel(channel)->setIsoSpeed(isoSpeedId);
}

//----------------------------------------------------------------------------------------------------
cedar::dev::sensors::camera::IsoSpeed::Id
  cedar::dev::sensors::camera::Grabber::getCameraIsoSpeed(unsigned int channel)
{
  return this->getCameraChannel(channel)->getIsoSpeed();
}
#endif

//----------------------------------------------------------------------------------------------------
cv::Size cedar::dev::sensors::camera::Grabber::getCameraFrameSize( unsigned int channel)
{
  cv::Size size;
  size.width = getPropertyFromCamera(channel,cedar::dev::sensors::camera::Setting::FRAME_WIDTH);
  size.height = getPropertyFromCamera(channel,cedar::dev::sensors::camera::Setting::FRAME_HEIGHT);
  return size;
}

//----------------------------------------------------------------------------------------------------
double cedar::dev::sensors::camera::Grabber::getPropertyFromCamera(unsigned int channel, unsigned int propertyId)
{
  if (channel >= getNumCams())
  {
    CEDAR_THROW
    (
      cedar::aux::IndexOutOfRangeException,
      "cedar::dev::sensors::camera::Grabber::getPropertyFromCamera"
    );
  }

  double result = -1;
  cedar::dev::sensors::camera::ChannelPtr p_channel = getCameraChannel(channel);
  p_channel->mpVideoCaptureLock->lockForWrite();
  if (p_channel->mVideoCapture.isOpened())
  {
    result = p_channel->mVideoCapture.get(propertyId);
  }
  p_channel->mpVideoCaptureLock->unlock();
  return result;
}


