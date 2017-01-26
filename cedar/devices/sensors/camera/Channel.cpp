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
    along with cedar. If not, see <http://www.gnu.org/licenses/>.

========================================================================================================================

    Institute:   Ruhr-Universitaet Bochum
                 Institut fuer Neuroinformatik

    File:        CameraSettings.cpp

    Maintainer:  Georg Hartinger
    Email:       georg.hartinger@ini.rub.de
    Date:        2012 07 04

    Description:  Implementation for the cedar::dev::sensors::camera::Settings class

    Credits:

======================================================================================================================*/

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/devices/sensors/camera/Channel.h"
#include "cedar/auxiliaries/casts.h"
#include "cedar/devices/sensors/camera/backends/BackendType.h"
#include "cedar/devices/sensors/camera/backends/BackendCvVideoCapture.h"
#include "cedar/devices/sensors/camera/backends/BackendDc1394.h"
#include "cedar/devices/sensors/camera/backends/BackendV4L.h"

// SYSTEM INCLUDES
#include <iostream>

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------
cedar::dev::sensors::camera::Channel::Channel
(
  unsigned int cameraId,
  bool byGuid,
  cedar::dev::sensors::camera::BackendType::Id backendType,
  cedar::dev::sensors::camera::Decoding::Id decodeFilter
)
:
cedar::aux::Configurable(),
QObject(),
cedar::dev::sensors::visual::GrabberChannel(),
mpVideoCaptureLock(new QReadWriteLock()),
mBackendType(cedar::dev::sensors::camera::BackendType::CVCAPTURE), //will be set to auto
_mpBackendType(new cedar::aux::EnumParameter
                (
                  this,
                  "backend type",
                  cedar::dev::sensors::camera::BackendType::typePtr(),
                  backendType
                )
              ),
_mpDecodeFilter(new cedar::aux::EnumParameter
                 (
                   this,
                   "decoding",
                   cedar::dev::sensors::camera::Decoding::typePtr(),
                   decodeFilter
                 )
               ),
_mpByGuid(new cedar::aux::BoolParameter(this,"by GUID",byGuid)),
_mpCameraId(new cedar::aux::UIntParameter(this,"camera ID",cameraId)),
_mpGrabMode(new cedar::aux::EnumParameter
                (
                  this,
                  "frame mode",
                  cedar::dev::sensors::camera::VideoMode::typePtr(),
                  cedar::dev::sensors::camera::VideoMode::MODE_NOT_SET
                )
            ),
_mpFPS(new cedar::aux::EnumParameter
           (
             this,
             "camera fps",
             cedar::dev::sensors::camera::FrameRate::typePtr(),
             cedar::dev::sensors::camera::FrameRate::FPS_NOT_SET //FPS_15
           )
       )
#ifdef CEDAR_USE_LIB_DC1394
,
_mpIsoSpeed(new cedar::aux::EnumParameter
                (
                  this,
                  "iso speed",
                  cedar::dev::sensors::camera::IsoSpeed::typePtr(),
                  cedar::dev::sensors::camera::IsoSpeed::ISO_NOT_SET //ISO_200
                )
            )
#endif
{

  // create properties
  mpProperties = cedar::dev::sensors::camera::PropertiesPtr
               (
                 new cedar::dev::sensors::camera::Properties(this,mVideoCapture,mpVideoCaptureLock)
               );

  QObject::connect(_mpByGuid.get(),SIGNAL(valueChanged()),this,SLOT(deviceChanged()));
  QObject::connect(_mpCameraId.get(),SIGNAL(valueChanged()),this,SLOT(deviceChanged()));
  QObject::connect(_mpGrabMode.get(),SIGNAL(valueChanged()),this,SLOT(grabModeChanged()));
  QObject::connect(_mpFPS.get(),SIGNAL(valueChanged()),this,SLOT(deviceChanged()));

#ifdef CEDAR_USE_LIB_DC1394
  QObject::connect(_mpIsoSpeed.get(),SIGNAL(valueChanged()),this,SLOT(deviceChanged()));
#endif
}


