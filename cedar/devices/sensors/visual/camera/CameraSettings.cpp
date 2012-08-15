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

// SYSTEM INCLUDES

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------
cedar::dev::sensors::visual::CameraSettings::CameraSettings(cedar::dev::sensors::visual::CameraBackendType::Id backendType)
:
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

  // switch on backendtype and fill the settings
  switch (backendType)
  {
    
#ifdef CEDAR_USE_LIB_DC1394
    case cedar::dev::sensors::visual::CameraBackendType::DC1394:
    {
      //set visible of grab-mode to true
      //disable unavailable grab-modes (depends on cam)
      //disable unavailable fps (depends on mode)
      
      
      //set visible of width and heigt to false
      //mpHeight->setVisible(false);
      //mpWidth->setVisible(false);
      
      //connect signals for _mpGrabMode, _mpIsoSpeed

    }
    break;
#endif
    case cedar::dev::sensors::visual::CameraBackendType::VFL:
    case cedar::dev::sensors::visual::CameraBackendType::AUTO:
    case cedar::dev::sensors::visual::CameraBackendType::CVCAPTURE:
    default:
    {
    
#ifdef CEDAR_USE_LIB_DC1394
      //disable all grab-modes with firewire (if they are here)
#endif     

      // set visible of grab-mode to false
      // enable width and height


      //connect signals for mpWidth, mpHeight
    }
    
  } //end switch
  
  //connect signals for  _mpGrabMode, _mpFPS
  
  
  

}

cedar::dev::sensors::visual::CameraSettings::~CameraSettings()
{
}


//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------
//cedar::dev::sensors::visual::CameraSettingsSet& cedar::dev::sensors::visual::CameraSettings::getSettings()
//{
//  return mSettings;
//}

bool cedar::dev::sensors::visual::CameraSettings::setSetting(CameraSetting::Id settingId, double value)
{

}

double cedar::dev::sensors::visual::CameraSettings::getSetting(CameraSetting::Id settingId)
{

}
