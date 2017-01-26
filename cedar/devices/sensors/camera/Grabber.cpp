/*======================================================================================================================

    Copyright 2011, 2012, 2013, 2014, 2015, 2016, 2017 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany

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
#include "cedar/auxiliaries/sleepFunctions.h"
#include "cedar/devices/sensors/visual/exceptions.h"
#include "cedar/devices/sensors/camera/Grabber.h"
#include "cedar/devices/sensors/camera/enums/Setting.h"
#include "cedar/devices/sensors/camera/backends/BackendCvVideoCapture.h"


#ifdef CEDAR_USE_VIDEO_FOR_LINUX
#include "cedar/devices/sensors/camera/backends/BackendV4L.h"
#endif // CEDAR_USE_VIDEO_FOR_LINUX

#ifdef CEDAR_USE_LIB_DC1394
#include "cedar/devices/sensors/camera/backends/BackendDc1394.h"
#endif // CEDAR_USE_LIB_DC1394

// SYSTEM INCLUDES
#ifndef Q_MOC_RUN
  #include <boost/lexical_cast.hpp>
  #include <boost/math/special_functions/round.hpp> // rounding from double to int in cv::VideoCapture get and set methods
#endif
#include <QWriteLocker>
#include <sstream>

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
// Constructor for a single channel grabber
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


// Constructor for stereo grabber
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


void cedar::dev::sensors::camera::Grabber::connectSignals()
{
  for (unsigned int channel=0; channel<_mChannels->size(); ++channel)
  {
    QObject::connect
             (
               getCameraChannel(channel)->_mpBackendType.get(),
               SIGNAL(valueChanged()),
               this,
               SLOT(backendChanged())
             );

    QObject::connect
             (
               getCameraChannel(channel).get(),
               SIGNAL(changeCamera()),
               this,
               SLOT(settingChanged())
             );

    QObject::connect
             (
               getCameraChannel(channel).get(),
               SIGNAL(changeSetting()),
               this,
               SLOT(settingChanged())
             );
  }
}


cedar::dev::sensors::camera::Grabber::~Grabber()
{
  doCleanUp();
  cedar::aux::LogSingleton::getInstance()->freeing(this);
}

//--------------------------------------------------------------------------------------------------------------------
// slots
//--------------------------------------------------------------------------------------------------------------------

void cedar::dev::sensors::camera::Grabber::channelAdded(int) // int index
{
  this->connectSignals();
}


void cedar::dev::sensors::camera::Grabber::backendChanged()
{
  // update visible fields and create a new backend
  unsigned int num_channels = getNumChannels();
  for (unsigned int channel = 0; channel < num_channels; ++channel)
  {
    getCameraChannel(channel)->setBackendType(getCameraChannel(channel)->_mpBackendType->getValue());
  }

  //if already created, apply used parameter
  if (this->isCreated())
  {
    // delete and recreate all channels
    this->applyParameter();
    emit frameSizeChanged();
  }
}


void cedar::dev::sensors::camera::Grabber::settingChanged()
{
  // Settings have changed. This slot is only invoked if the capture-device is already created.
  // recreate grabber
  this->applyParameter(); // don't use this, because it sets mode to default
  emit frameSizeChanged();
}


//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::dev::sensors::camera::Grabber::onCloseGrabber()
{
  // close all captures
  unsigned int num_channels = getNumChannels();
  for (unsigned int channel = 0; channel < num_channels; ++channel)
  {
    getCameraChannel(channel)->mVideoCapture = cv::VideoCapture();
  }
}


void cedar::dev::sensors::camera::Grabber::onCreateGrabber()
{
  unsigned int num_channels = getNumChannels();

  if (this->isCreated())
  {
    this->closeGrabber();
  }

  // create capture device, which depends on the chosen backend
  for (unsigned int channel = 0; channel < num_channels; ++channel)
  {
    try
    {
      // backend already created on initialization, but destroyed on onApplyParameter()/onCreateGrabber()
      // so create it again, if it was already destroyed
      if (!getCameraChannel(channel)->mpBackend)
      {
       //throws a cedar::dev::sensors::camera::CreateBackendException
       getCameraChannel(channel)->createBackend();
      }

      // create capture device
      //throws a cedar::dev::sensors::camera::CreateBackendException
      getCameraChannel(channel)->mpBackend->createCaptureBackend();
      setChannelInfoString(channel,this->onGetSourceInfo(channel));

      // Backend device not longer used. Channel::mVideoCapture does the job
      getCameraChannel(channel)->mpBackend.reset();
    }
    catch (cedar::dev::sensors::camera::CreateBackendException& e)
    {
      std::string msg = this->getName() + " Channel " + cedar::aux::toString(channel)
                          + ": " + e.getMessage();
      CEDAR_THROW(cedar::dev::sensors::visual::CreateGrabberException,msg)
    }
  }
}


void cedar::dev::sensors::camera::Grabber::setDecodeFilter
(
  cedar::dev::sensors::camera::Decoding::Id filterId
)
{
  this->getCameraChannel(0)->_mpDecodeFilter->setValue(filterId);
}


void cedar::dev::sensors::camera::Grabber::setDecodeFilter
(
  unsigned int channel,
  cedar::dev::sensors::camera::Decoding::Id filterId
)
{
  if (channel >= getNumChannels())
  {
    CEDAR_THROW(cedar::aux::IndexOutOfRangeException,buildChannelErrorMessage(channel));
  }
  this->getCameraChannel(channel)->_mpDecodeFilter->setValue(filterId);
}


cedar::dev::sensors::camera::Decoding::Id
  cedar::dev::sensors::camera::Grabber::getDecodeFilter(unsigned int channel) const
{
  if (channel >= getNumChannels())
  {
    CEDAR_THROW(cedar::aux::IndexOutOfRangeException,buildChannelErrorMessage(channel));
  }
  return this->getCameraChannel(channel)->_mpDecodeFilter->getValue();
}


std::vector<std::string> cedar::dev::sensors::camera::Grabber::getAllSettings(unsigned int channel)
{
  if (channel >= getNumChannels())
  {
    CEDAR_THROW(cedar::aux::IndexOutOfRangeException,buildChannelErrorMessage(channel));
  }

  cedar::dev::sensors::camera::ConstChannelPtr p_channel = this->getCameraChannel(channel);

  std::vector<std::string> settings;
  settings.push_back("all settings of channel " + cedar::aux::toString(channel) + ":");


  std::string s;
  s = VideoMode::type().get(p_channel->getVideoMode()).prettyString();
  settings.push_back("camera video mode:\t" + s);

  cv::Size size = this->getCameraFrameSize(channel);
  s = cedar::aux::toString(size.width) + " x "+ cedar::aux::toString(size.height);
  settings.push_back("camera framesize:\t" + s);

#ifdef CEDAR_USE_LIB_DC1394
  if (getCameraChannel(channel)->_mpBackendType->getValue() == cedar::dev::sensors::camera::BackendType::DC1394)
  {
    s = cedar::dev::sensors::camera::FrameRate::type().get(p_channel->getFramerate()).prettyString();
    settings.push_back("camera FPS mode:\t" + s);

    s = cedar::dev::sensors::camera::IsoSpeed::type().get(p_channel->getIsoSpeed()).prettyString();
    settings.push_back("camera ISO-speed mode:\t" + s);
  }
#endif // CEDAR_USE_LIB_DC1394

  return settings;
}


std::vector<std::string> cedar::dev::sensors::camera::Grabber::getAllProperties(unsigned int channel)
{
  if (channel >= getNumChannels())
  {
    CEDAR_THROW(cedar::aux::IndexOutOfRangeException,buildChannelErrorMessage(channel));
  }

  // write out all properties as set in our structure as well as the values directly read from cam
  std::vector<std::string> properties;
  properties.push_back("All properties of channel " + cedar::aux::toString(channel) + ":");

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

    properties.push_back("\tId: " + cedar::aux::toString(prop_id));
    properties.push_back("\tName: " + prop_name);
    properties.push_back("\tDescription: " + prop_description);
    properties.push_back("\tValue: " + prop_value_text.str());
    properties.push_back("\tRaw-Value: " + cedar::aux::toString(prop_raw_value));
  }

  return properties;
}


void cedar::dev::sensors::camera::Grabber::onGrab(unsigned int channel)
{
  // lock videocapture object
  QWriteLocker locking(getCameraChannel(channel)->mpVideoCaptureLock);

  // read the frame
  if (!getCameraChannel(channel)->mVideoCapture.read(getImageMat(channel)))
  {
    std::string msg = "Could not read from cv::VideoCapture on channel " + cedar::aux::toString(channel);
    CEDAR_THROW(cedar::dev::sensors::visual::GrabberGrabException,msg)
  }

  // check if conversion from bayer-pattern to cv::Mat BGR format is needed
  cedar::dev::sensors::camera::Decoding::Id debayer_fiter;
  debayer_fiter = this->getCameraChannel(channel)->_mpDecodeFilter->getValue();

  if (debayer_fiter != cedar::dev::sensors::camera::Decoding::NONE)
  {
    cv::cvtColor(getImageMat(channel),getImageMat(channel),debayer_fiter);
  }

  // unlock object done by QWriteLocker destructor
}


//----------------------------------------------------------------------------------------------------
std::string cedar::dev::sensors::camera::Grabber::onGetSourceInfo(unsigned int channel)
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

  return ss.str();
}


void cedar::dev::sensors::camera::Grabber::onCleanUp()
{
  // close all captures
  this->onCloseGrabber();
}


unsigned int cedar::dev::sensors::camera::Grabber::getCameraId(unsigned int channel) const
{
  if (channel >= getNumChannels())
  {
    CEDAR_THROW(cedar::aux::IndexOutOfRangeException,buildChannelErrorMessage(channel));
  }
  return getCameraChannel(channel)->getCameraId();
}


bool cedar::dev::sensors::camera::Grabber::isGuid(unsigned int channel) const
{
  if (channel >= getNumChannels())
  {
    CEDAR_THROW(cedar::aux::IndexOutOfRangeException,buildChannelErrorMessage(channel));
  }
  return getCameraChannel(channel)->getByGuid();
}


double cedar::dev::sensors::camera::Grabber::getProperty(unsigned int channel, Property::Id propId)
{
  if (channel >= getNumChannels())
  {
    CEDAR_THROW(cedar::aux::IndexOutOfRangeException, buildChannelErrorMessage(channel));
  }

  cedar::dev::sensors::camera::PropertiesPtr p_prop = this->getCameraChannel(channel)->mpProperties;
  double value = p_prop->getProperty(propId);

  if (value == CAMERA_PROPERTY_NOT_SUPPORTED)
  {
    std::string prop_name = Property::type().get(propId).prettyString();
    cedar::aux::LogSingleton::getInstance()->warning
                                             (
                                               this->getName() + ": Property " + prop_name + "is not supported!",
                                               "cedar::dev::sensors::camera::Grabber::getProperty()"
                                             );
  }
  return value;
}


double cedar::dev::sensors::camera::Grabber::getPropertyValue(unsigned int channel,Property::Id propId)
{
  if (channel >= getNumChannels())
  {
    CEDAR_THROW(cedar::aux::IndexOutOfRangeException,buildChannelErrorMessage(channel));
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


double cedar::dev::sensors::camera::Grabber::getProperty(Property::Id propId)
{
  return getProperty(0,propId);
}


void cedar::dev::sensors::camera::Grabber::setProperty
(
  unsigned int channel,
  Property::Id propId,
  double value
)
{
  if (channel >= getNumChannels())
  {
    CEDAR_THROW(cedar::aux::IndexOutOfRangeException,buildChannelErrorMessage(channel));
  }

  // throws cedar::dev::sensors::camera::PropertyNotSetException if property could not set
  getCameraChannel(channel)->mpProperties->setProperty(propId, value);
}


cedar::dev::sensors::camera::PropertyMode::Id cedar::dev::sensors::camera::Grabber::getPropertyMode
(
  unsigned int channel,
  cedar::dev::sensors::camera::Property::Id propId
) const
{
  return getCameraChannel(channel)->mpProperties->getPropertyMode(propId);
}


void cedar::dev::sensors::camera::Grabber::setPropertyMode
(
  unsigned int channel,
  cedar::dev::sensors::camera::Property::Id propId,
  cedar::dev::sensors::camera::PropertyMode::Id modeId
)
{
  // throws cedar::dev::sensors::camera::PropertyNotSetException if property could not set
  getCameraChannel(channel)->mpProperties->setPropertyMode(propId, modeId);
}


void cedar::dev::sensors::camera::Grabber::setProperty(Property::Id propId, double value)
{
  setProperty(0,propId, value);
}


void cedar::dev::sensors::camera::Grabber::setCameraVideoMode
(
  unsigned int channel,
  cedar::dev::sensors::camera::VideoMode::Id modeId
)
{
  this->getCameraChannel(channel)->setVideoMode(modeId);
}


cedar::dev::sensors::camera::VideoMode::Id
  cedar::dev::sensors::camera::Grabber::getCameraVideoMode(unsigned int channel) const
{
  return this->getCameraChannel(channel)->getVideoMode();
}


void cedar::dev::sensors::camera::Grabber::setCameraFramerate(unsigned int channel,FrameRate::Id fpsId)
{
  this->getCameraChannel(channel)->setFramerate(fpsId);
}


cedar::dev::sensors::camera::FrameRate::Id
  cedar::dev::sensors::camera::Grabber::getCameraFramerate(unsigned int channel) const
{
  return this->getCameraChannel(channel)->getFramerate();
}


cv::Size cedar::dev::sensors::camera::Grabber::getCameraFrameSize( unsigned int channel)
{
  cv::Size size;
  size.width = getPropertyFromCamera(channel, cedar::dev::sensors::camera::Setting::FRAME_WIDTH);
  size.height = getPropertyFromCamera(channel, cedar::dev::sensors::camera::Setting::FRAME_HEIGHT);
  return size;
}


double cedar::dev::sensors::camera::Grabber::getPropertyFromCamera(unsigned int channel, unsigned int propertyId)
{
  if (channel >= getNumChannels())
  {
    CEDAR_THROW(cedar::aux::IndexOutOfRangeException,buildChannelErrorMessage(channel));
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

//----------------------------------------------------------------------------------------------------------------------
// methods used only with firewire cameras
//----------------------------------------------------------------------------------------------------------------------

#ifdef CEDAR_USE_LIB_DC1394

void cedar::dev::sensors::camera::Grabber::setCameraIsoSpeed(unsigned int channel,IsoSpeed::Id isoSpeedId)
{
  this->getCameraChannel(channel)->setIsoSpeed(isoSpeedId);
}


cedar::dev::sensors::camera::IsoSpeed::Id
  cedar::dev::sensors::camera::Grabber::getCameraIsoSpeed(unsigned int channel) const
{
  return this->getCameraChannel(channel)->getIsoSpeed();
}
#endif
