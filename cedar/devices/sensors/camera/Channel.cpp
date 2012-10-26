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
#include "cedar/devices/sensors/camera/backends/DeviceCvVideoCapture.h"
#include "cedar/devices/sensors/camera/backends/DeviceDc1394.h"
#include "cedar/devices/sensors/camera/backends/DeviceVfl.h"

// SYSTEM INCLUDES

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
mBackendType(-1),
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
             cedar::dev::sensors::camera::FrameRate::FPS_15
           )
       )
#ifdef CEDAR_USE_LIB_DC1394
,
_mpIsoSpeed(new cedar::aux::EnumParameter
                (
                  this,
                  "iso speed",
                  cedar::dev::sensors::camera::IsoSpeed::typePtr(),
                  cedar::dev::sensors::camera::IsoSpeed::ISO_200
                )
            )
#endif
{

  // create properties
  mpProperties = cedar::dev::sensors::camera::PropertiesPtr
               (
                 new cedar::dev::sensors::camera::Properties(this,mVideoCapture,mpVideoCaptureLock)
               );

  QObject::connect(_mpByGuid.get(),SIGNAL(valueChanged()),this,SLOT(settingChanged()));
  QObject::connect(_mpCameraId.get(),SIGNAL(valueChanged()),this,SLOT(settingChanged()));
  QObject::connect(_mpGrabMode.get(),SIGNAL(valueChanged()),this,SLOT(settingChanged()));
  QObject::connect(_mpFPS.get(),SIGNAL(valueChanged()),this,SLOT(settingChanged()));

#ifdef CEDAR_USE_LIB_DC1394
  QObject::connect(_mpIsoSpeed.get(),SIGNAL(valueChanged()),this,SLOT(settingChanged()));
#endif

}

  /// Destructor
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

//called from gui
void cedar::dev::sensors::camera::Channel::setBackendType
(
  cedar::dev::sensors::camera::BackendType::Id backendType
)
{
 // std::cout << "cedar::dev::sensors::camera::Channel::setBackendType ";
  if (mBackendType != backendType)
  {
   // std::cout << "change to " << cedar::dev::sensors::camera::BackendType::type().get(backendType).prettyString() << std::endl;

    mBackendType = backendType;

    this->createBackend();

    // switch on backendtype and fill the default values for settings
    switch (mBackendType)
    {

  #ifdef CEDAR_USE_LIB_DC1394
      case cedar::dev::sensors::camera::BackendType::DC1394:
      {
        _mpByGuid->setHidden(false);

        // set visible of grab-mode to true
        // disable unavailable grab-modes (depends on cam)
        // disable unavailable fps (depends on mode)

        _mpIsoSpeed->setHidden(false);

        // show all modes
        // show only available modes
        _mpGrabMode->enableAll();

        // set visible of fps to false, fps is only used in a console-program or for firewire cameras
        _mpFPS->setHidden(false);

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

        // set visible of fps to false, fps is only used in a console-program or for firewire cameras
        _mpFPS->setHidden(true);


  #ifdef CEDAR_USE_LIB_DC1394
        // disable all grab-modes with firewire (if they are here)
        hideFwVideoModes();
        _mpIsoSpeed->setHidden(true);

  #endif

        // set visible of grab-mode to false

      }

    } // end switch
  } // end changed backend
  else
  {
    //std::cout << "nothing changed" << std::endl;
  }

}


void cedar::dev::sensors::camera::Channel::settingChanged()
{
  // get sender and test if "byGUID" is changed
/*  cedar::aux::ParameterPtr p_sender
    = cedar::aux::ParameterPtr(cedar::aux::asserted_cast<cedar::aux::Parameter * >(QObject::sender()));

  // set BusID or GUID visible
  if (p_sender = _mpByGuid.get())
  {
  }*/

  emit settingsChanged();
}

unsigned int cedar::dev::sensors::camera::Channel::getCameraId()
{
  return _mpCameraId->getValue();
}

bool cedar::dev::sensors::camera::Channel::getByGuid()
{
  return _mpByGuid->getValue();
}

cedar::dev::sensors::camera::VideoMode::Id cedar::dev::sensors::camera::Channel::getVideoMode()
{
  return _mpGrabMode->getValue();
}

cedar::dev::sensors::camera::FrameRate::Id cedar::dev::sensors::camera::Channel::getFPS()
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

void cedar::dev::sensors::camera::Channel::setFPS(cedar::dev::sensors::camera::FrameRate::Id fps)
{
  if (_mpFPS->getValue() != fps)
  {
    _mpFPS->setValue(fps);
  }
}


#ifdef CEDAR_USE_LIB_DC1394

cedar::dev::sensors::camera::IsoSpeed::Id cedar::dev::sensors::camera::Channel::getIsoSpeed()
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
  // Add all properties to the properties-list
  int num_modes = cedar::dev::sensors::camera::VideoMode::type().list().size();
  for (int i=0; i<num_modes; i++)
  {
    cedar::dev::sensors::camera::VideoMode::Id prop_id
      = cedar::dev::sensors::camera::VideoMode::type().list().at(i).id();

    if (prop_id > cedar::dev::sensors::camera::VideoMode::NUM_9)
    {
      //  _mpGrabMode->disable(cedar::dev::sensors::camera::PropertyMode::AUTO);
        _mpGrabMode->disable(prop_id);
    }
    _mpGrabMode->enable(cedar::dev::sensors::camera::VideoMode::MODE_NOT_SET);
  }
}

void cedar::dev::sensors::camera::Channel::createBackend()
{
  if (mpBackend)
  {
    mpBackend.reset();
  }

  switch ( _mpBackendType->getValue().id() )
  {



  #ifdef CEDAR_USE_LIB_DC1394
    case cedar::dev::sensors::camera::BackendType::DC1394 :
    {
      cedar::dev::sensors::camera::DeviceDc1394Ptr dc1394_device
                                                   (
                                                     new cedar::dev::sensors::camera::DeviceDc1394
                                                         (
//                                                           shared_from_this()
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
      cedar::dev::sensors::camera::DeviceCvVideoCapturePtr cv_device
                                                           (
                                                             new cedar::dev::sensors::camera::DeviceCvVideoCapture
                                                             (
//                                                              shared_from_this()
                                                               this
                                                             )
                                                           );
      mpBackend = cv_device;
    }
  } // switch on backend

  mpBackend->initDevice();

}
