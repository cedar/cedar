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

    File:        CameraBackendType.h

    Maintainer:  Georg Hartinger
    Email:       georg.hartinger@ini.rub.de
    Date:        2012 07 04

    Description:  Implementation for the cedar::dev::sensors::visual::CameraDevice class

    Credits:

======================================================================================================================*/

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/devices/sensors/visual/camera/CameraSettings.h"
#include "cedar/devices/sensors/visual/CameraGrabber.h"
#include "cedar/auxiliaries/casts.h"


// SYSTEM INCLUDES

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------
cedar::dev::sensors::visual::CameraSettings::CameraSettings
(
  unsigned int cameraId,
  bool byGuid,
  cedar::dev::sensors::visual::CameraBackendType::Id backendType
)
:
cedar::aux::Configurable(),
mBackendType(-1),
_mpByGuid(new cedar::aux::BoolParameter(this,"by GUID",byGuid)),
_mpCameraId(new cedar::aux::UIntParameter(this,"camera ID",cameraId)),
_mpGrabMode(new cedar::aux::EnumParameter
                (
                  this,
                  "frame mode",
                  cedar::dev::sensors::visual::CameraVideoMode::typePtr(),
                  cedar::dev::sensors::visual::CameraVideoMode::MODE_NOT_SET
                )
            ),
_mpFPS(new cedar::aux::EnumParameter
           (
             this,
             "fps",
             cedar::dev::sensors::visual::CameraFrameRate::typePtr(),
             cedar::dev::sensors::visual::CameraFrameRate::FPS_15
           )
       )
#ifdef CEDAR_USE_LIB_DC1394
,
_mpIsoSpeed(new cedar::aux::EnumParameter
                (
                  this,
                  "iso speed",
                  cedar::dev::sensors::visual::CameraIsoSpeed::typePtr(),
                  cedar::dev::sensors::visual::CameraIsoSpeed::ISO_200
                )
            )
#endif
{
  this->setBackendType(backendType);

  QObject::connect(_mpByGuid.get(),SIGNAL(valueChanged()),this,SLOT(settingChanged()));
  QObject::connect(_mpCameraId.get(),SIGNAL(valueChanged()),this,SLOT(settingChanged()));
  QObject::connect(_mpGrabMode.get(),SIGNAL(valueChanged()),this,SLOT(settingChanged()));
  QObject::connect(_mpFPS.get(),SIGNAL(valueChanged()),this,SLOT(settingChanged()));

#ifdef CEDAR_USE_LIB_DC1394
  QObject::connect(_mpIsoSpeed.get(),SIGNAL(valueChanged()),this,SLOT(settingChanged()));
#endif

}

cedar::dev::sensors::visual::CameraSettings::~CameraSettings()
{
}


//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------


void cedar::dev::sensors::visual::CameraSettings::setBackendType
(
  cedar::dev::sensors::visual::CameraBackendType::Id backendType
)
{
//  std::cout << "setParametersFromBackend" << std::endl;
  if (mBackendType != backendType)
  {
    mBackendType = backendType;

    // switch on backendtype and fill the default values for settings
    switch (mBackendType)
    {

  #ifdef CEDAR_USE_LIB_DC1394
      case cedar::dev::sensors::visual::CameraBackendType::DC1394:
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
      case cedar::dev::sensors::visual::CameraBackendType::VFL:
  #endif // CEDAR_USE_VIDEO_FOR_LINUX

      case cedar::dev::sensors::visual::CameraBackendType::AUTO:
      case cedar::dev::sensors::visual::CameraBackendType::CVCAPTURE:
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


}


void cedar::dev::sensors::visual::CameraSettings::settingChanged()
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

unsigned int cedar::dev::sensors::visual::CameraSettings::getCameraId()
{
  return _mpCameraId->getValue();
}

bool cedar::dev::sensors::visual::CameraSettings::getByGuid()
{
  return _mpByGuid->getValue();
}

cedar::dev::sensors::visual::CameraVideoMode::Id cedar::dev::sensors::visual::CameraSettings::getVideoMode()
{
  return _mpGrabMode->getValue();
}

cedar::dev::sensors::visual::CameraFrameRate::Id cedar::dev::sensors::visual::CameraSettings::getFPS()
{
  return _mpFPS->getValue();
}

void cedar::dev::sensors::visual::CameraSettings::setCameraId(unsigned int CameraId, bool isGuid)
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


void cedar::dev::sensors::visual::CameraSettings::setVideoMode(cedar::dev::sensors::visual::CameraVideoMode::Id videoMode)
{
  if (_mpGrabMode->getValue() != videoMode)
  {
    _mpGrabMode->setValue(videoMode);
  }
}

void cedar::dev::sensors::visual::CameraSettings::setFPS(cedar::dev::sensors::visual::CameraFrameRate::Id fps)
{
  if (_mpFPS->getValue() != fps)
  {
    _mpFPS->setValue(fps);
  }
}


#ifdef CEDAR_USE_LIB_DC1394

cedar::dev::sensors::visual::CameraIsoSpeed::Id cedar::dev::sensors::visual::CameraSettings::getIsoSpeed()
{
  return _mpIsoSpeed->getValue();
}

void cedar::dev::sensors::visual::CameraSettings::setIsoSpeed(cedar::dev::sensors::visual::CameraIsoSpeed::Id isoSpeed)
{
  if (_mpIsoSpeed->getValue() != isoSpeed)
  {
    _mpIsoSpeed->setValue(isoSpeed);
  }
}

#endif // CEDAR_USE_LIB_DC1394


void cedar::dev::sensors::visual::CameraSettings::hideFwVideoModes()
{
  // Add all properties to the properties-list
  int num_modes = cedar::dev::sensors::visual::CameraVideoMode::type().list().size();
  for (int i=0; i<num_modes; i++)
  {
    cedar::dev::sensors::visual::CameraVideoMode::Id prop_id
      = cedar::dev::sensors::visual::CameraVideoMode::type().list().at(i).id();

    if (prop_id > cedar::dev::sensors::visual::CameraVideoMode::NUM_9)
    {
      //  _mpGrabMode->disable(cedar::dev::sensors::visual::CameraPropertyMode::AUTO);
        _mpGrabMode->disable(prop_id);
    }
    _mpGrabMode->enable(cedar::dev::sensors::visual::CameraVideoMode::MODE_NOT_SET);
  }
}