//! Destructor
cedar::dev::sensors::camera::Channel::~Channel()
{
  if (mpVideoCaptureLock)
  {
    delete mpVideoCaptureLock;
    mpVideoCaptureLock = 0;
  }
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

//invoked with signal from gui or internal through setValue of the ObjectParameter
void cedar::dev::sensors::camera::Channel::setBackendType
(
  cedar::dev::sensors::camera::BackendType::Id backendType
)
{
  if (mBackendType != backendType)
  {
    mBackendType = backendType;

    this->createBackend();

    // switch on backendtype and fill the default values for settings
    switch (mBackendType)
    {

#ifdef CEDAR_USE_LIB_DC1394
      case cedar::dev::sensors::camera::BackendType::DC1394:
      {
        // the visibility of available grab-modes and framerates is already set in createBackend()

        // show firewire related properties
        _mpByGuid->setHidden(false);
        _mpIsoSpeed->setHidden(false);
        _mpFPS->setHidden(false);

        // additional: set "auto" available
        _mpFPS->enable(cedar::dev::sensors::camera::FrameRate::FPS_NOT_SET);
        _mpGrabMode->enable(cedar::dev::sensors::camera::VideoMode::MODE_NOT_SET);
        _mpIsoSpeed->enable(cedar::dev::sensors::camera::IsoSpeed::ISO_NOT_SET);
      }
      break;
#endif

#ifdef CEDAR_USE_VIDEO_FOR_LINUX
      case cedar::dev::sensors::camera::BackendType::VFL:
#endif // CEDAR_USE_VIDEO_FOR_LINUX

      case cedar::dev::sensors::camera::BackendType::AUTO:
      case cedar::dev::sensors::camera::BackendType::CVCAPTURE:
      default:
      {
        // ignore byGUID (default values)
        _mpByGuid->setHidden(true);

        // fps depends on the camera
        _mpFPS->setHidden(true);

#ifdef CEDAR_USE_LIB_DC1394
        // disable all firewire related grab-modes (if they are here)
        hideFwVideoModes();
        _mpIsoSpeed->setHidden(true);
#endif
      }
    } // switch

    //set default values on backend-switching
    //disable the change-signals to prevent double-processing
#ifdef CEDAR_USE_LIB_DC1394
    _mpIsoSpeed->blockSignals(true);
    _mpIsoSpeed->setValue(cedar::dev::sensors::camera::IsoSpeed::ISO_NOT_SET);
    _mpIsoSpeed->blockSignals(false);
#endif
    _mpFPS->blockSignals(true);
    _mpFPS->setValue(cedar::dev::sensors::camera::FrameRate::FPS_NOT_SET);
    _mpFPS->blockSignals(false);

    _mpGrabMode->blockSignals(true);
    _mpGrabMode->setValue(cedar::dev::sensors::camera::VideoMode::MODE_NOT_SET);
    _mpGrabMode->blockSignals(false);
  } // end changed backend
}


void cedar::dev::sensors::camera::Channel::grabModeChanged()
{
  //invoked when user changes the grabmode as well as the backend turns entries on or off
  if (mpBackend)
  {

#ifdef CEDAR_USE_LIB_DC1394
    if (_mpBackendType->getValue() == cedar::dev::sensors::camera::BackendType::DC1394)
    {
      cedar::dev::sensors::camera::BackendDc1394Ptr p_backend;
      p_backend = boost::static_pointer_cast<cedar::dev::sensors::camera::BackendDc1394>(mpBackend);

      //disable signals from fps:
      _mpFPS->blockSignals(true);
      p_backend->updateFps();
      _mpFPS->blockSignals(false);
    }
#endif
  }
  else
  {
    emit changeSetting();
  }
}


void cedar::dev::sensors::camera::Channel::deviceChanged()
{
  // recreate grabber when grabbing, otherwise nothing to do
  if (!mpBackend)
  {
    emit changeCamera();
  }
}


unsigned int cedar::dev::sensors::camera::Channel::getCameraId() const
{
  return _mpCameraId->getValue();
}


bool cedar::dev::sensors::camera::Channel::getByGuid() const
{
  return _mpByGuid->getValue();
}


cedar::dev::sensors::camera::VideoMode::Id cedar::dev::sensors::camera::Channel::getVideoMode() const
{
  return _mpGrabMode->getValue();
}


cedar::dev::sensors::camera::FrameRate::Id cedar::dev::sensors::camera::Channel::getFramerate()  const
{
  return _mpFPS->getValue();
}


void cedar::dev::sensors::camera::Channel::setCameraId(unsigned int CameraId, bool isGuid)
{
  if (_mpCameraId->getValue() != CameraId)
  {
    _mpCameraId->setValue(CameraId);
  }

  if (_mpByGuid->getValue() != isGuid)
  {
    _mpByGuid->setValue(isGuid);
  }
}


void cedar::dev::sensors::camera::Channel::setVideoMode(cedar::dev::sensors::camera::VideoMode::Id videoMode)
{
  if (_mpGrabMode->getValue() != videoMode)
  {
    _mpGrabMode->setValue(videoMode);
  }
}


void cedar::dev::sensors::camera::Channel::setFramerate(cedar::dev::sensors::camera::FrameRate::Id fps)
{
  if (_mpFPS->getValue() != fps)
  {
    _mpFPS->setValue(fps);
  }
}


#ifdef CEDAR_USE_LIB_DC1394
cedar::dev::sensors::camera::IsoSpeed::Id cedar::dev::sensors::camera::Channel::getIsoSpeed() const
{
  return _mpIsoSpeed->getValue();
}

void cedar::dev::sensors::camera::Channel::setIsoSpeed(cedar::dev::sensors::camera::IsoSpeed::Id isoSpeed)
{
  if (_mpIsoSpeed->getValue() != isoSpeed)
  {
    _mpIsoSpeed->setValue(isoSpeed);
  }
}
#endif // CEDAR_USE_LIB_DC1394


void cedar::dev::sensors::camera::Channel::hideFwVideoModes()
{
  // loop through all properties of the properties-list
  int num_modes = cedar::dev::sensors::camera::VideoMode::type().list().size();
  for (int i=0; i<num_modes; i++)
  {
    cedar::dev::sensors::camera::VideoMode::Id prop_id
      = cedar::dev::sensors::camera::VideoMode::type().list().at(i).id();

    if (prop_id > cedar::dev::sensors::camera::VideoMode::NUM_9)
    {
      _mpGrabMode->disable(prop_id);
    }
    _mpGrabMode->enable(cedar::dev::sensors::camera::VideoMode::MODE_NOT_SET);
  }
}

void cedar::dev::sensors::camera::Channel::createBackend()
{
#ifdef DEBUG_CAMERA_GRABBER
  std::cout << __PRETTY_FUNCTION__ << std::endl;
#endif

  //save old state and restore at the end of this method
  cedar::dev::sensors::camera::VideoMode::Id used_mode_id = _mpGrabMode->getValue();

  if (mpBackend)
  {
    mpBackend.reset();
  }

  switch (_mpBackendType->getValue().id())
  {

#ifdef CEDAR_USE_LIB_DC1394
    case cedar::dev::sensors::camera::BackendType::DC1394 :
    {
      cedar::dev::sensors::camera::BackendDc1394Ptr dc1394_device
                                                   (
                                                     new cedar::dev::sensors::camera::BackendDc1394
                                                         (
                                                           this
                                                         )
                                                   );
      mpBackend = dc1394_device;
      break;
    }
#endif

#ifdef CEDAR_USE_VIDEO_FOR_LINUX
    case cedar::dev::sensors::camera::BackendType::VFL :
#endif // CEDAR_USE_VIDEO_FOR_LINUX

    case cedar::dev::sensors::camera::BackendType::AUTO :
    case cedar::dev::sensors::camera::BackendType::CVCAPTURE :


    default:
    {
      cedar::dev::sensors::camera::BackendCvVideoCapturePtr cv_device
                                                           (
                                                             new cedar::dev::sensors::camera::BackendCvVideoCapture
                                                             (
                                                               this
                                                             )
                                                           );
      mpBackend = cv_device;
    }
  } // switch

  // throws a cedar::dev::sensors::camera::CreateBackendException
  mpBackend->init();

  //restore previous used mode - if available;
  if (_mpGrabMode->isEnabled(used_mode_id))
  {
    _mpGrabMode->setValue(used_mode_id);
  }
  else
  {
    _mpGrabMode->setValue(cedar::dev::sensors::camera::VideoMode::MODE_NOT_SET);
  }
}
