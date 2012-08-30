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
#include "cedar/devices/sensors/visual/camera/enums/CameraVideoMode.h"
#include "cedar/devices/sensors/visual/camera/enums/CameraFrameRate.h"
#include "cedar/devices/sensors/visual/camera/enums/CameraIsoSpeed.h"
#include "cedar/devices/sensors/visual/CameraGrabber.h"


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
mCameraId(cameraId),
_mpByGuid(new cedar::aux::BoolParameter(this,"by GUID",byGuid)),
_mpBusId(new cedar::aux::UIntParameter(this,"bus ID",cameraId,0,255)),
_mpGuid(new cedar::aux::UIntParameter(this,"GUID",cameraId)),
mpWidth(new cedar::aux::UIntParameter(this,"frame width",0)),
mpHeight(new cedar::aux::UIntParameter(this,"frame height",0)),
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
  this->setParametersFromBackend(backendType);

  //QObject::connect(_mpByGuid.get(),SIGNAL(valueChanged()),this,SIGNAL(settingsChanged()));
  QObject::connect(_mpByGuid.get(),SIGNAL(valueChanged()),this,SLOT(settingChanged()));
  QObject::connect(_mpBusId.get(),SIGNAL(valueChanged()),this,SLOT(settingChanged()));
  QObject::connect(_mpGuid.get(),SIGNAL(valueChanged()),this,SLOT(settingChanged()));
  QObject::connect(mpWidth.get(),SIGNAL(valueChanged()),this,SLOT(settingChanged()));
  QObject::connect(mpHeight.get(),SIGNAL(valueChanged()),this,SLOT(settingChanged()));
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


void cedar::dev::sensors::visual::CameraSettings::setParametersFromBackend
(
  cedar::dev::sensors::visual::CameraBackendType::Id backendType
)
{

  if (mBackendType != backendType)
  {
    
    //save former camId
    if(_mpByGuid->getValue())
    {
      mCameraId = _mpGuid->getValue();
    }
    else
    {
      mCameraId = _mpBusId->getValue();
    }

    mBackendType = backendType;

    // switch on backendtype and fill the default values for settings
    switch (mBackendType)
    {

  #ifdef CEDAR_USE_LIB_DC1394
      case cedar::dev::sensors::visual::CameraBackendType::DC1394:
      {
        if (_mpByGuid->getValue())
        {
          _mpBusId->setValue(0);
          _mpBusId->setHidden(true);
          _mpGuid->setValue(mCameraId);
        }
        else
        {
          _mpBusId->setValue(mCameraId);
          _mpGuid->setValue(0);
          _mpGuid->setHidden(true);
        }

        // set visible of grab-mode to true
        // disable unavailable grab-modes (depends on cam)
        // disable unavailable fps (depends on mode)


        // set visible of width and height to false
        // mpHeight->setVisible(false);
        // mpWidth->setVisible(false);

        // connect signals for _mpGrabMode, _mpIsoSpeed

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
        _mpBusId->setValue(mCameraId);
        _mpGuid->setValue(0);
        _mpGuid->setHidden(true);

  #ifdef CEDAR_USE_LIB_DC1394
        //disable all grab-modes with firewire (if they are here)
  #endif

        // set visible of grab-mode to false
        // enable width and height

      }
      
    } //end switch
  } //end changed backend


}


void cedar::dev::sensors::visual::CameraSettings::settingChanged()
{
  emit settingsChanged();
}

unsigned int cedar::dev::sensors::visual::CameraSettings::getBusId()
{
  return _mpBusId->getValue();
}
unsigned int cedar::dev::sensors::visual::CameraSettings::getGuid()
{
  return _mpGuid->getValue();
}

bool cedar::dev::sensors::visual::CameraSettings::getByGuid()
{
  return _mpByGuid->getValue();
}



//void cedar::dev::sensors::visual::CameraSettings::connectSignals
//(
//  cedar::dev::sensors::visual::CameraGrabber* pCameraGrabber
//)
//{
////  QObject::connect(mpWidth.get(),SIGNAL(valueChanged()),pCameraGrabber,SLOT(cameraChanged()));
////  QObject::connect(mpHeight.get(),SIGNAL(valueChanged()),pCameraGrabber,SLOT(cameraChanged()));
////  QObject::connect(_mpGrabMode.get(),SIGNAL(valueChanged()),pCameraGrabber,SLOT(cameraChanged()));
////
////#ifdef CEDAR_USE_LIB_DC1394
////  QObject::connect(_mpIsoSpeed.get(),SIGNAL(valueChanged()),pCameraGrabber,SLOT(cameraChanged()));
////#endif
//}


   /*

//--------------------------------------------------------------------------------------------------------------------
bool cedar::dev::sensors::visual::CameraProperties::setSetting(cedar::dev::sensors::visual::CameraSetting::Id settingId, double value)
{

  //have to be switched over to signal handler !!!

  unsigned int prop_id = static_cast<unsigned int>(settingId);
  bool result = this->setPropertyRawValue(prop_id,value);
  unsigned int ivalue = static_cast<unsigned int>(value);

//  switch (settingId)
//  {
//    case cedar::dev::sensors::visual::CameraSetting::FPS :
//      //mCamSettings.fps = cedar::dev::sensors::visual::CameraFrameRate::type().get(ivalue).name();
//    case cedar::dev::sensors::visual::CameraSetting::ISO_SPEED :
//      //mCamSettings.iso_speed = cedar::dev::sensors::visual::CameraIsoSpeed::type().get(ivalue).name();
//    case cedar::dev::sensors::visual::CameraSetting::MODE :
//      //mCamSettings.mode = cedar::dev::sensors::visual::CameraVideoMode::type().get(ivalue).name();
//  }
//  return result;
  return false;
}

//--------------------------------------------------------------------------------------------------------------------
double cedar::dev::sensors::visual::CameraProperties::getSetting(CameraSetting::Id settingId)
{
//  switch (settingId)
//  {
//  case cedar::dev::sensors::visual::CameraSetting::FPS :
//    return static_cast<double>(CameraFrameRate::type().get(mCamSettings.fps).id());
//  case cedar::dev::sensors::visual::CameraSetting::ISO_SPEED :
//    return static_cast<double>(CameraIsoSpeed::type().get(mCamSettings.iso_speed).id());
//  case cedar::dev::sensors::visual::CameraSetting::MODE :
//    return static_cast<double>(CameraVideoMode::type().get(mCamSettings.mode).id());
//  case cedar::dev::sensors::visual::CameraSetting::FRAME_HEIGHT :
//    return getRawProperty(CameraSetting::FRAME_HEIGHT);
//  case cedar::dev::sensors::visual::CameraSetting::FRAME_WIDTH :
//    return getRawProperty(CameraSetting::FRAME_WIDTH);
//  default :
//    return CAMERA_PROPERTY_NOT_SUPPORTED;
//  }
  return 0.0f;
}
*/
