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
#include "cedar/devices/sensors/visual/camera/CameraProperties.h"

#include "cedar/devices/sensors/visual/camera/enums/CameraIsoSpeed.h"
#include "cedar/devices/sensors/visual/camera/enums/CameraProperty.h"
#include "cedar/devices/sensors/visual/camera/enums/CameraVideoMode.h"
#include "cedar/devices/sensors/visual/camera/enums/CameraFrameRate.h"
#include "cedar/devices/sensors/visual/camera/enums/CameraSetting.h"
#include "cedar/devices/sensors/visual/camera/enums/DeBayerFilter.h"

// SYSTEM INCLUDES

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------
cedar::dev::sensors::visual::CameraProperties::CameraProperties()
{

}

cedar::dev::sensors::visual::CameraProperties::~CameraProperties()
{
}


//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------
//cedar::dev::sensors::visual::CameraPropertiesSet& cedar::dev::sensors::visual::CameraProperties::getProperties()
//{
//  return mProperties;
//}

//bool cedar::dev::sensors::visual::CameraProperties::setProperty(unsigned int property_id,double value)
//{
//  //set the value with respect to concurrent access
//
//  //get old value
//  //set new value only if it is different
//  //check if value is set to the right value
//  //update setting-mode
//  //update property-mode
//  return false;
//}
//
//double cedar::dev::sensors::visual::CameraProperties::getProperty(unsigned int property)
//{
//
//}





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


bool cedar::dev::sensors::visual::CameraProperties::setPropertyRawValue(unsigned int property_id,double value)
{
  //lock videocapture

  //set property

  //unlock videocapture

  return false;
}


double cedar::dev::sensors::visual::CameraProperties::getPropertyRawValue(unsigned int property_id)
{
  return 0.0f;
}


cedar::dev::sensors::visual::CamProperty& cedar::dev::sensors::visual::CameraProperties::getPropertyPtr
(
  CameraProperty::Id propId
)
{
  return mPropertiesList.at(static_cast<unsigned int>(propId));

  //create empty CamProperty
  //cedar::dev::sensors::visual::CamPropertyPtr val(new cedar::dev::sensors::visual::CamProperty());
  //return val;
}


cedar::dev::sensors::visual::CameraPropertyMode::Id cedar::dev::sensors::visual::CameraProperties::getMode
(
  cedar::dev::sensors::visual::CameraProperty::Id propId
)
{

}

void cedar::dev::sensors::visual::CameraProperties::setMode
(
  cedar::dev::sensors::visual::CameraProperty::Id propId,
  cedar::dev::sensors::visual::CameraPropertyMode::Id mode
)
{

}

double cedar::dev::sensors::visual::CameraProperties::getMinValue(CameraProperty::Id propId)
{

}

double cedar::dev::sensors::visual::CameraProperties::getMaxValue(CameraProperty::Id propId)
{

}

bool cedar::dev::sensors::visual::CameraProperties::isSupported(CameraProperty::Id propId)
{

}

bool cedar::dev::sensors::visual::CameraProperties::isReadable(CameraProperty::Id propId)
{

}

bool cedar::dev::sensors::visual::CameraProperties::isOnOffCapable(CameraProperty::Id propId)
{

}

bool cedar::dev::sensors::visual::CameraProperties::isAutoCapable(CameraProperty::Id propId)
{

}

bool cedar::dev::sensors::visual::CameraProperties::isManualCapable(CameraProperty::Id propId)
{

}
